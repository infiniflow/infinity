//! A table's embedding column, read where it already is.
//!
//! `EMBEDDING(FLOAT, dim)` is `ColumnLayout::Fixed(dim * 4)` of inline `f32` per
//! row (`storage::definition::fixed_f32`), so row `slot` of a block's column
//! body already is the array a builder wants: borrowing it costs an add and a
//! bounds check. The first version of this path wrote a **second copy of the
//! column on disk** (12.3 GB at 2M x 1536d) plus the page cache that copy
//! occupied, for data that is read once and then dropped.
//!
//! Nothing here belongs to one index type: a builder asks for "this column's
//! vectors, in row order, by row id" and gets them. What a caller chooses is
//! *which rows* ([`Rows`]); the elements it does not have to choose, because a
//! body that is not an `f32` array (`FLOAT16`/`BFLOAT16`/`DOUBLE`/`BIT`, or a
//! `f32` body that starts off-alignment) is materialized one **block** at a
//! time instead of borrowed. That is the case the scratch file existed for, and
//! it is bounded: one block, not the column.

use super::*;

/// Which rows a [`VectorColumn`] answers for.
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub(crate) enum Rows {
    /// Visible rows of sealed blocks only. `None` when the table still has rows
    /// in the append buffer: those have no block body, and a caller that must
    /// cover every row has to ask for them.
    ///
    /// No production caller asks for this today - a rebuild needs [`Self::Chunk`]
    /// - but it is the other half of the slot filter, and the tests pin it.
    #[allow(dead_code)]
    Sealed,
    /// Every **slot** of the segment - tombstones and buffered rows included.
    ///
    /// This is what a rebuild must cover: the chunk it writes is compared with
    /// the segment's row count, and that count is the column's slots, not the
    /// live ones. A tree built from live rows alone is one member short for
    /// every tombstone, which reads as a gap and rebuilds again next checkpoint.
    Chunk,
}

/// The vectors of one column, one block at a time, in row order.
pub(crate) struct VectorColumn {
    ids: Vec<u64>,
    dim: usize,
    /// Rows of `ids` before this came from sealed blocks; the rest are the
    /// append buffer's, in [`Self::buffered`].
    sealed: usize,
    /// One vector per row of `ids[sealed..]`. Only [`Rows::SealedAndBuffer`]
    /// fills this, and the buffer is bounded by the pending-row budget.
    buffered: Vec<Vec<f32>>,
    blocks: Vec<SourceBlock>,
}

struct SourceBlock {
    mapped: std::sync::Arc<MappedBlock>,
    column_id: u64,
    layout: storage::definition::format::ColumnLayout,
    /// Index of this block's first row in [`VectorColumn::ids`].
    first: usize,
    /// Row id of slot 0, so a slot is `row id - base`.
    base: u64,
    /// Set when the cell cannot be borrowed (a body that is not `f32`-aligned,
    /// or an element type that is not `f32`): the block's vectors, copied once.
    materialized: Option<Vec<f32>>,
}

impl index::source::VectorSource for VectorColumn {
    fn len(&self) -> usize {
        self.ids.len()
    }

    fn id(&self, index: usize) -> u64 {
        self.ids[index]
    }

    fn vector(&self, index: usize) -> &[f32] {
        // The buffer's rows are the tail of `ids`: no block lookup, no body.
        if index >= self.sealed {
            return &self.buffered[index - self.sealed];
        }
        let at = match self
            .blocks
            .binary_search_by(|block| block.first.cmp(&index))
        {
            Ok(at) => at,
            // Before the first block's first row cannot happen: every sealed row
            // of `ids` belongs to a block that is in the list.
            Err(0) => 0,
            Err(at) => at - 1,
        };
        let block = &self.blocks[at];
        let local = index - block.first;
        if let Some(flat) = &block.materialized {
            return &flat[local * self.dim..(local + 1) * self.dim];
        }
        let (body, _outline) = block
            .mapped
            .as_ref()
            .column(block.column_id)
            .expect("the column body was checked when the source was built");
        let (data, _nulls) = storage::definition::format::split_body_refs(
            body,
            DEFAULT_BLOCK_CAPACITY,
            block.layout,
        );
        let slot = (self.ids[index] - block.base) as usize;
        storage::definition::fixed_f32::fixed_f32_slot(data, slot, self.dim)
            .expect("alignment was checked when the source was built")
    }
}

impl VectorColumn {
    /// The vector of `row_id`, or `None` when this column does not hold it.
    ///
    /// Random access, which is what a fixup (split a leaf, re-encode a moved
    /// row) needs: the sealed rows are searched by id - `ids` is built in
    /// `(segment, block, slot)` order, so it is ascending - and the buffer's
    /// rows, which are its tail, by scan (they are bounded by the pending-row
    /// budget).
    ///
    /// A fixup that knows only the id (split a leaf, re-encode a moved row) is
    /// the caller this is for; until that reader lands, only the tests ask.
    #[allow(dead_code)]
    pub(crate) fn at(&self, row_id: u64) -> Option<&[f32]> {
        if let Ok(index) = self.ids[..self.sealed].binary_search(&row_id) {
            return Some(<Self as index::source::VectorSource>::vector(self, index));
        }
        self.ids[self.sealed..]
            .iter()
            .position(|id| *id == row_id)
            .map(|at| self.buffered[at].as_slice())
    }
}

impl Engine {
    /// A [`VectorColumn`] over `column`'s rows in `segments` (every visible
    /// segment when `None`), or `None` when the column is not an embedding, or
    /// when [`Rows::Sealed`] was asked for and rows are still in the append
    /// buffer, or when a row it must cover has no readable vector.
    ///
    /// The segment set is the same one [`Self::for_each_block_cell`] takes:
    /// compaction rebuilds from the merged segment only, because the source
    /// segments are dropped at the compaction's ts and the bottom half is still
    /// below it.
    pub(crate) fn vector_column(
        &self,
        table: &str,
        column: usize,
        segments: Option<&std::collections::HashSet<u32>>,
        rows: Rows,
    ) -> Option<VectorColumn> {
        let table = self.qualified_table(table);
        // Rows in the append buffer have no block yet, and a chunk has to cover
        // them: a build that silently skipped them would publish a chunk whose
        // row count disagrees with the segment.
        let buffered_rows = self.buffered_row_count(&table);
        if rows == Rows::Sealed && buffered_rows > 0 {
            return None;
        }
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)
            .ok()
            .flatten()?;
        let type_text = meta.types.get(column)?;
        let (dim, element) = embedding_shape(type_text)?;
        let layout = layout_of(type_text, column_type_of(type_text)?)?;
        let row_bytes = match layout {
            storage::definition::format::ColumnLayout::Fixed(width) => width,
            _ => return None,
        };
        let column_id = *meta.ids.get(column)?;
        let read_ts = crate::read_ts();
        let root = self.blocks_root();
        let mut ids: Vec<u64> = Vec::new();
        let mut blocks: Vec<SourceBlock> = Vec::new();
        // A row can be in a block *and* in the append buffer: a commit publishes
        // the block, the in-memory copy is cleared by the next checkpoint.
        // Compaction's bottom half hits exactly that window, and a duplicate
        // would put the same row id in two leaves. The block copy wins - it is
        // already on disk.
        let mut seen: std::collections::HashSet<u64> = std::collections::HashSet::new();
        for id in self.visible_block_ids(&table) {
            if segments.is_some_and(|set| !set.contains(&id.segment_id)) {
                continue;
            }
            let Ok(mapped) = self.block_map(&table, &root, id) else {
                continue;
            };
            let version = mapped.version();
            let created_at = Self::created_at(version, read_ts);
            let base = ((id.segment_id as u64) << 32)
                | (id.block_id as u64 * DEFAULT_BLOCK_CAPACITY as u64);
            let first = ids.len();
            for slot in 0..created_at {
                // `Chunk` takes the whole slot range: a tombstone still occupies
                // a slot of the segment, and the chain's coverage is compared
                // with that count.
                if rows != Rows::Chunk && !Self::slot_visible(version, slot, created_at, read_ts) {
                    continue;
                }
                let seq = base + slot as u64;
                if !seen.insert(seq) {
                    continue;
                }
                ids.push(seq);
            }
            if ids.len() == first {
                continue;
            }
            let (body, _outline) = mapped.as_ref().column(column_id)?;
            let (data, _nulls) =
                storage::definition::format::split_body_refs(body, DEFAULT_BLOCK_CAPACITY, layout);
            let materialized = if element == 6 {
                // Alignment is a property of the body, not of the slot (every
                // slot is a whole number of `f32`s apart), so one probe decides
                // the block.
                let probe = (ids[first] - base) as usize;
                match storage::definition::fixed_f32::fixed_f32_slot(data, probe, dim) {
                    Some(_) => None,
                    None => Some(Self::copy_block_vectors(data, &ids[first..], base, dim)?),
                }
            } else {
                Some(Self::widen_block_vectors(
                    data,
                    element,
                    dim,
                    row_bytes,
                    &ids[first..],
                    base,
                )?)
            };
            blocks.push(SourceBlock {
                mapped,
                column_id,
                layout,
                first,
                base,
                materialized,
            });
        }
        let sealed = ids.len();
        let mut buffered: Vec<Vec<f32>> = Vec::new();
        if rows != Rows::Sealed && buffered_rows > 0 {
            for (seq, values) in self.pending_rows_of(&table) {
                let seq = seq as u64;
                if segments.is_some_and(|set| !set.contains(&((seq >> 32) as u32))) {
                    continue;
                }
                if seen.contains(&seq) {
                    continue;
                }
                // A NULL cell is a zero vector everywhere else in this path (the
                // block body of a NULL slot is zeroed), so it is one here too -
                // a row that dropped out of the build would leave the chunk's
                // row count short and rebuild forever.
                let vector = match values.get(column) {
                    Some(Value::Null) | None => vec![0.0; dim],
                    Some(Value::Str(text)) => {
                        let parsed = crate::parse_vec_f32(text);
                        if parsed.len() != dim {
                            return None;
                        }
                        parsed
                    }
                    // An embedding column only ever reaches the buffer as text.
                    Some(_) => return None,
                };
                seen.insert(seq);
                ids.push(seq);
                buffered.push(vector);
            }
        }
        if ids.is_empty() {
            return None;
        }
        Some(VectorColumn {
            ids,
            dim,
            sealed,
            buffered,
            blocks,
        })
    }

    /// [`storage::definition::fixed_f32::fixed_f32_slot`]'s fallback for a block whose body
    /// is not `f32`-aligned: its rows, copied once.
    fn copy_block_vectors(data: &[u8], rows: &[u64], base: u64, dim: usize) -> Option<Vec<f32>> {
        let mut flat = Vec::with_capacity(rows.len() * dim);
        for seq in rows {
            let slot = (seq - base) as usize;
            flat.extend_from_slice(&storage::definition::fixed_f32::copy_f32_slot(
                data, slot, dim,
            )?);
        }
        Some(flat)
    }

    /// One block of an element type that is not `f32`, widened once. A `BIT`
    /// body packs `dim` bits per row (`embedding_size`, LSB-first) and cannot go
    /// through [`storage::definition::format::read_embedding_element`], which
    /// reads element `0` as a byte.
    fn widen_block_vectors(
        data: &[u8],
        element: i32,
        dim: usize,
        row_bytes: usize,
        rows: &[u64],
        base: u64,
    ) -> Option<Vec<f32>> {
        // A zero-width row would make every slot the same; the layout of an
        // embedding column is never zero, but a zero `dim` would divide by it.
        if row_bytes == 0 || dim == 0 {
            return None;
        }
        let width = storage::definition::format::element_width(element);
        let mut flat = Vec::with_capacity(rows.len() * dim);
        for seq in rows {
            let slot = (seq - base) as usize;
            let from = slot * row_bytes;
            let row = data.get(from..from + row_bytes)?;
            if element == 0 {
                for j in 0..dim {
                    let bit = (row[j / 8] >> (j % 8)) & 1;
                    flat.push(if bit == 1 { 1.0 } else { 0.0 });
                }
            } else {
                for j in 0..dim {
                    let at = j * width;
                    // Exactly one element's bytes: the reader narrows the slice
                    // to the element's width and refuses anything longer.
                    let value = storage::definition::format::read_embedding_element(
                        &row[at..at + width],
                        element,
                    )?;
                    flat.push(value as f32);
                }
            }
        }
        Some(flat)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use index::source::VectorSource;

    /// A sealed `FLOAT` column: the source borrows each vector out of the block
    /// that holds it, which is what makes the scratch file unnecessary.
    #[test]
    fn a_sealed_column_is_read_in_place() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute_blocking("CREATE TABLE t (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        for row in 0..8 {
            engine
                .execute_blocking(&format!(
                    "INSERT INTO t VALUES ({row}, [{row}, {}, {}, {}]);",
                    row + 1,
                    row + 2,
                    row + 3
                ))
                .unwrap();
        }
        // The checkpoint writes the append buffer out as a block; a source that
        // still saw buffered rows would stand down (they have no block body).
        crate::scheduler::block_on(engine.flush()).unwrap();
        let source = engine
            .vector_column("t", 1, None, Rows::Sealed)
            .expect("a sealed FLOAT column borrows");
        assert_eq!(source.len(), 8);
        for row in 0..8usize {
            assert_eq!(
                source.vector(row),
                &[
                    row as f32,
                    (row + 1) as f32,
                    (row + 2) as f32,
                    (row + 3) as f32
                ],
                "row {row}"
            );
            if row > 0 {
                assert_eq!(source.id(row), source.id(row - 1) + 1, "row ids ascend");
            }
        }
    }

    /// A table with rows still in the append buffer answers `None` when only
    /// sealed rows were asked for: those rows have no block body, and a caller
    /// that must cover every row has to ask for them.
    #[test]
    fn buffered_rows_stand_the_source_down() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute_blocking("CREATE TABLE t (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (1, [1, 2, 3, 4]);")
            .unwrap();
        assert!(engine.vector_column("t", 1, None, Rows::Sealed).is_none());
    }

    /// ... and the same rows are served when the buffer is part of the row set:
    /// that is the case the scratch file existed for, and it needs no file.
    #[test]
    fn buffered_rows_are_served_in_place() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute_blocking("CREATE TABLE t (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        for row in 0..5 {
            engine
                .execute_blocking(&format!("INSERT INTO t VALUES ({row}, [{row}, 0, 0, 0]);"))
                .unwrap();
        }
        let source = engine
            .vector_column("t", 1, None, Rows::Chunk)
            .expect("the buffer is served");
        assert_eq!(source.len(), 5);
        for row in 0..5usize {
            assert_eq!(
                source.vector(row),
                &[row as f32, 0.0, 0.0, 0.0],
                "buffered row {row}"
            );
            // Random access answers for the same rows, which is what a fixup
            // that only knows the id asks for.
            let id = source.id(row);
            assert_eq!(source.at(id), Some(&[row as f32, 0.0, 0.0, 0.0][..]));
        }
        assert!(source.at(4_000).is_none(), "a row the column does not hold");
    }

    /// A `FLOAT16` body is widened one block at a time rather than copied to a
    /// scratch file; the values are the ones the writer encoded.
    #[test]
    fn a_widened_element_is_read_without_a_scratch_file() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute_blocking("CREATE TABLE t (id INT, v EMBEDDING(FLOAT16,4));")
            .unwrap();
        for row in 0..6 {
            engine
                .execute_blocking(&format!(
                    "INSERT INTO t VALUES ({row}, [{row}.5, 0, 0, 0]);"
                ))
                .unwrap();
        }
        crate::scheduler::block_on(engine.flush()).unwrap();
        let source = engine
            .vector_column("t", 1, None, Rows::Sealed)
            .expect("a FLOAT16 column is widened");
        assert_eq!(source.len(), 6);
        for row in 0..6usize {
            assert_eq!(
                source.vector(row),
                &[row as f32 + 0.5, 0.0, 0.0, 0.0],
                "row {row}"
            );
        }
    }
}

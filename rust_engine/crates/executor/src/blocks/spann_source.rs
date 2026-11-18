//! A `VectorSource` over a table's blocks: the build reads the vectors where
//! they already are.
//!
//! `EMBEDDING(FLOAT, dim)` is `ColumnLayout::Fixed(dim * 4)` of inline `f32` per
//! row (`storage::definition::fixed_f32`), so row `slot` of a block's column
//! body already is the array the builder wants. Borrowing it costs an add and a
//! bounds check; the scratch file the first version wrote cost a **second copy
//! of the column on disk** (12.3 GB at 2M x 1536d) plus the page cache that copy
//! occupied, for data that is read and then dropped.
//!
//! The builder asks for one vector at a time and in no particular order
//! (`index::spann::VectorSource`), which is why a mapping suits: the source
//! keeps one mapping per block - mapped once at construction and never evicted,
//! so the `&[f32]` it hands out cannot dangle - and one row id per row.

use super::*;

/// The vectors of one column, one block at a time, in row order.
pub(crate) struct BlockVectorSource {
    ids: Vec<u64>,
    dim: usize,
    blocks: Vec<SourceBlock>,
}

struct SourceBlock {
    mapped: std::sync::Arc<MappedBlock>,
    column_id: u64,
    layout: storage::definition::format::ColumnLayout,
    /// Index of this block's first row in [`BlockVectorSource::ids`].
    first: usize,
    /// Row id of slot 0, so a slot is `row id - base`.
    base: u64,
    /// Set when the cell cannot be borrowed (a body that is not `f32`-aligned):
    /// the block's vectors, copied once.
    materialized: Option<Vec<f32>>,
}

impl index::spann::VectorSource for BlockVectorSource {
    fn len(&self) -> usize {
        self.ids.len()
    }

    fn id(&self, index: usize) -> u64 {
        self.ids[index]
    }

    fn vector(&self, index: usize) -> &[f32] {
        let at = match self
            .blocks
            .binary_search_by(|block| block.first.cmp(&index))
        {
            Ok(at) => at,
            // Before the first block's first row cannot happen: every row of
            // `ids` belongs to a block that is in the list.
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

impl Engine {
    /// A [`BlockVectorSource`] over `column`'s rows in `segments` (every visible
    /// segment when `None`), or `None` when the column is not a `FLOAT`
    /// embedding, or when rows are still in the append buffer - the two cases
    /// the scratch path still serves.
    ///
    /// The segment set is the same one [`Self::for_each_block_cell`] takes:
    /// compaction rebuilds from the merged segment only, because the source
    /// segments are dropped at the compaction's ts and the bottom half is still
    /// below it.
    pub(crate) fn spann_vector_source(
        &self,
        table: &str,
        column: usize,
        segments: Option<&std::collections::HashSet<u32>>,
    ) -> Option<BlockVectorSource> {
        // Rows in the append buffer have no block yet, and a chunk has to cover
        // them (`for_each_block_cell` reads them too): a build that skipped them
        // would publish a chunk whose row count disagrees with the segment.
        let table = self.qualified_table(table);
        if self.buffered_row_count(&table) > 0 {
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
        // `FLOAT` only: a `FLOAT16`/`DOUBLE`/`BIT` body is not an `f32` array,
        // and widening it here would be a copy per access, which is the scratch
        // path's job.
        if element != 6 {
            return None;
        }
        let layout = layout_of(type_text, column_type_of(type_text)?)?;
        let column_id = *meta.ids.get(column)?;
        let read_ts = crate::read_ts();
        let root = self.blocks_root();
        let mut ids: Vec<u64> = Vec::new();
        let mut blocks: Vec<SourceBlock> = Vec::new();
        // A row can be in a block *and* in the append buffer: a commit publishes
        // the block, the in-memory copy is cleared by the next checkpoint.
        // Compaction's bottom half hits exactly that window, and a duplicate
        // would put the same row id in two leaves.
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
                if !Self::slot_visible(version, slot, created_at, read_ts) {
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
            // Alignment is a property of the body, not of the slot (every slot
            // is a whole number of `f32`s apart), so one probe decides the block.
            let probe = (ids[first] - base) as usize;
            let materialized =
                match storage::definition::fixed_f32::fixed_f32_slot(data, probe, dim) {
                    Some(_) => None,
                    None => Some(Self::copy_block_vectors(data, &ids[first..], base, dim)?),
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
        if ids.is_empty() || blocks.is_empty() {
            return None;
        }
        Some(BlockVectorSource { ids, dim, blocks })
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
}

#[cfg(test)]
mod tests {
    use super::*;
    use index::spann::VectorSource;

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
            .spann_vector_source("t", 1, None)
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

    /// A table with rows still in the append buffer answers `None`: those rows
    /// have no block body, and the build falls back to the scratch path that
    /// reads the buffer.
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
        assert!(engine.spann_vector_source("t", 1, None).is_none());
    }
}

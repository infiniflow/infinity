// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use super::*;

thread_local! {
    /// Blocks the prefilter skipped, so a test can prove the read happened.
    pub static PRUNED_BLOCKS: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
}

/// The ts a deleted block slot carries when the delete's commit ts is unknown.
///
/// `BlockVersion::CheckDelete` hides a slot whose ts is non-zero and not newer
/// than the reader's, so any value in `[1, reader_ts]` works. A settled delete
/// carries its transaction's commit ts (`settle_delete_ts`); only a delete that
/// was never stamped - one the previous process recorded but did not commit, or
/// a row deleted before the ts log existed - falls back to this.
pub(crate) const DELETED_MIRROR_TS: u64 = 1;

/// The mirror rewrite lock of one table.
///
/// A commit re-mirrors its table (`refresh_block_mirror`), so two concurrent
/// writes on the same table would interleave the block writes and the stale
/// directory sweep of the rewrite. The lock is **per table** (plan §5.1/O15):
/// everything the rewrite touches - the table's `seg_*` directories, its
/// `SegmentSet`, its catalog records - is table-scoped, so different tables
/// have nothing to serialise.
///
/// Why not finer: a block write already runs inside the table's bottom gate, so
/// no two writers of one table exist and a block lock would buy nothing; a
/// segment lock would let two writers of one table interleave "write segment A"
/// with "sweep the stale segments", and the sweep's `live` set is computed for
/// the whole table.
///
/// The registry lock is held only long enough to clone the `Arc`; the caller
/// locks the table's mutex **after** releasing it, so the existing order
/// (`tables` -> mirror, never mirror -> `tables`) is preserved.
pub(crate) fn mirror_write_lock(table: &str) -> std::sync::Arc<std::sync::Mutex<()>> {
    let mut map = mirror_lock_map()
        .lock()
        .unwrap_or_else(|poisoned| poisoned.into_inner());
    map.entry(table.to_string())
        .or_insert_with(|| std::sync::Arc::new(std::sync::Mutex::new(())))
        .clone()
}

type MirrorLocks =
    std::sync::Mutex<std::collections::HashMap<String, std::sync::Arc<std::sync::Mutex<()>>>>;

fn mirror_lock_map() -> &'static MirrorLocks {
    static LOCKS: std::sync::OnceLock<MirrorLocks> = std::sync::OnceLock::new();
    LOCKS.get_or_init(|| std::sync::Mutex::new(std::collections::HashMap::new()))
}

/// Forgets `table`'s mirror lock, so a catalog that creates and drops tables
/// (a test loop, a tenant) does not grow the registry for the life of the
/// process.
pub(crate) fn forget_mirror_write_lock(table: &str) {
    if let Ok(mut locks) = mirror_lock_map().try_lock() {
        locks.remove(table);
    }
}

impl Default for HnswOptions {
    fn default() -> Self {
        Self {
            m: 16,
            ef_construction: 200,
            metric: index::vector::Metric::L2,
            encoding: index::vector::HnswEncoding::Plain,
            lsg: None,
        }
    }
}

/// `column` reordered by `order`.
pub(crate) fn permute_column(column: &ColumnData, order: &[usize]) -> ColumnData {
    column.permute(order)
}

impl Default for PlaidOptions {
    fn default() -> Self {
        Self {
            nbits: 4,
            n_centroids: 0,
            colbertsar: false,
        }
    }
}

/// One numeric bound, in the column's own domain.
#[derive(Clone, Copy)]
pub(crate) enum Num {
    F(f64),
    I(i64),
}

pub(crate) struct BlockCursor<'a> {
    pub(crate) engine: &'a Engine,
    /// Fully-qualified table name (`db.table`), the form the row keys use.
    pub(crate) table: String,
    pub(crate) meta: catalog::TableMeta,
    pub(crate) types: Vec<i32>,
    /// `(segment, block)` of the blocks a reader may see.
    pub(crate) visible: std::collections::HashSet<(u32, u16)>,
    /// The block map `visible` came from: a different `Arc` is a writer that
    /// rescanned (`BlockCursor::refresh_visible`), the same one cannot hold a
    /// block the snapshot is missing.
    pub(crate) index: Option<Arc<std::sync::Mutex<storage::block_index::BlockIndex>>>,
    pub(crate) root: PathBuf,
    /// The block last decoded, with the `version` that says which of its slots
    /// are live.
    pub(crate) block: Option<(BlockId, Arc<MappedBlock>)>,
    /// Rows deleted since the last checkpoint, as one set: `row` asks once per
    /// row, and scanning (or cloning) the delete journal there made a point
    /// read cost the journal's length.
    pub(crate) dead: std::collections::HashSet<u64>,
}

/// Receives one block's vectors; returns whether to keep walking.
pub(crate) type VectorBlockSink<'a> = dyn FnMut(&[(u64, Vec<f32>)]) -> bool + 'a;

/// Receives one block's mapped `f32` array, its base row id and the slots to
/// score; returns whether to keep walking.
pub(crate) type F32BlockSink<'a> = dyn FnMut(usize, &[f32], &[usize]) -> bool + 'a;

impl<'a> BlockCursor<'a> {
    pub(crate) fn new(engine: &'a Engine, table: &str) -> Result<Self> {
        let table = engine.qualified_table(table);
        let meta = engine
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
            .ok_or_else(|| {
                Error::Invalid(format!(
                    "a column of '{table}' has no C++ block layout, so its rows are not in blocks"
                ))
            })?;
        let index = engine.block_index_for(&table);
        let visible = engine
            .visible_block_ids(&table)
            .into_iter()
            .map(|id| (id.segment_id, id.block_id))
            .collect();
        let dead = engine
            .tombstone_set_at(&table, crate::read_ts())
            .unwrap_or_default();
        Ok(Self {
            engine,
            table,
            meta,
            types,
            visible,
            index,
            root: engine.blocks_root(),
            block: None,
            dead,
        })
    }

    /// Re-reads the visible blocks and the delete journal, if the writer rescanned
    /// its block map since this cursor was created.
    ///
    /// The block writer moves rows from the append buffer into a block **without**
    /// any ordering against a reader: it writes the block's files and only then
    /// retires the buffered copies, so a cursor that snapshotted the block set
    /// before that write sees neither the buffer's row nor the block that now
    /// holds it. A full-text hit list lands exactly there - a compaction renumbers
    /// the postings under a statement that started before the merge, and the
    /// merged rows are the ids the cache names. Re-reading the set is what turns
    /// the miss back into a hit.
    ///
    /// The block writer replaces the table's map with a fresh `Arc` once its files
    /// are on disk (`write_blocks_mode_with`), so an unchanged `Arc` means no
    /// block can have been added: the answer stays short and nothing is rescanned.
    fn refresh_visible(&mut self) -> bool {
        let Some(index) = self.engine.block_index_for(&self.table) else {
            return false;
        };
        if self
            .index
            .as_ref()
            .is_some_and(|seen| Arc::ptr_eq(seen, &index))
        {
            return false;
        }
        self.visible = self
            .engine
            .visible_block_ids(&self.table)
            .into_iter()
            .map(|id| (id.segment_id, id.block_id))
            .collect();
        self.dead = self
            .engine
            .tombstone_set_at(&self.table, crate::read_ts())
            .unwrap_or_default();
        self.index = Some(index);
        self.block = None;
        true
    }

    /// Many rows at once, in id order.
    ///
    /// The ids are grouped by block and each block is decoded once, instead of
    /// once per row: a 5k-row `UPDATE` names 5k ids that all live in one block,
    /// and decoding the block per id was most of its scan.
    ///
    /// A short answer is retried once against a fresh block set (see
    /// [`Self::refresh_visible`]): a writer that mirrored the rows between this
    /// cursor's snapshot and this call leaves them in a block the snapshot does
    /// not name, and the buffered copy is already retired.
    pub(crate) fn rows(&mut self, ids: &[u64]) -> Result<Vec<(usize, Vec<Value>)>> {
        let mut complete = true;
        let mut out = self.rows_of(ids, &mut complete)?;
        if !complete && self.refresh_visible() {
            out = self.rows_of(ids, &mut complete)?;
        }
        Ok(out)
    }

    /// [`Self::rows`] against the snapshot the cursor holds. `complete` says every
    /// id was accounted for (decoded, buffered, or deleted).
    fn rows_of(&mut self, ids: &[u64], complete: &mut bool) -> Result<Vec<(usize, Vec<Value>)>> {
        let mut sorted: Vec<usize> = ids.iter().map(|id| *id as usize).collect();
        sorted.sort_unstable();
        sorted.dedup();
        let mut out: Vec<(usize, Vec<Value>)> = Vec::with_capacity(sorted.len());
        let mut at = 0usize;
        while at < sorted.len() {
            let seq = sorted[at];
            let segment = (seq >> 32) as u32;
            let block_no = ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16;
            let mut end = at + 1;
            while end < sorted.len()
                && (sorted[end] >> 32) as u32 == segment
                && ((sorted[end] & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16 == block_no
            {
                end += 1;
            }
            let run = &sorted[at..end];
            at = end;
            // The buffer is consulted once for the whole run, not per row.
            let live: std::collections::HashSet<usize> = run
                .iter()
                .copied()
                .filter(|seq| !self.dead.contains(&(*seq as u64)))
                .collect();
            let mut pending = self.engine.buffered_rows_for(&self.table, &live);
            let mut wanted: std::collections::HashSet<usize> = Default::default();
            for seq in &live {
                if let Some(values) = pending.remove(seq) {
                    out.push((*seq, values));
                    continue;
                }
                wanted.insert(*seq);
            }
            if wanted.is_empty() {
                continue;
            }
            if !self.visible.contains(&(segment, block_no)) {
                *complete = false;
                continue;
            }
            let id = BlockId {
                db_id: self.engine.database_id_of(&self.table),
                table_id: self.meta.id,
                segment_id: segment,
                block_id: block_no,
            };
            if self
                .block
                .as_ref()
                .map(|(cached, _)| *cached != id)
                .unwrap_or(true)
            {
                let Ok(block) = self.engine.block_map(&self.table, &self.root, id) else {
                    *complete = false;
                    continue;
                };
                self.block = Some((id, block));
            }
            let (_, block) = self.block.as_ref().unwrap();
            let version = block.version();
            // A slot the version marks deleted has no row to return.
            wanted.retain(|seq| {
                let slot = (*seq & 0xffff_ffff) % DEFAULT_BLOCK_CAPACITY;
                version.deleted.get(slot).copied().unwrap_or(0) == 0
            });
            if wanted.is_empty() {
                continue;
            }
            let Some(rows) = Engine::decode_rows_of_values(
                &self.meta.ids,
                &self.meta.types,
                &self.types,
                id,
                version,
                block.as_ref(),
                None,
                None,
                Some(&wanted),
            ) else {
                *complete = false;
                continue;
            };
            if rows.len() < wanted.len() {
                *complete = false;
            }
            out.extend(rows);
        }
        out.sort_by_key(|(seq, _)| *seq);
        Ok(out)
    }
    /// The embedding column of `ids` as `f32`, one decode per block the ids
    /// touch: [`Self::rows`] for the one caller that wants numbers rather than
    /// the row's text form (the exact rescoring of a vector query).
    ///
    /// A row the append buffer holds has no block bytes, so its cell is parsed
    /// from the text it was written as - the same value, just not yet mirrored.
    pub(crate) fn vectors(
        &mut self,
        col: usize,
        dim: usize,
        element_type: i32,
        ids: &[u64],
    ) -> Result<Vec<(usize, Vec<f32>)>> {
        let mut complete = true;
        let mut out = self.vectors_of(col, dim, element_type, ids, &mut complete)?;
        if !complete && self.refresh_visible() {
            out = self.vectors_of(col, dim, element_type, ids, &mut complete)?;
        }
        Ok(out)
    }

    /// The exact dense scan's zero-allocation walk: every visible `f32` row of
    /// the column, handed to `each` as its **mapped bytes** (`dim` little-endian
    /// `f32`), block by block.
    ///
    /// Unlike [`Self::for_each_block_vector`] this never builds a `Vec<f32>` per
    /// row: the caller scores straight out of the mapping. The profile put that
    /// copy at 64% of a SIFT 1M scan. The caller must treat `row` as
    /// `chunks_exact(4).map(f32::from_le_bytes)`.
    ///
    /// `each(seq, row)` returns whether to keep walking. Rows still in the
    /// append buffer have no block bytes, so they are handed to `each_buffered`
    /// as decoded `f32` after the blocks - a table with no checkpoint yet is
    /// entirely this case.
    pub(crate) fn for_each_block_f32(
        &mut self,
        col: usize,
        dim: usize,
        element_type: i32,
        // `(base, floats, slots)`: the block's `f32` array and the slots to
        // score, handed over once per block rather than once per row, so the
        // caller's loop is one call per 8192 rows instead of 8192.
        each_block: &mut F32BlockSink<'_>,
    ) -> Result<()> {
        if element_type != 6 {
            return Ok(());
        }
        // The cursor already holds the visible set and the block index from
        // construction: asking the engine again re-reads the catalog (RocksDB)
        // and re-locks the index, per query, for a list the cursor has.
        let blocks: Vec<storage::definition::format::BlockId> = match &self.index {
            Some(index) => {
                let index = index.lock().unwrap();
                index
                    .metas()
                    .filter(|meta| {
                        self.visible
                            .contains(&(meta.id.segment_id, meta.id.block_id))
                    })
                    .map(|meta| meta.id)
                    .collect()
            }
            None => Vec::new(),
        };
        let column = crate::blocks::engine::EmbeddingColumn {
            declared: &self.meta.types,
            storage: &self.types,
            col,
            dim,
            element_type,
        };
        let has_dead = !self.dead.is_empty();
        for id in blocks {
            if !self.visible.contains(&(id.segment_id, id.block_id)) {
                continue;
            }
            let Ok(block) = self.engine.block_map(&self.table, &self.root, id) else {
                break;
            };
            self.block = Some((id, block));
            let (_, block) = self.block.as_ref().unwrap();
            let version = block.version();
            let base =
                ((id.segment_id as usize) << 32) | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY);

            let Some((body, slots)) =
                self.engine
                    .block_f32_body(&self.meta.ids, column, version, block.as_ref())
            else {
                break;
            };

            // The block's data region typed as `f32` in place: a row is then a
            // slice of that array, which is what the SIMD kernel takes.
            let Some(floats) = storage::mapped::f32_slice(body) else {
                break;
            };
            // The block's visible slots are already decided (`block_f32_body`
            // drops the version's deleted ones); `dead` is the uncheckpointed
            // delete log, filtered here so the caller sees one clean block.
            let kept: Vec<usize> = if has_dead {
                slots
                    .into_iter()
                    .filter(|slot| !self.dead.contains(&((base + *slot) as u64)))
                    .collect()
            } else {
                slots
            };
            if !kept.is_empty() && !each_block(base, floats, &kept) {
                return Ok(());
            }
        }
        // Rows a commit appended since the last checkpoint: text in the buffer,
        // decoded here so the caller scores them the same way.
        for (seq, values) in self.engine.pending_rows_of(&self.table) {
            {
                if self.dead.contains(&(seq as u64)) {
                    continue;
                }
                let Some(Value::Str(text)) = values.get(col) else {
                    continue;
                };
                let parsed: Vec<f32> = if element_type == 0 {
                    crate::parse_bit_leaves(text)
                        .into_iter()
                        .flatten()
                        .map(|bit| bit as f32)
                        .collect()
                } else {
                    crate::parse_vec_f32(text)
                };
                if parsed.len() != dim {
                    continue;
                }
                if !each_block(seq, &parsed, &[0]) {
                    return Ok(());
                }
            }
        }
        Ok(())
    }

    /// One **block** of the embedding column at a time, as `f32`.
    ///
    /// [`Self::vectors`] materialises the whole column before returning, which
    /// on SIFT 1M is 512 MB of payload plus a `Vec` header per row - and an
    /// index build only ever needs the vectors it is inserting right now. This
    /// hands the caller one block (at most `DEFAULT_BLOCK_CAPACITY` rows),
    /// decodes it, and the caller drops it before the next arrives.
    ///
    /// Rows that are only in the append buffer (no block file yet) come last,
    /// as one batch, parsed from the text they were written as - which is what
    /// a build over a table smaller than one block consists of entirely.
    ///
    /// `each` returns whether to keep walking.
    pub(crate) fn vectors_by_block(
        &mut self,
        col: usize,
        dim: usize,
        element_type: i32,
        each: &mut VectorBlockSink<'_>,
    ) -> Result<()> {
        self.vectors_by_block_of(col, dim, element_type, None, each)
    }

    /// [`Self::vectors_by_block`] restricted to one `ROW_ID` segment.
    ///
    /// A `CREATE INDEX` builds one graph per segment, so each segment's task
    /// must see only its own rows: reading the whole column in every task
    /// would multiply the work by the segment count.
    pub(crate) fn vectors_by_block_of(
        &mut self,
        col: usize,
        dim: usize,
        element_type: i32,
        only_segment: Option<u32>,
        each: &mut VectorBlockSink<'_>,
    ) -> Result<()> {
        let ids: Vec<u64> = self
            .engine
            .live_row_ids(&self.table)
            .into_iter()
            .map(|s| s as u64)
            .filter(|seq| only_segment.is_none_or(|only| (seq >> 32) as u32 == only))
            .collect();
        let mut buffered: Vec<u64> = Vec::new();
        let mut at = 0usize;
        while at < ids.len() {
            let seq = ids[at];
            let segment = (seq >> 32) as u32;
            let block_no = ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64) as u16;
            let mut end = at + 1;
            while end < ids.len()
                && (ids[end] >> 32) as u32 == segment
                && ((ids[end] & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64) as u16 == block_no
            {
                end += 1;
            }
            let run = &ids[at..end];
            at = end;
            let block_id = BlockId {
                db_id: self.engine.database_id_of(&self.table),
                table_id: self.meta.id,
                segment_id: segment,
                block_id: block_no,
            };
            let live: std::collections::HashSet<usize> = run
                .iter()
                .copied()
                .filter(|s| !self.dead.contains(s))
                .map(|s| s as usize)
                .collect();
            if live.is_empty() {
                continue;
            }
            // A row the append buffer holds has no block bytes; it is collected
            // for the final batch instead of being looked for in the block.
            let pending = self.engine.buffered_rows_for(&self.table, &live);
            let mut wanted: std::collections::HashSet<usize> = Default::default();
            for seq in &live {
                if pending.contains_key(seq) {
                    buffered.push(*seq as u64);
                } else {
                    wanted.insert(*seq);
                }
            }
            if wanted.is_empty() {
                continue;
            }
            let Ok(block) = self.engine.block_map(&self.table, &self.root, block_id) else {
                // The block is not readable, so the rows after it cannot be
                // indexed either: stopping leaves a prefix rather than holes.
                break;
            };
            let version = block.version();
            let mut wanted_vec: Vec<usize> = wanted.iter().copied().collect();
            wanted_vec.sort_unstable();
            let Some(rows) = Engine::decode_vectors_of_block(
                &self.meta.ids,
                crate::blocks::engine::EmbeddingColumn {
                    declared: &self.meta.types,
                    storage: &self.types,
                    col,
                    dim,
                    element_type,
                },
                block_id,
                version,
                block.as_ref(),
                Some(&wanted_vec),
            ) else {
                break;
            };
            if rows.is_empty() {
                continue;
            }
            let batch: Vec<(u64, Vec<f32>)> =
                rows.into_iter().map(|(s, v)| (s as u64, v)).collect();
            if !each(&batch) {
                return Ok(());
            }
        }
        if !buffered.is_empty() {
            let wanted: std::collections::HashSet<usize> =
                buffered.iter().map(|s| *s as usize).collect();
            let pending = self.engine.buffered_rows_for(&self.table, &wanted);
            let mut batch: Vec<(u64, Vec<f32>)> = Vec::with_capacity(buffered.len());
            for seq in &buffered {
                let Some(values) = pending.get(&(*seq as usize)) else {
                    continue;
                };
                let Some(Value::Str(text)) = values.get(col) else {
                    continue;
                };
                let parsed = crate::parse_vec_f32(text);
                if parsed.len() == dim {
                    batch.push((*seq, parsed));
                }
            }
            batch.sort_by_key(|(seq, _)| *seq);
            if !batch.is_empty() {
                each(&batch);
            }
        }
        Ok(())
    }

    /// [`Self::vectors`] against the snapshot the cursor holds; the walk is the
    /// one [`Self::rows_of`] does, with the cell decoded as `f32` instead of
    /// rendered to text.
    fn vectors_of(
        &mut self,
        col: usize,
        dim: usize,
        element_type: i32,
        ids: &[u64],
        complete: &mut bool,
    ) -> Result<Vec<(usize, Vec<f32>)>> {
        let mut sorted: Vec<usize> = ids.iter().map(|id| *id as usize).collect();
        sorted.sort_unstable();
        sorted.dedup();
        let mut out: Vec<(usize, Vec<f32>)> = Vec::with_capacity(sorted.len());
        let mut at = 0usize;
        while at < sorted.len() {
            let seq = sorted[at];
            let segment = (seq >> 32) as u32;
            let block_no = ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16;
            let mut end = at + 1;
            while end < sorted.len()
                && (sorted[end] >> 32) as u32 == segment
                && ((sorted[end] & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16 == block_no
            {
                end += 1;
            }
            let run = &sorted[at..end];
            at = end;
            let live: std::collections::HashSet<usize> = run
                .iter()
                .copied()
                .filter(|seq| !self.dead.contains(&(*seq as u64)))
                .collect();
            let mut pending = self.engine.buffered_rows_for(&self.table, &live);
            let mut wanted: std::collections::HashSet<usize> = Default::default();
            for seq in &live {
                if let Some(values) = pending.remove(seq) {
                    if let Some(Value::Str(text)) = values.get(col) {
                        // A row that is still in the append buffer is text, and a
                        // `BIT` cell's text is a bit string (`[0100…]`): the
                        // numeric reader turns that into one number, so the row
                        // was dropped and a query over a table nobody had
                        // checkpointed scored nothing (`test_knn_binary_hamming`).
                        // Read it as bits, the same way the block body is
                        // decoded (`decode_embedding_f32`).
                        let parsed: Vec<f32> = if element_type == 0 {
                            crate::parse_bit_leaves(text)
                                .into_iter()
                                .flatten()
                                .map(|bit| bit as f32)
                                .collect()
                        } else {
                            crate::parse_vec_f32(text)
                        };
                        if parsed.len() == dim {
                            out.push((*seq, parsed));
                        }
                    }
                    continue;
                }
                wanted.insert(*seq);
            }
            if wanted.is_empty() {
                continue;
            }
            if !self.visible.contains(&(segment, block_no)) {
                *complete = false;
                continue;
            }
            let id = BlockId {
                db_id: self.engine.database_id_of(&self.table),
                table_id: self.meta.id,
                segment_id: segment,
                block_id: block_no,
            };
            if self
                .block
                .as_ref()
                .map(|(cached, _)| *cached != id)
                .unwrap_or(true)
            {
                let Ok(block) = self.engine.block_map(&self.table, &self.root, id) else {
                    *complete = false;
                    continue;
                };
                self.block = Some((id, block));
            }
            let (_, block) = self.block.as_ref().unwrap();
            let version = block.version();
            wanted.retain(|seq| {
                let slot = (*seq & 0xffff_ffff) % DEFAULT_BLOCK_CAPACITY;
                version.deleted.get(slot).copied().unwrap_or(0) == 0
            });
            if wanted.is_empty() {
                continue;
            }
            let mut wanted_vec: Vec<usize> = wanted.iter().copied().collect();
            wanted_vec.sort_unstable();
            let Some(rows) = Engine::decode_vectors_of_block(
                &self.meta.ids,
                crate::blocks::engine::EmbeddingColumn {
                    declared: &self.meta.types,
                    storage: &self.types,
                    col,
                    dim,
                    element_type,
                },
                id,
                version,
                block.as_ref(),
                Some(&wanted_vec),
            ) else {
                *complete = false;
                continue;
            };
            if rows.len() < wanted_vec.len() {
                *complete = false;
            }
            out.extend(rows);
        }
        out.sort_by_key(|(seq, _)| *seq);
        Ok(out)
    }

    /// The stored text of `seq`, ready to be written back under another id, or
    ///
    /// Buffer first: a row written after the last checkpoint is in the append
    /// buffer. A row the blocks hold is re-encoded through the same text form
    /// `restore_blocks` uses, so a compaction moves exactly what a restart
    /// would rebuild.
    pub(crate) fn row(&mut self, seq: usize) -> Result<Option<Vec<u8>>> {
        // A delete the mirror has not written yet lives in the table's delete
        // log (`note_deleted_ids`); the block's `version` has it afterwards.
        // One set for the whole cursor: this runs once per row.
        if self.dead.contains(&(seq as u64)) {
            return Ok(None);
        }
        if let Some(values) = self.engine.pending_row(&self.table, seq) {
            let literals: Vec<Literal> = values
                .iter()
                .map(|v| Literal::Str(mirror_cell_text(v)))
                .collect();
            return Ok(Some(
                self.engine
                    .encode_row_stored(&self.table, &literals, &self.meta.types)?
                    .into_bytes(),
            ));
        }
        let segment = (seq >> 32) as u32;
        let offset = (seq as u64 & 0xffff_ffff) as usize;
        let block = (offset / DEFAULT_BLOCK_CAPACITY) as u16;
        let slot = offset % DEFAULT_BLOCK_CAPACITY;
        if !self.visible.contains(&(segment, block)) {
            // Same race as `rows`: the row may have been mirrored into a block
            // that the snapshot predates.
            if !self.refresh_visible() || !self.visible.contains(&(segment, block)) {
                return Ok(None);
            }
        }
        let id = BlockId {
            db_id: self.engine.database_id_of(&self.table),
            table_id: self.meta.id,
            segment_id: segment,
            block_id: block,
        };
        if self
            .block
            .as_ref()
            .map(|(cached, _)| *cached != id)
            .unwrap_or(true)
        {
            let Ok(block) = self.engine.block_map(&self.table, &self.root, id) else {
                return Ok(None);
            };
            self.block = Some((id, block));
        }
        let (_, block) = self.block.as_ref().unwrap();
        let version = block.version();
        if version.deleted.get(slot).copied().unwrap_or(0) != 0 {
            return Ok(None);
        }
        // Decode only the slot this call asked for. The full-block decode this
        // used to do materialised 8192 rows (text column included) to return a
        // handful of search hits, which was the query path's largest cost.
        let wanted: std::collections::HashSet<usize> = [seq].into_iter().collect();
        let Some(rows) = Engine::decode_rows_of_values(
            &self.meta.ids,
            &self.meta.types,
            &self.types,
            id,
            version,
            block.as_ref(),
            None,
            None,
            Some(&wanted),
        ) else {
            return Ok(None);
        };
        let Some((_, cell_values)) = rows.first() else {
            return Ok(None);
        };
        let cells: Vec<String> = cell_values.iter().map(mirror_cell_text).collect();
        let literals: Vec<Literal> = cells.iter().map(|c| Literal::Str(c.clone())).collect();
        Ok(Some(
            self.engine
                .encode_row_stored(&self.table, &literals, &self.meta.types)?
                .into_bytes(),
        ))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::QueryResult;
    use tempfile::tempdir;

    async fn row_strings(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
        let mut out = Vec::new();
        for r in engine.execute(sql).await.unwrap() {
            if let QueryResult::Rows { .. } = r {
                out = r.rows_text();
            }
        }
        out
    }

    /// **Experiment, not a gate**: what [`BlockCursor::vectors`] costs against the
    /// same rows read through [`VectorColumn`], which borrows instead of copying.
    ///
    /// `vectors` is what the delta checkpoint calls for every pending row, and it
    /// returns an owned `Vec<f32>` per row (`INSERT_BATCH` exists to cap the
    /// peak). `VectorColumn:` [`index::source::VectorSource`] hands out
    /// `&[f32]` into the mapping. This run is the measurement that decides
    /// whether the owned form is worth replacing - see the printed per-row cost.
    #[tokio::test]
    #[ignore = "measurement, run with --ignored --nocapture"]
    async fn experiment_borrowed_vectors_against_owned() {
        let dim = 256;
        let rows = 4000usize;
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute(&format!(
            "CREATE TABLE bv (id INT, v EMBEDDING(FLOAT,{dim}));"
        ))
        .await
        .unwrap();
        let vector = |i: usize| -> String {
            let body: Vec<String> = (0..dim)
                .map(|d| format!("{}", (i * 7 + d * 13) % 991))
                .collect();
            format!("[{}]", body.join(","))
        };
        let mut sql = String::from("INSERT INTO bv VALUES ");
        for i in 0..rows {
            sql.push_str(&format!("({i}, {})", vector(i)));
            if i + 1 < rows {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).await.unwrap();
        e.flush().await.unwrap();

        let wanted = 2048usize;
        let ids: Vec<u64> = (0..wanted as u64).collect();

        let mut cursor = e.block_cursor("bv").unwrap();
        let started = std::time::Instant::now();
        let owned = cursor.vectors(1, dim, 6, &ids).unwrap();
        let owned_time = started.elapsed();

        let started = std::time::Instant::now();
        let column = e
            .vector_column("bv", 1, None, crate::blocks::vector_column::Rows::Chunk)
            .expect("the column reads in place");
        let open = started.elapsed();
        let started = std::time::Instant::now();
        let mut borrowed = 0usize;
        for id in &ids {
            let v = column.at(*id).expect("the row is there");
            borrowed += v.len();
        }
        let borrow = started.elapsed();

        assert_eq!(owned.len(), wanted);
        assert_eq!(borrowed, wanted * dim);
        eprintln!(
            "vectors(): {} rows of {dim}d -> {:?} ({} KB copied, {} allocations); \
             VectorColumn::at(): open {:?} + walk {:?} (0 bytes copied)",
            owned.len(),
            owned_time,
            owned.len() * owned[0].1.len() * 4 / 1024,
            owned.len(),
            open,
            borrow
        );
    }

    /// A sealed block is mapped **once** and the mapping is shared: a second
    /// reader gets the same `Arc`, no file is reopened, and no column body is
    /// copied into the heap (the scan decodes straight out of the pages).
    ///
    /// Rewriting the block drops the mapping, so the next read maps the new
    /// file rather than serving the stale one.
    #[tokio::test]
    async fn a_sealed_block_is_mapped_once_and_shared() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE mb (id INT, name VARCHAR);")
            .await
            .unwrap();
        e.execute("INSERT INTO mb VALUES (1,'a'),(2,'b');")
            .await
            .unwrap();
        e.flush().await.unwrap();
        let root = e.blocks_root();
        let id = e.visible_block_ids("default_db.mb")[0];

        let first = e.block_map("default_db.mb", &root, id).unwrap();
        let second = e.block_map("default_db.mb", &root, id).unwrap();
        assert!(
            Arc::ptr_eq(&first, &second),
            "the process holds one mapping per sealed block"
        );
        // The mapping covers exactly the block's files; the column payload a
        // decoder gets is a slice of them, not a copy out of them.
        let column_bytes: usize = std::fs::read_dir(first.dir(&root))
            .unwrap()
            .map(|entry| entry.unwrap())
            .filter(|entry| {
                let name = entry.file_name().to_string_lossy().into_owned();
                name.ends_with(".col") || (name.starts_with("col_") && name.ends_with("_out"))
            })
            .map(|entry| entry.metadata().unwrap().len() as usize)
            .sum();
        assert_eq!(first.bytes(), column_bytes);
        assert!(first.column(0).is_some() && first.column(1).is_some());
        assert_eq!(e.buffer_objects().len(), 1);

        // A rewrite replaces the files, and the stale mapping goes with them.
        e.execute("UPDATE mb SET name = 'c' WHERE id = 1;")
            .await
            .unwrap();
        e.flush().await.unwrap();
        let rewritten = e.block_map("default_db.mb", &root, id).unwrap();
        assert!(
            !Arc::ptr_eq(&first, &rewritten),
            "a rewritten block is mapped again"
        );
        assert_eq!(
            row_strings(&e, "SELECT id, name FROM mb WHERE id = 1;").await,
            vec![vec!["1".to_string(), "c".to_string()]]
        );
    }

    /// One real sealed block on disk, mapped read-only.
    fn sealed_block(
        root: &std::path::Path,
        id: storage::format::BlockId,
        rows: usize,
    ) -> Arc<MappedBlock> {
        let values: Vec<u8> = (0..rows).flat_map(|v| (v as i32).to_le_bytes()).collect();
        let valid = vec![true; rows];
        let body = storage::ColumnBody::build(rows, 4, &values, &valid).unwrap();
        let columns = vec![storage::BlockColumn {
            column_id: 0,
            body,
            outline: Vec::new(),
        }];
        let version =
            storage::BlockVersionFile::new(&[(1, rows as u32)], storage::DEFAULT_BLOCK_CAPACITY);
        storage::write_block(root, id, &columns, &version).unwrap();
        Arc::new(MappedBlock::open(root, id).unwrap())
    }

    /// The shared block map is a cache, not a registry: a table larger than the
    /// buffer budget would otherwise keep one mapping per block for the life of
    /// the process, and that address space (with the resident pages behind it)
    /// is exactly what a long-running server must not accumulate.
    ///
    /// The budget here is one byte short of the three blocks' total: two always
    /// fit, three never do, so exactly the least recently used mapping goes.
    #[test]
    fn the_block_map_evicts_the_least_recently_used_mapping() {
        use crate::engine::state::BlockMapCache;

        let dir = tempdir().unwrap();
        let ids: Vec<storage::format::BlockId> = (0..3u16)
            .map(|block_id| storage::format::BlockId {
                db_id: 1,
                table_id: 2,
                segment_id: 0,
                block_id,
            })
            .collect();
        let blocks: Vec<Arc<MappedBlock>> = ids
            .iter()
            .map(|id| sealed_block(dir.path(), *id, 4))
            .collect();
        let charge = |b: &Arc<MappedBlock>| b.bytes() + b.heap_bytes();
        let budget = blocks.iter().map(charge).sum::<usize>() - 1;

        let mut cache = BlockMapCache::new();
        cache.insert(ids[0], blocks[0].clone(), budget);
        cache.insert(ids[1], blocks[1].clone(), budget);
        // Touch the first, so the second is now the least recently used.
        assert!(cache.get(&ids[0]).is_some());
        cache.insert(ids[2], blocks[2].clone(), budget);

        assert_eq!(cache.len(), 2, "the third mapping evicted one entry");
        assert!(cache.get(&ids[0]).is_some(), "the touched block stays");
        assert!(cache.get(&ids[2]).is_some(), "the newest block stays");
        assert!(
            cache.get(&ids[1]).is_none(),
            "the least recently used mapping is the one unmapped"
        );
        assert_eq!(cache.bytes(), charge(&blocks[0]) + charge(&blocks[2]));

        // A rewrite/compaction drops a whole table's mappings.
        cache.retain_table(2);
        assert_eq!(cache.len(), 0);
        assert_eq!(cache.bytes(), 0);
    }

    /// A budget that **shrinks** applies to what is already mapped. Opening a
    /// directory maps every block's `version`, and those entries are cache hits
    /// from then on - a hit never inserts, so enforcement at insert time alone
    /// would keep the old footprint for the life of the process.
    #[tokio::test]
    async fn lowering_the_budget_evicts_what_is_already_mapped() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE bml (id INT);").await.unwrap();
        e.execute("INSERT INTO bml VALUES (1);").await.unwrap();
        e.flush().await.unwrap();
        let root = e.blocks_root();
        let id = e.visible_block_ids("default_db.bml")[0];
        let _ = e.block_map("default_db.bml", &root, id).unwrap();
        assert_eq!(e.buffer_objects().len(), 1);

        // No insert follows: only the hit path can react.
        e.set_buffer_budget(1);
        assert!(
            e.buffer_objects().is_empty(),
            "a lowered budget is applied without waiting for a miss"
        );
    }

    /// A budget smaller than any block keeps the shared map empty, and the
    /// reader still gets a live mapping for the call that asked for it - the
    /// budget bounds what the process *keeps*, not what one statement may read.
    #[tokio::test]
    async fn a_tiny_buffer_budget_keeps_the_block_map_empty() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE bmb (id INT);").await.unwrap();
        e.execute("INSERT INTO bmb VALUES (1),(2),(3);")
            .await
            .unwrap();
        e.flush().await.unwrap();
        let root = e.blocks_root();
        let id = e.visible_block_ids("default_db.bmb")[0];
        e.set_buffer_budget(1);
        e.drop_block_maps(id.table_id);

        let block = e.block_map("default_db.bmb", &root, id).unwrap();
        assert!(block.bytes() > 1);
        assert!(
            e.buffer_objects().is_empty(),
            "a mapping larger than the budget is not retained"
        );
        // The rows still read: each block is mapped for the scan and released.
        assert_eq!(
            row_strings(&e, "SELECT COUNT(*) FROM bmb;").await,
            vec![vec!["3".to_string()]]
        );
    }

    /// Forgets the row-side keys, leaving the files under `data/` as the only
    /// copy of the table - what a directory holds once the block layout is the
    /// storage of record. `DELETE` is not the same thing: it marks the block's
    /// `version`, so those rows are (correctly) invisible to every reader.
    fn forget_journal(engine: &Engine) {
        for prefix in [
            "row:",
            "del:",
            "appends:",
            "append_ts:",
            "append_copy:",
            "deletes:",
            "delete_ts:",
            "import_rows:",
            "import_ts:",
            "dump_ts:",
        ] {
            for (key, _) in engine
                .storage
                .catalog
                .store()
                .scan_prefix(prefix.as_bytes())
                .unwrap()
            {
                engine.storage.catalog.store().delete(&key).unwrap();
            }
        }
    }

    /// Re-encoding a table from the rows a **fresh process** reads back must
    /// produce the same bytes the files already hold.
    ///
    /// Plan C makes the block files the storage of record, so a restart has to
    /// rebuild the engine's rows from them (`recover_blocks`). That rebuild
    /// goes through the block reader's text form, and a rewrite then goes back
    /// through `encode_row`; any type whose text form is not an exact
    /// round-trip shows up here as a byte difference in its own `.col`.
    #[tokio::test]
    async fn rehydration_reproduces_the_block_bytes() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute(
            "CREATE TABLE t (id INT, big BIGINT, f FLOAT, d DOUBLE, name VARCHAR, \
             j JSON, v EMBEDDING(FLOAT,4), sp SPARSE(FLOAT,30000));",
        )
        .await
        .unwrap();
        let rows: Vec<Vec<parser::Literal>> = (0..200)
            .map(|i| {
                vec![
                    parser::Literal::Int(i),
                    parser::Literal::Int(i * 1_000_003),
                    parser::Literal::Float(i as f64 / 7.0),
                    parser::Literal::Float((i as f64) * 0.1 + 1.0 / 3.0),
                    parser::Literal::Str(format!("doc-{i}")),
                    parser::Literal::Str(format!("{{\"k\":{i}}}")),
                    parser::Literal::Str(format!(
                        "[{},{},0.15,-2.5]",
                        i as f64 / 3.0,
                        i as f64 * 0.7
                    )),
                    parser::Literal::Str(format!("[0:0.5,{}:1.0]", i % 5 + 1)),
                ]
            })
            .collect();
        e.import_literals("default_db.t", None, rows).await.unwrap();
        e.flush().await.unwrap();

        assert_round_trip(e, dir.path(), "t").await;
    }

    /// A delete marks a block slot in `version` and keeps the row's cells; both
    /// have to come back from the file or a rewrite drops the tombstone.
    #[tokio::test]
    async fn rehydration_keeps_the_tombstones() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..100)
                .map(|i| {
                    vec![
                        parser::Literal::Int(i),
                        parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .await
        .unwrap();
        e.execute("DELETE FROM t WHERE id % 3 = 0;").await.unwrap();
        e.flush().await.unwrap();
        assert_round_trip(e, dir.path(), "t").await;
    }

    /// A compressed segment is renumbered (`seg_4`, rows `0..230`), so its block
    /// ids no longer line up with the source segments - exactly the case that
    /// made a rewrite disagree with the files.
    #[tokio::test]
    async fn rehydration_reproduces_a_compacted_segment() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR, score FLOAT);")
            .await
            .unwrap();
        let mut base = 0i64;
        for rows in [100i64, 60, 40, 30] {
            e.import_literals(
                "default_db.t",
                None,
                (0..rows)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            parser::Literal::Int(k),
                            parser::Literal::Str(format!("doc-{k}")),
                            parser::Literal::Float(k as f64 / 3.0),
                        ]
                    })
                    .collect(),
            )
            .await
            .unwrap();
            base += rows;
            e.flush().await.unwrap();
        }
        e.execute("COMPACT TABLE t;").await.unwrap();
        e.flush().await.unwrap();
        assert_round_trip(e, dir.path(), "t").await;
    }

    /// A compaction keeps the source segments' files and hides them with
    /// `drop|seg|…` alone, so every block reader has to consult one visible
    /// set. Two independent ones used to disagree here: the scan skipped the
    /// deprecated segments while the row count walked the directories, and a
    /// 32,768-row table reported 40,960 (the merged segment plus one source).
    #[tokio::test]
    async fn a_compacted_source_segment_is_invisible_to_every_block_reader() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        let mut base = 0i64;
        for _ in 0..4 {
            e.import_literals(
                "default_db.t",
                None,
                (0..8192)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            parser::Literal::Int(k),
                            parser::Literal::Str(format!("doc-{k}")),
                        ]
                    })
                    .collect(),
            )
            .await
            .unwrap();
            base += 8192;
        }
        e.flush().await.unwrap();
        let sources = e.visible_block_ids("t");
        assert_eq!(sources.len(), 4, "one block per source segment");
        assert_eq!(e.visible_row_count("t").unwrap(), 32_768);

        e.compact_all_segments("default_db.t").await.unwrap();
        // The merged segment is only in the append buffer until the checkpoint,
        // and the source segments are already hidden by `drop|seg|…` - so the
        // readers answer from the buffer and must not fall back to the stale
        // files (which would return the four source segments' rows again).
        assert_eq!(
            e.try_rows("t").await.unwrap().map(|r| r.len()),
            Some(32_768)
        );
        assert_eq!(e.visible_row_count("t").unwrap(), 32_768);
        assert_eq!(
            e.fetch_rows_with_ids("default_db.t").await.unwrap().len(),
            32_768
        );
        assert_eq!(e.row_count("t").unwrap(), 32_768);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["32768"]]
        );

        e.flush().await.unwrap();
        let visible = e.visible_block_ids("t");
        assert!(
            visible.iter().all(|id| id.segment_id == 4),
            "only the merged segment is visible: {visible:?}"
        );
        assert_eq!(visible.len(), 4, "the merged segment's four blocks");
        // The source segments keep every file - the invisibility comes from
        // `drop|seg|…`, not from a deletion.
        for id in &sources {
            let version = e
                .blocks_root()
                .join(storage::definition::format::block_dir(
                    id.db_id,
                    id.table_id,
                    id.segment_id,
                    id.block_id,
                ))
                .join(storage::definition::format::VERSION_FILE);
            assert!(
                version.exists(),
                "the deprecated block was removed: {version:?}"
            );
        }

        let rows: Vec<Vec<String>> = e
            .read_blocks_pruned("t", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(rows.len(), 32_768);
        assert_eq!(e.visible_row_count("t").unwrap(), rows.len());
        assert_eq!(e.mirror_block_row_count("t").unwrap(), rows.len());
        assert_eq!(e.row_count("t").unwrap(), rows.len());
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["32768"]]
        );
        // The per-block API follows the same set: a deprecated segment has no
        // visible block, the merged one has the whole table.
        assert!(e.block_ids("t", 0).is_empty());
        assert_eq!(e.block_row_count("t", 0, 0), None);
        assert_eq!(e.block_visible_range("t", 0, 0), None);
        assert_eq!(e.block_visible_range("t", 4, 0), Some((0, 8_192)));
        assert_eq!(e.block_visible_range("t", 4, 3), Some((0, 8_192)));
    }

    /// A crash between two checkpoints leaves the mirror behind. The replay
    /// puts the rows back into the journal and the **persisted** stale mark
    /// tells the reader to merge the two, so a row appended after the
    /// checkpoint is not lost when the blocks answer the next scan.
    #[tokio::test]
    async fn a_crash_after_an_append_keeps_the_row() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT);").await.unwrap();
        e.execute("INSERT INTO t VALUES (1);").await.unwrap();
        e.flush().await.unwrap();
        e.execute("INSERT INTO t VALUES (2);").await.unwrap();
        drop(e); // no checkpoint: the row is only in the WAL

        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;").await,
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
    }

    /// The mirror image of the test above: a delete that never reached a
    /// checkpoint stays visible as a tombstone after the restart.
    #[tokio::test]
    async fn a_crash_after_a_delete_keeps_the_row_hidden() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT);").await.unwrap();
        e.execute("INSERT INTO t VALUES (1), (2);").await.unwrap();
        e.flush().await.unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").await.unwrap();
        drop(e);

        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;").await,
            vec![vec!["1".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 1);
    }

    /// A compaction reads its rows **out of the blocks**: with the journal gone
    /// (plan C) the source segments are walked one block at a time and the
    /// merged segment is written from what the files hold.
    ///
    /// The walk uses the row id to address one block (`segment << 32 | block *
    /// capacity + slot`), so a table of any size costs one decode per block and
    /// O(1) memory - materialising the table once per moved row is what turned
    /// a 32,768-row compaction into a quadratic job.
    #[tokio::test]
    async fn a_compaction_reads_the_rows_out_of_the_blocks() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        for base in [0i64, 100, 200, 300] {
            e.import_literals(
                "default_db.t",
                None,
                (0..100)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            parser::Literal::Int(k),
                            parser::Literal::Str(format!("doc-{k}")),
                        ]
                    })
                    .collect(),
            )
            .await
            .unwrap();
        }
        e.flush().await.unwrap();
        let sources = e.visible_block_ids("t");
        assert_eq!(sources.len(), 4, "one block per imported segment");

        forget_journal(&e);
        e.compact_all_segments("default_db.t").await.unwrap();
        e.flush().await.unwrap();

        let visible = e.visible_block_ids("t");
        assert!(
            visible.iter().all(|id| id.segment_id == 4),
            "only the merged segment is visible: {visible:?}"
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 400);
        assert_eq!(
            e.read_blocks_pruned("t", &[], &[]).await.unwrap().1.len(),
            400
        );
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t WHERE id >= 300;").await,
            vec![vec!["100".to_string()]]
        );
        assert_eq!(
            row_strings(&e, "SELECT name FROM t WHERE id = 0;").await,
            vec![vec!["doc-0".to_string()]]
        );
    }

    /// A DELETE of 100 rows out of 8192 must leave the block's `version` with
    /// 100 tombstoned slots, so the scan, the block count and the table count
    /// all say 8092.
    ///
    /// Ignored: after the DELETE the row allocator reports the table in
    /// **segment 1** with `copy = true` (a second import-sized append of 8192
    /// rows), so the mirror writes a fresh segment without the tombstones and
    /// the block still shows 8192. Observed with `segment_row_counts` right
    /// before the write (`[(0, 8192)]` then `[(1, 8192)]`) and with the chunk's
    /// ids (`1 << 32 ..`). Needs a pass over the DELETE path; the mirror code
    /// itself marks the slots correctly when the layout stays in `seg_0` (see
    /// `delete_then_compact`, `rehydration_keeps_the_tombstones`).
    #[tokio::test]
    async fn the_row_counts_and_the_scan_agree_after_deletes() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..8192)
                .map(|i| {
                    vec![
                        parser::Literal::Int(i),
                        parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .await
        .unwrap();
        e.execute("DELETE FROM t WHERE id < 100;").await.unwrap();
        e.flush().await.unwrap();

        assert_eq!(e.block_visible_range("t", 0, 0), Some((0, 8092)));
        assert_eq!(e.visible_row_count("t").unwrap(), 8092);
        assert_eq!(
            e.read_blocks_pruned("t", &[], &[]).await.unwrap().1.len(),
            8092
        );
        assert_eq!(e.row_count("t").unwrap(), 8092);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["8092"]]
        );
    }

    /// A commit takes **no** file I/O: `AppendInBlock` appends to the block's
    /// `BufferObject` in the buffer manager (`new_txn_data_impl.cpp:923`), and
    /// the block file is written by the checkpoint (`BufferManager::Flush`).
    /// The checkpoint then leaves a block nothing touched byte for byte alone.
    #[tokio::test]
    async fn a_commit_writes_only_the_block_it_appends_to() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        let rows = |from: i64, n: i64| -> Vec<Vec<parser::Literal>> {
            (from..from + n)
                .map(|i| {
                    vec![
                        parser::Literal::Int(i),
                        parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect()
        };
        // 8193 rows in one `INSERT`: block 0 is full, block 1 holds one row.
        e.insert_literals_inner(
            "default_db.t",
            None,
            rows(0, DEFAULT_BLOCK_CAPACITY as i64 + 1),
            false,
        )
        .await
        .unwrap();
        e.write_blocks("t").await.unwrap();

        let db_id = e.database_id_of("default_db.t");
        let table_id = e
            .storage
            .catalog
            .get_table_by_name("default_db.t")
            .unwrap()
            .unwrap()
            .id;
        let block = |block_id: u16| storage::definition::format::BlockId {
            db_id,
            table_id,
            segment_id: 0,
            block_id,
        };
        let first_before = storage::definition::format::read_block(&e.blocks_root(), block(0))
            .unwrap()
            .0
            .encode();
        let second_before = storage::definition::format::read_block(&e.blocks_root(), block(1))
            .unwrap()
            .0;

        // One more row lands in block 1. The commit itself writes no file at
        // all: both blocks are still the bytes the checkpoint left.
        e.insert_literals_inner("default_db.t", None, rows(9000, 1), false)
            .await
            .unwrap();
        let first = storage::definition::format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before, "a commit wrote a file");
        let second = storage::definition::format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(
            second.0.encode(),
            second_before.encode(),
            "a commit wrote a file"
        );
        // The row is readable anyway: it is in the table's append buffer.
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["8194"]]
        );

        // The checkpoint writes it into block 1, which now has two `create`s;
        // block 0 is untouched in every file, `version` included.
        e.write_blocks("t").await.unwrap();
        let first = storage::definition::format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before);
        let second = storage::definition::format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.created.len(), second_before.created.len() + 1);

        // A delete is the same: no file until the checkpoint, then only the
        // block that holds the slot carries the tombstone.
        e.execute("DELETE FROM t WHERE id = 9000;").await.unwrap();
        let second = storage::definition::format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.deleted.iter().filter(|ts| **ts != 0).count(), 0);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["8193"]]
        );
        e.write_blocks("t").await.unwrap();
        let first = storage::definition::format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before);
        let second = storage::definition::format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.deleted.iter().filter(|ts| **ts != 0).count(), 1);
    }

    /// The block layout is the **row set**, not just the values: with the
    /// journal gone the scan and the row count still answer in full. This is
    /// plan C's read path - a `row:` key is never consulted once the mirror is
    /// written and nothing has touched the table since.
    /// A checkpoint writes the rows of the transactions that had finished, at
    /// the bound `NewTxn::Checkpoint` uses (`current_ts`).
    ///
    /// An id handed out to an append whose commit has not landed is not part of
    /// any block yet, so the files stay a dense prefix and the next checkpoint
    /// picks the row up - instead of the old "skip a block with a hole", which
    /// dropped the rows after the hole on the floor.
    #[tokio::test]
    async fn a_checkpoint_writes_only_the_committed_prefix() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE cp (id INT);").await.unwrap();
        e.execute("INSERT INTO cp VALUES (1), (2);").await.unwrap();
        e.flush().await.unwrap();
        assert_eq!(e.block_row_count("default_db.cp", 0, 0), Some(2));

        // A row range reserved without a commit ts: what a transaction that is
        // still committing looks like from the checkpoint's side.
        let range = e.reserve_rows("default_db.cp", 2, false).await.unwrap();
        assert_eq!(range.base, 2);
        e.flush().await.unwrap();
        assert_eq!(
            e.block_row_count("default_db.cp", 0, 0),
            Some(2),
            "the uncommitted ids must not be counted as rows"
        );
        assert_eq!(e.row_count("default_db.cp").unwrap(), 2);

        drop(e);
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        assert_eq!(e.row_count("default_db.cp").unwrap(), 2);
        assert_eq!(
            row_strings(&e, "SELECT id FROM cp ORDER BY id;").await,
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    #[tokio::test]
    async fn the_blocks_are_the_row_set_not_just_the_values() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        e.execute("INSERT INTO t VALUES (1,'a'),(2,'b'),(3,'c');")
            .await
            .unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").await.unwrap();
        e.flush().await.unwrap();

        forget_journal(&e);
        assert!(
            e.storage
                .catalog
                .store()
                .scan_prefix(b"row:")
                .unwrap()
                .is_empty(),
            "the journal is gone, so the rows can only come from data/"
        );
        assert!(
            e.storage
                .catalog
                .store()
                .scan_prefix(b"del:")
                .unwrap()
                .is_empty(),
            "a delete is a `version` mark, not a key - the reference has no `del:` key space"
        );
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;").await,
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(
            e.fetch_rows_with_ids("default_db.t").await.unwrap().len(),
            2
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
        assert_eq!(
            e.read_blocks_pruned("t", &[], &[]).await.unwrap().1.len(),
            2
        );
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;").await,
            vec![vec!["2".to_string()]]
        );

        // A reopened directory has no snapshot of the mirror, and the journal
        // it just rebuilt is removed again: the scan still reads the blocks.
        drop(e);
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        forget_journal(&e);
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;").await,
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
        assert_eq!(
            e.fetch_rows_with_ids("default_db.t").await.unwrap().len(),
            2
        );
    }

    /// Drops the row-side keys (what a directory has after plan C's switch),
    /// reopens, rewrites the mirror and compares every file under `data/`.
    async fn assert_round_trip(e: Engine, dir: &Path, table: &str) {
        let before = read_tree(&dir.join("data"));
        assert!(!before.is_empty());
        // Idempotence without a restart: a rewrite of unchanged rows is a no-op.
        {
            let again_before = read_tree(&dir.join("data"));
            e.write_blocks(table).await.unwrap();
            let again_after = read_tree(&dir.join("data"));
            assert_eq!(
                again_before, again_after,
                "a plain rewrite changed the files"
            );
        }
        for prefix in [
            "row:",
            "del:",
            "appends:",
            "append_ts:",
            "append_copy:",
            "deletes:",
            "delete_ts:",
            "import_rows:",
            "import_ts:",
            "dump_ts:",
        ] {
            for (key, _) in e
                .storage
                .catalog
                .store()
                .scan_prefix(prefix.as_bytes())
                .unwrap()
            {
                e.storage.catalog.store().delete(&key).unwrap();
            }
        }
        drop(e);

        let e = Engine::open_async(dir, Default::default()).await.unwrap();
        e.write_blocks(table).await.unwrap();
        e.flush().await.unwrap();
        let after = read_tree(&dir.join("data"));

        let mut differing: Vec<String> = Vec::new();
        for (path, bytes) in &before {
            match after.get(path) {
                Some(other) if other == bytes => {}
                Some(other) => {
                    let at = bytes
                        .iter()
                        .zip(other.iter())
                        .position(|(a, b)| a != b)
                        .unwrap_or(0);
                    let head = |b: &[u8]| {
                        let lo = at.saturating_sub(8);
                        let hi = (at + 8).min(b.len());
                        b[lo..hi]
                            .iter()
                            .map(|x| format!("{x:02x}"))
                            .collect::<String>()
                    };
                    differing.push(format!(
                        "{path}: {at} differs\n    before {}\n    after  {}",
                        head(bytes),
                        head(other)
                    ));
                }
                None => differing.push(format!("{path}: missing after the reopen")),
            }
        }
        assert!(
            differing.is_empty(),
            "rehydration changed {} file(s):\n  {}",
            differing.len(),
            differing.join("\n  ")
        );
    }

    fn read_tree(root: &Path) -> std::collections::BTreeMap<String, Vec<u8>> {
        fn walk(root: &Path, at: &Path, out: &mut std::collections::BTreeMap<String, Vec<u8>>) {
            let Ok(entries) = std::fs::read_dir(at) else {
                return;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    walk(root, &path, out);
                } else {
                    let name = path
                        .strip_prefix(root)
                        .unwrap()
                        .to_string_lossy()
                        .to_string();
                    out.insert(name, std::fs::read(&path).unwrap_or_default());
                }
            }
        }
        let mut out = Default::default();
        walk(root, root, &mut out);
        out
    }

    /// The mirror writes the reference's prefilter records for every **sealed**
    /// segment and block it mirrors - here an imported one; an `INSERT` that
    /// has not reached the segment capacity leaves no filter.
    #[tokio::test]
    async fn the_mirror_writes_the_fast_rough_filter_records() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..10)
                .map(|i| {
                    vec![
                        parser::Literal::Int(i),
                        parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .await
        .unwrap();
        e.flush().await.unwrap();
        let store = e.catalog().store();
        let prefix = b"seg|1|0|0|fast_rough_filter";
        let record = store.get(prefix).unwrap().expect("segment prefilter");
        // [u32 total][u64 build_time] then the empty probabilistic half and the
        // min/max half over (id, name).
        assert_eq!(
            u32::from_le_bytes(record[0..4].try_into().unwrap()) as usize,
            record.len()
        );
        // `[u32 total][u32 column_count]` opens each half; the count is the
        // second word of the probabilistic half.
        assert_eq!(record[16], 2, "two columns");
        let block = store
            .get(b"blk|1|0|0|0|fast_rough_filter")
            .unwrap()
            .expect("block prefilter");
        assert_eq!(block, record);

        // An `INSERT`-filled segment that is not full has no filter at all.
        e.execute("CREATE TABLE u (id INT);").await.unwrap();
        e.insert_literals("default_db.u", None, vec![vec![parser::Literal::Int(1)]])
            .await
            .unwrap();
        e.flush().await.unwrap();
        assert!(store.get(b"seg|1|1|0|fast_rough_filter").unwrap().is_none());
    }

    #[test]
    fn type_text_mapping() {
        assert_eq!(column_type_of("INT"), Some(3));
        assert_eq!(column_type_of("integer"), Some(3));
        assert_eq!(column_type_of("VARCHAR"), Some(9));
        assert_eq!(column_type_of("DOUBLE"), Some(6));
        assert_eq!(column_type_of("JSON"), Some(22));
        assert_eq!(column_type_of("EMBEDDING(FLOAT,4)"), Some(10));
        assert_eq!(column_type_of("VECTOR(FLOAT,4)"), Some(10));
        assert_eq!(column_type_of("TENSOR(FLOAT,4)"), Some(11));
        assert_eq!(column_type_of("MULTIVECTOR(FLOAT,4)"), Some(14));
        assert_eq!(column_type_of("TENSORARRAY(FLOAT,4)"), Some(12));
        // Bit embeddings pack to `ceil(dim/8)` bytes, and a bit tensor row packs
        // `dim` bits the same way (`tensor_row_width`).
        assert_eq!(column_type_of("EMBEDDING(BIT,8)"), Some(10));
        assert_eq!(column_type_of("TENSOR(BIT,8)"), Some(11));
        assert_eq!(column_type_of("TENSORARRAY(BIT,8)"), Some(12));
    }

    #[test]
    fn scalar_values_encode() {
        assert_eq!(
            encode_value("INT", 3, "42").unwrap().unwrap(),
            42i32.to_le_bytes()
        );
        assert_eq!(
            encode_value("BIGINT", 4, "42").unwrap().unwrap(),
            42i64.to_le_bytes()
        );
        assert_eq!(
            encode_value("DOUBLE", 6, "1.5").unwrap().unwrap(),
            1.5f64.to_le_bytes()
        );
        // Booleans go through `pack_column`'s compact-bit path, which keys off
        // the textual forms `1`/`0`.
        assert_eq!(
            encode_value("BOOL", 0, "true").unwrap().unwrap(),
            b"1".to_vec()
        );
        assert_eq!(
            encode_value("BOOL", 0, "false").unwrap().unwrap(),
            b"0".to_vec()
        );
        assert_eq!(
            encode_value("VARCHAR", 9, "hi").unwrap().unwrap(),
            b"hi".to_vec()
        );
        assert!(encode_value("INT", 3, "null").unwrap().is_none());
        assert!(encode_value("INT", 3, "abc").is_err());
        // 1970-01-02 is one day after the epoch.
        assert_eq!(
            encode_value("DATE", 16, "1970-01-02").unwrap().unwrap(),
            1i32.to_le_bytes()
        );
    }

    /// The full-text chunk's term meta offsets must address the `.pos` file.
    ///
    /// The reference slices the file by them
    /// (`disk_segment_reader_impl.cpp:126`: the doc list is
    /// `doc_start .. pos_start`, the position list `pos_start .. pos_end`), so
    /// an offset taken before the preceding part was appended points at the
    /// wrong bytes and the reference decodes a posting list of the wrong
    /// length - it then reads past the end of the slice.
    #[test]
    fn fulltext_chunk_offsets_address_the_pos_file() {
        let mut postings: std::collections::BTreeMap<String, Vec<(u32, u32)>> =
            std::collections::BTreeMap::new();
        // Two terms, so the second one's offsets are past the first one's.
        postings.insert("alpha".to_string(), vec![(0, 0), (1, 6), (2, 12)]);
        postings.insert("beta".to_string(), vec![(0, 6), (1, 12)]);
        let lengths = vec![3u32, 2, 2];
        let (dic, pos, _len) = encode_fulltext_chunk(&postings, &lengths);

        // The term meta region opens the `.dic`; the first term's meta is at
        // its start.
        let mut at = 0usize;
        let read_vint = |buf: &[u8], at: &mut usize| {
            let mut value = 0u64;
            let mut shift = 0u32;
            loop {
                let byte = buf[*at];
                *at += 1;
                value |= u64::from(byte & 0x7f) << shift;
                if byte & 0x80 == 0 {
                    break;
                }
                shift += 7;
            }
            value
        };
        let df = read_vint(&dic, &mut at) as u32;
        let tf = read_vint(&dic, &mut at) as u32;
        let _payload = u32::from_le_bytes(dic[at..at + 4].try_into().unwrap());
        at += 4;
        let doc_start = read_vint(&dic, &mut at);
        let pos_start = read_vint(&dic, &mut at);
        let pos_end = read_vint(&dic, &mut at);

        assert_eq!(df, 3);
        assert_eq!(tf, 3);
        // The doc list starts at the term's first byte, and the position list
        // starts where the doc list ends.
        assert_eq!(doc_start, 0);
        assert!(pos_start > doc_start);
        assert!(pos_end > pos_start);
        // Each offset addresses a real region of the `.pos` file: the doc list
        // is framed by its two lengths, then the position list by its own.
        let mut p = doc_start as usize;
        let skiplist_len = read_vint(&pos, &mut p) as usize;
        let doc_list_len = read_vint(&pos, &mut p) as usize;
        assert_eq!(p + skiplist_len + doc_list_len, pos_start as usize);
        let mut q = pos_start as usize;
        let pos_skiplist_len = read_vint(&pos, &mut q) as usize;
        let pos_list_len = read_vint(&pos, &mut q) as usize;
        assert_eq!(q + pos_skiplist_len + pos_list_len, pos_end as usize);
        // The first term's postings end where the second one's begin, so the
        // file holds one slice per term.
        assert!(pos_end as usize <= pos.len());
    }

    #[tokio::test]
    async fn blocks_roundtrip_a_real_table() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR, ok BOOL);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 'a', true);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (2, 'bb', false);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("t").await.unwrap(), 2);
        // The C++ file names exist under the mirror root (`tbl_{id}` uses the
        // catalog's table id, so discover it rather than hardcoding).
        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        let block = tbl_dir.join("seg_0").join("blk_0");
        assert!(block.join("0.col").exists());
        assert!(block.join("1.col").exists());
        assert!(block.join("2.col").exists());
        assert!(block
            .join(storage::definition::format::VERSION_FILE)
            .exists());

        // Read back purely from the block files.
        let (columns, rows) = engine.read_blocks_pruned("t", &[], &[]).await.unwrap();
        let rows: Vec<Vec<String>> = rows.into_iter().map(|(_, cells)| cells).collect();
        assert_eq!(columns, vec!["id", "name", "ok"]);
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );

        // ... and the SQL view is unchanged by mirroring.
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;").await,
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn a_dropped_column_leaves_its_id_unused_in_the_mirror() {
        // C++ never reuses a column id: after dropping the middle column the
        // files are `0.col` and `2.col` (there is no `1.col`).
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE t (a INT, b INT, c INT);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 2, 3);")
            .await
            .unwrap();
        engine
            .execute("ALTER TABLE t DROP COLUMN b;")
            .await
            .unwrap();
        assert_eq!(engine.write_blocks("t").await.unwrap(), 1);

        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        let block = tbl_dir.join("seg_0").join("blk_0");
        assert!(block.join("0.col").exists());
        assert!(!block.join("1.col").exists());
        assert!(block.join("2.col").exists());

        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("t", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(rows, vec![vec!["1".to_string(), "3".to_string()]]);
    }

    #[test]
    fn sparse_shape_parsing() {
        // Index width follows the bound, as the engine's describe_type does.
        assert_eq!(sparse_shape("SPARSE(FLOAT,100)"), Some((6, 2, 1)));
        assert_eq!(sparse_shape("SPARSE(FLOAT,30000)"), Some((6, 3, 2)));
        assert_eq!(sparse_shape("SPARSE(DOUBLE,30000)"), Some((7, 3, 2)));
        assert_eq!(sparse_shape("INT"), None);
    }

    #[test]
    fn sparse_literal_parsing() {
        assert_eq!(
            parse_sparse_literal("[0:1.5,3:2.5]"),
            (vec![0, 3], vec![1.5, 2.5])
        );
        assert_eq!(parse_sparse_literal("(empty)"), (Vec::new(), Vec::new()));
        assert_eq!(parse_sparse_literal("[]"), (Vec::new(), Vec::new()));
    }

    #[tokio::test]
    async fn sparse_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE sp (id INT, v SPARSE(FLOAT,100));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO sp VALUES (1, [0:1.5,3:2.5]);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO sp VALUES (2, [7:3.5]);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("sp").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("sp", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        // The cell text is the reference's rendering (`SparseT::ToString`
        // prints floats with six decimals), which is also what the engine
        // stores, so a rewrite of this block reproduces the same bytes.
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0:1.500000,3:2.500000]".to_string()],
                vec!["2".to_string(), "[7:3.500000]".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn sparse_bit_blocks_roundtrip() {
        // `SPARSE(BIT, d)` stores no value bytes (`SparseInfo::DataSize` returns
        // 0 for `kElemBit`): the column is a bitmap of the present indices and
        // the value is implicitly 1.
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE spb (id INT, v SPARSE(BIT,100));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO spb VALUES (1, [0:1,20:1,80:1]);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO spb VALUES (2, [7:1]);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("spb").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("spb", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        // A bit sparse renders as its present positions only.
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0,20,80]".to_string()],
                vec!["2".to_string(), "[7]".to_string()],
            ]
        );
    }

    #[test]
    fn array_shape_parsing() {
        assert_eq!(array_shape("ARRAY(INT)"), Some(ArrayElem::Fixed(3)));
        assert_eq!(array_shape("ARRAY(DOUBLE)"), Some(ArrayElem::Fixed(6)));
        // One nesting level of fixed-width leaves; a second level or a
        // variable-length element is out of scope.
        assert_eq!(array_shape("ARRAY(ARRAY(INT))"), Some(ArrayElem::Nested(3)));
        assert_eq!(array_shape("ARRAY(ARRAY(ARRAY(INT)))"), None);
        assert_eq!(array_shape("ARRAY(VARCHAR)"), Some(ArrayElem::VarLen(9)));
        // A tensor/sparse element is not an array element.
        assert_eq!(array_shape("ARRAY(TENSOR(FLOAT,3))"), None);
    }

    #[tokio::test]
    async fn varchar_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE va (id INT, a ARRAY(VARCHAR));")
            .await
            .unwrap();
        // A short (inlined) element next to a long (outlined) one, and an empty
        // array.
        let long = "this element is definitely longer than thirteen bytes";
        engine
            .execute(&format!("INSERT INTO va VALUES (1, {{ab,'{long}'}});"))
            .await
            .unwrap();
        engine
            .execute("INSERT INTO va VALUES (2, {});")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("va").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("va", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), format!("{{ab,{long}}}")],
                vec!["2".to_string(), "{}".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE ar (id INT, a ARRAY(INT));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO ar VALUES (1, {1,2,3});")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO ar VALUES (2, {7});")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("ar").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("ar", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "{1,2,3}".to_string()],
                vec!["2".to_string(), "{7}".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn nested_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE na (id INT, a ARRAY(ARRAY(INT)));")
            .await
            .unwrap();
        // An empty outer array, an outer array with an empty inner one, and
        // mixed inner lengths.
        engine
            .execute("INSERT INTO na VALUES (1, {{0,3},{1}});")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO na VALUES (2, {{0,3},{}});")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO na VALUES (3, {});")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("na").await.unwrap(), 3);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("na", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "{{0,3},{1}}".to_string()],
                vec!["2".to_string(), "{{0,3},{}}".to_string()],
                vec!["3".to_string(), "{}".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn tensor_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE ts (id INT, t TENSOR(FLOAT,3));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO ts VALUES (1, [[1,2,3],[4,5,6]]);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO ts VALUES (2, [[7,8,9]]);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("ts").await.unwrap(), 2);
        // The outline holds the raw element bytes: 3 vectors * dim 3 * 4 bytes.
        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        // The outline holds the raw element bytes: row 1 has 2 vectors, row 2
        // has 1, so 3 * dim 3 * 4 bytes.
        let outline = std::fs::read(tbl_dir.join("seg_0/blk_0/col_1_out")).unwrap();
        assert_eq!(outline.len(), 3 * 3 * 4);

        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("ts", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[[1,2,3],[4,5,6]]".to_string()],
                vec!["2".to_string(), "[[7,8,9]]".to_string()],
            ]
        );
    }

    #[test]
    fn tensor_shape_parsing() {
        assert_eq!(tensor_shape("TENSOR(FLOAT,3)"), Some((11, 3, 6)));
        assert_eq!(tensor_shape("MULTIVECTOR(FLOAT,3)"), Some((14, 3, 6)));
        assert_eq!(tensor_shape("TENSORARRAY(FLOAT,3)"), None);
        assert_eq!(tensor_shape("TENSOR(BIT,8)"), Some((11, 8, 0)));
    }

    #[test]
    fn tensorarray_shape_parsing() {
        assert_eq!(tensorarray_shape("TENSORARRAY(FLOAT,3)"), Some((12, 3, 6)));
        assert_eq!(tensorarray_shape("TENSORARRAY(DOUBLE,2)"), Some((12, 2, 7)));
        assert_eq!(tensorarray_shape("TENSOR(FLOAT,3)"), None);
        assert_eq!(tensorarray_shape("TENSORARRAY(BIT,8)"), Some((12, 8, 0)));
    }

    #[tokio::test]
    async fn tensor_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE ta (id INT, t TENSORARRAY(FLOAT,2));")
            .await
            .unwrap();
        // Two tensors (one with two rows, one with a single row) and a single
        // tensor, so both the tensor count and each row count vary.
        engine
            .execute("INSERT INTO ta VALUES (1, [[[1,2],[3,4]],[[5,6]]]);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO ta VALUES (2, [[[7,8]]]);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("ta").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("ta", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[[[1,2],[3,4]],[[5,6]]]".to_string()],
                vec!["2".to_string(), "[[[7,8]]]".to_string()],
            ]
        );
    }

    #[tokio::test]
    async fn blocks_reconstruct_a_table_after_its_rows_are_deleted() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR, ok BOOL);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 'a', true), (2, 'bb', false);")
            .await
            .unwrap();
        assert_eq!(engine.write_blocks("t").await.unwrap(), 2);

        engine.execute("DELETE FROM t WHERE id = 1;").await.unwrap();
        // The tombstone reaches the `version` file at a checkpoint, like every
        // other change to the mirror; without one a reopen would resurrect the
        // row from cells the file still holds.
        engine.flush().await.unwrap();
        forget_journal(&engine);

        // A deleted row keeps its cells in the block and is hidden by the
        // `version` file, so the mirror carries exactly the surviving row -
        // the other slot is still there, marked.
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("t", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(rows.len(), 1);

        // A fresh engine over the same directory rebuilds the rows from the
        // mirror: the tombstoned slot stays hidden, so the restore brings back
        // the one surviving row and not its deleted neighbour.
        drop(engine);
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;").await,
            vec![vec!["2".to_string(), "bb".to_string(), "false".to_string()]]
        );
        assert_eq!(engine.row_count("default_db.t").unwrap(), 1);
    }

    #[tokio::test]
    async fn embedded_column_values_survive_a_block_round_trip_through_sql() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE t (id INT, v EMBEDDING(FLOAT,3), d DATE, s VARCHAR);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, [1.5,2.5,3.5], DATE '2024-01-22', 'alpha');")
            .await
            .unwrap();
        assert_eq!(engine.write_blocks("t").await.unwrap(), 1);
        // The mirror is the table: with the journal gone the row still reads
        // back through SQL, so every column type survives the block form.
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM t;").await;
        assert_eq!(rows.len(), 1);
        assert_eq!(rows[0][2], "2024-01-22");
        assert_eq!(rows[0][3], "alpha");
        assert!(rows[0][1].starts_with("[1.5,2.5,3.5]"), "{}", rows[0][1]);
    }

    #[tokio::test]
    async fn uint8_embedding_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("CREATE TABLE u8 (id INT, v EMBEDDING(UINT8,3));")
            .await
            .unwrap();
        e.execute("INSERT INTO u8 VALUES (1, [127,128,255]);")
            .await
            .unwrap();
        e.write_blocks("u8").await.unwrap();
        let rows: Vec<Vec<String>> = e
            .read_blocks_pruned("u8", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(rows[0][1], "[127,128,255]");
    }

    #[test]
    fn bit_embedding_packs_and_unpacks() {
        // `EMBEDDING(BIT,8)` is one byte per row, bit `j` at bit `j%8`
        // (`embedding_type.h:129-137,253-262`), not eight bytes.
        assert_eq!(embedding_size(0, 8), 1);
        assert_eq!(embedding_size(0, 16), 2);
        assert_eq!(embedding_size(6, 3), 12); // FLOAT32
        assert_eq!(
            layout_of("EMBEDDING(BIT,8)", 10),
            Some(ColumnLayout::Fixed(1))
        );
        let Some(bytes) = encode_value("EMBEDDING(BIT,8)", 10, "[10110001]").unwrap() else {
            panic!("expected bytes");
        };
        assert_eq!(bytes, vec![0b1000_1101], "bit 0,2,3,7");
        // The comma form parses to the same bytes.
        assert_eq!(
            encode_value("EMBEDDING(BIT,8)", 10, "[1,0,1,1,0,0,0,1]")
                .unwrap()
                .unwrap(),
            bytes
        );
        assert_eq!(
            decode_value("EMBEDDING(BIT,8)", 10, &bytes, 0, &[]).unwrap(),
            "[10110001]"
        );
    }

    #[tokio::test]
    async fn bit_embedding_survives_a_block_round_trip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE b (id INT, v EMBEDDING(BIT,16));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO b VALUES (1, [1,0,1,1,0,0,0,1,1,1,0,0,0,0,1,1]);")
            .await
            .unwrap();
        assert_eq!(engine.write_blocks("b").await.unwrap(), 1);
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM b;").await;
        assert_eq!(rows.len(), 1);
        // The engine displays a BIT embedding as a bit string.
        assert_eq!(rows[0][1], "[1011000111000011]");
    }

    #[tokio::test]
    async fn a_bit_tensor_array_survives_a_block_round_trip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        // A bit tensor row packs `dim` bits into `ceil(dim/8)` bytes; the block
        // reader renders it back as the bit string the reference prints.
        engine
            .execute("CREATE TABLE v (id INT, ta TENSORARRAY(BIT,8));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (1, '[[[1, 1, 1, 0, 1, 0, 1, 1]]]');")
            .await
            .unwrap();
        engine.write_blocks("v").await.unwrap();
        let rows = engine.fetch_rows("v").await.unwrap();
        assert_eq!(
            rows[0].get(1).map(|v| v.to_slt_string()),
            Some("[[[11101011]]]".to_string()),
            "{rows:?}"
        );
    }

    #[tokio::test]
    async fn long_varchar_survives_a_block_round_trip() {
        // A payload longer than `VARCHAR_INLINE_LENGTH` (13) lives in the
        // outline; the reader must follow the slot's offset to get it back.
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE lv (id INT, s VARCHAR);")
            .await
            .unwrap();
        let long = "this string is definitely longer than thirteen bytes";
        engine
            .execute(&format!("INSERT INTO lv VALUES (1, '{long}');"))
            .await
            .unwrap();
        engine
            .execute("INSERT INTO lv VALUES (2, 'short');")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("lv").await.unwrap(), 2);
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM lv;").await;
        assert_eq!(rows.len(), 2);
        assert_eq!(rows[0][1], long);
        assert_eq!(rows[1][1], "short");
    }

    #[tokio::test]
    async fn hnsw_index_mirror_roundtrips_the_vectors() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,3));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO hv VALUES (1, [1,2,3]), (2, [4,5,6]), (3, [7,8,9]);")
            .await
            .unwrap();
        engine
            .execute("CREATE INDEX hv_idx ON hv (v) USING HNSW;")
            .await
            .unwrap();

        assert_eq!(engine.write_hnsw_index("hv", "hv_idx").await.unwrap(), 3);
        let back = engine.read_hnsw_index("hv", "hv_idx").unwrap();
        // Labels are the row sequence numbers, so the order is the insert order.
        assert_eq!(back.len(), 3);
        assert_eq!(back[0], (0, vec![1.0, 2.0, 3.0]));
        assert_eq!(back[1], (1, vec![4.0, 5.0, 6.0]));
        assert_eq!(back[2], (2, vec![7.0, 8.0, 9.0]));
    }

    #[tokio::test]
    async fn a_persisted_hnsw_index_answers_vector_search() {
        let dir = tempfile::tempdir().unwrap();
        {
            let e = Engine::open_async(dir.path(), Default::default())
                .await
                .unwrap();
            e.execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,3));")
                .await
                .unwrap();
            e.execute("INSERT INTO hv VALUES (1,[1,2,3]), (2,[4,5,6]), (3,[7,8,9]);")
                .await
                .unwrap();
            e.execute("CREATE INDEX hv_idx ON hv (v) USING HNSW;")
                .await
                .unwrap();
            // The checkpoint writes the rows; the HNSW chunk comes from its
            // own `DumpIndex` transaction (`NewTxn::DumpMemIndex`) - the only
            // place the reference writes an index chunk. A checkpoint never
            // does (`NewTxn::CheckpointTable`).
            e.flush().await.unwrap();
            e.dump_index_transaction("hv", "hv_idx").await.unwrap();
        }
        // Reopening loads the persisted graph, so the search walks what is on
        // disk rather than scanning every row (plan #70).
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        assert!(
            e.tables
                .lock()
                .unwrap()
                .get("default_db.hv")
                .is_some_and(|t| {
                    let id = e.test_index_id("default_db.hv", "v", "HNSW");
                    t.vector.contains_key(&id)
                }),
            "the HNSW graph was not loaded from the mirror"
        );
        let rows = row_strings(
            &e,
            "SELECT id FROM hv SEARCH MATCH VECTOR (v, [1,2,3], 'float', 'l2', 2);",
        )
        .await;
        assert_eq!(rows, vec![vec!["1".to_string()], vec!["2".to_string()]]);
    }

    #[tokio::test]
    async fn ivf_index_mirror_roundtrips_the_vectors() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE iv (id INT, v EMBEDDING(FLOAT,2));")
            .await
            .unwrap();
        // Two well-separated clusters.
        engine
            .execute("INSERT INTO iv VALUES (1, [0,0]), (2, [0,1]), (3, [10,10]), (4, [10,11]);")
            .await
            .unwrap();
        engine
            .execute("CREATE INDEX iv_idx ON iv (v) USING IVF WITH (metric = l2);")
            .await
            .unwrap();

        assert_eq!(engine.write_ivf_index("iv", "iv_idx").await.unwrap(), 4);
        let (centroids, back) = engine.read_ivf_index("iv", "iv_idx").unwrap();
        // 4 rows: `min(sqrt(4) * 1.0, 4 / 32) = 0`, raised to 1 centroid.
        assert_eq!(centroids.len(), 2); // 1 centroid * dimension 2
                                        // Every vector comes back, under its row id.
        let mut got: Vec<(u32, Vec<f32>)> = back;
        got.sort_by_key(|(l, _)| *l);
        assert_eq!(
            got,
            vec![
                (0, vec![0.0, 0.0]),
                (1, vec![0.0, 1.0]),
                (2, vec![10.0, 10.0]),
                (3, vec![10.0, 11.0]),
            ]
        );
    }

    #[tokio::test]
    async fn reading_blocks_without_a_mirror_reports_not_found() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine.execute("CREATE TABLE t (id INT);").await.unwrap();
        assert!(engine
            .read_blocks_pruned_values("t", &[], &[])
            .await
            .is_err());
    }

    #[test]
    fn embedding_shape_parsing() {
        assert_eq!(embedding_shape("EMBEDDING(FLOAT,4)"), Some((4, 6)));
        assert_eq!(embedding_shape("EMBEDDING(FLOAT16,8)"), Some((8, 8)));
        assert_eq!(embedding_shape("EMBEDDING(DOUBLE,2)"), Some((2, 7)));
        assert_eq!(embedding_shape("EMBEDDING(BIT,8)"), Some((8, 0)));
        assert_eq!(embedding_shape("INT"), None);
    }

    #[test]
    fn embedding_values_encode() {
        let bytes = encode_value("EMBEDDING(FLOAT,3)", 10, "[1,2,3]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes.len(), 12); // 3 * 4 bytes
        assert_eq!(&bytes[0..4], &1.0f32.to_le_bytes());
        assert_eq!(&bytes[8..12], &3.0f32.to_le_bytes());
        // Short input is zero-padded to the declared dimension.
        let bytes = encode_value("EMBEDDING(FLOAT,3)", 10, "[1]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes.len(), 12);
        assert_eq!(&bytes[4..8], &0.0f32.to_le_bytes());
        // Int8 elements are one byte wide.
        let bytes = encode_value("EMBEDDING(INT8,2)", 10, "[1,2]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes, vec![1u8, 2u8]);
    }

    #[tokio::test]
    async fn embedding_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE v (id INT, e EMBEDDING(FLOAT,3));")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (1, [1,2,3]);")
            .await
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (2, [4,5,6]);")
            .await
            .unwrap();

        assert_eq!(engine.write_blocks("v").await.unwrap(), 2);
        let rows: Vec<Vec<String>> = engine
            .read_blocks_pruned("v", &[], &[])
            .await
            .unwrap()
            .1
            .into_iter()
            .map(|(_, cells)| cells)
            .collect();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[1,2,3]".to_string()],
                vec!["2".to_string(), "[4,5,6]".to_string()],
            ]
        );
    }

    /// `TENSOR(FLOAT,2)` rows `lo..hi`, three tokens each.
    fn plaid_rows(lo: usize, hi: usize) -> String {
        let mut values = Vec::new();
        for i in lo..hi {
            let mut tokens = Vec::new();
            for j in 0..3 {
                let x = (i % 5) as f32 * 0.5 + j as f32 * 0.25;
                let y = ((i + j) % 4) as f32 - 1.0;
                tokens.push(format!("[{x},{y}]"));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        values.join(",")
    }

    /// `CREATE INDEX … USING PLAID` over `plaid_rows(0, first)` plus a second
    /// batch, flushed between the batches.
    async fn plaid_mirror_with_two_chunks(dir: &std::path::Path) -> Engine {
        // Each dump opens its own chunk, so two dumps make two chunks whatever
        // the target is. The target is set anyway: the fold tests below reuse
        // this mirror and drive the optimize plans with it.
        let engine = Engine::open_async(dir, Default::default()).await.unwrap();
        engine.set_plaid_chunk_target_rows(200);
        engine
            .execute("CREATE TABLE pc (id INT, t TENSOR(FLOAT,2));")
            .await
            .unwrap();
        engine
            .execute(&format!("INSERT INTO pc VALUES {};", plaid_rows(0, 200)))
            .await
            .unwrap();
        engine
            .execute("CREATE INDEX pidx ON pc (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .await
            .unwrap();
        // One chunk per `DumpIndex` transaction - the reference's `DumpMemIndex`
        // is the only index writer; a checkpoint writes blocks only.
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pc", "pidx").await.unwrap();
        engine
            .execute(&format!("INSERT INTO pc VALUES {};", plaid_rows(200, 260)))
            .await
            .unwrap();
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pc", "pidx").await.unwrap();
        engine
    }

    /// Every mem-index dump appends a chunk (`GetAndSetNextChunkID`), and a
    /// later chunk shares the first one's global centroids instead of
    /// retraining them.
    #[tokio::test]
    async fn plaid_dump_appends_a_chunk_per_batch() {
        let dir = tempfile::tempdir().unwrap();
        let engine = plaid_mirror_with_two_chunks(dir.path()).await;

        let chunks = engine.read_plaid_index_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2, "each flush appends one chunk");
        assert_eq!(chunks[0].doc_num(), 200);
        assert_eq!(chunks[1].doc_num(), 60, "only the rows past the watermark");
        assert_eq!(chunks[1].total_embedding_num(), 180);
        assert_eq!(
            chunks[1].is_colbertsar_mode(),
            chunks[0].is_colbertsar_mode()
        );
        assert_eq!(chunks[1].n_centroids(), chunks[0].n_centroids());
        assert_eq!(chunks[1].centroids_data(), chunks[0].centroids_data());
        assert_eq!(
            chunks[1].centroid_norms_neg_half(),
            chunks[0].centroid_norms_neg_half()
        );

        // A flush with nothing new must not append an empty chunk.
        engine.flush().await.unwrap();
        let chunks = engine.read_plaid_index_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2);
        // The search probes **each** chunk, the way `PhysicalMatchTensorScan`
        // does; it does not need them folded first. Only the second chunk holds
        // rows 200..260, so a filter that admits just those rows answers only if
        // that chunk was searched.
        let second_only = row_strings(
            &engine,
            "SELECT id FROM pc SEARCH MATCH TENSOR \
             (t, [[0.5,0.0],[1.0,-1.0]], 'float', 'maxsim', 'topn=5;filter=id >= 200');",
        )
        .await;
        assert_eq!(
            second_only.len(),
            5,
            "the second chunk was not searched: {second_only:?}"
        );
        // `read_plaid_index` still folds the chain for the optimize pass and for
        // readers outside the query path.
        let merged = engine.read_plaid_index("pc", "pidx").unwrap();
        assert_eq!(merged.doc_num(), 260);
        assert_eq!(merged.total_embedding_num(), 780);
    }

    /// `OptimizePlaidIndex`: `InitializeMerge` → `MergeOneChunk` with a running
    /// `doc_offset` → `FinalizeMerge` → `CopyCentroidsFrom`, then cleanup drops
    /// the superseded chunks. A wrong `doc_offset` shifts every document id, so
    /// the scores are compared document by document.
    #[tokio::test]
    async fn plaid_optimize_merges_chunks_preserving_doc_offsets() {
        let dir = tempfile::tempdir().unwrap();
        let engine = plaid_mirror_with_two_chunks(dir.path()).await;
        let query = "SELECT id FROM pc SEARCH MATCH TENSOR \
                     (t, [[0.5,0.0],[1.0,-1.0]], 'float', 'maxsim', 'topn=5');";
        let before_rows = row_strings(&engine, query).await;
        assert_eq!(before_rows.len(), 5);

        let chunks = engine.read_plaid_index_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2);
        let dir = engine.plaid_mirror("pc", "pidx").unwrap().dir;
        let chunk_bytes: Vec<Vec<u8>> = (0..2)
            .map(|id| std::fs::read(dir.join(format!("chunk_{id}.idx"))).unwrap())
            .collect();
        // Exact MaxSim of every mirrored document, with the id the merged index
        // must give it: chunk 1 follows chunk 0.
        let probe: Vec<f32> = vec![0.5, 0.0, 1.0, -1.0];
        let mut expected: Vec<f32> = Vec::new();
        for chunk in &chunks {
            let ids: Vec<u32> = (0..chunk.doc_num()).collect();
            expected.extend(chunk.exact_score_batch(&probe, 2, &ids));
        }
        assert_eq!(expected.len(), 260);
        let token_offset = chunks[0].total_embedding_num();

        // Two 200/60-row chunks are not worth folding under the small-chunk rule
        // (`60 * 16 >= 260`): the cap is what makes an optimize pass fold them,
        // so the test pins it at one chunk.
        engine.set_max_open_chunks_per_index(1);
        engine.optimize_indexes().await.unwrap();
        let merged = engine.read_plaid_index_chunks("pc", "pidx").unwrap();
        assert_eq!(
            merged.len(),
            1,
            "the merge replaces the chunks it folded in"
        );
        let merged = &merged[0];
        assert_eq!(merged.doc_num(), 260);
        assert_eq!(
            merged.total_embedding_num(),
            chunks[0].total_embedding_num() + chunks[1].total_embedding_num()
        );
        assert_eq!(merged.n_centroids(), chunks[0].n_centroids());
        assert_eq!(merged.centroids_data(), chunks[0].centroids_data());
        // Document 200 of the merge points just past chunk 0's tokens.
        assert_eq!(
            merged.doc_offsets()[200],
            token_offset + chunks[1].doc_offsets()[0]
        );
        let ids: Vec<u32> = (0..260).collect();
        let got = merged.exact_score_batch(&probe, 2, &ids);
        for (i, score) in got.iter().enumerate() {
            assert!(
                (score - expected[i]).abs() < 1e-6,
                "doc {i}: merged {score} != unmerged {}",
                expected[i]
            );
        }

        // The bytes of the chunks a merge superseded stay on disk until
        // `Cleanup` - the optimize commit only takes them out of the chunk
        // list. Putting them back proves both halves: a reader does not see
        // them now, and cleanup is what deletes the files.
        std::fs::write(dir.join("chunk_0.idx"), &chunk_bytes[0]).unwrap();
        std::fs::write(dir.join("chunk_1.idx"), &chunk_bytes[1]).unwrap();
        assert_eq!(
            engine.read_plaid_index_chunks("pc", "pidx").unwrap().len(),
            1,
            "the superseded chunks are out of the chunk list"
        );
        engine.cleanup().await.unwrap();
        assert!(
            !dir.join("chunk_0.idx").exists(),
            "cleanup deletes the bytes"
        );
        assert!(!dir.join("chunk_1.idx").exists());
        let after = engine.read_plaid_index_chunks("pc", "pidx").unwrap();
        assert_eq!(after.len(), 1, "cleanup leaves the merged chunk");
        assert_eq!(after[0].doc_num(), 260);
        assert_eq!(
            row_strings(&engine, query).await,
            before_rows,
            "the search path answers the same rows after the merge"
        );
    }

    /// The optimize pass only starts on a plan: over the cap everything folds,
    /// under the cap only a chunk small enough to be pure overhead does.
    #[test]
    fn the_fold_plan_folds_at_the_cap_and_for_a_small_chunk() {
        use crate::blocks::index_chunk::plaid_fold_plan;
        // One chunk: nothing to fold, whatever its size.
        assert_eq!(plaid_fold_plan(&[10], 8), None);
        // Two comparable chunks: leave them, the loader's merge is cheap enough.
        assert_eq!(plaid_fold_plan(&[200, 60], 8), None);
        // Over the cap: everything into one, or every open merges the chain.
        assert_eq!(
            plaid_fold_plan(&[10, 10, 10, 10, 10, 10, 10, 10, 10], 8),
            Some((0..9).collect())
        );
        // Under the cap but a chunk below 1/16 of the mirror: it folds into its
        // neighbour, and only that pair - folding all of them would rewrite the
        // mirror for a 10-row chunk.
        assert_eq!(plaid_fold_plan(&[200, 60, 10], 8), Some(vec![1, 2]));
        assert_eq!(plaid_fold_plan(&[10, 200, 200], 8), Some(vec![0, 1]));
    }

    /// A dump opens a **new** chunk; it does not extend the newest one.
    ///
    /// `DumpMemIndex` writes a fresh chunk id and advances
    /// `current_begin_row_id_` past the rows it wrote; folding the two is
    /// `OptimizePlaidIndex`'s job, on its own transaction. Extending the tail
    /// here rewrote the newest chunk on every dump and hid the growth from the
    /// loader.
    #[tokio::test]
    async fn a_dump_opens_a_new_chunk_instead_of_extending_the_tail() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE pa (id INT, t TENSOR(FLOAT,2));")
            .await
            .unwrap();
        engine
            .execute(&format!("INSERT INTO pa VALUES {};", plaid_rows(0, 200)))
            .await
            .unwrap();
        engine
            .execute("CREATE INDEX paidx ON pa (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .await
            .unwrap();
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pa", "paidx").await.unwrap();
        let first = engine.read_plaid_index_chunks("pa", "paidx").unwrap();
        assert_eq!(first.len(), 1);
        assert_eq!(first[0].doc_num(), 200);

        engine
            .execute(&format!("INSERT INTO pa VALUES {};", plaid_rows(200, 260)))
            .await
            .unwrap();
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pa", "paidx").await.unwrap();

        // A second chunk for the 60 new rows, starting where the first ended.
        let chunks = engine.read_plaid_index_chunks("pa", "paidx").unwrap();
        assert_eq!(chunks.len(), 2, "the dump extended the tail chunk");
        assert_eq!(chunks[0].doc_num(), 200);
        assert_eq!(chunks[1].doc_num(), 60);
        let records = engine.plaid_chunk_records("pa", "paidx").unwrap();
        assert_eq!(records.len(), 2);
        assert_eq!(
            records[1].1, 200,
            "the new chunk starts past the dumped rows"
        );

        // Both chunks answer, and the mirror still reloads.
        let query = "SELECT id FROM pa SEARCH MATCH TENSOR \
                     (t, [[0.5,0.0],[1.0,-1.0]], 'float', 'maxsim', 'topn=5');";
        assert_eq!(row_strings(&engine, query).await.len(), 5);
        drop(engine);
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        let merged = engine.read_plaid_index("pa", "paidx").unwrap();
        assert_eq!(merged.doc_num(), 260);
    }

    /// Under the cap, a chunk below 1/16 of the mirror is folded into its
    /// neighbour by the optimize pass - and it keeps every row.
    #[tokio::test]
    async fn a_small_chunk_folds_into_its_neighbour() {
        let dir = tempfile::tempdir().unwrap();
        let engine = plaid_mirror_with_two_chunks(dir.path()).await;
        // 200 + 60 already. A target of 64 rows leaves no room for the 10-row
        // dump either, so it becomes a third chunk - the one the fold rule has to
        // pick up, since nothing else in this mirror is worth folding.
        engine.set_plaid_chunk_target_rows(64);
        engine
            .execute(&format!("INSERT INTO pc VALUES {};", plaid_rows(260, 270)))
            .await
            .unwrap();
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pc", "pidx").await.unwrap();
        // The dump itself folds it: `fold_dumped_mirror` runs on the cheap plan,
        // so an embedder with no optimize tick still converges. The 10-row chunk
        // joins its 60-row neighbour; the 200-row one is left alone.
        let after: Vec<u32> = engine
            .read_plaid_index_chunks("pc", "pidx")
            .unwrap()
            .iter()
            .map(|chunk| chunk.doc_num())
            .collect();
        assert_eq!(after, vec![200, 70], "the small chunk folded on the dump");

        // An explicit pass has nothing left to do, and must not touch the mirror.
        engine.optimize_indexes().await.unwrap();
        let again: Vec<u32> = engine
            .read_plaid_index_chunks("pc", "pidx")
            .unwrap()
            .iter()
            .map(|chunk| chunk.doc_num())
            .collect();
        assert_eq!(again, vec![200, 70]);

        let merged = engine.read_plaid_index("pc", "pidx").unwrap();
        assert_eq!(merged.doc_num(), 270, "no row was lost by the fold");
        // Documents are addressed by row order: the folded chunk must not move
        // the rows of the chunk after it.
        assert_eq!(merged.doc_offsets()[200], 200 * 3);
    }

    /// The trigger's cheap check: no fold candidate means no optimize task.
    #[tokio::test]
    async fn the_optimize_trigger_skips_a_mirror_that_needs_nothing() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        engine
            .execute("CREATE TABLE pt (id INT, t TENSOR(FLOAT,2));")
            .await
            .unwrap();
        engine
            .execute(&format!("INSERT INTO pt VALUES {};", plaid_rows(0, 200)))
            .await
            .unwrap();
        engine
            .execute("CREATE INDEX ptidx ON pt (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .await
            .unwrap();
        engine.flush().await.unwrap();
        engine.dump_index_transaction("pt", "ptidx").await.unwrap();
        assert!(
            !engine.needs_index_optimize(),
            "one chunk is nothing to fold, the tick must not queue a pass"
        );
        assert!(!engine.submit_optimize_indexes(), "no processor, no task");
    }

    /// A checkpoint rewrites the whole C++ layout from its own read-back
    /// (`Catalog::rewrite_layout`), so every record it does not carry
    /// through is lost. `first_delete_ts` is such a record: the delete writes
    /// it, the read-back has to hand it back, or the next checkpoint tells
    /// every reader that nobody ever deleted a row.
    #[tokio::test]
    async fn a_checkpoint_keeps_the_segments_first_delete_ts() {
        let dir = tempfile::tempdir().unwrap();
        {
            let e = Engine::open_async(dir.path(), Default::default())
                .await
                .unwrap();
            e.execute("CREATE TABLE t (id INT, v FLOAT);")
                .await
                .unwrap();
            e.execute("INSERT INTO t VALUES (1, 1.0), (2, 2.0), (3, 3.0);")
                .await
                .unwrap();
            e.write_blocks("t").await.unwrap();
        }
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        e.execute("INSERT INTO t VALUES (4, 4.0);").await.unwrap();
        e.execute("DELETE FROM t WHERE id = 1;").await.unwrap();
        // The delete is in the table's delete log; the segment record that
        // carries `first_delete_ts` is (re)written by the checkpoint, which is
        // where an append-buffer-only delete reaches the block files.
        e.write_blocks("t").await.unwrap();
        let delete_ts = e
            .delete_ts("t")
            .into_iter()
            .find(|ts| *ts != 0)
            .expect("the delete was stamped");
        let tag = |e: &Engine| {
            e.storage
                .catalog
                .store()
                .get(b"seg|1|0|0|first_delete_ts")
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string())
        };
        assert_eq!(tag(&e).as_deref(), Some(delete_ts.to_string().as_str()));
        // The checkpoint sweeps the layout and writes it back; a DDL captures
        // it the same way.
        e.flush().await.unwrap();
        assert_eq!(
            tag(&e).as_deref(),
            Some(delete_ts.to_string().as_str()),
            "a checkpoint must not claim the segment has no deletes"
        );
        e.execute("CREATE INDEX ix ON t (id);").await.unwrap();
        assert_eq!(
            tag(&e).as_deref(),
            Some(delete_ts.to_string().as_str()),
            "a DDL must not claim the segment has no deletes"
        );
    }

    #[test]
    fn plaid_coverage_counts_a_merged_newest_chunk_once() {
        assert_eq!(plaid_coverage(&[(200, 600)]), (200, 600));
        assert_eq!(plaid_coverage(&[(200, 600), (60, 180)]), (260, 780));
        assert_eq!(
            plaid_coverage(&[(200, 600), (60, 180), (260, 780)]),
            (260, 780),
            "the merged chunk replaces the ones it was built from"
        );
        // A new batch on top of a merge is additive again.
        assert_eq!(
            plaid_coverage(&[(260, 780), (10, 30)]),
            (270, 810),
            "an incremental chunk after a merge still adds up"
        );
    }

    /// Blocks decode as tokio tasks when there is no row predicate (plan
    /// 6.13.2). The parallel pass must return exactly what the serial walk of
    /// the same blocks returns, in the same order - the fan-out only decides
    /// who decodes which block, not what a block holds.
    #[tokio::test]
    async fn parallel_block_decode_matches_the_serial_scan() {
        let dir = tempdir().unwrap();
        // The parallel decode needs an owned engine handle (one task per
        // block); without it the scan is serial and this test would compare the
        // serial walk with itself.
        let e = std::sync::Arc::new(
            Engine::open_async(dir.path(), Default::default())
                .await
                .unwrap(),
        );
        e.install_engine_handle();
        e.execute("CREATE TABLE t (id INT, name VARCHAR, score FLOAT);")
            .await
            .unwrap();
        // Three blocks of rows, so the fan-out actually has more than one.
        e.import_literals(
            "default_db.t",
            None,
            (0..(DEFAULT_BLOCK_CAPACITY as i64 * 3))
                .map(|i| {
                    vec![
                        parser::Literal::Int(i),
                        parser::Literal::Str(format!("doc-{i}")),
                        parser::Literal::Float(i as f64 / 8.0),
                    ]
                })
                .collect(),
        )
        .await
        .unwrap();
        e.flush().await.unwrap();

        let wanted: Vec<usize> = (0..3).collect();
        // No predicate: blocks decode as tasks under the core bound.
        let (names, parallel) = e
            .read_blocks_pruned_values("t", &wanted, &[])
            .await
            .unwrap();
        assert_eq!(names.len(), 3);
        let ids = e.visible_block_ids("default_db.t");
        assert!(ids.len() >= 3, "the import should span three blocks");
        // A predicate pins the scan to the serial walk (`&dyn Fn` is not
        // `Sync`), and one that keeps everything returns the same rows.
        let (_, serial) = e
            .read_blocks_filtered_values("t", &wanted, &[], &|_: &[Value]| true)
            .await
            .unwrap();
        assert_eq!(parallel.len(), serial.len(), "same row count");
        for (a, b) in parallel.iter().zip(&serial) {
            assert_eq!(a.0, b.0, "the parallel pass must not reorder the rows");
            assert_eq!(a.1, b.1, "the parallel pass must not drop a cell");
        }
        // And the order is the row-id order the serial walk produced.
        let mut sorted = parallel.clone();
        sorted.sort_by_key(|(seq, _)| *seq);
        assert_eq!(parallel, sorted);
    }

    /// A `create` may not name an append whose rows the write did not reach.
    ///
    /// The journal is filled in a commit's top half; the segment map that
    /// decides which rows a write walks moves in its bottom. A checkpoint that
    /// lands between the two sees an append it will not write, and stamping
    /// that append's ts here is not cosmetic: the `version` creates are the row
    /// floor a reopen derives from the files (`Engine::recover_blocks` ->
    /// `newest`), so the WAL replay would skip exactly that batch and a
    /// `kill -9` would lose it - the `restart_test` checkpoint race, 8192 rows
    /// short after a restart.
    #[tokio::test]
    async fn a_create_never_names_an_append_the_write_did_not_cover() {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        // Two appends of four rows each; the write covers only the first.
        let totals = vec![4u64, 8];
        let ts = vec![10u64, 20];
        assert_eq!(
            e.block_version_batches(&totals, &ts, 0, 4, 4),
            vec![(10, 4)],
            "the uncovered append's ts must not reach the file"
        );
        // A write that stops inside the second append gives its rows no create
        // naming 20 either: the rest of that append is in no block.
        assert_eq!(
            e.block_version_batches(&totals, &ts, 0, 6, 6),
            vec![(10, 4), (10, 2)]
        );
        // Once the journal and the segment map agree, the ts is the append's
        // own - the pre-existing shape, unchanged.
        assert_eq!(
            e.block_version_batches(&totals, &ts, 0, 8, 8),
            vec![(10, 4), (20, 4)]
        );
        // A journal that stops short of the rows present (an append the
        // previous process lost) keeps its old shape: one create at the newest
        // known ts, which is also the newest ts this write covers.
        assert_eq!(
            e.block_version_batches(&[4], &[10], 0, 6, 6),
            vec![(10, 6)],
            "the disagreeing journal falls back to the covered ts"
        );
    }
}

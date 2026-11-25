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

impl Engine {
    /// The upper bound a checkpoint writes at: the last commit whose bottom half
    /// finished (`NewTxnManager::current_ts_`, the checkpoint's `begin_ts`).
    ///
    /// Everything at or below it is in the block buffer, so a block's rows form
    /// a dense prefix; everything above it belongs to a transaction a reader at
    /// this ts could not see either.
    pub(crate) fn checkpoint_ts(&self) -> u64 {
        self.txn.current_ts()
    }

    /// Merges the segments `NewCompactionAlg` picks into one new segment: their
    /// live rows are re-keyed contiguously from the new segment's base row id,
    /// their tombstones are dropped, and every other segment is left exactly as
    /// it was.
    ///
    /// The reference (`NewTxn::Compact`) writes the merged rows into a fresh
    /// segment whose id is one past the highest existing segment id; the source
    /// segments become garbage that `Cleanup` reclaims. Mirroring that here
    /// keeps the segment ids the reference's fixtures assert (`{2}` after two
    /// imports are compacted, `{3}` after compacting `{2}`, and so on).
    /// `ManualCompactTask` hands `PhysicalCompact` the candidates
    /// `NewCompactionAlg` picked (`physical_compact_impl.cpp:36`). When it
    /// picks nothing the reference opens **no** transaction and leaves the
    /// directory untouched; merging anyway would move every row and spend a
    /// timestamp it never spends.
    pub async fn compact_table(&self, table: &str) -> Result<()> {
        let candidates = self.compaction_candidates(table);
        if candidates.is_empty() {
            return Ok(());
        }
        let built: std::pin::Pin<Box<dyn std::future::Future<Output = Result<()>> + Send + '_>> =
            Box::pin(self.compact_segments(table, &candidates));
        built.await
    }

    /// `NewTxn::Compact`: merge every segment of `table` into one new segment.
    ///
    /// This is the primitive the reference's compaction *tests* drive directly
    /// (`TestTxnCompact` calls `NewTxn::Compact` with explicit segment ids), so
    /// it merges unconditionally — even a single segment, which the manual and
    /// periodic tasks would never choose.
    pub async fn compact_all_segments(&self, table: &str) -> Result<()> {
        let all = self.segment_ids(table).await;
        self.compact_segments(table, &all).await
    }

    /// `NewTxn::OptimizeIndex` for the full-text indexes of `table`
    /// (`new_txn_index_impl.cpp:332-370` → `OptimizeFtIndex`, `:2179-2270`):
    /// merges each index's chunk list into a single
    /// `ft_{base:016x}_{rows:x}` chunk.
    ///
    /// No row moves — this only rewrites the inverted index's files, which is
    /// why the reference calls it from `OptimizeIndex` and not from `Compact`
    /// (`Compact` re-writes indexes from rows instead). Returns how many chunk
    /// merges happened; an index with one chunk costs nothing.
    ///
    /// The gate is a `tokio::sync::Mutex`, so waiting for it has to be an
    /// `.await`. A synchronous wrapper that bridged the wait with
    /// `scheduler::block_on` used to exist and was a deadlock: on a runtime
    /// worker that call is a park/unpark loop holding the worker, and an insert
    /// that owns the gate can only finish on that same worker - the optimizer
    /// then waits for itself (observed as a background optimize that never
    /// returns, with every client of the table stuck behind it).
    /// Public for embedders and the scale probe. **Await it inside the
    /// runtime**: it takes the table's batch gate, which an insert may hold
    /// across its own awaits, so bridging it from a runtime worker with
    /// `block_on` deadlocks (see the note above).
    pub async fn optimize_fulltext_async(&self, table: &str) -> Result<usize> {
        self.ensure_writable()?;
        let gate = self.batch_gate(table);
        let _batch = gate.lock().await;
        self.optimize_fulltext_locked(table).await
    }

    /// The body [`Self::optimize_fulltext_async`] runs: the gate is held.
    async fn optimize_fulltext_locked(&self, table: &str) -> Result<usize> {
        let key = self.table_key(table);
        // **The catalog is the input**, exactly as in the reference: a merge
        // reads `SegmentIndexMeta`'s chunk ids (`OptimizeFtIndex`,
        // `new_txn_index_impl.cpp:2193-2245`) and never a memory index. Walking
        // only the cached indexes left every index of a process that had not
        // answered a query on the column yet unmerged - a reopened engine kept
        // its chunk list fragmented for good, because nothing but a `MATCH` or a
        // `filter_fulltext` ever loads the cache.
        let catalog_indexes: Vec<(u64, String)> = self
            .storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .filter(|im| im.index_type.eq_ignore_ascii_case("FULLTEXT"))
            .map(|im| (im.id, im.column))
            .collect();
        if catalog_indexes.is_empty() {
            return Ok(0);
        }
        // Clone the handles while the `tables` guard is held, then drop it before
        // taking any index write lock. `text_index_for` takes `tables` first and
        // the index second, so waiting for an index lock with `tables` held is the
        // reverse order and can deadlock a concurrent seal or query.
        let cached: Vec<(u64, SharedTextIndex)> = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&key)
                .map(|entry| entry.text.iter().map(|(id, v)| (*id, v.clone())).collect())
                .unwrap_or_default()
        };
        // A merge is the only point at which the postings of a deleted row can
        // be reclaimed (a chunk is immutable once published), so it takes the
        // table's deleted rows with it.
        let deleted = self.tombstone_set(table)?;
        if crate::engine::text_index::ft_trace_enabled(&self.table_key(table)) {
            eprintln!(
                "[FT] optimize_fulltext_locked {table} indexes={} cached={}",
                catalog_indexes.len(),
                cached.len()
            );
        }
        let mut merges = 0;
        for (index_id, column) in catalog_indexes {
            let shared = cached
                .iter()
                .find(|(id, _)| *id == index_id)
                .map(|(_, shared)| shared.clone());
            match shared {
                Some(index) => {
                    // The merge rewrites whole chunks and fsyncs every file it
                    // writes, which `optimize_text_index_unlocked` already
                    // moves to the blocking pool (`TextIndex::merge_runs`) on
                    // its own - wrapping it again would hand a blocking thread
                    // to a function whose job is to wait for one.
                    let done = crate::engine::state::optimize_text_index_unlocked(&index, &deleted)
                        .await?;
                    if done == 0 {
                        continue;
                    }
                    merges += done;
                    if crate::engine::text_index::ft_trace_enabled(&self.table_key(table)) {
                        eprintln!(
                            "[FT] {table}.{column} optimize merged {done}, republishing records"
                        );
                    }
                    // The merge replaced chunks: the catalog's chunk list is what
                    // a reader maps, so it has to name the merged chunk and stop
                    // naming the sources the merge deleted (`OptimizeFtIndex` ->
                    // `RemoveChunkIDs`).
                    self.republish_text_chunks(table, index_id, &column).await;
                }
                None => {
                    // No cache entry: map the chunk list from the catalog, merge
                    // it, and publish the result. Nothing is loaded back into
                    // `tables`, so an optimize tick over a large catalog does not
                    // turn into a resident index per column.
                    let Some(root) = self.fulltext_index_root_of(table, index_id) else {
                        continue;
                    };
                    let chunks = self.fulltext_catalog_chunks_of(table, index_id)?;
                    if chunks.len() < 2 {
                        continue;
                    }
                    let dir = root.to_string_lossy().into_owned();
                    // The merge itself is the blocking part, and it is moved
                    // to the pool inside `merge_text_index_from_catalog` (the
                    // `TextIndex::merge_runs` call); wrapping it again would
                    // hand a blocking thread to a function that waits on one.
                    let (done, held): (usize, Vec<crate::engine::text_index::HeldTextChunk>) =
                        crate::engine::state::merge_text_index_from_catalog(
                            &dir, &chunks, &deleted,
                        )
                        .await?;
                    if done == 0 || held.is_empty() {
                        continue;
                    }
                    merges += done;
                    if crate::engine::text_index::ft_trace_enabled(&self.table_key(table)) {
                        eprintln!(
                            "[FT] {table}.{column} optimize merged {done} (uncached), \
                             republishing records"
                        );
                    }
                    self.republish_text_chunks_of(table, index_id, &column, &held);
                }
            }
        }
        Ok(merges)
    }

    /// Rewrites one full-text index's chunk records from the chunks it holds
    /// now.
    ///
    /// A merge retires its sources and writes one new chunk, so the record list
    /// has to be replaced: `RemoveChunkIDs` + `AddNewChunkIndex1`, exactly what
    /// `OptimizeFtIndex` does. The replacement is built **from the catalog**:
    /// a chunk whose `base_name` already has a record keeps its id, and only a
    /// chunk no record names yet (the merged one) takes the id
    /// `GetAndSetNextChunkID` hands out. Counting files instead would name the
    /// merged chunk after the first source's id and hand that id out twice.
    pub(crate) async fn republish_text_chunks(&self, table: &str, index_id: u64, column: &str) {
        let qualified = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&qualified) else {
            return;
        };
        if crate::engine::text_index::ft_trace_enabled(&qualified) {
            eprintln!(
                "[FT] republish {qualified}.{column} idx={index_id} tbl={}",
                meta.id
            );
        }
        let key = self.table_key(&qualified);
        // The handle is cloned out of `tables` and the guard dropped before the
        // index is awaited: a `std::sync::MutexGuard` may not cross an await.
        let shared = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&key)
                .and_then(|entry| entry.text.get(&index_id).cloned())
        };
        let held: Vec<(u32, String, u64, u64, u64)> = match shared {
            Some(shared) => {
                let index = shared.read().await;
                index
                    .reader
                    .segments
                    .iter()
                    .map(|s: &crate::engine::text_index::SealedSegment| {
                        let base_row_id = s.reader.base_row_id();
                        (
                            (base_row_id >> 32) as u32,
                            s.reader.base_name().to_string(),
                            base_row_id,
                            u64::from(s.docs),
                            s.total_len,
                        )
                    })
                    .collect()
            }
            None => Vec::new(),
        };
        self.republish_text_chunks_of(&qualified, index_id, column, &held);
    }

    /// [`Self::republish_text_chunks`] with the chunk list the merge produced,
    /// for a merge that ran on an index **not** held in the cache
    /// ([`crate::engine::state::merge_text_index_from_catalog`]).
    ///
    /// `held` is `(segment, base_name, base_row_id, row_count, term_count)` per
    /// chunk the index holds now.
    pub(crate) fn republish_text_chunks_of(
        &self,
        table: &str,
        index_id: u64,
        column: &str,
        held: &[(u32, String, u64, u64, u64)],
    ) {
        let qualified = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&qualified) else {
            return;
        };
        let set = catalog::layout::SegmentSet {
            db_id: self.database_id_of(&qualified).to_string(),
            table_id: meta.id.to_string(),
            segments: Vec::new(),
        };
        let existing = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &set.db_id,
            &set.table_id,
            &index_id.to_string(),
        )
        .unwrap_or_default();
        // The ids each surviving `base_name` already owns; only a chunk no
        // record names (the merged one) takes a fresh id from the counter.
        //
        // The counter is **per segment** (`SegmentIndexMeta::next_chunk_id`), so
        // it is read lazily for the segment being allocated: taking segment 0's
        // value and handing it to every segment restarts a segment that had
        // already used 0..19 at 0, and the new chunk overwrote one of its
        // files.
        let mut next_by_segment: std::collections::HashMap<u32, u32> = Default::default();
        let mut by_base: std::collections::HashMap<String, u32> = Default::default();
        for (_, chunks) in &existing {
            for (id, info) in chunks {
                if !info.base_name.is_empty() {
                    by_base.insert(info.base_name.clone(), *id as u32);
                }
            }
        }
        let mut per_segment: std::collections::BTreeMap<u32, Vec<catalog::layout::IndexChunk>> =
            Default::default();
        let ts = self.txn.current_ts().max(1);
        for (segment, base_name, base_row_id, row_count, term_count) in held {
            let chunk_id = match by_base.get(base_name) {
                Some(id) => *id,
                None => {
                    let next = next_by_segment.entry(*segment).or_insert_with(|| {
                        self.next_index_chunk_id(&set, &index_id.to_string(), *segment)
                    });
                    let id = *next;
                    *next += 1;
                    id
                }
            };
            per_segment
                .entry(*segment)
                .or_default()
                .push(catalog::layout::IndexChunk {
                    chunk_id,
                    base_name: base_name.clone(),
                    base_row_id: *base_row_id,
                    row_count: *row_count,
                    term_count: *term_count,
                    index_size: 0,
                    commit_ts: ts,
                });
        }
        // The segments the merge emptied keep no chunks at all
        // (`RemoveChunkIDs` on the last one).
        let mut segments: std::collections::BTreeSet<u32> =
            existing.iter().map(|(s, _)| *s).collect();
        segments.extend(per_segment.keys().copied());
        for segment in segments {
            let chunks = per_segment.remove(&segment).unwrap_or_default();
            if let Err(error) = catalog::layout::replace_segment_chunks(
                self.storage.catalog.store().as_ref(),
                &set,
                index_id,
                segment,
                &chunks,
            ) {
                eprintln!(
                    "[FT] republish {qualified}.{column} idx {index_id} seg {segment}: {error}"
                );
            }
        }
    }

    /// `PopulateIndex(…, DumpIndexCause::kCompact)` for every index after a
    /// merge: the merged segment gets its own chunk and the chunks of the
    /// source segments leave the index's list.
    ///
    /// The reader maps the catalog's chunk list, so a source segment's chunk
    /// (whose row ids the merge retired) must stop being listed; leaving it in
    /// would map postings onto rows nothing owns, and a query over them answers
    /// with rows the fetch cannot resolve. This is the reference's
    /// `CompactTxnStore::deprecate_ids_in_segments_` (`base_txn_store_impl.cpp`
    /// `ToWalEntry`, `kCompact`).
    async fn populate_compacted_indexes(
        &self,
        table: &str,
        merged_segment: u32,
        sources: &[u32],
        compact_ts: u64,
    ) {
        #[cfg(test)]
        crate::engine::statements::run_compaction_window_hook(table, "records");
        // Every record this pass writes carries the compaction's ts
        // (`PopulateIndex(…, kCompact)` runs inside the transaction, so
        // `current_ts()` - still the previous commit here - is not it).
        crate::engine::with_chunk_ts(compact_ts, async {
            self.populate_secondary_indexes(table);
        })
        .await;
        let qualified = self.qualified_table(table);
        let Ok(indexes) = self.storage.catalog.indexes_of_table(&qualified) else {
            return;
        };
        let key = self.table_key(&qualified);
        for im in indexes
            .iter()
            .filter(|im| im.index_type.eq_ignore_ascii_case("FULLTEXT"))
        {
            // Build the merged segment's chunk from its rows, then drop every
            // other segment's chunks.
            // A block, not a temporary chain: the `std::sync::MutexGuard` from
            // `tables.lock()` lives until the end of the enclosing statement,
            // and this statement is inside an `async fn` that awaits below - a
            // guard alive at any await point makes the future non-`Send`.
            let shared = {
                let tables = self.tables.lock().unwrap();
                tables.get(&key).and_then(|e| e.text.get(&im.id).cloned())
            };
            let Some(shared) = shared else {
                continue;
            };
            // The whole publish - the merged chunk's files, its record and the
            // records of the segments this compaction retired - under one gate:
            // a build that loaded the chunk list in between would map a layout
            // that is torn in two (`Engine::with_text_dir_gate`).
            let sources_owned: Vec<u32> = sources.to_vec();
            Self::with_text_dir_gate(&shared, async {
                crate::engine::state::seal_text_index_gated(&shared).await;
                self.rewrite_text_chunk_segments(&qualified, im.id, merged_segment, &sources_owned);
            })
            .await;
            // The record is written **separately** and with the compaction's
            // ts: `record_sealed_text_chunks_of_at` takes an explicit value
            // because `current_ts()` is still the previous commit while the
            // compaction transaction is open (`AddChunkIndexID1` under
            // `kStarted` stores what the commit resolves).
            self.record_sealed_text_chunks_at(&qualified, &im.column, Some(compact_ts))
                .await;
            // `reindex_compacted_text` already sealed and recorded the merged
            // chunk, under `current_ts()`; a record that exists keeps the ts it
            // was written with, so the merged segment's id is restamped here.
            let _ = self.stamp_chunk_ts_of(&qualified, &im.name, compact_ts, |segment, _, _| {
                segment == merged_segment
            });
        }
        // The **other** kinds: `PopulateIndex(…, kCompact)` calls the same
        // per-kind builders the import does, so the merged segment gets a chunk
        // from HNSW, IVF, BMP and the multi-vector indexes too
        // (`new_txn_data_impl.cpp:808`). Without them a query over the merged
        // segment fell back to the exact scan for every kind but full-text and
        // secondary - and the segment's directory had no chunk to publish.
        self.populate_merged_segment_indexes(&qualified, merged_segment, &indexes, compact_ts)
            .await;
    }

    /// `PopulateIndex` for the kinds that rebuild a whole segment from its
    /// blocks: HNSW, IVF, BMP and the multi-vector family.
    ///
    /// `PopulateIndex(…, kCompact)` runs these inside the compaction
    /// transaction (`new_txn_data_impl.cpp:808`), so the merged segment's chunk
    /// carries the compaction's ts. The port's compaction renumbers every row,
    /// so the memory forms of these indexes were dropped with the old layout
    /// (`entry.hnsw_mem.clear()` and friends) - the merged chunk can only come
    /// from a rebuild, which is what each builder does.
    async fn populate_merged_segment_indexes(
        &self,
        table: &str,
        merged_segment: u32,
        indexes: &[catalog::IndexMeta],
        compact_ts: u64,
    ) {
        // `with_chunk_ts` is a **task**-local scope, so the HNSW rebuild below
        // may await inside it: every task the build spawns inherits `ts`.
        crate::engine::with_chunk_ts(compact_ts, async {
            for im in indexes {
                if crate::is_secondary_kind(&im.index_type)
                    || im.index_type.eq_ignore_ascii_case("FULLTEXT")
                    || im.index_type.eq_ignore_ascii_case("SPANN")
                {
                    continue;
                }
                if im.index_type.eq_ignore_ascii_case("BMP") {
                    // Rebuild the index over the merged rows and seal it: the
                    // seal encodes the image the chunk has to be
                    // (`BMPHandler::SaveToPtr`), and `publish_index_chunks`
                    // writes exactly that.
                    let width = self
                        .table_schema(table)
                        .ok()
                        .and_then(|s| s.iter().position(|c| c == &im.column))
                        .and_then(|i| self.column_types(table).ok()?.get(i).cloned())
                        .map(|ty| crate::sparse_idx_width(&ty));
                    if let Some(width) = width {
                        let _ = self.sparse_index_by_id(table, &im.column, im.id, width);
                    }
                    self.seal_sparse_index(table, im.id).await;
                } else if im.index_type.eq_ignore_ascii_case("HNSW") {
                    // `PopulateIndex(…, kCompact)` rebuilds the merged segment
                    // and dumps its chunk, all inside the transaction.
                    let _ = self
                        .build_hnsw_index_of(table, &im.column, Some(im.id))
                        .await;
                    let _ = self.dump_hnsw_index(table, &im.name).await;
                    self.release_hnsw_memory(table, im.id);
                } else if im.index_type.eq_ignore_ascii_case("IVF") {
                    let _ = self.write_ivf_index(table, &im.name).await;
                    let key = self.table_key(table);
                    let mut tables = self.tables.lock().unwrap();
                    if let Some(rows) = tables.get_mut(&key).and_then(|e| e.ivf_mem.get_mut(&im.id))
                    {
                        rows.clear();
                    }
                } else if im.index_type.eq_ignore_ascii_case("PLAID")
                    || im.index_type.eq_ignore_ascii_case("SMVE")
                    || im.index_type.eq_ignore_ascii_case("EMVB")
                {
                    self.seal_multivector_index(table, im.id).await;
                    if im.index_type.eq_ignore_ascii_case("PLAID") {
                        let _ = self.write_plaid_index(table, &im.name).await;
                    }
                }
            }
            // The catalog records: `AddChunkIndexID1` inside the compaction
            // transaction. A BMP chunk is written by the mirror (its `.bmp`
            // spill is merged into one `chunk_N.idx` per segment), so this is
            // where its file and record appear together.
            if let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(table) {
                for im in indexes {
                    if crate::is_secondary_kind(&im.index_type)
                        || im.index_type.eq_ignore_ascii_case("FULLTEXT")
                        || im.index_type.eq_ignore_ascii_case("SPANN")
                    {
                        continue;
                    }
                    let _ = self
                        .publish_index_chunks(table, meta.id, Some((im.name.as_str(), compact_ts)))
                        .await;
                }
            }
        })
        .await;
        let _ = merged_segment;
        // The chunk files are new: reload the readers so the merged segment is
        // served from them instead of the exact scan.
        self.reload_vector_indexes();
    }

    /// Rebuilds the spann trees of the segments this compaction renumbered
    /// (`docs/SPANN_DESIGN.md` §9.8).
    ///
    /// The commit dropped every handle, because a tree names
    /// `segment << 32 | offset` and a merge renumbers the rows - keeping one
    /// answers with the wrong row, not just a stale one. Without this pass the
    /// index would stay flat until the next checkpoint; `publish_index_chunks`
    /// only dumps a segment whose chunk no longer matches its live rows, so the
    /// untouched segments keep their chunks and only the merged one is built.
    ///
    /// The handle is reloaded right away: `spann_lazy_load` is the same path a
    /// restart takes, and a failure there is not fatal - the query falls back
    /// to the exact scan.
    async fn rebuild_compacted_spann(&self, table: &str, ts: u64) {
        let qualified = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&qualified) else {
            return;
        };
        let Ok(indexes) = self.storage.catalog.indexes_of_table(&qualified) else {
            return;
        };
        for im in indexes
            .iter()
            .filter(|im| im.index_type.eq_ignore_ascii_case("SPANN"))
        {
            if let Err(e) = self
                .publish_index_chunks(&im.table, meta.id, Some((im.name.as_str(), ts)))
                .await
            {
                eprintln!("[SPANN] rebuild {} after compaction: {e}", im.name);
                continue;
            }
            self.spann_lazy_load(table, im.id);
        }
    }

    /// Keeps `merged_segment`'s chunks and drops the ones the compaction
    /// actually retired.
    ///
    /// `sources` is what the compaction merged, not "every segment but the
    /// merged one": `compact_segments` can be asked for a subset, and removing
    /// an untouched segment's records left its live rows with no registered
    /// postings.
    fn rewrite_text_chunk_segments(
        &self,
        table: &str,
        index_id: u64,
        merged_segment: u32,
        sources: &[u32],
    ) {
        let qualified = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&qualified) else {
            return;
        };
        let set = catalog::layout::SegmentSet {
            db_id: self.database_id_of(&qualified).to_string(),
            table_id: meta.id.to_string(),
            segments: Vec::new(),
        };
        let Ok(existing) = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &set.db_id,
            &set.table_id,
            &index_id.to_string(),
        ) else {
            return;
        };
        for (segment, chunks) in existing {
            if segment == merged_segment || chunks.is_empty() || !sources.contains(&segment) {
                continue;
            }
            // `RemoveChunkIDs` on a source segment: the chunk files stay until
            // `Cleanup`, but the reader no longer sees them.
            if let Err(error) = catalog::layout::replace_segment_chunks(
                self.storage.catalog.store().as_ref(),
                &set,
                index_id,
                segment,
                &[],
            ) {
                eprintln!("[FT] compact {qualified} idx {index_id} seg {segment}: {error}");
            }
        }
    }

    /// [`Self::optimize_fulltext_async`] for every open table — the full-text
    /// half of an optimize tick (`OptimizationProcessor` → `OptimizeAllIndexes`).
    pub(crate) async fn optimize_all_fulltext_indexes_async(&self) -> Result<usize> {
        let keys: Vec<String> = {
            let tables = self.tables.lock().unwrap();
            tables.keys().cloned().collect()
        };
        let mut merges = 0;
        for key in keys {
            merges += self.optimize_fulltext_async(&key).await?;
        }
        Ok(merges)
    }

    /// `NewTxn::Compact(db, table, segment_ids)`: merge exactly `sources` into
    /// one new segment and leave every other segment alone.
    ///
    /// The reference passes the ids `NewCompactionAlg` picked - at most
    /// `DBT_COMPACTION_M` (4) of them - so a table with more than four live
    /// segments keeps the rest: only the four become deprecated and the merged
    /// segment takes the next id after **every** segment, not after the picked
    /// ones (`SystemCache::ApplySegmentIDs`).
    pub(crate) async fn compact_segments(&self, table: &str, sources: &[u32]) -> Result<()> {
        // Compaction rewrites segments, so both the manual and the periodic
        // task require `kWritable` (`compaction_process_impl.cpp:270-295`).
        self.ensure_writable()?;
        // One write batch at a time per table: a compaction moves every row it
        // merges and drops the buffer entries of the segments it consumed, so
        // it must not interleave with an import's commit or with its block
        // write (a row the compaction cleared but never re-published left the
        // mirror with an empty block).
        let gate = self.batch_gate(table);
        let _batch = gate.lock().await;
        // The table's **bottom** gate is taken here and held for the whole
        // pass, not just across the commit. Two steps below read and write the
        // segments this compaction is about to retire - the pre-write of the
        // buffered rows (`write_blocks_bounded_locked`, which requires its caller
        // to hold this gate) and the source-row read - and an append that
        // published rows into a source segment between them would be retired
        // without ever being copied. `EngineTxn` commits take this gate but not
        // `batch_gate`, so the batch gate alone does not exclude them.
        let compact_gate = self.bottom_gate(table);
        let compact_turn = compact_gate.lock().await;
        self.invalidate_row_cache(table);
        // Compaction moves rows between segments without changing how many are
        // live, so the counter survives even though the mirror's row-id
        // snapshot does not (it stops matching the moment the first row moves).
        let live_rows = self.live_row_count(table)?;
        self.invalidate_block_mirror(table);
        let sources: std::collections::HashSet<u32> = sources.iter().copied().collect();
        let mut source_ids: Vec<u32> = sources.iter().copied().collect();
        source_ids.sort_unstable();
        // This pass moves rows by reading the **block files**. A segment the
        // append buffer still holds rows for has not got all of its rows there
        // yet, so those rows are written first - the reference reads rows out of
        // its blocks too, and it can always do so because `AppendInBlock` puts
        // them there during the commit. Skipping such a segment instead would
        // mean "a compaction right after an insert does nothing", which is what
        // `compact_all_segments`'s callers rely on it not doing.
        if source_ids.iter().any(|s| {
            let tables = self.tables.lock().unwrap();
            tables.get(&self.table_key(table)).is_some_and(|e| {
                (0..e.pending_rows.len()).any(|k| (e.pending_rows.row_id(k) >> 32) as u32 == *s)
            })
        }) {
            // No bound: every buffered row of this table is already committed
            // (a checkpoint's bound only decides which rows the *files* claim),
            // so this writes them all.
            let _ = self.write_blocks_bounded_locked(table, u64::MAX);
        }
        // The layout the row ids were allocated in, before anything moves:
        // `next_seg` counts from every segment, and the append log is rebased
        // against it.
        let layout = self.segment_row_counts(table);
        // One past every id the table has used, retired ones included.
        let next_seg = self.next_segment_id(table);
        // Only the picked segments' rows move, read one block at a time: a row
        // id names its segment, block and slot, so the walk never materialises
        // the table (compaction moves every row of its sources, and a table
        // scan per moved row would make it quadratic). The order - segment,
        // then offset - is the order the ids were handed out in, which is what
        // keeps the merged segment byte-identical to the reference's.
        let mut rows: Vec<(usize, Vec<u8>)> = Vec::new();
        {
            let mut cursor = self.block_cursor(table)?;
            for (segment, count) in &layout {
                if !sources.contains(segment) {
                    continue;
                }
                for offset in 0..*count {
                    let seq = ((*segment as usize) << 32) | offset as usize;
                    if let Some(text) = cursor.row(seq)? {
                        rows.push((seq, text));
                    }
                }
            }
        }
        // The new segment starts at its own row-id base (`segment_id << 32`).
        let base = (next_seg as usize) << 32;
        // Renumber through the transaction so WAL replay reproduces the
        // compacted state: a row moved from `old` to its destination also
        // leaves a tombstone for `old`, otherwise replay would resurrect it.
        let mut t = self.txn.begin();
        // `NewTxn::Compact`: exclusive per table against writes and index DDL.
        t.set_op(TxnOp::Compact {
            table: table.to_string(),
            segments: source_ids.clone(),
        });
        // `CompactTxnStore::ToWalEntry`: one `CompactV2` command carrying the
        // merged segment and the deprecated ids, instead of the row moves
        // (`new_txn_data_impl.cpp:753`), followed by the compaction's own
        // `DumpIndexV2` per index (`:502`; `CompactTxnStore` leaves the
        // `table_key` empty, so the port does too).
        let mut cmds = vec![self.compact_command(table, next_seg, rows.len(), &source_ids)?];
        cmds.extend(
            self.dump_index_commands(table, next_seg, rows.len(), "")
                .await?,
        );
        t.set_typed_commands(cmds);
        // `NewTxnManager::CommitTxn`: conflict check -> `PrepareCommitCompact`
        // -> WAL append+fsync. The reference writes the merged rows into the
        // new blocks while the statement runs and the block `version` +
        // `catalog|seg|`/`catalog|blk|` + `drop|seg|` markers in the prepare
        // phase (`new_txn_data_impl.cpp:2013`), i.e. everything the frame names
        // is on disk before the frame. The port committed first, so a kill in
        // between left the frame and the deprecations durable while the merged
        // segment had no blocks at all - the table came back with those rows
        // gone.
        let turn = self.txn.reserve_commit_ts();
        let compact_ts = turn.ts();
        // `compact_turn` (taken at the top of this pass) is still held, which is
        // what makes the commit and the pre-write above one step with respect to
        // every other writer of this table. The callback below is a synchronous
        // `FnOnce` on a runtime worker that already holds the commit turn and
        // `commit_lock`, so it can neither `.await` nor take the gate.
        self.txn
            .commit_at_deferred_with(t, compact_ts, async |_txn| {
                // Drop every source position that no compacted row reuses. The
                // destination positions are exactly `[base, base + rows.len())`; every
                // other key inside a merged segment is an old copy (including rows
                // deleted before the compaction that therefore never appear in `rows`)
                // and must go, or a later scan would see it again. Rows of segments
                // that were not merged stay where they are.
                let key = self.table_key(table);
                {
                    let mut tables = self.tables.lock().unwrap();
                    let entry = tables.entry(key).or_default();
                    entry.row_counter = Some((next_seg, rows.len() as u32, true));
                    entry.write_batches = 1;
                    // The merged rows all live in the new segment; their source segment
                    // metas are garbage collected with the old row keys, while the
                    // segments this compaction did not pick keep theirs.
                    entry.segments.retain(|id, _| !sources.contains(id));
                    if !rows.is_empty() {
                        entry.segments.insert(
                            next_seg,
                            SegmentEntry {
                                id: next_seg,
                                rows: rows.len() as u32,
                            },
                        );
                    }
                    // Compaction renumbered every row.
                    entry.row_cache = None;
                    // Every structure keyed by a row id refers to the old ids now:
                    // block-level indexes, the in-memory secondary tails, the full-text
                    // inverted index, the sparse documents and the vector buffers.
                    entry.secondary.indexes.clear();
                    entry.secondary.declared_types.clear();
                    entry.secondary.disk.clear();
                    // The full-text caches stay. Dropping them handed the next
                    // `MATCH` a full rebuild of the column, and a table that keeps
                    // taking writes is compacted every couple of minutes: on tens
                    // of thousands of rows that build outlives the next compaction,
                    // so its publish was refused and the build retried - one query
                    // blocked for the whole 20 minute gate
                    // (`TestParallelFulltextIndexReadWrite`, on the query after
                    // its writers stopped). `reindex_compacted_text` below adds
                    // the rows this merge moved under their new ids and marks the
                    // ids they left as overridden, which the sealed read paths
                    // skip before the top-n cut.
                    entry.text_epoch = entry.text_epoch.wrapping_add(1);
                    // A build that is running right now read the ids this
                    // compaction renumbers: publishing it later would leave the
                    // table's `MATCH` postings pointing at segments the merge just
                    // dropped, so the bump makes its publish a no-op. The caches
                    // kept above are re-stamped on the new epoch: their postings
                    // still name rows a reader below `compact_ts` can fetch, and
                    // `reindex_compacted_text` moves them before this commit is
                    // published.
                    let epoch = entry.text_epoch;
                    for built in entry.text_built.values_mut() {
                        built.0 = epoch;
                    }
                    entry.sparse.clear();
                    entry.mv.clear();
                    entry.hnsw_mem.clear();
                    entry.vector.clear();
                    entry.ivf.clear();
                    entry.ivf_mem.clear();
                    // A spann tree is keyed by row id, and the merge renumbers
                    // every row: the handle is not "a few stale entries" but a
                    // wrong answer. Drop it here; the rebuild right after the
                    // commit (`rebuild_spann_in`) refills the tree from the
                    // merged rows.
                    entry.spann.clear();
                    entry.spann_mem.clear();
                    entry.block_index = None;
                    entry.block_rows = None;
                    entry.live_rows = Some(live_rows);
                    // Every row moved to the merged segment: the files still describe
                    // the source layout until the next checkpoint rewrites them.
                    entry.mirror_dirty = true;
                }
                // The merged rows are one append of the compaction transaction: the
                // reference writes every block of the new segment with that commit ts
                // as its single `create`. The append log keeps the boundaries of the
                // segments this compaction did not pick - the mirror lays them out in
                // segment-id order and reads their block `version` files from the same
                // log - and drops the merged segments' own boundaries.
                if !rows.is_empty() {
                    let totals = self.rebase_appends(
                        table,
                        &layout,
                        &sources,
                        rows.len() as u64,
                        compact_ts,
                    );
                    let tkey = self.table_key(table);
                    {
                        let mut tables = self.tables.lock().unwrap();
                        let entry = tables.entry(tkey.clone()).or_default();
                        entry.append_totals = Some(totals.0.clone());
                        entry.append_ts = Some(totals.1.clone());
                        // A merge writes the new segment's blocks from the merged rows
                        // in one go, like an import: the null bitmap covers exactly
                        // those rows (tail bits of the last byte stay clear).
                        entry.append_copy = Some(totals.2.clone());
                    }
                    let _ = self.write_id_log(&appends_key(&tkey), &totals.0);
                    let _ = self.write_id_log(&append_ts_key(&tkey), &totals.1);
                    let _ = self.write_id_log(&append_copy_key(&tkey), &totals.2);
                    // `PopulateIndex` runs inside the compaction transaction too, so the
                    // new segment's chunks carry its ts and cover the whole segment.
                    self.note_import_ts(table, next_seg, compact_ts);
                    self.note_import_rows(table, next_seg, rows.len() as u64);
                }
                // `drop|seg|…` / `drop|idx_seg|…` at the compaction's ts: the source
                // segments stay on disk until `Cleanup`, and a reader at a later ts
                // ignores them because of these records.
                if !source_ids.is_empty() {
                    let db_id = self.database_id_of(table);
                    let table_id = self
                        .storage
                        .catalog
                        .get_table_by_name(table)?
                        .map(|m| m.id)
                        .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                    let index_ids: Vec<u64> = self
                        .storage
                        .catalog
                        .list_indexes()?
                        .iter()
                        .filter(|im| im.table == table || im.table == self.qualified_table(table))
                        .map(|im| im.id)
                        .collect();
                    self.storage.catalog.drop_segments(
                        &catalog::layout::SegmentSet {
                            db_id: db_id.to_string(),
                            table_id: table_id.to_string(),
                            segments: Vec::new(),
                        },
                        &index_ids,
                        &source_ids,
                        compact_ts,
                    )?;
                }
                // The merged rows are the new segment's blocks: the compaction is the
                // one commit whose rows move, so they are handed to the block writer
                // after `drop|seg|…` - before it, the rewrite would delete the source
                // segment directories the reference keeps until `Cleanup`.
                // The merge renumbers every row: whatever the append buffer held for
                // the source segments is gone with them.
                // The merged segments' rows are gone with them (they are re-published
                // under the new segment's ids below); the rows of segments this
                // compaction did not pick stay in the buffer - clearing all of them
                // dropped the rows of an import that had committed but whose mirror
                // write had not run yet, and the next write then found an empty block.
                {
                    let tkey = self.table_key(table);
                    let mut tables = self.tables.lock().unwrap();
                    if let Some(entry) = tables.get_mut(&tkey) {
                        entry.retain_buffered(|(seq, _)| !sources.contains(&((*seq >> 32) as u32)));
                    }
                }
                let merged: Vec<(usize, Vec<u8>)> = rows
                    .iter()
                    .enumerate()
                    .map(|(i, (_, text))| (base + i, text.clone()))
                    .collect();
                // Same rule as an import: the published rows and the block they
                // land in are one step, or a checkpoint between them writes a block
                // whose bitmap coverage came from a journal without these rows.
                let _held = crate::engine::statements::HeldBottom::enter(&self.table_key(table));
                self.publish_encoded_rows(table, &merged)?;
                // The moved rows are published under their new ids now: add them
                // to the full-text caches the merge kept (see the note where the
                // caches are no longer dropped).
                // Inside the commit callback: no `.await` is allowed here, so
                // the thread-local form is the one that applies.
                self.reindex_compacted_text(table, &rows, base, compact_ts)
                    .await;
                // `PrepareCommitCompact`: the merged segment's blocks and
                // records belong to the compaction transaction; writing them here
                // (before the frame) is what makes a kill harmless - the
                // `drop|seg|…` markers above are in the same pre-frame phase, so
                // a replay either sees the merged blocks or none of it.
                if !rows.is_empty() && self.has_block_layout(table) {
                    self.write_blocks_bounded_locked(table, compact_ts)?;
                }
                drop(_held);
                Ok(())
            })
            .await?;
        drop(compact_turn);
        // `CommitBottomCompact`: the merged rows are visible only once their
        // index entries are in place, so the commit stays unpublished until the
        // rebuild below finishes - `commit_at_deferred_with` leaves
        // `finish_bottom` to the caller.
        let _visible = BottomGuard::new(&self.txn, compact_ts);
        // The merge renumbered every row, so every index posting now points at
        // a dead row id. Rebuild the declared indexes from the merged rows.
        self.rebuild_indexes()?;
        // The spann tree is row-id keyed, so the merge invalidated it above.
        // Rebuild it from the merged rows now, inside the compaction's bottom
        // half: the commit stays unpublished until it is done, so no query can
        // observe the table between "tree dropped" and "tree rebuilt". The
        // segment set excludes the sources explicitly - they are only dropped
        // at `compact_ts`, and this half still reads below it.
        let live_segments: std::collections::HashSet<u32> = self
            .segment_row_counts(table)
            .into_iter()
            .map(|(segment, _)| segment)
            .filter(|segment| !sources.contains(segment))
            .chain(std::iter::once(next_seg))
            .collect();
        self.rebuild_spann_in(table, Some(&live_segments))?;
        if crate::engine::text_index::ft_trace_enabled(&self.table_key(table)) {
            let (segments, live_rows) = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&self.table_key(table))
                    .map(|e| (e.segments.keys().copied().collect::<Vec<_>>(), e.live_rows))
                    .unwrap_or_default()
            };
            eprintln!(
                "[FT] {table} compacted: next_seg={next_seg} merged_rows={} sources={source_ids:?} live_segments={segments:?} live_rows={live_rows:?}",
                rows.len()
            );
        }
        // The reference's compaction runs `PopulateIndex` for the merged
        // segment inside the compaction transaction, so that segment gets its
        // own index chunks and the source segments' chunks are deprecated
        // (`CompactTxnStore::ToWalEntry` -> `WalCmdDumpIndexV2`, `kCompact`).
        // The port reloads only the chunks the catalog lists, so the merged
        // segment's rows would otherwise stay in a memory tail and the old
        // chunks (whose row ids the merge retired) would keep answering.
        self.populate_compacted_indexes(table, next_seg, &source_ids, compact_ts)
            .await;
        // The trees of the merged segments were dropped with the commit (their
        // row ids are gone, §9.8). Rebuild them here instead of waiting for the
        // next checkpoint: the segment is fresh, so its chunk no longer matches
        // its live rows and `publish_index_chunks_of` dumps exactly that one.
        self.rebuild_compacted_spann(table, compact_ts).await;
        // The spann handles and their mem index were just dropped: the byte
        // ledger the watermarks read has to follow, or the table stays charged
        // for a tree nobody holds.
        self.refresh_memindex_charge(table);
        self.result_cache.lock().unwrap().clear();
        self.plan_cache.clear();
        // The commit is published only when `_visible` drops (end of scope), and
        // until then a query reads at a ts below `compact_ts` - it still sees the
        // source segments (`visible_block_ids`) and can publish an index of the
        // row ids this merge retires. Retire those: from `compact_ts` on, only an
        // index built against the merged layout may be served. The rebuild above
        // runs at `u64::MAX`, so it survives; a build a query started during the
        // merge is refused by `publish_text_index` - either because it started
        // before the epoch bump, or because the `ROW_ID` segments it indexed are
        // the ones this merge retires.
        //
        // Only the caches `reindex_compacted_text` moved onto the new ids carry
        // `compact_ts`; everything else was built from a retired layout. A
        // `built_ts < compact_ts` test is not enough to see that: a statement
        // reads at `current_ts() + 1`, so a build that started while this merge
        // was already running can claim a ts at or above `compact_ts` while it
        // still read the pre-merge rows (`TestParallelFulltextIndexReadWrite`,
        // `column "doctitle" is empty`).
        {
            let tkey = self.table_key(table);
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey).or_default();
            entry.text_min_ts = entry.text_min_ts.max(compact_ts);
            let stale: Vec<u64> = entry
                .text_built
                .iter()
                .filter(|(_, (_, built_ts))| *built_ts < entry.text_min_ts)
                .map(|(index_id, _)| *index_id)
                .collect();
            for index_id in stale {
                entry.text.remove(&index_id);
                entry.text_built.remove(&index_id);
            }
        }
        Ok(())
    }

    /// `WalCmdCompactV2` for a merge of `sources` into `next_seg`
    /// (`new_txn_data_impl.cpp:753-786`).
    pub(crate) fn compact_command(
        &self,
        table: &str,
        next_seg: u32,
        rows: usize,
        sources: &[u32],
    ) -> Result<storage::WalCmd> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let (db_name, table_name) = split_table_name(table);
        let mut indexes: Vec<(u64, String)> = self
            .storage
            .catalog
            .list_indexes()?
            .iter()
            .filter(|im| im.table == table || im.table == self.qualified_table(table))
            .map(|im| (im.id, im.name.clone()))
            .collect();
        indexes.sort();
        Ok(storage::WalCmd::Compact {
            db_name,
            db_id: self.database_id_of(table).to_string(),
            table_name,
            table_id: meta.id.to_string(),
            index_names: indexes.iter().map(|(_, name)| name.clone()).collect(),
            index_ids: indexes.iter().map(|(id, _)| id.to_string()).collect(),
            // `CompactTxnStore::ToWalEntry` reads `segment_infos_`, the copy
            // taken *before* the per-block/per-segment counts were filled in
            // (`new_txn_data_impl.cpp:758` vs `:766`), so the reference's
            // command carries 0 there. Replay ignores them (it commits the
            // segment and its block ids), so the port writes 0 too.
            new_segment_infos: vec![self.wal_segment_info(
                next_seg,
                rows,
                meta.columns.len(),
                false,
            )],
            deprecated_segment_ids: sources.to_vec(),
        })
    }

    /// Writes (or refreshes) the chunk of every spann index.
    ///
    /// `index_delta_mode = rebuild`: the chunk always describes the whole column
    /// at `ts`, so a checkpoint can produce it without a tail and without
    /// knowing which rows changed. A failure is not fatal to the checkpoint —
    /// the index then answers from its previous chunk, and a query whose rows it
    /// does not cover falls back to the exact scan.
    pub(crate) async fn publish_spann_chunks(&self, ts: u64) {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("SPANN") {
                continue;
            }
            let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&im.table) else {
                continue;
            };
            let _ = self
                .publish_index_chunks(&im.table, meta.id, Some((im.name.as_str(), ts)))
                .await;
        }
    }

    /// Publishes the PLAID mirror of every multi-vector index this process has
    /// built, so the chunks on disk cover the rows the checkpoint just wrote.
    ///
    /// PLAID is **rebuild-mode**: its chunk is a snapshot of the column, and
    /// before this only a `DumpMemIndex`-style transaction wrote one. A table
    /// loaded by an import therefore left chunks covering the rows of the last
    /// dump, and the first query after the import rebuilt the whole index - a
    /// full row scan plus centroid training over every token vector, on the read
    /// path. Measured on a 62 k-unit table: past the 120 s RPC timeout, so the
    /// query failed, and the client that timed out then desynchronised
    /// (`Select: out of order sequence response` for every later call).
    ///
    /// The checkpoint is where this belongs, for the same reason SPANN uses it
    /// (§9.4): the blocks are final, the write path is already the expensive
    /// one, and the query path then only reads. `write_plaid_index` appends one
    /// chunk per dump and shares the first chunk's centroids, so a checkpoint
    /// that appended little writes little.
    ///
    /// Best effort: a column whose index was never built in this process has
    /// nothing to publish, and a failure here leaves the previous chunks in
    /// place - `read_plaid_index` compares their document count with the rows it
    /// can read and falls back to a rebuild when they disagree.
    pub(crate) async fn publish_plaid_chunks(&self) {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("PLAID") {
                continue;
            }
            let Ok(index_id) = self.index_position(&im.table, &im.name) else {
                continue;
            };
            let Some(shared) = self.cached_multivector_index(&im.table, index_id as u64) else {
                continue;
            };
            // A tail below the chunk target waits for the dump that creates the
            // chunk and folds it (`fold_dumped_mirror`): publishing a stub here
            // would leave a tiny chunk nothing folds, and inflate the chain the
            // reader has to merge on every load.
            let target = self.plaid_chunk_target_rows();
            {
                let Ok(index) = shared.read() else {
                    continue;
                };
                // A mirror written before the row list existed can never be
                // loaded again once a row is deleted (see
                // `write_plaid_index`), so an index that has been built has to
                // be published once even with an empty tail. Only a built index:
                // a checkpoint on a table nothing has queried must not start a
                // full build here.
                let stale_layout =
                    index.plaid.is_some() && self.plaid_chunk_rows(&im.table, &im.name).is_none();
                if index.tail.len() < target && !stale_layout {
                    continue;
                }
            }
            {
                let Ok(mut index) = shared.write() else {
                    continue;
                };
                // `PlaidIndexInMem::BuildIndex` (or its incremental form): the
                // buffered rows are encoded against the centroids the index
                // already holds and the buffer is released. There is nothing to
                // `seal` - a PLAID column's chunk is the index, written by
                // `write_plaid_index` from the rows below.
                index.build_plaid();
            }
            if let Err(e) = self.write_plaid_index(&im.table, &im.name).await {
                eprintln!("plaid publish {}: {e}", im.name);
            }
        }
    }

    /// SPANN's optimize pass (§18, P5).
    ///
    /// The index is rebuild-mode, so "merge" is "dump once and keep one chunk":
    ///
    /// 1. **uncovered rows first** - a segment whose newest chunk no longer
    ///    matches its live rows is dumped, which is also what clears that
    ///    segment's rows from the mem index;
    /// 2. then fold - a segment left with more than one live chunk keeps only the
    ///    newest. A rebuild-mode chunk covers the *whole* segment, so every older
    ///    one is strictly subsumed; `max_open_chunks_per_index` decides when a
    ///    pass is worth running when nothing needs a dump (a crash between the
    ///    commit and the deprecation leaves extra chunks behind).
    ///
    /// It runs after [`Self::optimize_index_chunks`], not inside that
    /// transaction: a spann dump publishes its chunk records through the catalog
    /// itself, and folding it into the same `DumpIndex` command would write the
    /// record twice.
    pub(crate) async fn optimize_spann_indexes(&self, ts: u64) -> Result<()> {
        let max_open = self.max_open_chunks_per_index();
        for im in self.storage.catalog.list_indexes()? {
            if !im.index_type.eq_ignore_ascii_case("SPANN") {
                continue;
            }
            let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&im.table) else {
                continue;
            };
            let Ok(index_id) = self.index_position(&im.table, &im.name) else {
                continue;
            };
            let db_id = self.database_id_of(&im.table);
            let recorded: std::collections::HashMap<(u32, u64), u64> =
                catalog::layout::read_index_chunks(
                    self.storage.catalog.store().as_ref(),
                    &db_id.to_string(),
                    &meta.id.to_string(),
                    &index_id.to_string(),
                )
                .unwrap_or_default()
                .into_iter()
                .flat_map(|(segment, chunks)| {
                    chunks
                        .into_iter()
                        .map(move |(id, info)| ((segment, id), info.row_count))
                })
                .collect();
            let segments = self.segment_row_counts(&im.table);
            // Rows the chains still hold but the table no longer has: a
            // `DELETE` leaves tombstones and a chunk keeps the member, so the
            // reclaimable garbage is `chain - live`, read off the row counts
            // rather than by scanning the delete journal (which is cumulative,
            // and would make every pass rebuild again).
            let live_rows = self.row_count(&im.table).unwrap_or(0) as u64;
            let mut covered_total = 0u64;
            let mut needs_dump = false;
            let mut over_cap = false;
            for (segment, rows) in &segments {
                let Some(dir) = self.spann_chunk_dir(&im.table, &im.name, *segment) else {
                    continue;
                };
                let live = self.live_chunk_files(&im.table, &im.name, *segment, &dir);
                if live.len() > max_open {
                    over_cap = true;
                }
                let covered = live
                    .iter()
                    .map(|(id, _)| *id)
                    .max()
                    .and_then(|id| recorded.get(&(*segment, id)).copied())
                    .unwrap_or(0);
                covered_total += covered;
                if covered != *rows {
                    needs_dump = true;
                }
            }
            // The dead-row test is table-wide: `segment_row_counts` is a high
            // water mark (a delete does not lower it), so per-segment attribution
            // is not available without a scan. One segment's garbage is enough
            // to make a dump worth it, and a rebuild recomputes every segment.
            let dead = covered_total.saturating_sub(live_rows);
            let dead_share = live_rows > 0
                && dead.saturating_mul(100)
                    >= live_rows.saturating_mul(Self::SPANN_DEAD_REBUILD_PERCENT);
            if !needs_dump && !over_cap && !dead_share {
                continue;
            }
            if needs_dump || dead_share {
                // Forced: the deprecation loop below drops every layer but the
                // newest, which is only correct when the newest one is a full
                // snapshot. A delta here would leave the chain short of rows.
                // Failure is not fatal to the pass: the index keeps answering
                // from its previous chunk and the uncovered rows are scored
                // exactly (§9.7).
                let _ = self
                    .publish_index_chunks_forced(&im.table, meta.id, Some((im.name.as_str(), ts)))
                    .await;
            }
            // The counts as they are **after** the publication above: the chunk
            // it wrote is in the catalog now, and the snapshot test below has to
            // see it - the table read before the publication cannot.
            let published_rows: std::collections::HashMap<(u32, u64), u64> =
                catalog::layout::read_index_chunks(
                    self.storage.catalog.store().as_ref(),
                    &db_id.to_string(),
                    &meta.id.to_string(),
                    &index_id.to_string(),
                )
                .unwrap_or_default()
                .into_iter()
                .flat_map(|(segment, chunks)| {
                    chunks
                        .into_iter()
                        .map(move |(id, info)| ((segment, id), info.row_count))
                })
                .collect();
            for (segment, rows) in &segments {
                let Some(dir) = self.spann_chunk_dir(&im.table, &im.name, *segment) else {
                    continue;
                };
                let live = self.live_chunk_files(&im.table, &im.name, *segment, &dir);
                let Some(newest) = live.iter().map(|(id, _)| *id).max() else {
                    continue;
                };
                // Only a newest layer that is a **full snapshot** can stand
                // alone: dropping the older layers of a chain whose newest
                // layer is a delta would delete the rows only they hold. The
                // record's row count is the test (`published_index_chunks`
                // writes a snapshot's count as the segment's row count).
                let newest_rows = published_rows
                    .get(&(*segment, newest))
                    .copied()
                    .unwrap_or(0);
                if newest_rows != *rows {
                    continue;
                }
                let old: Vec<u32> = live
                    .iter()
                    .filter(|(id, _)| *id != newest)
                    .map(|(id, _)| *id as u32)
                    .collect();
                if !old.is_empty() {
                    self.deprecate_chunks(&im.table, &im.name, *segment, &old, ts)?;
                }
            }
        }
        Ok(())
    }

    /// Drops the clean pages of every index mapping, so RSS tracks live
    /// structures rather than the page cache (C++ `BufferManager::Flush` drops
    /// the clean pages it wrote back).
    ///
    /// Through the mapping registry rather than the directory: the walk it
    /// replaced opened every file under `fulltext`/`multivector`/`sparse`/
    /// `secondary`/`secondary_low` on every checkpoint, and `fadvise` on an
    /// already-mapped file does not drop the pages the mapping holds.
    pub(crate) fn drop_index_caches(&self) {
        storage::mapped::release_mapped_pages(usize::MAX);
    }

    /// The checkpoint itself, without the mode gate: [`Self::cleanup`] runs it
    /// on a read-only replica too, where the reference's cleanup task is
    /// allowed (`background_process_impl.cpp:109`).
    /// Bytes of appended-but-unsealed rows the writer may hold.
    pub(crate) fn pending_budget(&self) -> usize {
        self.context
            .pending_budget
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// `[storage]`-style knob for [`Self::pending_budget`].
    pub fn set_pending_budget(&self, bytes: usize) {
        self.context
            .pending_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// Checkpoints when the table's append buffer passes its budget.
    ///
    /// `TableEntry::pending_rows` holds the parsed values of every row a commit
    /// appended since the last checkpoint - it is the only copy the writer keeps,
    /// and nothing bounds it: the reference flushes it from a *timer*, so a bulk
    /// import that outruns the timer holds the table (measured: 400k x 1536d,
    /// 8.6 GiB of RSS for a 2.3 GiB column, no checkpoint in between). The write
    /// path is the only place that knows it is outrunning the timer, so it seals
    /// by itself here.
    ///
    /// Called **after** the commit returned, with no bottom gate held: a
    /// checkpoint takes `flush_lock` and then waits for that gate, which is the
    /// cycle `dump_secondary_over_capacity_blocking` documents.
    pub(crate) async fn seal_pending_over_budget(&self, table: &str) {
        let budget = self.pending_budget();
        if budget == 0 {
            return;
        }
        let pending = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&self.table_key(table))
                .map(|entry| entry.pending_bytes)
                .unwrap_or(0)
        };
        if pending <= budget {
            return;
        }
        // Best effort: the statement already committed, so a failure cannot be
        // reported as its error - the rows stay buffered and the next write or
        // the periodic checkpoint retries.
        if let Err(err) = self.flush_storage().await {
            eprintln!("checkpoint after the append buffer passed its budget: {err}");
        }
    }

    pub(crate) async fn flush_storage(&self) -> Result<()> {
        // One checkpoint at a time: the block mirror is rewritten in place.
        let _serial = self.storage.flush_lock.write().await;
        // `NewTxn::Checkpoint`: the blocks are written at this transaction's
        // `begin_ts`, so only rows of transactions that already finished reach
        // the files. The frame's payload is what the store flush covers, which
        // is this same bound plus anything that committed while the mirror ran
        // (see the frame at the end of this function).
        //
        // Named `bound_ts` at the block writer: it is the **visibility bound**
        // of the mirror, not this transaction's own ts (which a checkpoint does
        // not have - it writes rows other transactions committed).
        let bound_ts = self.checkpoint_ts();
        // The checkpoint **does not** dump the memory indexes: C++
        // `NewTxn::CheckpointTable` only flushes blocks, and a memory index
        // keeps its rows until a `DumpMemIndex` transaction seals them. The
        // tail is therefore still in memory after a checkpoint and is
        // reconstructed from the chunk gap on the next open
        // (`recover_mem_index`).
        //
        // The memory indexes still need a bound on a long import; that is the
        // quota (the reclaim round, `reclaim_unsealed`) and the per-batch
        // capacity dumps, not this function.
        // `INFINITY_PROFILE_OPEN=1` also times the checkpoint's phases: a
        // checkpoint that crosses a client's RPC timeout is invisible otherwise,
        // and it is the write path's one long step.
        let profile = std::env::var_os("INFINITY_PROFILE_OPEN").is_some();
        let mut phase = std::time::Instant::now();
        let ckpt_start = phase;
        let mut mark = |name: &str| {
            if profile {
                eprintln!("checkpoint {name}: {:?}", phase.elapsed());
            }
            phase = std::time::Instant::now();
        };
        // The C++ block mirror is the storage of record; a table it cannot
        // write (a column with no block layout) keeps its rows buffered only.
        let mirror = self.flush_blocks(bound_ts).await;
        mark("flush_blocks");
        // Every table's rows are in a block for this bound, so the log below it
        // is reclaimable - `CLEANUP` prunes by the frame alone otherwise, and
        // the rows a previous crash left in the log would go with it. A table
        // the mirror could not encode keeps its rows buffered, so a checkpoint
        // that skipped one does not move this floor.
        if mirror.as_ref().copied().unwrap_or(false) {
            self.mirrored_ts
                .store(bound_ts, std::sync::atomic::Ordering::Relaxed);
        }
        // `EngineOptions::object_dir`: publish this checkpoint's data files as
        // the reference does (`pm|object|` + packed objects).
        if let Some(pm) = self.persistence.as_ref() {
            // `PersistenceManager` holds a `MetaStore` trait object that is not
            // `Send`, so it cannot cross to the blocking pool. It runs inline;
            // a checkpoint is already the one call that owns `flush_lock`, and
            // its cost is the file sweep `flush_blocks` above.
            if let Err(e) = pm.persist_directory() {
                eprintln!("object dir: persisting failed: {e}");
            }
        }
        // spann is a **rebuild-mode** index (`docs/SPANN_DESIGN.md` §9.4): its
        // chunk is a snapshot of the whole column, so the checkpoint — after the
        // blocks are final — is the natural place to refresh it. There is no
        // memory tail to seal, which is why it does not wait for a
        // `DumpMemIndex`.
        self.publish_spann_chunks(bound_ts).await;
        // PLAID is rebuild-mode as well, and its snapshot was the one nothing
        // refreshed: the next process paid for it on its first query.
        self.publish_plaid_chunks().await;
        mark("spann+plaid");
        // The mirror writes the vector/IVF chunks, so reload after it.
        self.reload_vector_indexes();
        mark("reload_vector_indexes");
        // Bound the page cache the way the reference's buffer manager does:
        // segment files are written back and their clean pages dropped, so RSS
        // tracks live structures rather than file cache.
        self.drop_index_caches();
        // With `storage_type = "minio"` the C++-format data files are mirrored
        // into the bucket; the catalog and the row store follow at the end of
        // the checkpoint, once they are flushed.
        // Cloned out in a block: the `std::sync::MutexGuard` from `lock()` lives
        // to the end of its statement, and this statement spans an await.
        let client = {
            let guard = self.context.object_store.lock().unwrap();
            guard.clone()
        };
        if let Some(client) = client {
            let root = self.dir.join("data");
            // An S3 client is synchronous (plain TCP, optional rustls): the
            // upload is blocking I/O, so it goes to the blocking pool.
            crate::scheduler::scheduler()
                .spawn_blocking(move || {
                    let _ = storage::s3::sync_tree(&client, &root, "data");
                })
                .await;
        } // Mirror the catalog into the C++ key space / value encoding, in the
          // catalog store at `<dir>/catalog` (the path the C++ engine uses). A
          // C++-only catalog already rewrites on every DDL, so nothing is left to
          // mirror there. Best effort: a column type the C++ encoding does not
          // cover yet leaves the previous layout in place rather than failing the
          // flush.
        if !self.storage.catalog.is_catalog_only() {
            let _ = self
                .storage
                .catalog
                .rewrite_layout(self.storage.catalog.store().as_ref());
        }
        mirror?;
        mark("catalog_rewrite");
        // Make block/chunk files and their directory entries on the data
        // filesystem durable before `txn.flush()` raises `synced` (even when
        // `data/` and `wal/` live on separate filesystems).
        storage::wal::WalManager::sync_dir_fs(&self.dir.join("data")).await?;
        // The checkpoint's one store flush, at the end as in
        // `NewTxn::Checkpoint` (`kv_store()->Flush()`, `new_txn_impl.cpp:1814`).
        // It covers everything the mirror published - `catalog|seg|…`,
        // `catalog|blk|…`, `idx_seg|`, `idx_chunk|…` and the layout rewrite -
        // and it raises `synced`, the marker the WAL pruning reads. Without it
        // those records stay in the memtable and a `kill -9` discards them: the
        // reference then sees block files whose segment records do not exist.
        self.txn.flush()?;
        mark("store_flush");
        // The checkpoint's other two trees. `data/` alone is not enough for a
        // restart out of the bucket: the catalog holds the tables and the row
        // store holds their rows, so a directory restored from `data/` alone
        // has block files that belong to no table (measured: the table was
        // simply absent after the local files were wiped).
        // Same block: the guard may not cross the upload's await.
        let client = {
            let guard = self.context.object_store.lock().unwrap();
            guard.clone()
        };
        if let Some(client) = client {
            let root = self.dir.join("catalog");
            crate::scheduler::scheduler()
                .spawn_blocking(move || storage::s3::sync_tree(&client, &root, "catalog"))
                .await?;
        }
        // The frame goes last, once every file it claims is on disk: the block
        // mirror, the index chunks and the store are all written and flushed
        // above, so a power loss either keeps the frame together with its data
        // or loses it and reopens at the previous checkpoint.
        //
        // `NewTxn::Checkpoint` commits after the files are written, and the
        // frame names what the files hold. Writing the frame before the flush
        // would let a `kill -9` keep a frame claiming records the SSTs do not
        // have; naming the ts frozen at the *start* of the checkpoint would
        // under-claim - the SST is written at the end, so it also holds a
        // commit that landed while the mirror ran, and the replay would then
        // redo it (measured: a replayed `DROP COLUMN` of an already dropped
        // column aborted the open).
        self.txn.append_checkpoint_flushed().await?;
        self.txn.rotate_wal_if_large().await?;
        self.txn.sync_wal().await?;
        // The checkpoint is where the delta-maintained total is checked against
        // a full scan (plan 4.2): it also absorbs any path that changed a tail
        // without refreshing.
        self.recompute_memindex_total();
        if profile {
            eprintln!("checkpoint total: {:?}", ckpt_start.elapsed());
        }
        Ok(())
    }

    /// C++: a checkpoint with object storage — persist locally, then mirror the
    /// catalog and row store next to the C++-format `data` tree that
    /// [`Self::flush`] already uploads. No-op upload when no store is bound.
    pub async fn checkpoint_to_object_store(&self) -> Result<()> {
        self.flush().await?;
        // Same block: the guard may not cross the upload's await.
        let client = {
            let guard = self.context.object_store.lock().unwrap();
            guard.clone()
        };
        if let Some(client) = client {
            let root = self.dir.join("catalog");
            crate::scheduler::scheduler()
                .spawn_blocking(move || storage::s3::sync_tree(&client, &root, "catalog"))
                .await?;
        }
        Ok(())
    }

    /// `SHOW VARIABLE` at the given scope.
    ///
    /// C++: `Infinity::ShowVariable` (`src/main/infinity_impl.cpp:311`). The
    /// value set follows `PhysicalShow::ExecuteShowGlobalVariable` /
    /// `ExecuteShowSessionVariable`; an unknown name is `NoSysVar`, as the
    /// reference's `default:` arm reports.
    pub fn show_variable(&self, name: &str, scope: variables::SetScope) -> Result<QueryResult> {
        let key = name.to_ascii_lowercase();
        let value = match scope {
            variables::SetScope::Global => {
                let var = variables::GlobalVariable::from_name(&key)
                    .ok_or_else(|| Error::Invalid(format!("No system variable: {key}")))?;
                self.global_variable_value(var)?
            }
            variables::SetScope::Session => {
                let var = variables::SessionVariable::from_name(&key)
                    .ok_or_else(|| Error::Invalid(format!("No system variable: {key}")))?;
                self.session_variable_value(var)
                    .ok_or_else(|| Error::Invalid(format!("No system variable: {key}")))?
            }
        };
        Ok(QueryResult::values(
            vec!["value".to_string()],
            vec![vec![Value::Str(value)]],
        ))
    }

    /// Tasks queued or running on the attached `BGTaskProcessor`; `0` when the
    /// engine has none (no background work can be in flight).
    pub(crate) fn bg_task_count(&self) -> usize {
        self.bg_processor
            .lock()
            .unwrap()
            .as_ref()
            .map(|p| p.running_task_count())
            .unwrap_or(0)
    }

    /// `SHOW DATABASES`: `<database, dir, comment>`, as
    /// `PhysicalShow::ExecuteShowDatabases` (`physical_show_databases_impl.cpp`).
    pub(crate) fn show_databases(&self) -> Result<QueryResult> {
        let names = self.storage.catalog.list_databases()?;
        let rows = names
            .iter()
            .map(|name| {
                // `DatabaseInfo::db_entry_dir_` is the directory's name under
                // the data root, not its path, and it carries the database's
                // own id: `default_db` is created at startup, so it is `db_1`
                // even though `db1` sorts before it.
                vec![
                    name.clone(),
                    format!("db_{}", self.database_id(name)),
                    String::new(),
                ]
            })
            .collect();
        Ok(QueryResult::values(
            vec![
                "database".to_string(),
                "dir".to_string(),
                "comment".to_string(),
            ],
            text_rows(rows),
        ))
    }

    /// `SHOW TABLES [FROM <db>]`: the reference's eight columns
    /// (`physical_show_tables_impl.cpp`). The Rust catalog tracks neither a
    /// create timestamp nor per-table block counts, so those report `0`.
    pub(crate) fn show_tables(&self, db: Option<&str>) -> Result<QueryResult> {
        let db = db
            .map(str::to_string)
            .unwrap_or_else(|| self.current_db.lock().unwrap().clone());
        let mut rows: Vec<Vec<String>> = self
            .storage
            .catalog
            .list_tables()?
            .into_iter()
            .filter(|t| t.name.starts_with(&format!("{db}.")))
            .map(|t| {
                vec![
                    db.clone(),
                    t.name
                        .split_once('.')
                        .map(|(_, n)| n)
                        .unwrap_or(&t.name)
                        .to_string(),
                    t.id.to_string(),
                    t.columns.len().to_string(),
                    "0".to_string(),
                    "0".to_string(),
                    "0".to_string(),
                    String::new(),
                ]
            })
            .collect();
        rows.sort_by(|a, b| a[1].cmp(&b[1]));
        Ok(QueryResult::values(
            vec![
                "database".to_string(),
                "table".to_string(),
                "id".to_string(),
                "column_count".to_string(),
                "segment_count".to_string(),
                "block_count".to_string(),
                "create_ts".to_string(),
                "comment".to_string(),
            ],
            text_rows(rows),
        ))
    }

    /// Rotates and prunes the WAL without touching the KV store. Called by the
    /// periodic checkpoint loop so a slow `flush` cannot let the log grow
    /// unbounded; only fsynced records are dropped.
    ///
    /// In production the periodic trigger awaits this directly
    /// (`engine::tasks`); the `#[cfg(test)]` name below is only the spelling the
    /// tests use for one tick by hand. Both are async - a synchronous adapter
    /// could only be a `block_on`, and there is nothing left to bridge.
    #[cfg(test)]
    pub async fn checkpoint_wal(&self) {
        self.checkpoint_wal_async().await;
    }

    /// Rotates and prunes the WAL without touching the KV store.
    pub(crate) async fn checkpoint_wal_async(&self) {
        // The reference's checkpoint appends a `kNewCheckpoint` entry and then
        // drops everything up to it. The entry is what makes the directory
        // readable by the reference (`GetReplayEntries` takes
        // `max_checkpoint_ts` from it and refuses a writable directory without
        // one); truncating alone leaves an empty log, which it also rejects.
        //
        // The frame may only claim what a store flush has already made durable.
        // Nothing here writes blocks or an SST: a DDL committed since the last
        // `flush` lives in the RocksDB memtable (catalog writes carry no log)
        // and a row lives in the block buffer, so a frame that named the clock
        // would hide both from the next replay - the entries that recreate them
        // sit below the floor. Measured: `test_alter_simple` lost its table
        // after a restart that killed the process between this frame and the
        // `flush` that follows it.
        let _ = self.txn.append_checkpoint_flushed().await;
        // A checkpoint keeps its sealed segment; `Cleanup` reclaims it, as in
        // `WalManager::Cleanup`.
        let _ = self.txn.rotate_wal_if_large().await;
        // The frame claims only records a flush covered, so it may be synced on
        // its own.
        let _ = self.txn.sync_wal().await;
    }

    /// Runs the reference's optimize pass over the index mirrors: a mirror
    /// whose newest chunk does not cover every live row gets a merged chunk.
    ///
    /// Test-only name for [`Self::optimize_indexes_async`]: production submits
    /// the work to the background processor, which awaits that one.
    #[cfg(test)]
    pub async fn optimize_indexes(&self) -> Result<()> {
        self.optimize_indexes_async().await
    }

    /// The async body of [`Self::optimize_indexes`].
    pub async fn optimize_indexes_async(&self) -> Result<()> {
        // The optimize task only runs on a writable node
        // (`optimization_process_impl.cpp:174-185`).
        self.ensure_writable()?;
        // An optimize rewrites the index chunks; a dump that was staged before
        // it no longer matches the index state.
        self.index_epoch
            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        self.optimize_index_chunks().await?;
        // SPANN is rebuild-mode: its optimize is a dump plus a fold, and until
        // this call it had no branch at all - the doc's "OPTIMIZE INDEX does
        // nothing for spann" was literally true.
        let ts = self.checkpoint_ts();
        self.optimize_spann_indexes(ts).await
    }
}

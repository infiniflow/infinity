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

/// One document's tokenised terms and the byte cost of its postings, as produced
/// by [`prepare_doc`] before the table's write gate is taken.
pub(crate) type PreparedTerms = (Vec<(String, u32, u16)>, usize);

/// Full-text terms of one batch, tokenised before the table's gate is taken.
///
/// Keyed by column and by position in the batch (`base + position` is the row
/// id, which the commit only hands out later). The terms are **taken** when the
/// batch is inserted, so the type is built once and dropped.
pub(crate) struct PreparedText {
    /// The row id of each position in the batch, as the commit allocated them.
    ///
    /// A list, not `base + position`: an allocation that crossed a segment
    /// boundary has two ranges and the ids jump at the seam.
    seqs: Vec<usize>,
    columns: Vec<(String, Vec<Option<PreparedTerms>>)>,
}

impl PreparedText {
    /// Points the batch at the row ids the commit allocated.
    pub(crate) fn set_seqs(&mut self, seqs: Vec<usize>) {
        self.seqs = seqs;
    }

    /// The terms for `seq`, taken out of the batch. `None` when the position is
    /// outside the batch or another pass already took it.
    fn take(&mut self, column: &str, seq: usize) -> Option<PreparedTerms> {
        let position = self.seqs.iter().position(|held| *held == seq)?;
        let (_, rows) = self.columns.iter_mut().find(|(name, _)| name == column)?;
        rows.get_mut(position)?.take()
    }
}

/// One run's claim on the text index: taken before its documents are tokenised,
/// released once they are published into the tail.
///
/// The seal picks the rows it writes from the tail, and a chunk's `.len` covers
/// every row of its span. Without this claim, a writer that publishes row 9000
/// and seals while row 8000 is still being tokenised would write 8000 with
/// length 0, and the chunk that later publishes 8000 would start before the end
/// of that one - two chunks over one row.
///
/// `Drop` releases the claim, so a panic in the write path cannot leave the
/// barrier stuck forever.
/// Holds a "this run's rows are text-pending" mark for as long as it lives.
///
/// `try_write`, not `write().await`: `Drop` is synchronous, and the mark it
/// releases is best-effort bookkeeping (a seal that misses it under
/// contention still sees the rows, just not the early warning). An async
/// `Drop` does not exist, so the ticket cannot await.
struct PublishTicket<'a> {
    index: &'a tokio::sync::RwLock<crate::engine::text_index::TextIndex>,
    from_row: u64,
}

impl<'a> PublishTicket<'a> {
    fn announce(
        index: &'a tokio::sync::RwLock<crate::engine::text_index::TextIndex>,
        from_row: u64,
    ) -> Self {
        if let Ok(mut guard) = index.try_write() {
            guard.announce_publish(from_row);
        }
        Self { index, from_row }
    }
}

impl Drop for PublishTicket<'_> {
    fn drop(&mut self) {
        if let Ok(mut guard) = self.index.try_write() {
            guard.finish_publish(self.from_row);
        }
    }
}

impl Engine {
    /// Writes one `keys + offsets + PGM` chunk per `ROW_ID` segment and installs
    /// the readers (the reference's high-cardinality `SaveIndexInner`).
    pub(crate) fn write_secondary_pgm_chunks(
        &self,
        table: &str,
        index_id: u64,
        kind: storage::secondary_segment::KeyKind,
        by_segment: HashMap<u64, Vec<(storage::secondary_segment::Key, u32)>>,
    ) {
        use storage::secondary_segment::write_secondary_segment_typed;
        // A rebuild writes straight into the segment's own directory under the
        // id the catalog hands out, so the records can be read back from there
        // (`GetChunkIDs1`) instead of scanning a spill set. The id is the
        // caller's: it is the record being rebuilt, not "the column's index".
        let set = self.index_segment_set(table);
        // One chunk per **(index, segment)**, not per block: the reference's
        // `PopulateSecondaryIndexInner` feeds every block of the segment into
        // one `SecondaryIndexInMemT` (`InsertBlockData`) and then writes exactly
        // one chunk from it (`GetChunkIndexMetaInfo` + `AddNewChunkIndex1`;
        // `new_chunk_ids` receives a single id). Splitting by block multiplies
        // the chunk count by the block count and moves every `base_row_id`: a
        // reader still answers the same because the offsets are
        // segment-relative (`TrunkReaderT::OutPut`,
        // `index_filter_evaluators_impl.cpp:912`), but the persisted catalog no
        // longer matches the reference.
        let mut readers = Vec::new();
        let mut meta: Vec<(u64, usize)> = Vec::new();
        for (segment, mut entries) in by_segment {
            // `Key::cmp` is the chunk's own order: u64 for integers and hashed
            // text, the float order for a FLOAT/DOUBLE column.
            entries.sort_by(|a, b| a.0.order(&b.0));
            let next = self.next_index_chunk_id(&set, &index_id.to_string(), segment as u32);
            let seg_dir = self.index_segment_dir(table, index_id, segment as u32);
            if std::fs::create_dir_all(&seg_dir).is_err() {
                continue;
            }
            let path = seg_dir.join(format!("chunk_{next}.idx"));
            if write_secondary_segment_typed(&path, kind, &entries).is_err() {
                continue;
            }
            // `ChunkIndexMetaInfo::row_count_` counts **data rows**, not
            // entries (`SecondaryIndexInMemT::GetRowCount`): a JSON row
            // contributes one entry per flattened term but one row.
            let row_count = {
                let mut offsets: Vec<u32> = entries.iter().map(|(_, offset)| *offset).collect();
                offsets.sort_unstable();
                offsets.dedup();
                offsets.len()
            };
            // `ChunkIndexMetaInfo::base_row_id_` names the chunk's **first
            // row**: a segment dumped twice has two chunks, and the second
            // starts where the first ended rather than at the segment's base.
            let first = entries.iter().map(|(_, offset)| *offset).min().unwrap_or(0);
            let record = catalog::layout::IndexChunk {
                chunk_id: next,
                base_name: String::new(),
                base_row_id: (segment << 32) | u64::from(first),
                row_count: row_count as u64,
                term_count: 0,
                index_size: 0,
                commit_ts: chunk_commit_ts(self),
            };
            let index = catalog::layout::IndexSegment {
                index_id,
                segment_id: segment as u32,
                chunks: vec![record.clone()],
            };
            let _ = self
                .storage
                .catalog
                .append_index_chunk(&set, &index, &record);
            if let Ok(reader) =
                storage::secondary_segment::SecondarySegmentReader::open_with_kind(&path, kind)
            {
                // The chunk's offsets are already segment-relative, so the base
                // is the segment alone; its `ChunkIndexMetaInfo` covers every
                // row the segment holds.
                meta.push((segment << 32, row_count));
                readers.push((segment << 32, Arc::new(reader)));
            }
        }
        let tkey = self.table_key(table);
        // The readers are the **catalog's** chunk list, not this call's: a
        // build restricted to one segment (`build_index_of_segment`, which is
        // what an import does) writes one record, and replacing the list with
        // just that one dropped every other segment's chunk from the in-memory
        // reader set - a query then answered for the imported rows only.
        let readers: Vec<(u64, Arc<storage::secondary_segment::SecondarySegmentReader>)> = self
            .catalog_index_files(table, index_id)
            .into_iter()
            .filter_map(|(segment, _, path)| {
                let reader =
                    storage::secondary_segment::SecondarySegmentReader::open_with_kind(&path, kind)
                        .ok()?;
                Some(((segment as u64) << 32, Arc::new(reader)))
            })
            .collect();
        let meta: Vec<(u64, usize)> = readers
            .iter()
            .map(|(base, reader)| (*base, reader.row_count()))
            .collect();
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.disk.insert(index_id, readers);
        entry.secondary.chunk_meta.insert(index_id, meta);
    }

    /// Appends an already-decoded batch to the row cache, if the table has one.
    ///
    /// `rows` is what `publish_encoded_rows` just parsed: the cache used to
    /// parse the same stored text a second time, which doubled the parse for
    /// every insert on a scalar table.
    pub(crate) fn note_row_cache(&self, table: &str, rows: &[(usize, Vec<Value>)]) {
        if rows.is_empty() {
            return;
        }
        let key = self.table_key(table);
        {
            let tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get(&key) else {
                return;
            };
            if entry.row_cache.is_none() {
                return;
            }
        }
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&key) else {
            return;
        };
        let Some(cache) = entry.row_cache.as_mut() else {
            return;
        };
        // The cache is a `Vec` readers binary-search by row id, so it only takes
        // a batch that starts after everything it already holds. Two commits
        // whose bottoms finished out of order would otherwise leave it unsorted;
        // dropping it costs one cache miss, not a wrong answer.
        let first = rows[0].0;
        if cache.rows.last().is_some_and(|(last, _)| *last >= first) {
            entry.row_cache = None;
            return;
        }
        for (seq, values) in rows {
            cache.push(*seq, values.clone());
        }
        if cache.bytes > ROW_CACHE_BUDGET_BYTES {
            entry.row_cache = None;
        }
    }

    /// Applies `delta` to the cached live-row count of `table`.
    pub(crate) fn note_live_rows(&self, table: &str, delta: isize) {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        if let Some(live) = entry.live_rows {
            entry.live_rows = Some(if delta >= 0 {
                live.saturating_add(delta as usize)
            } else {
                live.saturating_sub(delta.unsigned_abs())
            });
        }
    }

    /// Deletes every `row:`/`del:` key of `name` in one batch.
    ///
    /// Two callers need it. `drop_table_all` sweeps after the catalog delete,
    /// so a crashed drop leaves keys no catalog entry refers to; and
    /// [`Self::create_table_clearing_orphans`] repeats the sweep when a name is
    /// created again, because `fetch_rows` walks `row:{table}:` and
    /// `next_row_seq` starts past the highest orphan id — without this a new
    /// table would read a dropped table's rows back. The reference cannot hit
    /// either window: its drop and its segment removal share one transaction.
    pub(crate) fn clear_orphan_rows(&self, name: &str) -> Result<()> {
        // Both spellings: `DROP TABLE t` and a qualified `db.t` write the same
        // rows under whichever name reached the executor, so a sweep keyed on
        // only one of them can miss half the prefixes.
        // Nothing to sweep: the rows and every journal live in the block
        // files, which the drop removes with the table's directory.
        let _ = name;
        Ok(())
    }

    pub(crate) fn write_id_log(&self, key: &[u8], ids: &[u64]) -> Result<()> {
        let mut bytes = Vec::with_capacity(ids.len() * 8);
        for id in ids {
            bytes.extend_from_slice(&id.to_le_bytes());
        }
        self.storage.catalog.store().put(key, &bytes)
    }

    /// The cumulative row count after every append transaction.
    ///
    /// The reference's block `version` records one `create` per append
    /// (`BlockVersion::Append`), so the mirror needs the same boundaries to
    /// write the same file. Only the boundaries are kept - the reference
    /// stamps each one with its commit ts and the port writes its mirror ts,
    /// which the format test takes out before comparing.
    pub(crate) fn append_totals(&self, table: &str) -> Vec<u64> {
        self.ensure_journals(table).appends
    }

    /// Records an append of `n` rows, so the next block mirror writes a
    /// `create` for it, and returns the record's index in the append log.
    ///
    /// `ts` is the transaction's commit timestamp when it is already known -
    /// which it is for every statement that allocates inside its commit. The
    /// record is then settled immediately and cannot be mistaken for an append
    /// that is still in flight: what a checkpoint writes is "the records
    /// stamped at or before its bound", and a record with no stamp yet belongs
    /// to a transaction that has not finished (`NewTxn::Checkpoint` writes the
    /// rows whose `commit_ts <= begin_ts`, `block_meta_impl.cpp:250-269`).
    pub(crate) fn note_append(&self, table: &str, n: usize, copy: bool, ts: Option<u64>) -> usize {
        if n == 0 {
            return usize::MAX;
        }
        let mut totals = self.append_totals(table);
        let next = totals.last().copied().unwrap_or(0) + n as u64;
        totals.push(next);
        let index = totals.len() - 1;
        let tkey = self.table_key(table);
        // Read before the entry is locked: the loader takes the same lock. The
        // timestamp log stays **aligned** with the totals: a record that has no
        // stamp yet keeps a zero, so a later settle writes its own slot instead
        // of padding the ones before it.
        let settled = {
            let mut log = self.append_ts(table);
            log.resize(index, 0);
            log.push(ts.unwrap_or(0));
            log
        };
        {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.append_totals = Some(totals.clone());
            entry.append_ts = Some(settled.clone());
            entry.note_write();
        }
        // A hint for the mirror, not committed state: if the process dies
        // before the rows do, the mirror clips every total to the rows it
        // actually has and falls back to one create when they disagree.
        let mut kinds = self.append_copy(table);
        kinds.resize(totals.len(), 0);
        if let Some(last) = kinds.last_mut() {
            *last = u64::from(copy);
        }
        {
            let mut tables = self.tables.lock().unwrap();
            tables.entry(tkey.clone()).or_default().append_copy = Some(kinds.clone());
        }
        index
    }

    /// Per-append provenance, aligned with [`Self::append_totals`].
    pub(crate) fn append_copy(&self, table: &str) -> Vec<u64> {
        self.ensure_journals(table).append_copy
    }

    /// The commit timestamp of every append, aligned with [`Self::append_totals`].
    pub(crate) fn append_ts(&self, table: &str) -> Vec<u64> {
        self.ensure_journals(table).append_ts
    }

    /// Records that `segment` (and the batches after it) were written by an
    /// import whose transaction committed at `ts`.
    pub(crate) fn note_import_ts(&self, table: &str, segment: u32, ts: u64) {
        self.note_import(table, segment, None, Some(ts));
    }

    /// Records a segment's import extent in memory.
    ///
    /// The extents live in the block `version` records (a `COPY` seals the
    /// segment it wrote, and the segment's prefilter plus its row count is the
    /// extent), so a fresh process rebuilds them in `ensure_journals`; the
    /// statement that performs the `COPY` only has to update the same cache.
    pub(crate) fn note_import(
        &self,
        table: &str,
        segment: u32,
        rows: Option<u64>,
        ts: Option<u64>,
    ) {
        let _ = self.ensure_journals(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(self.table_key(table)).or_default();
        let list = entry.imports.get_or_insert_with(Vec::new);
        match list.iter_mut().find(|(id, _, _)| *id == segment) {
            Some(slot) => {
                if let Some(rows) = rows {
                    slot.1 = rows;
                }
                if let Some(ts) = ts {
                    slot.2 = ts;
                }
            }
            None => list.push((segment, rows.unwrap_or(0), ts.unwrap_or(0))),
        }
    }

    /// The commit ts of the `COPY` that opened `segment`, if it was imported.
    pub(crate) fn import_ts(&self, table: &str, segment: u32) -> Option<u64> {
        let journals = self.ensure_journals(table);
        let _ = journals;
        let tables = self.tables.lock().unwrap();
        let entry = tables.get(&self.table_key(table))?;
        let (_, _, ts) = entry
            .imports
            .as_ref()?
            .iter()
            .find(|(id, _, _)| *id == segment)?;
        Some(*ts)
    }

    /// Records how many rows `segment` held when its `COPY` finished.
    pub(crate) fn note_import_rows(&self, table: &str, segment: u32, rows: u64) {
        self.note_import(table, segment, Some(rows), None);
    }

    /// Records deleted row ids **in memory** only.
    ///
    /// The block `version` is what makes a delete durable (`write_blocks`
    /// marks the slot at the next checkpoint), and `restore_blocks` rebuilds
    /// the log from those files, so the commit path only has to make the ids
    /// visible to the next mirror. Writing the log here would mean a KV write
    /// inside the commit's own bottom half.
    /// Returns the index of the first id it appended, so the caller can stamp
    /// exactly its own entries (`settle_delete_at`) even when another statement
    /// deleted rows in the meantime.
    pub(crate) fn note_deleted_ids(&self, table: &str, ids: &[usize]) -> usize {
        if ids.is_empty() {
            return usize::MAX;
        }
        let tkey = self.table_key(table);
        let mut log = self.deleted_ids(table);
        let first = log.len();
        log.extend(ids.iter().map(|s| *s as u64));
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.deleted_ids = Some(log);
        // Keep the lazily built bitmap in step with the journal it mirrors, or
        // the rows this commit just deleted would read as live until the next
        // journal rebuild.
        if let Some(set) = entry.deleted_set.as_mut() {
            Arc::make_mut(set).extend(ids.iter().map(|s| *s as u64));
        }
        // A deleted row leaves the spann mem index with it (`docs/SPANN_DESIGN.md`
        // §9.2). This is not only about the wasted decode: the mem list is half of
        // the coverage count (`index.len() + mem.len()`), so a tombstone left in it
        // makes the index look like it holds rows it does not, `all_covered` then
        // answers "incomplete", and every query pays the full row-set walk that the
        // count check exists to avoid. A rollback has nothing to undo here - the
        // rows it drops were never in the tree - and an `UPDATE` is a delete plus an
        // insert, so both halves go through this hook.
        if !entry.spann_mem.is_empty() {
            let gone: std::collections::HashSet<u64> = ids.iter().map(|s| *s as u64).collect();
            for tail in entry.spann_mem.values() {
                tail.forget(&gone);
            }
        }
        entry.note_write();
        drop(tables);
        first
    }

    /// The commit timestamp of every delete, aligned with [`Self::deleted_ids`].
    pub(crate) fn delete_ts(&self, table: &str) -> Vec<u64> {
        self.load_deletes(table).1
    }

    /// The write gate of `table` (see [`Self::batch_locks`]).
    ///
    /// **Lock order.** A writer takes `batch_gate(table)` → `tables` → the
    /// column's index lock. A query takes the index's *read* guard first and
    /// then `tables` (it fetches its hit rows while that guard is alive, see
    /// `text_index_for`), i.e. the opposite pair. The two cannot cycle because
    /// **no site acquires an index lock while it holds `tables`**: every one
    /// clones the `Arc` out of the map and drops the guard before locking the
    /// index. Keep that invariant when touching any `tables.lock()` block;
    /// `concurrent_insert_query_seal_and_optimize_do_not_deadlock` fails instead
    /// of hanging when it is broken.
    pub(crate) fn batch_gate(&self, table: &str) -> Arc<tokio::sync::Mutex<()>> {
        let key = self.table_key(table);
        let mut gates = self.batch_locks.lock().unwrap();
        Arc::clone(gates.entry(key).or_default())
    }

    /// `INSERT INTO t [(cols)] SELECT …`: evaluates the query through the
    /// normal SELECT path, maps its columns onto the target schema (by the
    /// explicit list when present, otherwise by position), casts each value to
    /// the target column's type, and appends the rows.
    pub(crate) async fn insert_from_select(
        &self,
        table: &str,
        columns: Option<&[String]>,
        query: &Statement,
    ) -> Result<QueryResult> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let query = query.clone();
        let (source_columns, block) = self.run_select(&query).await?;
        let rows = block.rows();
        // The reference only ever got as far as constants: `PhysicalInsert`
        // sends each projected expression through
        // `ExpressionState::CreateState`, which has no case for a column
        // reference, so an `INSERT … SELECT` that reads one dies in
        // `UnrecoverableException` (`expression_state_impl.cpp:68`) — after
        // the binder, so a bad column name still reports its bind error.
        // The port refuses the same queries instead of answering them.
        if let Some(column) = insert_select_column(&query, &source_columns) {
            return Err(Error::Unrecoverable(format!(
                "Unknown expression type: Column. expression name: {column}"
            )));
        }
        let target_indices: Vec<usize> = match columns {
            Some(cols) => {
                let mut indices = Vec::with_capacity(cols.len());
                for c in cols {
                    let idx = meta.columns.iter().position(|s| s == c).ok_or_else(|| {
                        Error::Invalid(format!(
                            "syntax: column '{c}' does not exist in table '{table}'"
                        ))
                    })?;
                    indices.push(idx);
                }
                indices
            }
            None => (0..meta.columns.len()).collect(),
        };
        if source_columns.len() != target_indices.len() {
            return Err(Error::Invalid(format!(
                "INSERT has {} target column(s) but SELECT returns {} column(s)",
                target_indices.len(),
                source_columns.len()
            )));
        }
        let mut literals = Vec::with_capacity(rows.len());
        for row in rows {
            let mut out: Vec<Literal> = (0..meta.columns.len())
                .map(|i| {
                    meta.defaults
                        .get(i)
                        .and_then(|d| d.as_deref())
                        .map(default_literal)
                        .unwrap_or(Literal::Null)
                })
                .collect();
            for (src, value) in row.iter().enumerate() {
                let target = target_indices[src];
                let ty = meta.types.get(target).map(String::as_str).unwrap_or("");
                out[target] = literal_from_value(value, ty);
            }
            literals.push(out);
        }
        self.insert_literals_inner(table, None, literals, false)
            .await
    }

    /// [`Self::insert_literals`], retried the way `execute_stmt_retrying`
    /// retries a statement.
    ///
    /// An `Append` conflicts with a `CreateIndex`/`DropIndex` on the same table
    /// (`new_txn_impl.cpp:3347`), and that conflict is retryable. The commit
    /// checks it *before* it allocates the row range, so a conflicting attempt
    /// has written nothing and the whole batch can run again - which is what
    /// this loop does. The front ends that insert directly (the Thrift `Insert`
    /// RPC and the REST `POST .../docs` handler) call this so a transient
    /// schema change does not surface as a client-visible error.
    pub async fn insert_literals_retrying(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        // The same split [`Self::insert_literals`] uses: one allocation is at
        // most `MAX_APPEND_ROWS` rows (`TableCache::PrepareAppendNolock`
        // refuses more), so a larger request is several commits. A retry stays
        // inside the batch it was made for: re-running a batch that already
        // committed would duplicate its rows.
        if rows.len() > Self::MAX_APPEND_ROWS {
            let total = rows.len();
            let mut inserted = 0usize;
            let mut left = rows;
            while !left.is_empty() {
                let take = left.len().min(Self::MAX_APPEND_ROWS);
                let batch: Vec<Vec<Literal>> = left.drain(..take).collect();
                // The recursive call is already within the limit, so it runs
                // one `insert_literals_inner` per attempt.
                let result =
                    match Box::pin(self.insert_literals_retrying(table, columns, batch)).await {
                        Ok(result) => result,
                        // A refusal after some batches committed has to say so:
                        // the committed prefix is durable, and a caller that
                        // resubmitted the whole request would duplicate it.
                        Err(error) if inserted > 0 && Self::is_admission_refusal(&error) => {
                            return Err(Error::Invalid(format!(
                                "{error}; {inserted} of {total} rows are committed - resubmit \
                                 from row {}",
                                inserted + 1
                            )));
                        }
                        Err(error) => return Err(error),
                    };
                match result {
                    QueryResult::Inserted { count } => inserted += count,
                    other => return Ok(other),
                }
            }
            return Ok(QueryResult::Inserted { count: inserted });
        }
        for _ in 0..RETRY_LIMIT {
            let before = self.txn.current_ts();
            match self
                .insert_literals_inner(table, columns, rows.clone(), false)
                .await
            {
                Err(Error::TxnConflict(_reason)) => {
                    // The retry spends another txn id, as
                    // `QueryStatementInternal` does for a re-run statement.
                    self.txn.end_statement();
                    self.txn.ensure_statement_id();
                    if self.txn.current_ts() == before {
                        std::thread::yield_now();
                    }
                    continue;
                }
                Err(Error::TxnConflictNoRetry(reason)) => {
                    return Err(Error::TxnConflictNoRetry(reason))
                }
                other => return other,
            }
        }
        Err(Error::TxnConflict(format!(
            "the insert kept conflicting for {RETRY_LIMIT} attempts"
        )))
    }

    /// Inserts literal rows into `table`. Every front end calls this directly -
    /// the Thrift `Insert` RPC and the HTTP `POST …/docs` handler included:
    /// rebuilding SQL text for a 1000-row batch and re-parsing it cost more
    /// than the insert itself, and the REST path paid it once per **row**.
    pub async fn insert_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        self.insert_in_batches(table, columns, rows, false).await
    }

    /// Commits `rows` in batches of at most [`Engine::MAX_APPEND_ROWS`], one
    /// transaction each.
    ///
    /// The reference gets this granularity for free: an `INSERT` carries a
    /// `DataBlock`, and `TableCache::PrepareAppendNolock` refuses more than
    /// `MAX_BLOCK_CAPACITY` rows. The port's primitive takes a `Vec`, and a
    /// caller (`insert_literals`, the binary import, `COPY`) can hand over far
    /// more - so the split is stated here rather than refusing the statement.
    async fn insert_in_batches(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
        new_segment: bool,
    ) -> Result<QueryResult> {
        if rows.len() <= Self::MAX_APPEND_ROWS {
            return self
                .insert_literals_inner(table, columns, rows, new_segment)
                .await;
        }
        // Validate the **whole** request before the first batch commits. The
        // batches below are separate transactions, so a row that is invalid
        // (a `NULL` in a NOT NULL column, a vector of the wrong dimension) in a
        // later batch would otherwise fail after the earlier ones were durable -
        // and a caller that retried the statement would duplicate them. The
        // reference has no such window: its whole `INSERT` is one DataBlock.
        self.validate_insert_rows(table, columns, &rows)?;
        let total = rows.len();
        let mut inserted = 0usize;
        let mut left = rows;
        while !left.is_empty() {
            let take = left.len().min(Self::MAX_APPEND_ROWS);
            let batch: Vec<Vec<Literal>> = left.drain(..take).collect();
            let result = self
                .insert_literals_inner(table, columns, batch, new_segment)
                .await;
            match result {
                Ok(QueryResult::Inserted { count }) => inserted += count,
                Ok(other) => return Ok(other),
                // The memory governor refuses per batch, so the batches before
                // this one are durable: a plain refusal here would leave the
                // caller unable to tell that from "nothing was written", and a
                // retry of the whole request would duplicate them.
                Err(error) if inserted > 0 && Self::is_admission_refusal(&error) => {
                    return Err(Error::Invalid(format!(
                        "{error}; {inserted} of {total} rows are committed - resubmit \
                         from row {}",
                        inserted + 1
                    )));
                }
                Err(error) => return Err(error),
            }
        }
        Ok(QueryResult::Inserted { count: inserted })
    }

    /// The checks [`Self::insert_literals_inner`] performs before it commits,
    /// without committing anything.
    ///
    /// It exists for the batched entry points, which split one request into
    /// several transactions ([`Self::insert_in_batches`]): validating only as
    /// each batch runs lets a later batch fail after earlier ones committed. The
    /// rules are the reference's insert checks - a known column list, a value
    /// that fits its column, no `NULL` in a NOT NULL column - so running them
    /// twice costs a pass over the rows and changes no outcome.
    fn validate_insert_rows(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: &[Vec<Literal>],
    ) -> Result<()> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        if let Some(cols) = columns {
            for c in cols {
                if !meta.columns.iter().any(|s| s == c) {
                    return Err(Error::Invalid(format!(
                        "syntax: column '{c}' does not exist in table '{table}'"
                    )));
                }
            }
        }
        let aligned = align_insert_rows(&meta.columns, &meta.defaults, columns, rows);
        for (at, row) in aligned.iter().enumerate() {
            for (i, nn) in meta.not_nulls.iter().enumerate() {
                if *nn && matches!(row.get(i), Some(Literal::Null) | None) {
                    return Err(Error::Invalid(format!(
                        "null value in NOT NULL column '{}'",
                        meta.columns.get(i).cloned().unwrap_or_default()
                    )));
                }
            }
            for (i, cell) in row.iter().enumerate() {
                if let Some(t) = meta.types.get(i) {
                    validate_insert_value(t, cell.to_slt_string().trim()).map_err(|e| match e {
                        Error::Invalid(m) => Error::Invalid(format!("row {}: {m}", at + 1)),
                        other => other,
                    })?;
                }
            }
        }
        Ok(())
    }

    /// `NewTxn::Import`: like [`Self::insert_literals`], but the batch always
    /// opens a fresh segment (`TableMeta::GetSegmentIDs1` gains a new id).
    pub async fn import_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        self.insert_in_batches(table, columns, rows, true).await
    }

    pub async fn insert_literals_inner(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
        new_segment: bool,
    ) -> Result<QueryResult> {
        // `PhysicalInsert::Execute` gates the storage mode inside the operator,
        // not in the SQL layer, so every caller of this primitive - the SQL
        // `INSERT`, the Thrift RPC, the REST handler, `COPY` - gets the gate.
        self.ensure_writable()?;
        // And the memory governor's admission, which the statement path applies
        // at the same point (`execute_statements`): this primitive is reached
        // directly by the RPC and the REST handler, so a direct insert has to be
        // refused at the same ceiling - before the row range is allocated and
        // before the WAL frame is written.
        self.admit_write()?;
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let schema = meta.columns.clone();
        // An explicit column list must name existing columns; the C++
        // engine reports this as a syntax error.
        if let Some(cols) = columns {
            for c in cols {
                if !schema.iter().any(|s| s == c) {
                    return Err(Error::Invalid(format!(
                        "syntax: column '{c}' does not exist in table '{table}'"
                    )));
                }
            }
        }
        let mut rows = align_insert_rows(&schema, &meta.defaults, columns, &rows);
        // An integer column truncates a fractional value, as the
        // reference's implicit cast does (`-6.3` -> `-6`).
        for row in rows.iter_mut() {
            for (i, cell) in row.iter_mut().enumerate() {
                let int_col = meta
                    .types
                    .get(i)
                    .map(|t| {
                        let t = t.trim().to_ascii_uppercase();
                        t.starts_with("INT")
                            || t.starts_with("TINYINT")
                            || t.starts_with("SMALLINT")
                            || t.starts_with("BIGINT")
                    })
                    .unwrap_or(false);
                if !int_col {
                    continue;
                }
                if let Literal::Float(f) = cell {
                    *cell = Literal::Int(*f as i64);
                }
            }
        }
        // enforce NOT NULL
        for row in &rows {
            for (i, nn) in meta.not_nulls.iter().enumerate() {
                if *nn && matches!(row.get(i), Some(Literal::Null) | None) {
                    return Err(Error::Invalid(format!(
                        "null value in NOT NULL column '{}'",
                        schema.get(i).cloned().unwrap_or_default()
                    )));
                }
            }
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        // The row ids are taken inside the commit below, not here: the range has
        // to be handed out in commit-ts order (`TxnAllocator::Process` ->
        // `PrepareAppend` only ever runs after `GetWriteCommitTS`).
        let types = self.column_types(table)?;
        // Render each cell's display text **once** for the whole batch. It used
        // to be rendered three times - for the check below, for the append/WAL
        // text and again for the index hooks - and on a table with a 2048-dim
        // vector that is three passes over 4096 floats per row (the profile put
        // 40% of an insert in the first of them alone).
        let cells: Vec<Vec<String>> = rows
            .iter()
            .map(|row| row.iter().map(|cell| cell.to_slt_string()).collect())
            .collect();
        // tensor-like values must match the declared dimension, and
        // sparse values must be in range
        for row in &cells {
            for (i, cell) in row.iter().enumerate() {
                if let Some(t) = types.get(i) {
                    validate_insert_value(t, cell.trim())?;
                }
            }
        }
        // A commit changes visible rows, so cached `SELECT` results must not
        // survive it. The SQL path clears on the mutating statement; callers
        // that reach this primitive directly (Thrift insert, import) would
        // otherwise keep serving a stale `count(*)`. (DML does not alter
        // schema, so `plan_cache` stays valid.)
        self.result_cache.lock().unwrap().clear();
        // The analyzer is the most expensive CPU step of a write and takes no
        // shared state, so it runs **before** the table's write gate: a second
        // writer to the same table then tokenises while the gate's holder is
        // committing and appending its block, instead of queueing behind the
        // whole statement. The batch is capped at `TEXT_BATCH_ROWS` so a big
        // `COPY` keeps the bounded shape (it tokenises a chunk at a time inside
        // the gate).
        let cells = Arc::new(cells);
        let rows = Arc::new(rows);
        let mut prepared_text = if rows.len() <= Self::TEXT_BATCH_ROWS {
            self.prepare_text_terms(table, &cells).await
        } else {
            None
        };
        // A memory index that already exceeds its quota is dumped
        // before these rows join it.
        let gate = self.batch_gate(table);
        let phase = std::time::Instant::now();
        self.note_index_insert(table, rows.len());
        // A scalar table's first batch in this process starts an empty decoded
        // row cache; the batch is appended after the commit.
        {
            let key = self.table_key(table);
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            if entry.row_cache.is_none() && entry.write_batches == 0 && scalar_only(&types) {
                entry.row_cache = Some(RowCache {
                    rows: Vec::new(),
                    bytes: 0,
                });
            }
        }
        // one transaction -> one WAL record + one KV batch (batched durability)
        let mut t = self.txn.begin();
        // `NewTxn::Append` / `NewTxn::Import`: the conflict matrix compares the
        // store types, so an append meets a schema or index change on the table.
        t.set_op(if new_segment {
            TxnOp::Import {
                table: table.to_string(),
            }
        } else {
            TxnOp::Append {
                table: table.to_string(),
            }
        });
        // Emit the reference `WalCmdAppendV2` (real typed columns) when the
        // table's types are covered; otherwise the transaction writes the
        // text-column block through `build_commands`.
        // A block that does not carry one row per row range makes an `Append`
        // command replay cannot decode (`append_rows` needs row *n*); fall back
        // to the text record in that case rather than write such a frame.
        let typed = if rows.len() >= 16 {
            let rows_bg = Arc::clone(&rows);
            let types_bg = types.clone();
            tokio::task::spawn_blocking(move || typed_append_block(&rows_bg, &types_bg))
                .await
                .unwrap_or_else(|_| typed_append_block(&rows, &types))
        } else {
            typed_append_block(&rows, &types)
        };
        // `GetWriteCommitTS` -> `SubmitForAllocation`: the range is allocated
        // inside the commit, after the conflict check, so the ids follow
        // commit-ts order and a transaction that loses the conflict never
        // spends one.
        let turn = self.txn.begin_write();
        let ts = turn.ts();
        // A `std::sync::Mutex` shared with the `before_wal` callback: it only
        // holds a `RowRange` (two integers) and is locked twice, for one
        // assignment and one read, never across an `.await`.
        let allocated = std::sync::Mutex::new(RowRange::single(0));
        let alloc_table = table.to_string();
        let alloc_table_id = meta.id.to_string();
        let alloc_rows = rows.len();
        let ts = self
            .txn
            .commit_at_deferred_with(t, ts, async |txn| {
                // Allocated **inline**: this callback runs on a runtime worker
                // that already holds the commit turn and `commit_lock`, so it
                // must not park (see `reserve_rows_at`).
                let range = self.reserve_rows_at(&alloc_table, alloc_rows, new_segment, ts)?;
                *allocated.lock().unwrap() = range;
                if let Some(block) = typed {
                    txn.set_typed_commands(vec![self.append_command(
                        &alloc_table,
                        &alloc_table_id,
                        range.ranges(),
                        block,
                    )]);
                }
                Ok(())
            })
            .await?;
        let committed = phase.elapsed();
        let bottom_started = std::time::Instant::now();
        let range = *allocated.lock().unwrap();
        let base = range.base;
        // Deferred publish: the commit becomes visible when the bottom half
        // below is done (the guard publishes, on the error path too).
        let _visible = BottomGuard::new(&self.txn, ts);
        // The bottom half: the block's `version` ts, the import extent, the
        // index entries and the block mirror. On a pooled engine this runs on
        // the table's `BottomExecutor` worker and the statement waits, as
        // `CommitBottomAppend` does.
        let bottom_table = table.to_string();
        let shard_key = bottom_table.clone();
        // The append record was created while the row ids were allocated, but
        // it carries no ts yet: stamping it there would make `committed_rows`
        // count rows that are still in flight, and a checkpoint in that window
        // sizes a block from ids that hold nothing. The reference keeps a
        // block's `created_` fields and its rows in one object; here the ts is
        // written below, right after the rows reach the buffer.
        let append_record = range.append;
        // The id of the k-th row, which is **not** `base + k` when the
        // allocation crossed a segment boundary: it has two ranges and the ids
        // jump at the seam. Everything below states a row by its position in
        // this batch, so this is the one place the seam is handled.
        let range_seq = move |k: usize| range.seq_at(k);
        let row_count = rows.len();
        let bottom_row_count = rows.len();
        let bottom_rows = rows;
        let bottom_cells = cells;
        // The gate now covers the **critical section only**: the index entries
        // and the block append of this commit. Everything above it - validation,
        // encoding, the WAL write and the row-range allocation - runs
        // concurrently, and the rows of two commits may reach the bottom in
        // either order (the block buffer keeps row order, `buffer_rows` explains
        // how). Optimize/compact still exclude this section, which is what the
        // gate guarantees for them.
        let gate_wait = std::time::Instant::now();
        let _batch = gate.lock().await;
        let gate_waited = gate_wait.elapsed();
        self.run_commit_bottom(&shard_key, move |engine: &Engine| {
            Box::pin(async move {
                // `AppendInBlock`: the rows land in the block before the index
                // entries and the commit publishes after all of it. The block
                // written at the end of this closure is the rows' only home - the
                // engine keeps no `row:` keys. The append record already carries
                // this transaction's ts (`reserve_rows_at`), so the block's
                // `version` needs nothing more here.
                if new_segment {
                    // An import seals the segment it writes, so it also has an
                    // extent (and, with it, the sealed-segment prefilter).
                    let segment = (base >> 32) as u32;
                    engine.note_import_ts(&bottom_table, segment, ts);
                    engine.note_import_rows(
                        &bottom_table,
                        segment,
                        (base as u64 & 0xffff_ffff) + bottom_row_count as u64,
                    );
                }
                // With no index of any kind in memory there is nothing to maintain;
                // skipping the per-row hook avoids a catalog lookup + string render
                // for every inserted row (the reference's insert path has the same
                // guard).
                if engine.table_has_memory_index(&bottom_table)
                    || engine.has_index_declaration(&bottom_table)
                {
                    let index_table = bottom_table.clone();
                    // The cells rendered once above are the index's input too: the
                    // full-text hook tokenises them, the multi-value/sparse/vector
                    // hooks parse their own column and the secondary hook decodes
                    // only the columns it indexes.
                    // The index work runs on **this table's** bottom worker: every
                    // index has its own lock and two tables share none, so a
                    // directory with a hundred thousand tables keeps its index
                    // updates in parallel instead of queueing behind one appender
                    // thread (the reference's `MemIndexAppender` is a single
                    // thread, which is the one structural deviation here).
                    let schema = engine.table_schema(&index_table).unwrap_or_default();
                    // One batch: the full-text terms of the whole append are
                    // tokenised without the column lock and inserted under it once.
                    let batch: Vec<(usize, &[String])> = (0..bottom_row_count)
                        .map(|k| (range_seq(k), bottom_cells[k].as_slice()))
                        .collect();
                    // A `NULL` must reach a secondary index as `Value::Null`
                    // (`value_to_tail_keys` skips it). The batch form converts the
                    // indexed column of each row only: converting the whole row
                    // copied every vector as text (23% of an insert for an index on
                    // `num`).
                    engine.update_secondary_indexes_batch_rows(
                        &index_table,
                        &bottom_rows,
                        &schema,
                        range_seq,
                    );
                    // The row ids are known now: point the pre-tokenised batch at
                    // the allocated range. A column that gained an index after the
                    // batch was tokenised is not covered and is tokenised here.
                    if let Some(prepared) = prepared_text.as_mut() {
                        prepared.set_seqs((0..bottom_row_count).map(range_seq).collect());
                    }
                    engine
                        .update_text_indexes_batch_async(
                            &index_table,
                            &batch,
                            &schema,
                            prepared_text.as_mut(),
                        )
                        .await;
                    engine.note_hnsw_insert_batch(&index_table, &batch).await;
                    let has_ivf = {
                        let tkey = engine.table_key(&index_table);
                        engine
                            .tables
                            .lock()
                            .unwrap()
                            .get(&tkey)
                            .is_some_and(|e| !e.ivf_mem.is_empty())
                    };
                    if has_ivf {
                        for (seq, texts) in &batch {
                            engine.note_ivf_insert(&index_table, *seq as u64, texts);
                        }
                    }
                    // C++ `AppendMemIndex`: a segment that reached
                    // `mem_index_capacity` rows is dumped right after the batch.
                    engine.dump_hnsw_over_capacity(&index_table).await;
                    engine.dump_secondary_over_capacity_blocking(&index_table);
                }
                // Fold this batch's index growth into the engine-wide total (plan
                // 4.2): one table's delta, not a scan of every table.
                engine.refresh_memindex_charge(&bottom_table);
                engine.bump_writes(&bottom_table);
                engine.note_live_rows(&bottom_table, bottom_row_count as isize);
                // No row key for the multi-value/tensor tables either: their WAL
                // frame is decoded back into row text by `block::to_data_block`
                // (tensor and sparse columns render their text form), so the block
                // is the only copy, as in the reference.
                // `AppendInBlock`: this batch's rows go into the block's buffer -
                // they are the batch's only copy, there is no `row:` key to read
                // them back from.
                // The decoded-row cache rides on this publish: convert directly
                // from `Literal` + rendered cells without encoding/re-parsing.
                engine.publish_literal_rows_with(
                    &bottom_table,
                    &bottom_rows,
                    &bottom_cells,
                    range_seq,
                    |table, batch| {
                        engine.note_row_cache(table, batch);
                        Ok(())
                    },
                )?;
                // The rows are readable now (`AppendInBlock`), so the record may
                // carry this transaction's ts (`BlockVersion::Append(commit_ts, …)`).
                engine.settle_append_at(&bottom_table, append_record, ts);
                Ok(())
            })
        })
        .await?;
        // Give the allocator's per-batch scratch back to the OS. Trimming on
        // every batch walks the whole heap; once every few batches is enough
        // to keep RSS flat and avoids adding milliseconds to each commit.
        self.batches_since_trim
            .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        if self
            .batches_since_trim
            .load(std::sync::atomic::Ordering::Relaxed)
            >= 8
        {
            self.batches_since_trim
                .store(0, std::sync::atomic::Ordering::Relaxed);
            storage::trim_allocator();
        }
        if std::env::var_os("INFINITY_PROFILE_INSERT").is_some() {
            eprintln!(
                "[INS] rows={row_count} gate={:?} commit={:?} bottom={:?}",
                gate_waited,
                committed,
                bottom_started.elapsed()
            );
        }
        // Only a table with no index declaration checks the quota here.
        // An indexed table already checks inside `update_text_indexes_batch`;
        // checking again after every insert would fire a second dump per batch,
        // which changes the number of chunks a mem-index dump produces - the
        // restart suite pins that count. The one-batch overshoot this leaves in
        // the append buffer is bounded, and the next write to the table closes
        // it.
        if !self.has_index_declaration(table) {
            self.enforce_quota_after_append(table);
        }
        // §9.2's row bound, not §9.5's byte watermarks: a spann mem index is the
        // query's candidate list, so one import can put a million rows into
        // every KNN query's rerank pool. This dump is a transaction of its own,
        // which is why it runs here - after the commit, with no bottom gate held
        // - and not inside the bottom half where the other index hooks fire.
        self.dump_spann_over_capacity(table).await;
        // The append buffer is the writer's only copy of the rows since the last
        // checkpoint, and the reference bounds it with a timer - which a bulk
        // import outruns. Seal it here when it passes its budget.
        self.seal_pending_over_budget(table).await;
        Ok(QueryResult::Inserted { count: row_count })
    }

    /// Validates a delete set against the transaction's begin timestamp, then
    /// commits and records the tombstones' commit ts. Mirrors the reference's
    /// `ConflictCheckTest.conflict_check_delete`: the first committer wins and
    /// concurrent deleters get `kTxnConflict`.
    /// The reference `WalCmdAppendV2` for one contiguous run of rows.
    ///
    /// `block` must be a typed block (`typed_append_block`); the WAL encoder
    /// then writes it in the reference's `DataBlock` layout.
    pub(crate) fn append_command(
        &self,
        table: &str,
        table_id: &str,
        ranges: &[(usize, usize)],
        block: Vec<storage::wal::block::BlockColumn>,
    ) -> storage::WalCmd {
        let (db_name, table_name) = split_table_name(table);
        // One `row_ranges` entry per contiguous run: an append that crossed a
        // segment boundary is two runs, and the reference's frame carries both
        // (`WalCmdAppendV2::row_ranges_`).
        storage::WalCmd::Append {
            db_id: self.database_id(&db_name).to_string(),
            db_name,
            table_name,
            table_id: table_id.to_string(),
            row_ranges: ranges
                .iter()
                .map(|(base, count)| {
                    (
                        storage::wal_entry::WalRowId::from_u64(*base as u64),
                        *count as u64,
                    )
                })
                .collect(),
            block,
        }
    }

    /// The reference `WalCmdDeleteV2` for a set of row sequences.
    pub(crate) fn delete_command(
        &self,
        table: &str,
        table_id: &str,
        seqs: &[usize],
    ) -> storage::WalCmd {
        let (db_name, table_name) = split_table_name(table);
        storage::WalCmd::Delete {
            db_id: self.database_id(&db_name).to_string(),
            db_name,
            table_name,
            table_id: table_id.to_string(),
            row_ids: seqs
                .iter()
                .map(|s| storage::wal_entry::WalRowId::from_u64(*s as u64))
                .collect(),
        }
    }

    /// A pure `DELETE`: it appends nothing, so the block's `version` gets the
    /// delete timestamps and no `create` of its own.
    pub(crate) async fn commit_with_deletes(
        &self,
        table: &str,
        deletes: &[usize],
        extra: &[(usize, Vec<Value>)],
        mut txn: Txn,
    ) -> Result<u64> {
        // See `insert_literals_inner`: a direct delete must invalidate the
        // cached `SELECT` results too.
        self.result_cache.lock().unwrap().clear();
        self.plan_cache.clear();
        self.invalidate_row_cache(table);
        self.note_live_rows(table, -(deletes.len() as isize));
        // The block mirror keeps its snapshot: deleted row ids are hidden by
        // the tombstones written here, and rows an UPDATE re-inserted arrive
        // with fresh ids that the journal serves. The mirror still has to mark
        // the slots (`BlockVersion::Delete`) once the commit is durable: the
        // reference hides a deleted row through the block's `version` file, so
        // a directory whose version was not refreshed would show it again.
        for seq in deletes {
            txn.delete_deferred(wal_delete_key(table, *seq));
        }
        // `NewTxn::Delete`: row level against other deletes and updates, table
        // level against a compaction.
        txn.set_op(TxnOp::Delete {
            table: table.to_string(),
            rows: deletes.iter().map(|seq| *seq as u64).collect(),
        });
        let ts = self.txn.commit_deferred(txn).await?;
        let _visible = BottomGuard::new(&self.txn, ts);
        // `CommitBottomDelete` on the table's worker: the block's `version`
        // records the delete ts. A `DELETE` stages no append - passing a count
        // here settled record 0 (`Some(0)` means "the last zero records", i.e.
        // "keep the log"), and `settle_append_at(0, …)` overwrote the first
        // `create` a previous process had written with this delete's ts.
        self.delete_bottom(table, deletes, None, extra.to_vec(), ts)
            .await?;
        Ok(ts)
    }

    /// `CommitBottomDelete`: the delete joins the table's log, the block's
    /// `version` carries both timestamps, and the rows the statement appended
    /// go into the block (`AppendInBlock`).
    pub(crate) async fn delete_bottom(
        &self,
        table: &str,
        deletes: &[usize],
        appended: AppendRecords,
        extra: Vec<(usize, Vec<Value>)>,
        ts: u64,
    ) -> Result<()> {
        let bottom_table = table.to_string();
        let shard_key = bottom_table.clone();
        let delete_count = deletes.len();
        let bottom_deletes = deletes.to_vec();
        let bottom_rows = extra;
        self.run_commit_bottom(&shard_key, move |engine: &Engine| {
            Box::pin(async move {
                // The delete joins the table's log only now: a conflicting
                // transaction must leave no trace, and this closure runs after the
                // commit was accepted.
                let first_delete = engine.note_deleted_ids(&bottom_table, &bottom_deletes);
                // The slot is hidden by the block's `version` (`BlockVersion::
                // Delete`), which is what the reference has: no tombstone key is
                // written. `column_type_of` covers every type the DDL accepts, so
                // `has_block_layout` is true for every table that exists.
                // An UPDATE stages its rewritten rows as an append in the same
                // transaction (`reserve_rows_at` logged the create), so the block's
                // `version` has to record this ts for it as well
                // (`BlockVersion::Append`). The record is named, not counted: an
                // `UPDATE` of n rows is **one** record, and settling "the last n"
                // re-sized the log from n - zeroing the creates a previous process
                // had written (`[(30,100),(30,200),(30,210)]` instead of the
                // reference's `[(14,100),(26,200),(30,210)]`).
                if let Some(record) = appended {
                    engine.settle_append_at(&bottom_table, record, ts);
                }
                // `BlockVersion::Delete(row_id, commit_ts)`: the slot carries the
                // deleting transaction's commit timestamp, and the block rewrite
                // marks it - there is no tombstone key to write.
                engine.settle_delete_at(&bottom_table, first_delete, delete_count, ts);
                engine.refresh_block_mirror_extra(&bottom_table, &bottom_rows)
            })
        })
        .await
    }

    /// Commits an `UPDATE`: the reference's `DeleteV2` and `AppendV2` in one
    /// entry, with the rewritten rows' ids allocated **inside** the commit.
    ///
    /// `prepare` runs between the conflict check and the WAL entry, gets the
    /// base id and fills in the entry's commands; the rows themselves reach the
    /// block in the bottom half.
    pub(crate) async fn commit_rewritten_rows<F>(
        &self,
        table: &str,
        deletes: &[usize],
        rows: &[Vec<Value>],
        txn: Txn,
        prepare: F,
    ) -> Result<RowRange>
    where
        F: FnOnce(&mut Txn, RowRange) -> Result<()>,
    {
        self.result_cache.lock().unwrap().clear();
        self.plan_cache.clear();
        self.invalidate_row_cache(table);
        self.note_live_rows(table, -(deletes.len() as isize));
        let mut txn = txn;
        for seq in deletes {
            txn.delete_deferred(wal_delete_key(table, *seq));
        }
        // `NewTxn::Update`: table level against another update, row level
        // against a delete, segment level against a memory-index dump.
        let mut segments: Vec<u32> = deletes.iter().map(|seq| (*seq >> 32) as u32).collect();
        segments.sort_unstable();
        segments.dedup();
        txn.set_op(TxnOp::Update {
            table: table.to_string(),
            rows: deletes.iter().map(|seq| *seq as u64).collect(),
            segments,
        });
        // `GetWriteCommitTS` -> `SubmitForAllocation`: the ids are taken in
        // commit-ts order, so a checkpoint always sees a dense prefix.
        let turn = self.txn.begin_write();
        let ts = turn.ts();
        let count = rows.len();
        let allocated = std::sync::Mutex::new(RowRange::single(0));
        // The append record `reserve_rows_at` created, so the bottom half
        // stamps **that** one; `None` when the statement rewrote no rows.
        let record = std::sync::Mutex::new(None);
        let ts = self
            .txn
            .commit_at_deferred_with(txn, ts, async |txn| {
                let range = self.reserve_rows_at(table, count, false, ts)?;
                *allocated.lock().unwrap() = range;
                *record.lock().unwrap() = (count > 0).then_some(range.append);
                prepare(txn, range)
            })
            .await?;
        let range = *allocated.lock().unwrap();
        let _visible = BottomGuard::new(&self.txn, ts);
        // One copy of the rewritten rows: the block buffer takes them as
        // `Value`s, and the caller keeps its slice for the index hooks.
        let extra: Vec<(usize, Vec<Value>)> = rows
            .iter()
            .enumerate()
            .map(|(k, row)| (range.seq_at(k), row.clone()))
            .collect();
        // The staged append record settles with its commit ts here, which is what
        // `committed_rows` reads; without it a checkpoint would not see the
        // rewritten rows (`reserve_rows_at` no longer stamps it).
        // Read out of the mutex before the await: the guard is not `Send`.
        let record = *record.lock().unwrap();
        self.delete_bottom(table, deletes, record, extra, ts)
            .await?;
        Ok(range)
    }

    /// Whether committing `batch` would leave the catalog exactly as it is.
    ///
    /// The C++-layout rewrite is a full rewrite: every DDL re-emits every
    /// record, so a statement that changed nothing still produces a batch.
    /// Comparing each item with what the store already holds is what tells the
    /// two apart.
    pub(crate) fn batch_is_noop(&self, batch: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<bool> {
        let store = self.storage.catalog.store().as_ref();
        for (key, value) in batch {
            let current = store.get(key)?;
            let unchanged = match value {
                Some(value) => current.as_deref() == Some(value.as_slice()),
                None => current.is_none(),
            };
            if !unchanged {
                return Ok(false);
            }
        }
        Ok(true)
    }

    /// `HIGHLIGHT col, …`: replaces each named output column with its text
    /// annotated by `<em>…</em>` around the query term occurrences, keeping only
    /// the sentences that contain a hit (`highlighter_impl.cpp`).
    pub(crate) fn apply_highlights(
        &self,
        res: &mut QueryResult,
        columns: &[String],
        search: Option<&SearchClause>,
        table: Option<&str>,
    ) {
        let QueryResult::Rows {
            columns: names,
            rows,
            ..
        } = res
        else {
            return;
        };
        let Some(query) = search.and_then(|s| {
            s.matches
                .iter()
                .find(|m| m.kind.eq_ignore_ascii_case("TEXT"))
                .and_then(|m| arg_str(m.args.first()))
        }) else {
            return;
        };
        let Some(table) = table else { return };
        for (i, name) in names.iter().enumerate() {
            if !columns.iter().any(|c| c == name) {
                continue;
            }
            let tokenizer = self.index_tokenizer(table, name);
            // `Highlighter::GetHighlight` analyses **both** sides with the
            // column's analyzer (`highlighter_impl.cpp:41-46`: the analyzer
            // `physical_project_impl.cpp:123` fetches for the index), so an
            // `ik`/`ngram`/`rag-chinese` field is highlighted with the tokeniser
            // that produced its postings - otherwise the document side would be
            // scanned by the standard tokenizer and a term the analyzer emitted
            // could never be found.
            //
            // The `standard` family and the delimiter families (`keyword`,
            // `whitespace[-chars]`) keep the byte scanner instead: it is the
            // measured form of the reference's output (its highlighter splits
            // `com/anarchism` where the index side unites `/`) and it is what a
            // column without an explicit analyzer already uses, so the default
            // and `analyzer='standard'` stay identical.
            let analyzer = tokenizer
                .analyzer
                .as_deref()
                .filter(|name| {
                    !name
                        .split('-')
                        .next()
                        .is_some_and(|family| family.eq_ignore_ascii_case("standard"))
                })
                .and_then(|name| analyzer::AnalyzerPool::instance().get_analyzer(name).ok());
            let qterms = tokenize_index_text(&query, &tokenizer);
            // The column is rebuilt once rather than patched cell by cell: a
            // text column is one buffer plus offsets, so writing a cell in
            // place would shift every later offset.
            let cells: Vec<Option<String>> = (0..rows.row_count)
                .map(|r| match rows.data.get(i).map(|c| c.value_at(r)) {
                    Some(Value::Null) | None => None,
                    Some(value) => {
                        let text = value.to_slt_string();
                        Some(match &analyzer {
                            Some(analyzer) => highlight_with_analyzer(&query, &text, analyzer),
                            None => highlight_text(&text, &qterms, &tokenizer.delims),
                        })
                    }
                })
                .collect();
            if let Some(column) = rows.data.get_mut(i) {
                *column = ColumnData::text_from_cells(&cells);
            }
        }
    }

    /// Feeds one row into every secondary index of `table`, resolving the
    /// targets first.
    ///
    /// Kept for the statement paths that insert one row at a time; a bulk load
    /// resolves the targets once and calls [`Self::insert_secondary_keys`].
    pub(crate) fn insert_secondary_keys(
        &self,
        targets: &[SecondaryTarget],
        seq: usize,
        values: &[Literal],
    ) -> Result<()> {
        // The evaluated row is only built when a functional index needs it: a
        // plain index reads its cell's text and pays nothing.
        let mut row_values: Option<Vec<Value>> = None;
        for target in targets {
            let Some(value) = values.get(target.pos) else {
                continue;
            };
            // `SecondaryIndexInMemT::InsertInner` skips a row whose null bit is
            // set: a NULL is not a key.
            if matches!(value, Literal::Null) {
                continue;
            }
            let keys = match &target.func {
                None => text_to_tail_keys(&target.declared, &value.to_slt_string()),
                Some(func) => {
                    let row = row_values
                        .get_or_insert_with(|| values.iter().map(Value::from_literal).collect());
                    match functional_value(func, target.pos, row) {
                        Some(value) => value_to_key(&target.declared, &value),
                        None => Vec::new(),
                    }
                }
            };
            for key in keys {
                target.index.insert(&key, seq as u64);
            }
        }
        Ok(())
    }

    /// `[storage] flush_dumps_mem_index`: whether `FLUSH DATA` also dumps every
    /// memory index (see `Engine::flush_storage`). Off by default, like the
    /// reference.
    pub fn set_flush_dumps_mem_index(&self, on: bool) {
        self.context
            .flush_dumps_mem_index
            .store(on, std::sync::atomic::Ordering::Relaxed);
    }

    /// Feeds a whole batch of rows into every secondary index of `table`.
    ///
    /// The per-row form resolved the table key (a formatted, lowercased
    /// string) and took the registry lock for every row, and looked the indexed
    /// column up in the schema once per index per row. A bulk statement paid
    /// that per row; this pays it once for the batch.
    ///
    /// Each key is the column's **order-preserving encoding**
    /// (`Key::ordered_bytes`) - the same domain a sealed chunk uses - so the
    /// memory tail is ordered like the column (a range scan slices it instead
    /// of decoding every key) and a dump can write its keys out directly.
    ///
    /// The input is the batch's **literals**, converted one row at a time but
    /// only at the indexed positions: the value form converted every column of
    /// every row (`Value::from_literal` renders a vector as text), which was 23%
    /// of an insert even when the only index was on `num`.
    pub(crate) fn update_secondary_indexes_batch_rows(
        &self,
        table: &str,
        rows: &[Vec<Literal>],
        schema: &[String],
        seq_of: impl Fn(usize) -> usize,
    ) {
        if rows.is_empty() {
            return;
        }
        let targets = self.secondary_targets(table, schema);
        if targets.is_empty() {
            return;
        }
        for (k, row) in rows.iter().enumerate() {
            // The id of the k-th row: `base + k` only while the allocation is
            // one contiguous range (a segment boundary splits it).
            let seq = seq_of(k) as u64;
            for target in &targets {
                let Some(literal) = row.get(target.pos) else {
                    continue;
                };
                // `NULL` stays `Value::Null` (`value_to_tail_keys` skips it)
                // and a float keeps its exact digits - both of which a text
                // round-trip would change.
                let value = Value::from_literal(literal);
                let keys = match &target.func {
                    None => value_to_tail_keys(&target.declared, &value),
                    Some(func) => {
                        // A functional index reads one column
                        // (`functional_parts` rejects anything else), so only
                        // that position needs a value.
                        let mut row = vec![Value::Null; target.pos + 1];
                        row[target.pos] = value;
                        match functional_value(func, target.pos, &row) {
                            Some(value) => value_to_key(&target.declared, &value),
                            None => Vec::new(),
                        }
                    }
                };
                for key in keys {
                    target.index.insert(&key, seq);
                }
            }
        }
    }

    /// `(position in the row, declared type, memory tail)` of each indexed
    /// column, resolved once per statement and held outside the table lock.
    ///
    /// The tail is an `Arc` (the registry only borrows it while building this
    /// list), so the rows go into it without excluding any other table - the
    /// reference writes into each `SecondaryIndexInMemT` the same way.
    /// Whether `table` has any secondary index at all.
    ///
    /// A per-row `secondary_targets` takes the table lock, walks the index map
    /// and allocates a `Vec` - for eight million rows on a table with no
    /// secondary index that is eight million locks spent to find nothing.
    pub(crate) fn has_secondary_indexes(&self, table: &str) -> bool {
        let tkey = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&tkey)
            .is_some_and(|entry| !entry.secondary.indexes.is_empty())
    }

    /// Whether `table` has any index the per-row write hook maintains.
    ///
    /// The hook (`update_text_indexes_batch_async`) also carries the sparse and
    /// multi-vector indexes - it is the one place every write path (SQL
    /// INSERT/UPDATE, `COPY`, the thrift insert) goes through - so a table with
    /// only a sparse (BMP) index needs it as much as one with a full-text
    /// index. Gating on full-text and vector alone dropped every row of a
    /// sparse column, which `SEARCH MATCH SPARSE` then could not see.
    ///
    /// On a table with none of them the hook is a lock plus an allocation that
    /// finds nothing, eight million times over a bulk `COPY`.
    ///
    /// Read-only: the `hnsw_probed` short-circuit belongs to
    /// `hnsw_mem_segments`, and setting it here would make that skip its probe.
    pub(crate) fn has_text_or_vector_indexes(&self, table: &str) -> bool {
        let key = self.table_key(table);
        let has_mem = {
            let tables = self.tables.lock().unwrap();
            tables.get(&key).is_some_and(|entry| {
                !entry.hnsw_mem.is_empty()
                    || !entry.ivf_mem.is_empty()
                    || !entry.sparse.is_empty()
                    || !entry.mv.is_empty()
                    || !entry.text.is_empty()
            })
        };
        if has_mem {
            return true;
        }
        // An index the registries have not opened yet (a fresh directory, or a
        // column this table entry has not seen): the catalog record decides.
        self.storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .iter()
            .any(|im| {
                (im.table == table || im.table == self.qualified_table(table))
                    && (im.index_type.eq_ignore_ascii_case("FULLTEXT")
                        || im.index_type.eq_ignore_ascii_case("HNSW")
                        || im.index_type.eq_ignore_ascii_case("IVF")
                        || im.index_type.eq_ignore_ascii_case("BMP")
                        || im.index_type.eq_ignore_ascii_case("SPARSE")
                        || im.index_type.eq_ignore_ascii_case("EMVB"))
            })
    }

    pub(crate) fn secondary_targets(&self, table: &str, schema: &[String]) -> Vec<SecondaryTarget> {
        let tkey = self.table_key(table);
        let tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get(&tkey) else {
            return Vec::new();
        };
        // The registries are keyed by index id; the column each one indexes
        // comes from the catalog record (`IndexMeta::column`), which for a
        // functional index is the expression it was declared with.
        let Ok(catalog) = self.storage.catalog.indexes_of_table(table) else {
            return Vec::new();
        };
        let columns: std::collections::HashMap<u64, String> =
            catalog.into_iter().map(|im| (im.id, im.column)).collect();
        entry
            .secondary
            .indexes
            .iter()
            .filter_map(|(id, index)| {
                let col = columns.get(id)?;
                let declared = entry.secondary.declared_types.get(id)?;
                // A functional index reads `base` and applies `func`; its
                // declared type is the function's return type
                // (`IndexSecondaryFunctional::func_return_type_`).
                let (pos, func) = match entry.secondary.functional.get(col) {
                    Some(functional) => (
                        schema.iter().position(|c| c == &functional.base)?,
                        Some(functional.func.clone()),
                    ),
                    None => (schema.iter().position(|c| c == col)?, None),
                };
                Some(SecondaryTarget {
                    pos,
                    declared: declared.clone(),
                    index: Arc::clone(index),
                    func,
                })
            })
            .collect()
    }

    /// The `Value` form of [`Self::update_indexes_on_insert`]: a callback path
    /// that only has the rendered cells cannot tell a NULL from an empty string,
    /// so every caller hands the row over as values.
    pub(crate) fn update_indexes_on_insert_text(
        &self,
        table: &str,
        seq: usize,
        values: &[Value],
        schema: &[String],
    ) -> Result<()> {
        for target in self.secondary_targets(table, schema) {
            let Some(value) = values.get(target.pos) else {
                continue;
            };
            let keys = match &target.func {
                None => value_to_tail_keys(&target.declared, value),
                Some(func) => match functional_value(func, target.pos, values) {
                    Some(value) => value_to_key(&target.declared, &value),
                    None => Vec::new(),
                },
            };
            for key in keys {
                target.index.insert(&key, seq as u64);
            }
        }
        Ok(())
    }

    /// Feeds an inserted/rewritten row into the in-memory HNSW segment of every
    /// vector index on `table` — the reference's `HnswIndexInMem`, which holds
    /// the rows written since the last dump. The sealed (on-disk) graph lives in
    /// `vector_indexes`.
    /// The per-segment HNSW graphs one row belongs to.
    ///
    /// The reference keys `MemIndex` by `(index_id, segment)`, so a row only
    /// ever enters its own segment's graph, and two HNSW indexes on one column
    /// own two graphs. Returns `(column, graph)` for each such index.
    ///
    /// Split out of [`Self::note_hnsw_insert`] so a **batch** can resolve the
    /// graphs once and then hand the whole run to `insert_batch`, which is what
    /// lets a `COPY` use the bucket fan-out instead of one graph insertion per
    /// row.
    async fn hnsw_graphs_for(
        &self,
        table: &str,
        seq: u64,
        schema: &[String],
    ) -> Vec<(
        u64,
        String,
        Arc<tokio::sync::Mutex<crate::hnsw_mem::HnswIndexInMem>>,
    )> {
        let key = self.table_key(table);
        // `table_schema` is a full catalog read, so it must not be paid per
        // row: probe the index set once per table, and once there is nothing
        // in `hnsw_mem` the hook has nothing to do.
        // Two questions, one pass, and this is the per-row path:
        // * `topup` - must the catalog be read to materialise a graph this
        //   registry does not hold yet? Exactly when a probe was asked for and
        //   has not run (`CREATE INDEX` sets `hnsw_topup`, the probe clears it).
        // * `probing` - is there any graph to feed? A table with none returns
        //   here without touching the catalog again.
        let (topup, probing) = {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key.clone()).or_default();
            // The first insert on a table has to find out whether it has an
            // HNSW index at all.
            let first = entry.hnsw_mem.is_empty() && !entry.hnsw_probed;
            entry.hnsw_probed |= first;
            let topup = std::mem::take(&mut entry.hnsw_topup) || first;
            (topup, topup || !entry.hnsw_mem.is_empty())
        };
        if !probing {
            return Vec::new();
        }
        // The table lock is only held to reach the per-segment mutexes; the
        // graph updates run under each segment's own lock.
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        // Materialise the graphs the catalog lists, once per table: this
        // runs on **every inserted row**, so re-reading the catalog here
        // would make a warm insert cost "rows x every index in the engine".
        //
        // A later `CREATE INDEX … USING HNSW` clears `hnsw_probed` (see the
        // `CREATE INDEX` path) and registers its own id, so the next row
        // re-probes and the new index gets a graph of its own - the
        // reference's `MemIndex` map is keyed by index id, so two indexes on
        // one column never share one.
        if topup {
            let Ok(indexes) = self.storage.catalog.list_indexes() else {
                return Vec::new();
            };
            let Ok(types) = self.column_types(table) else {
                return Vec::new();
            };
            let short = table.split('.').next_back().unwrap_or(table);
            for im in indexes {
                if !(im.table == table || im.table == short)
                    || !im.index_type.eq_ignore_ascii_case("HNSW")
                {
                    continue;
                }
                let Some(ci) = schema.iter().position(|c| c == &im.column) else {
                    continue;
                };
                let Some(dims) = types.get(ci).and_then(|t| type_dim(t)) else {
                    continue;
                };
                let options = crate::blocks::index_chunk::HnswOptions::from_options(&im.options);
                // One graph per `(index, segment)`: remember the options so
                // the graph for a segment this table has not written yet
                // can be created when a row of it arrives.
                entry.hnsw_options.insert(im.id, (dims, options));
            }
        }
        // The row goes to its own segment's graph, for every HNSW index on
        // the table: the reference keys `MemIndex` by `(index_id, segment)`,
        // so a segment's rows never enter another segment's graph.
        let segment = (seq >> 32) as u32;
        let by_id: std::collections::HashMap<u64, String> = self
            .storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .map(|im| (im.id, im.column))
            .collect();
        let mut out: Vec<(
            u64,
            String,
            Arc<tokio::sync::Mutex<crate::hnsw_mem::HnswIndexInMem>>,
        )> = Vec::new();
        for (id, column) in &by_id {
            let Some((dims, options)) = entry.hnsw_options.get(id).copied() else {
                continue;
            };
            // `HnswIndexInMem::begin_row_id_`: the segment's first row.
            let begin_row_id = (seq >> 32) << 32;
            let mem = entry.hnsw_mem.entry((*id, segment)).or_insert_with(|| {
                Arc::new(tokio::sync::Mutex::new(
                    crate::hnsw_mem::HnswIndexInMem::new_with(
                        dims,
                        &options,
                        segment,
                        begin_row_id,
                    ),
                ))
            });
            out.push((*id, column.clone(), std::sync::Arc::clone(&*mem)));
        }
        out
    }

    pub(crate) async fn note_hnsw_insert(&self, table: &str, seq: u64, cells: &[String]) {
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (index_id, column, mem) in self.hnsw_graphs_for(table, seq, &schema).await {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
            let mut mem = mem.lock().await;
            if v.len() == mem.dims {
                mem.insert(seq, &v).await;
                self.note_unsealed(table, index_id, 1);
            }
        }
    }

    /// [`Self::note_hnsw_insert`] for a whole run of rows.
    ///
    /// The reference inserts a `COPY`'s vectors through
    /// `HnswHandler::InsertVecs(block_offset, col, offset, row_count, …)`,
    /// which splits the rows into `kBuildBucketSize` buckets and builds them in
    /// parallel. Feeding one row at a time instead made a 1M-row import a
    /// serial graph build on a single core - the shape issue #3573 reports as
    /// "Rust HNSW build is 6x slower".
    ///
    /// Rows are grouped by segment (a graph is per `(index, segment)`) and each
    /// group is one `insert_batch`, which is what carries the bucket fan-out.
    pub(crate) async fn note_hnsw_insert_batch(&self, table: &str, rows: &[(usize, &[String])]) {
        if rows.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        // A run of rows normally shares one segment; group anyway, because a
        // `COPY` can straddle a segment boundary.
        let mut by_segment: std::collections::BTreeMap<u32, Vec<usize>> =
            std::collections::BTreeMap::new();
        for (at, (seq, _)) in rows.iter().enumerate() {
            by_segment.entry((*seq >> 32) as u32).or_default().push(at);
        }
        for (_, group) in by_segment {
            let first_seq = rows[group[0]].0 as u64;
            let graphs = self.hnsw_graphs_for(table, first_seq, &schema).await;
            if graphs.is_empty() {
                return;
            }
            for (index_id, column, mem) in graphs {
                let Some(ci) = schema.iter().position(|c| c == &column) else {
                    continue;
                };
                let dims = { mem.lock().await.dims };
                let batch: Vec<(u64, Vec<f32>)> = group
                    .iter()
                    .filter_map(|at| {
                        let (seq, cells) = rows[*at];
                        let text = cells.get(ci)?;
                        let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
                        (v.len() == dims).then_some((seq as u64, v))
                    })
                    .collect();
                if batch.is_empty() {
                    continue;
                }
                crate::hnsw_mem::HnswIndexInMem::insert_batch(
                    &mem,
                    &batch,
                    self.hnsw_build_workers(),
                )
                .await;
                self.note_unsealed(table, index_id, batch.len() as u64);
            }
        }
    }

    pub(crate) async fn update_vector_caches(&self, table: &str, seq: usize, cells: &[String]) {
        // Only the HNSW/IVF memory indexes remain to maintain here: the vector
        // query cache was deleted, the mapped blocks being what a scan reads.
        self.note_hnsw_insert(table, seq as u64, cells).await;
        self.note_ivf_insert(table, seq as u64, cells);
    }

    /// Feeds a row into the IVF memory segment (rows written since the last
    /// mirror dump), the reference's in-memory IVF form.
    pub(crate) fn note_ivf_insert(&self, table: &str, seq: u64, cells: &[String]) {
        let tkey = self.table_key(table);
        if self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .is_none_or(|e| e.ivf_mem.is_empty())
        {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        let by_id = self.index_column_map(table);
        let mut grew: Vec<u64> = Vec::new();
        for (index_id, rows) in entry.ivf_mem.iter_mut() {
            let Some(column) = by_id.get(index_id) else {
                continue;
            };
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
            let dims = types.get(ci).and_then(|t| type_dim(t)).unwrap_or(0);
            if !v.is_empty() && (dims == 0 || v.len() == dims) {
                rows.push((seq, v));
                grew.push(*index_id);
            }
        }
        // Outside the `ivf_mem` borrow: the same entry owns the marks map.
        for index_id in grew {
            entry.bump_unsealed(index_id, 1);
        }
    }

    /// Records that an index's in-memory tail grew, for the periodic reclaim
    /// loop's candidate test (`TableEntry::unsealed_marks`).
    ///
    /// A separate call rather than an inline `entry.bump_unsealed` because most
    /// hooks take their `tables` guard in a different scope than the check
    /// "did this row reach the tail". Over-reporting is safe (one wasted
    /// attempt, absorbed by the failure backoff); under-reporting would keep a
    /// table in memory forever, so a hook errs toward recording.
    pub(crate) fn note_unsealed(&self, table: &str, index_id: u64, rows: u64) {
        if rows == 0 {
            return;
        }
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.bump_unsealed(index_id, rows);
        }
    }

    /// `index id -> column` for the table's indexes, resolved from the catalog.
    ///
    /// The in-memory registries are keyed by index id, so a hook that walks one
    /// needs the column each id was built on to find its cell.
    fn index_column_map(&self, table: &str) -> std::collections::HashMap<u64, String> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .map(|im| (im.id, im.column))
            .collect()
    }

    /// The table's full-text columns, from the catalog.
    ///
    /// [`Self::index_column_map`] carries every index kind, so it cannot say
    /// which columns a *text* index belongs to - a column with only a secondary
    /// index has no text postings to write. A column that carries two full-text
    /// indexes (a sparse-gram one and a regular one) is listed once.
    fn fulltext_columns(&self, table: &str) -> Vec<String> {
        let mut columns: Vec<String> = Vec::new();
        for meta in self
            .storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
        {
            if meta.id > 0
                && meta.index_type.eq_ignore_ascii_case("FULLTEXT")
                && !columns.contains(&meta.column)
            {
                columns.push(meta.column);
            }
        }
        columns
    }

    /// Undoes the index hooks for rows a write staged but never committed.
    ///
    /// An `IMPORT` feeds every hook batch by batch (`insert_secondary_keys`,
    /// `update_text_indexes_batch`, the sparse/multi-vector hooks, the vector
    /// caches) while it reads its file, and only commits at the end. A file
    /// that turns invalid past its first batch therefore leaves entries for rows
    /// no block will ever hold - and the chunk a later dump writes would make
    /// them durable. This removes them again.
    ///
    /// A secondary tail is the one structure that needs a real removal
    /// (`SecondaryIndex::remove_row`): the other kinds' tails are keyed by row
    /// id, and the text index drops a document by its row id too.
    pub(crate) fn rollback_index_rows(&self, table: &str, seqs: &[usize]) {
        if seqs.is_empty() {
            return;
        }
        let tkey = self.table_key(table);
        let dead: std::collections::HashSet<u64> = seqs.iter().map(|s| *s as u64).collect();
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        for index in entry.secondary.indexes.values() {
            for seq in seqs {
                index.remove_row(*seq as u64);
            }
        }
        for sparse in entry.sparse.values_mut() {
            let mut guard = sparse.write().unwrap_or_else(|p| p.into_inner());
            for seq in seqs {
                guard.remove(*seq as u64);
            }
        }
        for mv in entry.mv.values_mut() {
            let mut guard = mv.write().unwrap_or_else(|p| p.into_inner());
            for seq in seqs {
                guard.remove(*seq as u64);
            }
        }
        for text in entry.text.values() {
            // `try_write`: one caller is `ImportStaging::drop`, which cannot
            // await. An index a writer holds right now keeps the row until its
            // own rollback path runs; the import's rows are never published in
            // that case.
            let Ok(mut guard) = text.try_write() else {
                continue;
            };
            for seq in seqs {
                guard.mem.delete(*seq as u64);
            }
        }
        // HNSW and IVF have no per-row removal (a graph and a clamped set), so
        // their tails are **dropped** instead: the next dump rebuilds them from
        // the blocks, which hold only committed rows. Dropping is cheap - the
        // handle is rebuilt on the next write - and it is the only answer that
        // cannot leave an uncommitted row in the graph.
        if !entry.hnsw_mem.is_empty() {
            entry.hnsw_mem.clear();
            entry.hnsw_topup = true;
        }
        for rows in entry.ivf_mem.values_mut() {
            rows.retain(|(seq, _)| !dead.contains(seq));
        }
    }

    pub(crate) fn update_multivector_indexes_batch(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
    ) {
        if rows.is_empty() {
            return;
        }
        let tkey = self.table_key(table);
        let indexes: Vec<(u64, String, Arc<std::sync::RwLock<MultiVectorIndex>>)> = {
            let tables = self.tables.lock().unwrap();
            let Some(e) = tables.get(&tkey) else {
                return;
            };
            if e.mv.is_empty() {
                return;
            }
            drop(tables);
            let by_id = self.index_column_map(table);
            self.tables
                .lock()
                .unwrap()
                .get(&tkey)
                .map(|e| {
                    e.mv.iter()
                        .filter_map(|(id, v)| Some((*id, by_id.get(id)?.clone(), v.clone())))
                        .collect()
                })
                .unwrap_or_default()
        };
        if indexes.is_empty() {
            return;
        }
        let Ok(types) = self.column_types(table) else {
            return;
        };
        for (index_id, column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let Some(dims) = types.get(ci).and_then(|t| type_dim(t)) else {
                continue;
            };
            if dims == 0 {
                continue;
            }
            let mut index = shared.write().unwrap();
            for (seq, cells) in rows {
                index.remove(*seq as u64);
                index.note_row();
                let Some(text) = cells.get(ci) else { continue };
                if let Some(doc) = parse_multivector_doc(text, dims) {
                    index.insert(*seq as u64, doc);
                }
            }
            drop(index);
            self.note_unsealed(table, index_id, rows.len() as u64);
        }
    }

    pub(crate) fn update_sparse_indexes_batch(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
    ) {
        if rows.is_empty() {
            return;
        }
        let tkey = self.table_key(table);
        let indexes: Vec<(u64, String, Arc<std::sync::RwLock<SparseIndex>>)> = {
            let tables = self.tables.lock().unwrap();
            let Some(e) = tables.get(&tkey) else {
                return;
            };
            if e.sparse.is_empty() {
                return;
            }
            drop(tables);
            let by_id = self.index_column_map(table);
            self.tables
                .lock()
                .unwrap()
                .get(&tkey)
                .map(|e| {
                    e.sparse
                        .iter()
                        .filter_map(|(id, v)| Some((*id, by_id.get(id)?.clone(), v.clone())))
                        .collect()
                })
                .unwrap_or_default()
        };
        if indexes.is_empty() {
            return;
        }
        for (index_id, column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let mut index = shared.write().unwrap();
            for (seq, cells) in rows {
                index.remove(*seq as u64);
                index.note_row();
                let Some(text) = cells.get(ci) else { continue };
                if text.trim().eq_ignore_ascii_case("null") {
                    continue;
                }
                let entries: Vec<(i32, f32)> = parse_sparse_str(text)
                    .into_iter()
                    .map(|(i, x)| (i as i32, x as f32))
                    .collect();
                index.insert(*seq as u64, entries);
            }
            drop(index);
            self.note_unsealed(table, index_id, rows.len() as u64);
        }
    }

    /// Documents tokenised per chunk while a batch is indexed. Bounded so a
    /// `COPY` of a million rows never holds every row's terms in the heap, and
    /// so one lock acquisition only covers a chunk's worth of work.
    const TEXT_BATCH_ROWS: usize = 4096;

    /// Tokenises a batch for every full-text column **before** the table's write
    /// gate is taken. The analyzer needs no shared state (only a snapshot of the
    /// column's tokenizer), so two writers to one table can overlap it while the
    /// gate's holder commits and appends its block; without this they queue for
    /// the whole statement and the analysis runs serialised.
    ///
    /// `None` when no column has a full-text index. The caller passes the batch
    /// back to [`Self::update_text_indexes_batch`], which uses it while every
    /// row id still matches (`base + position`) and falls back to tokenising for
    /// a column that gained an index in between (the set is re-read under the
    /// gate).
    pub(crate) async fn prepare_text_terms(
        &self,
        table: &str,
        cells: &Arc<Vec<Vec<String>>>,
    ) -> Option<PreparedText> {
        if cells.is_empty() {
            return None;
        }
        let tkey = self.table_key(table);
        // Clone the handles, then drop `tables`: an index lock is only ever taken
        // after it (see `batch_gate`'s lock-order note).
        let by_id = self.index_column_map(table);
        let indexes: Vec<(String, SharedTextIndex)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.text
                    .iter()
                    .filter_map(|(id, v)| Some((by_id.get(id)?.clone(), v.clone())))
                    .collect()
            })
            .unwrap_or_default();
        if indexes.is_empty() {
            return None;
        }
        let schema = self.table_schema(table).unwrap_or_default();
        let mut jobs = Vec::new();
        for (column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let tokenizer = {
                let index = shared.read().await;
                index.tokenizer.clone()
            };
            jobs.push((column, ci, tokenizer));
        }
        if jobs.is_empty() {
            return None;
        }
        let columns: Vec<(String, Vec<Option<PreparedTerms>>)> = if cells.len() >= 16 {
            let cells_bg = Arc::clone(cells);
            tokio::task::spawn_blocking(move || {
                jobs.into_iter()
                    .map(|(column, ci, tokenizer)| {
                        let rows = cells_bg
                            .iter()
                            .map(|row| {
                                let text = row.get(ci)?;
                                let doc = prepare_doc(0, text, &tokenizer);
                                Some((doc.terms, doc.cost))
                            })
                            .collect();
                        (column, rows)
                    })
                    .collect()
            })
            .await
            .unwrap_or_default()
        } else {
            jobs.into_iter()
                .map(|(column, ci, tokenizer)| {
                    let rows = cells
                        .iter()
                        .map(|row| {
                            let text = row.get(ci)?;
                            let doc = prepare_doc(0, text, &tokenizer);
                            Some((doc.terms, doc.cost))
                        })
                        .collect();
                    (column, rows)
                })
                .collect()
        };
        if columns.is_empty() {
            return None;
        }
        Some(PreparedText {
            seqs: Vec::new(),
            columns,
        })
    }

    /// Keeps existing inverted indexes in step with a batch of inserted or
    /// rewritten rows.
    ///
    /// Two phases per column: the documents are **tokenised without the write
    /// lock** (with a snapshot of the tokenizer), then the chunk is inserted
    /// under **one** lock acquisition. Before this, a multi-row INSERT or COPY
    /// took the column's write lock once per row and ran the analyzer inside it,
    /// which serialised the most expensive part of the write path against every
    /// reader of that column.
    /// The one form: the commit's bottom half is a **task** now, so the
    /// memindex quota's fallback dump is an `.await` like everything else on the
    /// chain. There used to be a synchronous twin that bridged it with
    /// `block_on`; that bridge would have parked the very worker the dump needs,
    /// which is the deadlock this port is removing.
    pub(crate) async fn update_text_indexes_batch_async(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
        prepared: Option<&mut PreparedText>,
    ) {
        self.update_text_indexes_batch_inner(table, rows, schema, prepared)
            .await;
        self.enforce_memindex_quota(Some(table));
    }

    /// The batch's work, without the quota check.
    async fn update_text_indexes_batch_inner(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
        mut prepared: Option<&mut PreparedText>,
    ) {
        if rows.is_empty() {
            return;
        }
        // The multi-vector and sparse indexes share this DML hook: it is the one
        // place every write path (SQL INSERT/UPDATE, COPY, Thrift insert) goes
        // through.
        self.update_multivector_indexes_batch(table, rows, schema);
        self.update_sparse_indexes_batch(table, rows, schema);
        let tkey = self.table_key(table);
        let by_id = self.index_column_map(table);
        let mut indexes: Vec<(u64, String, SharedTextIndex)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.text
                    .iter()
                    .filter_map(|(id, v)| Some((*id, by_id.get(id)?.clone(), v.clone())))
                    .collect()
            })
            .unwrap_or_default();
        // A writer must not depend on a reader having built the index. The
        // reference's write path only needs the column's mem indexer, which it
        // creates itself; here the documents go into the **cached** index, so a
        // column whose index is not cached yet - a build still in flight, a cache
        // a compaction dropped, a table entry that has not loaded it - was
        // skipped in silence and its postings existed nowhere. A rebuild recovers
        // them only from the blocks, and a `COPY` writes those when its statement
        // commits, so the rows of the import that hit the window are exactly the
        // ones a following build cannot see. Build the index here instead: the
        // read is the one the first query would have done, at most once per
        // column, and only while the cache has no entry for it.
        if indexes.len() < by_id.len() {
            for column in self.fulltext_columns(table) {
                if schema.iter().all(|c| c != &column) {
                    continue;
                }
                let tokenizer = self.index_tokenizer(table, &column);
                // One handle **per index id**, not one shared by every id on
                // the column: each record owns its own `SharedTextIndex`
                // (`text_index_build` keys the cache by id), so pairing one
                // handle with two ids made the publish loop run the same
                // documents into the same index twice - and the second pass
                // deleted rows the first had just written.
                let pending: Vec<u64> = self
                    .index_ids_on_column_of_kind(table, &column, &["FULLTEXT"])
                    .into_iter()
                    .filter(|id| !indexes.iter().any(|(has, _, _)| has == id))
                    .collect();
                for id in pending {
                    let Ok((shared, _)) = self
                        .text_index_with_tokenizer_of_id(table, id, &column, tokenizer.clone())
                        .await
                    else {
                        continue;
                    };
                    indexes.push((id, column.clone(), shared));
                }
            }
        }
        for (index_id, column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            // Phase 1, no lock held: snapshot the tokenizer, then tokenise.
            let tokenizer = {
                let index = shared.read().await;
                index.tokenizer.clone()
            };
            // The batch is bounded: a COPY of a million rows tokenises
            // `TEXT_BATCH_ROWS` documents at a time instead of holding every
            // document's terms in the heap, and the column lock is released
            // between chunks. This is the same bounded shape the reference's
            // inverter ring gives (a full ring is its backpressure), without the
            // ring's handshake.
            for chunk in rows.chunks(Self::TEXT_BATCH_ROWS) {
                // A chunk is written under one `base_row_id` and its `.len` spans
                // `base_row_id..=last_row`, so an unsealed segment may only hold
                // rows of a single `ROW_ID` segment (`row >> 32`). A `COPY` that
                // rolled over into a new segment hands this loop rows of the next
                // one: the previous tail is sealed before they are inserted, and
                // each segment run is handled on its own.
                let segment_of = |seq: usize| (seq as u64) >> 32;
                let mut at = 0usize;
                while at < chunk.len() {
                    let segment = segment_of(chunk[at].0);
                    let mut end = at + 1;
                    while end < chunk.len() && segment_of(chunk[end].0) == segment {
                        end += 1;
                    }
                    let run = &chunk[at..end];
                    at = end;
                    // Publication barrier: from here until the seal below, this
                    // run's rows are "text pending". A seal that ran meanwhile
                    // must not span them - it would write length 0 for a row
                    // whose text arrives a moment later, and the chunk that
                    // publishes it would then overlap this one.
                    let first_row = run[0].0 as u64;
                    let ticket = PublishTicket::announce(shared.as_ref(), first_row);
                    let prepared: Vec<PreparedDoc> = run
                        .iter()
                        .filter_map(|(seq, cells)| {
                            // Terms tokenised before the table's gate was taken.
                            if let Some(pre) = prepared.as_mut() {
                                if let Some((terms, cost)) = pre.take(&column, *seq) {
                                    return Some(PreparedDoc {
                                        seq: *seq as u64,
                                        terms,
                                        cost,
                                    });
                                }
                            }
                            let text = cells.get(ci)?;
                            Some(prepare_doc(*seq as u64, text, &tokenizer))
                        })
                        .collect();
                    if prepared.is_empty() {
                        continue;
                    }
                    // Phase 2, one lock acquisition for the run.
                    let capacity = self.memindex_capacity();
                    let mut index = shared.write().await;
                    if index.unsealed_segment().is_some_and(|held| held != segment) {
                        // Crossed into the next `ROW_ID` segment: seal the old
                        // rows in place. This happens once per 2^32 rows, so the
                        // lock-held write costs nothing measurable.
                        //
                        // This is the one seal that writes with the index lock
                        // held, so it must not take the directory's write gate -
                        // that would invert the `gate -> index` order every other
                        // writer uses (`crate::engine::text_index::dir_gate`). The
                        // chunk it writes is named after its own tail's first row
                        // id, and no concurrent merge or rebuild reaches that tail
                        // before this seal: both hold the gate and only see the
                        // tail once it is sealed.
                        index.seal().await;
                    }
                    for doc in &prepared {
                        index_delete_row(&mut index, doc.seq);
                    }
                    // `index_rows_prepared` takes the shared handle and locks
                    // per document: the spill it may run awaits its encode
                    // fan-out, so no guard of ours may be held across it.
                    drop(index);
                    index_rows_prepared(&shared, &prepared).await;
                    let mut index = shared.write().await;
                    // The rows are in the tail: the barrier can lift before the
                    // seal below, which is this writer's own and covers exactly
                    // the rows it just published. (Through the same guard - the
                    // ticket's `Drop` would re-enter the lock.)
                    index.finish_publish(first_row);
                    let seal = index.should_seal(capacity);
                    drop(index);
                    // The chunk above may already have been sealed inside the
                    // index, but the rows are unsealed from this loop's point of
                    // view until a dump publishes their chunk; recording them
                    // here is what lets the periodic loop find the table. A seal
                    // that just happened only makes the mark conservative.
                    self.note_unsealed(table, index_id, prepared.len() as u64);
                    if seal {
                        // The segment is written with the lock released: the
                        // tail moves to `TextIndex::inflight`, where queries keep
                        // reading it, so nobody waits for the fsync.
                        //
                        // The chunk is only reachable through its catalog record,
                        // so the file and the record are published together under
                        // the directory's write gate: a build that loaded the
                        // chunk list between the two would not see the chunk,
                        // re-index its rows and write the same
                        // `ft_{base_row_id}` name (`Engine::with_text_dir_gate`).
                        Self::with_text_dir_gate(&shared, async {
                            crate::engine::state::seal_text_index_gated(&shared).await;
                            self.record_sealed_text_chunks(table, &column).await;
                        })
                        .await;
                    }
                    drop(ticket);
                }
            }
        }
    }

    /// Records the comment given to `CREATE INDEX` (the SDK reads it back from
    /// `SHOW INDEX`).
    pub fn set_index_comment(&self, table: &str, index: &str, comment: &str) {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .comments
            .insert(index.to_string(), comment.to_string());
    }

    /// Resizes that budget.
    pub fn set_buffer_memory_limit(&self, bytes: usize) {
        // `BufferManager::SetMemoryLimit` in the reference; one implementation
        // with `set_buffer_budget` so the two entry points cannot drift.
        self.set_buffer_budget(bytes);
    }

    /// `WalCmdImportV2` for the segment a `COPY` is writing
    /// (`ImportTxnStore::ToWalEntry`, `base_txn_store_impl.cpp:378`): the
    /// segment's info, never the rows.
    /// Where an import stages the blocks it completes before its commit.
    ///
    /// The reference writes each full `DataBlock` into
    /// `TempDir/import<txn>/db_/tbl_/seg_/blk_/` and moves the directories into
    /// the data dir when the transaction commits (`NewTxn::Import`), which is
    /// what keeps an import's memory at one block. A block written straight
    /// into `data/` would be visible to every reader as soon as its files
    /// existed: `slot_visible` keys off the block's `created` prefix, not the
    /// ts.
    pub(crate) fn import_staging_root(&self, txn_id: u64) -> std::path::PathBuf {
        self.dir.join("import").join(format!("import{txn_id}"))
    }

    /// Stages the blocks a batch of this import just completed.
    ///
    /// This is the memory bound of an import. `NewTxn::Import` writes each
    /// `DataBlock` to `<temp>/import<txn>/db_/tbl_/seg_/blk_/` as soon as it is
    /// full, releases its buffer, and moves the directories into the data dir
    /// when the transaction commits. Without it the whole file stays resident
    /// as parsed values: GIST 1M is 960 dimensions per row, so that is ~6.6 GB
    /// of text and the values built from it, and the import peaked at 14.5 GiB.
    ///
    /// `rows` is the batch the caller has just parsed, **already in the append
    /// buffer**. Reading the block back out of the buffer copied the same 8192
    /// rows a second time, which on a 960-dim column is another 30 MB of text
    /// on top of the batch's own - so the rows are taken from `rows` instead
    /// and only what crosses a block boundary stays buffered.
    ///
    /// Only a block whose 8192 slots are **all** present is written. A partial
    /// block must stay buffered: the commit writes it together with the record
    /// that makes it visible, and a half block staged here would be re-read as
    /// if it were the whole window.
    ///
    /// The staged files are not published anywhere: no catalog record, no fast
    /// rough filter. The commit adopts the directories and then writes the
    /// table's blocks, which is what gives them both.
    /// Stages the blocks this run of rows holds **complete**, from the bytes
    /// the reader already produced.
    ///
    /// The rows it stages never become `Value`s at all: on a 960-dimension
    /// column that is the difference between a 19 KB `String` and a 3.8 KB slot
    /// per row, and the import's peak is dominated by it. A row outside a
    /// complete block (a block still filling, the statement's tail) is left to
    /// the caller, which stages it through text.
    ///
    /// `done` is the batch position `slots` starts at, `take` how many rows of
    /// that batch this run covers, and `base` the row id of `slots[done]`.
    pub(crate) fn stage_complete_slot_blocks(
        &self,
        table: &str,
        done: usize,
        take: usize,
        base: usize,
        slots: &[Option<Vec<Option<Vec<u8>>>>],
        root: &std::path::Path,
    ) -> Result<Vec<usize>> {
        if slots.is_empty() || take == 0 {
            return Ok(Vec::new());
        }
        let capacity = storage::definition::format::DEFAULT_BLOCK_CAPACITY;
        // Which blocks this run fills completely: every slot present, starting
        // at the block's first slot.
        let mut blocks: std::collections::BTreeMap<(u32, u16), usize> = Default::default();
        for k in 0..take {
            let (segment, block) = crate::blocks::block_of(base + k);
            *blocks.entry((segment, block)).or_default() += 1;
        }
        let types = self.table_schema(table).unwrap_or_default();
        let shape = crate::fast_rough_filter::Filters::new(&types);
        // The block's fast rough filter needs a `Value` per tracked cell. Fixed
        // width columns can give one from their slot (`decode_scalar_value`), so
        // a table of `INT`/`FLOAT`/`BIGINT` takes this path too; a column the
        // filter tracks but whose slot is not a scalar (`VARCHAR`, a date) would
        // have to go back through text, so the caller keeps the text path there.
        let can_decode = types.iter().enumerate().all(|(col, ty)| {
            !shape.tracks(col)
                || crate::blocks::column_type_of(ty).is_some_and(|t| (1..=8).contains(&t) || t == 0)
        });
        if !can_decode {
            return Ok(Vec::new());
        }
        let mut written: Vec<usize> = Vec::new();
        for ((segment, block), count) in blocks {
            let block_base = ((segment as usize) << 32) | (block as usize * capacity);
            // The block must be both complete and entirely inside this run.
            if count != capacity || block_base < base || block_base + capacity > base + take {
                continue;
            }
            let first = block_base - base + done;
            let rows: Vec<(usize, crate::blocks::RowCells<'_>)> = (0..capacity)
                .filter_map(|slot| {
                    let seq = block_base + slot;
                    let slot_bytes = slots.get(first + slot)?.clone()?;
                    Some((seq, crate::blocks::RowCells::Slots(slot_bytes)))
                })
                .collect();
            if rows.len() != capacity {
                // A record the reader could not produce: leave the block to the
                // text path rather than write a hole.
                continue;
            }
            // The filter's bounds, from the slots: a fixed-width column's slot
            // decodes straight to the `Value` the filter compares, so nothing
            // goes back through text. Columns the filter ignores contribute
            // nothing, which is every column of a pure vector table.
            let mut filter = crate::fast_rough_filter::Filters::new(&types);
            if !filter.tracks_nothing() {
                for (_, cells) in &rows {
                    if let crate::blocks::RowCells::Slots(slot_bytes) = cells {
                        filter.add_row(&decode_slot_values(&types, slot_bytes));
                    }
                }
            }
            self.write_import_block(table, root, segment, block, &rows)?;
            let mut tables = self.tables.lock().unwrap();
            tables
                .entry(self.table_key(table))
                .or_default()
                .staged_import_blocks
                .insert(
                    (segment, block),
                    crate::blocks::StagedImportBlock {
                        rows: capacity,
                        filter,
                    },
                );
            written.extend(
                (block_base..block_base + capacity).filter(|seq| (base..base + take).contains(seq)),
            );
        }
        if !written.is_empty() {
            self.discard_buffered_rows(table, &written);
        }
        Ok(written)
    }

    pub(crate) fn stage_complete_import_blocks_with<F>(
        &self,
        table: &str,
        rows: &[(usize, Vec<Value>)],
        root: &std::path::Path,
        mut slots: F,
    ) -> Result<Vec<usize>>
    where
        F: FnMut(usize) -> Option<Vec<Option<Vec<u8>>>>,
    {
        if rows.is_empty() {
            return Ok(Vec::new());
        }
        // Group by block; a block is written only when every one of its slots
        // is present, so the file always describes the window the commit will
        // read it as.
        type BlockKey = (u32, u16);
        type BlockRows<'a> = Vec<(usize, &'a Vec<Value>)>;
        let mut by_block: std::collections::BTreeMap<BlockKey, BlockRows<'_>> = Default::default();
        for (seq, values) in rows {
            by_block
                .entry(crate::blocks::block_of(*seq))
                .or_default()
                .push((*seq, values));
        }
        let capacity = storage::definition::format::DEFAULT_BLOCK_CAPACITY;
        let types = self.table_schema(table).unwrap_or_default();
        let mut written: Vec<usize> = Vec::new();
        for ((segment, block), chunk) in by_block {
            let base = ((segment as usize) << 32) | (block as usize * capacity);
            let full = chunk.len() == capacity
                && chunk
                    .iter()
                    .enumerate()
                    .all(|(slot, (seq, _))| *seq == base + slot);
            if !full {
                continue;
            }
            // The fast rough filter is built from the rows, which are about to
            // leave the buffer: build it now and hand it to the commit, or the
            // commit would have to decode the block it just wrote.
            let mut filter = crate::fast_rough_filter::Filters::new(&types);
            for (_, values) in &chunk {
                filter.add_row(values);
            }
            self.write_import_block(
                table,
                root,
                segment,
                block,
                &chunk
                    .iter()
                    .map(|(seq, values)| {
                        (*seq, crate::blocks::RowCells::Borrowed(values), slots(*seq))
                    })
                    .map(|(seq, cells, slot)| {
                        (
                            seq,
                            match slot {
                                Some(slots) => crate::blocks::RowCells::Slots(slots),
                                None => cells,
                            },
                        )
                    })
                    .collect::<Vec<_>>(),
            )?;
            let mut tables = self.tables.lock().unwrap();
            tables
                .entry(self.table_key(table))
                .or_default()
                .staged_import_blocks
                .insert(
                    (segment, block),
                    crate::blocks::StagedImportBlock {
                        rows: chunk.len(),
                        filter,
                    },
                );
            written.extend(chunk.iter().map(|(seq, _)| *seq));
        }
        if !written.is_empty() {
            self.discard_buffered_rows(table, &written);
        }
        Ok(written)
    }

    /// Removes rows from the append buffer.
    ///
    /// Used by the import path twice: after a full block is staged (its values
    /// are in the file now) and after a commit that failed (its rows must not
    /// become visible in a later block write).
    pub(crate) fn discard_buffered_rows(&self, table: &str, ids: &[usize]) {
        let key = self.table_key(table);
        let drop: std::collections::HashSet<usize> = ids.iter().copied().collect();
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&key) else {
            return;
        };
        entry.retain_buffered(|(seq, _)| !drop.contains(seq));
        entry.pending_bytes = entry.pending_rows.bytes();
    }

    /// Forgets the blocks an import staged, after the commit published them.
    pub(crate) fn forget_staged_import_blocks(&self, table: &str) {
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.staged_import_blocks.clear();
        }
    }

    pub(crate) fn import_command(&self, table: &str, segment: u32) -> Result<storage::WalCmd> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let (db_name, table_name) = split_table_name(table);
        let rows = self.segment_rows(table, segment as usize);
        Ok(storage::WalCmd::Import {
            db_name,
            db_id: self.database_id_of(table).to_string(),
            table_name,
            table_id: meta.id.to_string(),
            segment_info: self.wal_segment_info(segment, rows, meta.columns.len(), true),
        })
    }

    /// Commits one `COPY` transaction: `ImportV2` per segment it wrote
    /// (`ImportTxnStore::ToWalEntry`, `base_txn_store_impl.cpp:378`), never the
    /// rows, which are already in the blocks the import wrote.
    pub(crate) async fn commit_import_chunk(
        &self,
        table: &str,
        mut t: Txn,
        segments: &[u32],
        staged_rows: &crate::engine::import::StagedRows,

        batches: &[usize],
    ) -> Result<u64> {
        let mut cmds = Vec::new();
        for segment in segments {
            cmds.push(self.import_command(table, *segment)?);
        }
        // The dump runs inside the import transaction, so its `DumpIndexV2`
        // rides in the same frame (`ImportTxnStore::ToWalEntry:387`).
        let table_key = self.table_catalog_key(table)?;
        for segment in segments {
            let rows = self.segment_rows(table, *segment as usize);
            cmds.extend(
                self.dump_index_commands(table, *segment, rows, &table_key)
                    .await?,
            );
        }
        if !cmds.is_empty() {
            t.set_typed_commands(cmds);
        }
        t.set_op(TxnOp::Import {
            table: table.to_string(),
        });
        // `PrepareCommitImport` -> `CommitBottomAppend`: the blocks and the
        // `create` boundaries they carry belong to the transaction, so they are
        // written **after** it commits - a statement that loses its commit
        // leaves no rows, no extent and no `version` boundary behind (the
        // reference deletes the segment directory in `PostRollback`).
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        // `NewTxnManager::CommitTxn`: conflict check -> `PrepareCommitImport`
        // -> WAL append+fsync -> `CommitBottom`. The prepare phase is where the
        // reference writes the segment/block records and the block `version`
        // (`CommitSegmentVersion`, `new_txn_data_impl.cpp:2154`), i.e. the
        // bytes the frame names are on disk **before** the frame. The port wrote
        // them afterwards, so a kill in between left a segment record naming
        // blocks that never got written and that slice's rows were gone.
        let segments_owned: Vec<u32> = segments.to_vec();
        let batches_owned: Vec<usize> = batches.to_vec();
        let table_owned = table.to_string();
        let txn_id = t.id;
        // The append records are created here (the WAL frame names them) but
        // **stamped after the rows are published**. The reference keeps a
        // block's `created_` fields and its rows in one object, so a reader
        // that sees the ts also sees the data; here the record would otherwise
        // carry a commit ts while its rows were still in flight, and a
        // checkpoint in that window sized a block from ids that held nothing
        // ("block 3.0 holds 0 of the 99 rows committed at ts 22").
        let mut records_owned: Vec<usize> = Vec::with_capacity(batches_owned.len());
        let ts = self
            .txn
            .commit_at_deferred_with(t, ts, async |_txn| {
                let table = table_owned.as_str();
                for rows in &batches_owned {
                    records_owned.push(self.note_append(table, *rows, true, None));
                }
                for segment in &segments_owned {
                    // The extent is what marks the segment sealed, and a sealed
                    // segment is the only one whose blocks (and segment) get a fast
                    // rough filter - so it has to be recorded **before** the block
                    // write.
                    let rows = self.segment_rows(table, *segment as usize) as u64;
                    self.note_import_rows(table, *segment, rows);
                    self.note_import_ts(table, *segment, ts);
                }
                Ok(())
            })
            .await?;
        // The published rows and the block they land in are one step: the
        // block's bitmap coverage comes from the append journal, so a
        // checkpoint between them would see a journal that already had these
        // rows while writing a block from the buffer before them (one row
        // off).
        //
        // This runs **after** the commit and **before** `BottomGuard` (which
        // is what publishes), so the block is still written before the
        // transaction is visible - the reference's `CommitBottomAppend` order.
        //
        // **Every exit after the commit must finish the bottom**, success or
        // failure. The commit registered `ts` in `bottom_txns` (pending), and
        // `finish_bottom` only advances the visible clock from the *front* of
        // that queue - so a `?` here (the block write, a cancelled index
        // populate) would leave the entry pending forever and stall visibility
        // for every later commit on every table, not just this one. The effects
        // are therefore a block: its result is applied after `finish_bottom`,
        // the same shape `run_commit_bottom`'s statement path uses ("the
        // publish runs even when the effects fail, or the commit barrier would
        // wait for a transaction that already owns a timestamp").
        //
        // The order is what keeps that honest: `finish_bottom` runs only after
        // the block write and the index population have returned, so a row
        // whose block never made it on disk is not published by this call -
        // it stays unreachable, which is what it was before the statement
        // started.
        // It is outside `before_wal` because the gate has to be awaited: inside
        // that callback (a synchronous `FnOnce` running on a runtime worker
        // that already holds the commit turn and `commit_lock`) acquiring it
        // would have to bridge with `block_on`, parking a worker that holds a
        // `tokio::sync::Mutex` guard.
        // A guard for the *cancellation* path only. The `effects` block below
        // covers a returned error, but a dropped future (the statement is
        // cancelled while the block write or the index populate runs) leaves no
        // chance to run code after it - and that entry would block the visible
        // clock just the same. `BottomGuard` finishes on `Drop`, so the wait
        // cannot outlive the future.
        //
        // It does **not** publish early: `finish_bottom` only advances the
        // clock past a transaction when every earlier one is done, and the
        // block write and the index populate complete before the guard is
        // armed again below (`mem::forget` keeps the drop from firing a second
        // time there - harmless, since `finish_bottom` is idempotent, but it
        // keeps the intent readable).
        let bottom_guard = BottomGuard::new(&self.txn, ts);
        let effects: Result<()> = async {
            let gate = self.bottom_gate(table);
            let _turn = gate.lock().await;
            let _held = crate::engine::statements::HeldBottom::enter(&self.table_key(table));
            // The rows are replayed from the staged set a batch at a time (the
            // text may be on disk), but the block write still happens once, for
            // the whole statement: the block's `created` boundaries come from
            // the append journal, which carries every batch of this transaction,
            // so a write that saw only one batch would size a block from rows it
            // was never handed ("block 0.0 holds 8191 of the 8192 rows committed
            // at ts 10").
            self.publish_staged_rows(table, staged_rows, &batches_owned, |_batch| Ok(()))?;
            // The rows are readable now, so the records may carry this
            // transaction's ts (`BlockVersion::Append(commit_ts, …)`).
            for record in &records_owned {
                self.settle_append_at(table, *record, ts);
            }
            // `AppendInBlock` / `CommitBottomAppend`: the reference writes each
            // block inside the import transaction, so a `COPY` leaves readable
            // block files (and `SHOW BLOCKS` sees them) without a checkpoint. A
            // table whose columns the block format cannot express has no mirror
            // - it keeps its rows in the store.
            if self.has_block_layout(table) {
                // Blocks this statement completed while it ran are staged;
                // moving them in is what makes them readable, and the buffer
                // only holds the incomplete tail.
                let staging = self.import_staging_root(txn_id);
                if let Err(err) = self.adopt_import_staging(&staging) {
                    // Falling back to writing the remaining rows keeps the
                    // statement correct: the buffered tail is written below,
                    // and a staging directory that could not be moved is
                    // recovered on the next open from the WAL extent.
                    eprintln!("import: adopting staged blocks failed: {err}");
                }
                self.write_blocks_import(table, ts)?;
            }
            // The staged blocks are blocks of the table now; the commit's own
            // write above has given them their catalog records.
            self.forget_staged_import_blocks(table);
            // Async: the staging root can hold every block a large `COPY`
            // spilled, and removing it synchronously would hold the worker
            // (§7.2).
            let _ = tokio::fs::remove_dir_all(self.import_staging_root(txn_id)).await;
            // `PopulateIndex(…, DumpIndexCause::kImport)` for every index and
            // every segment this statement wrote - inside the transaction, with
            // the import's own ts, exactly as `NewTxn::Import` does before it
            // returns. Without this each index is chunked by whichever *append*
            // trigger fires first (the row capacity, or the memory quota's
            // watermark, which reads every tail in the engine), so a 150k-row
            // `COPY` ends up with the chunks of whatever batch boundaries the
            // writer happened to reach - not the reference's one per segment.
            //
            // **Unbounded read**: the builders scan the rows, and a statement's
            // own `read_ts` does not include the rows this commit just wrote
            // (they were published into the buffer under `ts`, which is at or
            // above the statement's snapshot). The reference's `PopulateIndex`
            // reads the segment's blocks directly and has no such bound, so the
            // populate runs at `u64::MAX` - the same visibility the index
            // maintenance paths use. The table's gate is held, so no other
            // writer can publish into the window this opens.
            crate::engine::state::with_read_ts_override_async(u64::MAX, async {
                // Boxed: `populate_import_indexes` awaits a full seal per index,
                // and inlining it into `commit_import_chunk` - itself inlined
                // into the statement chain - overflowed the worker's stack
                // (AGENTS.md §7.7).
                let populated: std::pin::Pin<
                    Box<dyn std::future::Future<Output = ()> + Send + '_>,
                > = Box::pin(self.populate_import_indexes(table, &segments_owned, ts));
                populated.await;
            })
            .await;
            Ok(())
        }
        .await;
        // The block write and the populate have returned, so the rows are on
        // disk: finishing here (rather than before them) is what keeps a
        // half-written row out of a reader's view. `effects?` then decides
        // whether the statement reports success.
        std::mem::forget(bottom_guard);
        self.txn.finish_bottom(ts);
        effects?;
        let _visible = BottomGuard::new(&self.txn, ts);
        // `CommitBottomAppend` for the imported segments on the table's worker.
        let bottom_table = table.to_string();
        let shard_key = bottom_table.clone();
        self.run_commit_bottom(&shard_key, move |engine: &Engine| {
            Box::pin(async move {
                engine.dump_hnsw_over_capacity(&bottom_table).await;
                engine.dump_secondary_over_capacity_blocking(&bottom_table);
                Ok(())
            })
        })
        .await?;
        Ok(ts)
    }

    /// Moves the storage between modes (C++ `Storage::SetStorageMode`).
    ///
    /// Only the mode and its legality are modelled: the reference rebuilds the
    /// WAL/catalog/processors on each transition, while this port built them
    /// once in [`Self::open_with_options`] (see [`StorageMode`]).
    pub fn set_storage_mode(&self, mode: StorageMode) -> Result<()> {
        self.storage.set_storage_mode(mode)
    }

    /// Binds an object store for the C++-format data files.
    pub fn set_object_store(&self, client: Arc<storage::s3::S3Client>) {
        *self.context.object_store.lock().unwrap() = Some(client);
    }

    /// Sets the buffer budget (the reference's `[buffer] buffer_manager_size`).
    ///
    /// The reference spends it on every `BufferObject`, data blocks included.
    /// Sealed blocks are mapped here, so the whole budget belongs to the
    /// in-memory index buffers, as the reference spends it on the index side.
    pub fn set_buffer_manager_size(&self, bytes: usize) {
        self.set_buffer_budget(bytes);
    }

    /// Sets `[wal] wal_flush`: when a commit's WAL entry reaches the disk
    /// (`flush_at_once` / `only_write` / `flush_per_second`).
    pub fn set_wal_flush(&self, mode: txn::WalFlush) {
        self.txn.set_wal_flush(mode);
    }

    /// Sets the decoded-block budget (C++ `BufferManager`'s
    /// `buffer_manager_size`), which is also the HNSW chunk budget.
    ///
    /// The port has no BufferManager - sealed blocks are mapped and evicted by
    /// the kernel - but the same number is the ceiling on decoded HNSW chunks
    /// and on the block map, so a deployment that sets a small value still gets
    /// a small footprint instead of the built-in 8 GiB.
    pub fn set_buffer_budget(&self, bytes: usize) {
        self.context
            .buffer_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
        self.context
            .hnsw_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
        // Apply it to what is already mapped: opening the catalog maps every
        // block's `version`, so a budget set after `open()` would otherwise
        // only take effect when a *new* block is mapped - and those mappings
        // are hits forever after.
        self.context
            .block_maps
            .lock()
            .unwrap()
            .trim_to(bytes.max(1));
    }

    /// Sets the process RSS ceiling (`0` = unset).
    ///
    /// The port has no BufferManager, so sealed blocks are page cache the kernel
    /// evicts under pressure - but a container's `memory_limit` kills before the
    /// kernel gets the chance. When the cleanup pass sees the process above this,
    /// it hands mapped pages back (`madvise`/`fadvise`) whether or not the index
    /// that owns them went idle.
    pub fn set_memory_limit(&self, bytes: u64) {
        self.context
            .memory_limit
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
        // The measured pressure is reused for half a second, so a ceiling that
        // just changed has to invalidate it - otherwise lowering the limit leaves
        // the old verdict in place while writes keep landing.
        self.context
            .memory_checked_at
            .store(0, std::sync::atomic::Ordering::Relaxed);
    }

    /// Builds the **first** chunk of a spann segment on the background task
    /// instead of the checkpoint.
    ///
    /// Off by default: a synchronous build makes the index readable as soon as
    /// the statement returns. An asynchronous one returns immediately, keeps
    /// answering from the memory tree (or the exact scan), and publishes the
    /// chain when `SpannRebuildTask` finishes (its progress is
    /// `spann_build_progress` in `SHOW INDEX`). A checkpoint's delta stays
    /// inline either way - it is the new rows, not a rebuild.
    pub fn set_spann_async_build(&self, async_build: bool) {
        self.context
            .spann_async_build
            .store(async_build, std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets the two byte budgets a delta checkpoint's `PageTree` runs under:
    /// cached clean pages, and the dirty pages that are the delta itself.
    ///
    /// The dirty budget is the one with a visible consequence: an insert that
    /// would spend past it is refused (`PageTreeError::Full`), the rows go back
    /// to the tail, and the **next** checkpoint publishes them. The defaults
    /// (64 MiB / 512 MiB) are sized for a real segment; a test that wants the
    /// refusal picks a few kilobytes.
    pub fn set_spann_page_budgets(&self, page_bytes: usize, dirty_bytes: usize) {
        self.context
            .spann_page_budget
            .store(page_bytes.max(1), std::sync::atomic::Ordering::Relaxed);
        self.context
            .spann_dirty_budget
            .store(dirty_bytes.max(1), std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets `[buffer] unsealed_memory_quota` (bytes).
    ///
    /// This is the budget for **everything not yet on disk**: the append buffer
    /// plus every in-memory index tail. Crossing it drives two things that run
    /// the same drain - the write path's quota check and the periodic loop - so
    /// the two can never disagree about what "over" means. `0` turns the whole
    /// mechanism off.
    pub fn set_unsealed_memory_quota(&self, bytes: u64) {
        self.set_memindex_quota(bytes)
    }

    /// The old name for [`Self::set_unsealed_memory_quota`].
    ///
    /// Kept because the field it writes (`Context::memindex_quota`) is still the
    /// engine's one byte ledger; callers that predate the rename keep working.
    pub fn set_memindex_quota(&self, bytes: u64) {
        self.context
            .memindex_quota
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets `[storage] mem_index_capacity` (rows). The reference dumps an
    /// in-memory index as soon as it holds this many rows; `0` disables the
    /// row trigger.
    pub fn set_memindex_capacity(&self, rows: usize) {
        self.context
            .memindex_capacity
            .store(rows, std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets `[buffer] unsealed_reclaim_interval` (seconds): how often the
    /// staleness sweep runs. `0` disables that path.
    pub fn set_unsealed_reclaim_interval(&self, secs: u64) {
        self.context.unsealed_reclaim_interval.store(
            secs.saturating_mul(1000),
            std::sync::atomic::Ordering::Relaxed,
        );
    }

    /// Sets `[buffer] unsealed_max_tables_per_round`. `0` means unlimited.
    pub fn set_unsealed_max_tables_per_round(&self, tables: usize) {
        self.context
            .unsealed_max_tables_per_round
            .store(tables, std::sync::atomic::Ordering::Relaxed);
    }

    /// `[buffer] unsealed_reclaim_interval` in milliseconds, read back by the
    /// CLI so an absent key falls through to the engine default instead of
    /// becoming `0` (which would disable the sweep).
    pub fn unsealed_reclaim_interval_ms(&self) -> u64 {
        self.context
            .unsealed_reclaim_interval
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// `SET VARIABLE <name> <value>` at `scope`.
    ///
    /// C++: `PhysicalCommand::ExecuteSet` (`physical_command_impl.cpp:68-150`).
    /// Every session variable is read-only; of the global variables only
    /// `profile` (bool) and `profile_record_capacity` (non-negative int) are
    /// settable, and an unknown name is `kInvalidCommand` (3076). Setting
    /// `result_cache`/the other knobs is `SET CONFIG`, which this engine
    /// already serves.
    pub fn set_variables(&self, scope: Scope, values: &[(String, String)]) -> Result<()> {
        for (name, value) in values {
            let name = name.to_ascii_lowercase();
            match scope {
                Scope::Session => {
                    if variables::SessionVariable::from_name(&name).is_none() {
                        return Err(Error::Invalid(format!("Unknown session variable: {name}")));
                    }
                    return Err(Error::Invalid(format!(
                        "Session variable: {name} is read-only"
                    )));
                }
                Scope::Global => {
                    let Some(var) = variables::GlobalVariable::from_name(&name) else {
                        return Err(Error::Invalid(format!("unknown global variable {name}")));
                    };
                    match var {
                        variables::GlobalVariable::EnableProfile => {
                            let enabled = match value.to_ascii_lowercase().as_str() {
                                "true" | "on" | "1" => true,
                                "false" | "off" | "0" => false,
                                other => {
                                    return Err(Error::Invalid(format!(
                                        "Global variable: {name} expects a boolean, got '{other}'"
                                    )))
                                }
                            };
                            self.profile_enabled
                                .store(enabled, std::sync::atomic::Ordering::Relaxed);
                        }
                        variables::GlobalVariable::ProfileRecordCapacity => {
                            let capacity: i64 = value.parse().map_err(|_| {
                                Error::Invalid(format!(
                                    "Global variable: {name} expects an integer, got '{value}'"
                                ))
                            })?;
                            if capacity < 0 {
                                return Err(Error::Invalid(format!(
                                    "Try to set profile record capacity with invalid value {capacity}"
                                )));
                            }
                            self.queries.lock().unwrap().resize(capacity as usize);
                        }
                        _ => {
                            return Err(Error::Invalid(format!(
                                "Global variable: {name} is read-only"
                            )))
                        }
                    }
                }
            }
        }
        Ok(())
    }

    /// Accounts `count` rows about to be inserted into `table` against each
    /// HNSW memory index, and raises the reclaim flag when that crosses the
    /// budget.
    ///
    /// The reference dumps an index when its tracer reports the memory limit
    /// (`MemIndexTracer::IncreaseMemoryUsage`), and this port used to do the
    /// same inline. It no longer does: a dump here is a chunk write plus an
    /// index catalog scan **on the statement's latency**, and it can take
    /// another table's bottom gate while this one holds its own. The decided
    /// shape is one drain, on the trigger loop's tick (`reclaim_unsealed`), so
    /// the write path only keeps the charge current and raises
    /// [`Context::unsealed_reclaim_requested`].
    ///
    /// The separate **row-capacity** bound (`dump_hnsw_over_capacity`) is
    /// untouched: it is a different question - how many rows one in-memory
    /// graph may hold - and it is what keeps a long import's footprint flat.
    pub(crate) fn note_index_insert(&self, table: &str, count: usize) {
        let quota = self
            .context
            .memindex_quota
            .load(std::sync::atomic::Ordering::Relaxed) as usize;
        if quota == 0 || count == 0 {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            // The index metadata may hold either a qualified or a short table
            // name, so match fully when both are qualified and by short name
            // when either side is unqualified.
            let same_table = im.table == table
                || (im.table.rsplit('.').next() == table.rsplit('.').next()
                    && (!im.table.contains('.') || !table.contains('.')));
            if !same_table {
                continue;
            }
            let Some(col) = schema.iter().position(|c| c == &im.column) else {
                continue;
            };
            let dim = types.get(col).and_then(|t| type_dim(t)).unwrap_or(0);
            // One node costs its vector plus the level-0 neighbour list
            // (`Mmax0 = 2M`, four bytes per id) and a little bookkeeping:
            // the reference charges 168 bytes per row for `dim = 4, M = 16`
            // (`IncreaseMemoryUsage mem_inc 1008` for six rows).
            let bytes = (dim * 4 + 152) * count;
            let tkey = self.table_key(&im.table);
            // Only the charge moves. The reference subtracts what a dump
            // released and leaves the rest accounted, so the counter is a
            // running total; the scan recomputes it when it drains.
            if let Some(entry) = self.tables.lock().unwrap().get_mut(&tkey) {
                *entry.index_pending_bytes.entry(im.id).or_insert(0) += bytes;
            }
        }
        // One atomic read after the charge update, not per index: the scan's
        // own check is authoritative, this only has to wake it up.
        if (self.budget_bytes() as u64) > self.reclaim_target() {
            self.context
                .unsealed_reclaim_requested
                .store(true, std::sync::atomic::Ordering::Relaxed);
        }
    }
}

/// The values of one row whose cells are already its block-file bytes.
///
/// Only the columns a [`crate::fast_rough_filter::Filters`] tracks matter; a
/// column it ignores (an `EMBEDDING`, a `TENSOR`) becomes `Value::Null`, which
/// `add_row` skips. Every scalar fixed-width type decodes through the same
/// `decode_scalar_value` the block reader uses, so the bounds match the ones
/// the text path would have derived.
fn decode_slot_values(types: &[String], slots: &[Option<Vec<u8>>]) -> Vec<Value> {
    slots
        .iter()
        .enumerate()
        .map(|(col, slot)| {
            let (Some(slot), Some(ty)) = (slot.as_deref(), types.get(col)) else {
                return Value::Null;
            };
            let Some(column_type) = crate::blocks::column_type_of(ty) else {
                return Value::Null;
            };
            if column_type == 0 {
                return Value::Bool(slot.first().copied().unwrap_or(0) != 0);
            }
            crate::blocks::decode_scalar_value(column_type, slot.len(), slot, 0)
                .unwrap_or(Value::Null)
        })
        .collect()
}

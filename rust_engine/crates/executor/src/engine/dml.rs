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
    base: usize,
    columns: Vec<(String, Vec<Option<PreparedTerms>>)>,
}

impl PreparedText {
    /// Points the batch at the row range the commit allocated.
    pub(crate) fn set_base(&mut self, base: usize) {
        self.base = base;
    }

    /// The terms for `seq`, taken out of the batch. `None` when the position is
    /// outside the batch or another pass already took it.
    fn take(&mut self, column: &str, seq: usize) -> Option<PreparedTerms> {
        let position = seq.checked_sub(self.base)?;
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
struct PublishTicket<'a> {
    index: &'a std::sync::RwLock<crate::engine::text_index::TextIndex>,
    from_row: u64,
}

impl<'a> PublishTicket<'a> {
    fn announce(
        index: &'a std::sync::RwLock<crate::engine::text_index::TextIndex>,
        from_row: u64,
    ) -> Self {
        if let Ok(mut guard) = index.write() {
            guard.announce_publish(from_row);
        }
        Self { index, from_row }
    }
}

impl Drop for PublishTicket<'_> {
    fn drop(&mut self) {
        if let Ok(mut guard) = self.index.write() {
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
        // One chunk per **block**, as the reference does: the mem indexer is
        // handed a block at a time (`InsertBlockData`,
        // `secondary_index_in_mem_impl.cpp:104`) and dumps only that block into
        // the chunk. The offsets stay segment-relative - the reader ORs every
        // chunk's hits into a segment-wide bitmask (`TrunkReaderT::OutPut`,
        // `index_filter_evaluators_impl.cpp:912`) - so only the split point
        // changes here, not the values.
        let block_rows = storage::format::DEFAULT_BLOCK_CAPACITY as u64;
        let mut readers = Vec::new();
        let mut meta: Vec<(u64, usize)> = Vec::new();
        for (segment, entries) in by_segment {
            let mut by_block: std::collections::BTreeMap<u64, Vec<_>> = Default::default();
            for entry in entries {
                by_block
                    .entry(u64::from(entry.1) / block_rows)
                    .or_default()
                    .push(entry);
            }
            for (block, mut entries) in by_block {
                // `Key::cmp` is the chunk's own order: u64 for integers and
                // hashed text, the float order for a FLOAT/DOUBLE column.
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
                // `AddNewChunkIndex1`.
                let record = catalog::layout::IndexChunk {
                    chunk_id: next,
                    base_name: String::new(),
                    base_row_id: (segment << 32) | (block * block_rows),
                    row_count: entries.len() as u64,
                    term_count: 0,
                    index_size: 0,
                    commit_ts: self.txn.current_ts().max(1),
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
                    // The chunk's offsets are already segment-relative, so the
                    // base is the segment alone; its `ChunkIndexMetaInfo` covers
                    // the rows the block held.
                    meta.push((segment << 32, entries.len()));
                    readers.push((segment << 32, Arc::new(reader)));
                }
            }
        }
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.disk.insert(index_id, readers);
        // A rebuild replaces the whole chunk set, so its meta replaces the list
        // a previous dump appended to.
        entry.secondary.chunk_meta.insert(index_id, meta);
    }

    /// Appends a committed batch to the decoded-row cache, if the table has one.
    pub(crate) fn append_row_cache(&self, table: &str, base: usize, encoded: &[String]) {
        let key = self.table_key(table);
        if !self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.row_cache.is_some())
            .unwrap_or(false)
        {
            return;
        }
        // Decode through the same path as a scan: the stored text is the
        // canonical rendering (e.g. a FLOAT column renders `1.000000` even when
        // the literal was `1`).
        let defaults = self.table_defaults(table).unwrap_or_default();
        let types = self.column_types(table).unwrap_or_default();
        let mut decoded = Vec::with_capacity(encoded.len());
        for text in encoded {
            match self.parse_stored_row(table, &defaults, &types, text.as_bytes()) {
                Ok(values) => decoded.push(values),
                Err(_) => return,
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
        if cache.rows.last().is_some_and(|(last, _)| *last >= base) {
            entry.row_cache = None;
            return;
        }
        for (k, values) in decoded.into_iter().enumerate() {
            cache.push(base + k, values);
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
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
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
        // Keep the lazily built lookup in step with the journal it mirrors, or
        // the rows this commit just deleted would read as live until the next
        // journal rebuild.
        if let Some(set) = entry.deleted_set.as_mut() {
            Arc::make_mut(set).extend(ids.iter().map(|s| *s as u64));
        }
        entry.mirror_dirty = true;
        entry.mirror_epoch += 1;
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
        let (source_columns, rows) = self.run_select(&query)?;
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
    pub fn insert_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        crate::scheduler::block_on(self.insert_literals_inner(table, columns, rows, false))
    }

    /// `NewTxn::Import`: like [`Self::insert_literals`], but the batch always
    /// opens a fresh segment (`TableMeta::GetSegmentIDs1` gains a new id).
    pub fn import_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        crate::scheduler::block_on(self.insert_literals_inner(table, columns, rows, true))
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
        // tensor-like values must match the declared dimension, and
        // sparse values must be in range
        for row in &rows {
            for (i, cell) in row.iter().enumerate() {
                if let Some(t) = types.get(i) {
                    validate_insert_value(t, cell.to_slt_string().trim())?;
                }
            }
        }
        // A commit changes visible rows, so cached `SELECT` results must not
        // survive it. The SQL path clears on the mutating statement; callers
        // that reach this primitive directly (Thrift insert, import) would
        // otherwise keep serving a stale `count(*)`.
        self.result_cache.lock().unwrap().clear();
        self.plan_cache.clear();
        // The analyzer is the most expensive CPU step of a write and takes no
        // shared state, so it runs **before** the table's write gate: a second
        // writer to the same table then tokenises while the gate's holder is
        // committing and appending its block, instead of queueing behind the
        // whole statement. The batch is capped at `TEXT_BATCH_ROWS` so a big
        // `COPY` keeps the bounded shape (it tokenises a chunk at a time inside
        // the gate).
        let (mut prepared_text, prepared_cells) = if rows.len() <= Self::TEXT_BATCH_ROWS {
            let cells: Vec<Vec<String>> = rows
                .iter()
                .map(|row| row.iter().map(|cell| cell.to_slt_string()).collect())
                .collect();
            (self.prepare_text_terms(table, &cells), Some(cells))
        } else {
            (None, None)
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
        let mut encoded_rows: Vec<String> = Vec::with_capacity(rows.len());
        for row in rows.iter() {
            // The rows are written by the bottom half, not by this batch: the
            // reference writes them in `AppendInBlock` and only then publishes,
            // so a reader never sees a row whose index entry is still missing.
            // Replay rebuilds the same keys from the WAL block
            // (`append_writes`).
            encoded_rows.push(self.encode_row_stored(table, row, &types)?);
        }
        // Emit the reference `WalCmdAppendV2` (real typed columns) when the
        // table's types are covered; otherwise the transaction writes the
        // text-column block through `build_commands`.
        // A block that does not carry one row per row range makes an `Append`
        // command replay cannot decode (`append_rows` needs row *n*); fall back
        // to the text record in that case rather than write such a frame.
        let typed = typed_append_block(&rows, &types);
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
            .commit_at_deferred_with(t, ts, |txn| {
                // Allocated **inline**: this callback runs on a runtime worker
                // that already holds the commit turn and `commit_lock`, so it
                // must not park (see `reserve_rows_at`).
                let range = self.reserve_rows_at(&alloc_table, alloc_rows, new_segment, ts)?;
                *allocated.lock().unwrap() = range;
                if let Some(block) = typed {
                    txn.set_typed_commands(vec![self.append_command(
                        &alloc_table,
                        &alloc_table_id,
                        range.base,
                        alloc_rows,
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
        let row_count = rows.len();
        let bottom_rows = rows;
        let bottom_encoded = encoded_rows;
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
        self.run_commit_bottom(&shard_key, move |engine| {
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
                    (base as u64 & 0xffff_ffff) + bottom_rows.len() as u64,
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
                let index_rows: Vec<(usize, Vec<String>)> = match prepared_cells {
                    // Reuse the cells the pre-gate tokeniser already rendered.
                    Some(cells) => cells
                        .into_iter()
                        .enumerate()
                        .map(|(k, cells)| (base + k, cells))
                        .collect(),
                    None => bottom_rows
                        .iter()
                        .enumerate()
                        .map(|(k, row)| (base + k, row.iter().map(|v| v.to_slt_string()).collect()))
                        .collect(),
                };
                // The index work runs on **this table's** bottom worker: every
                // index has its own lock and two tables share none, so a
                // directory with a hundred thousand tables keeps its index
                // updates in parallel instead of queueing behind one appender
                // thread (the reference's `MemIndexAppender` is a single
                // thread, which is the one structural deviation here).
                let schema = engine.table_schema(&index_table).unwrap_or_default();
                // One pass for the whole batch: the per-row form resolved the
                // table key and took the registry lock once per row.
                //
                // The cells come from `bottom_rows` (real `Value`s), not from
                // the rendered text: a `NULL` must reach the index as
                // `Value::Null`, because `value_to_tail_keys` skips it - a
                // string form would make it an ordinary key
                // (`SecondaryIndexInMemT::InsertInner` skips the null bit).
                let values: Vec<Vec<Value>> = bottom_rows
                    .iter()
                    .map(|row| row.iter().map(Value::from_literal).collect())
                    .collect();
                engine.update_secondary_indexes_batch(&index_table, base, &values, &schema);
                // One batch: the full-text terms of the whole append are
                // tokenised without the column lock and inserted under it once.
                let batch: Vec<(usize, &[String])> = index_rows
                    .iter()
                    .map(|(seq, texts)| (*seq, texts.as_slice()))
                    .collect();
                // The row ids are known now: point the pre-tokenised batch at
                // the allocated range. A column that gained an index after the
                // batch was tokenised is not covered and is tokenised here.
                if let Some(prepared) = prepared_text.as_mut() {
                    prepared.set_base(base);
                }
                engine.update_text_indexes_batch(
                    &index_table,
                    &batch,
                    &schema,
                    prepared_text.as_mut(),
                );
                for (seq, texts) in &index_rows {
                    engine.update_vector_caches(&index_table, *seq, texts);
                }
                // C++ `AppendMemIndex`: a segment that reached
                // `mem_index_capacity` rows is dumped right after the batch.
                engine.dump_hnsw_over_capacity(&index_table);
                engine.dump_secondary_over_capacity_blocking(&index_table);
            }
            // Fold this batch's index growth into the engine-wide total (plan
            // 4.2): one table's delta, not a scan of every table.
            engine.refresh_memindex_charge(&bottom_table);
            engine.bump_writes(&bottom_table);
            engine.note_live_rows(&bottom_table, bottom_rows.len() as isize);
            engine.append_row_cache(&bottom_table, base, &bottom_encoded);
            // No row key for the multi-value/tensor tables either: their WAL
            // frame is decoded back into row text by `block::to_data_block`
            // (tensor and sparse columns render their text form), so the block
            // is the only copy, as in the reference.
            // `AppendInBlock`: this batch's rows go into the block's buffer -
            // they are the batch's only copy, there is no `row:` key to read
            // them back from.
            let published: Vec<(usize, String)> = bottom_encoded
                .iter()
                .enumerate()
                .map(|(k, text)| (base + k, text.clone()))
                .collect();
            engine.publish_encoded_rows(&bottom_table, &published)?;
            // The rows are readable now (`AppendInBlock`), so the record may
            // carry this transaction's ts (`BlockVersion::Append(commit_ts, …)`).
            engine.settle_append_at(&bottom_table, append_record, ts);
            Ok(())
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
            self.enforce_quota_after_append(table).await;
        }
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
        base: usize,
        count: usize,
        block: Vec<storage::wal::block::BlockColumn>,
    ) -> storage::WalCmd {
        let (db_name, table_name) = split_table_name(table);
        storage::WalCmd::Append {
            db_id: self.database_id(&db_name).to_string(),
            db_name,
            table_name,
            table_id: table_id.to_string(),
            row_ranges: vec![(
                storage::wal_entry::WalRowId::from_u64(base as u64),
                count as u64,
            )],
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

    pub(crate) async fn commit_with_deletes(
        &self,
        table: &str,
        deletes: &[usize],
        appended: usize,
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
        // records the delete ts (and an UPDATE's rewritten rows' append ts).
        self.delete_bottom(table, deletes, Some(appended), extra.to_vec(), ts)
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
        self.run_commit_bottom(&shard_key, move |engine| {
            // The delete joins the table's log only now: a conflicting
            // transaction must leave no trace, and this closure runs after the
            // commit was accepted.
            let first_delete = engine.note_deleted_ids(&bottom_table, &bottom_deletes);
            // The slot is hidden by the block's `version` (`BlockVersion::
            // Delete`), which is what the reference has: no tombstone key is
            // written. `column_type_of` covers every type the DDL accepts, so
            // `has_block_layout` is true for every table that exists.
            // An UPDATE stages its rewritten rows as an append in the same
            // transaction (`reserve_rows` already logged the create), so the
            // block's `version` has to record this ts for them as well
            // (`BlockVersion::Append`).
            if let Some(count) = appended {
                engine.settle_append_ts(&bottom_table, count, ts);
            }
            // `BlockVersion::Delete(row_id, commit_ts)`: the slot carries the
            // deleting transaction's commit timestamp, and the block rewrite
            // marks it - there is no tombstone key to write.
            engine.settle_delete_at(&bottom_table, first_delete, delete_count, ts);
            engine.refresh_block_mirror_extra(&bottom_table, &bottom_rows)
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
    ) -> Result<usize>
    where
        F: FnOnce(&mut Txn, usize) -> Result<()>,
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
        let ts = self
            .txn
            .commit_at_deferred_with(txn, ts, |txn| {
                let range = self.reserve_rows_at(table, count, false, ts)?;
                *allocated.lock().unwrap() = range;
                prepare(txn, range.base)
            })
            .await?;
        let range = *allocated.lock().unwrap();
        let base = range.base;
        let _visible = BottomGuard::new(&self.txn, ts);
        // One copy of the rewritten rows: the block buffer takes them as
        // `Value`s, and the caller keeps its slice for the index hooks.
        let extra: Vec<(usize, Vec<Value>)> = rows
            .iter()
            .enumerate()
            .map(|(k, row)| (base + k, row.clone()))
            .collect();
        // The staged append record settles with its commit ts here, which is what
        // `committed_rows` reads; without it a checkpoint would not see the
        // rewritten rows (`reserve_rows_at` no longer stamps it).
        self.delete_bottom(table, deletes, Some(count), extra, ts)
            .await?;
        Ok(base)
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
            for row in rows.iter_mut() {
                let Some(text) = row.get(i).map(|v| v.to_slt_string()) else {
                    continue;
                };
                row[i] = Value::Str(match &analyzer {
                    Some(analyzer) => highlight_with_analyzer(&query, &text, analyzer),
                    None => highlight_text(&text, &qterms, &tokenizer.delims),
                });
            }
        }
    }

    /// Maintains declared indexes on insert.
    pub(crate) fn update_indexes_on_insert(
        &self,
        table: &str,
        seq: usize,
        values: &[Literal],
        schema: &[String],
    ) -> Result<()> {
        let targets = self.secondary_targets(table, schema);
        self.insert_secondary_keys(&targets, seq, values)
    }

    /// [`Self::update_indexes_on_insert`] with the targets already resolved, for
    /// a bulk load that resolves them once per batch.
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
    /// of decoding every key) and a dump can write its keys out directly. The
    /// tails are cloned out under the table lock and appended to outside it.
    pub(crate) fn update_secondary_indexes_batch(
        &self,
        table: &str,
        base: usize,
        rows: &[Vec<Value>],
        schema: &[String],
    ) {
        if rows.is_empty() {
            return;
        }
        let targets = self.secondary_targets(table, schema);
        if targets.is_empty() {
            return;
        }
        for (k, values) in rows.iter().enumerate() {
            let seq = (base + k) as u64;
            for target in &targets {
                let Some(value) = values.get(target.pos) else {
                    continue;
                };
                // A JSON document contributes one key per flattened term, a
                // functional index one per evaluated row and every other column
                // exactly one.
                let keys = match &target.func {
                    None => value_to_tail_keys(&target.declared, value),
                    Some(func) => match functional_value(func, target.pos, values) {
                        Some(value) => value_to_key(&target.declared, &value),
                        None => Vec::new(),
                    },
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
        let prefix = format!("{table}.");
        if self
            .vector_cache
            .lock()
            .unwrap()
            .keys()
            .any(|k| k.starts_with(&prefix))
        {
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
    pub(crate) fn note_hnsw_insert(&self, table: &str, seq: u64, cells: &[String]) {
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
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        // The table lock is only held to reach the per-segment mutexes; the
        // graph updates run under each segment's own lock.
        let segments: Vec<(String, Arc<Mutex<crate::hnsw_mem::HnswIndexInMem>>)> = {
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
                    return;
                };
                let Ok(types) = self.column_types(table) else {
                    return;
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
                    let opt = |name: &str| -> Option<usize> {
                        im.options
                            .iter()
                            .find(|(k, _)| k.eq_ignore_ascii_case(name))
                            .and_then(|(_, v)| v.parse().ok())
                    };
                    let m = opt("M").unwrap_or(16);
                    let efc = opt("EF_CONSTRUCTION").unwrap_or(200);
                    let opt = |name: &str| {
                        im.options
                            .iter()
                            .find(|(k, _)| k.eq_ignore_ascii_case(name))
                            .map(|(_, v)| v.clone())
                    };
                    let metric =
                        index::vector::Metric::parse(&opt("metric").unwrap_or_else(|| "l2".into()));
                    let encoding =
                        index::vector::HnswEncoding::parse(&opt("encode").unwrap_or_default());
                    entry.hnsw_mem.entry(im.id).or_insert_with(|| {
                        Arc::new(Mutex::new(crate::hnsw_mem::HnswIndexInMem::new_with(
                            dims, m, efc, metric, encoding,
                        )))
                    });
                }
            }
            // The rows are fed to every HNSW index on the table, each of which
            // keeps its own graph (the reference's per-`MemIndex` object). The
            // index id -> column map decides which cell feeds which graph.
            let by_id: std::collections::HashMap<u64, String> = self
                .storage
                .catalog
                .indexes_of_table(&self.qualified_table(table))
                .unwrap_or_default()
                .into_iter()
                .map(|im| (im.id, im.column))
                .collect();
            entry
                .hnsw_mem
                .iter()
                .filter_map(|(id, m)| Some((by_id.get(id)?.clone(), m.clone())))
                .collect()
        };
        for (column, mem) in segments {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
            if let Ok(mut mem) = mem.lock() {
                if v.len() == mem.dims {
                    mem.insert(seq, &v);
                }
            }
        }
    }

    pub(crate) fn update_vector_caches(&self, table: &str, seq: usize, cells: &[String]) {
        let prefix = format!("{table}.");
        let caches: Vec<(String, SharedVectorCache)> = self
            .vector_cache
            .lock()
            .unwrap()
            .iter()
            .filter(|(k, _)| k.starts_with(&prefix))
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect();
        // The HNSW/IVF memory indexes are maintained regardless of whether a
        // query cache exists.
        self.note_hnsw_insert(table, seq as u64, cells);
        self.note_ivf_insert(table, seq as u64, cells);
        if caches.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (key, shared) in caches {
            let column = &key[prefix.len()..];
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v = parse_vec_str(text);
            let mut cache = shared.write().unwrap();
            if v.len() != cache.dims {
                continue;
            }
            match cache.rows.iter_mut().find(|(s, _)| *s == seq) {
                Some(entry) => entry.1 = v,
                None => cache.rows.push((seq, v)),
            }
            cache.live.insert(seq);
        }
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
            }
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

    pub(crate) fn update_multivector_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let by_id = self.index_column_map(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<MultiVectorIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.mv.iter()
                    .filter_map(|(id, v)| Some((by_id.get(id)?.clone(), v.clone())))
                    .collect()
            })
            .unwrap_or_default();
        if indexes.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(dims) = types.get(ci).and_then(|t| type_dim(t)) else {
                continue;
            };
            if dims == 0 {
                continue;
            }
            let mut index = shared.write().unwrap();
            index.remove(seq as u64);
            // The row is seen whether or not it carries a tensor. A NULL cell
            // adds no PLAID document, but the coverage counter has to advance
            // anyway: `is_complete(reserved)` compares it with the table's live
            // rows, and a row it never counts keeps every later search
            // rebuilding the index from scratch.
            index.note_row();
            let Some(text) = cells.get(ci) else { continue };
            if let Some(doc) = parse_multivector_doc(text, dims) {
                index.insert(seq as u64, doc);
            }
        }
    }

    /// Keeps existing sparse indexes in step with an inserted/rewritten row.
    pub(crate) fn update_sparse_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let by_id = self.index_column_map(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<SparseIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.sparse
                    .iter()
                    .filter_map(|(id, v)| Some((by_id.get(id)?.clone(), v.clone())))
                    .collect()
            })
            .unwrap_or_default();
        if indexes.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let mut index = shared.write().unwrap();
            index.remove(seq as u64);
            // The index has seen this row whether or not it carries postings:
            // the coverage check counts rows, not documents (a NULL sparse cell
            // writes no term). A rewritten row counts twice, which only makes
            // the index look more complete than it is - it was just updated.
            index.note_row();
            let Some(text) = cells.get(ci) else { continue };
            // A NULL cell stores the literal `null`; it has no postings.
            if text.trim().eq_ignore_ascii_case("null") {
                continue;
            }
            let entries: Vec<(i32, f32)> = parse_sparse_str(text)
                .into_iter()
                .map(|(i, x)| (i as i32, x as f32))
                .collect();
            index.insert(seq as u64, entries);
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
    pub(crate) fn prepare_text_terms(
        &self,
        table: &str,
        cells: &[Vec<String>],
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
        let mut columns: Vec<(String, Vec<Option<PreparedTerms>>)> = Vec::new();
        for (column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let tokenizer = {
                let index = shared.read().unwrap();
                index.tokenizer.clone()
            };
            let rows = cells
                .iter()
                .map(|cells| {
                    let text = cells.get(ci)?;
                    let doc = prepare_doc(0, text, &tokenizer);
                    Some((doc.terms, doc.cost))
                })
                .collect();
            columns.push((column, rows));
        }
        if columns.is_empty() {
            return None;
        }
        Some(PreparedText { base: 0, columns })
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
    /// The synchronous form, for the commit's bottom half.
    ///
    /// The bottom job runs on a blocking-pool thread (or inline when no pool is
    /// attached), so the one wait it contains - the memindex quota's fallback
    /// dump - is bridged with `block_on`; that is a blocking thread by
    /// construction, which is exactly where a nested `block_on` is legal.
    /// Async callers use [`Self::update_text_indexes_batch_async`] instead.
    pub(crate) fn update_text_indexes_batch(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
        prepared: Option<&mut PreparedText>,
    ) {
        self.update_text_indexes_batch_inner(table, rows, schema, prepared);
        crate::scheduler::block_on(self.enforce_memindex_quota(Some(table)));
    }

    /// [`Self::update_text_indexes_batch`] for a caller that is already a task:
    /// the quota's fallback dump is an `.await` instead of a bridged wait.
    pub(crate) async fn update_text_indexes_batch_async(
        &self,
        table: &str,
        rows: &[(usize, &[String])],
        schema: &[String],
        prepared: Option<&mut PreparedText>,
    ) {
        self.update_text_indexes_batch_inner(table, rows, schema, prepared);
        self.enforce_memindex_quota(Some(table)).await;
    }

    /// The batch's work, without the quota check.
    fn update_text_indexes_batch_inner(
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
        for (seq, cells) in rows {
            self.update_multivector_indexes(table, *seq, cells);
            self.update_sparse_indexes(table, *seq, cells);
        }
        let tkey = self.table_key(table);
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
        for (column, shared) in indexes {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            // Phase 1, no lock held: snapshot the tokenizer, then tokenise.
            let tokenizer = {
                let index = shared.read().unwrap();
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
                    let mut index = shared.write().unwrap();
                    if index.unsealed_segment().is_some_and(|held| held != segment) {
                        // Crossed into the next `ROW_ID` segment: seal the old
                        // rows in place. This happens once per 2^32 rows, so the
                        // lock-held write costs nothing measurable.
                        index.seal();
                    }
                    for doc in &prepared {
                        index_delete_row(&mut index, doc.seq);
                    }
                    index_rows_prepared(&mut index, &prepared);
                    // The rows are in the tail: the barrier can lift before the
                    // seal below, which is this writer's own and covers exactly
                    // the rows it just published. (Through the same guard - the
                    // ticket's `Drop` would re-enter the lock.)
                    index.finish_publish(first_row);
                    let seal = index.should_seal(capacity);
                    drop(index);
                    if seal {
                        // The segment is written with the lock released: the
                        // tail moves to `TextIndex::inflight`, where queries keep
                        // reading it, so nobody waits for the fsync.
                        crate::engine::state::seal_text_index_unlocked(&shared);
                        // The chunk is on disk; its catalog record is what a
                        // reader goes by, so write it before the rows can be
                        // queried from a fresh build.
                        self.record_sealed_text_chunks(table, &column);
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
        self.context
            .buffer_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
        self.context
            .hnsw_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// `WalCmdImportV2` for the segment a `COPY` is writing
    /// (`ImportTxnStore::ToWalEntry`, `base_txn_store_impl.cpp:378`): the
    /// segment's info, never the rows.
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
        encoded_rows: &[(usize, String)],
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
            cmds.extend(self.dump_index_commands(table, *segment, rows, &table_key)?);
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
        let encoded_owned: Vec<(usize, String)> = encoded_rows.to_vec();
        let table_owned = table.to_string();
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
            .commit_at_deferred_with(t, ts, |_txn| {
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
        // It is outside `before_wal` because the gate has to be awaited: inside
        // that callback (a synchronous `FnOnce` running on a runtime worker
        // that already holds the commit turn and `commit_lock`) acquiring it
        // would have to bridge with `block_on`, parking a worker that holds a
        // `tokio::sync::Mutex` guard.
        {
            let gate = self.bottom_gate(table);
            let _turn = gate.lock().await;
            let _held = crate::engine::statements::HeldBottom::enter(&self.table_key(table));
            self.publish_encoded_rows(table, &encoded_owned)?;
            // The buffer holds the values now; the stored text of the whole
            // file (hundreds of MB) has no reader left.
            drop(encoded_owned);
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
                self.write_blocks_import(table, ts)?;
            }
        }
        let _visible = BottomGuard::new(&self.txn, ts);
        // `CommitBottomAppend` for the imported segments on the table's worker.
        let bottom_table = table.to_string();
        let shard_key = bottom_table.clone();
        self.run_commit_bottom(&shard_key, move |engine| {
            engine.dump_hnsw_over_capacity(&bottom_table);
            engine.dump_secondary_over_capacity_blocking(&bottom_table);
            Ok(())
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
        self.set_buffer_memory_limit(bytes);
    }

    /// Sets `[wal] wal_flush`: when a commit's WAL entry reaches the disk
    /// (`flush_at_once` / `only_write` / `flush_per_second`).
    pub fn set_wal_flush(&self, mode: txn::WalFlush) {
        self.txn.set_wal_flush(mode);
    }

    /// Sets `[buffer] memindex_memory_quota` (bytes). A memory index is dumped
    /// into a fresh C++ chunk once its buffered size reaches this quota; `0`
    /// keeps every row in memory until a checkpoint.
    /// Sets the decoded-block budget (C++ `BufferManager`'s
    /// `buffer_manager_size`).
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
    }

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
    /// HNSW memory index. The reference dumps an index when it reaches
    /// `mem_index_capacity` rows (or, in this port, the buffered-size quota)
    /// **before** taking the new rows, which is what this reproduces (one
    /// `chunk_{n}.idx` per dump).
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
            {
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey.clone()).or_default();
                let cur = entry.index_pending_bytes.entry(im.id).or_insert(0);
                // Only the bookkeeping happens here. The dump itself is
                // proposed **after** the rows are written
                // (`enforce_memindex_quota`, which `update_text_indexes_batch`
                // runs once the batch is indexed): proposing it before the
                // insert let the worker seal a graph that did not hold these
                // rows yet, so the batch produced no chunk - and in the full
                // suite, where the worker is slower, that lost one
                // (`test_mem_bmp`, `test_optimize_from_different_database`).
                *cur += bytes;
            }
        }
    }
}

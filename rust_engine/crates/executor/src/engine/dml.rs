use super::*;

impl Engine {
    /// Writes one `keys + offsets + PGM` chunk per `ROW_ID` segment and installs
    /// the readers (the reference's high-cardinality `SaveIndexInner`).
    pub(crate) fn write_secondary_pgm_chunks(
        &self,
        table: &str,
        column: &str,
        kind: storage::secondary_segment::KeyKind,
        by_segment: HashMap<u64, Vec<(storage::secondary_segment::Key, u32)>>,
    ) {
        use storage::secondary_segment::write_secondary_segment_typed;
        let dir = self
            .dir
            .join("secondary")
            .join(index_key(table, column).replace('/', "_"));
        // One chunk per **block**, as the reference does: the mem indexer is
        // handed a block at a time (`InsertBlockData`,
        // `secondary_index_in_mem_impl.cpp:104`) and dumps only that block into
        // the chunk. The offsets stay segment-relative - the reader ORs every
        // chunk's hits into a segment-wide bitmask (`TrunkReaderT::OutPut`,
        // `index_filter_evaluators_impl.cpp:912`) - so only the split point
        // changes here, not the values.
        let block_rows = storage::cpp_format::DEFAULT_BLOCK_CAPACITY as u64;
        // Every call rewrites the whole spill set; the file name carries the row
        // count, so a leftover from an earlier (smaller) snapshot would publish
        // a second chunk for the same block.
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                if entry.path().extension().is_some_and(|e| e == "idx") {
                    let _ = std::fs::remove_file(entry.path());
                }
            }
        }
        let mut readers = Vec::new();
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
                let path = dir.join(format!("seg_{segment}_{block}_{}.idx", entries.len()));
                // Always rewrite: the name does not capture the data, and a
                // stale chunk from an earlier run would shadow this one.
                if write_secondary_segment_typed(&path, kind, &entries).is_err() {
                    continue;
                }
                if let Ok(reader) =
                    storage::secondary_segment::SecondarySegmentReader::open_with_kind(&path, kind)
                {
                    // The chunk's offsets are already segment-relative, so the
                    // base is the segment alone.
                    readers.push((segment << 32, Arc::new(reader)));
                }
            }
        }
        let tkey = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .secondary
            .disk
            .insert(column.to_string(), readers);
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
        let mut decoded = Vec::with_capacity(encoded.len());
        for text in encoded {
            match self.parse_stored_row(table, &defaults, text.as_bytes()) {
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
    /// The block `version` is what makes a delete durable (`write_cpp_blocks`
    /// marks the slot at the next checkpoint), and `restore_cpp_blocks` rebuilds
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
        entry.mirror_dirty = true;
        entry.mirror_epoch += 1;
        first
    }

    /// The commit timestamp of every delete, aligned with [`Self::deleted_ids`].
    pub(crate) fn delete_ts(&self, table: &str) -> Vec<u64> {
        self.load_deletes(table).1
    }

    /// The write gate of `table` (see [`Self::batch_locks`]).
    pub(crate) fn batch_gate(&self, table: &str) -> Arc<Mutex<()>> {
        let key = self.table_key(table);
        let mut gates = self.batch_locks.lock().unwrap();
        Arc::clone(gates.entry(key).or_default())
    }

    /// `INSERT INTO t [(cols)] SELECT …`: evaluates the query through the
    /// normal SELECT path, maps its columns onto the target schema (by the
    /// explicit list when present, otherwise by position), casts each value to
    /// the target column's type, and appends the rows.
    pub(crate) fn insert_from_select(
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
        self.insert_literals(table, None, literals)
    }

    /// Inserts literal rows into `table`. The Thrift `Insert` RPC calls this
    /// directly: rebuilding SQL text for a 1000-row batch and re-parsing it cost
    /// more than the insert itself.
    pub fn insert_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        // The Thrift `Insert` RPC reaches this directly, so the mode gate has
        // to live here and not only in `execute` (`PhysicalInsert`).
        self.ensure_writable()?;
        self.insert_literals_inner(table, columns, rows, false)
    }

    /// `NewTxn::Import`: like [`Self::insert_literals`], but the batch always
    /// opens a fresh segment (`TableMeta::GetSegmentIDs1` gains a new id).
    pub fn import_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        self.ensure_writable()?;
        self.insert_literals_inner(table, columns, rows, true)
    }

    pub(crate) fn insert_literals_inner(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
        new_segment: bool,
    ) -> Result<QueryResult> {
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
        // A memory index that already exceeds its quota is dumped
        // before these rows join it.
        let gate = self.batch_gate(table);
        let _batch = gate.lock().unwrap();
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
        let allocated = std::cell::Cell::new(RowRange::single(0));
        let alloc_table = table.to_string();
        let alloc_table_id = meta.id.to_string();
        let alloc_rows = rows.len();
        let ts = self.txn.commit_at_deferred_with(t, ts, |txn| {
            let range = self.reserve_rows_at(&alloc_table, alloc_rows, new_segment, ts)?;
            allocated.set(range);
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
        })?;
        let range = allocated.get();
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
        let row_count = rows.len();
        let bottom_rows = rows;
        let bottom_encoded = encoded_rows;
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
            if engine.any_in_memory_index() || engine.has_index_declaration(&bottom_table) {
                // Every index kind is maintained by the appender thread, the
                // way `AppendIndex` hands each index meta to its own
                // `AppendMemIndex`: the bottom half only waits for it
                // (`PostCommit`'s semaphores), it does not walk the batch
                // itself.
                let index_table = bottom_table.clone();
                let index_rows: Vec<(usize, Vec<String>)> = bottom_rows
                    .iter()
                    .enumerate()
                    .map(|(k, row)| (base + k, row.iter().map(|v| v.to_slt_string()).collect()))
                    .collect();
                engine.run_text_append(move |engine| {
                    let schema = engine.table_schema(&index_table).unwrap_or_default();
                    for (seq, texts) in &index_rows {
                        engine.update_indexes_on_insert_text(&index_table, *seq, texts, &schema)?;
                        engine.update_text_indexes(&index_table, *seq, texts, &schema);
                        engine.update_vector_caches(&index_table, *seq, texts);
                    }
                    // C++ `AppendMemIndex`: a segment that reached
                    // `mem_index_capacity` rows is dumped right after the batch.
                    engine.dump_hnsw_over_capacity(&index_table);
                    engine.dump_secondary_over_capacity(&index_table);
                    Ok(())
                })?;
            }
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
            engine.publish_encoded_rows(&bottom_table, &published)
        })?;
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

    pub(crate) fn commit_with_deletes(
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
        let ts = self.txn.commit_deferred(txn)?;
        let _visible = BottomGuard::new(&self.txn, ts);
        // `CommitBottomDelete` on the table's worker: the block's `version`
        // records the delete ts (and an UPDATE's rewritten rows' append ts).
        self.delete_bottom(
            table,
            deletes,
            AppendRecords::Last(appended),
            extra.to_vec(),
            ts,
        )?;
        Ok(ts)
    }

    /// `CommitBottomDelete`: the delete joins the table's log, the block's
    /// `version` carries both timestamps, and the rows the statement appended
    /// go into the block (`AppendInBlock`).
    pub(crate) fn delete_bottom(
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
            match appended {
                AppendRecords::None => {}
                AppendRecords::Last(count) => engine.settle_append_ts(&bottom_table, count, ts),
            }
            // `BlockVersion::Delete(row_id, commit_ts)`: the slot carries the
            // deleting transaction's commit timestamp, and the block rewrite
            // marks it - there is no tombstone key to write.
            engine.settle_delete_at(&bottom_table, first_delete, delete_count, ts);
            engine.refresh_cpp_block_mirror_extra(&bottom_table, &bottom_rows)
        })
    }

    /// Commits an `UPDATE`: the reference's `DeleteV2` and `AppendV2` in one
    /// entry, with the rewritten rows' ids allocated **inside** the commit.
    ///
    /// `prepare` runs between the conflict check and the WAL entry, gets the
    /// base id and fills in the entry's commands; the rows themselves reach the
    /// block in the bottom half.
    pub(crate) fn commit_rewritten_rows<F>(
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
        let allocated = std::cell::Cell::new(RowRange::single(0));
        let ts = self.txn.commit_at_deferred_with(txn, ts, |txn| {
            let range = self.reserve_rows_at(table, count, false, ts)?;
            allocated.set(range);
            prepare(txn, range.base)
        })?;
        let range = allocated.get();
        let base = range.base;
        let _visible = BottomGuard::new(&self.txn, ts);
        // One copy of the rewritten rows: the block buffer takes them as
        // `Value`s, and the caller keeps its slice for the index hooks.
        let extra: Vec<(usize, Vec<Value>)> = rows
            .iter()
            .enumerate()
            .map(|(k, row)| (base + k, row.clone()))
            .collect();
        self.delete_bottom(table, deletes, AppendRecords::None, extra, ts)?;
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
            let qterms = tokenize_index_text(&query, &tokenizer);
            for row in rows.iter_mut() {
                let Some(text) = row.get(i).map(|v| v.to_slt_string()) else {
                    continue;
                };
                row[i] = Value::Str(highlight_text(&text, &qterms, &tokenizer.delims));
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
        let texts: Vec<String> = values.iter().map(|v| v.to_slt_string()).collect();
        self.update_indexes_on_insert_text(table, seq, &texts, schema)
    }

    /// Feeds a whole batch of rows into every secondary index of `table`.
    ///
    /// The per-row form resolved the table key (a formatted, lowercased
    /// string) and took the registry lock for every row, and looked the indexed
    /// column up in the schema once per index per row. A bulk statement paid
    /// that per row; this pays it once for the batch.
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
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        if entry.secondary.indexes.is_empty() {
            return;
        }
        // `(position of the indexed column in the row, index)`, resolved once.
        let mut targets: Vec<(usize, &mut SecondaryIndex)> = Vec::new();
        for (col, idx) in entry.secondary.indexes.iter_mut() {
            if let Some(pos) = schema.iter().position(|c| c == col) {
                targets.push((pos, idx));
            }
        }
        // Only the indexed cells are rendered: the key is the cell's stored
        // text, and a wide table has no reason to format the rest. A varchar
        // cell *is* its stored text, so it goes in without a copy; everything
        // else is formatted into one buffer reused across the batch.
        let mut key = String::new();
        for (k, values) in rows.iter().enumerate() {
            let seq = (base + k) as u64;
            for (pos, idx) in targets.iter_mut() {
                let Some(value) = values.get(*pos) else {
                    continue;
                };
                match value {
                    Value::Str(text) => idx.insert(text.as_bytes(), seq),
                    other => {
                        key.clear();
                        key.push_str(&other.to_slt_string());
                        idx.insert(key.as_bytes(), seq);
                    }
                }
            }
        }
    }

    pub(crate) fn update_indexes_on_insert_text(
        &self,
        table: &str,
        seq: usize,
        values: &[String],
        schema: &[String],
    ) -> Result<()> {
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        for (col, idx) in entry.secondary.indexes.iter_mut() {
            if let Some(ci) = schema.iter().position(|c| c == col) {
                if let Some(v) = values.get(ci) {
                    idx.insert(v.as_bytes(), seq as u64);
                }
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
        let probing = {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key.clone()).or_default();
            if entry.hnsw_mem.is_empty() && !entry.hnsw_probed {
                entry.hnsw_probed = true;
                true
            } else {
                !entry.hnsw_mem.is_empty()
            }
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
            if entry.hnsw_mem.is_empty() && entry.hnsw_probed {
                // The probe above marked the table as examined; a table with
                // no HNSW index leaves `hnsw_mem` empty forever.
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
                    entry.hnsw_mem.entry(im.column.clone()).or_insert_with(|| {
                        Arc::new(Mutex::new(crate::hnsw_mem::HnswIndexInMem::new_with(
                            dims, m, efc, metric, encoding,
                        )))
                    });
                }
            }
            entry
                .hnsw_mem
                .iter()
                .map(|(c, m)| (c.clone(), m.clone()))
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
        for (column, rows) in entry.ivf_mem.iter_mut() {
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

    pub(crate) fn update_multivector_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<MultiVectorIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| e.mv.iter().map(|(c, v)| (c.clone(), v.clone())).collect())
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
            let Some(text) = cells.get(ci) else { continue };
            if let Some(doc) = parse_multivector_doc(text, dims) {
                index.insert(seq as u64, doc);
            }
        }
    }

    /// Keeps existing sparse indexes in step with an inserted/rewritten row.
    pub(crate) fn update_sparse_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<SparseIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.sparse
                    .iter()
                    .map(|(c, v)| (c.clone(), v.clone()))
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

    /// Keeps existing inverted indexes in step with an inserted/rewritten row.
    pub(crate) fn update_text_indexes(
        &self,
        table: &str,
        seq: usize,
        cells: &[String],
        schema: &[String],
    ) {
        // The multi-vector and sparse indexes share this DML hook: it is the one
        // place every write path (SQL INSERT/UPDATE, COPY, Thrift insert) goes
        // through.
        self.update_multivector_indexes(table, seq, cells);
        self.update_sparse_indexes(table, seq, cells);
        self.enforce_memindex_quota();
        let tkey = self.table_key(table);
        let indexes: Vec<(String, SharedTextIndex)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| e.text.iter().map(|(c, v)| (c.clone(), v.clone())).collect())
            .unwrap_or_default();
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let mut index = shared.write().unwrap();
            index_delete_row(&mut index, seq as u64);
            index_row(&mut index, seq as u64, text);
            index.maybe_seal(self.memindex_capacity());
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
    pub(crate) fn commit_import_chunk(
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
        let ts = self.txn.commit_at_deferred(t, ts)?;
        let _visible = BottomGuard::new(&self.txn, ts);
        // Every block the import wrote carries this transaction's ts: the
        // reference's `CommitBottomAppend` appends each block's row count to the
        // block's `version` inside the import transaction, so all of them share
        // it, not just the last one.
        for rows in batches {
            self.note_append(table, *rows, true, Some(ts));
        }
        for segment in segments {
            // The extent is what marks the segment sealed, and a sealed segment
            // is the only one whose blocks (and segment) get a fast rough
            // filter - so it has to be recorded **before** the block write.
            let rows = self.segment_rows(table, *segment as usize) as u64;
            self.note_import_rows(table, *segment, rows);
            self.note_import_ts(table, *segment, ts);
        }
        self.publish_encoded_rows(table, encoded_rows)?;
        // `AppendInBlock` / `CommitBottomAppend`: the reference writes each
        // block inside the import transaction, so a `COPY` leaves readable
        // block files (and `SHOW BLOCKS` sees them) without a checkpoint. A
        // table whose columns the block format cannot express has no mirror -
        // it keeps its rows in the store.
        if self.has_block_layout(table) {
            self.write_cpp_blocks_commit(table, ts)?;
        }
        // `CommitBottomAppend` for the imported segments on the table's worker.
        let bottom_table = table.to_string();
        let shard_key = bottom_table.clone();
        self.run_commit_bottom(&shard_key, move |engine| {
            engine.dump_hnsw_over_capacity(&bottom_table);
            engine.dump_secondary_over_capacity(&bottom_table);
            Ok(())
        })?;
        Ok(ts)
    }

    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    pub(crate) fn copy_from(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // `NewTxn::Import` is **one** transaction: it writes every block and
        // commits once. The extent each segment ends up with is what decides
        // how many index chunks the mirror publishes for it.
        let before = self.segment_row_counts(table);
        let result = self.copy_from_inner(table, path, opts);
        if result.is_ok() {
            self.settle_import_extent(table, &before);
        }
        result
    }

    pub(crate) fn copy_from_inner(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // The import writes its blocks inside its own commit, so a background
        // compaction must not run between the commit and the write.
        let gate = self.batch_gate(table);
        let _batch = gate.lock().unwrap();
        if self.storage.catalog.get_table_by_name(table)?.is_none() {
            return Err(Error::NotFound(format!("table '{table}'")));
        }
        let resolved = resolve_data_path(path);
        // Read raw bytes: `FORMAT FVECS` is binary, everything else is text.
        // Missing fixtures stay a statement-level success with no rows.
        let t_read0 = std::time::Instant::now();
        let bytes_in = match std::fs::read(&resolved) {
            Ok(b) => b,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let t_read = t_read0.elapsed();
        // `FORMAT FVECS` is a binary vector file: `i32 dim` then `dim` f32s per
        // record. It can only target a table whose single column is the vector.
        if opts
            .iter()
            .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case("FVECS"))
        {
            let types = self.column_types(table)?;
            if types.len() != 1 {
                return Err(Error::Invalid(format!(
                    "import: fvecs needs a table with exactly one vector column, got {}",
                    types.len()
                )));
            }
            let profile = std::env::var_os("INFINITY_PROFILE_IMPORT").is_some();
            let t0 = std::time::Instant::now();
            let mut rows: Vec<Vec<Literal>> = Vec::new();
            let bytes = bytes_in.as_slice();
            let mut pos = 0usize;
            while pos + 4 <= bytes.len() {
                let dim = i32::from_le_bytes(bytes[pos..pos + 4].try_into().unwrap()) as usize;
                pos += 4;
                // A record must fit; a truncated/garbage header is a format error.
                if dim == 0 || pos + dim * 4 > bytes.len() {
                    return Err(Error::Invalid(
                        "import: malformed fvecs file (bad record header)".into(),
                    ));
                }
                let vals: Vec<String> = (0..dim)
                    .map(|i| {
                        let b = &bytes[pos + i * 4..pos + i * 4 + 4];
                        format!("{}", f32::from_le_bytes(b.try_into().unwrap()))
                    })
                    .collect();
                pos += dim * 4;
                rows.push(vec![Literal::Str(format!("[{}]", vals.join(",")))]);
            }
            if rows.is_empty() || pos != bytes.len() {
                return Err(Error::Invalid(
                    "import: malformed fvecs file (trailing bytes)".into(),
                ));
            }
            let t_parse = t0.elapsed();
            let t1 = std::time::Instant::now();
            // `ApplySegmentIDs`: a segment of the statement's own, with no
            // append record yet - the records are written when it commits.
            let opened = self.begin_import(table)?;
            let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened)?;
            let base = range.base;
            self.note_index_insert(table, rows.len());
            let mut t = self.txn.begin();
            t.set_op(TxnOp::Import {
                table: table.to_string(),
            });
            t.set_typed_commands(vec![self.import_command(table, (base >> 32) as u32)?]);
            let t_encode = t1.elapsed();
            let t2 = std::time::Instant::now();
            // `PrepareCommitImport` -> `CommitBottomAppend`: the block and the
            // `create` boundary belong to the transaction, so they are written
            // once it commits - a statement that loses its commit leaves
            // neither behind.
            let turn = self.txn.reserve_commit_ts();
            let ts = turn.ts();
            let ts = self.txn.commit_at_deferred(t, ts)?;
            let t_commit = t2.elapsed();
            let t3 = std::time::Instant::now();
            let _visible = BottomGuard::new(&self.txn, ts);
            self.note_append(table, rows.len(), true, Some(ts));
            let segment = (base >> 32) as u32;
            let imported = self.segment_rows(table, segment as usize) as u64;
            self.note_import_rows(table, segment, imported);
            self.note_import_ts(table, segment, ts);
            self.publish_rows(table, base, &rows)?;
            // `AppendInBlock`: the import's block is readable as soon as the
            // statement commits, exactly like the CSV path.
            if self.has_block_layout(table) {
                self.write_cpp_blocks_commit(table, ts)?;
            }
            drop(_visible);
            let t_bottom = t3.elapsed();
            if profile {
                eprintln!(
                    "import profile: rows={} read={:?} parse={:?} encode={:?} commit={:?} bottom={:?}",
                    rows.len(),
                    t_read,
                    t_parse,
                    t_encode,
                    t_commit,
                    t_bottom
                );
            }
            return Ok(QueryResult::Inserted { count: rows.len() });
        }
        let delim = opts
            .iter()
            .find(|(k, _)| k == "DELIMITER")
            .map(|(_, v)| decode_delimiter(v))
            .unwrap_or(',');
        let types_for_scan = self.column_types(table)?;
        let defaults = self.table_defaults(table)?;
        let format_of = |name: &str| {
            opts.iter()
                .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case(name))
        };
        // `FORMAT JSONL` (one object per line) and `FORMAT JSON` (one array)
        // are small in practice; the CSV path streams.
        if format_of("JSONL") || format_of("JSON") {
            let text = String::from_utf8_lossy(&bytes_in).into_owned();
            if format_of("JSONL") {
                return self.copy_from_jsonl(table, &text, &types_for_scan, &defaults);
            }
            return self.copy_from_json(table, &text, &types_for_scan, &defaults);
        }
        // CSV: read line by line and commit in blocks, so an 8-million-row
        // import does not hold the file and its parsed rows in memory.
        let file = match std::fs::File::open(&resolved) {
            Ok(f) => f,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let mut reader = std::io::BufReader::new(file);
        let mut total = 0usize;
        // The statement's own segment cursor (`ApplySegmentIDs`): the table's
        // shared counter belongs to whoever allocated last, so reading it again
        // per batch would start this file's next block after an `INSERT`'s rows.
        let mut cursor: Option<ImportCursor> = None;
        // `NewTxn::Import` is one transaction for the whole statement, so the
        // rows are staged into a single `Txn` and committed once - at the end,
        // for every size (the commit below keys on `staged`).
        let t = self.txn.begin();
        let mut staged = 0usize;
        let mut segments: Vec<u32> = Vec::new();
        // One entry per staged batch: the block `version` keeps a `create`
        // boundary per append, and a statement that rolls over into a second
        // segment must keep the boundaries of both.
        let mut batches_all: Vec<usize> = Vec::new();
        // `(row id, stored text)` per row, so the commit can hand the block the
        // values it publishes without a second pass over the file.
        let mut encoded_rows: Vec<(usize, String)> = Vec::new();
        loop {
            let mut batch: Vec<Vec<Literal>> = Vec::with_capacity(Self::COPY_BATCH_ROWS);
            let mut eof = false;
            while batch.len() < Self::COPY_BATCH_ROWS {
                let mut line = String::new();
                use std::io::BufRead;
                match reader.read_line(&mut line) {
                    Ok(0) => {
                        eof = true;
                        break;
                    }
                    Ok(_) => {}
                    Err(e) => return Err(Error::io(&resolved, e)),
                }
                let line = line.trim_end_matches(['\n', '\r']);
                if line.is_empty() {
                    continue;
                }
                let fields: Vec<String> = csv_fields(line, delim);
                let mut cells: Vec<String> = fields.iter().map(|f| normalize_cell(f)).collect();
                if cells.len() > types_for_scan.len() {
                    return Err(Error::Invalid(format!(
                        "import: the file has {} columns but the table has {}",
                        cells.len(),
                        types_for_scan.len()
                    )));
                }
                for (i, c) in cells.iter_mut().enumerate() {
                    if c.is_empty() {
                        if let Some(Some(d)) = defaults.get(i) {
                            *c = d.clone();
                        }
                    }
                }
                for (i, c) in cells.iter().enumerate() {
                    if let Some(t) = types_for_scan.get(i) {
                        validate_insert_value(t, c.trim()).map_err(|e| match e {
                            Error::Invalid(m) if m.starts_with("parser:") => Error::Invalid(m),
                            other => Error::Invalid(format!("import: {other}")),
                        })?;
                    }
                }
                batch.push(cells.into_iter().map(Literal::Str).collect());
            }
            if !batch.is_empty() {
                // The statement keeps its own segment cursor: the table's
                // shared counter belongs to whoever allocated last, and an
                // `INSERT` during a long `COPY` moves it on.
                //
                // `Self::SEGMENT_CAPACITY` bounds one segment: the reference
                // seals a full segment and continues in the next, so a `COPY`
                // of more rows than that ends up with two segments - the one
                // the extent fills and the one the remainder opens.
                let schema = self.table_schema(table).unwrap_or_default();
                let mut start = 0usize;
                while start < batch.len() {
                    let mut current = cursor.take();
                    if current
                        .as_ref()
                        .is_some_and(|c| c.offset as usize >= Self::SEGMENT_CAPACITY)
                    {
                        current = None;
                    }
                    let room = current.as_ref().map_or(Self::SEGMENT_CAPACITY, |c| {
                        Self::SEGMENT_CAPACITY - c.offset as usize
                    });
                    let take = (batch.len() - start).min(room);
                    let (range, next_cursor) = match current {
                        Some(cursor) => self.reserve_import_rows(table, take, cursor)?,
                        None => {
                            let opened = self.begin_import(table)?;
                            self.reserve_import_rows(table, take, opened)?
                        }
                    };
                    let base = range.base;
                    cursor = Some(next_cursor);
                    batches_all.push(take);
                    self.note_index_insert(table, take);
                    for (k, row) in batch[start..start + take].iter().enumerate() {
                        // The rows ride in the transaction only as its `ImportV2`
                        // extent; their values go to the block buffer at commit.
                        let encoded = self.encode_row_stored(table, row, &types_for_scan)?;
                        encoded_rows.push((base + k, encoded));
                    }
                    let segment = (base >> 32) as u32;
                    if !segments.contains(&segment) {
                        segments.push(segment);
                    }
                    for (k, row) in batch[start..start + take].iter().enumerate() {
                        self.update_indexes_on_insert(table, base + k, row, &schema)?;
                        let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
                        self.update_text_indexes(table, base + k, &texts, &schema);
                        self.update_vector_caches(table, base + k, &texts);
                    }
                    total += take;
                    staged += take;
                    start += take;
                }
            }
            if eof {
                break;
            }
        }
        if staged > 0 {
            // One `create` boundary per staged batch, across every segment this
            // statement opened.
            self.commit_import_chunk(table, t, &segments, &encoded_rows, &batches_all)?;
        } else {
            drop(t);
        }
        Ok(QueryResult::Inserted { count: total })
    }

    /// Loads a `FORMAT JSONL` copy: one JSON object per line, keyed by column
    /// name. Sparse columns come as `{index: value}` objects (float) or index
    /// arrays (bit); everything else is a scalar.
    pub(crate) fn copy_from_jsonl(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        for line in text.lines() {
            let line = line.trim();
            if line.is_empty() {
                continue;
            }
            if let Some(Json::Obj(fields)) = expression::json::parse(line) {
                objects.push(fields);
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Loads a `FORMAT JSON` copy: the whole file is one JSON array whose
    /// elements are objects keyed by column name.
    pub(crate) fn copy_from_json(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        if let Some(Json::Arr(items)) = expression::json::parse(text.trim()) {
            for it in items {
                if let Json::Obj(fields) = it {
                    objects.push(fields);
                }
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Inserts one row per JSON object (keyed by column name). Sparse columns
    /// come as `{index: value}` objects (float) or index arrays (bit);
    /// everything else is a scalar.
    pub(crate) fn insert_json_objects(
        &self,
        table: &str,
        objects: &[Vec<(String, expression::json::Json)>],
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let schema = self.table_schema(table)?;
        let mut rows: Vec<Vec<Literal>> = Vec::new();
        for fields in objects {
            let mut cells: Vec<Literal> = Vec::with_capacity(schema.len());
            for (i, name) in schema.iter().enumerate() {
                let ty = types.get(i).cloned().unwrap_or_default();
                let value = fields.iter().find(|(k, _)| k == name).map(|(_, v)| v);
                let cell = match value {
                    Some(Json::Obj(pairs)) if is_sparse_type(&ty) => {
                        let mut ps: Vec<(i64, f64)> = pairs
                            .iter()
                            .filter_map(|(k, v)| {
                                let idx = k.trim().parse::<i64>().ok()?;
                                let val = match v {
                                    Json::Int(x) => *x as f64,
                                    Json::Float(x) => *x,
                                    _ => return None,
                                };
                                Some((idx, val))
                            })
                            .collect();
                        ps.sort_by_key(|(i, _)| *i);
                        let body: Vec<String> =
                            ps.iter().map(|(i, v)| format!("{i}:{v:.6}")).collect();
                        Literal::Str(format!("[{}]", body.join(",")))
                    }
                    Some(Json::Arr(items)) if is_sparse_type(&ty) => {
                        let idx: Vec<String> = items
                            .iter()
                            .filter_map(|x| match x {
                                Json::Int(i) => Some(i.to_string()),
                                Json::Float(f) => Some((*f as i64).to_string()),
                                _ => None,
                            })
                            .collect();
                        Literal::Str(format!("[{}]", idx.join(",")))
                    }
                    // an absent / null field takes the column default
                    Some(Json::Null) | None => defaults
                        .get(i)
                        .and_then(|d| d.as_deref())
                        .map(default_literal)
                        .unwrap_or(Literal::Null),
                    Some(other) => Literal::Str(json_scalar_text(other)),
                };
                cells.push(cell);
            }
            // validate each typed cell (sparse range, tensor dimension)
            for (i, cell) in cells.iter().enumerate() {
                if let Some(ty) = types.get(i) {
                    validate_insert_value(ty, cell.to_slt_string().trim())?;
                }
            }
            rows.push(cells);
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        let opened = self.begin_import(table)?;
        let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened)?;
        let base = range.base;
        self.note_index_insert(table, rows.len());
        let mut t = self.txn.begin();
        t.set_op(TxnOp::Import {
            table: table.to_string(),
        });
        // `PrepareCommitImport` seals the segment and writes its rows into the
        // block as part of the transaction, after it commits.
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        let _ts = self.txn.commit_at_deferred(t, ts)?;
        let _visible = BottomGuard::new(&self.txn, _ts);
        self.note_append(table, rows.len(), true, Some(_ts));
        self.note_import_ts(table, (base >> 32) as u32, _ts);
        self.publish_rows(table, base, &rows)?;
        let schema = self.table_schema(table).unwrap_or_default();
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row, &schema)?;
            let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
            self.update_text_indexes(table, base + k, &texts, &schema);
            self.update_vector_caches(table, base + k, &texts);
        }
        self.dump_hnsw_over_capacity(table);
        self.dump_secondary_over_capacity(table);
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Writes `table` to a file (`COPY t TO …`): CSV, JSONL, or the binary
    /// `fvecs` (per row: `i32 dim` followed by `dim` little-endian `f32`).
    pub(crate) fn copy_to(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        let rows = self.fetch_rows(table)?;
        let schema = self.table_schema(table)?;
        let column_types = self.column_types(table)?;
        let opt = |name: &str| -> Option<&str> {
            opts.iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.as_str())
        };
        let format = opt("FORMAT").unwrap_or("CSV").to_ascii_uppercase();
        let jsonl = format == "JSONL";
        let fvecs = format == "FVECS";
        // `COLUMNS 'a,b'` projects the export (the SDK's `columns` argument).
        let projected: Option<Vec<usize>> = opt("COLUMNS").map(|list| {
            list.split(',')
                .map(str::trim)
                .filter(|c| !c.is_empty())
                .filter_map(|c| schema.iter().position(|x| x == c))
                .collect()
        });
        let indices: Vec<usize> = projected.unwrap_or_else(|| (0..schema.len()).collect());
        // `OFFSET n` skips rows; `LIMIT n` caps the exported count.
        let offset: usize = opt("OFFSET").and_then(|v| v.parse().ok()).unwrap_or(0);
        let limit: Option<usize> = opt("LIMIT").and_then(|v| v.parse().ok());
        let selected: Vec<&Vec<Value>> = rows
            .iter()
            .skip(offset)
            .take(limit.unwrap_or(usize::MAX))
            .collect();
        let mut out: Vec<u8> = Vec::new();
        if fvecs {
            for row in &selected {
                let Some(&ci) = indices.first() else { continue };
                let text = row.get(ci).map(|v| v.to_slt_string()).unwrap_or_default();
                let inner = text.trim().trim_start_matches('[').trim_end_matches(']');
                let vals: Vec<f32> = inner
                    .split(',')
                    .map(str::trim)
                    .filter(|v| !v.is_empty())
                    .filter_map(|v| v.parse::<f32>().ok())
                    .collect();
                out.extend_from_slice(&(vals.len() as i32).to_le_bytes());
                for v in vals {
                    out.extend_from_slice(&v.to_le_bytes());
                }
            }
        } else if jsonl {
            for row in &selected {
                let fields: Vec<String> = indices
                    .iter()
                    .map(|&i| {
                        let name = &schema[i];
                        let ty = column_types.get(i).map(String::as_str).unwrap_or("");
                        let v = row.get(i).cloned().unwrap_or(Value::Null);
                        format!("\"{name}\":{}", json_value_typed(&v, ty))
                    })
                    .collect();
                out.extend_from_slice(format!("{{{}}}\n", fields.join(",")).as_bytes());
            }
        } else {
            for row in &selected {
                let line = indices
                    .iter()
                    .map(|&i| match row.get(i) {
                        Some(Value::Str(s)) if s.contains(',') => format!("\"{s}\""),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    })
                    .collect::<Vec<_>>()
                    .join(",");
                out.extend_from_slice(line.as_bytes());
                out.push(b'\n');
            }
        }
        // Tolerate unwritable export paths (statement-level success).
        let _ = std::fs::write(resolve_write_path(path), out);
        Ok(QueryResult::Inserted {
            count: selected.len(),
        })
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

    /// Sets `[buffer] memindex_memory_quota` (bytes). A memory index is dumped
    /// into a fresh C++ chunk once its buffered size reaches this quota; `0`
    /// keeps every row in memory until a checkpoint.
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
            // A vector node carries its payload plus graph bookkeeping; the
            // constant term keeps small dimensions over the quota in the same
            // number of rows as the reference.
            let bytes = (dim * 4 + 256) * count;
            let tkey = self.table_key(&im.table);
            let dump = {
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey.clone()).or_default();
                let cur = entry
                    .index_pending_bytes
                    .entry(im.column.clone())
                    .or_insert(0);
                if quota > 0 && *cur >= quota {
                    // The dump empties the memory index; this batch is the
                    // first thing in the next one.
                    *cur = bytes;
                    true
                } else {
                    if quota > 0 {
                        *cur += bytes;
                    }
                    false
                }
            };
            if dump && self.dump_cpp_hnsw_index(&im.table, &im.name).is_ok() {
                // The dumped segment leaves memory; these rows start the next
                // one, so the counter keeps this batch's bytes instead of
                // resetting to zero (`AppendMemIndex` accounts the new segment
                // from the first row it receives).
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&tkey) {
                    e.hnsw_mem.remove(&im.column);
                    e.index_pending_bytes.insert(im.column.clone(), bytes);
                    e.hnsw_probed = false;
                }
            }
        }
    }
}

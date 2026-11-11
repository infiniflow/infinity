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
    pub fn compact_table(&self, table: &str) -> Result<()> {
        // `ManualCompactTask` hands `PhysicalCompact` the candidates
        // `NewCompactionAlg` picked (`physical_compact_impl.cpp:36`). When it
        // picks nothing the reference opens **no** transaction and leaves the
        // directory untouched; merging anyway would move every row and spend a
        // timestamp it never spends.
        let candidates = self.compaction_candidates(table);
        if candidates.is_empty() {
            return Ok(());
        }
        self.compact_segments(table, &candidates)
    }

    /// `NewTxn::Compact`: merge every segment of `table` into one new segment.
    ///
    /// This is the primitive the reference's compaction *tests* drive directly
    /// (`TestTxnCompact` calls `NewTxn::Compact` with explicit segment ids), so
    /// it merges unconditionally — even a single segment, which the manual and
    /// periodic tasks would never choose.
    pub fn compact_all_segments(&self, table: &str) -> Result<()> {
        let all = self.segment_ids(table);
        self.compact_segments(table, &all)
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
    pub fn optimize_fulltext(&self, table: &str) -> Result<usize> {
        self.ensure_writable()?;
        // A merge replaces chunk files, so it must not interleave with a seal
        // or a compaction of the same table.
        let gate = self.batch_gate(table);
        let _batch = gate.lock().unwrap();
        let key = self.table_key(table);
        // Clone the handles while the `tables` guard is held, then drop it before
        // taking any index write lock. `text_index_for` takes `tables` first and
        // the index second, so waiting for an index lock with `tables` held is the
        // reverse order and can deadlock a concurrent seal or query.
        let indexes: Vec<_> = {
            let tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get(&key) else {
                return Ok(0);
            };
            entry.text.values().cloned().collect()
        };
        let mut merges = 0;
        for index in indexes {
            // The merge rewrites whole chunks: keep its I/O out of the column's
            // write lock so queries and inserts are not held behind it.
            merges += crate::engine::state::optimize_text_index_unlocked(&index)?;
        }
        Ok(merges)
    }

    /// [`Self::optimize_fulltext`] for every open table — the full-text half of
    /// an optimize tick (`OptimizationProcessor` → `OptimizeAllIndexes`).
    ///
    /// The reference walks the catalog's index metas and merges each
    /// `kFullText` index's chunk list (`new_txn_index_impl.cpp:349-353`); this
    /// port walks the tables whose indexes are open, which is a subset: a table
    /// nobody touched since the process started has no `TextIndex` in memory
    /// yet, and its chunks are merged by a later tick (opening the table runs
    /// `load_sealed`, which sees every chunk). Loading it here instead would
    /// mean calling `text_index_for`, whose row-count check can start a rebuild
    /// — an optimize tick must not rewrite an index it was only asked to
    /// compact. Returns the number of chunk merges.
    pub fn optimize_all_fulltext_indexes(&self) -> Result<usize> {
        let keys: Vec<String> = {
            let tables = self.tables.lock().unwrap();
            tables.keys().cloned().collect()
        };
        let mut merges = 0;
        for key in keys {
            merges += self.optimize_fulltext(&key)?;
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
    pub(crate) fn compact_segments(&self, table: &str, sources: &[u32]) -> Result<()> {
        // Compaction rewrites segments, so both the manual and the periodic
        // task require `kWritable` (`compaction_process_impl.cpp:270-295`).
        self.ensure_writable()?;
        // One write batch at a time per table: a compaction moves every row it
        // merges and drops the buffer entries of the segments it consumed, so
        // it must not interleave with an import's commit or with its block
        // write (a row the compaction cleared but never re-published left the
        // mirror with an empty block).
        let gate = self.batch_gate(table);
        let _batch = gate.lock().unwrap();
        self.invalidate_row_cache(table);
        // Compaction moves rows between segments without changing how many are
        // live, so the counter survives even though the mirror's row-id
        // snapshot does not (it stops matching the moment the first row moves).
        let live_rows = self.live_row_count(table)?;
        self.invalidate_block_mirror(table);
        let sources: std::collections::HashSet<u32> = sources.iter().copied().collect();
        let mut source_ids: Vec<u32> = sources.iter().copied().collect();
        source_ids.sort_unstable();
        // The layout the row ids were allocated in, before anything moves:
        // `next_seg` counts from every segment, and the append log is rebased
        // against it.
        let layout = self.segment_row_counts(table);
        let next_seg = layout
            .iter()
            .map(|(id, _)| *id)
            .max()
            .map_or(0, |id| id + 1);
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
        cmds.extend(self.dump_index_commands(table, next_seg, rows.len(), "")?);
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
        self.txn.commit_at_deferred_with(t, compact_ts, |_txn| {
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
                entry.text.clear();
                entry.sparse.clear();
                entry.mv.clear();
                entry.hnsw_mem.clear();
                entry.vector.clear();
                entry.ivf.clear();
                entry.ivf_mem.clear();
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
                let totals =
                    self.rebase_appends(table, &layout, &sources, rows.len() as u64, compact_ts);
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
            let gate = self.bottom_gate(table);
            let _turn = gate.lock().unwrap();
            let _held = crate::engine::statements::HeldBottom::enter(&self.table_key(table));
            self.publish_encoded_rows(table, &merged)?;
            // `PrepareCommitCompact`: the merged segment's blocks and
            // records belong to the compaction transaction; writing them here
            // (before the frame) is what makes a kill harmless - the
            // `drop|seg|…` markers above are in the same pre-frame phase, so
            // a replay either sees the merged blocks or none of it.
            if !rows.is_empty() && self.has_block_layout(table) {
                self.write_blocks_bounded(table, compact_ts)?;
            }
            drop(_held);
            drop(_turn);
            Ok(())
        })?;
        // `CommitBottomCompact`: the merged rows are visible only once their
        // index entries are in place, so the commit stays unpublished until the
        // rebuild below finishes - `commit_at_deferred_with` leaves
        // `finish_bottom` to the caller.
        let _visible = BottomGuard::new(&self.txn, compact_ts);
        // The merge renumbered every row, so every index posting now points at
        // a dead row id. Rebuild the declared indexes from the merged rows.
        self.rebuild_indexes()?;
        self.result_cache.lock().unwrap().clear();
        self.plan_cache.clear();
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

    /// The checkpoint itself, without the mode gate: [`Self::cleanup`] runs it
    /// on a read-only replica too, where the reference's cleanup task is
    /// allowed (`background_process_impl.cpp:109`).
    pub(crate) fn flush_storage(&self) -> Result<()> {
        // One checkpoint at a time: the block mirror is rewritten in place.
        let _serial = self.storage.flush_lock.lock().unwrap();
        // `NewTxn::Checkpoint`: the blocks are written at this transaction's
        // `begin_ts`, so only rows of transactions that already finished reach
        // the files. The frame's payload is what the store flush covers, which
        // is this same bound plus anything that committed while the mirror ran
        // (see the frame at the end of this function).
        let checkpoint_ts = self.checkpoint_ts();
        // The checkpoint **does not** dump the memory indexes: C++
        // `NewTxn::CheckpointTable` only flushes blocks, and a memory index
        // keeps its rows until a `DumpMemIndex` transaction seals them. The
        // tail is therefore still in memory after a checkpoint and is
        // reconstructed from the chunk gap on the next open
        // (`recover_mem_index`).
        //
        // The memory indexes still need a bound on a long import; that is the
        // quota (`enforce_memindex_quota`) and the per-batch capacity dumps, not
        // this function.
        // The C++ block mirror is the storage of record; a table it cannot
        // write (a column with no block layout) keeps its rows buffered only.
        let mirror = self.flush_blocks(checkpoint_ts);
        // `EngineOptions::object_dir`: publish this checkpoint's data files as
        // the reference does (`pm|object|` + packed objects).
        if let Some(pm) = self.persistence.as_ref() {
            if let Err(e) = pm.persist_directory() {
                eprintln!("object dir: persisting failed: {e}");
            }
        }
        // The mirror writes the vector/IVF chunks, so reload after it.
        self.reload_vector_indexes();
        // Bound the page cache the way the reference's buffer manager does:
        // segment files are written back and their clean pages dropped, so RSS
        // tracks live structures rather than file cache.
        for sub in [
            "fulltext",
            "multivector",
            "sparse",
            "secondary",
            "secondary_low",
        ] {
            storage::drop_tree_cache(&self.dir.join(sub));
        }
        // With `storage_type = "minio"` the C++-format data files are mirrored
        // into the bucket; the catalog and the row store follow at the end of
        // the checkpoint, once they are flushed.
        if let Some(client) = self.context.object_store.lock().unwrap().clone() {
            let root = self.dir.join("data");
            let _ = storage::s3::sync_tree(&client, &root, "data");
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
        // The checkpoint's one store flush, at the end as in
        // `NewTxn::Checkpoint` (`kv_store()->Flush()`, `new_txn_impl.cpp:1814`).
        // It covers everything the mirror published - `catalog|seg|…`,
        // `catalog|blk|…`, `idx_seg|`, `idx_chunk|…` and the layout rewrite -
        // and it raises `synced`, the marker the WAL pruning reads. Without it
        // those records stay in the memtable and a `kill -9` discards them: the
        // reference then sees block files whose segment records do not exist.
        self.txn.flush()?;
        // The checkpoint's other two trees. `data/` alone is not enough for a
        // restart out of the bucket: the catalog holds the tables and the row
        // store holds their rows, so a directory restored from `data/` alone
        // has block files that belong to no table (measured: the table was
        // simply absent after the local files were wiped).
        if let Some(client) = self.context.object_store.lock().unwrap().clone() {
            storage::s3::sync_tree(&client, &self.dir.join("catalog"), "catalog")?;
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
        let _ = self.txn.append_checkpoint_flushed();
        let _ = self.txn.rotate_wal_if_large();
        self.txn.sync_wal()?;
        Ok(())
    }

    /// C++: a checkpoint with object storage — persist locally, then mirror the
    /// catalog and row store next to the C++-format `data` tree that
    /// [`Self::flush`] already uploads. No-op upload when no store is bound.
    pub fn checkpoint_to_object_store(&self) -> Result<()> {
        self.flush()?;
        if let Some(client) = self.context.object_store.lock().unwrap().clone() {
            storage::s3::sync_tree(&client, &self.dir.join("catalog"), "catalog")?;
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
        Ok(QueryResult::Rows {
            columns: vec!["value".to_string()],
            rows: vec![vec![Value::Str(value)]],
        })
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
        Ok(QueryResult::Rows {
            columns: vec![
                "database".to_string(),
                "dir".to_string(),
                "comment".to_string(),
            ],
            rows: text_rows(rows),
        })
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
        Ok(QueryResult::Rows {
            columns: vec![
                "database".to_string(),
                "table".to_string(),
                "id".to_string(),
                "column_count".to_string(),
                "segment_count".to_string(),
                "block_count".to_string(),
                "create_ts".to_string(),
                "comment".to_string(),
            ],
            rows: text_rows(rows),
        })
    }

    /// Rotates and prunes the WAL without touching the KV store. Called by the
    /// periodic checkpoint loop so a slow `flush` cannot let the log grow
    /// unbounded; only fsynced records are dropped.
    pub fn checkpoint_wal(&self) {
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
        let _ = self.txn.append_checkpoint_flushed();
        // A checkpoint keeps its sealed segment; `Cleanup` reclaims it, as in
        // `WalManager::Cleanup`.
        let _ = self.txn.rotate_wal_if_large();
        // The frame claims only records a flush covered, so it may be synced on
        // its own.
        let _ = self.txn.sync_wal();
    }

    /// Runs the reference's optimize pass over the index mirrors: a mirror
    /// whose newest chunk does not cover every live row gets a merged chunk.
    pub fn optimize_indexes(&self) -> Result<()> {
        // The optimize task only runs on a writable node
        // (`optimization_process_impl.cpp:174-185`).
        self.ensure_writable()?;
        // An optimize rewrites the index chunks; a dump that was staged before
        // it no longer matches the index state.
        self.index_epoch
            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        self.optimize_index_chunks()
    }
}

use super::*;

/// A block's rows as the mirror stores them: the column texts, then
/// `(row id, cells)` per row.
pub(crate) type BlockRows = (Vec<String>, Vec<(usize, Vec<String>)>);

/// A block's visible rows with the cells already decoded: `(row id, cells)`.
pub(crate) type DecodedRows = Vec<(usize, Vec<Value>)>;

/// Same as [`BlockRows`], with the cells already decoded to `Value`.
pub(crate) type BlockValueRows = (Vec<String>, DecodedRows);

/// A row predicate: `true` keeps the row.
pub(crate) type RowFilter<'a> = Option<&'a dyn Fn(&[Value]) -> bool>;

/// One block's share of a scan, after the version filter.
///
/// The two failures are kept apart because the scan distinguishes them: a block
/// that will not *open* at the head of the table means the table has no blocks
/// at all, while a block that opened but would not *decode* only ends the walk.
pub(crate) enum BlockDecode {
    Rows(DecodedRows),
    OpenFailed,
    DecodeFailed,
}

/// The column ids a row with `ts` was written with, in schema order.
///
/// `timeline` is the table's schema history (`Catalog::column_timeline`),
/// ascending; `current` is what a row newer than every recorded change was
/// written with, and the fallback when the history does not reach back far
/// enough.
fn ids_at(timeline: &[(u64, Vec<u64>)], ts: u64, current: &[u64]) -> Vec<u64> {
    match timeline
        .iter()
        .rev()
        .find(|(change_ts, _)| *change_ts <= ts)
    {
        Some((_, ids)) => ids.clone(),
        None => current.to_vec(),
    }
}

/// Rewrites one replayed row onto the table's current columns.
///
/// The frame's cells follow `frame_ids` - the column ids that were live when
/// the row was written - while the append buffer (and every block) is
/// positional against the current schema. Matching by id is what makes a row
/// survive a schema change: a cell whose column was dropped is discarded, and
/// a column the row predates takes the default it was added with. Counting
/// positions instead put a dropped column's value into its neighbour.
fn reorder_row_text(
    text: &str,
    frame_ids: &[u64],
    current_ids: &[u64],
    defaults: &[Option<String>],
) -> String {
    if frame_ids == current_ids {
        return text.to_string();
    }
    let cells: Vec<&str> = text.split('\t').collect();
    let mut out: Vec<String> = Vec::with_capacity(current_ids.len());
    for (position, id) in current_ids.iter().enumerate() {
        match frame_ids.iter().position(|frame| frame == id) {
            Some(at) => out.push(cells.get(at).copied().unwrap_or_default().to_string()),
            None => out.push(default_cell_text(
                defaults.get(position).cloned().flatten().as_deref(),
            )),
        }
    }
    out.join("\t")
}

/// One row's cells, either owned (decoded from a block, or a replayed journal
/// entry) or borrowed from the write's row set.
///
/// The mirror copies nothing it does not have to: a bulk `COPY` publishes its
/// rows and mirrors them straight after, so every cell is already in `extra`
/// and borrowing is the whole win (18% of the profile was `Vec<Value>::clone`).
pub(crate) enum RowCells<'a> {
    Borrowed(&'a Vec<Value>),
    Owned(Vec<Value>),
}

impl std::ops::Deref for RowCells<'_> {
    type Target = Vec<Value>;
    fn deref(&self) -> &Vec<Value> {
        match self {
            RowCells::Borrowed(values) => values,
            RowCells::Owned(values) => values,
        }
    }
}

/// Calls `each` for every row of `rows` whose id falls in `[first, last)`.
///
/// `rows` is the write's whole row set, sorted by row id (the append buffer is,
/// and it is what a checkpoint passes), so a block's window is a contiguous
/// slice found by two binary searches - no scan, no per-row range check.
///
/// `ordered` is the caller's verdict on the whole slice, computed **once** per
/// write rather than per block: verifying it here walked each 8192-row window
/// again, and the block loop runs 1024 times on an 8 M-row `COPY`.
fn for_rows_in_window<'a>(
    rows: &'a [(usize, Vec<Value>)],
    first: usize,
    last: usize,
    ordered: bool,
    each: impl FnMut(&'a (usize, Vec<Value>)),
) {
    if rows.is_empty() {
        return;
    }
    if ordered {
        let start = rows.partition_point(|(seq, _)| *seq < first);
        let end = rows.partition_point(|(seq, _)| *seq < last);
        rows[start..end].iter().for_each(each);
    } else {
        rows.iter()
            .filter(|(seq, _)| *seq >= first && *seq < last)
            .for_each(each);
    }
}

/// Whether `rows` is sorted by row id, i.e. whether the window lookup is a
/// binary search rather than a filter.
fn is_id_ordered(rows: &[(usize, Vec<Value>)]) -> bool {
    rows.windows(2).all(|w| w[0].0 <= w[1].0)
}

/// The stored text form of a column default: the same value `AddColumnsData`
/// writes into the block, so the buffered row and the block agree.
fn default_cell_text(default: Option<&str>) -> String {
    match default {
        Some(text) => mirror_cell_text(&Value::from_literal(&default_literal(text))),
        None => mirror_cell_text(&Value::Null),
    }
}

impl Engine {
    /// Root of the C++-format block mirror. The C++ engine keeps the table
    /// data under `<data_dir>/data/db_{db}/…`, next to `<data_dir>/catalog`.
    pub(crate) fn blocks_root(&self) -> PathBuf {
        self.dir.join("data")
    }

    /// Qualifies an unqualified table name with the session database, which is
    /// how the catalog and row keys are spelled.
    pub(crate) fn qualified_table(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.current_db.lock().unwrap(), table)
        }
    }

    /// The rows a mirror rewrite should write, in row-id order.
    ///
    /// The block files are the authoritative copy of every row the current
    /// mirror covers, so a rewrite takes those rows from the blocks and only
    /// the rows appended since (or never mirrored) from the row journal. That
    /// keeps a commit-time re-mirror idempotent: it must not copy the journal
    /// back over cells the blocks already hold.
    /// One block's rows: the journal's values for the ids in the block's
    /// window, with the block already on disk winning for the ids it holds.
    ///
    /// The window is a contiguous run of `DEFAULT_BLOCK_CAPACITY` row ids
    /// (`segment << 32 | block * capacity + n`), which is exactly what the
    /// reference's block covers. Reading it this way is what bounds the mirror's
    /// memory: a table is mirrored block by block, the way the reference streams
    /// the rows through its buffer manager.
    #[allow(clippy::too_many_arguments)] // one argument per piece of the block layout it mirrors
    pub(crate) fn mirror_block_rows<'a>(
        &self,
        table: &str,
        meta: &catalog::TableMeta,
        types: &[i32],
        segment_id: u32,
        block_index: u16,
        start: u64,
        end: u64,
        extra: &'a [(usize, Vec<Value>)],
        extra_ordered: bool,
    ) -> Result<Vec<(usize, RowCells<'a>)>> {
        let first = ((segment_id as usize) << 32) | start as usize;
        let last = ((segment_id as usize) << 32) | end as usize;
        // The write's row set already carries the append buffer: `extra` is
        // `merged`, which is `pending_rows` merged with the caller's rows. So a
        // block's rows are a window of `extra`, and asking `pending_range` for
        // them as well copied the same cells a second time - the profile showed
        // `Vec<Value>::clone` at 18%, and half of it was this second copy.
        // The window as an ordered `Vec`, not a `BTreeMap`: `extra_ordered`
        // says the rows already come sorted, so a map only adds one tree insert
        // per row (14% of the profile on an 8 M-row import, 1024 blocks over).
        let mut journal: Vec<(usize, &'a Vec<Value>)> = Vec::new();
        for_rows_in_window(extra, first, last, extra_ordered, |(seq, values)| {
            journal.push((*seq, values));
        });
        // The mirror is the authoritative copy of the rows it already holds
        // (`try_rows` prefers it for the same reason): a re-mirror must not
        // copy a lossy journal back over cells the blocks already have.
        let id = BlockId {
            db_id: self.database_id_of(table),
            table_id: meta.id,
            segment_id,
            block_id: block_index,
        };
        let root = self.blocks_root();
        // A column the block never got a file for is repaired before it is read:
        // the decode below treats a current column without data as unreadable,
        // not as empty.
        self.ensure_block_columns(table, meta, id)?;
        // A block that is not on disk yet is empty; a block that *is* there and
        // cannot be decoded is an error. Treating the second as the first made a
        // rewrite drop every row the block held, and the next write then failed
        // its "the chunk tiles the committed window" check.
        let version_file = root
            .join(storage::definition::format::block_dir(
                id.db_id,
                id.table_id,
                id.segment_id,
                id.block_id,
            ))
            .join(storage::definition::format::VERSION_FILE);
        let held = match self.block_map(table, &root, id) {
            Ok(block) => Self::decode_rows_of(
                &meta.ids,
                &meta.types,
                types,
                id,
                block.version(),
                block.as_ref(),
                None,
            )
            .ok_or_else(|| {
                Error::Invalid(format!(
                    "table '{table}': block {segment_id}.{block_index} could not be decoded"
                ))
            })?,
            Err(error) if version_file.exists() => {
                return Err(Error::Storage(format!(
                    "table '{table}': block {segment_id}.{block_index} exists but cannot be opened: {error}"
                )));
            }
            Err(_) => Vec::new(),
        };
        let mut out: Vec<(usize, RowCells<'a>)> = Vec::with_capacity(held.len().max(journal.len()));
        // The block wins for the ids it already holds (a re-mirror must not copy
        // a journal back over cells the block has), so its rows go in first and
        // the window skips them.
        let from_block: std::collections::HashSet<usize> =
            held.iter().map(|(seq, _)| *seq).collect();
        for (seq, cells) in held {
            out.push((
                seq,
                RowCells::Owned(cells.iter().map(|cell| parse_value(cell)).collect()),
            ));
        }
        // Everything else in the window: the rows this write is publishing.
        out.extend(
            journal
                .into_iter()
                .filter(|(seq, _)| !from_block.contains(seq))
                .map(|(seq, cells)| (seq, RowCells::Borrowed(cells))),
        );
        out.sort_unstable_by_key(|(seq, _)| *seq);
        Ok(out)
    }

    /// Writes `table`'s rows into `{data_dir}/data/db_{db}/tbl_{id}/seg_0/…`
    /// in the C++ block layout, returning the number of rows written.
    ///
    /// Rows are chunked `DEFAULT_BLOCK_CAPACITY` (8192) per block, as C++ does.
    /// Async because its one production caller - a snapshot - runs on a
    /// runtime worker, and the block write takes the table's bottom gate.
    /// Synchronous callers (unit tests) use `block_on`.
    pub async fn write_blocks(&self, table: &str) -> Result<usize> {
        self.write_blocks_bounded_async(table, self.checkpoint_ts())
            .await
    }

    /// [`Self::write_blocks_bounded`] for an async caller: it awaits the
    /// table's bottom gate instead of blocking on it.
    pub async fn write_blocks_bounded_async(
        &self,
        table: &str,
        checkpoint_ts: u64,
    ) -> Result<usize> {
        let gate = self.bottom_gate(table);
        let _turn = gate.lock().await;
        self.write_blocks_mode(table, &[], false, checkpoint_ts)
    }

    /// [`Self::write_blocks`] with the checkpoint's upper bound spelled out.
    ///
    /// The reference's checkpoint writes the rows of the transactions that had
    /// finished at its `begin_ts` (`NewTxn::Checkpoint` -> `GetRowCnt1` ->
    /// `BlockVersion::GetRowCount(begin_ts)`), so a row id handed out by a
    /// transaction still in flight stays out of the files.
    pub fn write_blocks_bounded(&self, table: &str, checkpoint_ts: u64) -> Result<usize> {
        // The reference's checkpoint flushes only the blocks whose data changed
        // since the last one (NewTxn::CheckpointTable writes a block when
        // block_lock->checkpoint_ts_ < min(checkpoint_ts, max_ts)), and a commit
        // writes only the blocks it appended to. So a bounded write is partial
        // too: the touched set is the blocks holding rows or deletes it carries.
        self.write_blocks_mode(table, &[], false, checkpoint_ts)
    }

    /// [`Self::write_blocks_bounded`] for a caller that **already holds** the
    /// table's bottom gate (a checkpoint's per-table step).
    pub(crate) fn write_blocks_bounded_locked(
        &self,
        table: &str,
        checkpoint_ts: u64,
    ) -> Result<usize> {
        self.write_blocks_mode(table, &[], false, checkpoint_ts)
    }

    /// Writes only the blocks this commit's rows and deletes land in.
    ///
    /// That is the reference's `AppendInBlock` / `DeleteInBlock`: a `COPY` of a
    /// few rows into a growing segment touches one block, not the table.
    /// Writes the blocks this commit's rows landed in.
    ///
    /// **The caller already holds the table's bottom gate** - both commit
    /// bottoms (`commit_import_chunk`, the binary import's tail) take it before
    /// calling - so this must not take it again: the gate is a
    /// `tokio::sync::Mutex` and re-entering it self-locks. (The `HeldBottom`
    /// marker cannot be trusted here: one of these callers runs inside a
    /// `before_wal` callback that may execute on a different thread than the
    /// one that set it.)
    pub(crate) fn write_blocks_commit(&self, table: &str, ts: u64) -> Result<usize> {
        self.write_blocks_mode(table, &[], false, ts)
    }

    /// [`Self::write_blocks_commit`] for an **import**, whose blocks the
    /// reference keeps in memory: the files are written (so `SHOW BLOCKS` and a
    /// reader see them) but not fsynced, and the next checkpoint is what makes
    /// them durable. Matches `WalCmdImportV2`, which carries the extent and
    /// no rows.
    pub(crate) fn write_blocks_import(&self, table: &str, ts: u64) -> Result<usize> {
        self.write_blocks_mode_with(table, &[], false, ts, false)
    }

    /// `full` rewrites every block; otherwise only the blocks this commit
    /// touched - the ones `extra`'s row ids land in and the ones that hold a
    /// slot the table's delete log marks (`AppendInBlock` / `DeleteInBlock`,
    /// `new_txn_data_impl.cpp:923` / `:1919`).
    pub(crate) fn write_blocks_mode(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
        full: bool,
        checkpoint_ts: u64,
    ) -> Result<usize> {
        self.write_blocks_mode_with(table, extra, full, checkpoint_ts, true)
    }

    /// [`Self::write_blocks_mode`] with control over the file sync.
    ///
    /// A commit that is an import passes `false`: the reference leaves those
    /// blocks in memory and its `IMPORT_V2` frame carries only the extent, so
    /// the checkpoint is what puts them on disk. Every other caller - a
    /// checkpoint, a compaction, a column rewrite - passes `true`.
    pub(crate) fn write_blocks_mode_with(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
        full: bool,
        checkpoint_ts: u64,
        sync_files: bool,
    ) -> Result<usize> {
        // The block writer runs under the **table's bottom gate**: the reference
        // writes a block's rows inside the table's bottom half (`AppendInBlock`),
        // so a checkpoint and an append of one table never write at the same
        // time while different tables proceed in parallel.
        //
        // A caller that is already the table's bottom must not re-enter the
        // gate. That is decided **by the caller**, not by `HeldBottom`: the
        // thread-local marker only describes the thread that entered it, and a
        // checkpoint's write now runs on a blocking-pool thread, where the
        // marker is absent - asking it there would take the gate a second time
        // and self-lock. `write_blocks_bounded` is the checkpoint entry: it
        // takes the gate. `write_blocks_flat` is for a caller that already
        // holds it (`flush_table`, the commit bottoms).
        // **No lock is taken here**: every caller already holds the table's
        // bottom gate. `write_blocks_mode` runs on a blocking thread (a
        // checkpoint leaf, a commit's bottom half, a snapshot's file copy), so
        // it must never touch a `tokio::sync::Mutex` - taking the gate is the
        // caller's job, and the async entry
        // (`write_blocks_bounded_async`) awaits it.
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        // The checkpoint writes the schema **as of its bound** (`NewTxn::Checkpoint`
        // reads `GetColumnDefs` at its begin ts): a column a transaction
        // committed after that bound has no cells in these rows and no place in
        // their files. The sweep below still uses the live schema, so the
        // `{id}.col` of such a column stays where the ALTER put it.
        let snapshot_ids = self
            .storage
            .catalog
            .schema_ids_at(&table, checkpoint_ts)
            .unwrap_or_else(|_| meta.ids.clone());
        let encode_meta = meta.at_ids(&snapshot_ids);
        let types: Vec<i32> = encode_meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block writer"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let layouts: Vec<ColumnLayout> = encode_meta
            .types
            .iter()
            .zip(&types)
            .map(|(t, ct)| {
                layout_of(t, *ct)
                    .ok_or_else(|| Error::Invalid(format!("no .col layout for column type '{t}'")))
            })
            .collect::<Result<_>>()?;

        // Every row the table ever wrote, *including* the deleted ones: the
        // reference's block is append-only, so a deleted row's cells stay in
        // the block and only the `version` file's `deleted_` array hides it
        // (`BlockVersion::Delete`). An UPDATE is a delete plus an append, so
        // its rewritten row sits at the end with a fresh id.
        //
        // The rows come one **block** at a time (`mirror_block_rows`), the way
        // the reference streams them through its buffer manager, so the
        // mirror's memory is a block and not a table. Rows the previous mirror
        // already covers keep their *block* values: the mirror is the
        // authoritative copy for them (a table can be re-mirrored on every
        // commit, and the row journal is allowed to be a lossy cache of the
        // same rows - `try_rows` prefers the blocks for exactly that
        // reason). New rows come from the journal.
        let mut block_rows: Vec<usize> = Vec::new();
        // `BlockVersion::Append` boundaries: one `create` per append, sized by
        // how many of that append's rows land in the block. Each carries the
        // append transaction's commit ts, like the reference
        // (`BlockVersion::Append(commit_ts, …)`, `new_txn_data_impl.cpp:957`).

        // Deleted row id -> the commit ts of the transaction that deleted it,
        // so the version file marks the block slot that holds it
        // (`BlockVersion::Delete`). A delete that commits after the checkpoint's
        // bound is not part of this write: the row is still visible at that ts
        // (`BlockVersion::save_to_file` drops the same entries).
        let delete_ts = self.delete_ts(&table);
        let deleted_at: std::collections::HashMap<usize, u64> = self
            .deleted_ids(&table)
            .iter()
            .enumerate()
            .map(|(k, id)| {
                let ts = delete_ts.get(k).copied().unwrap_or(0);
                (*id as usize, if ts == 0 { DELETED_MIRROR_TS } else { ts })
            })
            .filter(|(_, ts)| checkpoint_ts == u64::MAX || *ts <= checkpoint_ts)
            .collect();
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        let tkey = self.table_key(&table);
        // Everything the commits since the last checkpoint appended is in the
        // table's append buffer; this write is the checkpoint, so it takes them
        // all. `extra` carries the rows of a rewrite (a column change, a
        // replay, a compaction) that no commit buffered.
        let layout_total: u64 = self
            .segment_row_counts(&table)
            .iter()
            .map(|(_, rows)| *rows)
            .sum();
        // Nothing moved since the files were written: they are already the
        // newest copy, and re-encoding them would size the NULL bitmap from the
        // final row set instead of the append window the file was written with.
        let dirty = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).map(|e| e.mirror_dirty).unwrap_or(false)
        };
        if !dirty
            && self
                .block_index_for(&table)
                .map(|index| !index.lock().unwrap().is_empty())
                .unwrap_or(false)
        {
            return Ok(layout_total as usize);
        }
        // The write counter as it stands before any row is read: a commit that
        // lands while the mirror is being written bumps it, and the flag is only
        // cleared when nothing did (otherwise the new rows would look covered).
        //
        // `mirrored` is how many rows the files held after the last write. A
        // commit can only be *incremental* when that is known (a directory
        // opened afresh has no count until the first rewrite) and the table's
        // row set is the previous one plus this batch - otherwise another
        // transaction reserved ids that are not on disk yet, and the read side
        // has to keep merging the journal.
        let (epoch, prev_rows) = {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.block_index = None;
            let prev = entry.block_rows.clone();
            entry.block_rows = None;
            (entry.mirror_epoch, prev)
        };
        // A whole-table write (a checkpoint, a restore, a column rewrite) rebuilds
        // every block. A commit only rewrites the blocks its rows and deletes
        // land in - the reference's AppendInBlock / DeleteInBlock.
        let partial = !full;
        // The mirror's file writes are serialised; the table lock is **not**
        // held across them (a commit takes the bottom gate before this, and the
        // checkpoint takes the table lock on its own, so holding both here in
        // the other order deadlocks the two threads).
        // Per table: everything this rewrite touches is that table's
        // directories and records, so another table proceeds alongside. The
        // registry lock is released before this one is taken.
        let mirror_lock = crate::blocks::misc::mirror_write_lock(&tkey);
        let _serialised = mirror_lock
            .lock()
            .unwrap_or_else(|poisoned| poisoned.into_inner());
        // Taken under the lock: a checkpoint that queued behind this one must
        // not see a stale snapshot of the append buffer, or its own rows would
        // be dropped from the files by the write that lands last.
        let buffered = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .map(|e| e.pending_rows.clone())
                .unwrap_or_default()
        };
        let mut merged: Vec<(usize, Vec<Value>)> = extra.to_vec();
        if !buffered.is_empty() {
            let have: std::collections::HashSet<usize> =
                merged.iter().map(|(seq, _)| *seq).collect();
            merged.extend(buffered.into_iter().filter(|(seq, _)| !have.contains(seq)));
        }
        let extra_ordered = is_id_ordered(&merged);
        let extra: &[(usize, Vec<Value>)] = &merged;
        let mut written = 0usize;
        // A row id carries its segment in the high 32 bits (`segment_id << 32 |
        // in-segment offset`). The reference writes one `seg_{id}/` directory
        // per segment, with the block ids restarting at 0, and a segment's
        // blocks tile `[0, rows)` - so the allocator's per-segment counts are
        // enough to know every block's id window without reading the table.
        let mut segments: Vec<SegmentRecord> = Vec::new();
        let mut global_pos = 0usize;
        // The append log and the segment map move under other commits: read
        // them under the write lock, or a checkpoint that queued behind another
        // one records `create` boundaries that stop short of the rows it wrote.
        let totals = self.append_totals(&table);
        let append_ts = self.append_ts(&table);
        let mut counts = self.segment_row_counts(&table);
        // The reference decides per block, by ts: a block is flushed when
        // `record_ts < min(checkpoint_ts, block_max_ts)` where `record_ts` is
        // the ts at which the block was last written and `block_max_ts` the
        // newest change it holds (NewTxn::CheckpointTable).
        let touched: std::collections::HashSet<(u32, u16)> = if partial {
            let mut newest: std::collections::HashMap<(u32, u16), u64> = Default::default();
            for (seq, _) in &merged {
                let key = block_of(*seq);
                let offset = (*seq & 0xffff_ffff) as u64;
                let ts = append_ts_of(offset, &totals, &append_ts).unwrap_or(u64::MAX);
                let slot = newest.entry(key).or_insert(ts);
                *slot = (*slot).max(ts);
            }
            for id in deleted_at.keys() {
                let key = block_of(*id);
                let ts = deleted_at[id];
                let slot = newest.entry(key).or_insert(ts);
                *slot = (*slot).max(ts);
            }
            let mut set = std::collections::HashSet::new();
            for (key, ts) in newest {
                let record = self
                    .storage
                    .catalog
                    .store()
                    .get(
                        storage::catalog_format::catalog_block_key(
                            &db_id.to_string(),
                            &meta.id.to_string(),
                            key.0,
                            key.1,
                        )
                        .as_bytes(),
                    )?
                    .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
                    // No record means the block was never written: it has to be.
                    .unwrap_or(0);
                if record < checkpoint_ts.min(ts) {
                    set.insert(key);
                }
            }
            set
        } else {
            std::collections::HashSet::new()
        };
        if counts.is_empty() && !merged.is_empty() {
            // The allocator's per-segment map is per-process; a table the
            // catalog just re-registered (a DDL, a rename) has none. The rows
            // the commit buffered still say how far each segment goes.
            let mut by_segment: std::collections::BTreeMap<u32, u64> = Default::default();
            for (seq, _) in &merged {
                let segment = (*seq >> 32) as u32;
                let offset = (*seq as u64 & 0xffff_ffff) + 1;
                by_segment
                    .entry(segment)
                    .and_modify(|e| *e = (*e).max(offset))
                    .or_insert(offset);
            }
            counts = by_segment.into_iter().collect();
        }
        counts.sort_unstable();
        // Every segment the table holds **before** the clip below. A commit that
        // landed after this write's bound has no block in it yet, and its
        // directory must survive the sweep that drops segments the rewrite did
        // not produce - otherwise a checkpoint that started before an import
        // committed deleted the block the import had just written, and the
        // segment was left with rows and no file.
        let known_segments: Vec<u32> = counts.iter().map(|(segment, _)| *segment).collect();
        // `NewTxn::Checkpoint` writes the rows of the transactions that had
        // finished by `checkpoint_ts`: a block holds the rows whose append was
        // stamped at or before it (`GetRowCnt1` -> `GetRowCount(begin_ts)`).
        // The allocator's per-segment count also covers ids handed out to a
        // transaction that has not committed yet, and those rows are in no
        // block, so the extent is clipped here. Because ids are allocated in
        // commit-ts order, the committed records are a prefix of the row ids.
        if checkpoint_ts != u64::MAX {
            let committed = self.committed_rows(&table, checkpoint_ts);
            let mut seen = 0usize;
            let mut clipped: Vec<(u32, u64)> = Vec::with_capacity(counts.len());
            for (segment_id, rows) in counts {
                let take = committed.saturating_sub(seen).min(rows as usize);
                seen += rows as usize;
                if take > 0 {
                    clipped.push((segment_id, take as u64));
                }
            }
            counts = clipped;
        }
        for (segment_id, seg_rows_total) in counts {
            let seg_start = global_pos;
            global_pos += seg_rows_total as usize;
            let mut blocks: Vec<BlockRecord> = Vec::new();
            // The ts the segment was created with: the commit ts of the append
            // that opened it (`TableMeta::AddSegmentWithID(commit_ts, …)`).
            let mut seg_create_ts = CPP_MIRROR_TS;
            // The reference's prefilter (`BuildFastRoughFilterTask`): one per
            // block and one per segment, built when the segment is **sealed**.
            // An import seals every segment it commits
            // (`PrepareCommitImport` -> `ExecuteOnNewSealedSegment`), an
            // `INSERT` only seals one that reaches `DEFAULT_SEGMENT_CAPACITY`
            // (`CommitBottomAppend`), and an unsealed segment has no filter at
            // all. A whole-table rewrite can leave fewer blocks than the
            // previous one, so it starts clean; a commit only rebuilds the tags
            // of the block it wrote.
            let block_count = (seg_rows_total as usize).div_ceil(DEFAULT_BLOCK_CAPACITY);
            let sealed = self
                .imported_extent(&table, segment_id, seg_rows_total)
                .is_some()
                || seg_rows_total as usize == Self::SEGMENT_ROW_CAPACITY;
            // A segment that is sealed by *this* write needs the prefilter of
            // every one of its blocks (`ExecuteOnNewSealedSegment` walks the
            // whole segment), so that write falls back to rewriting the
            // segment. "Sealed by this write" is the segment-level record
            // being absent: a segment sealed by an import may have had its
            // first blocks mirrored by an earlier checkpoint, and if this write
            // only touches the last one, the incremental path would leave those
            // blocks without a `blk|…|fast_rough_filter` record the reference
            // has.
            let last_block = block_count.saturating_sub(1) as u16;
            let already_recorded = self
                .storage
                .catalog
                .store()
                .get(
                    format!("seg|{}|{}|{}|fast_rough_filter", db_id, meta.id, segment_id)
                        .as_bytes(),
                )?
                .is_some();
            let seals_now = sealed && !already_recorded;
            let seg_partial =
                partial && !seals_now && !(sealed && touched.contains(&(segment_id, last_block)));
            for (k, _) in self
                .storage
                .catalog
                .store()
                .scan_prefix(format!("blk|{}|{}|{}|", db_id, meta.id, segment_id).as_bytes())?
            {
                // The prefix also covers the block's `fast_rough_filter` tag.
                // It is not a block record (its key has no block id in the
                // record's position, and `nth(5)` reads the record's tail), so
                // the sweep below used to delete the filter of every block this
                // write touched - the reference keeps them until the segment is
                // re-sealed.
                if k.ends_with(b"|fast_rough_filter") {
                    continue;
                }
                if seg_partial {
                    let block_id = String::from_utf8_lossy(&k)
                        .split('|')
                        .nth(5)
                        .and_then(|s| s.parse::<u16>().ok());
                    if block_id.is_some_and(|b| {
                        (b as usize) < block_count && !touched.contains(&(segment_id, b))
                    }) {
                        continue;
                    }
                }
                self.storage.catalog.store().delete(&k)?;
            }
            let mut seg_filters = crate::fast_rough_filter::Filters::new(&encode_meta.types);
            // Drain the segment block by block so each block's values are freed
            // once it is encoded: holding the whole table's decoded rows while
            // the texts are rendered doubled the transient on a long import.
            for block_index in 0..block_count {
                if seg_partial && !touched.contains(&(segment_id, block_index as u16)) {
                    // Untouched: its files and its `version` already say
                    // everything this commit would write, so only its catalog
                    // record is republished, with the ts it already carries.
                    let ts = self
                        .storage
                        .catalog
                        .store()
                        .get(
                            storage::catalog_format::catalog_block_key(
                                &db_id.to_string(),
                                &meta.id.to_string(),
                                segment_id,
                                block_index as u16,
                            )
                            .as_bytes(),
                        )?
                        .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
                        .unwrap_or(CPP_MIRROR_TS);
                    if block_index == 0 {
                        seg_create_ts = ts;
                    }
                    blocks.push(BlockRecord {
                        block_id: block_index as u16,
                        commit_ts: ts,
                    });
                    continue;
                }
                // `block_start` stays in the table-wide append order the
                // version batches are recorded in; the slot a deleted row
                // occupies is inside its own segment's block.
                let start = block_index * DEFAULT_BLOCK_CAPACITY;
                let end = (start + DEFAULT_BLOCK_CAPACITY).min(seg_rows_total as usize);
                let slot_base = seg_start + start;
                // Drop the mapping of this block **before** the rows are
                // merged: a mapped view of an older (shorter) file would make
                // the merge miss the rows the file holds, and the rewrite
                // would drop them.
                self.drop_block_map(BlockId {
                    db_id,
                    table_id: meta.id,
                    segment_id,
                    block_id: block_index as u16,
                });
                let chunk = self.mirror_block_rows(
                    &table,
                    &encode_meta,
                    &types,
                    segment_id,
                    block_index as u16,
                    start as u64,
                    end as u64,
                    extra,
                    extra_ordered,
                )?;
                // A block is addressed by slot: row *n* of block *b* is id
                // `segment << 32 | b * capacity + n`. The window is the
                // committed prefix, and ids are handed out in commit-ts order
                // (`SubmitForAllocation`), so the chunk must tile it exactly;
                // anything else means the row buffer lost a row, which must not
                // be written to disk as a shifted block.
                let first_id = ((segment_id as usize) << 32) | start;
                // Every id from `first_id` on has to be the next slot: a hole
                // means the row buffer lost a row, which must not reach disk as
                // a shifted block. A short tail is a **commit in flight**, not
                // a lost row: the append record is stamped while the
                // transaction commits (`reserve_rows_at`) and the rows join the
                // buffer afterwards, in the commit's bottom half
                // (`publish_encoded_rows` -> `AppendInBlock`), so a checkpoint
                // that reads the log between the two sees a window a few rows
                // wider than the data. The block is written for the rows that
                // exist; the rest stay buffered and the next checkpoint writes
                // them (the buffer keeps every row this write did not take).
                //
                // An **empty** block over a non-empty window is still an error:
                // that is not a racing tail, it is a window built from ids
                // nothing ever wrote.
                let contiguous = chunk
                    .iter()
                    .enumerate()
                    .all(|(slot, (seq, _))| *seq == first_id + slot);
                if !contiguous || chunk.len() > end - start || chunk.is_empty() {
                    return Err(Error::Invalid(format!(
                        "table '{table}': block {segment_id}.{block_index} holds {} of the {} rows \
                         committed at ts {checkpoint_ts} (ids {}..{}, window {first_id}..{})",
                        chunk.len(),
                        end - start,
                        chunk.first().map(|(seq, _)| *seq).unwrap_or(0),
                        chunk.last().map(|(seq, _)| *seq).unwrap_or(0),
                        first_id + (end - start)
                    )));
                }
                block_rows.extend(
                    chunk
                        .iter()
                        .map(|(seq, _)| *seq)
                        .filter(|seq| !deleted_at.contains_key(seq)),
                );
                let texts: Vec<Vec<String>> = chunk
                    .iter()
                    .map(|(_, row): &(usize, RowCells)| row.iter().map(mirror_cell_text).collect())
                    .collect();
                // A block opened by `COPY` keeps the reference's import-time
                // bitmap coverage (see `ColumnBody::build_with`).
                let bitmap_bits = self.block_bitmap_bits(&table, slot_base, chunk.len());
                let block_columns = self.encode_block_columns(
                    &encode_meta.types,
                    &encode_meta.ids,
                    &types,
                    &layouts,
                    &texts,
                    bitmap_bits,
                )?;
                let batches =
                    self.block_version_batches(&totals, &append_ts, slot_base, chunk.len());
                let mut version = BlockVersionFile::new(&batches, DEFAULT_BLOCK_CAPACITY);
                // The slot of a deleted row is its in-segment offset inside the
                // block (`BlockVersion::Delete`); the id window is the block's.
                for (seq, _) in &chunk {
                    if let Some(ts) = deleted_at.get(seq) {
                        version.delete(((*seq as u64 & 0xffff_ffff) - start as u64) as usize, *ts);
                    }
                }
                // The block's own record carries the ts of the append that
                // opened it, which is exactly the first `create` of its version
                // file (`SegmentMeta::AddBlockWithID(commit_ts, …)`).
                let block_ts = batches.first().map(|(ts, _)| *ts).unwrap_or(CPP_MIRROR_TS);
                if block_index == 0 {
                    seg_create_ts = block_ts;
                }
                let id = BlockId {
                    db_id,
                    table_id: meta.id,
                    segment_id,
                    block_id: block_index as u16,
                };
                // Only this block's file is about to be rewritten: its mapping
                // is stale, the rest of the table's are still good.
                self.drop_block_map(id);
                let block_dir =
                    storage::definition::format::block_dir(db_id, meta.id, segment_id, id.block_id);
                // A rewrite replaces the block's contents; a file the schema no
                // longer has (`ALTER TABLE … DROP COLUMN` leaves `1.col`
                // behind) must not survive next to it, or the reference reads
                // a column the catalog does not know.
                let mut keep: Vec<String> =
                    vec![storage::definition::format::VERSION_FILE.to_string()];
                // The live schema, not the snapshot: a column added after the
                // bound has a file here that this write does not produce.
                for (i, id) in meta.ids.iter().enumerate() {
                    keep.push(storage::definition::format::column_file(*id));
                    if meta
                        .types
                        .get(i)
                        .and_then(|t| column_type_of(t))
                        .is_some_and(is_var_buffer_type)
                    {
                        keep.push(storage::definition::format::column_outline_file(*id));
                    }
                }
                if let Ok(entries) = std::fs::read_dir(root.join(&block_dir)) {
                    for entry in entries.flatten() {
                        let name = entry.file_name().to_string_lossy().to_string();
                        if !keep.contains(&name) {
                            let _ = storage::chunk_store::remove_file(entry.path());
                        }
                    }
                }
                storage::definition::format::write_block_synced(
                    &root,
                    id,
                    &block_columns,
                    &version,
                    sync_files,
                )
                .map_err(|e| Error::Io {
                    path: root.join(&block_dir),
                    source: e,
                })?;
                // The reference opens an outline file for every variable-length
                // column (`ColumnMeta::FilePaths` lists `col_{id}_out` for a
                // kVarBuffer type) and aborts the whole read when it is missing,
                // even though the file is empty for a column whose data is inline.
                for (i, ty) in types.iter().enumerate() {
                    if is_var_buffer_type(*ty) {
                        let path = root.join(&block_dir).join(
                            storage::definition::format::column_outline_file(meta.ids[i]),
                        );
                        if !path.exists() {
                            let _ = storage::definition::format::write_synced(&path, b"");
                        }
                    }
                }
                if sealed {
                    let mut block_filters =
                        crate::fast_rough_filter::Filters::new(&encode_meta.types);
                    for (_, cells) in &chunk {
                        block_filters.add_row(cells);
                    }
                    // `segment_meta->begin_ts()` is the creating transaction's
                    // begin ts, one below its commit ts - the same for every
                    // block of the segment.
                    let build_time = seg_create_ts.saturating_sub(1);
                    if let Err(e) = self.storage.catalog.store().put(
                        format!(
                            "blk|{}|{}|{}|{}|fast_rough_filter",
                            db_id, meta.id, segment_id, block_index
                        )
                        .as_bytes(),
                        &block_filters.encode(build_time),
                    ) {
                        eprintln!("[FRF] put failed: {e}");
                    }
                    seg_filters.merge(&block_filters);
                }
                blocks.push(BlockRecord {
                    block_id: block_index as u16,
                    commit_ts: block_ts,
                });
                written += chunk.len();
            }
            // `first_delete_ts` is the commit ts of the first delete that
            // touched this segment (`SegmentMeta::CommitFirstDeleteTS`): a
            // reader whose `begin_ts` is below it skips the block's delete
            // flags, so leaving it at UNCOMMIT_TS resurrects deleted rows.
            let segment_deleted = self
                .deleted_ids(&table)
                .iter()
                .any(|id| (*id >> 32) as u32 == segment_id);
            let first_delete_ts = if segment_deleted {
                self.delete_ts(&table)
                    .into_iter()
                    .filter(|ts| *ts != 0)
                    .min()
                    .unwrap_or(UNCOMMIT_TS)
            } else {
                UNCOMMIT_TS
            };
            if !seg_partial && sealed && seg_create_ts != CPP_MIRROR_TS {
                let _ = self.storage.catalog.store().put(
                    format!("seg|{}|{}|{}|fast_rough_filter", db_id, meta.id, segment_id)
                        .as_bytes(),
                    &seg_filters.encode(seg_create_ts.saturating_sub(1)),
                );
            }
            segments.push(SegmentRecord {
                segment_id,
                create_ts: seg_create_ts,
                first_delete_ts,
                blocks,
            });
        }
        // Drop the segment directories this rewrite did not produce. They are
        // removed *after* the new blocks are on disk so a concurrent reader
        // never finds the table's only block missing; a compaction renumbers
        // the rows into a new segment id, and leaving the source segment
        // behind would let the reference read a second copy of the data.
        let tbl_dir = root.join(format!("db_{db_id}/tbl_{}", meta.id));
        // A segment a compaction deprecated keeps its directory: the reference
        // only writes `drop|seg|…` and leaves the files for `Cleanup`
        // (`NewTxn::Compact` -> deprecated segment list; `Cleanup` removes
        // them). Deleting it here would make a reader at an older ts lose rows.
        let dropped = self
            .storage
            .catalog
            .dropped_segments(&db_id.to_string(), &meta.id.to_string())
            .unwrap_or_default();
        let live: Vec<String> = segments
            .iter()
            .map(|segment| format!("seg_{}", segment.segment_id))
            .chain(dropped.iter().map(|segment| format!("seg_{segment}")))
            .chain(
                known_segments
                    .iter()
                    .map(|segment| format!("seg_{segment}")),
            )
            .collect();
        if let Ok(entries) = std::fs::read_dir(&tbl_dir) {
            for entry in entries.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                if name.starts_with("seg_") && !live.contains(&name) {
                    storage::chunk_store::remove_tree(entry.path());
                }
            }
        }
        // Publish the segment layer the C++ reader enumerates
        // (`catalog|seg|…` / `catalog|blk|…`, `GetTableSegments`): without it
        // the table opens with no segments and every query returns nothing.
        self.storage.catalog.publish_segments(&SegmentSet {
            db_id: db_id.to_string(),
            table_id: meta.id.to_string(),
            segments,
        })?;
        // The mirror has new block files; the table's block map must rescan.
        // Release the write lock before touching the table entry: the files are
        // on disk, and taking `tables` under `MIRROR_WRITE_LOCK` is what
        // inverts against the checkpoint's `tables` → mirror order.
        drop(_serialised);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey.clone()).or_default();
        entry.block_index = None;
        // A whole-table rewrite rebuilt the row set from scratch; a commit only
        // wrote the blocks it touched, so its row set is the previous one with
        // this batch's rows added and its deletes removed.
        let written_ids: std::collections::HashSet<usize> = block_rows.iter().copied().collect();
        // A whole-table write replaces the row set; a commit adds the rows it
        // wrote to the previous set and drops the ones it deleted.
        let rows_now: Vec<usize> = if partial {
            let mut rows: Vec<usize> = prev_rows
                .as_ref()
                .map(|rows| rows.as_ref().clone())
                .unwrap_or_default();
            rows.extend(block_rows.iter().copied());
            rows.retain(|id| !deleted_at.contains_key(id));
            rows.sort_unstable();
            rows.dedup();
            rows
        } else {
            block_rows.clone()
        };
        entry.block_rows = Some(std::sync::Arc::new(rows_now.clone()));
        // What the files hold now: the row set this write walked.
        entry.mirror_rows = Some(rows_now.len() as u64);
        // The mark retires only when this write took **every** row it was
        // handed: a row outside the segment windows it walked (`counts` and the
        // append buffer disagreeing) is still only in memory, and a commit that
        // landed mid-flush is in the buffer too - the next read merges it.
        // Drop exactly the rows this write put in a block - `block_rows` is that
        // set, and a row the segment windows did not reach is not in it, so it
        // stays buffered for the next checkpoint. Clearing by the *input* set
        // would drop a row whose window was short; clearing the whole buffer
        // would drop the rows a concurrent commit added meanwhile.
        entry.retain_buffered(|(seq, _)| !written_ids.contains(seq));
        // The mark retires only when nothing is left to write and no commit
        // landed while this one ran.
        let covered = entry.mirror_epoch == epoch && entry.pending_rows.is_empty();
        entry.mirror_dirty = !covered;
        drop(tables);
        Ok(written)
    }

    /// The `BlockVersion::Append` batches for one block: one `(ts, rows)` per
    /// append, sized by how many of that append's rows the block holds.
    ///
    /// `totals` is the cumulative row count after every append and `ts` the
    /// commit ts of the transaction that made it; the two are aligned
    /// (`settle_append_ts`). `totals` is a hint for the mirror rather than
    /// committed state, so when it disagrees with the rows actually mirrored
    /// (a table written before the log existed, or an append the process lost)
    /// the block gets one create for all of them, which is what the mirror
    /// always wrote.
    pub(crate) fn block_version_batches(
        &self,
        totals: &[u64],
        ts: &[u64],
        block_start: usize,
        rows_in_block: usize,
    ) -> Vec<(u64, u32)> {
        let end = block_start + rows_in_block;
        // An append the previous process lost, or one made before the ts log
        // existed, has no commit ts of its own. The newest known ts is the
        // closest stand-in: it keeps the create visible to a reader at the
        // current ts, and puts it where the append order says it belongs.
        let fallback = ts
            .iter()
            .rev()
            .copied()
            .find(|t| *t != 0)
            .unwrap_or(CPP_MIRROR_TS);
        let mut batches: Vec<(u64, u32)> = Vec::new();
        let mut prev = 0u64;
        for (i, &total) in totals.iter().enumerate() {
            if total <= prev {
                continue;
            }
            let lo = prev.max(block_start as u64);
            let hi = total.min(end as u64);
            if hi > lo {
                let batch_ts = match ts.get(i).copied().unwrap_or(0) {
                    0 => fallback,
                    t => t,
                };
                batches.push((batch_ts, (hi - lo) as u32));
            }
            prev = total;
            if total >= end as u64 {
                break;
            }
        }
        let sum: u64 = batches.iter().map(|(_, rows)| u64::from(*rows)).sum();
        if sum != rows_in_block as u64 {
            return vec![(fallback, rows_in_block as u32)];
        }
        batches
    }

    /// Encodes one block's rows (already rendered as text) into the column
    /// files, in the C++ layout.
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn encode_block_columns(
        &self,
        declared: &[String],
        column_ids: &[u64],
        types: &[i32],
        layouts: &[ColumnLayout],
        chunk: &[Vec<String>],
        bitmap_bits: Option<usize>,
    ) -> Result<Vec<BlockColumn>> {
        let mut block_columns = Vec::with_capacity(types.len());
        for (col, column_type) in types.iter().enumerate() {
            let kind = outline_kind(&declared[col]);
            // Resolved once per column, not once per cell: `embedding_shape`
            // upper-cases the type string, so the per-cell call allocated
            // 16.7 M `String`s on an 8 M-row `COPY`.
            let shape = embedding_shape(&declared[col]);
            let mut slots: Vec<Option<Vec<u8>>> = Vec::with_capacity(chunk.len());
            // Self-outlining columns build their own outline; the generic
            // packer produces one for varchar/json.
            let mut self_outline = Vec::new();
            for row in chunk {
                let text = row.get(col).map(String::as_str).unwrap_or("");
                match kind {
                    OutlineKind::Tensor => {
                        slots.push(encode_tensor_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::TensorArray => slots.push(encode_tensorarray_slot(
                        &declared[col],
                        text,
                        &mut self_outline,
                    )?),
                    OutlineKind::Sparse => {
                        slots.push(encode_sparse_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::Array => {
                        slots.push(encode_array_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::None => slots.push(encode_value_with_shape(
                        &declared[col],
                        shape,
                        *column_type,
                        text,
                    )?),
                }
            }
            let bytes: Vec<Option<&[u8]>> = slots.iter().map(|s| s.as_deref()).collect();
            let (body, packed_outline) = pack_column_with_coverage(
                *column_type,
                layouts[col],
                &bytes,
                DEFAULT_BLOCK_CAPACITY,
                bitmap_bits,
            )
            .map_err(|e| Error::Storage(format!("pack column {col}: {e}")))?;
            let outline = if kind == OutlineKind::None {
                packed_outline
            } else {
                self_outline
            };
            block_columns.push(BlockColumn {
                column_id: column_ids.get(col).copied().unwrap_or(col as u64),
                body,
                outline,
            });
        }
        Ok(block_columns)
    }

    /// Refreshes the C++ block mirror of `table` if it already has one.
    ///
    /// The reference writes a block's files while the append transaction
    /// commits (`AppendInBlock`), not at some later "flush", so a directory it
    /// leaves behind is always readable. A mirror that exists must behave the
    /// same way: a crash after a commit that only updated the catalog would
    /// leave `catalog|blk|…` records for a block with no files, and the
    /// reference aborts on that ("Failed to find object for local path
    /// …/blk_0/version", `MetaTableObject::RestoreTableCache`).
    ///
    /// A table without a mirror stays without one: its columns may not be
    /// expressible in the format, and `publish_segments` (inside
    /// [`Self::write_blocks`]) is what would make it visible to the
    /// reference in the first place.
    pub fn refresh_block_mirror(&self, table: &str) -> Result<()> {
        self.refresh_block_mirror_extra(table, &[])
    }

    /// [`Self::refresh_block_mirror`] with the rows the commit publishes.
    ///
    /// The reference writes a batch into its blocks as part of the commit
    /// (`AppendInBlock`), and the port keeps no `row:` keys for it, so the new
    /// rows have to reach the block writer directly - including the very first
    /// commit of a table, which creates the `tbl_{id}/seg_{id}/blk_{id}/`
    /// directories (the reference has no other place a row can live).
    pub(crate) fn refresh_block_mirror_extra(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
    ) -> Result<()> {
        if extra.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        // No file I/O here: the rows go into the table's append buffer, which
        // is what the reference's `AppendInBlock` does to the block's
        // `BufferObject`. The block file is written by the checkpoint.
        let tkey = self.table_key(&table);
        // The reference casts every value to its column's type when it appends
        // it (DataBlock::AppendValue), so an out-of-range integer wraps to the
        // column's width and a float assigned to an integer column truncates.
        // Keeping the raw value would leave text the block writer cannot encode
        // and, later, a read that fails its own type check.
        let types = self.column_types(&table)?;
        let coercions: Vec<Coercion> = types.iter().map(|ty| coercion_of(ty)).collect();
        let rows: Vec<(usize, Vec<Value>)> = extra
            .iter()
            .map(|(seq, row)| (*seq, coerce_row(row, &coercions)))
            .collect();
        {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.buffer_rows(rows);
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
        }
        Ok(())
    }

    /// Mirrors every table into the C++ block layout.
    ///
    /// Called from [`Engine::flush`], so a normal clean shutdown leaves the data
    /// directory readable by the C++ engine. Tables whose columns are not yet
    /// packable (array/tensor/sparse/embedding) are skipped rather than
    /// mis-encoded.
    ///
    /// The unit of work is one **table** (plan §5.1): its blocks, its own
    /// indexes, its chunk layer, and the memory tails the fresh mirror makes
    /// redundant. A table's internal order is unchanged - blocks, then indexes,
    /// then the publish, then the tails, with `version` still the last file the
    /// block writer writes - and two tables share nothing, so they run side by
    /// side. The global steps (the checkpoint ts, the index-cache reload, the
    /// catalog layout, the final `store.flush`) stay in the caller.
    pub async fn flush_blocks(&self, checkpoint_ts: u64) -> Result<()> {
        let tables = self.storage.catalog.list_tables()?;
        self.flush_tables(&tables, checkpoint_ts, checkpoint_workers())
            .await
    }

    /// The same checkpoint with an explicit worker count.
    ///
    /// [`Self::flush_blocks`] takes it from `INFINITY_CHECKPOINT_CONCURRENCY`;
    /// the scaling probe passes it directly, because a test cannot change a
    /// process-wide variable without racing the rest of the suite.
    pub(crate) async fn flush_tables(
        &self,
        tables: &[catalog::TableMeta],
        checkpoint_ts: u64,
        workers: usize,
    ) -> Result<()> {
        let workers = workers.min(tables.len()).max(1);
        if workers <= 1 {
            for meta in tables {
                self.flush_table(meta, checkpoint_ts).await?;
            }
            return Ok(());
        }
        // One table's checkpoint is an **async** unit: it waits for the
        // table's bottom gate (`.await`), then writes that table's blocks and
        // index chunks - and only those writes, each one a file write, go to
        // the blocking pool inside the body.
        //
        // A task needs an owned engine handle, so the fan-out runs when the
        // engine lives in an `Arc` (every serving path does); a bare `Engine`
        // (a unit test that opened a directory directly) takes the serial pass
        // below, which is the same work.
        if let Some(engine) = self.engine_handle() {
            let metas: Vec<catalog::TableMeta> = tables.to_vec();
            return crate::fanout::for_each_async(metas, workers, move |meta| {
                let engine = engine.clone();
                async move { engine.flush_table(&meta, checkpoint_ts).await }
            })
            .await;
        }
        for meta in tables {
            self.flush_table(meta, checkpoint_ts).await?;
        }
        Ok(())
    }

    /// One table's share of a checkpoint.
    ///
    /// The table's bottom gate is held for the whole step: the block files it
    /// writes cover every row the table has at that moment, so the tails it
    /// clears afterwards are exactly the rows those files now hold (plan §5.2).
    /// An append queued behind the gate cannot slip a row in between.
    ///
    /// The shape is: **await the gate**, then hand each write to the blocking
    /// pool **one at a time** - the block mirror, then one call per index - and
    /// touch the in-memory registries only after each write came back. No
    /// `std::sync::Mutex` is held across an await, and no blocking pool thread
    /// ever runs more than one file write.
    async fn flush_table(&self, meta: &catalog::TableMeta, checkpoint_ts: u64) -> Result<()> {
        let key = self.table_key(&meta.name);
        let gate = self.bottom_gate(&key);
        let _turn = gate.lock().await;
        let _held = crate::engine::statements::HeldBottom::enter(&key);
        if meta.types.iter().all(|t| column_type_of(t).is_some()) {
            let table = meta.name.clone();
            self.flush_leaf(move |engine| {
                engine.write_blocks_bounded_locked(&table, checkpoint_ts)
            })
            .await?;
        }
        // **No index work here.** `NewTxn::CheckpointTable`
        // (`new_txn_data_impl.cpp:1438`) walks the table's blocks and flushes
        // each block's `version` and column files - it never touches an index.
        // An index reaches disk in its own `DumpIndex` transaction
        // (`NewTxn::DumpMemIndex` / `PopulateIndex`), which is the only place a
        // chunk is written and where its `GetAndSetNextChunkID` id and commit ts
        // come from. Rebuilding an index from the rows on every checkpoint
        // rewrote the whole table per tick (a measured `write_fulltext_index`
        // held a CPU for minutes while rows kept arriving) and invented chunk
        // records no dump transaction names.
        self.refresh_memindex_charge(&meta.name);
        Ok(())
    }

    /// Runs one file write of a checkpoint on the blocking pool.
    ///
    /// `write` is exactly one table's block mirror or exactly one index's
    /// chunk write - the granularity a blocking thread should hold. When the
    /// engine has no `Arc` handle (a bare `Engine` in a unit test) the write
    /// runs in place; it is the same call either way.
    async fn flush_leaf<F>(&self, write: F) -> Result<usize>
    where
        F: FnOnce(&Engine) -> Result<usize> + Send + 'static,
    {
        match self.engine_handle() {
            Some(handle) => {
                crate::scheduler::scheduler()
                    .spawn_blocking(move || write(&handle))
                    .await
            }
            None => write(self),
        }
    }

    /// Mirrors each index's sealed chunk into `idx_{id}/seg_0/chunk_0.idx` and
    /// records it in the catalog. Best effort per index: a mirror this engine
    /// does not have simply publishes nothing.
    /// Re-stamps the index's published `idx_chunk|…|{chunk_id}` records with
    /// `ts`, the commit timestamp of the dump transaction that just sealed it.
    ///
    /// The reference writes that record inside the dump transaction itself
    /// (`SegmentIndexMeta::AddChunkIndexID1`). The port's mirror re-publishes the
    /// whole chunk layer at checkpoint time, so a dump that happens *after* the
    /// last checkpoint must touch the records it already published; the next
    /// mirror preserves the value from [`dump_ts_key`].
    pub(crate) fn stamp_chunk_ts(&self, table: &str, index: &str, ts: u64) -> Result<()> {
        let table = self.qualified_table(table);
        let db_id = self.database_id_of(&table);
        let table_id = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .map(|m| m.id)
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.index_position(&table, index)?;
        let store = self.storage.catalog.store();
        let prefix = format!("idx_chunk|{db_id}|{table_id}|{index_id}|");
        let value = ts.to_string().into_bytes();
        for (key, _) in store.scan_prefix(prefix.as_bytes())? {
            let text = String::from_utf8_lossy(&key).to_string();
            // Only the id records (numeric suffix); `…|chunk_info` and
            // `…|next_chunk_id` are owned by the mirror.
            let Some(suffix) = text.rsplit('|').next() else {
                continue;
            };
            if suffix.parse::<u64>().is_ok() {
                store.put(&key, &value)?;
            }
        }
        Ok(())
    }

    /// The C++ chunk of a SMVE or EMVB index, built from the table's rows the
    /// way the reference's memory dump builds it.
    ///
    /// SMVE: `u32 width, topk, embedding_dim, n_docs`, the `dim × width`
    /// projection matrix (the reference's `mt19937(42)` sequence, so the bytes
    /// match) and the BMP index (`BMPHandler::SaveToPtr`). EMVB:
    /// `EMVBIndex::SaveIndexInner`.
    /// The C++ chunk of the table's SMVE or EMVB index, **as it was dumped**:
    /// the memory index is built when the index is created and fed by every
    /// insert, and `flush()` dumps it into the segment files - the chunk is
    /// that dump, not a rebuild from the rows.
    pub(crate) fn multivector_chunk(
        &self,
        table: &str,
        index_id: u64,
        kind: &str,
        segment: u32,
    ) -> Option<Vec<u8>> {
        let key = self.table_key(table);
        let index = {
            let tables = self.tables.lock().unwrap();
            tables.get(&key)?.mv.get(&index_id).cloned()?
        };
        let guard = index.read().ok()?;
        if kind.eq_ignore_ascii_case("SMVE") {
            let smve = guard.smve.as_ref()?;
            let mut out = Vec::new();
            for v in [
                smve.width(),
                smve.topk(),
                smve.embedding_dim(),
                smve.chunk_row_count(),
            ] {
                out.extend_from_slice(&v.to_le_bytes());
            }
            for v in smve.projection() {
                out.extend_from_slice(&v.to_le_bytes());
            }
            let docs: Vec<storage::bmp::SparseDoc> = smve
                .docs()
                .iter()
                .map(|(row, entries)| {
                    (
                        *row as u64,
                        entries
                            .iter()
                            .map(|(term, value)| (*term as i32, *value))
                            .collect(),
                    )
                })
                .collect();
            out.extend_from_slice(&storage::bmp::encode_bmp(
                index::sparse::BMP_BLOCK_SIZE,
                storage::bmp::IdxWidth::I32,
                &docs,
                smve.width() as usize,
            ));
            Some(out)
        } else {
            let emvb = guard.tail_emvb.lock().ok()?;
            if let Some(tail) = emvb.as_ref() {
                return Some(tail.data.encode());
            }
            // The tail was sealed before this publish: `MultiVectorIndex::seal`
            // wrote the memory index's EMVB next to the raw-token segment
            // (`seg_{segment}_{n}_{docs}.emvb` in the index's own directory),
            // and that file is the same layout as the C++ chunk
            // (`EmvbData::encode`). Without it the mirror published no chunk at
            // all for an index nothing had searched yet.
            let prefix = format!("seg_{segment}_");
            let mut newest: Option<(u64, std::path::PathBuf)> = None;
            for entry in std::fs::read_dir(&guard.dir).ok()?.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                if !name.starts_with(&prefix) || !name.ends_with(".emvb") {
                    continue;
                }
                let ordinal = name[prefix.len()..]
                    .split('_')
                    .next()
                    .and_then(|n| n.parse::<u64>().ok())
                    .unwrap_or(0);
                if newest.as_ref().map(|(o, _)| ordinal > *o).unwrap_or(true) {
                    newest = Some((ordinal, entry.path()));
                }
            }
            newest.and_then(|(_, path)| storage::chunk_store::read_file(path).ok())
        }
    }

    /// `only` publishes a single index's chunks for the dump transaction that
    /// seals it: every record it writes carries that transaction's commit ts.
    /// The checkpoint passes `None` and mirrors the whole table with the ts the
    /// last dump recorded.
    pub(crate) fn publish_index_chunks(
        &self,
        table: &str,
        table_id: u64,
        only: Option<(&str, u64)>,
    ) -> Result<()> {
        let mut indexes = self.storage.catalog.indexes_of_table(table)?;
        if let Some((name, _)) = only {
            indexes.retain(|im| im.name == name);
        }
        self.publish_index_chunks_of(table, table_id, &indexes, only)
    }

    fn publish_index_chunks_of(
        &self,
        table: &str,
        table_id: u64,
        indexes: &[catalog::IndexMeta],
        only: Option<(&str, u64)>,
    ) -> Result<()> {
        let db_id = self.database_id_of(table);
        let root = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{table_id}"));
        // Chunks are keyed by `(index, segment)`: the reference writes each
        // segment's index under `idx_{id}/seg_{s}/` with its own chunk ids
        // (`SegmentIndexMeta::next_chunk_id` is per segment), so a table with
        // two segments has two independent chunk families per index.
        let mut segments = self.segment_row_counts(table);
        if segments.is_empty() {
            segments.push((0, 0));
        }
        let mut published = Vec::new();
        for im in indexes {
            if im.table != table && im.table != self.qualified_table(table) {
                continue;
            }
            if only.is_some_and(|(name, _)| name != im.name) {
                continue;
            }
            // The list already carries the C++ id; only a hand-written
            // record without one needs the lookup.
            let index_id = if im.id > 0 {
                im.id as usize
            } else {
                let Ok(id) = self.index_position(table, &im.name) else {
                    continue;
                };
                id
            };
            // Every chunk the index holds, not just the first: a chunk the
            // catalog does not list is invisible to the reference's readers.
            //
            // The chunk's granularity depends on the index kind. Full-text,
            // secondary and BMP chunks are **blocks** (100 + 100 rows inserted
            // into one block leave one chunk of 200 rows, because every dump
            // re-dumps the block it touches); HNSW/IVF/PLAID chunks are
            // **memory-index dumps** (the same insert leaves two chunks of 100
            // rows each). Measured, not assumed: a probe with two flushes over
            // one block gives `row_count` 200/200/200 for full-text, secondary
            // and BMP and 100+100 for HNSW.
            let declared = self
                .table_schema(table)
                .ok()
                .and_then(|schema| {
                    let ci = schema.iter().position(|c| c == &im.column)?;
                    self.column_types(table).ok()?.get(ci).cloned()
                })
                .unwrap_or_default();
            let sparse = if im.index_type.eq_ignore_ascii_case("BMP") {
                Some(self.sparse_rows_by_block(table, &im.column))
            } else {
                None
            };
            // The chunk record names the transaction whose dump sealed it
            // (`SegmentIndexMeta::AddChunkIndexID1`); the port's last `DumpIndex`
            // for this index is that transaction.
            let dump_ts = self
                .storage
                .catalog
                .store()
                .get(&crate::dump_ts_key(&self.qualified_table(table), &im.name))
                .ok()
                .flatten()
                .filter(|v| v.len() >= 8)
                .map(|v| u64::from_le_bytes(v[..8].try_into().unwrap()))
                .unwrap_or(0);
            // The chunk records this index already has, by first row id: a
            // re-dump of a block rewrites its own chunk under its own id
            // instead of appending a second one.
            let existing_chunks: std::collections::HashMap<u64, u32> =
                catalog::layout::read_index_chunks(
                    self.storage.catalog.store().as_ref(),
                    &db_id.to_string(),
                    &table_id.to_string(),
                    &index_id.to_string(),
                )
                .unwrap_or_default()
                .into_iter()
                .flat_map(|(_, chunks)| chunks)
                .map(|(id, info)| (info.base_row_id, id as u32))
                .collect();
            for (segment, seg_rows) in &segments {
                let seg_dir = root.join(format!("idx_{index_id}/seg_{segment}"));
                // A chunk's ts is the commit ts of the transaction whose dump
                // produced it. A segment filled by `COPY` was populated inside
                // the import transaction (`PopulateIndex`), so its chunks
                // carry the import's ts; a later explicit dump stamps its own.
                let chunk_ts = match only {
                    Some((_, ts)) => ts,
                    None => dump_ts.max(self.import_ts(table, *segment).unwrap_or(0)),
                };
                // A segment that still holds exactly what one `COPY` wrote gets
                // the import's chunk layout (one per capacity for full-text, one
                // for the whole segment for secondary and sparse).
                let imported = self.imported_extent(table, *segment, *seg_rows).is_some();
                // A plain `CREATE INDEX` leaves the type empty, which is the
                // secondary default everywhere else (`is_secondary_kind`).
                let chunks = if crate::is_secondary_kind(&im.index_type) {
                    // The dump already wrote each chunk into the segment's own
                    // directory under the id the catalog handed out, so the
                    // records are read back from there. Deriving them from the
                    // `secondary/` spill set instead named chunk 0 for every
                    // dump and made the last one overwrite the others.
                    let low = declared.trim().to_uppercase().starts_with("BOOL")
                        || declared.trim().to_uppercase().starts_with("JSON")
                        || im.options.iter().any(|(k, v)| {
                            k.eq_ignore_ascii_case("CARDINALITY") && v.eq_ignore_ascii_case("low")
                        });
                    if low {
                        publish_lowcard_chunks_from_dir(&seg_dir, *segment)
                    } else {
                        publish_secondary_chunks_from_dir(
                            &seg_dir,
                            crate::secondary_key_kind(&declared),
                            *segment,
                        )
                    }
                } else if im.index_type.eq_ignore_ascii_case("BMP") {
                    // Rebuilt from the rows: the reference's chunk is the block and
                    // every dump extends it, while the port's spill files are per
                    // dump, so merging has to happen here. The id a record already
                    // named for a block is kept (`PopulateBMPIndexInner` and
                    // `DumpMemIndex` only *take* ids from `GetAndSetNextChunkID`).
                    let empty = Default::default();
                    let set = SegmentSet {
                        db_id: db_id.to_string(),
                        table_id: table_id.to_string(),
                        segments: Vec::new(),
                    };
                    let mut next_id =
                        self.next_index_chunk_id(&set, &index_id.to_string(), *segment);
                    publish_bmp_chunks(
                        &seg_dir,
                        sparse
                            .as_ref()
                            .and_then(|m| m.get(segment))
                            .cloned()
                            .unwrap_or(empty),
                        &crate::blocks::chunk::BmpChunkShape {
                            width: crate::sparse_idx_width(&declared),
                            term_bound: crate::sparse_declared_bound(&declared).unwrap_or(0),
                            block_size: crate::index_block_size(im),
                            segment: *segment,
                            single_chunk: imported,
                        },
                        &existing_chunks,
                        &mut next_id,
                    )
                } else if im.index_type.eq_ignore_ascii_case("FULLTEXT") {
                    // Written in place by the seal; its files are named after
                    // the chunk's base name, not `chunk_N.idx`. The ids come
                    // from the catalog counter like every other kind
                    // (`AddNewChunkIndex1` in `DumpMemIndex`), so a chunk a
                    // record already names keeps its id and a re-dump of the
                    // same base name never takes a second one.
                    let set = SegmentSet {
                        db_id: db_id.to_string(),
                        table_id: table_id.to_string(),
                        segments: Vec::new(),
                    };
                    let mut next_id =
                        self.next_index_chunk_id(&set, &index_id.to_string(), *segment);
                    publish_fulltext_chunks(&seg_dir, &existing_chunks, &mut next_id)
                } else if im.index_type.eq_ignore_ascii_case("PLAID") {
                    // `PopulatePlaidIndexInner` / `DumpSegmentMemIndex`'s
                    // `kPLAID`: the built memory index is dumped into the chunk
                    // the record names. `write_plaid_index` appends only the
                    // rows past the chunks already on disk, so the create-index
                    // publish writes the first chunk and a later dump the rest.
                    std::fs::create_dir_all(&seg_dir).ok();
                    let before = catalog::layout::read_index_chunks(
                        self.storage.catalog.store().as_ref(),
                        &db_id.to_string(),
                        &table_id.to_string(),
                        &index_id.to_string(),
                    )
                    .unwrap_or_default()
                    .into_iter()
                    .find(|(s, _)| *s == *segment)
                    .map(|(_, chunks)| chunks.len())
                    .unwrap_or(0);
                    let _ = self.write_plaid_index(table, &im.name);
                    catalog::layout::read_index_chunks(
                        self.storage.catalog.store().as_ref(),
                        &db_id.to_string(),
                        &table_id.to_string(),
                        &index_id.to_string(),
                    )
                    .unwrap_or_default()
                    .into_iter()
                    .find(|(s, _)| *s == *segment)
                    .map(|(_, chunks)| chunks)
                    .map(|chunks| {
                        // Only the chunks this publish could have written are
                        // stamped; the earlier ones keep their own ts.
                        chunks
                            .into_iter()
                            .skip(before)
                            .map(|(id, info)| IndexChunk {
                                chunk_id: id as u32,
                                base_name: info.base_name.clone(),
                                base_row_id: info.base_row_id,
                                row_count: info.row_count,
                                term_count: info.term_count,
                                index_size: info.index_size,
                                commit_ts: chunk_ts,
                            })
                            .collect()
                    })
                    .unwrap_or_default()
                } else if im.index_type.eq_ignore_ascii_case("SMVE")
                    || im.index_type.eq_ignore_ascii_case("EMVB")
                {
                    // These two write their chunk through their own file worker
                    // in the reference (`smve_index_file_worker`,
                    // `emvb_index_file_worker`), but the file name is still
                    // `chunk_{id}.idx` with the id `GetAndSetNextChunkID` hands
                    // out (`ChunkIndexMeta::IndexFileName`) - never a literal
                    // `chunk_0`, which two dumps would share.
                    let Some(bytes) =
                        self.multivector_chunk(table, index_id as u64, &im.index_type, *segment)
                    else {
                        continue;
                    };
                    let set = SegmentSet {
                        db_id: db_id.to_string(),
                        table_id: table_id.to_string(),
                        segments: Vec::new(),
                    };
                    let chunk_id = self.next_index_chunk_id(&set, &index_id.to_string(), *segment);
                    if std::fs::create_dir_all(&seg_dir).is_err()
                        || storage::definition::format::write_synced(
                            &seg_dir.join(format!("chunk_{chunk_id}.idx")),
                            &bytes,
                        )
                        .is_err()
                    {
                        continue;
                    }
                    vec![IndexChunk {
                        chunk_id,
                        base_name: String::new(),
                        base_row_id: u64::from(*segment) << 32,
                        row_count: *seg_rows,
                        term_count: 0,
                        index_size: bytes.len() as u64,
                        commit_ts: chunk_ts,
                    }]
                } else {
                    let deprecated = self.deprecated_chunk_ids(table, &im.name, *segment);
                    publish_chunk_files(
                        &seg_dir,
                        *seg_rows,
                        im.index_type.eq_ignore_ascii_case("HNSW"),
                        *segment,
                        &deprecated,
                    )
                };
                if chunks.is_empty() {
                    continue;
                }
                let chunks: Vec<_> = chunks
                    .into_iter()
                    .map(|c| IndexChunk {
                        commit_ts: chunk_ts,
                        ..c
                    })
                    .collect();
                published.push(IndexSegment {
                    index_id: index_id as u64,
                    segment_id: *segment,
                    chunks,
                });
            }
        }
        if published.is_empty() {
            return Ok(());
        }
        // **Append**, never replace: the records a writer already published
        // under an id `GetAndSetNextChunkID` handed out are the chunk list, and
        // rewriting the whole list here would drop the ones whose ids differ
        // from the directory names this mirror happened to produce. Only a
        // chunk without a record is added (`AddNewChunkIndex1`).
        let set = SegmentSet {
            db_id: db_id.to_string(),
            table_id: table_id.to_string(),
            segments: Vec::new(),
        };
        for index in &published {
            let existing: std::collections::HashMap<u64, u64> = catalog::layout::read_index_chunks(
                self.storage.catalog.store().as_ref(),
                &set.db_id,
                &set.table_id,
                &index.index_id.to_string(),
            )
            .unwrap_or_default()
            .into_iter()
            .find(|(segment, _)| *segment == index.segment_id)
            .map(|(_, chunks)| {
                chunks
                    .into_iter()
                    .map(|(id, info)| (id, info.row_count))
                    .collect()
            })
            .unwrap_or_default();
            for chunk in &index.chunks {
                // A record that already exists is the chunk list; but the file
                // it names can have been rewritten in place, and then the
                // record's `row_count` is stale. A sparse dump reuses a block's
                // chunk id and grows the file from 100 to 200 rows, so treating
                // "the id is listed" as "the record is current" left the catalog
                // describing half the rows. Write when anything differs.
                if existing.get(&(chunk.chunk_id as u64)) == Some(&chunk.row_count) {
                    continue;
                }
                let _ = self.storage.catalog.append_index_chunk(&set, index, chunk);
            }
        }
        Ok(())
    }

    /// A sparse column's documents, grouped by segment and then by physical
    /// block, in the BMP encoder's form: `(row id within the segment,
    /// [(term, value)])`.
    ///
    /// The segment is part of the key: every segment restarts its block ids at
    /// zero, and the reference keeps one BMP index per segment
    /// (`idx_{id}/seg_{s}/chunk_{block}.idx`).
    pub(crate) fn sparse_rows_by_block(
        &self,
        table: &str,
        column: &str,
    ) -> std::collections::BTreeMap<
        u32,
        std::collections::BTreeMap<u64, Vec<storage::bmp::SparseDoc>>,
    > {
        let mut by_block = std::collections::BTreeMap::new();
        let Ok(schema) = self.table_schema(table) else {
            return by_block;
        };
        let Some(ci) = schema.iter().position(|c| c == column) else {
            return by_block;
        };
        let Ok(rows) = self.fetch_rows_with_ids_projected_all(table, ci) else {
            return by_block;
        };
        for (seq, cells) in rows {
            let Some(Value::Str(text)) = cells.get(ci) else {
                continue;
            };
            let entries: Vec<(i32, f32)> = crate::parse_sparse_str(text)
                .into_iter()
                .map(|(term, value)| (term as i32, value as f32))
                .collect();
            let row = seq as u64 & 0xffff_ffff;
            by_block
                .entry((seq >> 32) as u32)
                .or_default()
                .entry(row / DEFAULT_BLOCK_CAPACITY as u64)
                .or_default()
                .push((row, entries));
        }
        by_block
    }

    /// Reads a table back from the C++ block layout.
    ///
    /// This is the interop direction §2.6.1 cares about: a `db_*/tbl_*/seg_*/`
    /// blk_*` directory written by the C++ engine (or by [`Self::write_blocks`])
    /// reconstructs the table. Values are inserted through the normal `INSERT`
    /// path, so declared types, defaults and indexes all behave as usual.
    pub fn load_blocks(&self, table: &str) -> Result<usize> {
        let qualified = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&qualified)?
            .ok_or_else(|| Error::NotFound(format!("table '{qualified}'")))?;
        let (columns, rows) = self.read_blocks(table)?;
        if rows.is_empty() {
            return Ok(0);
        }
        let mut values = Vec::with_capacity(rows.len());
        for row in &rows {
            let cells: Vec<String> = row
                .iter()
                .enumerate()
                .map(|(i, v)| sql_literal(meta.types.get(i).map(String::as_str).unwrap_or(""), v))
                .collect();
            values.push(format!("({})", cells.join(",")));
        }
        let sql = format!(
            "INSERT INTO {qualified} ({}) VALUES {};",
            columns.join(", "),
            values.join(",")
        );
        self.execute_blocking(&sql)?;
        Ok(rows.len())
    }

    /// Recovery on startup: restore every table's block mirror at the ids the
    /// mirror itself holds.
    ///
    /// Opening a directory is two sources of truth, exactly as in the
    /// reference: `RestoreTableCache` rebuilds the segments from the block
    /// metadata, then the WAL is replayed on top. The block metadata only
    /// records the rows a flush made durable — a crash right after an append
    /// leaves the block's `version` at the flushed count and the newer rows in
    /// the log — so the two have to be merged, not chosen between:
    ///
    /// * the replay already wrote the post-flush rows at their own ids, and
    /// * the mirror supplies the flushed ones, which are missing from the KV
    ///   store because their WAL records were truncated by the checkpoint.
    ///
    /// The restore therefore writes only the ids the store does not have, which
    /// also makes it idempotent: a cleanly closed directory restores nothing.
    /// A tombstoned slot is **not** written back - the block's `version` is what
    /// hides it, and a journal copy would make the two disagree on the next
    /// open (the ids still join the delete log, because a later rewrite re-marks
    /// the slot from there).
    ///
    /// A table whose columns the block format cannot express simply has no
    /// mirror; that (and only that) is skipped silently.
    /// Returns the newest timestamp the files carry: everything up to it is
    /// already in the blocks, so the WAL replay starts above it. The reference
    /// gets the same floor from its checkpoint record; with the row KV gone
    /// (plan C) the files are the only place it can come from.
    pub fn recover_blocks(&self) -> Result<u64> {
        let mut newest = 0u64;
        for meta in self.storage.catalog.list_tables()? {
            newest = newest.max(self.restore_blocks(&meta.name)?);
        }
        Ok(newest)
    }

    /// Rebuilds a table's rows **and the append/delete journals** from its block
    /// files - the read side of the mirror, and what a restart has instead of a
    /// second KV.
    ///
    /// The reference keeps that history in the files themselves: a block's
    /// `version` holds one `create` per append (the transaction's ts and the
    /// cumulative row count) and one ts per deleted slot, and a sealed segment
    /// is the one it wrote a `fast_rough_filter` for. Rebuilding from those is
    /// what makes the port's checkpoint rewrite reproduce the same `version`
    /// bytes, and it is why `appends:`/`append_ts:`/`deletes:` do not have to be
    /// persisted anywhere the reference would not have them.
    pub(crate) fn restore_blocks(&self, table: &str) -> Result<u64> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Ok(0);
        };
        let root = self.blocks_root();
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Ok(0); // a column the block format cannot express has no file
        };
        let blocks: Vec<BlockId> = self.visible_block_ids(&table);
        let _writes: Vec<(Vec<u8>, Vec<u8>)> = Vec::new();
        let mut appends: Vec<u64> = Vec::new();
        let mut append_ts: Vec<u64> = Vec::new();
        let mut appends_copy: Vec<u64> = Vec::new();
        let mut deletes: Vec<u64> = Vec::new();
        let mut delete_ts: Vec<u64> = Vec::new();
        let mut segment_rows: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut segment_first_ts: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut total = 0u64;
        let mut newest = 0u64;
        for id in &blocks {
            let Ok(block) = self.block_map(&table, &root, *id) else {
                continue;
            };
            let version = block.version();
            let Some(rows) = self.decode_block_cells(&table, &root, &meta, &types, *id, None)
            else {
                continue;
            };
            let mut seen = 0u64;
            for create in &version.created {
                let cumulative = create.cumulative_row_count.max(0) as u64;
                let added = cumulative.saturating_sub(seen);
                seen = cumulative;
                total += added;
                appends.push(total);
                append_ts.push(create.create_ts);
                appends_copy.push(0);
                newest = newest.max(create.create_ts);
                segment_first_ts
                    .entry(id.segment_id)
                    .or_insert(create.create_ts);
            }
            *segment_rows.entry(id.segment_id).or_default() += rows.len() as u64;
            for (slot, (seq, _cells)) in rows.iter().enumerate() {
                // `BlockVersion::Delete` keeps the cells and marks the slot, and
                // that mark is the whole tombstone - the block is the row's only
                // home, so nothing is written back to a row store.
                let tombstone = version.deleted.get(slot).copied().filter(|t| *t != 0);
                if let Some(ts) = tombstone {
                    deletes.push(*seq as u64);
                    delete_ts.push(ts);
                    newest = newest.max(ts);
                }
            }
        }
        // The journals are not written anywhere: they are derived from the same
        // files on demand (`Engine::ensure_journals`), which is why the port
        // needs no second KV. `segment_rows`/`segment_first_ts` only feed that
        // derivation now.
        Ok(newest)
    }

    /// Applies the row commands a WAL replay found above the blocks' own floor:
    /// an append becomes rows in its block (`AppendInBlock`, with the frame's
    /// commit ts as the block's `create`), a delete becomes a mark on the slot
    /// the row occupies (`DeleteInBlock`).
    /// The reference's replay ends in exactly this shape - its `WalCmdAppendV2`
    /// carries the `DataBlock` that `AppendInBlock` writes - and it is why a
    /// recovered directory has no second copy of the rows anywhere.
    pub(crate) fn apply_replayed_rows(
        &self,
        appends: Vec<txn::ReplayedAppend>,
        deletes: Vec<txn::ReplayedDelete>,
    ) -> Result<()> {
        if appends.is_empty() && deletes.is_empty() {
            return Ok(());
        }
        let mut order: Vec<String> = Vec::new();
        let mut rows: std::collections::HashMap<String, Vec<(usize, Vec<Value>)>> =
            Default::default();
        // The block files do not change while this replay runs (it only fills
        // the append buffer), so the ids they already hold are computed once
        // per table. Re-reading them per frame made a directory with a few
        // hundred import frames quadratic: the 100k-row insert suite never got
        // past its restart.
        let mut on_disk_by_table: std::collections::HashMap<
            String,
            std::collections::HashSet<usize>,
        > = Default::default();
        // The schema each table went through, read once per table from the
        // catalog (see `Catalog::column_timeline`).
        let mut timelines: std::collections::HashMap<String, Vec<(u64, Vec<u64>)>> =
            Default::default();
        for append in &appends {
            let table = self.qualified_table(&append.table);
            // The frame names the table **id** it was written for. A snapshot
            // restore re-creates the table under a new id (and a dropped name
            // can come back the same way), so rows of a frame that names an
            // older id belong to a table that no longer exists - replaying them
            // appended the pre-restore rows on top of the restored ones.
            let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
                continue;
            };
            if append.table_id != u64::MAX && append.table_id != meta.id {
                continue;
            }
            // The ids the frame's cells belong to, and the ones the buffer
            // is positional against.
            let mut current_ids = meta.ids.clone();
            current_ids.sort_unstable();
            let timeline = timelines.entry(table.clone()).or_insert_with(|| {
                self.storage
                    .catalog
                    .column_timeline(&table)
                    .unwrap_or_default()
            });
            let frame_ids = ids_at(timeline, append.commit_ts, &current_ids);
            let defaults = self.table_defaults(&table).unwrap_or_default();
            let types = meta.types.clone();
            // A row the block files already hold is not re-appended: the frame
            // may describe a commit that reached the blocks before the crash
            // (a DDL rewrite logs its rows and then writes them), and buffering
            // it again would show the row twice.
            let on_disk = on_disk_by_table.entry(table.clone()).or_insert_with(|| {
                self.read_blocks_including_deleted(&table)
                    .into_iter()
                    .map(|(seq, _)| seq)
                    .collect()
            });
            let mut batch = Vec::with_capacity(append.rows.len());
            for (seq, text) in &append.rows {
                if on_disk.contains(&(*seq as usize)) {
                    continue;
                }
                // The row is placed on the ids its frame was written with;
                // `None` means a snapshot restore hides them, and the text is
                // already in the schema the restore left behind.
                let text = reorder_row_text(text, &frame_ids, &current_ids, &defaults);
                // A row whose text form the row parser cannot rebuild is not
                // buffered: the append buffer is the only place a row the block
                // files do not hold can live, and the port keeps no row journal
                // to hold its text instead.
                if let Ok(row) = self.parse_stored_row(&table, &defaults, &types, text.as_bytes()) {
                    batch.push((*seq as usize, row));
                }
            }
            // One `create` per append transaction, stamped with its commit ts.
            self.note_append(&table, batch.len(), false, None);
            self.settle_append_ts(&table, 1, append.commit_ts);
            if !rows.contains_key(&table) {
                order.push(table.clone());
            }
            rows.entry(table).or_default().extend(batch);
        }
        for delete in &deletes {
            let table = self.qualified_table(&delete.table);
            // A tombstone for an id the table no longer has (see the append
            // loop) cannot mark a row of the table that exists now.
            match self.storage.catalog.get_table_by_name(&table) {
                Ok(Some(meta)) if delete.table_id == u64::MAX || delete.table_id == meta.id => {}
                _ => continue,
            }
            let ids: Vec<usize> = delete.row_ids.iter().map(|id| *id as usize).collect();
            self.note_deleted_ids(&table, &ids);
            self.settle_delete_ts(&table, ids.len(), delete.commit_ts);
            if !rows.contains_key(&table) {
                order.push(table.clone());
            }
            rows.entry(table).or_default();
        }
        for table in order {
            let extra = rows.remove(&table).unwrap_or_default();
            self.refresh_block_mirror_extra(&table, &extra)?;
        }
        Ok(())
    }

    /// `NewTxn::AddColumnsData` (`new_txn_data_impl.cpp:1195`): every block the
    /// table already has gains one file per added column, filled with that
    /// column's default for the rows the block holds.
    ///
    /// The reference writes **only** the added columns' files
    /// (`AddColumnsDataInBlock` -> `NewCatalog::AddNewBlockColumn` plus the
    /// default fill) and leaves every other column file and the block's
    /// `version` untouched: `{column_id}.col` names the column **id**, ids are
    /// never recycled, so adding a column needs no rewrite and no renumbering.
    /// Re-encoding the whole block instead (what the port did) rewrote every
    /// existing column of every block, which is O(rows) work for an O(blocks)
    /// change and left the block's `version` different from the reference's.
    pub(crate) fn add_column_blocks(&self, table: &str) -> Vec<(u32, u16, usize, Option<usize>)> {
        let table = self.qualified_table(table);
        let mut out = Vec::new();
        let mut global_pos = 0usize;
        for (segment_id, seg_rows_total) in self.block_segment_row_counts(&self.table_key(&table)) {
            let block_count = (seg_rows_total as usize).div_ceil(DEFAULT_BLOCK_CAPACITY);
            for block_index in 0..block_count {
                let start = block_index * DEFAULT_BLOCK_CAPACITY;
                let end = (start + DEFAULT_BLOCK_CAPACITY).min(seg_rows_total as usize);
                // The bitmap coverage a rewrite of this block would use, so an
                // imported block keeps its import bitmap. It comes from the
                // append journal, which is derived from the block files under
                // the **current** schema: this read has to happen before the
                // DDL stages the new column.
                let bitmap_bits = self.block_bitmap_bits(&table, global_pos + start, end - start);
                out.push((segment_id, block_index as u16, end - start, bitmap_bits));
            }
            global_pos += seg_rows_total as usize;
        }
        out
    }

    /// Materialises a column the block does not have yet, with its default.
    ///
    /// `AddColumnsData` does this for every block that exists when the `ALTER`
    /// runs. A block the checkpoint writes afterwards, or one the block index
    /// did not know at the time, can still miss a column - and then every decode
    /// of it fails, because a current column without a file is not "empty", it
    /// is unreadable. Repairing it here (only files that are *absent*, never an
    /// existing value) is the same statement applied late.
    fn ensure_block_columns(
        &self,
        table: &str,
        meta: &catalog::TableMeta,
        id: BlockId,
    ) -> Result<()> {
        let root = self.blocks_root();
        let Some(version) = self.read_block_version(id) else {
            return Ok(());
        };
        let rows = version
            .created
            .last()
            .map(|c| c.cumulative_row_count as usize)
            .unwrap_or(0);
        if rows == 0 {
            return Ok(());
        }
        let dir = root.join(storage::definition::format::block_dir(
            id.db_id,
            id.table_id,
            id.segment_id,
            id.block_id,
        ));
        let missing: Vec<usize> = (0..meta.ids.len())
            .filter(|i| {
                !dir.join(storage::definition::format::column_file(meta.ids[*i]))
                    .exists()
            })
            .collect();
        if missing.is_empty() {
            return Ok(());
        }
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
            .ok_or_else(|| {
                Error::Invalid(format!(
                    "table '{table}': a column the block format cannot express"
                ))
            })?;
        let layouts: Vec<ColumnLayout> = meta
            .types
            .iter()
            .zip(&types)
            .map(|(t, ct)| {
                layout_of(t, *ct)
                    .ok_or_else(|| Error::Invalid(format!("no .col layout for column type '{t}'")))
            })
            .collect::<Result<_>>()?;
        let defaults = self.table_defaults(table)?;
        let columns = self.encode_block_columns(
            &missing
                .iter()
                .map(|i| meta.types[*i].clone())
                .collect::<Vec<_>>(),
            &missing.iter().map(|i| meta.ids[*i]).collect::<Vec<_>>(),
            &missing.iter().map(|i| types[*i]).collect::<Vec<_>>(),
            &missing.iter().map(|i| layouts[*i]).collect::<Vec<_>>(),
            &vec![
                missing
                    .iter()
                    .map(|i| match defaults.get(*i).and_then(|d| d.as_deref()) {
                        Some(d) => mirror_cell_text(&Value::from_literal(&default_literal(d))),
                        None => mirror_cell_text(&Value::Null),
                    })
                    .collect::<Vec<String>>();
                rows
            ],
            None,
        )?;
        self.drop_block_maps(meta.id);
        for column in &columns {
            storage::definition::format::write_block_column(&root, id, column).map_err(|e| {
                Error::Io {
                    path: root.clone(),
                    source: e,
                }
            })?;
        }
        Ok(())
    }

    /// [`Self::add_column_files`] for an **async** caller (`ALTER TABLE ADD
    /// COLUMN`, whose commit effects run on a runtime worker).
    ///
    /// The gate is awaited rather than bridged with `block_on`: a `block_on` on
    /// a worker would park the very thread that has to make progress elsewhere
    /// in the same statement.
    pub(crate) async fn add_column_files_async(
        &self,
        table: &str,
        first: usize,
        blocks: &[(u32, u16, usize, Option<usize>)],
    ) -> Result<()> {
        let bottom_gate = self.bottom_gate(table);
        let _bottom = (!crate::engine::statements::HeldBottom::holds(&self.table_key(table)))
            .then(|| bottom_gate.lock());
        let _bottom = match _bottom {
            Some(turn) => Some(turn.await),
            None => None,
        };
        self.add_column_files_inner(table, first, blocks)
    }

    pub(crate) fn add_column_files(
        &self,
        table: &str,
        first: usize,
        blocks: &[(u32, u16, usize, Option<usize>)],
    ) -> Result<()> {
        // This writes a new column file into block directories the table's block
        // writer may be rewriting, so it takes the same table bottom gate - and
        // this function is the caller that must take it, so the flag is `true`.
        let bottom_gate = self.bottom_gate(table);
        let _bottom = (!crate::engine::statements::HeldBottom::holds(&self.table_key(table)))
            .then(|| crate::scheduler::block_on(bottom_gate.lock()));
        self.add_column_files_inner(table, first, blocks)
    }

    fn add_column_files_inner(
        &self,
        table: &str,
        first: usize,
        blocks: &[(u32, u16, usize, Option<usize>)],
    ) -> Result<()> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Ok(());
        };
        if first >= meta.types.len() {
            return Ok(());
        }
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block writer"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let layouts: Vec<ColumnLayout> = meta
            .types
            .iter()
            .zip(&types)
            .map(|(t, ct)| {
                layout_of(t, *ct)
                    .ok_or_else(|| Error::Invalid(format!("no .col layout for column type '{t}'")))
            })
            .collect::<Result<_>>()?;
        let defaults = self.table_defaults(&table)?;
        // The cell text the mirror would write for the default: the same
        // `Value` -> text round trip a rewrite used, so the bytes match.
        let cells: Vec<String> = (first..meta.types.len())
            .map(|i| match defaults.get(i).and_then(|d| d.as_deref()) {
                Some(d) => mirror_cell_text(&Value::from_literal(&default_literal(d))),
                None => mirror_cell_text(&Value::Null),
            })
            .collect();
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        // The caller's list comes from the block index, which does not know a
        // block the checkpoint wrote but has not published yet. Such a block
        // would never get the new column file, and every later decode of it
        // fails (`{id}.col` missing is an error, not "the column is empty").
        // Take the union with what the directory holds.
        let mut work: Vec<(u32, u16, usize, Option<usize>)> = blocks.to_vec();
        let table_root = root
            .join(format!("db_{db_id}"))
            .join(format!("tbl_{}", meta.id));
        if let Ok(segments) = std::fs::read_dir(&table_root) {
            for segment in segments.flatten() {
                let name = segment.file_name().to_string_lossy().to_string();
                let Some(segment_id) = name
                    .strip_prefix("seg_")
                    .and_then(|s| s.parse::<u32>().ok())
                else {
                    continue;
                };
                let Ok(block_dirs) = std::fs::read_dir(segment.path()) else {
                    continue;
                };
                for block_dir in block_dirs.flatten() {
                    let name = block_dir.file_name().to_string_lossy().to_string();
                    let Some(block_id) = name
                        .strip_prefix("blk_")
                        .and_then(|s| s.parse::<u16>().ok())
                    else {
                        continue;
                    };
                    if work
                        .iter()
                        .any(|(s, b, _, _)| *s == segment_id && *b == block_id)
                    {
                        continue;
                    }
                    let id = BlockId {
                        db_id,
                        table_id: meta.id,
                        segment_id,
                        block_id,
                    };
                    let Some(version) = self.read_block_version(id) else {
                        continue;
                    };
                    let rows = version
                        .created
                        .last()
                        .map(|c| c.cumulative_row_count as usize)
                        .unwrap_or(0);
                    if rows == 0 {
                        continue;
                    }
                    work.push((segment_id, block_id, rows, None));
                }
            }
        }
        if work.is_empty() {
            // No segment: the column's catalog record is all there is to write
            // (`AddColumnsData` returns early on the same condition).
            return Ok(());
        }
        for (segment_id, block_index, rows, bitmap_bits) in &work {
            {
                let id = BlockId {
                    db_id,
                    table_id: meta.id,
                    segment_id: *segment_id,
                    block_id: *block_index,
                };
                self.drop_block_maps(meta.id);
                for (k, i) in (first..meta.types.len()).enumerate() {
                    let chunk: Vec<Vec<String>> =
                        (0..*rows).map(|_| vec![cells[k].clone()]).collect();
                    let columns = self.encode_block_columns(
                        &meta.types[i..i + 1],
                        &meta.ids[i..i + 1],
                        &types[i..i + 1],
                        &layouts[i..i + 1],
                        &chunk,
                        *bitmap_bits,
                    )?;
                    let dir =
                        storage::definition::format::write_block_column(&root, id, &columns[0])
                            .map_err(|e| Error::Io {
                                path: root.clone(),
                                source: e,
                            })?;
                    // The reference opens an outline file for every
                    // variable-length column and aborts the read when it is
                    // missing, even when the file is empty.
                    if is_var_buffer_type(types[i]) {
                        let path = dir.join(storage::definition::format::column_outline_file(
                            meta.ids[i],
                        ));
                        if !path.exists() {
                            let _ = storage::definition::format::write_synced(&path, b"");
                        }
                    }
                }
            }
        }
        self.invalidate_block_mirror(&table);
        Ok(())
    }

    /// The rows still in the append buffer have no block file yet, so
    /// `AddColumnsData` never reaches them (it walks the table's segments);
    /// the reference's block **buffer** gains the column instead. Append the
    /// default cell to each buffered row, so the mirror writes it with the
    /// block.
    pub(crate) fn patch_buffered_add_columns(&self, table: &str, first: usize, defaults: &[Value]) {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&key) else {
            return;
        };
        for (_, cells) in entry.pending_rows.iter_mut() {
            cells.truncate(first);
            cells.extend(defaults.iter().cloned());
        }
        // The cells changed shape, so the running byte total is stale: an added
        // column with a large default would otherwise be uncharged, and the
        // buffer could cross the quota before anything counted it. DDL is off
        // the write path, so the extra pass is affordable.
        entry.retain_buffered(|_| true);
    }

    /// The buffer half of `DropColumnsData`: a buffered row loses the cell the
    /// schema no longer has (`position` is where the column was).
    pub(crate) fn patch_buffered_drop_column(&self, table: &str, position: usize) {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&key) else {
            return;
        };
        for (_, cells) in entry.pending_rows.iter_mut() {
            if position < cells.len() {
                cells.remove(position);
            }
        }
        // The dropped column's bytes are still in the running total, which would
        // make the quota think the buffer is bigger than it is and drain early.
        entry.retain_buffered(|_| true);
    }

    /// `Cleanup` for an `ALTER TABLE … DROP COLUMN`: removes the block column
    /// files the `drop|blk_col|` markers name.
    ///
    /// `DropColumnsData` writes one marker per block and leaves the file
    /// (`new_txn_data_impl.cpp:1403`); the reference never deletes the marker,
    /// only the file, and neither does this. The marker carries the column def
    /// the block had, so the id of the file to remove is in its key.
    pub(crate) fn reclaim_dropped_block_columns(&self) -> Result<usize> {
        let store = self.storage.catalog.store();
        let root = self.blocks_root();
        let mut removed = 0usize;
        let mut touched: Vec<u64> = Vec::new();
        for (k, _) in store.scan_prefix(b"drop|blk_col|")? {
            let key = String::from_utf8_lossy(&k).to_string();
            let Some(rest) = key.strip_prefix("drop|blk_col|") else {
                continue;
            };
            // `{db_id}/{table_id}/{segment_id}/{block_id}/{column_def}`
            let mut parts = rest.splitn(5, '/');
            let (Some(db), Some(table), Some(seg), Some(block), Some(def)) = (
                parts.next(),
                parts.next(),
                parts.next(),
                parts.next(),
                parts.next(),
            ) else {
                continue;
            };
            // `{"column_id":N,"column_name":…}`
            let Some(id) = def
                .split("\"column_id\":")
                .nth(1)
                .and_then(|s| s.split(|c: char| !c.is_ascii_digit()).next())
                .and_then(|s| s.parse::<u64>().ok())
            else {
                continue;
            };
            let dir = root.join(format!("db_{db}/tbl_{table}/seg_{seg}/blk_{block}"));
            for name in [
                storage::definition::format::column_file(id),
                storage::definition::format::column_outline_file(id),
            ] {
                if dir.join(&name).exists() {
                    let _ = storage::chunk_store::remove_file(dir.join(&name));
                    removed += 1;
                }
            }
            if let Ok(table_id) = table.parse::<u64>() {
                touched.push(table_id);
            }
        }
        touched.sort_unstable();
        touched.dedup();
        for table_id in touched {
            self.drop_block_maps(table_id);
        }
        Ok(removed)
    }

    /// Whether `seq` names a row a delete may hide: a live slot of a visible
    /// block.
    ///
    /// The reference decides this from the block's `BlockOffset` and `version`;
    /// with the rows in the blocks there is no key to look up.
    pub(crate) fn row_is_present(&self, table: &str, seq: usize) -> bool {
        let table = self.qualified_table(table);
        // Every check below reads the engine's state *in place*: this runs once
        // per row of a bulk `DELETE`/`UPDATE`, and cloning the delete journal
        // and the append buffer per row made a 5k-row statement quadratic
        // (300 us/row against 16 us/row with nothing buffered).
        if self.is_deleted(&table, seq) {
            return false;
        }
        // A row a commit appended since the last checkpoint is in the table's
        // append buffer, not in a block file: it exists all the same
        // (`DeleteInBlock` addresses the same slot whether or not the block has
        // been written out yet).
        if self.pending_contains(&table, seq) {
            return true;
        }
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return false;
        };
        let id = BlockId {
            db_id: self.database_id_of(&table),
            table_id: meta.id,
            segment_id: (seq >> 32) as u32,
            block_id: ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16,
        };
        if !self.visible_block_ids(&table).contains(&id) {
            return false;
        }
        // The block's mapping is shared, so a bulk statement reads each
        // `version` file once instead of once per row.
        let Ok(block) = self.block_map(&table, &self.blocks_root(), id) else {
            return false;
        };
        let version = block.version();
        let slot = (seq & 0xffff_ffff) % DEFAULT_BLOCK_CAPACITY;
        if slot >= Self::visible_rows_of(version) {
            return false;
        }
        version.deleted.get(slot).copied().unwrap_or(0) == 0
    }

    /// Whether `seq` is in the table's append buffer. Scans it in place - no
    /// copy of the rows, which is what a per-row caller can afford.
    pub(crate) fn pending_contains(&self, table: &str, seq: usize) -> bool {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|entry| entry.buffered_row(seq).is_some())
            .unwrap_or(false)
    }

    /// Whether the delete journal holds `seq`.
    ///
    /// One hash lookup into the journal's set. The journal itself is a `Vec`
    /// and used to be scanned here, which a bulk `DELETE`/`UPDATE` pays once
    /// per row (`row_is_present`) and made the statement quadratic in the
    /// journal's length.
    pub(crate) fn is_deleted(&self, table: &str, seq: usize) -> bool {
        self.deleted_set(table).contains(&(seq as u64))
    }

    /// The blocks a reader may see: the table's block index minus every segment
    /// a compaction deprecated, in `(segment, block)` order.
    ///
    /// This is the **only** place visibility is decided, so the scan, the
    /// per-block range and the row count cannot disagree. A compaction keeps
    /// the source segments' files and records and hides them with `drop|seg|…`
    /// alone, so a reader that misses the mark returns the merged rows *and*
    /// the source rows: 40,960 rows for a 32,768-row table.
    pub(crate) fn visible_block_ids(&self, table: &str) -> Vec<BlockId> {
        self.visible_block_ids_inner(table)
    }

    pub(crate) fn visible_block_ids_inner(&self, table: &str) -> Vec<BlockId> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let db_id = self.database_id_of(&table);
        // Through the catalog, not the raw store: a compaction marks the
        // source segments inside its transaction, and a read afterwards must
        // see those marks even before the checkpoint writes them through.
        //
        // A mark only hides its segment from readers that started after it. A
        // compaction records the drops with its commit ts and publishes the
        // merged blocks afterwards, so a reader at an older ts - a query that was
        // already running - must keep the source segments: its full-text index
        // still names their rows, and hiding them turned every hit into a row
        // that could not be fetched (`0` rows on a table full of matches).
        // `u64::MAX` (a checkpoint, an index rebuild, `Cleanup`) applies them all.
        let read_ts = crate::read_ts();
        let dropped: std::collections::HashSet<u32> = self
            .storage
            .catalog
            .dropped_segments_with_ts(&db_id.to_string(), &meta.id.to_string())
            .unwrap_or_default()
            .into_iter()
            .filter(|(_, ts)| *ts <= read_ts)
            .map(|(segment, _)| segment)
            .collect();
        let Some(index) = self.block_index_for(&table) else {
            return Vec::new();
        };
        let index = index.lock().unwrap();
        index
            .metas()
            .filter(|meta| !dropped.contains(&meta.id.segment_id))
            .map(|meta| meta.id)
            .collect()
    }

    pub(crate) fn read_block_version(&self, id: BlockId) -> Option<BlockVersionFile> {
        let dir = self
            .blocks_root()
            .join(storage::definition::format::block_dir(
                id.db_id,
                id.table_id,
                id.segment_id,
                id.block_id,
            ));
        BlockVersionFile::decode(
            &storage::chunk_store::read_file(dir.join(storage::definition::format::VERSION_FILE))
                .ok()?,
        )
        .ok()
    }

    /// The rows a block's `version` makes visible: its created rows minus the
    /// slots marked deleted (`BlockVersion::GetRowCount`).
    /// How many rows of `version` exist at `read_ts` (`cumulative_row_count` of
    /// the newest `create` the reader can see).
    pub(crate) fn created_at(version: &BlockVersionFile, read_ts: u64) -> usize {
        version
            .created
            .iter()
            .rfind(|c| c.create_ts <= read_ts)
            .map(|c| c.cumulative_row_count.max(0) as usize)
            .unwrap_or(0)
    }

    /// Whether slot `slot` of the block is visible at `read_ts`.
    pub(crate) fn slot_visible(
        version: &BlockVersionFile,
        slot: usize,
        created_at: usize,
        read_ts: u64,
    ) -> bool {
        if slot >= created_at {
            return false;
        }
        match version.deleted.get(slot).copied().unwrap_or(0) {
            0 => true,
            deleted => deleted > read_ts,
        }
    }

    pub(crate) fn visible_rows_of(version: &BlockVersionFile) -> usize {
        let created = version
            .created
            .last()
            .map(|c| c.cumulative_row_count.max(0) as usize)
            .unwrap_or(0);
        let deleted = version
            .deleted
            .iter()
            .take(created)
            .filter(|ts| **ts != 0)
            .count();
        created - deleted
    }

    /// Rows reserved in each segment, read off the blocks: a segment's extent is
    /// its last block's window end (`block * capacity + created`), deleted rows
    /// included - the reference keeps a block append-only, so its `version` says
    /// how far the segment was written even when slots were later marked.
    ///
    /// This is the layout a directory whose rows live only in the blocks still
    /// has: the allocator's in-memory map is empty after a restart, and the row
    /// journal holds nothing to derive it from.
    pub(crate) fn block_segment_row_counts(&self, table: &str) -> Vec<(u32, u64)> {
        let mut out: std::collections::BTreeMap<u32, u64> = Default::default();
        for id in self.visible_block_ids(table) {
            let Some(version) = self.read_block_version(id) else {
                continue;
            };
            let created = version
                .created
                .last()
                .map(|c| c.cumulative_row_count.max(0) as u64)
                .unwrap_or(0);
            let end = u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64 + created;
            out.entry(id.segment_id)
                .and_modify(|e| *e = (*e).max(end))
                .or_insert(end);
        }
        out.into_iter().collect()
    }

    /// The rows the block mirror holds, in the very blocks a scan reads.
    ///
    /// The count follows the same visible set and the same `version` the scan
    /// decodes, so `count(*)`-style counters and a full scan of the same table
    /// cannot disagree - which they did while one side walked every block
    /// directory (deprecated segments included) and the other skipped the
    /// segments `drop|seg|…` had deprecated.
    ///
    /// A stale mirror answers nothing at all: after a compaction the merged
    /// segment is still only in the journal, and a number derived from the old
    /// files would describe rows that have moved.
    pub fn visible_row_count(&self, table: &str) -> Result<usize> {
        Ok(self.block_visible_row_count(table))
    }

    /// The rows the block files make visible, whether or not they are current.
    ///
    /// A count never has to be refused: the files say how many rows they hold,
    /// and the journal says what changed above them (see [`Engine::row_count`]).
    pub(crate) fn block_visible_row_count(&self, table: &str) -> usize {
        // The count answers the same question the scan does, so it is taken at
        // the same timestamp - including which deletes have happened.
        let read_ts = crate::read_ts();
        let deleted = self.tombstone_set_at(table, read_ts).unwrap_or_default();
        let pending = self.pending_rows_of(table);
        let mut rows = 0usize;
        // Every row id the visible blocks cover, live or not: the append buffer
        // may still list one of them.
        let mut have: std::collections::HashSet<usize> = Default::default();
        for id in self.visible_block_ids(table) {
            let Some(version) = self.read_block_version(id) else {
                continue;
            };
            let created = Self::created_at(&version, read_ts);
            let mut live = 0usize;
            for slot in 0..created {
                if !Self::slot_visible(&version, slot, created, read_ts) {
                    continue;
                }
                live += 1;
                // A delete the checkpoint has not written yet is in the table's
                // delete log and not in the block's `version`; a delete it has
                // written is in both, and counting it twice would under-report.
                let seq = ((id.segment_id as u64) << 32)
                    | (u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64 + slot as u64);
                if deleted.contains(&seq) {
                    live = live.saturating_sub(1);
                }
            }
            rows += live;
            // A row the mirror has since written is still listed in the append
            // buffer. The scan drops such a row by id (it merges the buffer over
            // the blocks and skips ids the blocks already produced), so the
            // count has to drop it too: `DROP COLUMN` removes the rewritten ids
            // from the buffer, but a rewrite that lands under another name - a
            // rename racing it - leaves them there, and counting both sides
            // reported twice the table.
            let base = ((id.segment_id as u64) << 32)
                | (u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64);
            for slot in 0..created as u64 {
                have.insert((base + slot) as usize);
            }
        }
        // Rows a commit appended since the last checkpoint live in the append
        // buffer, not in a file yet.
        let view = self.append_view(table, read_ts);
        rows + pending
            .iter()
            .filter(|(seq, _)| {
                !have.contains(seq)
                    && !deleted.contains(&(*seq as u64))
                    && view.as_ref().is_none_or(|view| view.covers(*seq, read_ts))
            })
            .count()
    }

    /// `NewCatalog::GetBlockVisibleRange` for one block of a visible segment:
    /// the rows its `version` makes visible. A deprecated segment has no
    /// visible block at all.
    pub fn block_visible_range(
        &self,
        table: &str,
        segment: u32,
        block: u16,
    ) -> Option<(usize, usize)> {
        let id = self
            .visible_block_ids(table)
            .into_iter()
            .find(|id| id.segment_id == segment && u32::from(id.block_id) == u32::from(block))?;
        let version = self.read_block_version(id)?;
        Some((0, Self::visible_rows_of(&version)))
    }

    /// Reads one row by its storage id, one block at a time.
    ///
    /// The id names the segment, the block and the slot
    /// (`segment << 32 | block * capacity + slot`), so a lookup touches one
    /// block instead of walking the table: a compaction moves every row of its
    /// sources, and materialising the table once per row would make it
    /// quadratic. The block is decoded once and kept while the caller walks the
    /// rows in order, so a segment costs one decode per block and O(1) memory -
    /// the reference's scan streams through the buffer manager the same way.
    ///
    /// Journal first: a row written after the last checkpoint (and every row
    /// while the mirror is dirty) is there, and so is its tombstone.
    pub(crate) fn block_cursor(&self, table: &str) -> Result<BlockCursor<'_>> {
        BlockCursor::new(self, table)
    }

    /// Maps a block, sharing one mapping per block with every other reader in
    /// the process.
    ///
    /// The reference's block read goes through `BufferManager::Get`, which reads
    /// the block into an owned buffer and evicts it by budget. A sealed block's
    /// files never change, so the port maps them instead: the pages stay in the
    /// page cache (which is what the budget was approximating) and the mapping
    /// is shared, not copied. A block the index does not know yet (written
    /// between its open and this read) is mapped from its directory.
    pub(crate) fn block_map(
        &self,
        table: &str,
        root: &Path,
        id: BlockId,
    ) -> std::io::Result<Arc<MappedBlock>> {
        if let Some(block) = self.context.block_maps.lock().unwrap().get(&id) {
            self.context
                .block_map_hits
                .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
            return Ok(block.clone());
        }
        self.context
            .block_map_misses
            .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        let block = match self.block_index_for(table) {
            Some(index) => {
                let index = index.lock().unwrap();
                match index.get_block(&id) {
                    Ok((_, block)) => block,
                    Err(_) => Arc::new(MappedBlock::open(root, id)?),
                }
            }
            None => Arc::new(MappedBlock::open(root, id)?),
        };
        // Another reader may have mapped the block while this one did the I/O:
        // keep the first mapping, so a block has exactly one in the process.
        let mut maps = self.context.block_maps.lock().unwrap();
        Ok(maps.entry(id).or_insert(block).clone())
    }

    /// Drops the mappings of every block of `table_id`: the block files under
    /// it changed (a rewrite, a compaction, a delete), so the mappings no
    /// longer describe them.
    pub(crate) fn drop_block_maps(&self, table_id: u64) {
        self.context
            .block_maps
            .lock()
            .unwrap()
            .retain(|id, _| id.table_id != table_id);
    }

    /// Drops the mapping of one block, for a rewrite that only touches that
    /// block.
    ///
    /// [`Self::drop_block_maps`] drops the whole table's, which is what a
    /// compaction needs (it rewrites every block). A block-at-a-time write must
    /// not: the loop re-maps the next block anyway, so dropping the table's
    /// mappings turns one `COPY` into "unmap everything, map one, unmap
    /// everything, …". Measured on an 8 M-row import that was the remaining
    /// 76% of the profile after the row lookup was made a slice.
    pub(crate) fn drop_block_map(&self, id: BlockId) {
        self.context.block_maps.lock().unwrap().remove(&id);
    }

    /// The append/delete/import journals a table's block files record.
    ///
    /// The reference keeps this history in the files themselves: one `create`
    /// per append and one ts per deleted slot in each block's `version`, and a
    /// segment's prefilter for its import extent. The KV copies are therefore
    /// only a cache, and a fresh process rebuilds them here.
    ///
    /// Returns `(appends, append_ts, append_copy, deletes, delete_ts, imports)`
    /// with `imports` as `(segment, rows, first create ts)`.
    #[allow(clippy::type_complexity)]
    pub(crate) fn journals_from_blocks(
        &self,
        table: &str,
    ) -> (
        Vec<u64>,
        Vec<u64>,
        Vec<u64>,
        Vec<u64>,
        Vec<u64>,
        Vec<(u32, u64, u64)>,
    ) {
        let table = self.qualified_table(table);
        let db_id = self.database_id_of(&table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Default::default();
        };
        let root = self.blocks_root();
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Default::default();
        };
        let mut appends = Vec::new();
        let mut append_ts = Vec::new();
        let mut append_copy = Vec::new();
        let mut deletes = Vec::new();
        let mut delete_ts = Vec::new();
        let mut segment_rows: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut segment_first_ts: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut total = 0u64;
        for id in self.visible_block_ids(&table) {
            let Ok(block) = self.block_map(&table, &root, id) else {
                continue;
            };
            let version = block.version();
            let Some(rows) = self.decode_block_cells(&table, &root, &meta, &types, id, None) else {
                continue;
            };
            let mut seen = 0u64;
            for create in &version.created {
                let cumulative = create.cumulative_row_count.max(0) as u64;
                let added = cumulative.saturating_sub(seen);
                seen = cumulative;
                total += added;
                appends.push(total);
                append_ts.push(create.create_ts);
                append_copy.push(0);
                segment_first_ts
                    .entry(id.segment_id)
                    .or_insert(create.create_ts);
            }
            *segment_rows.entry(id.segment_id).or_default() += rows.len() as u64;
            for (slot, (seq, _cells)) in rows.iter().enumerate() {
                if let Some(ts) = version.deleted.get(slot).copied().filter(|t| *t != 0) {
                    deletes.push(*seq as u64);
                    delete_ts.push(ts);
                }
            }
        }
        let mut imports = Vec::new();
        for (segment, rows) in &segment_rows {
            let sealed = self
                .storage
                .catalog
                .store()
                .get(format!("seg|{db_id}|{}|{segment}|fast_rough_filter", meta.id).as_bytes())
                .ok()
                .flatten()
                .is_some();
            if !sealed {
                continue;
            }
            let first = segment_first_ts.get(segment).copied().unwrap_or(0);
            imports.push((*segment, *rows, first));
        }
        (appends, append_ts, append_copy, deletes, delete_ts, imports)
    }

    /// Reads the block mirror back, returning `(columns, rows)`.
    ///
    /// This never consults the KV store, so a successful read proves the mirror
    /// carries the table on its own.
    pub fn read_blocks(&self, table: &str) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (columns, rows) = self.read_blocks_wanted(table, None, &[])?;
        Ok((columns, rows.into_iter().map(|(_, cells)| cells).collect()))
    }

    /// Like [`Self::read_blocks`] but decodes only `wanted` columns; the
    /// others are returned as `"null"`. This is the column-projected read path
    /// for `SELECT id, name` (plan #87).
    pub fn read_blocks_selected(
        &self,
        table: &str,
        wanted: &[usize],
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (columns, rows) = self.read_blocks_wanted(table, Some(wanted), &[])?;
        Ok((columns, rows.into_iter().map(|(_, cells)| cells).collect()))
    }

    /// [`Self::read_blocks_selected`] plus block pruning: a block whose
    /// `blk|…|fast_rough_filter` record proves the predicate cannot match is
    /// not read at all (C++ `FastRoughFilter::MayContain`, applied per block by
    /// the scan).
    pub fn read_blocks_pruned(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<BlockRows> {
        self.read_blocks_wanted(table, Some(wanted), prune)
    }

    /// [`Self::read_blocks_pruned`] with the cells already decoded to
    /// `Value`: what a scan reads, so it does not parse the text back.
    ///
    /// The decode runs on the CPU pool (`spawn_cpu`) and this awaits it, so the
    /// statement task keeps its worker while a wide block is decoded
    /// (plan 6.13).
    pub fn read_blocks_pruned_values(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<BlockValueRows> {
        self.read_blocks_values(table, Some(wanted), prune, None, None)
    }

    /// [`Self::read_blocks_pruned_values`] with a row predicate applied
    /// while the rows are built: a rejected row never becomes a `Vec`.
    pub fn read_blocks_filtered_values(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
        keep: &dyn Fn(&[Value]) -> bool,
    ) -> Result<BlockValueRows> {
        self.read_blocks_values(table, Some(wanted), prune, Some(keep), None)
    }

    /// [`Self::read_blocks_pruned_values`] restricted to `rows`: a row the
    /// index did not return is skipped before its cells are decoded, so an
    /// indexed query costs its candidates rather than the table.
    pub fn read_blocks_candidates_values(
        &self,
        table: &str,
        wanted: &[usize],
        rows: &std::collections::HashSet<usize>,
    ) -> Result<BlockValueRows> {
        // `None` = every column: a result row carries all of them, and asking for
        // a column subset here would pad the rest with NULL.
        let wanted = (!wanted.is_empty()).then_some(wanted);
        self.read_blocks_values(table, wanted, &[], None, Some(rows))
    }

    /// Every row the blocks hold, tombstoned slots included.
    ///
    /// The index mirrors are built from a column and keep deleted rows (the
    /// reference's chunks count every document a segment ever held), so this is
    /// the block counterpart of [`Engine::fetch_rows_including_deleted`].
    pub(crate) fn read_blocks_including_deleted(&self, table: &str) -> Vec<(usize, Vec<String>)> {
        self.read_blocks_including_deleted_from(table, 0)
    }

    /// [`Self::read_blocks_including_deleted`] from row id `first` on.
    ///
    /// Blocks below `first` are never decoded, so a reopened engine can index
    /// only the rows its sealed chunks do not cover - the reference's
    /// `MemoryIndexer` does the same when it starts past a dumped segment.
    pub(crate) fn read_blocks_including_deleted_from(
        &self,
        table: &str,
        first: usize,
    ) -> Vec<(usize, Vec<String>)> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Vec::new();
        };
        let root = self.blocks_root();
        let mut out = Vec::new();
        for id in self.visible_block_ids(&table) {
            // A block covers `segment << 32 | block * capacity` for
            // `DEFAULT_BLOCK_CAPACITY` rows, so its end decides whether it can
            // be skipped without decoding it.
            let block_first = (u64::from(id.segment_id) << 32)
                + u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64;
            if block_first + DEFAULT_BLOCK_CAPACITY as u64 <= first as u64 {
                continue;
            }
            if let Some(cells) = self.decode_block_cells(&table, &root, &meta, &types, id, None) {
                out.extend(cells.into_iter().filter(|(seq, _)| *seq >= first));
            }
        }
        for (seq, values) in self.pending_rows_of(&table) {
            if seq < first {
                continue;
            }
            out.push((seq, values.iter().map(mirror_cell_text).collect()));
        }
        out.sort_by_key(|(seq, _)| *seq);
        out
    }

    /// Number of rows the block mirror currently holds, over every visible
    /// segment and with the deleted slots taken out.
    ///
    /// The old version of this walked `seg_0` only and counted a block's
    /// created rows, so after a compaction it reported the *deprecated* source
    /// segment (8,192) while a scan of the same table returned the merged
    /// segment's 32,768 - the "40,960 vs 32,768" divergence between the two
    /// row counts.
    pub fn mirror_block_row_count(&self, table: &str) -> Result<usize> {
        self.visible_row_count(table)
    }

    /// The cells of one mirror block, in row order, as the block reader renders
    /// them (`null` for a NULL slot).
    ///
    /// A block covers a fixed `DEFAULT_BLOCK_CAPACITY` window of its segment, so
    /// row `n` of block `b` is the id `segment << 32 | b * capacity + n` - the id
    /// the reference stored it under.
    pub(crate) fn decode_block_cells(
        &self,
        table: &str,
        root: &Path,
        meta: &catalog::TableMeta,
        types: &[i32],
        id: BlockId,
        wanted: Option<&[usize]>,
    ) -> Option<Vec<(usize, Vec<String>)>> {
        let block = self.block_map(table, root, id).ok()?;
        Self::decode_rows_of(
            &meta.ids,
            &meta.types,
            types,
            id,
            block.version(),
            block.as_ref(),
            wanted,
        )
    }

    /// One block's visible rows, as a scan step: map it, decode it, then drop
    /// the slots the block's `version` hides *at the statement's* `read_ts`.
    ///
    /// This is the unit `read_blocks_values` fans out over tokio tasks (plan
    /// 6.13.2): it touches no shared mutable state - the mapping is an `Arc`,
    /// the column bytes are immutable - so blocks decode in parallel and the
    /// row order is restored by the caller's `zip` with its block list.
    #[allow(clippy::too_many_arguments)] // one argument per piece of the block it reads
    fn decode_block_visible(
        &self,
        table: &str,
        root: &Path,
        meta: &catalog::TableMeta,
        types: &[i32],
        id: BlockId,
        wanted: Option<&[usize]>,
        keep: RowFilter<'_>,
        rows_wanted: Option<&std::collections::HashSet<usize>>,
        read_ts: u64,
    ) -> BlockDecode {
        let Ok(block) = self.block_map(table, root, id) else {
            return BlockDecode::OpenFailed;
        };
        let version = block.version();
        let Some(cells) = Self::decode_rows_of_values(
            &meta.ids,
            &meta.types,
            types,
            id,
            version,
            block.as_ref(),
            wanted,
            keep,
            rows_wanted,
        ) else {
            return BlockDecode::DecodeFailed;
        };
        // A deleted slot keeps its cells in the block file (the reference hides
        // it with the version, it does not rewrite the block), so the scan
        // drops it here - the same slots `visible_row_count` subtracts, which
        // is what keeps the two numbers equal.
        //
        // The verdict is taken at the statement's `begin_ts`
        // (`BlockVersion::GetRowCount(begin_ts)`): a slot created after it does
        // not exist yet, and one deleted after it is still there.
        //
        // The slot comes from the row id, not from the position in `cells`: a
        // `keep` predicate drops rejected rows during the decode, so the k-th
        // surviving row is not the k-th slot and enumerating the survivors let
        // a deleted row through whenever some live low slot took its place.
        let created_at = Self::created_at(version, read_ts);
        BlockDecode::Rows(
            cells
                .into_iter()
                .filter(|(seq, _)| {
                    let slot = (*seq as u64 & 0xffff_ffff) as usize % DEFAULT_BLOCK_CAPACITY;
                    Self::slot_visible(version, slot, created_at, read_ts)
                })
                .collect(),
        )
    }

    /// The cells of one block, given its files - the decoder both the engine's
    /// cached reader and the block-backed row source use, so a scan reads
    /// exactly what a rewrite writes.
    pub(crate) fn decode_rows_of(
        ids: &[u64],
        type_text: &[String],
        types: &[i32],
        id: BlockId,
        version: &storage::definition::format::BlockVersionFile,
        columns: &dyn BlockColumns,
        wanted: Option<&[usize]>,
    ) -> Option<Vec<(usize, Vec<String>)>> {
        let rows = Self::decode_rows_of_values(
            ids, type_text, types, id, version, columns, wanted, None, None,
        )?;
        Some(
            rows.into_iter()
                .map(|(seq, cells)| (seq, cells.iter().map(mirror_cell_text).collect()))
                .collect(),
        )
    }

    /// The rows of one block, as the `Value`s the engine works with.
    ///
    /// A scalar cell (`INT8..INT64`, `FLOAT`, `DOUBLE`, the 16-bit floats, a
    /// `BOOL`) is built from its slot directly. Rendering it to text and parsing
    /// it back - what the block mirror used to do for every cell - cost a
    /// `format!` and a re-parse per cell; the types that only exist as text
    /// (varchar, json, date, tensor, sparse, ...) keep the text form, and its
    /// parse, because that is also what a rewrite writes.
    #[allow(clippy::too_many_arguments)] // one argument per piece of the decoded row
    pub(crate) fn decode_rows_of_values(
        ids: &[u64],
        type_text: &[String],
        types: &[i32],
        id: BlockId,
        version: &storage::definition::format::BlockVersionFile,
        columns: &dyn BlockColumns,
        wanted: Option<&[usize]>,
        keep: RowFilter<'_>,
        rows_wanted: Option<&std::collections::HashSet<usize>>,
    ) -> Option<DecodedRows> {
        let row_count: usize = version
            .created
            .last()
            .map(|c| c.cumulative_row_count as usize)
            .unwrap_or(0);
        // Everything a cell needs from its **column** - the body split, the
        // outline kind, the layout and the outline bytes - is the same for
        // every row. Splitting the body per cell copied the whole block's
        // `[data][bitmap]` once per cell, which made a block read
        // O(rows x block_bytes): 40,000 rows spent 68% of the time in memmove.
        struct Prepared<'a> {
            /// Borrowed slices of the block: splitting a body is arithmetic, and
            /// copying it made a ten-row query allocate and copy every column
            /// body of every block it touched (~160 KB per query here).
            body_data: &'a [u8],
            body_null: &'a [u8],
            kind: OutlineKind,
            outline: &'a [u8],
            shape: Option<(usize, i32)>,
            layout: ColumnLayout,
        }
        let mut prepared: Vec<Option<Prepared>> = Vec::with_capacity(types.len());
        for (col, column_type) in types.iter().enumerate() {
            if let Some(wanted) = wanted {
                if !wanted.contains(&col) {
                    prepared.push(None);
                    continue;
                }
            }
            let column_id = ids.get(col).copied().unwrap_or(col as u64);
            let (body, outline) = columns.column(column_id)?;
            let layout = layout_of(&type_text[col], *column_type)?;
            let (body_data, body_null) =
                storage::definition::format::split_body_refs(body, DEFAULT_BLOCK_CAPACITY, layout);
            prepared.push(Some(Prepared {
                body_data,
                body_null,
                kind: outline_kind(&type_text[col]),
                outline,
                shape: embedding_shape(&type_text[col]),
                layout,
            }));
        }
        // One row buffer for the whole block: a row the predicate rejects never
        // becomes its own `Vec`, which is what a selective scan spent most of
        // its time allocating.
        let mut scratch: Vec<Value> = Vec::with_capacity(types.len());
        let mut out = Vec::with_capacity(row_count);
        // The slots of this block the index kept, ascending. Row ids inside a
        // block are consecutive, so a cursor over a sorted list answers "is this
        // slot wanted" in O(1) amortised - a `HashSet` test per slot hashed
        // 2048 integers per block, which `perf` showed inside this loop.
        let wanted_slots: Option<Vec<usize>> = rows_wanted.map(|rows| {
            let base =
                ((id.segment_id as usize) << 32) | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY);
            let mut slots: Vec<usize> = rows
                .iter()
                .filter(|seq| **seq >= base && **seq < base + DEFAULT_BLOCK_CAPACITY)
                .map(|seq| *seq - base)
                .collect();
            slots.sort_unstable();
            slots
        });
        // An index answered: only the slots it kept are decoded, so the loop
        // walks *those* slots instead of the block's whole window. Scanning
        // `0..row_count` and testing each slot cost a point read 8192
        // iterations to decode one row (~8 us per row on an index scan).
        let mut all_at = 0usize;
        let mut kept_at = 0usize;
        loop {
            let row = match &wanted_slots {
                Some(slots) => {
                    // The index may name a row this block does not hold: a row
                    // committed after the block was written lives in the append
                    // buffer, and a recovered engine merges it from there. Slots
                    // are sorted ascending, so the first one at or past the
                    // block's row count ends the walk -- decoding it anyway read
                    // past the column bodies and produced zeroed values (a
                    // recovered row came back as `c1 = 0`).
                    let mut found = None;
                    while let Some(slot) = slots.get(kept_at) {
                        kept_at += 1;
                        if *slot < row_count {
                            found = Some(*slot);
                            break;
                        }
                    }
                    match found {
                        Some(slot) => slot,
                        None => break,
                    }
                }
                None => {
                    if all_at >= row_count {
                        break;
                    }
                    all_at += 1;
                    all_at - 1
                }
            };
            let seq = ((id.segment_id as usize) << 32)
                | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY + row);
            scratch.clear();
            let cells = &mut scratch;
            for (col, column_type) in types.iter().enumerate() {
                let Some(column) = prepared[col].as_ref() else {
                    cells.push(Value::Null);
                    continue;
                };
                let body_data = column.body_data;
                let body_null = column.body_null;
                let outline = column.outline;
                let value =
                    if body_null.len() > row / 8 && body_null[row / 8] & (1 << (row % 8)) == 0 {
                        Value::Null
                    } else if column.kind == OutlineKind::None {
                        match column.layout {
                            ColumnLayout::CompactBit => {
                                match body_data.get(row / 8).map(|b| b & (1 << (row % 8)) != 0) {
                                    Some(bit) => Value::Bool(bit),
                                    None => Value::Null,
                                }
                            }
                            ColumnLayout::Fixed(width) => {
                                match decode_scalar_value(*column_type, width, body_data, row) {
                                    Some(value) => value,
                                    // A fixed layout this decoder does not cover
                                    // (`DATE`, `TIME`, `DATETIME`): keep the text.
                                    None => decode_value_shaped(
                                        &type_text[col],
                                        *column_type,
                                        body_data,
                                        row,
                                        outline,
                                        column.shape,
                                    )
                                    .map(parse_owned_value)
                                    .unwrap_or(Value::Null),
                                }
                            }
                            ColumnLayout::VarLen(_) => decoded_text_value(
                                decode_value_shaped(
                                    &type_text[col],
                                    *column_type,
                                    body_data,
                                    row,
                                    outline,
                                    column.shape,
                                ),
                                *column_type,
                            ),
                        }
                    } else {
                        // Tensor / sparse / array slots point into the column's
                        // outline; their cells exist only as text.
                        let text = match column.kind {
                            OutlineKind::Tensor => {
                                let at = row * 8;
                                decode_tensor_value(
                                    &type_text[col],
                                    body_data.get(at..at + 8).unwrap_or(&[]),
                                    outline,
                                )
                            }
                            OutlineKind::TensorArray => {
                                let at = row * 8;
                                decode_tensorarray_value(
                                    &type_text[col],
                                    body_data.get(at..at + 8).unwrap_or(&[]),
                                    outline,
                                )
                            }
                            OutlineKind::Sparse => {
                                let at = row * 16;
                                decode_sparse_value(
                                    &type_text[col],
                                    body_data.get(at..at + 16).unwrap_or(&[]),
                                    outline,
                                )
                            }
                            OutlineKind::Array => {
                                let at = row * 8;
                                decode_array_value(
                                    &type_text[col],
                                    body_data.get(at..at + 8).unwrap_or(&[]),
                                    outline,
                                )
                            }
                            OutlineKind::None => None,
                        };
                        text.map(parse_owned_value).unwrap_or(Value::Null)
                    };
                cells.push(value);
            }
            match keep {
                Some(keep) => {
                    if keep(cells) {
                        out.push((seq, cells.clone()));
                    }
                }
                // No predicate: the row takes its buffer with it, which is the
                // one allocation a scanned row always paid.
                None => {
                    out.push((seq, std::mem::take(&mut scratch)));
                    scratch = Vec::with_capacity(types.len());
                }
            }
        }
        Some(out)
    }

    /// The append log as a reader needs it: one snapshot of the records, the
    /// stamps and the segment bases, taken **once** per scan (a per-row lookup
    /// would allocate three vectors per row).
    pub(crate) fn append_view(&self, table: &str, read_ts: u64) -> Option<AppendView> {
        if read_ts == u64::MAX {
            return None;
        }
        Some(AppendView {
            totals: self.append_totals(table),
            stamps: self.append_ts(table),
            bases: self.segment_row_counts(table),
        })
    }

    /// [`Self::pending_rows_of`] restricted to the id window `[first, last)`,
    /// in row-id order.
    /// How many rows sit in the append buffer - the rows written since the
    /// last checkpoint, which the block files do not hold yet.
    ///
    /// Cheap enough for a gate that runs on a timer (the rows themselves are
    /// not copied), unlike [`Self::pending_rows_of`].
    pub(crate) fn buffered_row_count(&self, table: &str) -> usize {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.pending_rows.len())
            .unwrap_or(0)
    }

    /// The rows the commits since the last checkpoint appended, by row id
    /// (the reference's `BufferObject` for the blocks they land in).
    pub(crate) fn pending_rows_of(&self, table: &str) -> Vec<(usize, Vec<Value>)> {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.pending_rows.clone())
            .unwrap_or_default()
    }

    /// The buffered rows among `ids`, looked up once for the whole batch: the
    /// per-row `pending_row` paid a `table_key` (a formatted, lowercased
    /// string) and a lock for every id.
    pub(crate) fn buffered_rows_for(
        &self,
        table: &str,
        ids: &std::collections::HashSet<usize>,
    ) -> std::collections::HashMap<usize, Vec<Value>> {
        let key = self.table_key(table);
        let tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get(&key) else {
            return Default::default();
        };
        if entry.pending_rows.is_empty() {
            return Default::default();
        }
        entry
            .pending_rows
            .iter()
            .filter(|(seq, _)| ids.contains(seq))
            .map(|(seq, values)| (*seq, values.clone()))
            .collect()
    }
    /// One row of the append buffer. The point-read path must not clone the
    /// whole buffer (a 20k-row insert made every search pay for 20k rows), and
    /// it must not scan it either: the buffer keeps a `seq -> position` map, so
    /// this is one lookup plus the row's own clone.
    pub(crate) fn pending_row(&self, table: &str, seq: usize) -> Option<Vec<Value>> {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.buffered_row(seq).cloned())
    }

    /// The stored text of one buffered row, for the paths that read a table the
    /// block cursor cannot serve (a table whose columns have no block layout has
    /// nothing but the append buffer).
    pub(crate) fn pending_row_text(&self, table: &str, seq: usize) -> Result<Option<Vec<u8>>> {
        if self.is_deleted(table, seq) {
            return Ok(None);
        }
        let Some(values) = self.pending_row(table, seq) else {
            return Ok(None);
        };
        let types = self.column_types(table)?;
        let literals: Vec<Literal> = values
            .iter()
            .map(|v| Literal::Str(mirror_cell_text(v)))
            .collect();
        Ok(Some(
            self.encode_row_stored(table, &literals, &types)?
                .into_bytes(),
        ))
    }

    /// The append buffer as a scan reads it, **already in `Value`s**.
    ///
    /// This is the table whose rows have not reached a checkpoint yet - the
    /// shape a fresh import leaves behind, and what the SDK benchmark measures.
    /// The generic path renders every cell to text (`mirror_cell_text`) and the
    /// caller parses it back, so a row that is already a `Value` pays two
    /// allocations per cell; here it is cloned once and used as it is.
    ///
    /// `None` means the table has blocks, and the caller takes the block path
    /// (where the same rows may be shadowed by a block's cells).
    pub(crate) fn buffered_rows_values(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
    ) -> Option<DecodedRows> {
        let table = self.qualified_table(table);
        if !self.visible_block_ids(&table).is_empty() {
            return None;
        }
        let key = self.table_key(&table);
        if self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|entry| entry.pending_rows.is_empty())
            .unwrap_or(true)
        {
            return None;
        }
        let read_ts = crate::read_ts();
        let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
        let view = self.append_view(&table, read_ts);
        // The buffer is cloned cell by cell under its own lock: cloning the
        // whole row first (`pending_rows_of`) copied the columns this read does
        // not want - a 20,000-row point filter copied 20,000 varchar strings to
        // throw them away.
        let tables = self.tables.lock().unwrap();
        let entry = tables.get(&key)?;
        let mut out: Vec<(usize, Vec<Value>)> = Vec::with_capacity(entry.pending_rows.len());
        for (seq, values) in &entry.pending_rows {
            if dead.contains(&(*seq as u64)) {
                continue;
            }
            if let Some(view) = &view {
                if !view.covers(*seq, read_ts) {
                    continue;
                }
            }
            let cells: Vec<Value> = values
                .iter()
                .enumerate()
                .map(|(i, v)| match wanted {
                    Some(w) if !w.contains(&i) => Value::Null,
                    _ => v.clone(),
                })
                .collect();
            out.push((*seq, cells));
        }
        drop(tables);
        out.sort_by_key(|(seq, _)| *seq);
        Some(out)
    }

    /// The rows whose `bounds` column falls in range, read straight out of the
    /// blocks: a `DELETE`/`UPDATE` needs the row ids, not the rows, and
    /// materialising 100k `Value` rows to find 5k of them was most of the
    /// statement.
    pub(crate) fn row_ids_in_bounds(
        &self,
        table: &str,
        bounds: &ColumnBounds,
    ) -> Option<Vec<usize>> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()?;
        let root = self.blocks_root();
        let read_ts = crate::read_ts();
        let mut out: Vec<usize> = Vec::new();
        for id in self.visible_block_ids(&table) {
            let Ok(block) = self.block_map(&table, &root, id) else {
                break;
            };
            let version = block.version();
            let row_count: usize = version
                .created
                .last()
                .map(|c| c.cumulative_row_count as usize)
                .unwrap_or(0);
            let created_at = Self::created_at(version, read_ts);
            let scan = Self::column_scan(&block, &meta, &types, bounds)?;
            for slot in 0..row_count {
                if !Self::slot_visible(version, slot, created_at, read_ts) || !scan.matches(slot) {
                    continue;
                }
                out.push(
                    ((id.segment_id as usize) << 32)
                        | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY + slot),
                );
            }
        }
        // Rows above the last checkpoint are buffered as `Value`s; the same
        // bounds are applied to them directly.
        let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
        let lo = bounds.lo.as_ref();
        let hi = bounds.hi.as_ref();
        for (seq, values) in self.pending_rows_of(&table) {
            if dead.contains(&(seq as u64)) {
                continue;
            }
            let Some(value) = values.get(bounds.pos) else {
                continue;
            };
            if matches!(value, Value::Null) {
                continue;
            }
            let low = lo.is_none_or(|lo| {
                matches!(
                    value.partial_cmp_value(lo),
                    Some(std::cmp::Ordering::Greater | std::cmp::Ordering::Equal)
                )
            });
            let high = hi.is_none_or(|hi| {
                matches!(
                    value.partial_cmp_value(hi),
                    Some(std::cmp::Ordering::Less | std::cmp::Ordering::Equal)
                )
            });
            if low && high {
                out.push(seq);
            }
        }
        out.sort_unstable();
        out.dedup();
        Some(out)
    }
    /// Every visible row of `table` for the columns at `wanted`, as typed
    /// column buffers.
    ///
    /// The caller decides what to do with them (filter, sort, project); this
    /// only decodes, in column order, skipping the rows the block's version and
    /// the tombstone set hide.
    /// The bounds check for one block, or `None` when the column cannot be
    /// read that way (var-length, a 16-bit float, no layout).
    pub(crate) fn column_scan<'a>(
        block: &'a MappedBlock,
        meta: &catalog::TableMeta,
        types: &[i32],
        bounds: &ColumnBounds,
    ) -> Option<ColumnScan<'a>> {
        let column_type = *types.get(bounds.pos)?;
        let column_id = meta.ids.get(bounds.pos).copied()?;
        let declared = meta.types.get(bounds.pos)?;
        let layout = layout_of(declared, column_type)?;
        let (body, _) = <MappedBlock as storage::format::BlockColumns>::column(block, column_id)?;
        let (data, nulls) =
            storage::definition::format::split_body_refs(body, DEFAULT_BLOCK_CAPACITY, layout);
        let number = |value: &Value| -> Option<Num> {
            match value {
                Value::Float(f) => Some(Num::F(*f)),
                Value::Int(i) => Some(Num::I(*i)),
                _ => None,
            }
        };
        Some(ColumnScan {
            data,
            nulls,
            layout,
            column_type,
            lo: match &bounds.lo {
                Some(value) => Some(number(value)?),
                None => None,
            },
            hi: match &bounds.hi {
                Some(value) => Some(number(value)?),
                None => None,
            },
        })
    }

    pub(crate) fn read_columns_batch(
        &self,
        table: &str,
        wanted: &[usize],
        bounds: Option<&ColumnBounds>,
    ) -> Option<Batch> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()?;
        let root = self.blocks_root();
        let read_ts = crate::read_ts();
        let blocks = self.visible_block_ids(&table);
        let mut seqs: Vec<usize> = Vec::new();
        let mut data: Vec<ColumnData> = wanted
            .iter()
            .map(|pos| column_buffer(&meta.types[*pos]))
            .collect();
        let mut valid: Vec<Vec<bool>> = vec![Vec::new(); wanted.len()];
        let mut out_of_order = false;
        let mut last = 0usize;
        let mut shown_slots: Vec<usize> = Vec::new();
        for id in &blocks {
            let Ok(block) = self.block_map(&table, &root, *id) else {
                break;
            };
            let version = block.version();
            let row_count: usize = version
                .created
                .last()
                .map(|c| c.cumulative_row_count as usize)
                .unwrap_or(0);
            let created_at = Self::created_at(version, read_ts);
            // A predicate on a fixed-width column is applied here, on the
            // mapped bytes: decoding every row into `Value`s and comparing
            // afterwards is what made a filtered scan cost more than an
            // unfiltered one.
            let scan = bounds.and_then(|bounds| Self::column_scan(&block, &meta, &types, bounds));
            // The slots this block shows, ascending: one pass over the
            // version decides them and every column decodes just those.
            shown_slots.clear();
            for slot in 0..row_count {
                if Self::slot_visible(version, slot, created_at, read_ts) {
                    if let Some(scan) = &scan {
                        if !scan.matches(slot) {
                            continue;
                        }
                    }
                    shown_slots.push(slot);
                    let seq = ((id.segment_id as usize) << 32)
                        | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY + slot);
                    if seq < last {
                        out_of_order = true;
                    }
                    last = seq;
                    seqs.push(seq);
                }
            }
            for (out, pos) in wanted.iter().enumerate() {
                Self::decode_block_column(
                    &meta.ids,
                    &meta.types,
                    &types,
                    block.as_ref(),
                    *pos,
                    &shown_slots,
                    &mut data[out],
                    &mut valid[out],
                )?;
            }
        }
        // Rows a commit appended since the last checkpoint live in the append
        // buffer; they are `Value`s, so they are appended as they are. The
        // buffer is walked in place: copying it first cost a full row copy per
        // scan (20k buffered rows made every query 20 ms slower).
        let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
        let view = self.append_view(&table, read_ts);
        let buffered_empty = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&self.table_key(&table))
                .map(|entry| entry.pending_rows.is_empty())
                .unwrap_or(true)
        };
        if !buffered_empty {
            let tables = self.tables.lock().unwrap();
            let entry = tables.get(&self.table_key(&table));
            for (seq, values) in entry.map(|e| e.pending_rows.as_slice()).unwrap_or(&[]) {
                let seq = *seq;
                if dead.contains(&(seq as u64)) {
                    continue;
                }
                if let Some(view) = &view {
                    if !view.covers(seq, read_ts) {
                        continue;
                    }
                }
                if seq < last {
                    out_of_order = true;
                }
                last = seq;
                seqs.push(seq);
                for (out, pos) in wanted.iter().enumerate() {
                    match values.get(*pos) {
                        Some(v) if !matches!(v, Value::Null) => {
                            data[out].push_value(v);
                            valid[out].push(true);
                        }
                        _ => {
                            data[out].push_value(&Value::Null);
                            valid[out].push(false);
                        }
                    }
                }
            }
        }
        if blocks.is_empty() && seqs.is_empty() {
            return None;
        }
        // A delete that has not reached the block's `version` yet lives in the
        // tombstone set; the row path filters those, and so must this one.
        if !dead.is_empty() && !seqs.is_empty() {
            let keep: Vec<bool> = seqs.iter().map(|s| !dead.contains(&(*s as u64))).collect();
            if keep.iter().any(|k| !k) {
                for k in 0..wanted.len() {
                    data[k].retain(&keep);
                    let mut at = 0usize;
                    valid[k].retain(|_| {
                        let v = keep.get(at).copied().unwrap_or(false);
                        at += 1;
                        v
                    });
                }
                seqs = seqs
                    .into_iter()
                    .zip(keep.iter())
                    .filter(|(_, k)| **k)
                    .map(|(s, _)| s)
                    .collect();
            }
        }
        let row_count = seqs.len();
        if out_of_order && row_count > 0 {
            let mut order: Vec<usize> = (0..row_count).collect();
            order.sort_by_key(|i| seqs[*i]);
            for column in &mut data {
                *column = permute_column(column, &order);
            }
            for v in &mut valid {
                *v = order.iter().map(|i| v[*i]).collect();
            }
        }
        Some(Batch {
            columns: wanted
                .iter()
                .map(|pos| meta.columns.get(*pos).cloned().unwrap_or_default())
                .collect(),
            data,
            valid,
            row_count,
        })
    }
    /// The rows at `rows` only, decoded in rowid order, as typed column
    /// buffers.
    ///
    /// The indexed-lookup counterpart of read_columns_batch: that one decodes
    /// every visible row of the table, which is right for a scan and wrong for a
    /// secondary-index lookup that already knows the few rows it matched. Each
    /// block named by a rowid is read and decoded once, and the decoder's
    /// visibility mask is narrowed to the requested slots, so the emitted cells
    /// line up one-for-one with the sorted run of rowids.
    pub(crate) fn read_columns_batch_for_rows(
        &self,
        table: &str,
        wanted: &[usize],
        rows: &[u64],
    ) -> Option<Batch> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()?;
        let root = self.blocks_root();
        let read_ts = crate::read_ts();
        let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
        // Only the buffered rows this read asks for are copied out of the
        // append buffer: an index lookup names a handful of ids, and cloning
        // the whole buffer for them made every lookup pay for the buffer's
        // size (20k rows = 20 ms per query).
        // Nothing is buffered for a table that was checkpointed, and building
        // the membership set anyway hashed every id of every lookup (11% of a
        // 1000-row indexed range query).
        let buffered: std::collections::HashMap<usize, Vec<Value>> = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&self.table_key(&table)) {
                Some(entry) if !entry.pending_rows.is_empty() => {
                    let requested: std::collections::HashSet<usize> =
                        rows.iter().map(|seq| *seq as usize).collect();
                    entry
                        .pending_rows
                        .iter()
                        .filter(|(seq, _)| requested.contains(seq))
                        .cloned()
                        .collect()
                }
                _ => Default::default(),
            }
        };
        let blocks = self.visible_block_ids(&table);

        let mut order: Vec<usize> = (0..rows.len()).collect();
        order.sort_by_key(|i| rows[*i]);

        let mut data: Vec<ColumnData> = wanted
            .iter()
            .map(|pos| column_buffer(&meta.types[*pos]))
            .collect();
        let mut valid: Vec<Vec<bool>> = vec![Vec::new(); wanted.len()];
        let mut count = 0usize;

        let mut at = 0usize;
        while at < order.len() {
            let seq = rows[order[at]] as usize;
            let segment_id = (seq >> 32) as u32;
            let offset = (seq as u64) & 0xffff_ffff;
            let block_index = (offset / DEFAULT_BLOCK_CAPACITY as u64) as u16;
            let mut end = at + 1;
            while end < order.len() {
                let s = rows[order[end]] as usize;
                let o = (s as u64) & 0xffff_ffff;
                if (s >> 32) as u32 != segment_id
                    || (o / DEFAULT_BLOCK_CAPACITY as u64) as u16 != block_index
                {
                    break;
                }
                end += 1;
            }
            let slots: Vec<usize> = (at..end)
                .map(|k| ((rows[order[k]] & 0xffff_ffff) % DEFAULT_BLOCK_CAPACITY as u64) as usize)
                .collect();
            let decoded = blocks
                .iter()
                .find(|b| b.segment_id == segment_id && b.block_id == block_index)
                .and_then(|id| {
                    let block = self.block_map(&table, &root, *id).ok()?;
                    let version = block.version();
                    let row_count: usize = version
                        .created
                        .last()
                        .map(|c| c.cumulative_row_count as usize)
                        .unwrap_or(0);
                    let created_at = Self::created_at(version, read_ts);
                    // Only the candidate slots this block holds, and only the
                    // ones its `version` still shows: the decoder walks that
                    // list, never the block's whole window.
                    let shown: Vec<usize> = slots
                        .iter()
                        .copied()
                        .filter(|slot| {
                            *slot < row_count
                                && Self::slot_visible(version, *slot, created_at, read_ts)
                        })
                        .collect();
                    let mut cols: Vec<ColumnData> = wanted
                        .iter()
                        .map(|pos| column_buffer(&meta.types[*pos]))
                        .collect();
                    let mut vals: Vec<Vec<bool>> = vec![Vec::new(); wanted.len()];
                    for (o, pos) in wanted.iter().enumerate() {
                        Self::decode_block_column(
                            &meta.ids,
                            &meta.types,
                            &types,
                            block.as_ref(),
                            *pos,
                            &shown,
                            &mut cols[o],
                            &mut vals[o],
                        )?;
                    }
                    Some((cols, vals, shown))
                });
            // The decoder emits only the *shown* slots of this run, in
            // ascending slot order, so the k-th shown slot is the k-th row of
            // its output. A hidden slot shortens the output and would shift
            // every later row, so each slot is mapped through the same
            // visibility the decoder used - not through its own position.
            let visible_run: Vec<Option<usize>> = match &decoded {
                Some((_, _, shown)) => {
                    let mut emitted = 0usize;
                    slots
                        .iter()
                        .map(|slot| {
                            // `shown` is ascending, so a pointer walk answers
                            // "was this slot emitted" in O(1) amortised.
                            if shown.get(emitted) == Some(slot) {
                                let at_emitted = emitted;
                                emitted += 1;
                                Some(at_emitted)
                            } else {
                                None
                            }
                        })
                        .collect()
                }
                None => vec![None; slots.len()],
            };
            for k in at..end {
                let s = rows[order[k]] as usize;
                let is_dead = dead.contains(&(s as u64));
                let run_index = visible_run[k - at];
                count += 1;
                for (o, pos) in wanted.iter().enumerate() {
                    let cell = if is_dead {
                        None
                    } else if let (Some((cols, vals, _)), Some(ri)) = (&decoded, run_index) {
                        if vals[o].get(ri).copied() == Some(false) {
                            Some(Value::Null)
                        } else {
                            Some(cols[o].value_at(ri))
                        }
                    } else {
                        // Either the block does not hold this slot at all (the
                        // row was written after the last checkpoint, so its
                        // slot is past the block's `row_count`), or the version
                        // hides it. The append buffer decides: a buffered row
                        // is the row, and only a genuinely absent one is NULL.
                        // Checking the buffer only when `decoded` was `None`
                        // made a row inserted after a checkpoint read as NULL
                        // through an indexed lookup, because the block for its
                        // slot existed but did not cover it yet.
                        buffered.get(&s).and_then(|v| v.get(*pos).cloned())
                    };
                    match cell {
                        Some(v) if !matches!(v, Value::Null) => {
                            data[o].push_value(&v);
                            valid[o].push(true);
                        }
                        _ => {
                            data[o].push_value(&Value::Null);
                            valid[o].push(false);
                        }
                    }
                }
            }
            at = end;
        }

        Some(Batch {
            columns: wanted
                .iter()
                .map(|pos| meta.columns.get(*pos).cloned().unwrap_or_default())
                .collect(),
            data,
            valid,
            row_count: count,
        })
    }
    /// Appends one column of a decoded block to `out`, skipping the rows the
    /// version hides.
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn decode_block_column(
        ids: &[u64],
        type_text: &[String],
        types: &[i32],
        columns: &dyn BlockColumns,
        pos: usize,
        // The block's slots to emit, ascending: every visible slot for a scan,
        // the index's candidates for a lookup. Walking them costs the kept
        // rows, not the block's whole window.
        slots: &[usize],
        out: &mut ColumnData,
        valid: &mut Vec<bool>,
    ) -> Option<()> {
        let column_type = types.get(pos)?;
        let column_id = ids.get(pos).copied().unwrap_or(pos as u64);
        let (raw_body, raw_outline) = columns.column(column_id)?;
        let layout = layout_of(&type_text[pos], *column_type)?;
        let body = storage::definition::format::split_body_for_layout(
            raw_body,
            DEFAULT_BLOCK_CAPACITY,
            layout,
        );
        let kind = outline_kind(&type_text[pos]);
        let shape = embedding_shape(&type_text[pos]);
        for row in slots.iter().copied() {
            let null = body.null_bitmap.len() > row / 8
                && body.null_bitmap[row / 8] & (1 << (row % 8)) == 0;
            if null {
                out.push_value(&Value::Null);
                valid.push(false);
                continue;
            }
            // A text column is written straight into its buffer: the cell never
            // becomes a `Value` and, for varchar, never a `String` either.
            if matches!(out, ColumnData::Text(_)) {
                if kind != OutlineKind::None {
                    return None;
                }
                let ColumnData::Text(text) = out else {
                    unreachable!()
                };
                let ok = decode_text_cell(
                    &type_text[pos],
                    *column_type,
                    layout,
                    &body.data,
                    row,
                    raw_outline,
                    text,
                )?;
                valid.push(ok);
                continue;
            }
            let value = match kind {
                OutlineKind::None => match layout {
                    ColumnLayout::CompactBit => body
                        .data
                        .get(row / 8)
                        .map(|b| Value::Bool(b & (1 << (row % 8)) != 0))
                        .unwrap_or(Value::Null),
                    ColumnLayout::Fixed(width) => {
                        match decode_scalar_value(*column_type, width, &body.data, row) {
                            Some(v) => v,
                            None => decode_value_shaped(
                                &type_text[pos],
                                *column_type,
                                &body.data,
                                row,
                                raw_outline,
                                shape,
                            )
                            .map(parse_owned_value)
                            .unwrap_or(Value::Null),
                        }
                    }
                    ColumnLayout::VarLen(_) => decoded_text_value(
                        decode_value_shaped(
                            &type_text[pos],
                            *column_type,
                            &body.data,
                            row,
                            raw_outline,
                            shape,
                        ),
                        *column_type,
                    ),
                },
                _ => return None,
            };
            if let Value::Str(text) = value {
                out.push_text(text);
                valid.push(true);
            } else {
                valid.push(!matches!(value, Value::Null));
                out.push_value(&value);
            }
        }
        Some(())
    }
    /// Every row of the table as the engine works with it: the block files'
    /// cells (decoded by `decode_rows_of_values`) plus the append buffer's,
    /// which are `Value`s already.
    pub(crate) fn read_blocks_values(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
        prune: &[crate::fast_rough_filter::PruneCond],
        keep: RowFilter<'_>,
        rows_wanted: Option<&std::collections::HashSet<usize>>,
    ) -> Result<BlockValueRows> {
        self.read_blocks_values_at(table, wanted, prune, keep, rows_wanted, crate::read_ts())
    }

    /// [`Self::read_blocks_values`] with the statement's read timestamp passed
    /// **by value**.
    ///
    /// The read timestamp lives in a thread-local, which is correct only while
    /// the statement stays on its own thread. Decoding now runs on the blocking
    /// pool, where that thread-local does not exist and would read
    /// `u64::MAX` - i.e. every row would look visible, including rows deleted
    /// after the statement began. Taking the timestamp before the dispatch and
    /// passing it in keeps the snapshot semantics intact.
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn read_blocks_values_at(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
        prune: &[crate::fast_rough_filter::PruneCond],
        keep: RowFilter<'_>,
        rows_wanted: Option<&std::collections::HashSet<usize>>,
        read_ts: u64,
    ) -> Result<BlockValueRows> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block reader"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();

        let mut rows: Vec<(usize, Vec<Value>)> = Vec::new();
        // The mirror is segmented (`seg_{id}/blk_{n}`), and a row's id carries
        // its segment in the high 32 bits, so the block map - not a hardcoded
        // `seg_0` - decides what a row's id is. Reading only `seg_0` here made
        // a compacted table (whose rows live in the next segment id) come back
        // at the wrong ids, and the restore then wrote a second copy of every
        // row.
        // A compaction leaves the source segments' directories in place (the
        // reference keeps them until `Cleanup`), so the block index still lists
        // them. A reader must skip them, exactly as the reference skips a
        // segment whose `drop|seg|` ts is at or below its `begin_ts`.
        let blocks: Vec<BlockId> = self.visible_block_ids(&table);
        // A table whose first commit has not reached a checkpoint yet has no
        // block file at all: its rows are in the append buffer, and that is
        // still a readable table.
        let buffered = self.pending_rows_of(&table);
        if blocks.is_empty() && buffered.is_empty() {
            return Err(Error::NotFound(format!("no blocks for table '{table}'")));
        }
        // Resolve the conditions once against the schema; a column the record
        // does not cover (vector/sparse/tensor/bool) keeps its condition and
        // `may_contain` answers `true` for it.
        let resolved: Vec<(usize, crate::fast_rough_filter::PruneOp, expression::Value)> = prune
            .iter()
            .filter_map(|cond| {
                meta.columns
                    .iter()
                    .position(|c| c.eq_ignore_ascii_case(&cond.column))
                    .map(|i| (i, cond.op, cond.value.clone()))
            })
            .collect();
        // The blocks that survive pruning and the index's row filter, in scan
        // order. Selecting them is serial (each test is a catalog read), the
        // decoding that follows is not (plan 6.13.2).
        let mut candidates: Vec<(usize, BlockId)> = Vec::new();
        for (at, id) in blocks.into_iter().enumerate() {
            let BlockId {
                segment_id,
                block_id: block_index,
                ..
            } = id;
            if !resolved.is_empty() {
                // Skip a block the prefilter rules out without reading a single
                // column file.
                let key = format!(
                    "blk|{}|{}|{}|{}|fast_rough_filter",
                    db_id, meta.id, segment_id, block_index
                );
                if let Ok(Some(record)) = self.storage.catalog.store().get(key.as_bytes()) {
                    if let Some(filters) = crate::fast_rough_filter::Filters::decode(&record) {
                        if !filters.may_contain_all(&resolved) {
                            #[cfg(test)]
                            PRUNED_BLOCKS.with(|n| n.set(n.get() + 1));
                            continue;
                        }
                    }
                }
            }
            // An index answered: a block that holds none of the wanted rows is
            // never read. Skipping the row *decode* (below) still paid for the
            // block read, its version file and its column bodies - on a 20k-row
            // table a ten-row result touched twenty blocks.
            if let Some(rows) = rows_wanted {
                let any = rows.iter().any(|seq| {
                    ((*seq >> 32) as u32) == id.segment_id
                        && (*seq as u64 & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64
                            == u64::from(id.block_id)
                });
                if !any {
                    continue;
                }
            }
            candidates.push((at, id));
        }
        let decode = |id: BlockId, keep: RowFilter<'_>| {
            self.decode_block_visible(
                &table,
                &root,
                &meta,
                &types,
                id,
                wanted,
                keep,
                rows_wanted,
                read_ts,
            )
        };
        // A predicate is `&dyn Fn`, which is not `Sync`, so the parallel path
        // only exists when there is none. It is also the common case: a scan
        // without a pushed-down equality decodes plain (plan 6.13.2).
        let decoded: Vec<BlockDecode> = match keep {
            None if candidates.len() > 1
                && self.engine_handle().is_some()
                && !crate::fanout::on_a_runtime_worker() =>
            {
                let workers = std::thread::available_parallelism()
                    .map(|n| n.get())
                    .unwrap_or(1)
                    .min(candidates.len());
                // A block decode is the pure-CPU leaf: it reads mapped bytes
                // and builds `Value`s. Each block is a tokio task under the
                // core bound, and only the decode itself runs on the blocking
                // pool.
                //
                // The body needs its inputs owned (a task outlives this frame):
                // the table name, the block root, the schema and the row filter
                // are cloned once for the whole scan, not per block.
                let engine = self.engine_handle().expect("checked above");
                let table_owned = table.clone();
                let root_owned = root.clone();
                let meta_owned = meta.clone();
                let types_owned = types.clone();
                let wanted_owned = wanted.map(|w| w.to_vec());
                let rows_owned = rows_wanted.cloned();
                let blocks: Vec<(usize, BlockId)> = candidates.clone();
                crate::fanout::map_blocking(blocks, workers, move |(at, id)| {
                    let value = engine.decode_block_visible(
                        &table_owned,
                        &root_owned,
                        &meta_owned,
                        &types_owned,
                        id,
                        wanted_owned.as_deref(),
                        None,
                        rows_owned.as_ref(),
                        read_ts,
                    );
                    Ok((at, value))
                })
                .map(|decoded| decoded.into_iter().map(|(_, value)| value).collect())
                .unwrap_or_else(|_| candidates.iter().map(|(_, id)| decode(*id, None)).collect())
            }
            _ => candidates.iter().map(|(_, id)| decode(*id, keep)).collect(),
        };
        for ((at, _id), decoded) in candidates.into_iter().zip(decoded) {
            match decoded {
                BlockDecode::Rows(cells) => rows.extend(cells),
                // The first block that cannot be opened is "no blocks for this
                // table"; one that opened but did not decode, or any failure
                // after the first block, ends the scan - exactly the split the
                // sequential walk made.
                BlockDecode::OpenFailed if at == 0 => {
                    return Err(Error::NotFound(format!("no blocks for table '{table}'")))
                }
                BlockDecode::OpenFailed | BlockDecode::DecodeFailed => break,
            }
        }
        // Rows a commit appended since the last checkpoint are in the table's
        // append buffer, not in a file yet: the scan reads them over the blocks
        // (the reference's `BufferObject` merge), tombstoned ones excluded.
        let pending = buffered;
        if !pending.is_empty() {
            let have: std::collections::HashSet<usize> = rows.iter().map(|(seq, _)| *seq).collect();
            // One snapshot of the append log and the deletes for the whole
            // buffer, not one per row.
            let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
            let view = self.append_view(&table, read_ts);
            for (seq, values) in pending {
                if have.contains(&seq) || dead.contains(&(seq as u64)) {
                    continue;
                }
                if let Some(view) = &view {
                    if !view.covers(seq, read_ts) {
                        continue;
                    }
                }
                let cells: Vec<Value> = values
                    .iter()
                    .enumerate()
                    .map(|(i, v)| match wanted {
                        Some(w) if !w.contains(&i) => Value::Null,
                        _ => v.clone(),
                    })
                    .collect();
                if keep.is_some_and(|keep| !keep(&cells)) {
                    continue;
                }
                rows.push((seq, cells));
            }
            rows.sort_by_key(|(seq, _)| *seq);
        }
        Ok((meta.columns.clone(), rows))
    }

    /// [`Self::read_blocks_values`] as the canonical cell text, which is what
    /// the index builders and the block-mirror dump compare against.
    pub(crate) fn read_blocks_wanted(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<BlockRows> {
        let (columns, rows) = self.read_blocks_values(table, wanted, prune, None, None)?;
        Ok((
            columns,
            rows.into_iter()
                .map(|(seq, cells)| (seq, cells.iter().map(mirror_cell_text).collect()))
                .collect(),
        ))
    }

    /// The C++ index id of `index_name` within `table`: the id the catalog
    /// handed out at `CREATE INDEX` (`IndexMeta::id`, a per-table counter
    /// starting at 1).
    ///
    /// This **must** be the catalog's id and not the index's position in some
    /// list: the catalog publishes the id in `catalog|idx|{db}|{tbl}|{name}|{ts}`
    /// and the reference reads the chunk of an index from `idx_{id}/seg_0/`, so
    /// a mirror that numbered the indexes differently wrote one index's chunk
    /// into another's directory (the HNSW chunk landed under the sparse
    /// index) and the reference then read the sparse chunk with the HNSW
    /// parser and aborted.
    pub(crate) fn index_position(&self, table: &str, index_name: &str) -> Result<usize> {
        let id = self
            .storage
            .catalog
            .index_by_name(table, index_name)?
            .map(|im| im.id)
            .filter(|id| *id > 0)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}' on '{table}'")))?;
        Ok(id as usize)
    }

    /// The vectors of `table`'s `column` as `(row id, vector)`, optionally
    /// restricted to one segment.
    ///
    /// The label is the **in-segment** offset (`seq as u32`): the reference's
    /// per-segment index is handed `SegmentOffset`s, and a chunk under
    /// `idx_{id}/seg_{s}/` is read back with the segment prepended.
    pub(crate) fn column_vectors(
        &self,
        table: &str,
        column: &str,
        segment: Option<u32>,
    ) -> Result<VectorRows> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}' on '{table}'")))?;
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            if let Some(only) = segment {
                if (seq >> 32) as u32 != only {
                    continue;
                }
            }
            if let Some(Value::Str(s)) = row.get(col_idx) {
                out.push((
                    seq as u32,
                    crate::parse_vec_str(s).iter().map(|x| *x as f32).collect(),
                ));
            }
        }
        Ok(out)
    }

    /// `dense_index_building_worker`: the reference's dense-index pool size,
    /// `hardware_concurrency() / 2` clamped to at least 2 (`config_impl.cpp:516-518`).
    pub(crate) fn hnsw_build_workers(&self) -> usize {
        std::thread::available_parallelism()
            .map(|count| count.get() / 2)
            .unwrap_or(1)
            .max(2)
    }

    /// The cells of `column` in every row the block mirror holds, tombstoned
    /// slots included - the reference's `IndexBuilder` reads the column, not the
    /// table, and decodes only that column's body.
    pub(crate) fn read_block_column_including_deleted(
        &self,
        table: &str,
        column: usize,
    ) -> Vec<(usize, String)> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Vec::new();
        };
        let root = self.blocks_root();
        let wanted = [column];
        let mut out = Vec::new();
        for id in self.visible_block_ids(&table) {
            if let Some(cells) =
                self.decode_block_cells(&table, &root, &meta, &types, id, Some(&wanted))
            {
                for (seq, mut cells) in cells {
                    let text = if column < cells.len() {
                        std::mem::take(&mut cells[column])
                    } else {
                        String::new()
                    };
                    out.push((seq, text));
                }
            }
        }
        for (seq, values) in self.pending_rows_of(&table) {
            if let Some(v) = values.get(column) {
                out.push((seq, mirror_cell_text(v)));
            }
        }
        out.sort_by_key(|(seq, _)| *seq);
        out
    }

    /// Builds a C++ HNSW `chunk_0.idx` body for `column`'s vectors (task #70).
    ///
    /// The graph is built by the engine's own `HnswIndex` and serialized with the
    /// C++ layout (§9.2): 56-byte header, plain vector region, graph region, then
    /// the `u32` label array. Byte-identity with a C++-built index is not
    /// required — the offsets are explicit, so the C++ loader reads it.
    pub(crate) fn hnsw_chunk(
        &self,
        table: &str,
        column: &str,
        options: &HnswOptions,
        segment: Option<u32>,
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.column_vectors(table, column, segment)?;
        if vectors.is_empty() {
            return Err(Error::Invalid(format!(
                "no vectors to index in '{table}.{column}'"
            )));
        }
        let dim = vectors[0].1.len();
        let mut index = HnswIndex::with_encoding(
            dim,
            options.m,
            options.ef_construction,
            options.ef_construction.max(64),
            options.metric,
            options.encoding,
        );
        // Train LVQ/LS mean/RaBitQ centroid and the LSG averages on the whole
        // column, then build the graph with the trained store.
        let rows: Vec<Vec<f32>> = vectors.iter().map(|(_, v)| v.clone()).collect();
        index.train_store(&rows, options.lsg);
        let labels: Vec<u32> = vectors.iter().map(|(label, _)| *label).collect();
        // Bucket-parallel construction, as the reference does on its dense-index
        // pool; a store that is not plain keeps the serial path.
        index.add_batch(&rows, self.hnsw_build_workers());
        Ok(encode_hnsw_chunk_from(&index, &labels))
    }

    /// The HNSW options of `index_name` (`metric`, `encode`, `build_type`,
    /// `lsg_config`).
    pub(crate) fn hnsw_options(&self, table: &str, index_name: &str) -> HnswOptions {
        let Some(im) = self
            .storage
            .catalog
            .index_by_name(table, index_name)
            .ok()
            .flatten()
        else {
            return HnswOptions::default();
        };
        let opt = |name: &str| {
            im.options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.clone())
        };
        let metric = index::vector::Metric::parse(&opt("metric").unwrap_or_else(|| "l2".into()));
        let encoding = index::vector::HnswEncoding::parse(&opt("encode").unwrap_or_default());
        let lsg = if opt("build_type")
            .map(|v| v.eq_ignore_ascii_case("lsg"))
            .unwrap_or(false)
        {
            Some(
                opt("lsg_config")
                    .and_then(|v| index::lsg::LsgConfig::parse(&v))
                    .unwrap_or_default(),
            )
        } else {
            None
        };
        HnswOptions {
            m: opt("M").and_then(|v| v.parse().ok()).unwrap_or(16),
            ef_construction: opt("EF_CONSTRUCTION")
                .and_then(|v| v.parse().ok())
                .unwrap_or(200),
            metric,
            encoding,
            lsg,
        }
    }

    /// Writes `table`'s index as a C++ HNSW chunk file under
    /// `{data_dir}/data/db_{db}/tbl_{tbl}/idx_{id}/seg_0/chunk_{n}.idx`,
    /// returning how many vectors were written (task #70).
    ///
    /// A chunk is only written when the mirror has none yet: re-writing it on
    /// every checkpoint would not match the reference, which appends a chunk per
    /// memory-index dump.
    pub fn write_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_hnsw_chunk(table, index_name, false, false)
            .map(|written| written.0)
    }

    /// Appends a fresh chunk, as the reference's memory-index dump does. Rows
    /// added since the previous dump end up in the new chunk.
    pub fn dump_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_hnsw_chunk(table, index_name, true, false)
            .map(|written| written.0)
    }

    /// The optimize merge: rebuilds the graph over every live row and appends
    /// it as the newest chunk, which then supersedes the incremental ones.
    ///
    /// Returns the segments it wrote a chunk for - the entries an optimize
    /// transaction records (`OptimizeIndexTxnStore::entries_`).
    pub(crate) fn merge_hnsw_index(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<Vec<ChunkRewrite>> {
        self.write_hnsw_chunk(table, index_name, false, true)
            .map(|written| written.1)
    }

    pub(crate) fn write_hnsw_chunk(
        &self,
        table: &str,
        index_name: &str,
        new_chunk: bool,
        force_append: bool,
    ) -> Result<(usize, Vec<ChunkRewrite>)> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .index_by_name(&table, index_name)?
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        // A MULTIVECTOR holds several sub-vectors per row; the C++ mirror is a
        // single-vector graph, so there is nothing to write for it.
        if let Some(i) = meta.columns.iter().position(|c| c == &column) {
            if meta
                .types
                .get(i)
                .map(|t| t.trim().to_ascii_uppercase().starts_with("MULTIVECTOR"))
                .unwrap_or(false)
            {
                return Err(Error::Invalid(format!(
                    "no single-vector HNSW mirror for multivector column '{column}'"
                )));
            }
        }
        // A checkpoint dumps the **memory segment** (rows since the previous
        // dump), not a graph rebuilt from every row: rebuilding cost O(table)
        // memory and time on every checkpoint and was the Gate C OOM driver.
        // The full rebuild remains the fallback when materialising the mirror
        // explicitly (`new_chunk == false`), e.g. after a restart.
        //
        // With more than one segment the buffered graph cannot be split - it
        // was built over every row - so a dump falls back to a per-segment
        // rebuild, which the reference's query side treats the same way (a
        // segment's chunks are merged at search time).
        let tkey = self.table_key(&table);
        let segments = self.segment_row_counts(&table);
        let segments = if segments.is_empty() {
            vec![(0, 0)]
        } else {
            segments
        };
        let from_memory = if new_chunk && segments.len() == 1 {
            let Some(mem) = self
                .tables
                .lock()
                .unwrap()
                .get(&tkey)
                .and_then(|e| e.hnsw_mem.get(&(index_id as u64)).cloned())
            else {
                // Nothing buffered since the last dump: a checkpoint has
                // nothing to write and must not rebuild the whole graph.
                return Ok((0, Vec::new()));
            };
            let Ok(mem) = mem.lock() else {
                return Ok((0, Vec::new()));
            };
            if mem.is_empty() {
                return Ok((0, Vec::new()));
            }
            let labels: Vec<u32> = mem.rows().iter().map(|s| *s as u32).collect();
            Some(encode_hnsw_chunk_from(mem.index(), &labels))
        } else {
            None
        };
        let db_id = self.database_id_of(&table);
        let options = self.hnsw_options(&table, index_name);
        let mut written = 0usize;
        let mut rewrites: Vec<ChunkRewrite> = Vec::new();
        for (segment, rows) in &segments {
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            // The chunk family is the catalog's (`GetChunkIDs1`), not the
            // directory's: a file whose record `Cleanup` removed must not raise
            // the next id, and an id a record already names must never be
            // handed out again (`GetAndSetNextChunkID` only moves forward).
            let set = self.index_segment_set(&table);
            let catalog_chunks: Vec<(u64, std::path::PathBuf)> = self
                .catalog_index_files(&table, index_id as u64)
                .into_iter()
                .filter(|(seg, _, _)| *seg == *segment)
                .map(|(_, id, path)| (id, path))
                .collect();
            let chunks: Vec<(u64, std::path::PathBuf)> = if catalog_chunks.is_empty() {
                // A C++ directory this port has not published: the files are
                // the only record, and adopting them keeps their ids.
                hnsw_chunks(&dir)
            } else {
                catalog_chunks
            };
            // A checkpoint only mirrors an index that has no chunk yet. Check
            // that before building: the build decodes every row and graph,
            // which on a long import is the dominant transient (Gate C RSS)
            // and is discarded here anyway.
            if !new_chunk && !force_append && !chunks.is_empty() {
                continue;
            }
            let built = match &from_memory {
                Some((count, chunk)) => Some((*count, chunk.clone())),
                None => {
                    if *rows == 0 {
                        None
                    } else {
                        self.hnsw_chunk(&table, &column, &options, Some(*segment))
                            .ok()
                    }
                }
            };
            let Some((count, chunk)) = built else {
                continue;
            };
            // `GetAndSetNextChunkID`: the counter the catalog keeps, raised past
            // every record this segment already names. Only ever forward, so an
            // id a merge or `Cleanup` retired never comes back.
            let chunk_id = self.next_index_chunk_id(&set, &index_id.to_string(), *segment);
            let name = format!("chunk_{chunk_id}.idx");
            storage::definition::format::write_synced(&dir.join(name), &chunk).map_err(|e| {
                Error::Io {
                    path: dir.clone(),
                    source: e,
                }
            })?;
            // `AddNewChunkIndex1`: the reader enumerates this record, not the
            // directory, so the dump publishes the chunk it just wrote.
            self.record_chunk(
                &set,
                index_id as u64,
                chunk_id,
                *segment,
                count as u64,
                chunk.len() as u64,
            );
            written += count;
            // The chunks this one supersedes: the reference's
            // `deprecate_chunks_`, which its optimize transaction removes from
            // the segment's chunk list.
            rewrites.push(ChunkRewrite {
                segment: *segment,
                chunk_id,
                row_count: count as u32,
                deprecate_ids: chunks.iter().map(|(id, _)| *id as u32).collect(),
            });
        }
        Ok((written, rewrites))
    }

    /// Whether `column` is a `MULTIVECTOR`, whose mirror has no single-vector
    /// graph in this build.
    pub(crate) fn multivector_column(&self, table: &str, column: &str) -> bool {
        let Ok(meta) = self
            .storage
            .catalog
            .get_table_by_name(&self.qualified_table(table))
        else {
            return false;
        };
        let Some(meta) = meta else {
            return false;
        };
        meta.columns
            .iter()
            .position(|c| c == column)
            .and_then(|i| meta.types.get(i))
            .is_some_and(|t| t.trim().to_ascii_uppercase().starts_with("MULTIVECTOR"))
    }

    /// Optimizes every vector mirror: HNSW chunks are merged when the newest
    /// one does not cover all live rows and the merge is affordable, PLAID
    /// chunks are folded into one by [`Self::merge_plaid_index`] (the
    /// reference's `OptimizeIndexInner` `kPLAID` case) as soon as a second
    /// chunk exists. A mirror that is already merged is left untouched, so
    /// repeated optimize ticks are no-ops.
    ///
    /// `OptimizationProcessor::NewNotifyOptimize`: **one** transaction for the
    /// whole pass, opened before the pass looks at any index and committed even
    /// when it merged nothing (`OptimizeAllIndexes` -> `CommitTxn`; an empty
    /// `OptimizeIndexTxnStore` still spends its commit ts and writes an entry
    /// without commands). The store records one entry per index segment the
    /// pass rewrote, so two merges never run together and a merge meets the
    /// appends and updates on its table. A failed pass is rolled back; a lost
    /// conflict is logged and left alone, which is what the processor does.
    pub async fn optimize_index_chunks(&self) -> Result<()> {
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        let mut txn = self.txn.begin();
        let mut entries: Vec<OptimizeEntry> = Vec::new();
        let outcome = self.merge_optimizable_indexes(&mut entries);
        // `OptimizeIndexTxnStore::ToWalEntry`: one `WalCmdDumpIndexV2` per
        // entry, carrying the merged chunk and the chunks it supersedes. The
        // reference's `dump_cause_` is not part of the frame. A failed pass never
        // gets here with its entries: it is rolled back.
        let commands = match &outcome {
            Ok(()) => self.optimize_index_commands(&entries)?,
            Err(_) => Vec::new(),
        };
        // The deprecations the commit has to publish: `RemoveChunkIDs` takes the
        // superseded chunks out of the segment's chunk list and one
        // `drop|idx_chunk|…` marker per id stays behind for `Cleanup`. The
        // entries are moved into the transaction, so keep the list here.
        let deprecations: Vec<(String, String, u32, Vec<u32>)> = entries
            .iter()
            .map(|entry| {
                (
                    entry.table.clone(),
                    entry.index.clone(),
                    entry.segment,
                    entry.deprecate_ids.clone(),
                )
            })
            .collect();
        txn.set_op(TxnOp::OptimizeIndex { entries });
        if !commands.is_empty() {
            txn.set_typed_commands(commands);
        }
        match outcome {
            Ok(()) => match self.txn.commit_at(txn, ts).await {
                Ok(_) => {
                    // `RemoveChunkIDs(deprecate_ids)` + `PrepareCommitDumpIndex`
                    // of the reference: the merged chunk is what the segment holds
                    // from now on, so the chunks it replaced leave the chunk list,
                    // but their bytes stay on disk until `Cleanup` deletes them.
                    for (table, index, segment, ids) in &deprecations {
                        // The merge is already committed, so a failed
                        // deprecation must not turn the pass into an error:
                        // the superseded chunks stay readable (a retry would
                        // merge them again), which is noisier than losing
                        // them but never loses a row.
                        if let Err(error) = self.deprecate_chunks(table, index, *segment, ids, ts) {
                            eprintln!(
                                "optimize: deprecating the chunks {index} superseded failed: {error}"
                            );
                        }
                    }
                    // The decoded graphs of the superseded chunks are no longer
                    // part of the index; reconcile the in-memory set now instead
                    // of at the next checkpoint.
                    self.reload_vector_indexes();
                    // The full-text half of `OptimizeIndexInner`
                    // (`new_txn_index_impl.cpp:349-353` → `OptimizeFtIndex`).
                    // A full-text chunk is only its three files — the chunk list
                    // *is* the directory — so this rewrite carries no catalog
                    // entry and therefore runs after the vector commit: a failed
                    // merge leaves the source chunks readable instead of rolling
                    // back the vector work.
                    if let Err(error) = self.optimize_all_fulltext_indexes_async().await {
                        eprintln!("optimize: merging the full-text chunks failed: {error}");
                    }
                    // The secondary half of `OptimizeIndexInner`
                    // (`new_txn_index_impl.cpp:336-340`): a segment with more than
                    // one chunk folds them into one. Like the full-text rewrite its
                    // catalog records are derived from the files, so it also runs
                    // after the vector commit.
                    if let Err(error) = self.optimize_all_secondary_indexes().await {
                        eprintln!("optimize: merging the secondary chunks failed: {error}");
                    }
                    Ok(())
                }
                // A concurrent merge (or a drop that took the index away) is a
                // conflict; `NewNotifyOptimize` logs it and leaves the chunks as
                // they are.
                Err(Error::TxnConflict(_)) | Err(Error::TxnConflictNoRetry(_)) => Ok(()),
                Err(error) => Err(error),
            },
            Err(error) => {
                self.txn.rollback(txn)?;
                Err(error)
            }
        }
    }

    /// `OptimizeIndexTxnStore::ToWalEntry`: the command per entry.
    pub(crate) fn optimize_index_commands(&self, entries: &[OptimizeEntry]) -> Result<Vec<WalCmd>> {
        let mut out = Vec::with_capacity(entries.len());
        for entry in entries {
            let table = entry.table.as_str();
            let Some(meta) = self.storage.catalog.get_table_by_name(table)? else {
                continue;
            };
            let (db_name, table_name) = crate::split_table_name(table);
            out.push(WalCmd::DumpIndex {
                db_name,
                db_id: self.database_id_of(table).to_string(),
                table_name,
                table_id: meta.id.to_string(),
                index_name: entry.index.clone(),
                index_id: self.index_position(table, &entry.index)?.to_string(),
                segment_id: entry.segment,
                chunk_infos: entry.chunk_infos.clone(),
                deprecate_ids: entry.deprecate_ids.clone(),
                table_key: self.table_key(table),
            });
        }
        Ok(out)
    }

    /// The merge half of an optimize pass, recording what it rewrote.
    pub(crate) fn merge_optimizable_indexes(&self, entries: &mut Vec<OptimizeEntry>) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                if let Some(rewrite) = self.merge_plaid_index(&im.table, &im.name)? {
                    entries.push(self.optimize_entry(&im.table, &im.name, &rewrite));
                }
                continue;
            }
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            // A MULTIVECTOR column has no single-vector graph in this build, so
            // its mirror has nothing to merge (`write_hnsw_chunk` refuses
            // it); skipping it keeps one such index from failing the pass.
            if self.multivector_column(&im.table, &im.column) {
                continue;
            }
            // The rows an optimize would merge are the *live* ones: a table
            // whose writes have not been checkpointed (or whose rows were just
            // replayed from the WAL) keeps them in the append buffer, and the
            // block files hold none of them yet.
            let Ok(rows) = self.row_count(&im.table) else {
                continue;
            };
            let rows = rows + self.buffered_row_count(&im.table);
            let Some(dir) = self.hnsw_dir(&im.table, &im.name) else {
                continue;
            };
            let chunks = self.live_chunk_files(&im.table, &im.name, 0, &dir);
            if chunks.len() < 2 {
                continue;
            }
            let covered = self
                .hnsw_chunk_path(&im.table, &im.name)
                .ok()
                .and_then(|p| storage::chunk_store::read_file(p).ok())
                .and_then(|b| {
                    knn_format::decode_hnsw_chunk(
                        &b,
                        4,
                        knn_format::VERTEX_L0_SIZE,
                        knn_format::VERTEX_LX_SIZE,
                    )
                    .ok()
                })
                .map(|c| c.header.cur_vec_num as usize)
                .unwrap_or(0);
            if covered >= rows {
                continue;
            }
            // A merge holds the vectors, the rebuilt graph and the encoded
            // chunk at the same time. On a multi-gigabyte table that transient
            // is larger than the whole memory budget (C++ streams it through
            // the buffer manager), so leave the incremental chunks alone
            // rather than rebuilding the table every tick.
            let dim = self
                .table_schema(&im.table)
                .ok()
                .and_then(|s| s.iter().position(|c| c == &im.column))
                .and_then(|ci| {
                    self.column_types(&im.table)
                        .ok()
                        .and_then(|t| t.get(ci).cloned())
                })
                .and_then(|t| crate::type_dim(&t))
                .unwrap_or(0);
            let rebuild_bytes = rows.saturating_mul(dim).saturating_mul(4 * 3 + 64);
            if rebuild_bytes > 1024 * 1024 * 1024 {
                continue;
            }
            for rewrite in self.merge_hnsw_index(&im.table, &im.name)? {
                entries.push(self.optimize_entry(&im.table, &im.name, &rewrite));
            }
        }
        Ok(())
    }

    /// The `OptimizeIndexStoreEntry` for one rewritten chunk: the reference's
    /// `new_chunk_infos_` (one entry, from `ChunkIndexMetaInfo`) and
    /// `deprecate_chunks_`.
    pub(crate) fn optimize_entry(
        &self,
        table: &str,
        index_name: &str,
        rewrite: &ChunkRewrite,
    ) -> OptimizeEntry {
        OptimizeEntry {
            table: self.qualified_table(table),
            index: index_name.to_string(),
            segment: rewrite.segment,
            chunk_infos: vec![storage::wal_entry::WalChunkIndexInfo {
                chunk_id: rewrite.chunk_id,
                base_name: String::new(),
                base_row_id: storage::wal_entry::WalRowId::new(rewrite.segment, 0),
                row_count: rewrite.row_count,
                term_count: 0,
                index_size: 0,
                deprecate_ts: u64::MAX,
            }],
            deprecate_ids: rewrite.deprecate_ids.clone(),
        }
    }

    /// Drops `idx_{id}/seg_{s}` directories whose segment no longer exists.
    ///
    /// Compaction merges the source segments into a new segment id and the
    /// reference deletes their index chunks with them; a `seg_{s}` directory
    /// left behind still holds a full copy of the index and would be read back
    /// as a second one.
    pub(crate) fn prune_dead_segment_index_dirs(&self) -> Result<()> {
        for meta in self.storage.catalog.list_tables()? {
            let db_id = self.database_id_of(&meta.name);
            let tbl_dir = self
                .blocks_root()
                .join(format!("db_{db_id}/tbl_{}", meta.id));
            // The block index is what a reader enumerates, and it survives a
            // restart; the allocator's map does not (`segment_ids` is empty for
            // a directory this process merely opened), and an empty set here
            // deletes every live chunk directory.
            let mut live_segments: Vec<u32> = self
                .visible_block_ids(&meta.name)
                .into_iter()
                .map(|id| id.segment_id)
                .collect();
            if live_segments.is_empty() {
                live_segments = self.segment_ids(&meta.name);
            }
            live_segments.sort_unstable();
            live_segments.dedup();
            let live: Vec<String> = live_segments
                .into_iter()
                .map(|segment| format!("seg_{segment}"))
                .collect();
            let Ok(entries) = std::fs::read_dir(&tbl_dir) else {
                continue;
            };
            for entry in entries.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                if !name.starts_with("idx_") {
                    continue;
                }
                let Ok(inner) = std::fs::read_dir(entry.path()) else {
                    continue;
                };
                for sub in inner.flatten() {
                    let seg = sub.file_name().to_string_lossy().to_string();
                    if seg.starts_with("seg_") && !live.contains(&seg) {
                        storage::chunk_store::remove_tree(sub.path());
                    }
                }
            }
            // The full-text chunks live in their own directory, named after the
            // row id they start at (`fulltext/<table>.<column>/ft_{row:016x}*`).
            // A compaction renumbers the rows it takes, so the chunks of the
            // segments it deprecated name ids nothing owns; the loader skips
            // them, and this is what finally removes them - the reference does
            // the same in `Cleanup` (`CleanChunkIndex`), which is why its reader
            // can be built from the catalog alone.
            let fulltext_root = self.dir.join("fulltext");
            let own_prefix = format!("{}.", meta.name.replace('/', "_"));
            if let Ok(entries) = std::fs::read_dir(&fulltext_root) {
                for entry in entries.flatten() {
                    let index_dir = entry.path();
                    if !index_dir.is_dir() {
                        continue;
                    }
                    // `fulltext/<db>.<table>.<column>`: only this table's.
                    let dir_name = index_dir
                        .file_name()
                        .map(|n| n.to_string_lossy().to_string())
                        .unwrap_or_default();
                    if !dir_name.starts_with(&own_prefix) {
                        continue;
                    }
                    let Ok(files) = std::fs::read_dir(&index_dir) else {
                        continue;
                    };
                    for file in files.flatten() {
                        let name = file.file_name().to_string_lossy().to_string();
                        let Some(hex) = name
                            .strip_prefix("ft_")
                            .and_then(|rest| rest.split('.').next())
                            .and_then(|rest| rest.split('_').next())
                        else {
                            continue;
                        };
                        let Ok(row) = u64::from_str_radix(hex, 16) else {
                            continue;
                        };
                        if !live.contains(&format!("seg_{}", row >> 32)) {
                            let _ = storage::chunk_store::remove_file(file.path());
                        }
                    }
                }
            }
        }
        Ok(())
    }

    /// Reclaims the chunk bytes an optimize deprecated, and - as a safety net
    /// for a directory whose deprecation marker is missing - the chunks a
    /// merged chunk plainly supersedes.
    ///
    /// The reference splits the two halves: the optimize commit takes the
    /// superseded chunks out of the segment's chunk list and leaves a
    /// `drop|idx_chunk|…` marker, and `Cleanup` deletes the files those markers
    /// name (`CleanChunkIndex` -> `ChunkIndexMeta::UninitSet`). This is that
    /// second half.
    pub fn cleanup_index_chunks(&self) -> Result<()> {
        self.prune_dead_segment_index_dirs()?;
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                let Ok(mirror) = self.plaid_mirror(&im.table, &im.name) else {
                    continue;
                };
                self.reclaim_deprecated_chunks(&im.table, &im.name, 0, &mirror.dir)?;
                // The safety net: a merge that ran before the markers existed
                // leaves a chunk whose record is gone but whose bytes are not.
                // PLAID chunks carry their own document count in the header, so
                // "the newest chunk holds every document of the older ones" is
                // readable without decoding.
                let chunks = self.live_chunk_files(&im.table, &im.name, 0, &mirror.dir);
                let Some(((_, newest), older)) = chunks.split_last() else {
                    continue;
                };
                let Some(newest_docs) = plaid_chunk_doc_num(newest) else {
                    continue;
                };
                let superseded: u32 = older
                    .iter()
                    .filter_map(|(_, path)| plaid_chunk_doc_num(path))
                    .sum();
                if newest_docs < superseded {
                    continue;
                }
                for (_, path) in older {
                    let _ = storage::chunk_store::remove_file(path);
                }
                continue;
            }
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            let Ok(rows) = self.row_count(&im.table) else {
                continue;
            };
            let Some(dir) = self.hnsw_dir(&im.table, &im.name) else {
                continue;
            };
            self.reclaim_deprecated_chunks(&im.table, &im.name, 0, &dir)?;
            // The catalog's list, so a chunk a merge already took out of it is
            // not read back here.
            let chunks = self.live_chunk_files(&im.table, &im.name, 0, &dir);
            let covered = self
                .hnsw_chunk_path(&im.table, &im.name)
                .ok()
                .and_then(|p| storage::chunk_store::read_file(p).ok())
                .and_then(|b| {
                    knn_format::decode_hnsw_chunk(
                        &b,
                        4,
                        knn_format::VERTEX_L0_SIZE,
                        knn_format::VERTEX_LX_SIZE,
                    )
                    .ok()
                })
                .map(|c| c.header.cur_vec_num as usize)
                .unwrap_or(0);
            // The newest chunk covers the table only when it is a merged
            // chunk; incremental dumps cover just their own rows. A committed
            // optimize already dropped the chunks it superseded (the entry's
            // `deprecate_ids`), so this is the fallback for a crash between that
            // commit and the deletion.
            if covered < rows {
                continue;
            }
            if let Some((keep, _)) = chunks.last() {
                for (id, path) in &chunks {
                    if id != keep {
                        let _ = storage::chunk_store::remove_file(path);
                    }
                }
            }
        }
        Ok(())
    }

    /// The directory holding `table`'s HNSW mirror, if the table exists.
    pub(crate) fn hnsw_dir(&self, table: &str, index_name: &str) -> Option<PathBuf> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let index_id = self.index_position(&table, index_name).ok()?;
        let db_id = self.database_id_of(&table);
        Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id)),
        )
    }

    /// The chunk ids of one segment index that an optimize deprecated and
    /// `cleanup_index_chunks` has not reclaimed yet: the reference's
    /// `drop|idx_chunk|…` markers (`PrepareCommitDumpIndex`,
    /// `new_txn_index_impl.cpp:3043`).
    pub(crate) fn deprecated_chunk_ids(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
    ) -> std::collections::HashSet<u64> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Default::default();
        };
        let Ok(index_id) = self.index_position(&table, index_name) else {
            return Default::default();
        };
        let db_id = self.database_id_of(&table);
        catalog::layout::deprecated_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
        )
        .map(|ids| ids.into_iter().collect())
        .unwrap_or_default()
    }

    /// The chunks of one segment that are still part of the index, from the
    /// catalog's chunk list (`GetChunkIDs1`).
    ///
    /// The reference never enumerates a directory for this: `ColumnIndexReader`
    /// walks `SegmentIndexIDs -> ChunkIDs`, and an optimize's `RemoveChunkIDs`
    /// makes a superseded chunk unreachable the moment it leaves the list, even
    /// though its bytes are still on disk until `Cleanup`. Reading the
    /// directory instead would hand the merged chunk's siblings back to a
    /// query, and would raise the next id off a file no record names any more.
    pub(crate) fn live_chunk_files(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        _dir: &Path,
    ) -> Vec<(u64, PathBuf)> {
        let table = self.qualified_table(table);
        let Ok(index_id) = self.index_position(&table, index_name) else {
            return Vec::new();
        };
        self.catalog_index_files(&table, index_id as u64)
            .into_iter()
            .filter(|(seg, _, _)| *seg == segment)
            .map(|(_, id, path)| (id, path))
            .collect()
    }

    /// `SegmentIndexMeta::RemoveChunkIDs` + `PrepareCommitDumpIndex`: the
    /// chunks leave the segment's chunk list and one `drop|idx_chunk|…` marker
    /// is written per id, so `Cleanup` can delete the bytes later. Nothing on
    /// disk is touched here - that is the reference's split, and it is what
    /// keeps the chunk files readable until `cleanup_index_chunks` runs.
    pub(crate) fn deprecate_chunks(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        chunk_ids: &[u32],
        ts: u64,
    ) -> Result<()> {
        if chunk_ids.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            // A `DROP` took the table while the optimize transaction was
            // running: its chunk records went with it.
            return Ok(());
        };
        let index_id = self.index_position(&table, index_name)?;
        let db_id = self.database_id_of(&table);
        catalog::layout::deprecate_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
            chunk_ids,
            ts,
        )?;
        Ok(())
    }

    /// `Cleanup`'s half of a deprecation: delete the bytes of every chunk a
    /// `drop|idx_chunk|…` marker names, then drop the marker together with the
    /// chunk's record. A crash in between leaves the marker, so the next
    /// cleanup retries; the other order would make a leftover file look live
    /// again.
    pub(crate) fn reclaim_deprecated_chunks(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        dir: &Path,
    ) -> Result<()> {
        let deprecated = self.deprecated_chunk_ids(table, index_name, segment);
        if deprecated.is_empty() {
            return Ok(());
        }
        let mut gone = Vec::new();
        for id in deprecated {
            let path = dir.join(format!("chunk_{id}.idx"));
            // Always ask the store, never `path.exists()`: on an object profile
            // the bytes live in an object and the logical path may be absent
            // while the chunk is still there. The id joins `gone` - and its
            // deprecation marker is cleared - only when the store reports the
            // bytes as removed (a missing local file alone is that success).
            if storage::chunk_store::remove_file(&path).is_ok() {
                gone.push(id);
            }
        }
        if gone.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            return Ok(());
        };
        let Ok(index_id) = self.index_position(&table, index_name) else {
            return Ok(());
        };
        let db_id = self.database_id_of(&table);
        catalog::layout::reclaim_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
            &gone,
        )?;
        Ok(())
    }

    /// The newest chunk file of `table`'s HNSW mirror.
    pub(crate) fn hnsw_chunk_path(&self, table: &str, index_name: &str) -> Result<PathBuf> {
        let dir = self
            .hnsw_dir(table, index_name)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        self.live_chunk_files(table, index_name, 0, &dir)
            .pop()
            .map(|(_, p)| p)
            .ok_or_else(|| Error::NotFound(format!("no HNSW mirror at {}", dir.display())))
    }

    /// Writes `table`'s full-text index as the C++ `.dic`/`.pos`/`.len` chunk
    /// family under `{data_dir}/data/db_{db}/tbl_{tbl}/idx_{id}/seg_0/`,
    /// returning the number of documents indexed.
    pub fn write_fulltext_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let im = self
            .storage
            .catalog
            .index_by_name(&table, index_name)?
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let col = meta
            .columns
            .iter()
            .position(|c| c == &im.column)
            .ok_or_else(|| Error::NotFound(format!("column '{}'", im.column)))?;
        let index_id = self.index_position(&table, index_name)?;
        let db_id = self.database_id_of(&table);
        let tokenizer = self.index_tokenizer(&table, &im.column);
        // The reference's chunk keeps a deleted row: an UPDATE's old row stays
        // in the chunk it was written to and only the block's `version` hides
        // it, so the corpus the chunk indexes is every row ever appended.
        let rows = self.fetch_rows_including_deleted(&table)?;
        // The reference seals one chunk per **mem-index dump**, covering the
        // rows appended since the previous one and split at block boundaries;
        // the files are named `ft_{:016x}` after the chunk's first row id
        // (`new_txn_index_impl.cpp:860`). A 20000-row insert therefore ends up
        // as `ft_0000000000000000`, `ft_0000000000002000`,
        // `ft_0000000000004000`, and a second run that appended 110 rows to a
        // 100-row table as a single `ft_0000000000000064` - not as a rewrite
        // of the first chunk.
        //
        // The index is **per segment**: `MemoryIndexer` is constructed with
        // `segment_index_meta.GetSegmentIndexDir()` and a base row id whose
        // high bits are the segment, so an import into a second segment writes
        // `idx_{id}/seg_1/ft_0000000100000000.*` rather than extending the
        // first segment's chunk.
        let mut by_segment: std::collections::BTreeMap<u32, Vec<&(usize, Vec<Value>)>> =
            Default::default();
        for row in rows.iter() {
            by_segment
                .entry((row.0 as u64 >> 32) as u32)
                .or_default()
                .push(row);
        }
        let mut written = 0usize;
        for (segment, seg_rows) in by_segment {
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            // How many documents the chunks already on disk cover: a dump only
            // indexes the rows past them. A chunk that claims more rows than
            // the segment has (the table shrank) is dropped instead. The file
            // name carries the full `RowID`, so the count has to be taken from
            // the in-segment offset.
            let mut base = 0usize;
            let mut stale = Vec::new();
            if let Ok(entries) = std::fs::read_dir(&dir) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    let name = path.file_name().and_then(|n| n.to_str()).unwrap_or("");
                    if !name.starts_with("ft_") {
                        continue;
                    }
                    stale.push(path.clone());
                    let Some(hex) = name
                        .strip_prefix("ft_")
                        .and_then(|n| n.strip_suffix(".len"))
                    else {
                        continue;
                    };
                    let Ok(start) = u64::from_str_radix(hex, 16) else {
                        continue;
                    };
                    let docs = std::fs::metadata(&path)
                        .map(|m| m.len() / 4)
                        .unwrap_or_default();
                    base = base.max(((start & 0xffff_ffff) + docs) as usize);
                }
            }
            if base > seg_rows.len() {
                for path in &stale {
                    let _ = storage::chunk_store::remove_file(path);
                }
                base = 0;
            }
            // The rows of `segment` are cut into chunks the way the reference
            // cuts them:
            //
            // * an **imported** extent is dumped once per `mem_index_capacity`
            //   (`PopulateFtIndexInner` checks the size after each block), so an
            //   import smaller than the capacity is a single chunk spanning
            //   every block of the segment;
            // * rows appended later (or a segment no `COPY` wrote) are dumped
            //   per **physical block** (`offset / 8192`): the memory indexer is
            //   handed one block at a time, and a block keeps its `base_row_id`
            //   even when rows in it were deleted.
            let mut ranges: Vec<Vec<&(usize, Vec<Value>)>> = Vec::new();
            let imported = self.imported_extent(&table, segment, seg_rows.len() as u64);
            let tail_start = match imported {
                Some(head) if base < head as usize => {
                    let capacity = self.memindex_capacity().max(1);
                    let head = head as usize;
                    let mut start = base;
                    let mut at = base;
                    let mut accumulated = 0usize;
                    while at < head {
                        let take = (head - at).min(DEFAULT_BLOCK_CAPACITY);
                        at += take;
                        accumulated += take;
                        if accumulated >= capacity {
                            ranges.push(seg_rows[start..at].to_vec());
                            start = at;
                            accumulated = 0;
                        }
                    }
                    if at > start {
                        ranges.push(seg_rows[start..at].to_vec());
                    }
                    at
                }
                _ => base,
            };
            let mut by_block: std::collections::BTreeMap<u64, Vec<&(usize, Vec<Value>)>> =
                Default::default();
            for row in seg_rows.iter().skip(tail_start) {
                by_block
                    .entry((row.0 as u64 & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64)
                    .or_default()
                    .push(row);
            }
            ranges.extend(by_block.into_values());
            for block_rows in ranges {
                // The chunk is named after the row it starts at, which is the
                // block's first row only when the dump began on a block
                // boundary. The name carries the whole `RowID` - segment in
                // the high 32 bits - which is what the reference's
                // `MemoryIndexer` uses as its base name.
                let base_row_id = block_rows[0].0 as u64;
                // term -> (document ordinal, the term's position in the
                // document), in term order (the FST requires sorted keys). The
                // document ordinal restarts at zero in every chunk: the
                // chunk's `base_row_id` is what puts it back on the segment's
                // row ids (`MemoryIndexer::doc_count_` counts rows within the
                // chunk).
                let mut postings: std::collections::BTreeMap<String, Vec<(u32, u32)>> =
                    std::collections::BTreeMap::new();
                let mut lengths: Vec<u32> = Vec::with_capacity(block_rows.len());
                for (doc, (_, row)) in block_rows.iter().enumerate() {
                    let text = row.get(col).map(|v| v.to_slt_string()).unwrap_or_default();
                    let terms = crate::tokenize_index_positions(&text, &tokenizer);
                    lengths.push(terms.len() as u32);
                    for (t, position) in terms {
                        postings.entry(t).or_default().push((doc as u32, position));
                    }
                }

                let (dic, pos, len) = encode_fulltext_chunk(&postings, &lengths);
                let base = storage::definition::format::fulltext_base_name(base_row_id, None);
                for (name, bytes) in [
                    (format!("{base}.dic"), &dic),
                    (format!("{base}.pos"), &pos),
                    (format!("{base}.len"), &len),
                ] {
                    let path = dir.join(&name);
                    storage::definition::format::write_synced(&path, bytes)
                        .map_err(|e| Error::Io { path, source: e })?;
                }
                written += block_rows.len();
            }
        }
        Ok(written)
    }

    /// Reads back a mirror written by [`Self::write_hnsw_index`], returning
    /// `(row id, vector)` pairs **without** consulting the index metadata.
    pub fn read_hnsw_index(&self, table: &str, index_name: &str) -> Result<VectorRows> {
        let table = self.qualified_table(table);
        let path = self.hnsw_chunk_path(&table, index_name)?;
        let bytes = storage::chunk_store::read_file(&path)
            .map_err(|_| Error::NotFound(format!("no HNSW mirror at {}", path.display())))?;
        let parts = self
            .read_hnsw_chunk_parts(&table, index_name, &bytes)
            .ok_or_else(|| Error::Invalid("bad HNSW chunk".to_string()))?;
        let index = HnswIndex::from_chunk_parts(
            parts.dims,
            parts.m.max(1) as usize,
            (parts.ef as usize).max(64),
            parts.options.metric,
            parts.options.encoding,
            &parts.store_meta,
            &parts.records,
            parts.n,
            Vec::new(),
            Vec::new(),
            None,
            0,
        )
        .ok_or_else(|| Error::Invalid("bad HNSW store".to_string()))?;
        Ok((0..parts.n)
            .zip(parts.labels)
            .map(|(i, label)| (label, index.vector(i)))
            .collect())
    }

    /// The decoded pieces of an HNSW chunk: header, one row id per vector (in
    /// node order), the vectors themselves, and the graph.
    #[allow(clippy::type_complexity)]
    /// Decodes one HNSW chunk with the encoding/metric recorded in the
    /// catalog. The store meta length and the record size follow from those
    /// options and the dimension stored in the meta.
    pub(crate) fn read_hnsw_chunk_parts(
        &self,
        table: &str,
        index_name: &str,
        bytes: &[u8],
    ) -> Option<HnswChunkParts> {
        let options = self.hnsw_options(table, index_name);
        if bytes.len() < 32 {
            return None;
        }
        let u64_at = |at: usize| -> Option<u64> {
            bytes
                .get(at..at + 8)
                .and_then(|b| b.try_into().ok())
                .map(u64::from_le_bytes)
        };
        let m = u64_at(0)?;
        let ef = u64_at(8)?;
        let n = u64_at(16)? as usize;
        let dims = u64_at(24)? as usize;
        let meta_len = hnsw_store_meta_len(dims, options.metric, options.encoding);
        let record_size = hnsw_record_size(dims, options.encoding);
        let chunk = knn_format::decode_hnsw_chunk_ext(
            bytes,
            meta_len,
            record_size,
            knn_format::VERTEX_L0_SIZE,
            knn_format::VERTEX_LX_SIZE,
        )
        .ok()?;
        let vertices = knn_format::decode_graph_region(
            &chunk.graph,
            chunk.cur_vec_num as usize,
            chunk.mmax0,
            chunk.mmax,
        )
        .ok()?;
        Some(HnswChunkParts {
            m,
            ef,
            n,
            dims,
            max_layer: chunk.max_layer.max(0) as usize,
            enterpoint: (chunk.enterpoint >= 0).then_some(chunk.enterpoint as usize),
            store_meta: chunk.store_meta,
            records: chunk.vectors,
            vertices,
            labels: chunk.labels,
            options,
        })
    }

    pub(crate) fn assemble_loaded_index(
        parts: HnswChunkParts,
        chunk_id: u64,
        stamp: (u64, u64),
    ) -> Option<LoadedVectorIndex> {
        if parts.n == 0 {
            return None;
        }
        let levels: Vec<usize> = parts
            .vertices
            .iter()
            .map(|v| v.layer_count.max(0) as usize)
            .collect();
        let mut links: Vec<Vec<Vec<usize>>> = vec![Vec::new()];
        for (node, v) in parts.vertices.iter().enumerate() {
            while links[0].len() <= node {
                links[0].push(Vec::new());
            }
            links[0][node] = v.neighbors_l0.iter().map(|&x| x as usize).collect();
            for (l, nbrs) in v.upper.iter().enumerate() {
                let layer = l + 1;
                while links.len() <= layer {
                    links.push(Vec::new());
                }
                while links[layer].len() <= node {
                    links[layer].push(Vec::new());
                }
                links[layer][node] = nbrs.iter().map(|&x| x as usize).collect();
            }
        }
        let entry = parts.enterpoint.map(|e| e.min(parts.n.saturating_sub(1)));
        let graph_bytes: usize = links
            .iter()
            .flat_map(|layer| layer.iter())
            .map(|neighbors| neighbors.len() * 4 + 24)
            .sum();
        let bytes = parts.records.len() + parts.labels.len() * 4 + levels.len() * 8 + graph_bytes;
        let index = HnswIndex::from_chunk_parts(
            parts.dims,
            parts.m.max(1) as usize,
            (parts.ef as usize).max(64),
            parts.options.metric,
            parts.options.encoding,
            &parts.store_meta,
            &parts.records,
            parts.n,
            levels,
            links,
            entry,
            parts.max_layer,
        )?;
        Some(LoadedVectorIndex {
            index: HnswHandler::from_index(index),
            labels: parts.labels,
            chunk_id,
            bytes,
            stamp,
        })
    }

    /// Loads `index_name`'s persisted HNSW graph into an in-memory index, so
    /// `SEARCH MATCH VECTOR` can walk the graph that is on disk (plan #70).
    pub fn load_vector_index(&self, table: &str, index_name: &str) -> Result<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let path = self.hnsw_chunk_path(&table, index_name)?;
        let bytes = storage::chunk_store::read_file(&path)
            .map_err(|_| Error::NotFound(format!("no HNSW mirror at {}", path.display())))?;
        let parts = self
            .read_hnsw_chunk_parts(&table, index_name, &bytes)
            .ok_or_else(|| Error::Invalid("bad HNSW chunk".to_string()))?;
        Self::assemble_loaded_index(parts, 0, file_stamp(&path))
            .ok_or_else(|| Error::Invalid("HNSW mirror has no vectors".to_string()))
    }

    /// Every sealed HNSW chunk of `index_name`, oldest first. The reference
    /// keeps one graph per segment (`chunk_{n}.idx`); a query searches each and
    /// merges. Missing/undecodable chunks are skipped so one bad segment cannot
    /// disable the index.
    pub fn load_vector_indexes(&self, table: &str, index_name: &str) -> Vec<LoadedVectorIndex> {
        self.load_vector_indexes_after(table, index_name, None)
    }

    /// Loads only the chunks written after `after` (all when `None`), so a
    /// checkpoint reload does not re-decode graphs it already holds in memory.
    pub fn load_vector_indexes_after(
        &self,
        table: &str,
        index_name: &str,
        after: Option<u64>,
    ) -> Vec<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let Some(dir) = self.hnsw_dir(&table, index_name) else {
            return Vec::new();
        };
        let mut out = Vec::new();
        for (chunk_id, path) in self.live_chunk_files(&table, index_name, 0, &dir) {
            if after.is_some_and(|a| chunk_id <= a) {
                continue;
            }
            let Ok(bytes) = storage::chunk_store::read_file(&path) else {
                continue;
            };
            let Some(parts) = self.read_hnsw_chunk_parts(&table, index_name, &bytes) else {
                continue;
            };
            if let Some(loaded) = Self::assemble_loaded_index(parts, chunk_id, file_stamp(&path)) {
                out.push(loaded);
            }
        }
        out
    }

    /// The `chunk_{id}.idx` ids currently on disk for `index_name`. A merged
    /// chunk replaces the chunks cleanup deleted, so the in-memory set must be
    /// reconciled with this list (the reference's buffer manager only holds
    /// live index buffers).
    pub fn hnsw_chunk_ids(&self, table: &str, index_name: &str) -> Vec<u64> {
        let table = self.qualified_table(table);
        let Some(dir) = self.hnsw_dir(&table, index_name) else {
            return Vec::new();
        };
        self.live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .map(|(id, _)| id)
            .collect()
    }

    /// Decodes one on-disk chunk by id.
    pub fn load_vector_index_chunk(
        &self,
        table: &str,
        index_name: &str,
        chunk_id: u64,
    ) -> Option<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let dir = self.hnsw_dir(&table, index_name)?;
        let path = self
            .live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .find(|(id, _)| *id == chunk_id)
            .map(|(_, path)| path)?;
        let bytes = storage::chunk_store::read_file(&path).ok()?;
        let parts = self.read_hnsw_chunk_parts(&table, index_name, &bytes)?;
        Self::assemble_loaded_index(parts, chunk_id, file_stamp(&path))
    }

    /// `chunk_id -> (file size, mtime seconds)` for the live chunks.
    pub fn hnsw_chunk_stamps(
        &self,
        table: &str,
        index_name: &str,
    ) -> std::collections::HashMap<u64, (u64, u64)> {
        let table = self.qualified_table(table);
        let Some(dir) = self.hnsw_dir(&table, index_name) else {
            return Default::default();
        };
        self.live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .map(|(id, path)| (id, file_stamp(&path)))
            .collect()
    }

    /// Builds a C++ IVF `chunk_0.idx` body (plain storage, `kPlain`) for
    /// `column`'s vectors: a deterministic k-means partitions them into
    /// `centroid_count` parts (task #70).
    pub(crate) fn ivf_chunk(
        &self,
        table: &str,
        column: &str,
        options: &IvfOptions,
        segment: Option<u32>,
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.column_vectors(table, column, segment)?;
        if vectors.is_empty() {
            return Err(Error::Invalid(format!(
                "no vectors to index in '{table}.{column}'"
            )));
        }
        let dim = vectors[0].1.len();
        // C++ `IVF_Index_Data::Train`: `min(sqrt(n) * ratio, n / min_points)`,
        // raised to 1 when that underflows (`ivf_index_data_impl.cpp:195-201`).
        let k = ((vectors.len() as f64).sqrt() * options.ratio as f64)
            .min((vectors.len() / options.min_points.max(1)) as f64) as usize;
        let k = k.clamp(1, vectors.len());
        // Deterministic seeding: pick k evenly spaced vectors.
        let mut centroids: Vec<Vec<f32>> = (0..k)
            .map(|c| vectors[c * vectors.len() / k].1.clone())
            .collect();
        let mut assign = vec![0usize; vectors.len()];
        for _ in 0..10 {
            let mut changed = false;
            for (i, (_, v)) in vectors.iter().enumerate() {
                let mut best = 0;
                let mut best_d = f32::INFINITY;
                for (c, cent) in centroids.iter().enumerate() {
                    let d = index::vector::l2_sq(v, cent);
                    if d < best_d {
                        best_d = d;
                        best = c;
                    }
                }
                if assign[i] != best {
                    assign[i] = best;
                    changed = true;
                }
            }
            // Recompute each centroid as the mean of its members.
            let mut sums = vec![vec![0.0f64; dim]; k];
            let mut counts = vec![0usize; k];
            for (i, (_, v)) in vectors.iter().enumerate() {
                counts[assign[i]] += 1;
                for (d, x) in v.iter().enumerate() {
                    sums[assign[i]][d] += *x as f64;
                }
            }
            for c in 0..k {
                if counts[c] > 0 {
                    centroids[c] = sums[c]
                        .iter()
                        .map(|s| (*s / counts[c] as f64) as f32)
                        .collect();
                }
            }
            if !changed {
                break;
            }
        }

        let header = knn_format::IvfChunkHeader {
            row_count: vectors.len() as u32,
            embedding_count: vectors.len() as u32,
            dimension: dim as u32,
            centroid_count: k as u32,
        };
        if options.storage.eq_ignore_ascii_case("scalar_quantization")
            || options.storage.eq_ignore_ascii_case("product_quantization")
        {
            let flat: Vec<f32> = centroids.iter().flatten().copied().collect();
            let chunk = if options.storage.eq_ignore_ascii_case("scalar_quantization") {
                Self::encode_ivf_scalar_quantized(
                    &header,
                    &flat,
                    &vectors,
                    &assign,
                    options.sq_bits,
                )?
            } else {
                Self::encode_ivf_product_quantized(&header, &flat, &vectors, &assign, options)?
            };
            return Ok((vectors.len(), chunk));
        }
        let mut parts = vec![knn_format::IvfPlainPart::default(); k];
        for (i, (label, v)) in vectors.iter().enumerate() {
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            for x in v {
                part.vectors.extend_from_slice(&x.to_le_bytes());
            }
        }
        let flat: Vec<f32> = centroids.into_iter().flatten().collect();
        let chunk = knn_format::encode_ivf_plain_chunk(&header, &flat, &parts)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))?;
        Ok((vectors.len(), chunk))
    }

    /// Scalar-quantized chunk (`IVF_Part_Storage_SQ`): train the shared
    /// `(a, b)` affine parameters on the residuals
    /// (`IVF_Parts_Storage_Info<kScalarQuantization>`), then store each
    /// vector's codes plus its `dot_v_e` (`AppendOneEmbedding`).
    pub(crate) fn encode_ivf_scalar_quantized(
        header: &knn_format::IvfChunkHeader,
        centroids: &[f32],
        vectors: &[(u32, Vec<f32>)],
        assign: &[usize],
        sq_bits: u8,
    ) -> Result<Vec<u8>> {
        let dim = header.dimension as usize;
        let residuals: Vec<Vec<f32>> = vectors
            .iter()
            .zip(assign)
            .map(|((_, v), part)| {
                let at = part * dim;
                v.iter()
                    .zip(&centroids[at..at + dim])
                    .map(|(x, c)| x - c)
                    .collect()
            })
            .collect();
        let (a, b) = index::ivf::train_scalar_quantization(&residuals, dim, sq_bits);
        let mut parts: Vec<knn_format::IvfSqPart> = (0..header.centroid_count)
            .map(|_| knn_format::IvfSqPart {
                segment_offsets: Vec::new(),
                codes: Vec::new(),
                dot_v_e: Vec::new(),
            })
            .collect();
        for (i, (label, v)) in vectors.iter().enumerate() {
            let (codes, dot_v_e) =
                index::ivf::encode_scalar_quantized(v, &residuals[i], &a, &b, sq_bits);
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            part.codes.extend_from_slice(&codes);
            part.dot_v_e.push(dot_v_e);
        }
        knn_format::encode_ivf_sq_chunk(header, centroids, &a, &b, &parts, sq_bits)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))
    }

    /// Product-quantized chunk (`IVF_Part_Storage_PQ`): train one codebook per
    /// subspace on the residuals, store one code per subspace and the
    /// `-0.5 * ||c||^2` weights the search formulas need.
    pub(crate) fn encode_ivf_product_quantized(
        header: &knn_format::IvfChunkHeader,
        centroids: &[f32],
        vectors: &[(u32, Vec<f32>)],
        assign: &[usize],
        options: &IvfOptions,
    ) -> Result<Vec<u8>> {
        let dim = header.dimension as usize;
        let residuals: Vec<Vec<f32>> = vectors
            .iter()
            .zip(assign)
            .map(|((_, v), part)| {
                let at = part * dim;
                v.iter()
                    .zip(&centroids[at..at + dim])
                    .map(|(x, c)| x - c)
                    .collect()
            })
            .collect();
        let (real, codewords, norms) = index::ivf::train_product_quantization(
            &residuals,
            dim,
            options.pq_subspace_num,
            options.pq_bits,
        );
        let word_size = if options.pq_bits > 12 { 2 } else { 1 };
        let mut parts: Vec<knn_format::IvfPqPart> = (0..header.centroid_count)
            .map(|_| knn_format::IvfPqPart {
                segment_offsets: Vec::new(),
                last_code_id: 0,
                codes: Vec::new(),
                word_size,
            })
            .collect();
        for (i, (label, _)) in vectors.iter().enumerate() {
            let codes = index::ivf::encode_product_quantized(
                &residuals[i],
                dim,
                options.pq_subspace_num,
                real,
                &codewords,
            );
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            index::ivf::pq_append_codes(
                &mut part.codes,
                part.last_code_id as usize,
                &codes,
                options.pq_bits,
            );
            part.last_code_id += options.pq_subspace_num as u64;
        }
        let codebook = knn_format::IvfPqCodebook {
            real_subspace_centroid_num: real as u32,
            codewords,
            subspace_weights: norms,
        };
        knn_format::encode_ivf_pq_chunk(header, centroids, &codebook, &parts)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))
    }

    /// Decodes one product-quantized chunk into a searchable segment.
    pub(crate) fn decode_ivf_pq_chunk(
        bytes: &[u8],
        options: &IvfOptions,
    ) -> Option<index::ivf::IvfSegment> {
        if bytes.len() < 20 {
            return None;
        }
        let u32_at = |at: usize| u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
        let dim = u32_at(8) as usize;
        let centroid_count = u32_at(12) as usize;
        if dim == 0 || centroid_count == 0 {
            return None;
        }
        let real = u32_at(16 + centroid_count * dim * 4) as usize;
        let word_size = if options.pq_bits > 12 { 2 } else { 1 };
        let chunk = knn_format::decode_ivf_pq_chunk(
            bytes,
            word_size,
            dim * real,
            real * options.pq_subspace_num,
        )
        .ok()?;
        let parts = chunk
            .parts
            .into_iter()
            .map(|part| index::ivf::IvfPqPartData {
                offsets: part.segment_offsets,
                codes: part.codes,
            })
            .collect();
        Some(index::ivf::IvfSegment::Product(index::ivf::IvfPqIndex {
            dims: dim,
            centroids: chunk.centroids,
            centroid_count,
            subspace_num: options.pq_subspace_num,
            real_centroid_num: chunk.codebook.real_subspace_centroid_num as usize,
            codewords: chunk.codebook.codewords,
            norms_neg_half: chunk.codebook.subspace_weights,
            bits: options.pq_bits,
            parts,
        }))
    }

    /// Writes `table`'s index as a C++ IVF chunk file (`kPlain`) under
    /// `…/idx_{id}/seg_0/chunk_0.idx`, returning how many vectors were written.
    pub fn write_ivf_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .index_by_name(&table, index_name)?
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let options = self.ivf_storage_option(&table, index_name);
        // One chunk per segment: the reference trains and dumps per segment
        // (`PopulateIvfIndexInner` is handed a `SegmentMeta`), and the chunk's
        // offsets are in-segment (`RowID(segment, offset)`).
        let db_id = self.database_id_of(&table);
        let mut segments = self.segment_row_counts(&table);
        if segments.is_empty() {
            segments.push((0, 0));
        }
        let mut written = 0usize;
        let set = self.index_segment_set(&table);
        for (segment, rows) in segments {
            if rows == 0 {
                continue;
            }
            // A checkpoint mirrors an index whose chunks already cover the
            // segment: the chunk is the whole segment's vectors, so rewriting it
            // under a fresh id would append a duplicate to the chunk list every
            // tick. "A chunk exists" is not the same as "the segment is
            // covered" - an insert after the first dump leaves vectors the
            // existing chunk does not hold, and `seal_index_tail` clears
            // `ivf_mem` right after this returns, so skipping the dump lost
            // them.
            // Each IVF dump writes the segment's **whole** vector set, so the
            // chunks are successive snapshots of it and their row counts must
            // not be summed (100 rows, then 200, sums to 300). The newest
            // snapshot is the coverage.
            let covered: u64 = catalog::layout::read_index_chunks(
                self.storage.catalog.store().as_ref(),
                &db_id.to_string(),
                &meta.id.to_string(),
                &index_id.to_string(),
            )
            .unwrap_or_default()
            .into_iter()
            .find(|(id, _)| *id == segment)
            .and_then(|(_, chunks)| chunks.into_iter().map(|(_, info)| info.row_count).max())
            .unwrap_or(0);
            if covered >= rows {
                continue;
            }
            let (count, chunk) = self.ivf_chunk(&table, &column, &options, Some(segment))?;
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            // `GetAndSetNextChunkID`: the counter, never a literal `chunk_0`.
            let chunk_id = self.next_index_chunk_id(&set, &index_id.to_string(), segment);
            let name = format!("chunk_{chunk_id}.idx");
            storage::definition::format::write_synced(&dir.join(&name), &chunk).map_err(|e| {
                Error::Io {
                    path: dir.join(&name),
                    source: e,
                }
            })?;
            // `AddNewChunkIndex1`: the reader enumerates this record, not the
            // directory.
            let record = catalog::layout::IndexChunk {
                chunk_id,
                base_name: String::new(),
                base_row_id: u64::from(segment) << 32,
                row_count: count as u64,
                term_count: 0,
                index_size: chunk.len() as u64,
                commit_ts: self.txn.current_ts().max(1),
            };
            let index = catalog::layout::IndexSegment {
                index_id: index_id as u64,
                segment_id: segment,
                chunks: vec![record.clone()],
            };
            let _ = self
                .storage
                .catalog
                .append_index_chunk(&set, &index, &record);
            written += count;
        }
        Ok(written)
    }

    /// Reads back a mirror written by [`Self::write_ivf_index`], returning
    /// the IVF chunks so a query can probe them (plan #89).
    pub fn load_ivf_indexes(&self, table: &str, index_name: &str) -> Vec<index::ivf::IvfSegment> {
        let options = self.ivf_storage_option(table, index_name);
        let mut out = Vec::new();
        // The catalog's chunk list (`GetChunkIDs1`), over every segment: the
        // reference probes the chunks a segment records, and an entry `Cleanup`
        // retired is not one of them.
        let Some(index_id) = self.index_position(table, index_name).ok() else {
            return Vec::new();
        };
        for (_, _, path) in self.catalog_index_files(table, index_id as u64) {
            let Ok(bytes) = storage::chunk_store::read_file(&path) else {
                continue;
            };
            if options.storage.eq_ignore_ascii_case("scalar_quantization") {
                if let Some(segment) = Self::decode_ivf_sq_chunk(&bytes, options.sq_bits) {
                    out.push(segment);
                }
                continue;
            }
            if options.storage.eq_ignore_ascii_case("product_quantization") {
                if let Some(segment) = Self::decode_ivf_pq_chunk(&bytes, &options) {
                    out.push(segment);
                }
                continue;
            }
            let Ok((header, flat, parts)) = knn_format::decode_ivf_plain_chunk(&bytes, 4) else {
                continue;
            };
            let dims = header.dimension as usize;
            if dims == 0 {
                continue;
            }
            let parts: Vec<Vec<(u32, Vec<f32>)>> = parts
                .into_iter()
                .map(|part| {
                    part.segment_offsets
                        .into_iter()
                        .enumerate()
                        .map(|(i, offset)| {
                            let at = i * dims * 4;
                            let vector = (0..dims)
                                .map(|d| {
                                    f32::from_le_bytes(
                                        part.vectors[at + d * 4..at + d * 4 + 4]
                                            .try_into()
                                            .unwrap(),
                                    )
                                })
                                .collect();
                            (offset, vector)
                        })
                        .collect()
                })
                .collect();
            out.push(index::ivf::IvfSegment::Plain(index::ivf::IvfIndex::new(
                dims,
                header.centroid_count as usize,
                flat,
                parts,
            )));
        }
        out
    }

    /// The index's IVF options (`IndexIVFStorageOption` + the centroid option),
    /// defaulting exactly like the reference: plain storage and
    /// `centroids_num_ratio = 1.0` / `min_points_per_centroid = 32`.
    pub(crate) fn ivf_storage_option(&self, table: &str, index_name: &str) -> IvfOptions {
        let table = self.qualified_table(table);
        let options = self
            .storage
            .catalog
            .index_by_name(&table, index_name)
            .ok()
            .flatten()
            .map(|im| im.options)
            .unwrap_or_default();
        let find = |key: &str| {
            options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(key))
                .map(|(_, v)| v.trim().trim_matches(|c| c == '\'' || c == '"').to_string())
        };
        IvfOptions {
            storage: find("STORAGE_TYPE").unwrap_or_else(|| "plain".to_string()),
            sq_bits: find("SCALAR_QUANTIZATION_BITS")
                .and_then(|v| v.parse::<u8>().ok())
                .filter(|b| *b == 4 || *b == 8)
                .unwrap_or(8),
            pq_subspace_num: find("PRODUCT_QUANTIZATION_SUBSPACE_NUM")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|v| *v > 0)
                .unwrap_or(1),
            pq_bits: find("PRODUCT_QUANTIZATION_SUBSPACE_BITS")
                .and_then(|v| v.parse::<u8>().ok())
                .filter(|b| [4u8, 8, 12, 16].contains(b))
                .unwrap_or(8),
            ratio: find("CENTROIDS_NUM_RATIO")
                .and_then(|v| v.parse::<f32>().ok())
                .unwrap_or(1.0),
            min_points: find("MIN_POINTS_PER_CENTROID")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|v| *v > 0)
                .unwrap_or(32),
        }
    }

    /// Decodes one scalar-quantized chunk into a searchable segment.
    pub(crate) fn decode_ivf_sq_chunk(bytes: &[u8], sq_bits: u8) -> Option<index::ivf::IvfSegment> {
        let chunk = knn_format::decode_ivf_sq_chunk(bytes, sq_bits).ok()?;
        let dims = chunk.header.dimension as usize;
        if dims == 0 {
            return None;
        }
        let parts = chunk
            .parts
            .into_iter()
            .map(|part| index::ivf::IvfSqPartData {
                offsets: part.segment_offsets,
                codes: part.codes,
                dot_v_e: part.dot_v_e,
            })
            .collect();
        Some(index::ivf::IvfSegment::Scalar(index::ivf::IvfSqIndex {
            dims,
            centroids: chunk.centroids,
            centroid_count: chunk.header.centroid_count as usize,
            a: chunk.common_vec_a,
            b: chunk.common_vec_b,
            sq_bits,
            parts,
        }))
    }

    /// Reads back a mirror written by [`Self::write_ivf_index`], returning
    /// the centroids and `(row id, vector)` pairs.
    pub fn read_ivf_index(&self, table: &str, index_name: &str) -> Result<(Vec<f32>, VectorRows)> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.index_position(&table, index_name)?;
        // The catalog's chunk list, not a literal `chunk_0.idx`: the id comes
        // from `GetAndSetNextChunkID`, so the file is named after it.
        let dir = self.blocks_root().join(format!(
            "db_{}/tbl_{}/idx_{index_id}/seg_0",
            self.database_id_of(&table),
            meta.id
        ));
        let path = self
            .live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .map(|(_, path)| path)
            .next()
            .ok_or_else(|| {
                Error::NotFound(format!("no IVF mirror for '{index_name}' on '{table}'"))
            })?;
        let bytes = storage::chunk_store::read_file(&path)
            .map_err(|_| Error::NotFound(format!("no IVF mirror at {}", path.display())))?;
        let (header, centroids, parts) = knn_format::decode_ivf_plain_chunk(&bytes, 4)
            .map_err(|e| Error::Invalid(format!("bad IVF chunk: {e}")))?;
        let dim = header.dimension as usize;
        let mut out = Vec::new();
        for part in parts {
            for (i, label) in part.segment_offsets.iter().enumerate() {
                let at = i * dim * 4;
                let v = part.vectors[at..at + dim * 4]
                    .as_chunks::<4>()
                    .0
                    .iter()
                    .map(|c| f32::from_le_bytes(*c))
                    .collect();
                out.push((*label, v));
            }
        }
        Ok((centroids, out))
    }

    /// PLAID index options of `index_name` (`nbits`, `n_centroids`,
    /// `colbertsar`), from `CREATE INDEX … USING PLAID WITH (…)`.
    pub(crate) fn plaid_options(&self, table: &str, index_name: &str) -> PlaidOptions {
        let Some(im) = self
            .storage
            .catalog
            .index_by_name(table, index_name)
            .ok()
            .flatten()
        else {
            return PlaidOptions::default();
        };
        let mut options = PlaidOptions::default();
        for (key, value) in &im.options {
            let value = value.trim();
            if key.eq_ignore_ascii_case("nbits") {
                if let Ok(v) = value.parse() {
                    options.nbits = v;
                }
            } else if key.eq_ignore_ascii_case("n_centroids")
                || key.eq_ignore_ascii_case("centroids")
            {
                // `centroids` is the SQL spelling (`CREATE INDEX … WITH
                // (centroids = 4096)`); `n_centroids` is the catalog field the
                // record carries. Both name the same option.
                if let Ok(v) = value.parse() {
                    options.n_centroids = v;
                }
            } else if key.eq_ignore_ascii_case("colbertsar_mode") {
                options.colbertsar = value.eq_ignore_ascii_case("true") || value == "1";
            }
        }
        options
    }

    /// The tensor documents of `table.column` as `(row id, tokens)`.
    pub(crate) fn column_docs(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<Vec<(u32, Vec<Vec<f32>>)>> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}' on '{table}'")))?;
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            let text = match row.get(col_idx) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => continue,
            };
            if let Some(doc) = crate::parse_multivector_doc(&text, dims) {
                out.push((seq as u32, doc));
            }
        }
        Ok(out)
    }

    /// Builds a PLAID index over `column` and serializes it in the reference's
    /// `PlaidIndex::SaveIndexInner` layout (the `chunk_0.idx` the file worker
    /// writes).
    pub(crate) fn plaid_chunk(
        &self,
        table: &str,
        column: &str,
        dims: usize,
        options: &PlaidOptions,
    ) -> Result<(usize, Vec<u8>)> {
        let docs = self.column_docs(table, column, dims)?;
        let total: usize = docs.iter().map(|(_, d)| d.len()).sum();
        if total == 0 {
            return Err(Error::Invalid(format!(
                "no tensors to index in '{table}.{column}'"
            )));
        }
        let flat: Vec<f32> = docs
            .iter()
            .flat_map(|(_, d)| d.iter().flatten().copied())
            .collect();
        let lens: Vec<u32> = docs.iter().map(|(_, d)| d.len() as u32).collect();
        let mut k = if options.n_centroids == 0 {
            PlaidIndex::compute_auto_n_centroids(total as u64)
        } else {
            options.n_centroids
        };
        // Mirror `PlaidIndexInMem::BuildIndexFromScratch`: downgrade below the
        // k-means data bound, keep the multiple-of-8 alignment.
        while k > 8 && total < 32 * k as usize {
            k >>= 1;
        }
        k = k.div_ceil(8) * 8;
        if (k as usize) > total {
            return Err(Error::Invalid(format!(
                "not enough tensors for a PLAID index in '{table}.{column}': {total} < {k}"
            )));
        }
        let mut index = PlaidIndex::new(0, dims as u32, options.nbits, k, options.colbertsar)?;
        index.train(k, &flat, total, 4)?;
        index.add_multiple_docs_embeddings(&flat, &lens)?;
        Ok((total, index.to_bytes()))
    }

    /// Where a table's PLAID mirror lives and what it indexes.
    pub(crate) fn plaid_mirror(&self, table: &str, index_name: &str) -> Result<PlaidMirror> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .index_by_name(&table, index_name)?
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let ci = meta
            .columns
            .iter()
            .position(|c| c == &column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let type_text = meta
            .types
            .get(ci)
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let (_, dims, elem) = tensor_shape(&type_text)
            .ok_or_else(|| Error::Invalid(format!("not a tensor column: '{type_text}'")))?;
        let db_id = self.database_id_of(&table);
        Ok(PlaidMirror {
            dir: self
                .blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id)),
            column,
            dims: dims as u32,
            elem,
        })
    }

    /// Every `chunk_{id}.idx` of `table`'s PLAID mirror, decoded in chunk order.
    pub(crate) fn read_plaid_chunks(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<Vec<PlaidChunkFile>> {
        let mirror = self.plaid_mirror(table, index_name)?;
        let nbits = self
            .plaid_options(&self.qualified_table(table), index_name)
            .nbits;
        let mut chunks = Vec::new();
        for (id, path) in self.live_chunk_files(table, index_name, 0, &mirror.dir) {
            // Map the chunk and keep its packed residual codes file-backed (the
            // reference's buffer-manager chunk): reading them into a `Vec` doubled
            // the resident size of a large index. A zero-length or unopenable file
            // falls back to a plain read.
            let bad = |e: &dyn std::fmt::Display| {
                Error::Invalid(format!("bad PLAID chunk '{}': {e}", path.display()))
            };
            let index = match storage::buffer::mapped::MappedFile::open(&path) {
                Ok(mapped) if !mapped.is_empty() => {
                    let backing: std::sync::Arc<dyn AsRef<[u8]> + Send + Sync> =
                        std::sync::Arc::new(mapped);
                    PlaidIndex::from_shared(backing, mirror.dims, nbits).map_err(|e| bad(&e))?
                }
                _ => {
                    let bytes = storage::chunk_store::read_file(&path).map_err(|e| Error::Io {
                        path: path.clone(),
                        source: e,
                    })?;
                    PlaidIndex::from_bytes(0, mirror.dims, nbits, &bytes)
                        .map_err(|e| bad(&e))?
                        .0
                }
            };
            chunks.push(PlaidChunkFile { id, index });
        }
        Ok(chunks)
    }

    /// Reads every chunk of `table`'s PLAID mirror individually.
    pub fn read_plaid_index_chunks(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<Vec<PlaidIndex>> {
        Ok(self
            .read_plaid_chunks(table, index_name)?
            .into_iter()
            .map(|chunk| chunk.index)
            .collect())
    }

    /// `PlaidIndex::InitializeMerge` → `MergeOneChunk` with a cumulative
    /// `doc_offset` → `FinalizeMerge` → `CopyCentroidsFrom`, driven the way
    /// `NewTxn::OptimizePlaidIndex` (`new_txn_index_impl.cpp:2348-2424`) drives
    /// it.
    pub(crate) fn merge_plaid_chunk_indexes(chunks: &[PlaidIndex]) -> Result<PlaidIndex> {
        let first = chunks
            .first()
            .ok_or_else(|| Error::Invalid("no PLAID chunk to merge".into()))?;
        let mut merged = PlaidIndex::new(
            0,
            first.embedding_dimension(),
            first.nbits,
            first.n_centroids(),
            first.is_colbertsar_mode(),
        )?;
        merged.initialize_merge(first.n_centroids());
        let mut doc_offset = 0u32;
        for chunk in chunks {
            if chunk.doc_num() == 0 {
                continue;
            }
            merged.merge_one_chunk(chunk, doc_offset)?;
            doc_offset += chunk.doc_num();
        }
        merged.finalize_merge();
        // `preserve_ivf_lists = true`: `MergeOneChunk` already rebuilt them for
        // the merged document ids.
        merged.copy_centroids_from(
            first.centroids_data(),
            first.centroid_norms_neg_half(),
            first.n_centroids(),
            first.quantizer(),
            true,
        )?;
        Ok(merged)
    }

    /// The optimize merge (`NewTxn::OptimizePlaidIndex`): folds every chunk
    /// into one and appends it as the newest `chunk_{n}.idx`. The chunks it
    /// folded in are dropped (the reference deprecates them in the chunk meta;
    /// the mirror has no meta, so the files themselves are the record) —
    /// [`Self::cleanup_index_chunks`] keeps the same rule as a safety net
    /// for a removal that failed. Returns the merged document count.
    /// Returns the chunk it folded the segment's chunks into, or `None` when
    /// there was nothing to merge - the entry an optimize transaction records.
    pub(crate) fn merge_plaid_index(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<Option<ChunkRewrite>> {
        let mirror = self.plaid_mirror(table, index_name)?;
        let chunks = self.read_plaid_chunks(table, index_name)?;
        if chunks.len() < 2 {
            return Ok(None);
        }
        // `GetAndSetNextChunkID`: merged chunks take the catalog's next id, so
        // they never land on a source's file or a retired id.
        let set = self.index_segment_set(table);
        let index_id = self.index_position(table, index_name)? as u64;
        let next = self.next_index_chunk_id(&set, &index_id.to_string(), 0);
        let deprecate_ids: Vec<u32> = chunks.iter().map(|c| c.id as u32).collect();
        let indexes: Vec<PlaidIndex> = chunks.into_iter().map(|c| c.index).collect();
        let merged = Self::merge_plaid_chunk_indexes(&indexes)?;
        let bytes = merged.to_bytes();
        let path = mirror.dir.join(format!("chunk_{next}.idx"));
        storage::definition::format::write_synced(&path, &bytes).map_err(|e| Error::Io {
            path: path.clone(),
            source: e,
        })?;
        // The merged chunk is recorded here **and** rides in the transaction's
        // `WalCmdDumpIndexV2` (`optimize_index_commands`): the command is what
        // gives it its commit ts, and this write is what makes it visible to a
        // reader that lands before the commit. A rolled-back pass leaves it in
        // the list beside its sources, which the superseded ids below retire on
        // the next successful optimize.
        self.record_chunk(
            &set,
            index_id,
            next,
            0,
            merged.doc_num() as u64,
            bytes.len() as u64,
        );
        Ok(Some(ChunkRewrite {
            segment: 0,
            chunk_id: next,
            row_count: merged.doc_num(),
            deprecate_ids,
        }))
    }

    /// Writes `table`'s PLAID index as `idx_{id}/seg_0/chunk_{n}.idx`, appending
    /// one chunk per dump the way the reference's memory-index dump does: the
    /// rows past the ones already mirrored become the new chunk, sharing the
    /// first chunk's global centroids instead of retraining.
    pub fn write_plaid_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let mirror = self.plaid_mirror(&table, index_name)?;
        if mirror.elem != 6 {
            return Err(Error::Invalid(
                "PLAID only indexes TENSOR(FLOAT, dim) columns".into(),
            ));
        }
        let options = self.plaid_options(&table, index_name);
        std::fs::create_dir_all(&mirror.dir).map_err(|e| Error::Io {
            path: mirror.dir.clone(),
            source: e,
        })?;
        let chunks = self.read_plaid_chunks(&table, index_name)?;
        if chunks.is_empty() {
            let docs_len = self
                .column_docs(&table, &mirror.column, mirror.dims as usize)
                .map(|docs| docs.len())
                .unwrap_or(0);
            let (count, bytes) =
                self.plaid_chunk(&table, &mirror.column, mirror.dims as usize, &options)?;
            // `GetAndSetNextChunkID`, never a literal `chunk_0`: the counter
            // only moves forward, so an id a merge or `Cleanup` retired never
            // comes back and two dumps never share a file.
            let set = self.index_segment_set(&table);
            let index_id = self.index_position(&table, index_name)? as u64;
            let chunk_id = self.next_index_chunk_id(&set, &index_id.to_string(), 0);
            let path = mirror.dir.join(format!("chunk_{chunk_id}.idx"));
            storage::definition::format::write_synced(&path, &bytes).map_err(|e| Error::Io {
                path: path.clone(),
                source: e,
            })?;
            // `row_cnt_` is the **document** count (`ChunkIndexMetaInfo`), which
            // is what the chunk covers; `count` here is the token total.
            self.record_chunk(
                &set,
                index_id,
                chunk_id,
                0,
                docs_len as u64,
                bytes.len() as u64,
            );
            // `SegmentIndexMeta::SavePlaidGlobalCentroids`: the trained
            // centroids and their quantizer live in a sidecar next to the
            // chunk, and a reference reader needs it to open the segment.
            if let Some(index) = self
                .read_plaid_index_chunks(&table, index_name)
                .ok()
                .and_then(|chunks| chunks.into_iter().next())
            {
                let centroids_path = mirror.dir.join("plaid_centroids.bin");
                storage::definition::format::write_synced(
                    &centroids_path,
                    &index.save_plaid_centroids(),
                )
                .map_err(|e| Error::Io {
                    path: centroids_path.clone(),
                    source: e,
                })?;
            }
            return Ok(count);
        }
        // The mirrored rows are the concatenation of the chunks, so a new dump
        // only carries the rows past that watermark. A merged newest chunk
        // already covers the older ones and replaces them in the count.
        let sizes: Vec<(u32, u32)> = chunks
            .iter()
            .map(|c| (c.index.doc_num(), c.index.total_embedding_num()))
            .collect();
        let (watermark, indexed) = plaid_coverage(&sizes);
        let watermark = watermark as usize;
        let docs = self.column_docs(&table, &mirror.column, mirror.dims as usize)?;
        if docs.len() <= watermark {
            return Ok(indexed as usize);
        }
        let fresh = &docs[watermark..];
        let lens: Vec<u32> = fresh
            .iter()
            .map(|(_, tokens)| tokens.len() as u32)
            .collect();
        let total: usize = lens.iter().map(|len| *len as usize).sum();
        if total == 0 {
            return Ok(indexed as usize);
        }
        let flat: Vec<f32> = fresh
            .iter()
            .flat_map(|(_, tokens)| tokens.iter().flatten().copied())
            .collect();
        let first = &chunks[0].index;
        let mut index = PlaidIndex::new(
            0,
            mirror.dims,
            first.nbits,
            first.n_centroids(),
            first.is_colbertsar_mode(),
        )?;
        // `PlaidIndex::ShareGlobalCentroids`: a later chunk reuses the trained
        // centroids and quantizer of the first one.
        index.copy_centroids_from(
            first.centroids_data(),
            first.centroid_norms_neg_half(),
            first.n_centroids(),
            first.quantizer(),
            false,
        )?;
        index.add_multiple_docs_embeddings(&flat, &lens)?;
        // `GetAndSetNextChunkID`: the counter, not "one past the newest file".
        let set = self.index_segment_set(&table);
        let index_id = self.index_position(&table, index_name)? as u64;
        let next = self.next_index_chunk_id(&set, &index_id.to_string(), 0);
        let bytes = index.to_bytes();
        let path = mirror.dir.join(format!("chunk_{next}.idx"));
        storage::definition::format::write_synced(&path, &bytes).map_err(|e| Error::Io {
            path: path.clone(),
            source: e,
        })?;
        // The record's `row_cnt_` is the document count, not the token total.
        self.record_chunk(
            &set,
            index_id,
            next,
            0,
            fresh.len() as u64,
            bytes.len() as u64,
        );
        Ok(indexed as usize + total)
    }

    /// Reads back a mirror written by [`Self::write_plaid_index`]. A merged
    /// newest chunk is read alone; otherwise the chunks are merged the same way
    /// [`Self::merge_plaid_index`] merges them.
    pub fn read_plaid_index(&self, table: &str, index_name: &str) -> Result<PlaidIndex> {
        let chunks = self.read_plaid_chunks(table, index_name)?;
        let doc_nums: Vec<u32> = chunks.iter().map(|c| c.index.doc_num()).collect();
        let mut indexes: Vec<PlaidIndex> = if plaid_merged_tail(&doc_nums) {
            // The merge already folded every older chunk into the newest one.
            chunks.into_iter().rev().take(1).map(|c| c.index).collect()
        } else {
            chunks.into_iter().map(|c| c.index).collect()
        };
        match indexes.len() {
            0 => Err(Error::NotFound(format!(
                "no PLAID mirror for '{index_name}'"
            ))),
            1 => Ok(indexes.remove(0)),
            _ => Self::merge_plaid_chunk_indexes(&indexes),
        }
    }
}

/// Casts a row's values to their columns' types, the way the reference's
/// DataBlock::AppendValue does on append: an integer column truncates a float
/// and wraps a value that does not fit its width.
/// Rounds every element of an embedding text ("[1.1,2.2]") to the half it can
/// represent, in place.
fn round_half_elements(text: &str, bf16: bool) -> String {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .and_then(|s| s.strip_suffix(']'))
        .unwrap_or("");
    if inner.trim().is_empty() {
        return text.to_string();
    }
    let parts: Vec<String> = inner
        .split(',')
        .map(|p| {
            let v: f32 = p.trim().parse().unwrap_or(0.0);
            let r = if bf16 {
                f32::from_bits((v.to_bits() >> 16) << 16)
            } else {
                f16_bits_to_f32(::common::fp16::f32_to_f16_bits(v))
            };
            format!("{r}")
        })
        .collect();
    format!("[{}]", parts.join(","))
}

/// IEEE half -> f32 (the inverse of `common::fp16::f32_to_f16_bits`).
fn f16_bits_to_f32(bits: u16) -> f32 {
    let sign = u32::from(bits >> 15) << 31;
    let exp = u32::from((bits >> 10) & 0x1f);
    let frac = u32::from(bits & 0x3ff);
    if exp == 0 {
        // Subnormal (or zero): value = frac * 2^-24
        let v = frac as f32 * (2f32.powi(-24));
        return if sign != 0 { -v } else { v };
    }
    if exp == 31 {
        return f32::from_bits(sign | 0x7f80_0000 | (frac << 13));
    }
    f32::from_bits(sign | ((exp + 112) << 23) | (frac << 13))
}

/// What `coerce_row` does to one column, decided once per column.
///
/// The type text is only read to find the column's head (`INT8`, `BIGINT`, …)
/// and whether it is a 16-bit float. Deriving that per cell upper-cased the
/// whole declared type twice per value - 16.7 M `String`s on an 8 M-row,
/// two-column `COPY`.
#[derive(Clone, Copy)]
enum Coercion {
    None,
    Narrow(fn(i64) -> i64),
    RoundHalf { bf16: bool },
}

fn coercion_of(ty: &str) -> Coercion {
    let upper = ty.trim().to_ascii_uppercase();
    let head = upper
        .split('(')
        .next()
        .unwrap_or("")
        .split(',')
        .next()
        .unwrap_or("")
        .trim();
    // A 16-bit float column rounds every element to the half it can represent;
    // the block writer does that on encode, so the append buffer has to as well
    // or a read before the next checkpoint shows more precision than the
    // column can hold.
    if head == "EMBEDDING" && (upper.contains("FLOAT16") || upper.contains("BFLOAT16")) {
        return Coercion::RoundHalf {
            bf16: upper.contains("BFLOAT16"),
        };
    }
    match head {
        "TINYINT" | "INT8" => Coercion::Narrow(|n| (n as i8) as i64),
        "SMALLINT" | "INT16" => Coercion::Narrow(|n| (n as i16) as i64),
        "INT" | "INTEGER" | "INT32" => Coercion::Narrow(|n| (n as i32) as i64),
        _ => Coercion::None,
    }
}

fn coerce_row(row: &[Value], coercions: &[Coercion]) -> Vec<Value> {
    // A row whose columns all pass through is handed over as it is: this runs
    // for every row of every `COPY`, and the clone is the only cost.
    if coercions.iter().all(|c| matches!(c, Coercion::None)) {
        return row.to_vec();
    }
    let mut out = row.to_vec();
    for (i, v) in out.iter_mut().enumerate() {
        let Some(coercion) = coercions.get(i) else {
            continue;
        };
        match coercion {
            Coercion::None => {}
            Coercion::RoundHalf { bf16 } => {
                if let Value::Str(text) = v {
                    *v = Value::Str(round_half_elements(text, *bf16));
                }
            }
            Coercion::Narrow(narrow) => {
                let widened = match v {
                    Value::Int(n) => Some(*n),
                    Value::Float(f) => Some(*f as i64),
                    Value::Str(s) => s.trim().parse::<f64>().ok().map(|f| f as i64),
                    _ => None,
                };
                if let Some(n) = widened {
                    *v = Value::Int(narrow(n));
                }
            }
        }
    }
    out
}
/// Tables a checkpoint mirrors at a time.
///
/// Independent of `INFINITY_FLUSH_DUMP_CONCURRENCY` (that one splits a bulk
/// dump across **indexes**): a checkpoint's unit is one table, so it defaults
/// to the core count and is clamped so a stray value cannot start hundreds of
/// threads.
fn checkpoint_workers() -> usize {
    std::env::var("INFINITY_CHECKPOINT_CONCURRENCY")
        .ok()
        .and_then(|value| value.trim().parse::<usize>().ok())
        .unwrap_or_else(|| {
            std::thread::available_parallelism()
                .map(|n| n.get())
                .unwrap_or(1)
        })
        .clamp(1, 64)
}

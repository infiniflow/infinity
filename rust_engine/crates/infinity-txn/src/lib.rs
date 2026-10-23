//! `infinity-txn`: the transaction commit pipeline.
//!
//! Mirrors the engine's order (plan 附录 A.4): **WriteWAL (serial) -> CommitBottom
//! (KV atomic write)**. Every commit appends a *typed* command list
//! (`WalCmd`, [`infinity_storage::wal_entry`]) to the WAL, in the reference's
//! own `WalEntry` byte layout; recovery reads it back through
//! [`WalEntryIterator::make`] and replays each command, instead of replaying a
//! raw KV put batch.
//!
//! Commands the committing layer can identify are emitted typed: row puts
//! become [`WalCmd::Append`] (the row blob rides in the block's single varchar
//! column, the engine stores rows as encoded text) and tombstones become
//! [`WalCmd::Delete`]. Catalog/DDL records are carried in a
//! [`WalCmd::KvBatch`] because their value codec lives in `infinity-catalog`
//! and replaying them as `WalCmdCreateTableV2`/… needs catalog operations the
//! executor owns; the batch is applied verbatim so nothing is dropped.

#![forbid(unsafe_code)]

pub mod state;

pub use state::{transaction_type_to_str, txn_state_to_str, TransactionType, TxnState};

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result, TxnTimeStamp};
use infinity_storage::data_block::{BlockValue, Column, ColumnVector, DataBlock};
use infinity_storage::{
    KvBatchKind, MetaStore, WalCmd, WalEntry, WalEntryIterator, WalManager, WalRowId,
};

/// Row record prefix of the engine's KV keys (`row:{table}:{seq:016}`).
const ROW_PREFIX: &str = "row:";
/// Tombstone prefix (`del:{table}:{seq:016}`).
const DEL_PREFIX: &str = "del:";

/// C++: `TxnInfo` (`new_txn_manager.cppm`): what `SHOW TRANSACTIONS` reads
/// from the manager's `txn_map_`.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct TxnInfo {
    pub txn_id: u64,
    /// The statement text the transaction was started with.
    pub txn_text: String,
    pub begin_ts: TxnTimeStamp,
}

/// Keys a transaction read, for the conflict check (`NewTxn`'s read set).
#[derive(Default)]
struct ReadSet {
    keys: Vec<Vec<u8>>,
    prefixes: Vec<Vec<u8>>,
}

/// A pending transaction.
pub struct Txn {
    pub id: u64,
    pub begin_ts: TxnTimeStamp,
    /// The statement text (C++ `NewTxn::txn_text_`), for `SHOW TRANSACTIONS`.
    pub txn_text: String,
    writes: Vec<(Vec<u8>, Vec<u8>)>,
    /// Tombstones this transaction writes. They are validated against newer
    /// commits at commit time (`NewTxn::CommitTxn` conflict check).
    deletes: Vec<Vec<u8>>,
    reads: ReadSet,
    /// The manager's active-transaction registry (C++ `txn_map_`). Dropping a
    /// transaction that was neither committed nor rolled back releases its
    /// slot, as `~NewTxn` calls `RollBackTxn` in the reference.
    active: Arc<Mutex<HashMap<u64, TxnInfo>>>,
}

impl Txn {
    pub fn put(&mut self, key: impl Into<Vec<u8>>, value: impl Into<Vec<u8>>) {
        self.writes.push((key.into(), value.into()));
    }
    /// Records a tombstone write. The key is validated at commit: if another
    /// transaction deleted it after this one began, the commit gets
    /// `kTxnConflict`.
    pub fn delete(&mut self, key: impl Into<Vec<u8>>) {
        self.deletes.push(key.into());
    }
    /// Records a read of `key`, so a commit by a transaction that overlaps
    /// this one's lifetime invalidates the commit (C++ `NewTxn::GetStore`).
    pub fn read(&mut self, key: impl Into<Vec<u8>>) {
        self.reads.keys.push(key.into());
    }
    /// Records a read of every key under `prefix` (C++ `NewTxn` range read).
    pub fn read_prefix(&mut self, prefix: impl Into<Vec<u8>>) {
        self.reads.prefixes.push(prefix.into());
    }
    pub fn len(&self) -> usize {
        self.writes.len() + self.deletes.len()
    }
    pub fn is_empty(&self) -> bool {
        self.writes.is_empty() && self.deletes.is_empty()
    }
}

impl Drop for Txn {
    fn drop(&mut self) {
        if let Ok(mut active) = self.active.lock() {
            active.remove(&self.id);
        }
    }
}

/// Owns the WAL + KV pair and hands out transactions.
pub struct TxnManager {
    store: Arc<dyn MetaStore>,
    /// Where a transaction's **catalog** records land.
    ///
    /// The reference has one `kv_instance_` for the catalog and keeps rows in
    /// block files; this engine keeps rows in a KV of its own, so a transaction
    /// can span both. Its row commands (`Append`/`Delete`) apply to `store`,
    /// everything else to this one - which is the catalog the DDL writes into.
    /// Both are the same store in a build that keeps them together.
    catalog_store: Arc<dyn MetaStore>,
    wal: Mutex<WalManager>,
    wal_path: PathBuf,
    /// `wal/wal.log`: the file the **reference** reads. It holds exactly one
    /// entry, the `kNewCheckpoint` command a checkpoint writes, because the
    /// reference replays every record in that file and rejects one it cannot
    /// decode. This engine's own redo records live in `wal.rust` instead (they
    /// are applied at the checkpoint, so the reference has nothing to replay).
    checkpoint_path: PathBuf,
    next_txn_id: AtomicU64,
    /// `NewTxnManager::current_ts_`: the timestamp of the last **completed**
    /// commit. A new transaction's `begin_ts` is `current_ts + 1`, so it sees
    /// every commit up to here and none in flight.
    current_ts: AtomicU64,
    /// `NewTxnManager::prepare_commit_ts_`: the high-water mark that committers
    /// take from. A write transaction takes `+= 2`, which keeps the odd values
    /// for read-only snapshots (`GetReadCommitTS` is `current_ts + 1`), so a
    /// reader's snapshot can never land between a write transaction's begin and
    /// its commit.
    prepare_commit_ts: AtomicU64,
    /// Monotonic high-water mark of commit timestamps applied to the KV. The
    /// marker written into each batch must never regress, or a concurrent
    /// checkpoint would keep a much longer WAL than necessary.
    max_applied: AtomicU64,
    /// Highest commit timestamp appended to the active log (rotation seal).
    last_appended: AtomicU64,
    /// Highest commit timestamp whose batch has been fsynced to the KV store;
    /// only records up to this point may be dropped from the log.
    synced: AtomicU64,
    /// `total_commit_count`: successful commits since open (the reference's
    /// `NewTxnManager::total_committed_txn_count`).
    total_committed: AtomicU64,
    /// `NewTxnManager::total_rollbacked_txn_count`.
    total_rolled_back: AtomicU64,
    /// Last commit timestamp that wrote each key. C++ keeps the same
    /// information as per-transaction stores and compares them in
    /// `CheckConflict1`; keyed by key it answers the same question for the
    /// engine's KV-shaped writes. Rebuilt from the WAL on open (see
    /// [`TxnManager::open`]), never persisted separately.
    write_ts: Mutex<HashMap<Vec<u8>, TxnTimeStamp>>,
    /// C++ `NewTxnManager::txn_map_`: the live transactions, in begin order for
    /// `SHOW TRANSACTIONS`.
    active: Arc<Mutex<HashMap<u64, TxnInfo>>>,
    /// Serialises conflict validation + commit so the first committer wins.
    commit_lock: Mutex<()>,
}

impl TxnManager {
    /// Opens the manager, seeding counters from any existing WAL (recovery)
    /// and rebuilding the conflict state from it.
    ///
    /// C++ rebuilds its conflict view from the replayed `WalEntry`s as well
    /// (`NewTxnManager::ReplayWalEntries`); nothing is persisted separately.
    /// Records truncated by a checkpoint do not need to be rebuilt: a new
    /// transaction's `begin_ts` is the WAL/KV high-water mark, so an older
    /// commit can never be "newer than begin_ts".
    pub fn open(wal_path: impl Into<PathBuf>, store: Arc<dyn MetaStore>) -> Result<Self> {
        let checkpoint_path = wal_path.into();
        // This engine's redo records go to `wal.rust` (plus `wal.rust.{ts}`
        // segments); `wal.log` is reserved for the reference-framed checkpoint
        // entry, so the reference never has to decode a record this engine
        // wrote. Sealing still accepts `wal.log.{ts}` for older directories.
        let wal_path = checkpoint_path.with_file_name("wal.rust");
        let recs = read_all_segments(&wal_path)?;
        let applied_ts = store
            .get(APPLIED_TS_KEY)?
            .and_then(|v| {
                v.get(..8)
                    .map(|b| TxnTimeStamp::from_le_bytes(b.try_into().unwrap()))
            })
            .unwrap_or(0);
        let max_ts = recs.iter().map(|r| r.commit_ts).max().unwrap_or(0);
        let max_id = recs
            .iter()
            .map(|r| r.txn_id.max(0) as u64)
            .max()
            .unwrap_or(0);
        let mut write_ts = HashMap::new();
        for rec in &recs {
            for key in record_write_keys(rec) {
                write_ts.insert(key, rec.commit_ts);
            }
        }
        let wal = WalManager::open(&wal_path)?;
        // The reference replays the WAL, then reserves exactly one timestamp
        // for the memory-index recovery before its startup checkpoint
        // (`Storage::Init` -> `RecoverMemIndex` -> `BeginRecoveryTxn`, which
        // sets `prepare_commit_ts_ = current_ts_ + 2` and leaves `current_ts_`
        // alone). The reservation is permanent: the startup checkpoint takes
        // `current + 4` and every later commit is offset by the same two.
        let start_ts = max_ts.max(applied_ts);
        Ok(Self {
            catalog_store: store.clone(),
            store,
            wal: Mutex::new(wal),
            wal_path,
            checkpoint_path,
            next_txn_id: AtomicU64::new(max_id + 1),
            // A checkpoint may have truncated the WAL, so the applied marker in
            // the KV store is the other source of the timestamp high-water mark.
            // The reference seeds both pointers from `start_ts`, the checkpoint
            // frame's commit ts (`NewTxnManager(..., start_ts)`).
            current_ts: AtomicU64::new(start_ts),
            prepare_commit_ts: AtomicU64::new(start_ts + 2),
            max_applied: AtomicU64::new(applied_ts),
            last_appended: AtomicU64::new(max_ts),
            synced: AtomicU64::new(applied_ts),
            total_committed: AtomicU64::new(0),
            total_rolled_back: AtomicU64::new(0),
            write_ts: Mutex::new(write_ts),
            active: Arc::new(Mutex::new(HashMap::new())),
            commit_lock: Mutex::new(()),
        })
    }

    /// Points the transaction manager at the store a directory keeps its
    /// catalog records in (`<dir>/catalog` while rows live in
    /// `<dir>/persistence`).
    pub fn with_catalog_store(mut self, store: Arc<dyn MetaStore>) -> Self {
        self.catalog_store = store;
        self
    }

    pub fn begin(&self) -> Txn {
        self.begin_with_text("")
    }

    /// C++: `NewTxnManager::BeginTxnShared` — registers the transaction in
    /// `txn_map_` so `SHOW TRANSACTIONS` and `active_txn_count` see it.
    pub fn begin_with_text(&self, txn_text: impl Into<String>) -> Txn {
        let id = self.next_txn_id.fetch_add(1, Ordering::SeqCst);
        // `NewTxnManager::BeginTxnShared`: `begin_ts = current_ts_ + 1`. It is
        // one past the last completed commit, so it is *below* this
        // transaction's own commit timestamp (which is `current_ts_ + 2`) and
        // above every commit that finished before it began.
        let begin_ts = self.current_ts.load(Ordering::SeqCst) + 1;
        let info = TxnInfo {
            txn_id: id,
            txn_text: txn_text.into(),
            begin_ts,
        };
        self.active.lock().unwrap().insert(id, info.clone());
        Txn {
            id,
            begin_ts,
            txn_text: info.txn_text,
            writes: Vec::new(),
            deletes: Vec::new(),
            reads: ReadSet::default(),
            active: self.active.clone(),
        }
    }

    /// C++: `NewTxnManager::CheckConflict1` — `true` when some transaction
    /// committed after `txn.begin_ts` touched a key or key range the
    /// transaction read or wrote.
    fn conflict(&self, txn: &Txn, seen: &HashMap<Vec<u8>, TxnTimeStamp>) -> Option<String> {
        let newer = |key: &[u8]| seen.get(key).is_some_and(|ts| *ts > txn.begin_ts);
        for (key, _) in &txn.writes {
            if newer(key) {
                return Some(format!(
                    "key {} written by a newer transaction (begin_ts={})",
                    key_text(key),
                    txn.begin_ts
                ));
            }
        }
        for key in &txn.deletes {
            if newer(key) {
                return Some(format!(
                    "key {} deleted by a newer transaction (begin_ts={})",
                    key_text(key),
                    txn.begin_ts
                ));
            }
        }
        for key in &txn.reads.keys {
            if newer(key) {
                return Some(format!(
                    "key {} read by this transaction changed (begin_ts={})",
                    key_text(key),
                    txn.begin_ts
                ));
            }
        }
        for prefix in &txn.reads.prefixes {
            if seen
                .iter()
                .any(|(key, ts)| *ts > txn.begin_ts && key.starts_with(prefix))
            {
                return Some(format!(
                    "range {} read by this transaction changed (begin_ts={})",
                    key_text(prefix),
                    txn.begin_ts
                ));
            }
        }
        None
    }

    /// Commits: validate against newer transactions, append the typed command
    /// list to the WAL (serial), then apply the batch atomically to the KV.
    /// Returns the commit timestamp.
    pub fn commit(&self, txn: Txn) -> Result<TxnTimeStamp> {
        // C++ distinguishes a read-only transaction (no WAL record, commit ts
        // `current_ts + 1`, `GetReadCommitTS`) from an empty one.
        let read_only = txn.is_empty();
        if read_only && txn.reads.keys.is_empty() && txn.reads.prefixes.is_empty() {
            return Err(Error::Txn("empty transaction".into()));
        }
        if read_only {
            let _commit = self.commit_lock.lock().unwrap();
            return Ok(self.current_ts() + 1);
        }
        // `NewTxnManager::GetWriteCommitTS`: `prepare_commit_ts_ += 2`, taken
        // *before* the conflict check, so a rolled-back transaction still spends
        // its timestamp - the reference never hands the same value out twice.
        let commit_ts = self.reserve_commit_ts();
        self.commit_at(txn, commit_ts)
    }

    /// Takes the next write commit timestamp without committing anything.
    ///
    /// A caller that has to stamp the bytes it is about to write with the
    /// commit timestamp (the catalog keys of a DDL carry it, as the
    /// reference's `KeyEncode::CatalogDbKey(db, commit_ts)` does) reserves the
    /// value here and hands it back through [`Self::commit_at`]. The reference
    /// has the same split: the transaction's store is filled before the
    /// timestamp exists, and `GetWriteCommitTS` is what pins it.
    pub fn reserve_commit_ts(&self) -> TxnTimeStamp {
        self.prepare_commit_ts.fetch_add(2, Ordering::SeqCst) + 2
    }

    /// Commits a write transaction whose commit timestamp was already taken by
    /// [`Self::reserve_commit_ts`].
    ///
    /// An empty transaction is valid here, unlike [`Self::commit`]: the
    /// reference opens a transaction for every statement and commits it even
    /// when the statement wrote nothing (`CREATE DATABASE … IF NOT EXISTS` on
    /// an existing name stages no store but still takes a write commit ts,
    /// `NewTxn::Commit` -> `GetWriteCommitTS`), so the timestamp is spent and
    /// the log keeps the entry.
    pub fn commit_at(&self, txn: Txn, commit_ts: TxnTimeStamp) -> Result<TxnTimeStamp> {
        let _commit = self.commit_lock.lock().unwrap();
        {
            let seen = self.write_ts.lock().unwrap();
            if let Some(reason) = self.conflict(&txn, &seen) {
                // The timestamp is spent; `CommitBottom` advances `current_ts_`
                // past it in commit-ts order even when the transaction rolled
                // back, so the clock keeps the same hole.
                self.current_ts.fetch_max(commit_ts, Ordering::SeqCst);
                return Err(Error::TxnConflict(reason));
            }
        }
        let entry = WalEntry::new(
            txn.id as i64,
            commit_ts,
            build_commands(&txn.writes, &txn.deletes),
        );
        {
            let mut wal = self.wal.lock().unwrap();
            wal.append(&entry)?;
            self.last_appended.fetch_max(commit_ts, Ordering::SeqCst);
        }
        let mut writes: Vec<(Vec<u8>, Option<Vec<u8>>)> = txn
            .writes
            .iter()
            .cloned()
            .map(|(k, v)| (k, Some(v)))
            .collect();
        for key in &txn.deletes {
            writes.push((key.clone(), None));
        }
        // The applied-timestamp marker rides along with the batch, so a replay
        // after a reopen never re-applies records that are already in the KV.
        let applied = self.max_applied.fetch_max(commit_ts, Ordering::SeqCst);
        let applied = applied.max(commit_ts);
        writes.push((
            APPLIED_TS_KEY.to_vec(),
            Some(applied.to_le_bytes().to_vec()),
        ));
        apply_split(self.store.as_ref(), self.catalog_store.as_ref(), writes)?;
        {
            let mut seen = self.write_ts.lock().unwrap();
            for (key, _) in &txn.writes {
                seen.insert(key.clone(), commit_ts);
            }
            for key in &txn.deletes {
                seen.insert(key.clone(), commit_ts);
            }
        }
        // `CommitBottom` / `FinalizeTxn`: the clock moves only once the batch is
        // in the store, so a reader's `begin_ts` is `current_ts + 1` and never
        // sees a commit that is still being applied.
        self.current_ts.store(commit_ts, Ordering::SeqCst);
        self.total_committed.fetch_add(1, Ordering::Relaxed);
        Ok(commit_ts)
    }

    /// C++: `NewTxnManager::RollBackTxn`. Drops the transaction's writes and
    /// releases its registry slot and conflict bookkeeping, so a later
    /// transaction neither sees it as active nor conflicts with it.
    pub fn rollback(&self, txn: Txn) -> Result<()> {
        if let Ok(mut active) = self.active.lock() {
            active.remove(&txn.id);
        }
        self.total_rolled_back.fetch_add(1, Ordering::Relaxed);
        // `txn` is dropped here; its `Drop` removes the slot a second time,
        // which is a no-op.
        drop(txn);
        Ok(())
    }

    /// C++: `NewTxnManager::ActiveTxnCount`.
    pub fn active_txn_count(&self) -> usize {
        self.active.lock().unwrap().len()
    }

    /// C++: `NewTxnManager::GetTxnInfoArray` (`SHOW TRANSACTIONS`).
    pub fn get_txn_info_array(&self) -> Vec<TxnInfo> {
        let mut out: Vec<TxnInfo> = self.active.lock().unwrap().values().cloned().collect();
        out.sort_by_key(|info| info.txn_id);
        out
    }

    /// C++: `NewTxnManager::GetTxnInfoByID`.
    pub fn get_txn_info_by_id(&self, txn_id: u64) -> Option<TxnInfo> {
        self.active.lock().unwrap().get(&txn_id).cloned()
    }

    /// C++: `NewTxnManager::GetOldestAliveTS` — the oldest begin timestamp
    /// among live transactions (0 when idle).
    pub fn oldest_alive_ts(&self) -> TxnTimeStamp {
        self.active
            .lock()
            .unwrap()
            .values()
            .map(|info| info.begin_ts)
            .min()
            .unwrap_or(0)
    }

    /// C++: `NewTxnManager::total_committed_txn_count`.
    pub fn total_committed_txn_count(&self) -> u64 {
        self.total_committed.load(Ordering::Relaxed)
    }

    /// C++: `NewTxnManager::total_rollbacked_txn_count`.
    pub fn total_rollbacked_txn_count(&self) -> u64 {
        self.total_rolled_back.load(Ordering::Relaxed)
    }

    /// C++: `NewTxnManager::current_transaction_id` — the id the next `begin`
    /// hands out.
    pub fn current_transaction_id(&self) -> u64 {
        self.next_txn_id.load(Ordering::Relaxed)
    }

    /// C++: `NewTxnManager::CurrentTS` — the timestamp of the last commit.
    pub fn current_ts(&self) -> TxnTimeStamp {
        self.current_ts.load(Ordering::Relaxed)
    }

    /// C++: `NewTxnManager::PrepareCommitTS` — the high-water mark committers
    /// take from. It sits one above `CurrentTS` while a write transaction is
    /// between `GetWriteCommitTS` and `CommitBottom`, and equal to it otherwise.
    pub fn prepare_commit_ts(&self) -> TxnTimeStamp {
        self.prepare_commit_ts.load(Ordering::Relaxed)
    }

    /// Replays the WAL into the KV store for all records after `checkpoint_ts`.
    /// Each record is decoded through [`WalEntryIterator::make`] and its typed
    /// commands are applied. Idempotent: re-applying is safe.
    pub fn recover(&self, checkpoint_ts: TxnTimeStamp) -> Result<usize> {
        let mut applied = 0;
        for path in segment_paths(&self.wal_path)? {
            let mut iter = WalEntryIterator::make(&path, false)?;
            while iter.has_next() {
                let Some(entry) = iter.next() else {
                    break; // torn / corrupt tail
                };
                if entry.commit_ts <= checkpoint_ts {
                    continue;
                }
                let mut writes = Vec::new();
                for cmd in &entry.cmds {
                    match cmd {
                        WalCmd::Append {
                            db_name,
                            table_name,
                            row_ranges,
                            block,
                            ..
                        } => writes.extend(
                            append_writes(db_name, table_name, row_ranges, block)?
                                .into_iter()
                                .map(|(k, v)| (k, Some(v))),
                        ),
                        WalCmd::Delete {
                            db_name,
                            table_name,
                            row_ids,
                            ..
                        } => writes.extend(
                            delete_writes(db_name, table_name, row_ids)
                                .into_iter()
                                .map(|(k, v)| (k, Some(v))),
                        ),
                        WalCmd::KvBatch {
                            kind: KvBatchKind::Delete,
                            writes: batch,
                        } => writes.extend(batch.iter().map(|(k, _)| (k.clone(), None))),
                        WalCmd::KvBatch { writes: batch, .. } => {
                            writes.extend(batch.iter().cloned().map(|(k, v)| (k, Some(v))))
                        }
                        WalCmd::Checkpoint { .. } => {}
                        other => {
                            // The reference's log carries its catalog DDL
                            // (`WalCmdCreateDatabaseV2`, `CreateTableV2`,
                            // `CreateIndexV2`, …). Their effect is already in
                            // the C++ key space this engine opens the catalog
                            // from (`Engine::open` -> the C++ layout reader),
                            // and replaying them needs catalog operations the
                            // executor owns, so skip them: aborting here made a
                            // directory the reference had just written
                            // unopenable ("cannot replay typed command
                            // CreateDatabaseV2 without the executor"). Only the
                            // row commands above have to reach the KV store.
                            let _ = other.command_type();
                        }
                    }
                }
                writes.push((
                    APPLIED_TS_KEY.to_vec(),
                    Some(entry.commit_ts.to_le_bytes().to_vec()),
                ));
                apply_split(self.store.as_ref(), self.catalog_store.as_ref(), writes)?;
                self.max_applied
                    .fetch_max(entry.commit_ts, Ordering::SeqCst);
                // `CommitReplayTxn`: a replayed entry moves both pointers to its
                // own commit ts, so the live clock resumes above it.
                self.current_ts.fetch_max(entry.commit_ts, Ordering::SeqCst);
                self.prepare_commit_ts
                    .fetch_max(entry.commit_ts, Ordering::SeqCst);
                applied += 1;
            }
        }
        Ok(applied)
    }

    /// The highest commit timestamp already applied to the KV store.
    pub fn applied_ts(&self) -> Result<TxnTimeStamp> {
        Ok(self
            .store
            .get(APPLIED_TS_KEY)?
            .and_then(|v| {
                v.get(..8)
                    .map(|b| TxnTimeStamp::from_le_bytes(b.try_into().unwrap()))
            })
            .unwrap_or(0))
    }

    /// Re-publishes the checkpoint frame so it covers `max_commit_ts`, without
    /// spending a timestamp.
    ///
    /// A reader derives its `begin_ts` from the newest checkpoint frame and
    /// hides a catalog record stamped above it (`SegmentIndexMeta::LoadChunkIDs1`
    /// skips a chunk whose ts is greater). A `DumpIndex` commits after the last
    /// checkpoint and stamps the chunk records it sealed, so the frame has to be
    /// advanced for those chunks to stay readable. This writes no transaction of
    /// its own - the sequence of allocated timestamps must not move.
    pub fn cover_with_checkpoint(&self, max_commit_ts: TxnTimeStamp) -> Result<()> {
        let _commit = self.commit_lock.lock().unwrap();
        let current = self.current_ts.load(Ordering::SeqCst);
        if max_commit_ts < current {
            return Ok(());
        }
        // The frame's own commit ts is the clock's current position: this is not
        // a transaction of its own, so it must not move the clock (a frame
        // stamped ahead of it would make the next open start past the WAL).
        self.write_checkpoint_frame(max_commit_ts, max_commit_ts + 1)
    }

    /// Writes the `wal.log` checkpoint frame (`commit_ts` = the frame's own
    /// stamp, `max_commit_ts` = the newest transaction it covers) and fsyncs it.
    fn write_checkpoint_frame(
        &self,
        commit_ts: TxnTimeStamp,
        max_commit_ts: TxnTimeStamp,
    ) -> Result<()> {
        let entry = WalEntry::new(-1, commit_ts, vec![WalCmd::Checkpoint { max_commit_ts }]);
        let mut buf = Vec::with_capacity(entry.size_in_bytes());
        entry.write_adv(&mut buf);
        if let Some(parent) = self.checkpoint_path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        use std::io::Write;
        let mut file = std::fs::File::create(&self.checkpoint_path)
            .map_err(|e| Error::io(&self.checkpoint_path, e))?;
        file.write_all(&buf)
            .map_err(|e| Error::io(&self.checkpoint_path, e))?;
        file.sync_all()
            .map_err(|e| Error::io(&self.checkpoint_path, e))?;
        Ok(())
    }

    /// C++ `NewCheckpoint`: append the checkpoint entry the reference keeps.
    ///
    /// `GetReplayEntries` derives `last_commit_ts`/`max_checkpoint_ts` from the
    /// newest `kNewCheckpoint` command and refuses to open a writable directory
    /// that has data but no checkpoint ("WAL replay: No checkpoint found in
    /// wal", `wal_manager_impl.cpp:670`). The entry therefore has to survive the
    /// truncation that follows it, and the log must never be left empty.
    pub fn append_checkpoint(&self, max_commit_ts: TxnTimeStamp) -> Result<()> {
        let _commit = self.commit_lock.lock().unwrap();
        // The caller usually passes the KV's applied marker, which stays 0
        // until a recovery runs; the manager's own high-water marks are always
        // current.
        let floor = max_commit_ts
            .max(self.max_applied.load(Ordering::SeqCst))
            .max(self.last_appended.load(Ordering::SeqCst))
            .max(self.current_ts.load(Ordering::SeqCst))
            // The startup reservation already puts the clock at `current + 2`,
            // and the reference's startup checkpoint is written at `current +
            // 4` even when the directory holds nothing yet.
            .max(self.prepare_commit_ts.load(Ordering::SeqCst));
        if floor == 0 {
            return Ok(());
        }
        // A checkpoint is a write transaction in the reference
        // (`NewTxn::Checkpoint`, `TransactionType::kNewCheckpoint`), so it
        // spends a timestamp like any other: `BeginTxnShared` sets the begin ts
        // to `current_ts_ + 1` and the commit takes `prepare_commit_ts_ += 2`.
        // The command carries the **begin** ts (`checkpoint_ts =
        // txn_context_ptr_->begin_ts_`), which is odd, and `NewTxn::Checkpoint`
        // rejects an even one outright ("last checkpoint ts isn't correct",
        // `new_txn_impl.cpp:1754`); a directory whose checkpoint frame names an
        // even ts makes the reference abort on startup. The frame's own commit
        // ts is the even value one above it.
        let begin_ts = self.current_ts.load(Ordering::SeqCst) + 1;
        let commit_ts = self.prepare_commit_ts.fetch_add(2, Ordering::SeqCst) + 2;
        let entry = WalEntry::new(
            -1,
            commit_ts,
            vec![WalCmd::Checkpoint {
                max_commit_ts: begin_ts,
            }],
        );
        let mut buf = Vec::with_capacity(entry.size_in_bytes());
        entry.write_adv(&mut buf);
        // `wal.log` is the reference's file: one checkpoint entry, nothing this
        // engine would have to be understood for.
        // The directory may not exist yet: tests open a bare path, and the
        // first checkpoint is what creates the file.
        if let Some(parent) = self.checkpoint_path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        // Written and fsynced: this file is the only thing that tells the
        // reference which checkpoint the directory holds, and it refuses to
        // open a directory whose checkpoint entry a power loss took away.
        {
            use std::io::Write;
            let mut file = std::fs::File::create(&self.checkpoint_path)
                .map_err(|e| Error::io(&self.checkpoint_path, e))?;
            file.write_all(&buf)
                .map_err(|e| Error::io(&self.checkpoint_path, e))?;
            file.sync_all()
                .map_err(|e| Error::io(&self.checkpoint_path, e))?;
        }
        // The checkpoint transaction is committed: its commit ts is the clock's
        // new position, exactly as `CommitBottom` leaves `current_ts_`.
        self.current_ts.store(commit_ts, Ordering::SeqCst);
        Ok(())
    }

    /// Drops the WAL prefix that the KV store already holds. The caller must
    /// pass a timestamp whose records have been fsynced (`flush()` first), so a
    /// crash can never lose a record that this call discards. `meta:applied_ts`
    /// keeps recovery correct; this only bounds the log's size, as the
    /// reference does at a checkpoint.
    pub fn truncate_wal_up_to(&self, applied: TxnTimeStamp) -> Result<bool> {
        let applied = applied.min(self.synced.load(Ordering::SeqCst));
        if applied == 0 {
            return Ok(false);
        }
        let mut wal = self.wal.lock().unwrap();
        // Rotate a full active log (O(1)), then drop every sealed segment whose
        // records are all applied. This keeps the log's footprint bounded
        // without ever copying the remaining tail.
        const SEGMENT_BYTES: u64 = 64 * 1024 * 1024;
        if wal.file_size()? >= SEGMENT_BYTES {
            let seal = self.last_appended.load(Ordering::SeqCst).max(applied);
            wal.rotate(seal)?;
        }
        let mut dropped = false;
        for (path, seal_ts) in sealed_segments(&self.wal_path)? {
            if seal_ts <= applied {
                std::fs::remove_file(&path).map_err(|e| Error::io(&path, e))?;
                dropped = true;
            }
        }
        Ok(dropped)
    }

    pub fn flush(&self) -> Result<()> {
        self.wal.lock().unwrap().flush()?;
        self.store.flush()?;
        // Everything the KV holds is now durable, so the log up to this point
        // can be dropped by the next checkpoint. The checkpoint entry itself is
        // appended by the caller that owns the checkpoint (`flush_storage`,
        // `checkpoint_wal`): a checkpoint is a write transaction in the
        // reference and therefore spends a timestamp, so it must not be
        // appended twice for one flush.
        let applied = self.max_applied.load(Ordering::SeqCst);
        self.synced.fetch_max(applied, Ordering::SeqCst);
        Ok(())
    }

    /// Highest commit timestamp that the KV store has fsynced.
    pub fn synced_ts(&self) -> TxnTimeStamp {
        self.synced.load(Ordering::SeqCst)
    }

    pub fn wal_path(&self) -> &PathBuf {
        &self.wal_path
    }
}

// ---------------------------------------------------------------------------
// Commit -> typed commands
// ---------------------------------------------------------------------------

/// Splits `row:{table}:{seq}` / `del:{table}:{seq}` into `(table, seq)`.
/// Applies a transaction's writes to the two stores it can span.
///
/// A row command (`row:`/`del:`, the keys [`build_commands`] turns into
/// `Append`/`Delete`) belongs to the row store; everything else is a catalog
/// record and belongs to the catalog store - the same split the reference makes
/// between its block files and `kv_instance_`. The replay marker
/// (`meta:applied_ts`) also belongs to the row store, because it says how much
/// of the row WAL is in it. Both halves are written as one atomic batch each,
/// rows first: a crash between them replays the catalog records from the WAL
/// into the catalog store, which is idempotent.
fn apply_split(
    store: &dyn MetaStore,
    catalog_store: &dyn MetaStore,
    writes: Vec<(Vec<u8>, Option<Vec<u8>>)>,
) -> Result<()> {
    let mut rows: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
    let mut catalog: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
    for (key, value) in writes {
        let is_row = parse_row_key(&key, ROW_PREFIX).is_some()
            || parse_row_key(&key, DEL_PREFIX).is_some()
            || key == APPLIED_TS_KEY;
        if is_row {
            // A row tombstone is a **record** in the row store
            // (`del:<table>:<seq>` => "1"), so a delete keeps its marker. A
            // catalog delete is the absence of the key: the reference's
            // `KVInstance::Delete` removes the record, and leaving a "1" behind
            // would resurrect it as a record with that value.
            rows.push((key, Some(value.unwrap_or_else(|| b"1".to_vec()))));
        } else {
            catalog.push((key, value));
        }
    }
    if !rows.is_empty() {
        store.atomic_batch(&rows)?;
    }
    if !catalog.is_empty() {
        catalog_store.atomic_batch(&catalog)?;
    }
    Ok(())
}

fn parse_row_key<'a>(key: &'a [u8], prefix: &str) -> Option<(&'a str, u64)> {
    let s = std::str::from_utf8(key).ok()?;
    let rest = s.strip_prefix(prefix)?;
    let (table, seq) = rest.rsplit_once(':')?;
    if table.is_empty() {
        return None;
    }
    Some((table, seq.parse().ok()?))
}

/// The engine's row key for `table`/`seq` (`row:{table}:{seq:016}`).
fn row_key(table: &str, seq: u64) -> Vec<u8> {
    format!("row:{table}:{seq:016}").into_bytes()
}

/// The engine's tombstone key for `table`/`seq` (`del:{table}:{seq:016}`).
fn del_key(table: &str, seq: u64) -> Vec<u8> {
    format!("del:{table}:{seq:016}").into_bytes()
}

/// `(db_name, table_name)` out of the encoded `db.table` key part.
fn split_table(table: &str) -> (String, String) {
    match table.split_once('.') {
        Some((db, name)) => (db.to_string(), name.to_string()),
        None => (String::new(), table.to_string()),
    }
}

/// Inverse of [`split_table`]: the engine's `db.table` key part.
fn join_table(db_name: &str, table_name: &str) -> String {
    if db_name.is_empty() {
        table_name.to_string()
    } else {
        format!("{db_name}.{table_name}")
    }
}

/// Classifies a txn's KV writes + tombstones into typed WAL commands.
///
/// Row records become one [`WalCmd::Append`] per contiguous table run (the
/// stored row text occupies a single varchar column of the block, because the
/// engine persists rows as encoded text, not columnar vectors) and
/// [`WalCmd::Delete`] carries the row ids. Everything else keeps its exact
/// records in a [`WalCmd::KvBatch`].
fn build_commands(writes: &[(Vec<u8>, Vec<u8>)], deletes: &[Vec<u8>]) -> Vec<WalCmd> {
    struct AppendGroup {
        table: String,
        rows: Vec<(u64, String)>,
    }

    let mut groups: Vec<AppendGroup> = Vec::new();
    let mut kv: Vec<(Vec<u8>, Vec<u8>)> = Vec::new();
    let mut kv_kind = KvBatchKind::Other;

    for (key, value) in writes {
        match (parse_row_key(key, ROW_PREFIX), std::str::from_utf8(value)) {
            (Some((table, seq)), Ok(text)) => match groups.last_mut() {
                Some(group) if group.table == table => group.rows.push((seq, text.to_string())),
                _ => groups.push(AppendGroup {
                    table: table.to_string(),
                    rows: vec![(seq, text.to_string())],
                }),
            },
            _ => {
                kv_kind = batch_kind(key);
                kv.push((key.clone(), value.clone()));
            }
        }
    }

    let mut cmds: Vec<WalCmd> = groups
        .into_iter()
        .map(|group| build_append(&group.table, &group.rows))
        .collect();

    let mut del_groups: Vec<(String, Vec<u64>)> = Vec::new();
    // Non-row deletes are catalog records that must disappear from the KV (the
    // layout sweep drops a dropped table's keys, the counters and
    // `meta|version`). They travel as their own `KvBatch` so replay removes the
    // keys instead of writing the placeholder value.
    let mut kv_deletes: Vec<(Vec<u8>, Vec<u8>)> = Vec::new();
    for key in deletes {
        match parse_row_key(key, DEL_PREFIX) {
            Some((table, seq)) => match del_groups.last_mut() {
                Some((name, ids)) if name == table => ids.push(seq),
                _ => del_groups.push((table.to_string(), vec![seq])),
            },
            None => kv_deletes.push((key.clone(), b"1".to_vec())),
        }
    }
    for (table, seqs) in del_groups {
        let (db_name, table_name) = split_table(&table);
        cmds.push(WalCmd::Delete {
            db_name,
            db_id: String::new(),
            table_name,
            table_id: String::new(),
            row_ids: seqs.into_iter().map(WalRowId::from_u64).collect(),
        });
    }

    if !kv.is_empty() {
        cmds.push(WalCmd::KvBatch {
            kind: kv_kind,
            writes: kv,
        });
    }
    if !kv_deletes.is_empty() {
        cmds.push(WalCmd::KvBatch {
            kind: KvBatchKind::Delete,
            writes: kv_deletes,
        });
    }
    cmds
}

/// Catalog / DDL keys of the engine's KV (`infinity-catalog`).
fn batch_kind(key: &[u8]) -> KvBatchKind {
    const CATALOG: [&[u8]; 5] = [b"db:", b"tbl:", b"tblname:", b"idx:", b"catalog|"];
    if CATALOG.iter().any(|p| key.starts_with(p)) {
        KvBatchKind::Catalog
    } else {
        KvBatchKind::Other
    }
}

/// C++: `WalCmdAppendV2` — `row_ranges_` plus the `DataBlock`. The block holds
/// one varchar column with the engine's encoded row text.
fn build_append(table: &str, rows: &[(u64, String)]) -> WalCmd {
    let mut block = DataBlock::new(vec![Column::Str(ColumnVector::new())], rows.len());
    for (_, text) in rows {
        block
            .append_value(0, BlockValue::Str(text.clone()))
            .expect("block capacity matches the row count");
    }
    block.finalize();
    // Contiguous row ids are one range, as the reference writes them.
    let mut row_ranges: Vec<(WalRowId, u64)> = Vec::new();
    for (seq, _) in rows {
        match row_ranges.last_mut() {
            Some((base, count)) if base.to_u64() + *count == *seq => *count += 1,
            _ => row_ranges.push((WalRowId::from_u64(*seq), 1)),
        }
    }
    let (db_name, table_name) = split_table(table);
    WalCmd::Append {
        db_name,
        db_id: String::new(),
        table_name,
        table_id: String::new(),
        row_ranges,
        block,
    }
}

/// Replay of [`WalCmd::Append`]: row keys and their stored row text.
fn append_writes(
    db_name: &str,
    table_name: &str,
    row_ranges: &[(WalRowId, u64)],
    block: &DataBlock,
) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
    let table = join_table(db_name, table_name);
    let mut out = Vec::new();
    let mut row = 0usize;
    for (base, count) in row_ranges {
        let base_seq = base.to_u64();
        for offset in 0..*count {
            let Some(text) = row_text(block, row) else {
                return Err(Error::Wal(format!(
                    "append command for {table} is missing row {row} of its block"
                )));
            };
            out.push((row_key(&table, base_seq + offset), text.into_bytes()));
            row += 1;
        }
    }
    Ok(out)
}

/// The stored text of one block row: the columns' rendered values joined by
/// tabs, which is the form the engine keeps in the KV store
/// (`Engine::encode_row_stored`).
///
/// This engine's own append records carry that text in a single varchar column
/// (`build_append`), so the one-column case returns the blob verbatim; the
/// reference's `WalCmdAppendV2` carries the table's real typed columns instead
/// (`DataBlock::WriteAdv`), so they have to be rendered back.
fn row_text(block: &DataBlock, row: usize) -> Option<String> {
    let mut cells = Vec::with_capacity(block.column_count());
    for column in 0..block.column_count() {
        cells.push(cell_text(&block.get_value(column, row)?));
    }
    Some(cells.join("\t"))
}

/// `.slt`/storage text of one typed value (`Literal::to_slt_string`).
fn cell_text(value: &BlockValue) -> String {
    match value {
        BlockValue::Null => "null".to_string(),
        BlockValue::Bool(v) => v.to_string(),
        BlockValue::TinyInt(v) => v.to_string(),
        BlockValue::SmallInt(v) => v.to_string(),
        BlockValue::Int32(v) => v.to_string(),
        BlockValue::Int(v) => v.to_string(),
        BlockValue::HugeInt(v) => v.to_string(),
        // Shortest round-trip form of the stored width: an `f32` column keeps
        // `0.1` as `0.1` instead of the widened `0.10000000149011612`.
        BlockValue::Float(v) => v.to_string(),
        BlockValue::Double(v) => v.to_string(),
        BlockValue::Decimal(v) => {
            if v.scale == 0 {
                v.value.to_string()
            } else {
                let divisor = 10u128.pow(v.scale);
                let magnitude = v.value.unsigned_abs();
                format!(
                    "{}{}.{:0width$}",
                    if v.value < 0 { "-" } else { "" },
                    magnitude / divisor,
                    magnitude % divisor,
                    width = v.scale as usize
                )
            }
        }
        BlockValue::Str(v) => v.clone(),
        BlockValue::Date(v) => infinity_storage::cpp_format::days_to_date(v.value),
        BlockValue::Time(v) => infinity_storage::cpp_format::secs_to_time(v.value),
        BlockValue::DateTime(v) => format!(
            "{} {}",
            infinity_storage::cpp_format::days_to_date(v.date),
            infinity_storage::cpp_format::secs_to_time(v.time)
        ),
        BlockValue::Timestamp(v) => format!(
            "{} {}",
            infinity_storage::cpp_format::days_to_date(v.date),
            infinity_storage::cpp_format::secs_to_time(v.time)
        ),
        BlockValue::Interval(v) => v.value.to_string(),
        BlockValue::Uuid(v) => v.iter().map(|b| format!("{b:02x}")).collect(),
        BlockValue::Embedding(v) => format!(
            "[{}]",
            v.iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join(",")
        ),
        BlockValue::Array(v) => format!(
            "[{}]",
            v.iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join(",")
        ),
        BlockValue::Mixed(v) => v.text(),
        BlockValue::Json(v) => v.compact(),
        BlockValue::RowId(v) => format!("{}:{}", v.segment_id, v.segment_offset),
    }
}

/// Replay of [`WalCmd::Delete`]: the tombstones the commit wrote.
fn delete_writes(db_name: &str, table_name: &str, row_ids: &[WalRowId]) -> Vec<(Vec<u8>, Vec<u8>)> {
    let table = join_table(db_name, table_name);
    row_ids
        .iter()
        .map(|row_id| (del_key(&table, row_id.to_u64()), b"1".to_vec()))
        .collect()
}

/// Keys a WAL entry wrote, used to rebuild the conflict state on open.
fn record_write_keys(entry: &WalEntry) -> Vec<Vec<u8>> {
    let mut keys = Vec::new();
    for cmd in &entry.cmds {
        match cmd {
            WalCmd::Append {
                db_name,
                table_name,
                row_ranges,
                ..
            } => {
                let table = join_table(db_name, table_name);
                for (base, count) in row_ranges {
                    let base_seq = base.to_u64();
                    for offset in 0..*count {
                        keys.push(row_key(&table, base_seq + offset));
                    }
                }
            }
            WalCmd::Delete {
                db_name,
                table_name,
                row_ids,
                ..
            } => {
                let table = join_table(db_name, table_name);
                for row_id in row_ids {
                    keys.push(del_key(&table, row_id.to_u64()));
                }
            }
            WalCmd::KvBatch { writes, .. } => keys.extend(writes.iter().map(|(k, _)| k.clone())),
            _ => {}
        }
    }
    keys
}

fn key_text(key: &[u8]) -> String {
    String::from_utf8_lossy(key).into_owned()
}

/// Sealed WAL segments next to `wal_path`, as `(path, seal timestamp)`, oldest
/// first. `rotate()` names them `wal.log.{seal_ts}`.
fn sealed_segments(wal_path: &std::path::Path) -> Result<Vec<(PathBuf, TxnTimeStamp)>> {
    let dir = wal_path
        .parent()
        .unwrap_or_else(|| std::path::Path::new("."));
    let mut out = Vec::new();
    if let Ok(entries) = std::fs::read_dir(dir) {
        for entry in entries.flatten() {
            let name = entry.file_name().to_string_lossy().to_string();
            if let Some(ts) = name
                .strip_prefix("wal.rust.")
                .or_else(|| name.strip_prefix("wal.log."))
                .and_then(|t| t.parse::<TxnTimeStamp>().ok())
            {
                out.push((entry.path(), ts));
            }
        }
    }
    out.sort_by_key(|(_, ts)| *ts);
    Ok(out)
}

/// Every WAL file in replay order: the sealed segments (oldest first), the
/// reference's active `wal.log`, then this engine's active `wal.rust`.
///
/// A directory written by the reference has `wal.log`/`wal.log.{ts}` and no
/// `wal.rust`; one written by this engine has the mirror image (plus a
/// `wal.log` that only carries a checkpoint entry). Reading both means a
/// directory either engine crashed on replays the records it did not flush.
fn segment_paths(wal_path: &std::path::Path) -> Result<Vec<PathBuf>> {
    let mut out: Vec<PathBuf> = sealed_segments(wal_path)?
        .into_iter()
        .map(|(path, _)| path)
        .collect();
    let reference_log = wal_path.with_file_name("wal.log");
    if reference_log.exists() {
        out.push(reference_log);
    }
    out.push(wal_path.to_path_buf());
    Ok(out)
}

/// Reads every sealed segment in timestamp order, then the active log.
fn read_all_segments(wal_path: &std::path::Path) -> Result<Vec<WalEntry>> {
    let mut out = Vec::new();
    for path in segment_paths(wal_path)? {
        out.extend(WalManager::read_all(&path)?);
    }
    Ok(out)
}

/// Key of the applied-commit-timestamp marker in the row/meta store.
const APPLIED_TS_KEY: &[u8] = b"meta:applied_ts";

#[cfg(test)]
mod tests;

/// Loom concurrency-model test (run with `RUSTFLAGS="--cfg loom"`):
///   RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom
///
/// Models the commit-timestamp allocation state machine: concurrent `fetch_add`
/// must hand out unique, gap-free timestamps regardless of interleaving.
#[cfg(loom)]
mod loom_model {
    use loom::sync::atomic::{AtomicU64, Ordering};
    use loom::sync::Arc;

    #[test]
    fn commit_ts_allocation_is_unique() {
        loom::model(|| {
            let counter = Arc::new(AtomicU64::new(0));
            let handles: Vec<_> = (0..2)
                .map(|_| {
                    let c = counter.clone();
                    loom::thread::spawn(move || {
                        let mut out = Vec::new();
                        for _ in 0..2 {
                            out.push(c.fetch_add(1, Ordering::SeqCst));
                        }
                        out
                    })
                })
                .collect();
            let mut all = Vec::new();
            for h in handles {
                all.extend(h.join().unwrap());
            }
            all.sort_unstable();
            assert_eq!(all, vec![0, 1, 2, 3], "commit timestamps must be unique");
        });
    }
}

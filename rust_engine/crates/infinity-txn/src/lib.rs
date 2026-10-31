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

pub mod conflict;
pub mod state;

pub use conflict::{conflict_between, Conflict, OptimizeEntry, RowKey, TxnOp};
pub use state::{transaction_type_to_str, txn_state_to_str, TransactionType, TxnState};

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result, TxnTimeStamp};
use infinity_storage::data_block::{BlockValue, DataBlock};
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

thread_local! {
    /// The id the current statement already took.
    ///
    /// `NewTxnManager::BeginTxnShared` runs once per statement — a `SELECT`,
    /// `USE` or `SHOW` spends an id without writing a WAL record. The statement
    /// entry point (`Engine::execute`, or a read-only RPC handler) calls
    /// [`TxnManager::begin_statement`], which takes the id; a write path that
    /// then calls [`TxnManager::begin`] reuses it instead of taking a second.
    /// A statement that begins several writes (a `CREATE DATABASE` that
    /// bootstraps `default_db` first) calls `begin_statement` again per write.
    static PENDING_TXN_ID: std::cell::RefCell<Option<u64>> = const { std::cell::RefCell::new(None) };
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
    /// The subset of `deletes` the caller applies itself in the bottom half
    /// (`DeleteInBlock`). They stay in `deletes` so the WAL entry keeps the
    /// reference's `DeleteV2` command, but the commit batch leaves them out so
    /// the row stays visible until the commit publishes.
    deferred_deletes: Vec<Vec<u8>>,
    /// Typed WAL commands a DDL prepared for this transaction (e.g.
    /// `WalCmdCreateTableV2`). When non-empty, commit emits these instead of a
    /// `KvBatch` for the catalog records, so the WAL matches the reference.
    typed_commands: Vec<WalCmd>,
    reads: ReadSet,
    /// What the transaction does, in the reference's terms (`NewTxn`'s
    /// `BaseTxnStore::type_` plus the fields its conflict rules read). The
    /// executor sets it before committing; [`TxnOp::Other`] keeps the key-set
    /// check for statements the matrix has no rule for.
    op: TxnOp,
    /// `TxnContext::last_kv_commit_ts_` / `last_commit_ts_`: the manager's pair
    /// when this transaction began. The conflict check needs them to tell a
    /// transaction that committed *before* this snapshot from one that only
    /// finished its KV batch at the same timestamp
    /// (`NewTxnManager::GetCheckCandidateTxns`).
    last_system_kv_commit_ts: TxnTimeStamp,
    last_system_commit_ts: TxnTimeStamp,
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
    /// Records a tombstone whose KV write the caller performs after the commit
    /// batch (the bottom half). The WAL entry still lists it, so the command
    /// stream matches the reference, which writes the tombstone in
    /// `DeleteInBlock` too.
    pub fn delete_deferred(&mut self, key: impl Into<Vec<u8>>) {
        let key = key.into();
        self.deferred_deletes.push(key.clone());
        self.deletes.push(key);
    }
    /// Records a read of `key`, so a commit by a transaction that overlaps
    /// this one's lifetime invalidates the commit (C++ `NewTxn::GetStore`).
    pub fn read(&mut self, key: impl Into<Vec<u8>>) {
        self.reads.keys.push(key.into());
    }
    /// Sets the typed WAL commands to emit instead of a `KvBatch` for this
    /// transaction's catalog records. The reference emits typed DDL commands
    /// (`WalCmdCreateTableV2`, …); this channel lets a DDL supply them.
    pub fn set_typed_commands(&mut self, cmds: Vec<WalCmd>) {
        self.typed_commands = cmds;
    }
    /// Records a read of every key under `prefix` (C++ `NewTxn` range read).
    pub fn read_prefix(&mut self, prefix: impl Into<Vec<u8>>) {
        self.reads.prefixes.push(prefix.into());
    }
    /// Declares what this transaction does (`NewTxn`'s store type). The
    /// conflict check turns into the reference's per-type matrix instead of the
    /// key-set comparison as soon as it is set.
    pub fn set_op(&mut self, op: TxnOp) {
        self.op = op;
    }
    pub fn op(&self) -> &TxnOp {
        &self.op
    }
    pub fn len(&self) -> usize {
        self.writes.len() + self.deletes.len()
    }
    pub fn is_empty(&self) -> bool {
        self.writes.is_empty() && self.deletes.is_empty() && self.typed_commands.is_empty()
    }
}

impl Drop for Txn {
    fn drop(&mut self) {
        if let Ok(mut active) = self.active.lock() {
            active.remove(&self.id);
        }
    }
}

type WalJob = Box<dyn FnOnce(&mut WalManager) + Send>;

/// The dedicated WAL writer thread.
///
/// The reference does not write frames on the committing thread: it hands the
/// entry to `new_wait_flush_` and a single thread appends it (`NewFlush`,
/// `wal_manager_impl.cpp:89`), which is what keeps the file in commit order
/// without every committer serialising on the file itself. `WalWriter` owns the
/// active log on such a thread; each call is one job and waits for its reply,
/// so the order the caller emits is the order on disk.
struct WalWriter {
    /// `None` once the writer is being shut down, which closes the queue.
    jobs: Option<std::sync::mpsc::Sender<WalJob>>,
    thread: Option<std::thread::JoinHandle<()>>,
    /// How many jobs the thread has run. A checkpoint has to be exactly one
    /// (swap + frame + fsync), so a test can pin that.
    #[cfg(test)]
    ran: Arc<std::sync::atomic::AtomicUsize>,
}

impl Drop for WalWriter {
    /// Closes the queue and joins the thread, so the active `wal.log` is closed
    /// on this thread's schedule and not whenever the OS gets round to dropping
    /// it - a reopen of the same directory must not race the close.
    fn drop(&mut self) {
        self.jobs.take();
        if let Some(thread) = self.thread.take() {
            let _ = thread.join();
        }
    }
}

impl WalWriter {
    fn start(wal: WalManager) -> Self {
        let (jobs, queue) = std::sync::mpsc::channel::<WalJob>();
        #[cfg(test)]
        let ran = Arc::new(std::sync::atomic::AtomicUsize::new(0));
        #[cfg(test)]
        let counter = Arc::clone(&ran);
        let thread = std::thread::Builder::new()
            .name("wal-writer".to_string())
            .spawn(move || {
                let mut wal = wal;
                while let Ok(job) = queue.recv() {
                    #[cfg(test)]
                    counter.fetch_add(1, Ordering::SeqCst);
                    job(&mut wal);
                }
            })
            .expect("spawn the WAL writer");
        Self {
            jobs: Some(jobs),
            thread: Some(thread),
            #[cfg(test)]
            ran,
        }
    }

    /// Runs one operation on the writer and waits for it to finish.
    fn run<T, F>(&self, f: F) -> Result<T>
    where
        T: Send + 'static,
        F: FnOnce(&mut WalManager) -> Result<T> + Send + 'static,
    {
        let (reply, answer) = std::sync::mpsc::channel();
        self.jobs
            .as_ref()
            .ok_or_else(|| Error::Wal("the WAL writer is shut down".into()))?
            .send(Box::new(move |wal| {
                let _ = reply.send(f(wal));
            }))
            .map_err(|_| Error::Wal("the WAL writer stopped".into()))?;
        answer
            .recv()
            .map_err(|_| Error::Wal("the WAL writer stopped".into()))?
    }

    fn append(&self, entry: &WalEntry) -> Result<()> {
        let entry = entry.clone();
        self.run(move |wal| wal.append(&entry).map(|_| ()))
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
    wal: WalWriter,
    /// The catalog ids of the tables this engine knows (`db.tbl` →
    /// `(db_id, table_id)`). The reference stamps them into every `Append` /
    /// `Delete` command (`WalCmdAppend::db_id_` / `table_id_`); a reader that
    /// resolves a table by id needs them, so an empty string is a real loss.
    table_ids: Mutex<HashMap<String, (String, String)>>,
    /// `wal/wal.log`: the **active** log the reference reads. It carries the
    /// transaction records and, after a checkpoint, the `kNewCheckpoint` frame
    /// the checkpoint wrote into the fresh file (the swap keeps the previous
    /// records in `wal.log.<seal_ts>`).
    wal_path: PathBuf,
    /// `WalManager::last_swap_wal_ts_`: the seal timestamp of the last swap, so
    /// a checkpoint with nothing new does not rotate an empty log.
    last_swap: AtomicU64,
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
    /// C++ `NewTxnManager::last_kv_commit_ts_` / `last_commit_ts_`: the pair a
    /// transaction freezes at begin ([`Txn::last_system_kv_commit_ts`]). The KV
    /// timestamp runs one step ahead of the commit ts because the batch is
    /// applied after the commit that produced it, which is the gap the conflict
    /// check has to resolve. Both move together, as they do under the
    /// reference's `locker_`.
    kv_clock: Mutex<(TxnTimeStamp, TxnTimeStamp)>,
    /// C++ `NewTxnManager::check_txn_map_`: every write transaction that took a
    /// commit ts, keyed by that ts, with the store type its conflict rules
    /// read. An entry is pruned once no live transaction can still conflict
    /// with it (`commit_ts < oldest begin_ts`).
    candidates: Mutex<std::collections::BTreeMap<TxnTimeStamp, Candidate>>,
    /// Serialises the checkpoint/cleanup writers, which do not take a commit
    /// ts turn of their own.
    commit_lock: Mutex<()>,
    /// C++ `NewTxnManager::wait_conflict_ck_` / `allocator_map_`: every write
    /// transaction that has taken a commit ts, keyed by that ts. Only the
    /// **smallest** one may run its commit, so the WAL entry, the row-id
    /// allocation and the block the append lands in all follow commit-ts order -
    /// which is what keeps a block's row ids a dense prefix (the reference's
    /// `TxnAllocator` hands ranges out in the same order).
    turns: Mutex<std::collections::BTreeSet<TxnTimeStamp>>,
    turn_cv: std::sync::Condvar,
    /// C++ `NewTxnManager::bottom_txns_`: transactions whose bottom half has
    /// been taken, keyed by commit ts. `true` when the bottom finished, so the
    /// clock can only advance past the **smallest** unfinished ts.
    bottom_txns: Mutex<std::collections::BTreeMap<TxnTimeStamp, bool>>,
    bottom_cv: std::sync::Condvar,
}

/// One entry of `NewTxnManager::check_txn_map_`: what a transaction that took a
/// commit ts does, and whether it has finished.
struct Candidate {
    txn_id: u64,
    begin_ts: TxnTimeStamp,
    op: TxnOp,
    /// `TxnContext::kv_commit_ts_`: when the transaction's KV batch became
    /// visible to new snapshots, or [`UNCOMMIT_TS`] while it is still in flight.
    kv_commit_ts: TxnTimeStamp,
}

/// C++ `UNCOMMIT_TS` (`default_values.cppm:83`): "not yet".
const UNCOMMIT_TS: TxnTimeStamp = TxnTimeStamp::MAX;

/// One write transaction's place in commit-ts order (`NewTxnManager::
/// GetWriteCommitTS` -> `wait_conflict_ck_`).
///
/// Dropping it gives the place back, so a statement that fails between taking
/// the timestamp and committing cannot pin every transaction behind it (the
/// reference's `RemoveMapElementForRollbackNoLock`).
pub struct WriteTurn<'a> {
    manager: &'a TxnManager,
    ts: TxnTimeStamp,
}

impl WriteTurn<'_> {
    pub fn ts(&self) -> TxnTimeStamp {
        self.ts
    }

    /// Blocks until this transaction is the smallest unfinished commit ts.
    pub fn wait(&self) {
        self.manager.turn_wait(self.ts);
    }
}

impl Drop for WriteTurn<'_> {
    fn drop(&mut self) {
        self.manager.turn_done(self.ts);
    }
}

/// Releases a place in commit-ts order on every exit path of a commit, and -
/// when the commit did not go through - lets the visibility clock move past the
/// spent timestamp, as `RemoveMapElementForRollbackNoLock` does.
struct CommitRelease<'a> {
    manager: &'a TxnManager,
    ts: TxnTimeStamp,
    committed: &'a std::cell::Cell<bool>,
}

impl Drop for CommitRelease<'_> {
    fn drop(&mut self) {
        if !self.committed.get() {
            self.manager.finish_candidate(self.ts, false);
            self.manager.finish_bottom(self.ts);
        }
        self.manager.turn_done(self.ts);
    }
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
        // One file holds both the checkpoint frame and the transaction records,
        // as the reference's `wal.log` does: `WalManager` swaps it to
        // `wal.log.<seal_ts>` at a checkpoint and opens a fresh `wal.log`. A
        // directory written by an older build of this engine also keeps its
        // records in `wal.rust`; `segment_paths` still reads that.
        let wal_path = wal_path.into();
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
        let manager = Self {
            catalog_store: store.clone(),
            store,
            table_ids: Mutex::new(HashMap::new()),
            wal: WalWriter::start(wal),
            wal_path,
            last_swap: AtomicU64::new(0),
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
            kv_clock: Mutex::new((0, 0)),
            candidates: Mutex::new(std::collections::BTreeMap::new()),
            commit_lock: Mutex::new(()),
            turns: Mutex::new(std::collections::BTreeSet::new()),
            turn_cv: std::sync::Condvar::new(),
            bottom_txns: Mutex::new(std::collections::BTreeMap::new()),
            bottom_cv: std::sync::Condvar::new(),
        };
        // `Storage::Init` -> `RecoverMemIndex` -> `BeginRecoveryTxn`: the
        // reference begins one transaction before its startup checkpoint, so it
        // spends an id there on every open (id 1 on a fresh directory).
        manager.begin_statement();
        manager.end_statement();
        Ok(manager)
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

    /// Takes the id the reference's `BeginTxnShared` takes for one statement.
    ///
    /// The next write on this thread reuses it; a read-only statement simply
    /// leaves it unused, which is exactly what the reference does (its txn map
    /// holds the read-only transaction until it is rolled back).
    pub fn begin_statement(&self) -> u64 {
        let id = self.next_txn_id.fetch_add(1, Ordering::SeqCst);
        PENDING_TXN_ID.with(|pending| *pending.borrow_mut() = Some(id));
        id
    }

    /// The current statement's id, taken if the caller has not taken one yet.
    ///
    /// An RPC that runs several statements (a `CREATE DATABASE` that then
    /// `USE`s it) takes one id per statement, as `BeginTxnShared` does.
    pub fn ensure_statement_id(&self) -> u64 {
        PENDING_TXN_ID.with(|pending| {
            let mut slot = pending.borrow_mut();
            match *slot {
                Some(id) => id,
                None => {
                    let id = self.next_txn_id.fetch_add(1, Ordering::SeqCst);
                    *slot = Some(id);
                    id
                }
            }
        })
    }

    /// Ends the statement scope opened by [`Self::begin_statement`].
    pub fn end_statement(&self) {
        PENDING_TXN_ID.with(|pending| *pending.borrow_mut() = None);
    }

    /// The id the next statement will take (without taking it).
    pub fn next_txn_id(&self) -> u64 {
        self.next_txn_id.load(Ordering::Relaxed)
    }

    /// C++: `NewTxnManager::BeginTxnShared` — registers the transaction in
    /// `txn_map_` so `SHOW TRANSACTIONS` and `active_txn_count` see it.
    pub fn begin_with_text(&self, txn_text: impl Into<String>) -> Txn {
        // `NewTxnManager::BeginTxnShared` is called once per **statement**, so a
        // read-only statement still spends an id (`SELECT`, `USE`, `SHOW …`).
        // The statement's id is taken by `begin_statement` when one is pending;
        // otherwise this call is its own statement.
        let id = PENDING_TXN_ID
            .with(|pending| pending.borrow_mut().take())
            .unwrap_or_else(|| self.next_txn_id.fetch_add(1, Ordering::SeqCst));
        // `NewTxnManager::BeginTxnShared`: `begin_ts = current_ts_ + 1`. It is
        // one past the last completed commit, so it is *below* this
        // transaction's own commit timestamp (which is `current_ts_ + 2`) and
        // above every commit that finished before it began.
        let begin_ts = self.current_ts.load(Ordering::SeqCst) + 1;
        // `BeginTxnShared` hands the transaction the manager's
        // `last_kv_commit_ts_` / `last_commit_ts_` as they are right now.
        let (last_system_kv_commit_ts, last_system_commit_ts) = *self.kv_clock.lock().unwrap();
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
            deferred_deletes: Vec::new(),
            typed_commands: Vec::new(),
            reads: ReadSet::default(),
            op: TxnOp::Other,
            last_system_kv_commit_ts,
            last_system_commit_ts,
            active: self.active.clone(),
        }
    }

    /// C++ `NewTxnManager::CheckConflict1`: the reference's matrix against every
    /// transaction that took an earlier commit ts, falling back to the key-set
    /// comparison for a statement the executor did not classify
    /// ([`TxnOp::Other`]).
    fn conflict(
        &self,
        txn: &Txn,
        commit_ts: TxnTimeStamp,
        seen: &HashMap<Vec<u8>, TxnTimeStamp>,
    ) -> Option<(Conflict, String)> {
        if let Some(conflict) = self.matrix_conflict(txn, commit_ts) {
            return Some(conflict);
        }
        if txn.op != TxnOp::Other {
            // A classified statement is judged by its store type alone: the
            // reference has no row-level `del:` key to compare, and its rules
            // already cover what the key check would have found.
            return None;
        }
        self.key_conflict(txn, seen)
            .map(|reason| (Conflict::Retry, reason))
    }

    /// The reference's `CheckConflictTxnStore` over the candidate map.
    ///
    /// `NewTxnManager::GetCheckCandidateTxns` decides which transactions are
    /// candidates, and the test is on the **KV** commit timestamp, not on the
    /// commit ts: the batch a transaction writes becomes visible a step after
    /// the commit that produced it, so a snapshot can be newer than a commit and
    /// still older than its batch. Three cases at the boundary equal to
    /// `begin_ts`, resolved with the pair the transaction froze at begin
    /// (`last_system_kv_commit_ts` / `last_system_commit_ts`):
    ///
    /// - `other_kv < last_kv`: the batch was already there before this
    ///   transaction began - no conflict;
    /// - `other_kv == last_kv`: the pair is the same one this transaction saw,
    ///   so the order is decided by the commit ts - a commit at or below the
    ///   frozen one happened first and does not conflict;
    /// - `other_kv > last_kv`: the batch appeared after this transaction began -
    ///   conflict.
    fn matrix_conflict(
        &self,
        txn: &Txn,
        commit_ts: TxnTimeStamp,
    ) -> Option<(Conflict, String)> {
        let candidates = self.candidates.lock().unwrap();
        for (ts, candidate) in candidates.iter() {
            if candidate.txn_id == txn.id {
                continue;
            }
            if candidate.kv_commit_ts < txn.begin_ts {
                // Committed (and visible) before this transaction began.
                continue;
            }
            if candidate.kv_commit_ts == txn.begin_ts {
                if candidate.kv_commit_ts < txn.last_system_kv_commit_ts {
                    continue;
                }
                if candidate.kv_commit_ts == txn.last_system_kv_commit_ts
                    && *ts <= txn.last_system_commit_ts
                {
                    continue;
                }
            }
            // A candidate that began after this transaction committed keeps the
            // ones behind it out too: they began even later.
            if candidate.begin_ts > commit_ts {
                break;
            }
            if let Some(conflict) = conflict_between(&txn.op, &candidate.op) {
                return Some((
                    conflict,
                    format!(
                        "transaction {} (commit ts {}) conflicts with {:?}",
                        candidate.txn_id, ts, candidate.op
                    ),
                ));
            }
        }
        None
    }

    /// The key-set check: a key this transaction wrote, deleted or read was
    /// touched after it began.
    fn key_conflict(&self, txn: &Txn, seen: &HashMap<Vec<u8>, TxnTimeStamp>) -> Option<String> {
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

    /// `GetWriteCommitTS` -> `check_txn_map_.emplace`: the transaction becomes a
    /// conflict candidate for everything that is still running.
    fn note_candidate(&self, txn: &Txn, commit_ts: TxnTimeStamp) {
        let oldest = self
            .active
            .lock()
            .unwrap()
            .values()
            .map(|info| info.begin_ts)
            .min()
            .unwrap_or(TxnTimeStamp::MAX);
        // The key log `CleanupTxnBottomNolock` prunes too: a write older than
        // every live transaction's snapshot can never conflict again.
        self.write_ts
            .lock()
            .unwrap()
            .retain(|_, ts| *ts >= oldest);
        let mut candidates = self.candidates.lock().unwrap();
        // A transaction whose KV batch is visible before the oldest live
        // transaction began can no longer conflict with anything
        // (`CleanupTxnBottomNolock` compares `KVCommitTS()` against the oldest
        // `begin_ts`, `new_txn_manager_impl.cpp:687`).
        candidates.retain(|_, candidate| candidate.kv_commit_ts >= oldest);
        candidates.insert(
            commit_ts,
            Candidate {
                txn_id: txn.id,
                begin_ts: txn.begin_ts,
                op: txn.op.clone(),
                // `TxnContext::kv_commit_ts_` starts at `UNCOMMIT_TS`.
                kv_commit_ts: UNCOMMIT_TS,
            },
        );
    }

    /// Registers a transaction the manager drives itself (`Checkpoint`,
    /// `Cleanup`): the reference opens both as write transactions, so they take
    /// a commit ts and take part in the conflict matrix (`Cleanup` versus a
    /// snapshot, `new_txn_impl.cpp:4410-4466`), but there is no `Txn` handle -
    /// by the time the frame is written the transaction is finished.
    fn note_standalone(&self, op: TxnOp, commit_ts: TxnTimeStamp) {
        let oldest = self
            .active
            .lock()
            .unwrap()
            .values()
            .map(|info| info.begin_ts)
            .min()
            .unwrap_or(TxnTimeStamp::MAX);
        let mut candidates = self.candidates.lock().unwrap();
        candidates.retain(|_, candidate| candidate.kv_commit_ts >= oldest);
        candidates.insert(
            commit_ts,
            Candidate {
                // The manager's own transactions have no id of their own in
                // `txn_map_`; `u64::MAX` can never equal a live transaction's.
                txn_id: u64::MAX,
                begin_ts: commit_ts - 1,
                op,
                kv_commit_ts: self.note_kv_commit(commit_ts),
            },
        );
    }

    /// A committed transaction stays a candidate (older snapshots still have to
    /// see it); a rolled-back one never happened.
    fn finish_candidate(&self, commit_ts: TxnTimeStamp, committed: bool) {
        let mut candidates = self.candidates.lock().unwrap();
        match candidates.get_mut(&commit_ts) {
            Some(candidate) if committed => candidate.kv_commit_ts = self.note_kv_commit(commit_ts),
            _ => {
                candidates.remove(&commit_ts);
            }
        }
    }

    /// `NewTxnManager::CommitKVInstance`: the transaction's batch is now
    /// visible, so the manager's `(last_kv_commit_ts_, last_commit_ts_)` pair
    /// moves to it.
    ///
    /// The reference reads `current_ts_ + 1` here, i.e. one past the commit the
    /// batch belongs to; the port applies the batch inside the commit itself,
    /// so the same value is `commit_ts + 1`.
    fn note_kv_commit(&self, commit_ts: TxnTimeStamp) -> TxnTimeStamp {
        let kv_commit_ts = commit_ts + 1;
        *self.kv_clock.lock().unwrap() = (kv_commit_ts, commit_ts);
        kv_commit_ts
    }

    /// Commits: validate against newer transactions, append the typed command
    /// list to the WAL (serial), then apply the batch atomically to the KV.
    /// Returns the commit timestamp.
    /// C++ `NewTxnManager::GetWriteCommitTS`: the transaction takes its place
    /// in the bottom-half queue *before* it commits, so a later transaction
    /// cannot publish its writes first.
    pub fn register_bottom(&self, commit_ts: TxnTimeStamp) {
        self.bottom_txns.lock().unwrap().insert(commit_ts, false);
    }

    /// C++ `NewTxnManager::CommitBottom`: mark this transaction's bottom done
    /// and let the clock move past every finished transaction from the front.
    /// Visibility therefore advances in commit-ts order even when the bottoms
    /// themselves finished the other way round.
    pub fn finish_bottom(&self, commit_ts: TxnTimeStamp) {
        let mut bottom = self.bottom_txns.lock().unwrap();
        if let Some(done) = bottom.get_mut(&commit_ts) {
            *done = true;
        }
        while bottom.iter().next().map_or(false, |(_, done)| *done) {
            let front = *bottom.keys().next().unwrap();
            bottom.remove(&front);
            self.current_ts.fetch_max(front, Ordering::SeqCst);
        }
        drop(bottom);
        self.bottom_cv.notify_all();
    }

    /// Waits until `commit_ts` is visible (`current_ts` has reached it).
    pub fn wait_visible(&self, commit_ts: TxnTimeStamp) {
        let mut bottom = self.bottom_txns.lock().unwrap();
        while self.current_ts.load(Ordering::SeqCst) < commit_ts {
            bottom = self.bottom_cv.wait(bottom).unwrap();
        }
    }

    /// [`Self::commit`] without publishing the commit; the caller calls
    /// [`Self::finish_bottom`] when its bottom work is done.
    pub fn commit_deferred(&self, txn: Txn) -> Result<TxnTimeStamp> {
        let turn = self.reserve_commit_ts();
        let commit_ts = turn.ts();
        self.commit_at_deferred(txn, commit_ts)
    }

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
        let turn = self.reserve_commit_ts();
        let commit_ts = turn.ts();
        self.commit_at(txn, commit_ts)
    }

    /// Takes the next write commit timestamp without committing anything.
    ///
    /// A caller that has to stamp the bytes it is about to write with the
    /// commit timestamp (the catalog keys of a DDL carry it, as the
    /// reference's `KeyEncode::CatalogDbKey(db, commit_ts)` does) reserves the
    /// value here and hands the value back through [`Self::commit_at`]. The
    /// reference has the same split: the transaction's store is filled before
    /// the timestamp exists, and `GetWriteCommitTS` is what pins it.
    ///
    /// The returned [`WriteTurn`] is the transaction's place in commit-ts
    /// order; holding it until the commit (or dropping it, if the statement
    /// fails first) is what keeps every later commit behind this one.
    pub fn reserve_commit_ts(&self) -> WriteTurn<'_> {
        let ts = self.prepare_commit_ts.fetch_add(2, Ordering::SeqCst) + 2;
        self.turns.lock().unwrap().insert(ts);
        WriteTurn { manager: self, ts }
    }

    /// C++ `NewTxnManager::GetWriteCommitTS`: takes the next commit ts **and**
    /// takes the transaction's place in the commit-ts order.
    ///
    /// Every statement that appends rows does this first and runs its commit
    /// (conflict check, row-id allocation, WAL entry) through
    /// [`Self::commit_at_deferred_with`]. Because the place is taken before the
    /// range is allocated (`SubmitForAllocation` in the reference), the ids a
    /// checkpoint sees form a dense prefix of the block. Same thing as
    /// [`Self::reserve_commit_ts`]; the name says why the append path takes it.
    pub fn begin_write(&self) -> WriteTurn<'_> {
        self.reserve_commit_ts()
    }

    /// Waits until every smaller commit ts has committed (or gone away).
    fn turn_wait(&self, commit_ts: TxnTimeStamp) {
        let mut turns = self.turns.lock().unwrap();
        // A ts taken through [`Self::reserve_commit_ts`] joins here, so it is
        // still serialised against the transactions that registered early.
        turns.insert(commit_ts);
        while turns.first().copied() != Some(commit_ts) {
            turns = self.turn_cv.wait(turns).unwrap();
        }
    }

    /// Lets the next commit ts run. Idempotent: the turn is released by the
    /// commit itself and again by [`WriteTurn`]'s `Drop`.
    fn turn_done(&self, commit_ts: TxnTimeStamp) {
        let mut turns = self.turns.lock().unwrap();
        if turns.remove(&commit_ts) {
            drop(turns);
            self.turn_cv.notify_all();
        }
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
    /// Records the catalog ids the reference stamps into `Append`/`Delete`
    /// (`WalCmdAppend::db_id_` / `table_id_`). The caller registers a table as
    /// soon as its catalog record exists, so every later DML entry names it the
    /// way the reference does.
    pub fn register_table_ids(&self, table: &str, db_id: &str, table_id: &str) {
        self.table_ids
            .lock()
            .unwrap()
            .insert(table.to_string(), (db_id.to_string(), table_id.to_string()));
    }

    pub fn commit_at(&self, txn: Txn, commit_ts: TxnTimeStamp) -> Result<TxnTimeStamp> {
        self.commit_at_deferred(txn, commit_ts).map(|ts| {
            self.finish_bottom(ts);
            ts
        })
    }

    /// [`Self::commit_at`] without publishing the commit: the caller finishes
    /// its own bottom work (in the executor: the index updates) and then calls
    /// [`Self::finish_bottom`], so a transaction is visible only once its rows
    /// *and* its index entries are in place.
    pub fn commit_at_deferred(&self, txn: Txn, commit_ts: TxnTimeStamp) -> Result<TxnTimeStamp> {
        self.commit_at_deferred_with(txn, commit_ts, |_| Ok(()))
    }

    /// [`Self::commit_at_deferred`] with a step that runs **after** the conflict
    /// check and **before** the WAL entry is built.
    ///
    /// That is where the reference allocates the append's row range
    /// (`SubmitForAllocation` sits between the conflict check and `SendToWAL`,
    /// `new_txn_manager_impl.cpp:207-314`): the range is part of the log entry,
    /// and a transaction that lost the conflict never spends one. `before_wal`
    /// gets the transaction so it can fill in the command it is about to log.
    pub fn commit_at_deferred_with<F>(
        &self,
        mut txn: Txn,
        commit_ts: TxnTimeStamp,
        before_wal: F,
    ) -> Result<TxnTimeStamp>
    where
        F: FnOnce(&mut Txn) -> Result<()>,
    {
        // `GetWriteCommitTS` -> `wait_conflict_ck_`: only the smallest commit ts
        // may write its entry, so the log (and everything the entry decides:
        // row ids, block slots) follows commit order.
        self.turn_wait(commit_ts);
        // The checkpoint reads `last_appended` to decide whether it has to
        // rotate, so appending the entry and publishing the new high-water mark
        // must be one step with respect to it (`WalManager::Flush` does both in
        // its own loop, `wal_manager_impl.cpp:288-294`). Taken **after** the
        // turn: a checkpoint never waits for a commit's turn.
        let _commit = self.commit_lock.lock().unwrap();
        // Every exit path below (conflict, allocation failure, WAL error) hands
        // the place back **and** lets the clock move past it, so one failed
        // statement cannot stall the commits after it - the reference's
        // `RemoveMapElementForRollbackNoLock` (`new_txn_manager_impl.cpp:888`).
        let committed = std::cell::Cell::new(false);
        let _release = CommitRelease {
            manager: self,
            ts: commit_ts,
            committed: &committed,
        };
        self.register_bottom(commit_ts);
        self.note_candidate(&txn, commit_ts);
        {
            let seen = self.write_ts.lock().unwrap();
            if let Some((kind, reason)) = self.conflict(&txn, commit_ts, &seen) {
                // The timestamp is spent; `CommitBottom` advances `current_ts_`
                // past it in commit-ts order even when the transaction rolled
                // back, so the clock keeps the same hole.
                drop(seen);
                self.finish_candidate(commit_ts, false);
                self.finish_bottom(commit_ts);
                return Err(match kind {
                    Conflict::Retry => Error::TxnConflict(reason),
                    Conflict::NoRetry => Error::TxnConflictNoRetry(reason),
                });
            }
        }
        before_wal(&mut txn)?;
        let entry = WalEntry::new(
            txn.id as i64,
            commit_ts,
            build_commands(
                &txn.writes,
                &txn.deletes,
                &self.table_ids.lock().unwrap(),
                &txn.typed_commands,
            ),
        );
        self.wal.append(&entry)?;
        self.last_appended.fetch_max(commit_ts, Ordering::SeqCst);
        let mut writes: Vec<(Vec<u8>, Option<Vec<u8>>)> = txn
            .writes
            .iter()
            .cloned()
            .map(|(k, v)| (k, Some(v)))
            .collect();
        for key in &txn.deletes {
            // A tombstone is applied by the caller's bottom half
            // (`DeleteInBlock`), not by this batch: the row must stay visible
            // until the commit publishes. It still rides in `deletes` so the
            // entry lists the `DeleteV2` command the reference writes.
            if txn.deferred_deletes.contains(key) {
                continue;
            }
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
        self.total_committed.fetch_add(1, Ordering::Relaxed);
        self.finish_candidate(commit_ts, true);
        committed.set(true);
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
        let mut appends = Vec::new();
        let mut deletes = Vec::new();
        self.recover_into(checkpoint_ts, checkpoint_ts, &mut appends, &mut deletes)
    }

    /// [`Self::recover`], with the two row commands handed back instead of being
    /// written to the KV store.
    ///
    /// The engine keeps its rows in the blocks, not in `row:` keys: an append
    /// that never reached a block is applied by the caller (`AppendInBlock`),
    /// and a delete marks the block's `version`. `block_floor` is how far the
    /// block files already go (the newest ts they carry), which is a *different*
    /// boundary from `checkpoint_ts` - the store's applied mark is written
    /// before the bottom half runs, so a crash in between leaves a commit the
    /// KV has and the block has not.
    pub fn recover_into(
        &self,
        checkpoint_ts: TxnTimeStamp,
        block_floor: TxnTimeStamp,
        appends: &mut Vec<ReplayedAppend>,
        deletes: &mut Vec<ReplayedDelete>,
    ) -> Result<usize> {
        let mut applied = 0;
        for path in segment_paths(&self.wal_path)? {
            let mut iter = WalEntryIterator::make(&path, false)?;
            while iter.has_next() {
                let Some(entry) = iter.next() else {
                    break; // torn / corrupt tail
                };
                let for_kv = entry.commit_ts > checkpoint_ts;
                let for_blocks = entry.commit_ts > block_floor;
                if !for_kv && !for_blocks {
                    continue;
                }
                let mut writes = Vec::new();
                for cmd in &entry.cmds {
                    match cmd {
                        WalCmd::Append {
                            db_name,
                            table_name,
                            table_id,
                            row_ranges,
                            block,
                            ..
                        } => {
                            // Rows live in the block, never in a `row:` key.
                            if for_blocks {
                                appends.push(ReplayedAppend {
                                    table: join_table(db_name, table_name),
                                    table_id: table_id.parse().unwrap_or(u64::MAX),
                                    commit_ts: entry.commit_ts,
                                    rows: append_rows(db_name, table_name, row_ranges, block)?,
                                });
                            }
                        }
                        WalCmd::Delete {
                            db_name,
                            table_name,
                            table_id,
                            row_ids,
                            ..
                        } => {
                            // The tombstone is the block's `version`; the KV
                            // keeps no `del:` key.
                            if for_blocks {
                                deletes.push(ReplayedDelete {
                                    table: join_table(db_name, table_name),
                                    table_id: table_id.parse().unwrap_or(u64::MAX),
                                    commit_ts: entry.commit_ts,
                                    row_ids: row_ids.iter().map(|id| id.to_u64()).collect(),
                                });
                            }
                        }
                        WalCmd::KvBatch {
                            kind: KvBatchKind::Delete,
                            writes: batch,
                        } => {
                            if for_kv {
                                writes.extend(batch.iter().map(|(k, _)| (k.clone(), None)));
                            }
                        }
                        WalCmd::KvBatch { writes: batch, .. } => {
                            if for_kv {
                                writes.extend(batch.iter().cloned().map(|(k, v)| (k, Some(v))))
                            }
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
                            // row commands above have to reach the storage.
                            let _ = other.command_type();
                        }
                    }
                }
                if for_kv {
                    writes.push((
                        APPLIED_TS_KEY.to_vec(),
                        Some(entry.commit_ts.to_le_bytes().to_vec()),
                    ));
                }
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

    /// The replay floor: the highest commit timestamp **both** stores hold.
    ///
    /// The marker is written to both (`apply_split`) and the answer is their
    /// minimum, so a store that lost its memtable to a crash drags the floor
    /// back instead of letting the other one skip the records it lost.
    pub fn applied_ts(&self) -> Result<TxnTimeStamp> {
        let marker = |store: &dyn MetaStore| -> Result<TxnTimeStamp> {
            Ok(store
                .get(APPLIED_TS_KEY)?
                .and_then(|v| {
                    v.get(..8)
                        .map(|b| TxnTimeStamp::from_le_bytes(b.try_into().unwrap()))
                })
                .unwrap_or(0))
        };
        Ok(marker(self.store.as_ref())?.min(marker(self.catalog_store.as_ref())?))
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

    /// Writes a `kNewCheckpoint` frame without rotating the log.
    ///
    /// Used by the dump-index path, which only needs the frame's ts to cover the
    /// chunks it sealed. Appending (rather than creating the file) keeps the
    /// records after the last real checkpoint intact.
    fn write_checkpoint_frame(
        &self,
        commit_ts: TxnTimeStamp,
        max_commit_ts: TxnTimeStamp,
    ) -> Result<()> {
        let entry = WalEntry::new(-1, commit_ts, vec![WalCmd::Checkpoint { max_commit_ts }]);
        // One job, so no commit can slip in between the append and the return.
        // The frame itself is written, not synced: the caller syncs it with
        // [`Self::sync_wal`] once the data it claims is on disk.
        self.wal.run(move |wal| wal.append(&entry).map(|_| ()))
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
        // The reference swaps the file first (`kNewCheckpoint` commit path,
        // `wal_manager_impl.cpp:288`), then writes the frame into the new
        // `wal.log`; the records it covers stay in `wal.log.<seal_ts>`.
        let seal = self.last_appended.load(Ordering::SeqCst);
        let rotate = seal > self.last_swap.load(Ordering::SeqCst);
        let entry = WalEntry::new(
            // A checkpoint is an ordinary transaction in the reference
            // (`BeginTxnShared` with `kNewCheckpoint`), so its frame carries a
            // real id.
            self.next_txn_id.fetch_add(1, Ordering::SeqCst) as i64,
            commit_ts,
            vec![WalCmd::Checkpoint {
                max_commit_ts: begin_ts,
            }],
        );
        // One job for the whole step, the way `WalManager::Flush` swaps inside
        // its loop when it reaches the checkpoint entry
        // (`wal_manager_impl.cpp:288-294`): swapping and appending as two jobs
        // let a commit land in the fresh log before the frame, which put a row
        // frame ahead of the checkpoint. The caller syncs the frame once the
        // data it claims is on disk (`TxnManager::sync_wal`).
        self.wal.run(move |wal| {
            if rotate && wal.file_size()? > 0 {
                wal.rotate(seal)?;
            }
            wal.append(&entry).map(|_| ())
        })?;
        if rotate {
            self.last_swap.store(seal, Ordering::SeqCst);
        }
        self.note_standalone(TxnOp::Checkpoint, commit_ts);
        // The checkpoint transaction is committed: its commit ts is the clock's
        // new position, exactly as `CommitBottom` leaves `current_ts_`.
        self.current_ts.store(commit_ts, Ordering::SeqCst);
        Ok(())
    }

    /// C++ `NewTxn::Cleanup`: the cleanup is a transaction of its own
    /// (`TransactionType::kCleanup`), so it spends a timestamp like any other
    /// and leaves one `CLEANUP` command carrying its **begin** ts
    /// (`CleanupTxnStore::ToWalEntry`). Replay ignores it; the frame exists so
    /// a cleanup is visible in the log exactly as it is in the reference's.
    pub fn append_cleanup(&self) -> Result<()> {
        let _commit = self.commit_lock.lock().unwrap();
        if self.current_ts.load(Ordering::SeqCst) == 0
            && self.prepare_commit_ts.load(Ordering::SeqCst) == 0
        {
            return Ok(());
        }
        let begin_ts = self.current_ts.load(Ordering::SeqCst) + 1;
        let commit_ts = self.prepare_commit_ts.fetch_add(2, Ordering::SeqCst) + 2;
        let entry = WalEntry::new(
            self.next_txn_id.fetch_add(1, Ordering::SeqCst) as i64,
            commit_ts,
            vec![WalCmd::Cleanup {
                timestamp: begin_ts,
            }],
        );
        self.wal.run(move |wal| wal.append(&entry).map(|_| ()))?;
        self.note_standalone(TxnOp::Cleanup, commit_ts);
        self.current_ts.store(commit_ts, Ordering::SeqCst);
        Ok(())
    }

    /// Rotates a full active log so a single file cannot grow without bound.
    ///
    /// The reference rotates when the WAL passes its size threshold and keeps
    /// every sealed segment until `Cleanup`; so does this engine (see
    /// [`Self::drop_sealed_wal_up_to`]).
    pub fn rotate_wal_if_large(&self) -> Result<()> {
        const SEGMENT_BYTES: u64 = 64 * 1024 * 1024;
        let seal = self.last_appended.load(Ordering::SeqCst);
        let rotated = self.wal.run(move |wal| {
            if wal.file_size()? >= SEGMENT_BYTES {
                wal.rotate(seal)?;
                return Ok(true);
            }
            Ok(false)
        })?;
        if rotated {
            self.last_swap.store(seal, Ordering::SeqCst);
        }
        Ok(())
    }

    /// Drops the sealed segments whose records the KV store already holds.
    ///
    /// C++ `WalManager::Cleanup` (`wal_manager_impl.cpp`): a checkpoint keeps
    /// its sealed file — the reference deletes `wal.log.<ts>` only at a
    /// cleanup — so the deletion lives here rather than in the checkpoint path.
    /// The caller must pass a timestamp whose records have been fsynced.
    pub fn drop_sealed_wal_up_to(&self, applied: TxnTimeStamp) -> Result<bool> {
        let applied = applied.min(self.synced.load(Ordering::SeqCst));
        if applied == 0 {
            return Ok(false);
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

    /// The checkpoint's store step: everything applied so far becomes an SST.
    ///
    /// This does not sync the log (only the checkpoint frame in
    /// [`Self::append_checkpoint`] is synced) — the SST is what carries the
    /// checkpoint. The checkpoint entry itself is appended by the caller that
    /// owns the checkpoint (`flush_storage`, `checkpoint_wal`): a checkpoint is
    /// a write transaction in the reference and therefore spends a timestamp, so
    /// it must not be appended twice for one flush.
    pub fn flush(&self) -> Result<()> {
        self.store.flush()?;
        let applied = self.max_applied.load(Ordering::SeqCst);
        self.synced.fetch_max(applied, Ordering::SeqCst);
        Ok(())
    }

    /// Makes the checkpoint frame durable.
    ///
    /// The only fsync on the log: the frame is the marker every later open (this
    /// engine's and the reference's) needs, and a checkpoint that lost it would
    /// reopen at an older position — or, once the older sealed segment has been
    /// pruned, not open at all. Commit records stay unsynced, as in the
    /// reference; the checkpoint's data is what carries them.
    pub fn sync_wal(&self) -> Result<()> {
        self.wal.run(|wal| wal.sync())
    }

    /// Highest commit timestamp the checkpoint has put into an SST.
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
    // The applied marker goes **last**: it is what tells a replay which records
    // the KV already holds, so it may only move once both halves are in. Written
    // with the rows, a crash between the two batches would leave the catalog
    // records out while the marker skipped them on the next open.
    let mut applied: Option<(Vec<u8>, Option<Vec<u8>>)> = None;
    for (key, value) in writes {
        if key == APPLIED_TS_KEY {
            applied = Some((key, value));
            continue;
        }
        let is_row = parse_row_key(&key, ROW_PREFIX).is_some()
            || parse_row_key(&key, DEL_PREFIX).is_some();
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
    if let Some(entry) = applied {
        // The marker goes into **both** stores, and the replay floor is their
        // minimum (see `TxnManager::applied_ts`). The reference keeps the
        // catalog and this marker in one KV, so a crash cannot leave the marker
        // ahead of the catalog. This engine has two stores and neither is
        // fsynced before a checkpoint, so a RocksDB background flush can make
        // one durable while the other still sits in its memtable. Writing both
        // and taking the minimum means the lagging store drags the floor back:
        // the replay then redoes both halves, and it converges because the KV
        // writes are puts on the same keys.
        store.atomic_batch(std::slice::from_ref(&entry))?;
        catalog_store.atomic_batch(std::slice::from_ref(&entry))?;
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
fn build_commands(
    writes: &[(Vec<u8>, Vec<u8>)],
    deletes: &[Vec<u8>],
    table_ids: &HashMap<String, (String, String)>,
    typed: &[WalCmd],
) -> Vec<WalCmd> {
    struct AppendGroup {
        table: String,
        rows: Vec<(u64, String)>,
    }

    let mut groups: Vec<AppendGroup> = Vec::new();
    let mut kv: Vec<(Vec<u8>, Vec<u8>)> = Vec::new();
    let mut kv_kind = KvBatchKind::Other;
    let mut del_groups: Vec<(String, Vec<u64>)> = Vec::new();

    for (key, value) in writes {
        // A tombstone **written** as `del:{table}:{seq} = "1"` (compaction
        // renumbers rows and leaves one per moved row) says the same thing a
        // `DeleteV2` does; the KV still receives the write, the WAL carries the
        // reference command instead of a `KvBatch` the reference cannot read.
        if value == b"1" {
            if let Some((table, seq)) = parse_row_key(key, DEL_PREFIX) {
                match del_groups.last_mut() {
                    Some((name, ids)) if name == table => ids.push(seq),
                    _ => del_groups.push((table.to_string(), vec![seq])),
                }
                continue;
            }
        }
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
        .map(|group| build_append(&group.table, &group.rows, table_ids))
        .collect();

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
        let (db_id, table_id) = table_ids
            .get(&table)
            .cloned()
            .unwrap_or_else(|| (String::new(), String::new()));
        cmds.push(WalCmd::Delete {
            db_name,
            db_id,
            table_name,
            table_id,
            row_ids: seqs.into_iter().map(WalRowId::from_u64).collect(),
        });
    }

    // A DDL that prepared typed commands emits them instead of the catalog
    // `KvBatch`, matching the reference's `WalCmdCreateTableV2`/… .
    if !typed.is_empty() {
        // The caller supplied the typed commands for this transaction (a DDL's
        // `WalCmdCreateTableV2`, an INSERT's reference `WalCmdAppendV2`); they
        // replace the auto-derived ones rather than joining them.
        return typed.to_vec();
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
fn build_append(
    table: &str,
    rows: &[(u64, String)],
    table_ids: &HashMap<String, (String, String)>,
) -> WalCmd {
    let texts: Vec<&str> = rows.iter().map(|(_, text)| text.as_str()).collect();
    let values = infinity_storage::wal::block::ColumnValues::varchars(
        &texts.iter().map(|t| Some(*t)).collect::<Vec<_>>(),
    );
    let block = vec![infinity_storage::wal::block::BlockColumn::new(
        infinity_storage::wal::ddl_def::DataType {
            logical_type: infinity_storage::wal::ddl_def::LogicalType::Varchar,
            type_info: infinity_storage::wal::ddl_def::TypeInfo::None,
        },
        values,
    )];
    // Contiguous row ids are one range, as the reference writes them.
    let mut row_ranges: Vec<(WalRowId, u64)> = Vec::new();
    for (seq, _) in rows {
        match row_ranges.last_mut() {
            Some((base, count)) if base.to_u64() + *count == *seq => *count += 1,
            _ => row_ranges.push((WalRowId::from_u64(*seq), 1)),
        }
    }
    let (db_name, table_name) = split_table(table);
    let (db_id, table_id) = table_ids
        .get(table)
        .cloned()
        .unwrap_or_else(|| (String::new(), String::new()));
    WalCmd::Append {
        db_name,
        db_id,
        table_name,
        table_id,
        row_ranges,
        block,
    }
}

/// An append the WAL recorded but the block files do not have yet.
#[derive(Debug, Clone)]
pub struct ReplayedAppend {
    /// Qualified table name (`db.tbl`, or `tbl` when the log has no db part).
    pub table: String,
    /// The table **id** the frame was written for. A restore re-creates the
    /// table under a new id, and a dropped name can be created again: rows of
    /// a frame that names an older id belong to a table that no longer exists.
    pub table_id: u64,
    /// The transaction's commit ts, which is the `create` the block's
    /// `version` has to carry (`BlockVersion::Append`).
    pub commit_ts: TxnTimeStamp,
    /// `(row id, stored row text)` per row of the frame's block.
    pub rows: Vec<(u64, String)>,
}

/// A delete the WAL recorded that the block's `version` does not mark yet.
#[derive(Debug, Clone)]
pub struct ReplayedDelete {
    pub table: String,
    /// See [`ReplayedAppend::table_id`].
    pub table_id: u64,
    pub commit_ts: TxnTimeStamp,
    pub row_ids: Vec<u64>,
}

/// Replay of [`WalCmd::Append`]: `(row id, stored row text)` per row, which the
/// caller writes into the block (`AppendInBlock`).
fn append_rows(
    _db_name: &str,
    _table_name: &str,
    row_ranges: &[(WalRowId, u64)],
    block: &[infinity_storage::wal::block::BlockColumn],
) -> Result<Vec<(u64, String)>> {
    // A block holding a column this build's `DataBlock` cannot represent (a
    // multi-vector, a tensor) has no rows to hand back here: the rows live in
    // the segment's block files, which the checkpoint wrote, and refusing the
    // frame would make the directory unopenable.
    let Some(block) = infinity_storage::wal::block::to_data_block(block) else {
        return Ok(Vec::new());
    };
    let block = &block;
    let mut out = Vec::new();
    let mut row = 0usize;
    for (base, count) in row_ranges {
        let base_seq = base.to_u64();
        for offset in 0..*count {
            // A column the encoder could not give a value for makes the row
            // unreadable here; the block the checkpoint wrote still holds it,
            // so the replay skips it rather than refusing to open the
            // directory.
            let Some(text) = row_text(block, row) else {
                row += 1;
                continue;
            };
            out.push((base_seq + offset, text));
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
    // A table whose columns carry no per-row scalar (a multi-vector) reports a
    // zero row count; the row still has to come back, so the bound only applies
    // when the block does count rows.
    if block.row_count() > 0 && row >= block.row_count() {
        return None;
    }
    let mut cells = Vec::with_capacity(block.column_count());
    for column in 0..block.column_count() {
        // Inside the block a missing value is a NULL slot (`BlockValue::Null`
        // is the encoder's own marker, the decoder reports the slot as absent),
        // and the stored text of a NULL row is the literal `null`.
        cells.push(match block.get_value(column, row) {
            Some(value) => cell_text(&value),
            None => "null".to_string(),
        });
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

/// Every WAL file in replay order: the sealed segments (oldest first), then the
/// active log.
///
/// The reference names its files `wal.log` / `wal.log.{ts}`; this engine writes
/// the same names. A directory written by an older build of this engine keeps
/// its records in `wal.rust` (with a checkpoint-only `wal.log`), so both are
/// read — a crash on either side then replays what it did not flush.
fn segment_paths(wal_path: &std::path::Path) -> Result<Vec<PathBuf>> {
    let mut out: Vec<PathBuf> = sealed_segments(wal_path)?
        .into_iter()
        .map(|(path, _)| path)
        .collect();
    // `wal.rust` is the older layout's entry file; the active `wal.log` is
    // pushed below and must not be listed twice.
    let legacy = wal_path.with_file_name("wal.rust");
    if legacy != wal_path && legacy.exists() {
        out.push(legacy);
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

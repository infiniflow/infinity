use super::*;

impl QueryResult {
    /// This result's rows as display text (what a `.slt` file or the CLI shows).
    pub fn rows_text(&self) -> Vec<Vec<String>> {
        match self {
            QueryResult::Rows { rows, .. } => rows
                .iter()
                .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
                .collect(),
            _ => Vec::new(),
        }
    }

    /// A result set whose rows are already display text (`SHOW`, `DESCRIBE`,
    /// `EXPLAIN`, ...): each cell becomes a `Value::Str`, which renders back to
    /// exactly the same text.
    pub fn text(columns: Vec<String>, rows: Vec<Vec<String>>) -> Self {
        QueryResult::Rows {
            columns,
            rows: rows
                .into_iter()
                .map(|r| r.into_iter().map(Value::Str).collect())
                .collect(),
        }
    }
}

thread_local! {
    /// Tables whose commit bottom this thread is running right now
    /// (see Engine::run_commit_bottom).
    static BOTTOM_HELD: std::cell::RefCell<std::collections::HashSet<String>> =
        std::cell::RefCell::new(std::collections::HashSet::new());
}

thread_local! {
    /// Runs inside a full-text build, right after the rows are read and before
    /// the index is published. The tests use it to land a compaction in exactly
    /// that window: the schedule that breaks a build is otherwise a race, and a
    /// compaction cannot be driven from the thread that is building.
    static TEXT_BUILD_HOOK: std::cell::RefCell<Option<Box<dyn FnMut()>>> =
        const { std::cell::RefCell::new(None) };
}

/// Installs [`TEXT_BUILD_HOOK`] for this thread; `None` removes it.
#[cfg(test)]
pub(crate) fn set_text_build_hook(hook: Option<Box<dyn FnMut()>>) {
    TEXT_BUILD_HOOK.with(|slot| *slot.borrow_mut() = hook);
}

#[cfg(test)]
fn run_text_build_hook() {
    TEXT_BUILD_HOOK.with(|slot| {
        if let Some(hook) = slot.borrow_mut().as_mut() {
            hook();
        }
    });
}

/// What [`Engine::publish_text_index`] did with a freshly built index.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) enum TextPublish {
    /// Stored in the table's cache: later statements reuse it.
    Cached,
    /// The rows were renumbered under the build, so its postings name ids of a
    /// layout that is gone. The build is worthless for every reader.
    Renumbered,
    /// A compaction committed after the build read: the postings still describe
    /// what this statement sees, but a statement above the merge would resolve
    /// the ids it left behind. Answer this one from it, cache nothing.
    ReadTooOld,
}

/// Marks one table's bottom as running on this thread for as long as it lives.
pub(crate) struct HeldBottom(String);

impl HeldBottom {
    /// Whether this thread already runs the table's bottom half.
    pub(crate) fn holds(table: &str) -> bool {
        BOTTOM_HELD.with(|held| held.borrow().contains(table))
    }

    pub(crate) fn enter(table: &str) -> Self {
        BOTTOM_HELD.with(|held| {
            held.borrow_mut().insert(table.to_string());
        });
        Self(table.to_string())
    }
}

impl Drop for HeldBottom {
    fn drop(&mut self) {
        BOTTOM_HELD.with(|held| {
            held.borrow_mut().remove(&self.0);
        });
    }
}

tokio::task_local! {
    /// The session a statement running on this task belongs to (plan §6.1).
    /// A statement outside a session leaves it unset and sees the process-wide
    /// default database.
    ///
    /// A **task** local, not a thread local: a statement now awaits (WAL,
    /// allocator, bottom, CPU decode), so it can be resumed on a different
    /// runtime worker than the one it started on. A thread local would then
    /// read `None` mid-statement and resolve an unqualified table against the
    /// wrong database.
    pub static ACTIVE_SESSION: Option<i64>;
}

thread_local! {
    /// The session a **pool thread** is acting for.
    ///
    /// The request path keeps the session in the task-local `ACTIVE_SESSION`,
    /// but work dispatched to the CPU/blocking pools (`spawn_cpu`, `spawn_io`)
    /// runs on a thread that has no task context. `scheduler` copies the
    /// task-local into this thread-local before running such a job, so a decode
    /// that resolves `db()` still sees the statement's database.
    static POOL_SESSION: std::cell::Cell<Option<i64>> = const { std::cell::Cell::new(None) };
}

/// Sets the pool thread's session for the duration of `f`.
pub(crate) fn with_pool_session<T>(session: Option<i64>, f: impl FnOnce() -> T) -> T {
    struct Reset(Option<i64>);
    impl Drop for Reset {
        fn drop(&mut self) {
            POOL_SESSION.with(|c| c.set(self.0));
        }
    }
    let previous = POOL_SESSION.with(|c| c.replace(session));
    let _restore = Reset(previous);
    f()
}

/// The session the current task (or pool thread) belongs to.
pub(crate) fn active_session() -> Option<i64> {
    // Inside a task, the task-local is authoritative (it may be `None` for a
    // statement that belongs to no session). A pool thread has no task context,
    // so it falls back to the session carried across the dispatch.
    match ACTIVE_SESSION.try_with(|id| *id) {
        Ok(id) => id,
        Err(_) => POOL_SESSION.with(|c| c.get()),
    }
}

/// Runs `f` with `id` as the task's session.
///
/// A task-local scope, not a guard: tokio restores the binding when the future
/// completes (including on a panic), which is the same "the binding must not
/// outlive the statement" contract the old thread-local guard had.
pub async fn with_session_scope<T, F>(id: i64, f: F) -> T
where
    F: std::future::Future<Output = T>,
{
    ACTIVE_SESSION.scope(Some(id), f).await
}

/// Runs a **synchronous** `f` with `id` as the task's session, for the
/// `block_on` adapters and the tests.
pub fn with_session_sync<T>(id: i64, f: impl FnOnce() -> T) -> T {
    crate::scheduler::block_on(with_session_scope(id, async { f() }))
}

/// Takes back the chunk-store registration when `open` fails after installing it.
///
/// The registration happens before the WAL directory is created and before
/// `TxnManager::open`, and both can fail; without this guard every `?` in
/// between would leave a store registered for a directory no engine has open -
/// and the manager behind it never drops, so the same object directory could not
/// be opened again in this process. Once the engine exists, [`Engine::drop`]
/// owns the cleanup and the guard is disarmed.
struct StoreRegistration {
    root: PathBuf,
    armed: bool,
}

impl StoreRegistration {
    fn new(root: &Path) -> Self {
        Self {
            root: root.to_path_buf(),
            armed: true,
        }
    }

    /// The constructed engine owns the cleanup from here on.
    fn disarm(&mut self) {
        self.armed = false;
    }
}

impl Drop for StoreRegistration {
    fn drop(&mut self) {
        if self.armed {
            storage::chunk_store::uninstall_chunk_store_for(&self.root);
        }
    }
}

impl Engine {
    pub fn open(dir: impl AsRef<Path>) -> Result<Self> {
        Self::open_with_options(dir, EngineOptions::default())
    }

    /// Opens without a WAL: catalog writes are buffered behind an undo journal
    /// and survive a reopen only if a checkpoint ran after they committed.
    pub fn open_without_wal(dir: impl AsRef<Path>) -> Result<Self> {
        Self::open_with_options(
            dir,
            EngineOptions {
                wal_enabled: false,
                ..EngineOptions::default()
            },
        )
    }

    pub fn open_with_capacity(dir: impl AsRef<Path>, exchange_capacity: usize) -> Result<Self> {
        Self::open_with_options(
            dir,
            EngineOptions {
                exchange_capacity,
                ..EngineOptions::default()
            },
        )
    }

    pub fn open_with_options(dir: impl AsRef<Path>, options: EngineOptions) -> Result<Self> {
        let dir: PathBuf = dir.as_ref().to_path_buf();
        // `Config::SnapshotDir()`: snapshots live next to the data root unless
        // the caller names a directory (the CLI always does).
        let dir_for = dir.clone();
        // The catalog lives at `<dir>/catalog`, the path and the substrate the
        // C++ engine uses for its catalog RocksDB, so a C++ engine can open the
        // directory the port wrote.
        let catalog_dir = dir.join("catalog");
        let catalog_store: Arc<dyn MetaStore> = storage::open_store(&catalog_dir)?;
        // A no-WAL open first undoes whatever the previous process wrote after
        // its last checkpoint, then wraps the store so this process's writes
        // are journaled too. Doing the rollback in both modes is defensive: a
        // journal can only be left behind by a no-WAL run.
        deferred_catalog::rollback_pending(catalog_store.as_ref(), &catalog_dir)?;
        let catalog_store: Arc<dyn MetaStore> = if options.wal_enabled {
            catalog_store
        } else {
            Arc::new(deferred_catalog::DeferredCatalogStore::new(
                catalog_store,
                &catalog_dir,
            ))
        };
        // The catalog **is** the reference's key space: every DDL writes the
        // C++ layout directly, and no native record exists beside it, so the
        // store holds exactly the keys a C++ engine would have written.
        let catalog = Catalog::with_store_catalog_only(catalog_store);
        catalog.init_key_space()?;
        // The reference keeps its data files in objects and describes them in
        // `pm|object|`; open the `data/` tree before anything reads a block.
        let persistence = match options.object_dir.as_ref() {
            Some(object_dir) => {
                let pm = storage::persistence::PersistenceManager::with_catalog(
                    catalog.store().clone(),
                    object_dir,
                    dir.join("data"),
                    storage::persistence::DEFAULT_OBJECT_SIZE_LIMIT,
                )?;
                pm.restore_missing_files()?;
                Some(Arc::new(pm))
            }
            None => None,
        };
        // `InfinityContext::instance().persistence_manager()`: once the engine
        // has a manager, **every** data file it writes is published through it
        // and every mapping is resolved through it - blocks, index chunks and
        // the full-text chunk cache alike, exactly as the reference's file
        // workers do. Without one the direct-disk store stays installed, so a
        // plain directory behaves as before.
        // Installed **for this engine's root**, not as the process default: a
        // process may hold several engines, each with its own manager and its
        // own directory, and the one opened last must not serve the earlier
        // ones' reads, writes and cleanups.
        // The registration is undone by the guard if anything below fails
        // (the WAL directory, `TxnManager::open`, the replay, the startup
        // checkpoint); once the engine is built, its `Drop` takes over.
        let mut registration = None;
        match persistence.as_ref() {
            Some(pm) => {
                storage::chunk_store::install_chunk_store_for(
                    &dir,
                    Arc::new(storage::chunk_store::PersistentChunkStore::new(
                        Arc::clone(pm),
                        &dir,
                    )),
                );
                registration = Some(StoreRegistration::new(&dir));
            }
            // A plain open never undoes a manager another open installed: one
            // engine per process makes this a no-op, and a process with several
            // (a test suite) keeps the object profile while it is in use.
            None => storage::chunk_store::install_local_chunk_store_unless_persistent(),
        }
        // The row/meta store, on the same substrate as the catalog.
        let wal_dir = dir.join("wal");
        std::fs::create_dir_all(&wal_dir).map_err(|e| Error::Io {
            path: wal_dir.clone(),
            source: e,
        })?;
        let txn = TxnManager::open(wal_dir.join("wal.log"), catalog.store().clone())?
            .with_catalog_store(catalog.store().clone());
        let engine = Self {
            storage: Storage {
                catalog,
                flush_lock: tokio::sync::RwLock::new(()),
                // The subsystems are built above, so the storage is writable
                // the moment it exists: the reference's standalone/leader
                // startup path reaches `kWritable` in `AdminToWriter` before
                // it serves a request, and every caller of `open` expects to
                // write. `set_storage_mode` moves it elsewhere.
                mode: Mutex::new(StorageMode::Writable),
            },
            txn,
            context: Context {
                block_maps: Mutex::new(std::collections::HashMap::new()),
                buffer_budget: std::sync::atomic::AtomicUsize::new(8 * 1024 * 1024 * 1024),
                block_map_hits: std::sync::atomic::AtomicU64::new(0),
                block_map_misses: std::sync::atomic::AtomicU64::new(0),
                config: Mutex::new(HashMap::new()),
                memindex_quota: std::sync::atomic::AtomicU64::new(0),
                memindex_total: std::sync::atomic::AtomicUsize::new(0),
                memindex_capacity: std::sync::atomic::AtomicUsize::new(64 * 1024),
                flush_dumps_mem_index: std::sync::atomic::AtomicBool::new(false),
                memindex_pending: Arc::new(std::sync::atomic::AtomicUsize::new(0)),
                object_store: Mutex::new(None),
                hnsw_budget: std::sync::atomic::AtomicUsize::new(8 * 1024 * 1024 * 1024),
                scan_gate: ScanGate::new(
                    std::env::var("INFINITY_SCAN_CONCURRENCY")
                        .ok()
                        .and_then(|v| v.parse().ok())
                        .unwrap_or(2),
                ),
                query_count: std::sync::atomic::AtomicU64::new(0),
                connected_at: std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .map(|d| d.as_secs().to_string())
                    .unwrap_or_default(),
                last_cleanup_ts: std::sync::atomic::AtomicU64::new(0),
            },
            dir,
            snapshot_dir: options
                .snapshot_dir
                .clone()
                .unwrap_or_else(|| dir_for.clone().join("snapshot")),
            persistence,
            index_epoch: std::sync::atomic::AtomicU64::new(0),
            result_cache: Mutex::new(HashMap::new()),
            plan_cache: Arc::new(planner::result_cache::SharedCache::new()),
            current_db: Mutex::new("default_db".to_string()),
            sessions: Mutex::new(HashMap::new()),
            tables: Arc::new(Mutex::new(HashMap::new())),
            ddl_lock: tokio::sync::Mutex::new(()),
            bg_processor: Mutex::new(None),
            self_weak: Mutex::new(std::sync::Weak::new()),
            batch_locks: Mutex::new(HashMap::new()),
            bottom_gates: Mutex::new(HashMap::new()),
            bottom_queues: Mutex::new(BottomQueues::default()),
            allocator: Mutex::new(None),
            allocator_jobs: std::sync::atomic::AtomicUsize::new(0),
            batches_since_trim: std::sync::atomic::AtomicUsize::new(0),
            vector_cache: Mutex::new(HashMap::new()),
            cpu_sample: Mutex::new(None),
            queries: Mutex::new(diagnostics::QueryRegistry::default()),
            profile_enabled: std::sync::atomic::AtomicBool::new(false),
        };
        // If this directory was written by the C++ engine (its catalog keys are
        // in the catalog store, but the native catalog is empty), take over its
        // schema so the engine can open a C++-produced directory (§2.6.1).
        //
        // The import has to happen *before* the block recovery: the recovery
        // walks the native tables, so with the schema still missing it would
        // find no table to recover the rows of, and the directory would open
        // with the right schema and no rows.
        if engine.storage.catalog.list_tables()?.is_empty()
            && !engine.storage.catalog.is_catalog_only()
        {
            let _ = engine
                .storage
                .catalog
                .import_layout(engine.storage.catalog.store().as_ref());
        }
        // The catalog records first: the reference keeps its catalog store
        // log-less and recovers every committed DDL and every segment/chunk
        // record by replaying the typed WAL command, so a directory that was
        // killed before its next checkpoint still has its schema. Replaying
        // before the blocks matters - the block recovery walks the tables.
        // A no-WAL engine's durable state is the last checkpoint: the catalog
        // journal above already undid everything written after it, and letting
        // the log put those DDLs and rows back would contradict the mode (the
        // reference replays only records up to its last checkpoint ts).
        let wal_enabled = options.wal_enabled;
        let applied = engine.txn.applied_ts()?;
        if wal_enabled {
            engine.txn.replay_commands(applied, |cmd, ts| {
                crate::wal_bridge::replay(&engine, cmd, ts)
            })?;
        }
        // The blocks next, then the WAL's rows on top: what the files carry is
        // already applied, so the replay starts above the newest timestamp they
        // hold (the reference's checkpoint record gives it the same floor).
        let hydrated = engine.recover_blocks()?;
        // Two floors, because the two halves of a commit reach the disk in
        // sequence: the KV batch is written first, the block in `CommitBottom`
        // after. A record above `applied` (the newest checkpoint frame) still
        // has KV work to do; a record above `hydrated` still has to reach a
        // block.
        if wal_enabled {
            let mut replayed_appends = Vec::new();
            let mut replayed_deletes = Vec::new();
            engine.txn.recover_into(
                applied,
                hydrated,
                &mut replayed_appends,
                &mut replayed_deletes,
            )?;
            engine.apply_replayed_rows(replayed_appends, replayed_deletes)?;
        }
        engine.rebuild_indexes()?;
        // `MetaTableObject::RestoreTableCache`: the allocator has no memory of
        // its own across a restart. The segment map, and the row id to continue
        // from, come from the catalog's segment/block layer and the blocks'
        // `version` files (`GetSegmentRowCount` / `GetCurrentSegmentRowCount`).
        // Without this the first checkpoint of the new process mirrors only the
        // segments written since the restart, and the block mirror deletes the
        // directories of the segments it did not produce - which is how a
        // replayed import lost the rows of every earlier segment.
        for meta in engine.storage.catalog.list_tables().unwrap_or_default() {
            let key = engine.table_key(&meta.name);
            let counts = engine.block_segment_row_counts(&key);
            let next = engine.next_row_seq(&key).unwrap_or(0);
            let mut tables = engine.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            for (id, rows) in counts {
                entry.segments.entry(id).or_insert(SegmentEntry {
                    id,
                    rows: rows.min(u32::MAX as u64) as u32,
                });
            }
            if next > 0 {
                entry.row_counter = Some(((next >> 32) as u32, (next & 0xffff_ffff) as u32, false));
            }
            entry.write_batches = entry.segments.len();
        }
        // `Storage::Init` finishes with two write transactions: the startup
        // checkpoint and (on a directory that has no system timestamp yet) the
        // creation of `default_db`. Both spend a timestamp, and a directory the
        // reference wrote carries their commit ts in the WAL and in the catalog
        // keys, so skipping them would shift every later timestamp by two.
        //
        // The checkpoint is a real one: `NewTxn::Checkpoint` flushes the KV
        // before it writes the frame, so everything the replay just put back
        // (DDL, segment marks, chunk records) is in an SST by the time the frame
        // claims it. Without the flush the next crash would lose those records
        // and the frame would hide the WAL entries that recreate them.
        // Through the transaction manager, not the store alone: the flush also
        // records how far the SST goes (`synced`), which is what a WAL-only
        // checkpoint frame is allowed to claim.
        engine.txn.flush()?;
        crate::scheduler::block_on(engine.txn.append_checkpoint_flushed())?;
        // The frame is appended unsynced (the commit path syncs in batches, as
        // NewFlush does); the startup checkpoint is its own batch, so sync it
        // here. Without this a directory whose only frame is this one would
        // reopen at an older position - or not at all once that segment is
        // pruned - after a power loss.
        crate::scheduler::block_on(engine.txn.sync_wal())?;
        if !engine.storage.catalog.has_database_record("default_db")? {
            crate::scheduler::block_on(engine.ddl_commit_typed(
                TxnOp::CreateDatabase {
                    db: "default_db".to_string(),
                },
                || engine.storage.catalog.create_database("default_db", false),
                |batch| crate::wal_bridge::create_database_command_from_batch(batch, "default_db"),
            ))?;
        }
        // The reference stamps `db_id`/`table_id` into every row command; a
        // directory opened here has to name its tables the same way.
        for meta in engine.storage.catalog.list_tables().unwrap_or_default() {
            engine.register_table_ids(&meta);
        }
        // `NewTxn::RecoverMemIndex`: a checkpoint does not dump a memory index,
        // so the rows its chunks do not cover are re-indexed into the tail
        // here, once the blocks, the WAL and the segment map are all in place.
        engine.recover_mem_index()?;
        // The in-memory indexes were just rebuilt; the quota reads this total.
        engine.recompute_memindex_total();
        // The engine is complete: its `Drop` releases the registration, and the
        // guard must not undo it under a live engine.
        if let Some(registration) = registration.as_mut() {
            registration.disarm();
        }
        Ok(engine)
    }

    /// Best-effort database id for a (possibly qualified) table name.
    ///
    /// The catalog tracks table ids but not database ids, so the id is the
    /// database's position in the catalog's listing (`default_db` first), made
    /// 1-based to match the reference: `default_db` is `db_1`, the first user
    /// database `db_2`, and so on. Unqualified names resolve against
    /// `default_db`.
    pub(crate) fn database_id_of(&self, table: &str) -> u64 {
        let db = if table.contains('.') {
            table.split('.').next().unwrap_or("default_db")
        } else {
            "default_db"
        };
        self.database_id(db)
    }

    /// The C++ database id of `db`: its 1-based position in the catalog's
    /// listing (`default_db` first).
    pub fn database_id(&self, db: &str) -> u64 {
        // The record carries the id `CREATE DATABASE` handed out
        // (`next_database_id_ + 1`), which is what names `db_{id}`.
        if let Ok(Some(id)) = self.storage.catalog.database_id_of_name(db) {
            return id;
        }
        // A database implied but not recorded (`default_db` on a directory that
        // predates its record) falls back to its position.
        self.storage
            .catalog
            .list_databases()
            .unwrap_or_default()
            .iter()
            .position(|d| d == db)
            .map(|p| p as u64 + 1)
            .unwrap_or(1)
    }

    /// Dumps every memory index when the unified budget is exceeded, mirroring
    /// the reference's memindex dump. The budget is one number for the whole
    /// engine (`memindex_bytes`: every dumpable family, including the HNSW
    /// charge and the IVF rows), not one per index.
    /// `memindex_memory_quota = 0` disables it.
    ///
    /// With a `BGTaskProcessor` attached this proposes one background
    /// `DumpMemIndexTask` (C++ `BGMemIndexTracer::IncreaseMemoryUsage`): the
    /// pending flag coalesces the per-row checks, and the task dumps whatever
    /// was buffered when the worker runs. Without a processor the dump stays
    /// synchronous, which is the engine's historical in-process behaviour.
    /// `table` is the one whose tails just changed (the per-row call); `None`
    /// asks for a full recompute first (an explicit check).
    /// The quota check, in its **async** form: every caller
    /// (`run_bottom`, `insert_literals_inner`, `copy_from_inner`,
    /// `execute_stmt_inner`) is already a task, so the fallback dump is an
    /// `.await` instead of a nested `block_on` on a runtime worker.
    pub(crate) async fn enforce_memindex_quota(&self, table: Option<&str>) {
        let quota = self
            .context
            .memindex_quota
            .load(std::sync::atomic::Ordering::Relaxed) as usize;
        if quota == 0 {
            return;
        }
        // No time gate. The reference's `MemIndexTracer::IncreaseMemoryUsage`
        // compares on **every** increase (`cur_index_memory_ >
        // index_memory_limit_ + proposed_dump_size_`, `memindex_tracer.cppm:103`)
        // and relies on `proposed_dump_` to avoid queueing a second task for an
        // index that already has one in flight - never on a clock.
        //
        // This used to skip a check whose predecessor was younger than 10 ms,
        // which made the outcome of a write depend on how fast the previous
        // statement had been: two `INSERT`s issued back to back landed as one
        // breach, so only one dump happened where the reference makes two.
        // The callers are **per statement** (`update_text_indexes_batch`), not
        // per row, so the clock bought nothing anyway.
        //
        // Fold in what changed before comparing: one table when the caller
        // names it (O(that table's indexes)), everything for an explicit check.
        match table {
            Some(table) => self.refresh_memindex_charge(table),
            None => {
                self.recompute_memindex_total();
            }
        }
        if self.budget_bytes() <= quota {
            return;
        }
        if self.bg_processor.lock().unwrap().is_some() {
            self.propose_dump(table.is_none());
            return;
        }
        // No background processor (a test, or shutdown): do the same work
        // inline. An explicit check (`table == None`) is a caller asking for
        // everything to be released, so it drains every tail; the per-write
        // path takes the largest one.
        if table.is_none() {
            self.dump_all_memory_indexes().await;
            // An explicit check may run outside any bottom half (a test, a
            // shutdown), so the block drain can walk every table.
            self.dump_all_sealed_blocks(None).await;
            // The block drain released rows after the index dump had already
            // folded its own result in, so the total is restated once more.
            self.recompute_memindex_total();
        } else {
            // Inside this table's bottom half: its own gate is held, so the
            // block drain must not reach for another table's.
            self.dump_under_pressure(table).await;
        }
    }

    /// The quota check for a table whose rows just grew, without the per-row
    /// gate.
    ///
    /// [`Self::enforce_memindex_quota`] is driven by the **index** write path,
    /// so a table with no index never reached it - and that is exactly the
    /// table whose append buffer grows unbounded. This runs at the end of a
    /// commit instead: it folds in this table's new charge and, if the engine
    /// is over budget, proposes the same background drain. Cheap enough for the
    /// commit path because it reads one table's running totals and one atomic.
    pub(crate) async fn enforce_quota_after_append(&self, table: &str) {
        let quota = self
            .context
            .memindex_quota
            .load(std::sync::atomic::Ordering::Relaxed) as usize;
        if quota == 0 {
            return;
        }
        self.refresh_memindex_charge(table);
        if self.budget_bytes() <= quota {
            return;
        }
        if self.bg_processor.lock().unwrap().is_some() {
            self.propose_dump(false);
            return;
        }
        // This runs after the commit returned, so no bottom gate is held and
        // the blocks of every table are fair game.
        self.dump_under_pressure(None).await;
    }

    /// Queues the background drain. The caller has already established that a
    /// processor is attached; one proposal is in flight at a time
    /// (`proposed_dump_`).
    fn propose_dump(&self, drain_all: bool) {
        let Some(processor) = self.bg_processor.lock().unwrap().clone() else {
            return;
        };
        if self
            .context
            .memindex_pending
            .compare_exchange(
                0,
                1,
                std::sync::atomic::Ordering::AcqRel,
                std::sync::atomic::Ordering::Relaxed,
            )
            .is_err()
        {
            return;
        }
        let engine = self.self_weak.lock().unwrap().clone();
        let queued = processor.submit(Box::new(DumpMemIndexTask {
            engine,
            pending: Arc::clone(&self.context.memindex_pending),
            drain_all,
        }));
        if !queued {
            // The processor is shutting down and dropped the task, so no worker
            // will clear the proposed-dump flag: clear it here or the quota
            // never proposes another dump.
            self.context
                .memindex_pending
                .store(0, std::sync::atomic::Ordering::Relaxed);
        }
    }

    /// The rows an indexed predicate names, for the columnar select.
    ///
    /// `col = v` probes the key (memory tail + sealed chunks); a conjunction of
    /// bounds on one column becomes one range read. `None` means the predicate
    /// is neither, or the index cannot order the key (a hashed varchar), and the
    /// caller scans the table instead.
    pub(crate) fn indexed_predicate_rows(
        &self,
        table: &str,
        schema: &[String],
        expr: &Expr,
    ) -> Option<Vec<u64>> {
        // `col <op> literal`; a literal on the left is left to the scan.
        fn axis(expr: &Expr) -> Option<(usize, parser::CmpOp, &Value)> {
            match expr {
                Expr::Cmp(a, op, b) => match (&**a, &**b) {
                    (Expr::Column(pos), Expr::Literal(value)) => Some((*pos, *op, value)),
                    _ => None,
                },
                _ => None,
            }
        }
        if let Some((pos, op, value)) = axis(expr) {
            let column = schema.get(pos)?;
            if op == parser::CmpOp::Eq {
                let literal = value_to_literal(value.clone());
                return self.index_lookup(table, column, &literal);
            }
            let (lo, hi) = match op {
                parser::CmpOp::Gt | parser::CmpOp::Ge => (value.clone(), Value::Int(i64::MAX)),
                parser::CmpOp::Lt | parser::CmpOp::Le => (Value::Int(i64::MIN), value.clone()),
                _ => return None,
            };
            return self.index_range_lookup(table, column, &[(lo, hi)]);
        }
        // `a >= lo AND a < hi` (either order, either bound): one interval.
        if let Expr::And(left, right) = expr {
            let (lpos, lop, lvalue) = axis(left)?;
            let (rpos, rop, rvalue) = axis(right)?;
            if lpos != rpos {
                return None;
            }
            let column = schema.get(lpos)?;
            let mut lo = Value::Int(i64::MIN);
            let mut hi = Value::Int(i64::MAX);
            for (op, value) in [(lop, lvalue), (rop, rvalue)] {
                match op {
                    parser::CmpOp::Gt | parser::CmpOp::Ge => lo = value.clone(),
                    parser::CmpOp::Lt | parser::CmpOp::Le => hi = value.clone(),
                    parser::CmpOp::Eq => {
                        lo = value.clone();
                        hi = value.clone();
                    }
                    _ => return None,
                }
            }
            return self.index_range_lookup(table, column, &[(lo, hi)]);
        }
        None
    }

    pub(crate) fn fetch_rows_by_ids(&self, table: &str, ids: &[u64]) -> Result<Vec<Vec<Value>>> {
        let defaults = self.table_defaults(table).unwrap_or_default();
        let types = self.column_types(table).unwrap_or_default();
        let mut rows = Vec::new();
        for (_, v) in self.row_texts(table, ids)? {
            rows.push(self.parse_stored_row(table, &defaults, &types, &v)?);
        }
        Ok(rows)
    }

    /// The session's current database.
    pub(crate) fn db(&self) -> String {
        // A statement that belongs to a session sees that session's database;
        // everything else (a test, a background task) sees the default.
        if let Some(id) = active_session() {
            if let Some(db) = self.sessions.lock().unwrap().get(&id) {
                return db.clone();
            }
        }
        self.current_db.lock().unwrap().clone()
    }

    /// Registers a session, with the database it starts in.
    pub fn open_session(&self, db: impl Into<String>) -> i64 {
        static NEXT: std::sync::atomic::AtomicI64 = std::sync::atomic::AtomicI64::new(1);
        let id = NEXT.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        self.sessions.lock().unwrap().insert(id, db.into());
        id
    }

    /// Drops a session. `false` when the id was never registered (or already
    /// dropped), which the server reports as `SessionNotFound`.
    pub fn close_session(&self, id: i64) -> bool {
        self.sessions.lock().unwrap().remove(&id).is_some()
    }

    /// The database `id` is in, or `None` for an unknown session.
    pub fn session_db(&self, id: i64) -> Option<String> {
        self.sessions.lock().unwrap().get(&id).cloned()
    }

    /// Runs one statement for `id`, so `USE` and unqualified table names see
    /// that session's database instead of the process-wide default.
    pub fn execute_in_session(&self, id: i64, sql: &str) -> Result<Vec<QueryResult>> {
        if self.session_db(id).is_none() {
            return Err(Error::NotFound(format!("session {id}")));
        }
        self.with_session(id, || self.execute_blocking(sql))
    }

    /// Runs `f` with `id` as the active session.
    ///
    /// This is the scope the wire layer needs once a statement can run on any
    /// pool thread: everything inside `f` — not only `execute`, but the
    /// catalog lookups and helper calls it makes — sees that session's
    /// database, and the previous binding is restored afterwards.
    pub fn with_session<T>(&self, id: i64, f: impl FnOnce() -> T) -> T {
        // The scope restores the binding when it completes, panic included, so
        // a failed statement cannot leave its session on a reused task.
        with_session_sync(id, f)
    }

    /// The form the wire handler awaits: the scope wraps the whole statement,
    /// across its suspensions.
    pub async fn with_session_async<T, F>(&self, id: i64, f: F) -> T
    where
        F: std::future::Future<Output = T>,
    {
        with_session_scope(id, f).await
    }

    /// True when at least one index (secondary / fulltext / vector / sparse /
    /// multi-vector) is maintained in memory. An insert into a table with none
    /// skips the per-row index hook entirely.
    pub(crate) fn any_in_memory_index(&self) -> bool {
        if !self.vector_cache.lock().unwrap().is_empty() {
            return true;
        }
        self.tables.lock().unwrap().values().any(|e| {
            !e.text.is_empty()
                || !e.hnsw_mem.is_empty()
                || !e.secondary.indexes.is_empty()
                || !e.ivf_mem.is_empty()
                || !e.mv.is_empty()
                || !e.sparse.is_empty()
        })
    }

    /// Whether **this table** holds an index the insert hook maintains.
    ///
    /// The hook used to ask whether *any* table had one, so a table with no
    /// index took the indexed path too and - while that path was one appender
    /// thread - queued behind every other table.
    pub(crate) fn table_has_memory_index(&self, table: &str) -> bool {
        let key = self.table_key(table);
        let prefix = format!("{key}.");
        if self
            .vector_cache
            .lock()
            .unwrap()
            .keys()
            .any(|cache| cache.starts_with(&prefix))
        {
            return true;
        }
        self.tables.lock().unwrap().get(&key).is_some_and(|entry| {
            !entry.text.is_empty()
                || !entry.hnsw_mem.is_empty()
                || !entry.secondary.indexes.is_empty()
                || !entry.ivf_mem.is_empty()
                || !entry.mv.is_empty()
                || !entry.sparse.is_empty()
        })
    }

    /// True when `table` has an index declaration. Consulted together with
    /// [`Self::any_in_memory_index`] so the per-row hook runs whenever any
    /// index state could need it.
    pub(crate) fn has_index_declaration(&self, table: &str) -> bool {
        self.storage
            .catalog
            .indexes_of_table(table)
            .map(|is| !is.is_empty())
            .unwrap_or(true)
    }

    /// Drops the decoded-row cache; every mutation that rewrites or removes
    /// rows calls it before the next scan.
    /// The engine as an owned handle, when it lives in an `Arc`.
    ///
    /// `Weak::upgrade` fails for an engine created without an `Arc`; callers
    /// then fall back to running the work on their own thread.
    pub fn engine_handle(&self) -> Option<Arc<Engine>> {
        self.self_weak.lock().unwrap().upgrade()
    }

    /// Records the engine's own handle so it can be handed to worker threads
    /// (`BottomExecutor`, `MemIndexAppender`, `TxnAllocator`).
    pub fn install_engine_handle(self: &Arc<Self>) {
        *self.self_weak.lock().unwrap() = Arc::downgrade(self);
    }

    /// Runs bottom halves on the shared blocking pool (`Engine::submit_bottom`).
    pub(crate) fn attach_bottom_pool(self: &Arc<Self>) {
        self.install_engine_handle();
        self.bottom_queues.lock().unwrap().attached = true;
    }

    /// Whether bottoms run on the shared pool rather than inline.
    pub(crate) fn bottom_pool_attached(&self) -> bool {
        self.bottom_queues.lock().unwrap().attached
    }

    /// Queues a bottom-half job on its table's FIFO.
    ///
    /// Returns `false` when no pool is attached (the caller then runs the job
    /// inline). One job per table is in flight; the drain loop runs the next
    /// queued job when the previous one finishes, so a table's bottoms keep
    /// their commit order without bounding the *number* of tables that can be
    /// in flight (the reference's `crc32(table_id) % n` routing does).
    pub(crate) fn submit_bottom(&self, task: Arc<BottomWork>) -> bool {
        let Some(engine) = self.engine_handle() else {
            return false;
        };
        let table = task.table_id.clone();
        {
            let mut queues = self.bottom_queues.lock().unwrap();
            if !queues.attached {
                return false;
            }
            queues
                .pending
                .entry(table.clone())
                .or_default()
                .push_back(task);
            if !queues.running.insert(table.clone()) {
                return true; // a job of this table is already draining
            }
        }
        // One task on the pool per burst: it drains this table's queue and
        // returns, so an idle table holds no thread.
        crate::scheduler::scheduler().submit(Box::new(move || loop {
            let next = {
                let mut queues = engine.bottom_queues.lock().unwrap();
                match queues
                    .pending
                    .get_mut(&table)
                    .and_then(|queue| queue.pop_front())
                {
                    Some(task) => task,
                    None => {
                        queues.running.remove(&table);
                        return;
                    }
                }
            };
            if storage::wal::bottom_executor::BottomTask::is_committing(&*next) {
                storage::wal::bottom_executor::BottomTask::commit_bottom(&*next);
            }
        }));
        true
    }

    /// Runs one statement's post-commit work the way `NewTxnManager::
    /// CommitBottom` does: on the `BottomExecutor` worker that owns `table`,
    /// the caller waiting for it, or inline when no pool is attached.
    pub(crate) async fn run_commit_bottom<F>(&self, table: &str, work: F) -> Result<()>
    where
        F: FnOnce(&Engine) -> Result<()> + Send + 'static,
    {
        // The bottom half of one table's commit is serialised by its gate, but a
        // step inside a bottom can run another: the memindex quota check dumps an
        // index from the insert's own bottom half, and the dump takes the bottom
        // of the same table. Re-entering the gate on this thread would deadlock
        // (an engine without a bottom executor runs the bottom inline), so the
        // inner call runs its work directly - the gate protects the table from
        // *other* threads, and this one already holds it.
        let key = self.table_key(table);
        if BOTTOM_HELD.with(|held| held.borrow().contains(&key)) {
            return work(self);
        }
        if self.engine_handle().is_none() || !self.bottom_pool_attached() {
            let gate = self.bottom_gate(table);
            let _turn = gate.lock().await;
            let _held = HeldBottom::enter(&key);
            return work(self);
        }
        let engine = self.engine_handle().expect("checked above");
        let work_key = key.clone();
        // The bottom may run on another thread, so the session travels with it
        // (the `HeldBottom` thread-local does the same for the gate).
        let session = active_session();
        // The read timestamp travels with it for the same reason: the pool
        // thread has no task context, so `read_ts()` there would otherwise read
        // whatever the previous job left in the thread-local.
        let read_ts = crate::engine::state::read_ts();
        let wrapped = move |engine: &Engine| {
            let _held = HeldBottom::enter(&work_key);
            // The bottom runs on a pool thread: carry the session across.
            with_pool_session(session, || {
                crate::engine::state::with_pool_read_ts(read_ts, || work(engine))
            })
        };
        let (done, finished) = tokio::sync::oneshot::channel();
        let task = Arc::new(BottomWork {
            engine,
            table_id: key.clone(),
            gates: vec![self.bottom_gate(table)],
            work: std::sync::Mutex::new(Some(
                Box::new(wrapped) as Box<dyn FnOnce(&Engine) -> Result<()> + Send>
            )),
            done: std::sync::Mutex::new(Some(done)),
            finished: std::sync::Mutex::new(Some(finished)),
        });
        self.submit_bottom(task.clone());
        task.wait().await
    }

    /// Starts `TxnAllocator` on first use and returns its queue.
    ///
    /// Like the reference, the allocator is a **thread**, not a lock: an append
    /// submits its table and row count and waits for the base id, so ids stay
    /// contiguous in submission order even when several transactions allocate
    /// at once. An engine that is not held in an `Arc` cannot be handed to the
    /// thread, so it falls back to allocating inline (the same fallback the
    /// bottom half uses).
    pub(crate) fn allocator(&self) -> Option<Arc<Allocator>> {
        if let Some(allocator) = self.allocator.lock().unwrap().clone() {
            return Some(allocator);
        }
        let engine = self.engine_handle()?;
        // Bounded, like the reference's blocking queue: every append blocks on
        // its answer anyway, so a producer that outran the allocator would pile
        // jobs up with no upper limit. The cap is generous enough that the
        // allocator only ever sees a burst of concurrently committing
        // statements, never a backlog. A tokio channel carries it (plan 6.12.4
        // step 2), so the send can become `.await` when the statement path is
        // async; today both ends are blocking threads.
        let (jobs, mut queue) = tokio::sync::mpsc::channel::<AllocJob>(ALLOC_QUEUE_CAPACITY);
        let weak = Arc::downgrade(&engine);
        // The consumer is a **task**, not a blocking-pool thread. Every job is
        // pure memory: it reads/writes the in-memory row counter (the one
        // `next_row_seq` path that touches the filesystem only runs on an empty
        // table, where `BlockIndex::open` finds no directory and returns
        // immediately).
        //
        // **A dedicated blocking thread, not a runtime task.** The allocator is
        // called from a commit's `before_wal` callback, which the commit chain
        // invokes **synchronously** while it holds the commit-ts turn; the
        // caller bridges that with `block_on`. If the consumer were a task, that
        // `block_on` would park the very runtime worker the task needs to be
        // polled on - every INSERT/IMPORT would deadlock the first time it
        // allocated (measured: 19 executor tests hung past 60 s). A blocking
        // thread makes the reply independent of the runtime, so the bridge
        // always completes.
        crate::scheduler::scheduler().spawn_worker(Box::new(move || {
            while let Some(job) = queue.blocking_recv() {
                match weak.upgrade() {
                    Some(engine) => {
                        engine
                            .allocator_jobs
                            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
                        job(&engine);
                    }
                    None => break,
                }
            }
        }));
        let allocator = Arc::new(Allocator { jobs });
        *self.allocator.lock().unwrap() = Some(allocator.clone());
        Some(allocator)
    }

    /// The gate a transaction's bottom half holds for `table`.
    ///
    /// `BottomExecutor` gives the reference the same shape with a thread pool
    /// sharded by table id: two transactions of one table take their bottom
    /// turn in order, transactions of different tables overlap.
    pub(crate) fn bottom_gate(&self, table: &str) -> Arc<tokio::sync::Mutex<()>> {
        let key = self.table_key(table);
        let mut gates = self.bottom_gates.lock().unwrap();
        Arc::clone(gates.entry(key).or_default())
    }

    pub(crate) fn invalidate_row_cache(&self, table: &str) {
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.row_cache = None;
        }
    }

    /// The live-row count, using the incremental counter when available.
    pub(crate) fn live_row_count(&self, table: &str) -> Result<usize> {
        let key = self.table_key(table);
        if let Some(live) = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.live_rows)
        {
            return Ok(live);
        }
        let live = self.row_count(table)?;
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .live_rows = Some(live);
        Ok(live)
    }

    /// Marks the C++ block mirror stale after a schema or row-id rewrite.
    /// Block reads fall back to the KV row store until the next checkpoint
    /// rewrites the mirror.
    /// The files no longer describe the rows - a column was dropped, rows were
    /// renumbered, a compaction moved them. The next checkpoint has to rewrite
    /// the table even though no append went through the allocator.
    pub(crate) fn invalidate_block_mirror(&self, table: &str) {
        let key = self.table_key(table);
        let table_id = self
            .storage
            .catalog
            .get_table_by_name(&key)
            .ok()
            .flatten()
            .map(|meta| meta.id);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.block_rows = None;
            entry.block_index = None;
            entry.live_rows = None;
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
        }
        // The files this engine mapped no longer describe the table: drop the
        // mappings so the next read maps what is on disk now.
        if let Some(table_id) = table_id {
            self.drop_block_maps(table_id);
        }
    }

    /// Whether the files under `data/` no longer describe the table's rows:
    /// a row was appended or deleted, a column was dropped or a compaction
    /// renumbered the rows, and the next checkpoint has not rewritten the
    /// mirror yet. A reader must then merge the append buffer over the blocks.
    ///
    /// Per process only: a restart rebuilds the buffer by replaying the WAL
    /// above the newest checkpoint frame, so a mark kept in the store would say
    /// nothing the replay does not.
    pub(crate) fn mirror_dirty(&self, table: &str) -> bool {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|entry| entry.mirror_dirty)
            .unwrap_or(false)
    }

    /// Whether the table has block files at all. A table whose rows were only
    /// ever buffered (an import replayed from the WAL, a directory that was
    /// never checkpointed) has none, and its clean `mirror_dirty` flag must not
    /// be read as "the blocks are the table".
    pub(crate) fn has_block_mirror(&self, table: &str) -> bool {
        self.block_index_for(table)
            .map(|index| !index.lock().unwrap().is_empty())
            .unwrap_or(false)
    }

    /// The key of a table's [`TableEntry`]: the fully-qualified lower-case
    /// name (the C++ catalog's `db.table` key).
    pub(crate) fn table_key(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_ascii_lowercase()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        }
    }

    /// Tells the WAL which catalog ids name `meta`'s table: the reference
    /// stamps them into every `Append`/`Delete` (`WalCmdAppend::db_id_` /
    /// `table_id_`), and a reader that resolves a table by id needs them.
    pub(crate) fn register_table_ids(&self, meta: &catalog::TableMeta) {
        let db = meta
            .name
            .split('.')
            .next()
            .unwrap_or("default_db")
            .to_string();
        let db_id = self.database_id(&db);
        self.txn
            .register_table_ids(&meta.name, &db_id.to_string(), &meta.id.to_string());
    }

    /// The sparse-gram full-text columns of one scan, for `RegexIndexRewrite`.
    /// Only a column indexed with a sparse-gram analyzer can narrow a
    /// `regex()` predicate.
    pub(crate) fn regex_index_catalog(
        &self,
        table: &str,
        columns: &[String],
    ) -> planner::regex_rewrite::RegexIndexCatalog {
        use planner::regex_rewrite::RegexIndexCatalog;
        let key = self.table_key(table);
        let mut catalog = RegexIndexCatalog::new();
        let tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get(&key) else {
            return catalog;
        };
        for (i, column) in columns.iter().enumerate() {
            let Some(analyzer) = entry.analyzers.get(column) else {
                continue;
            };
            // Only a `sparsegram` index rewrites regex queries; the family match
            // is the same case-sensitive one `AnalyzerPool::get_analyzer` uses.
            let Ok(analyzer::AnalyzerName::SparseGram { min_n, max_n, fold }) =
                analyzer::parse_analyzer_name(analyzer)
            else {
                continue;
            };
            let index_name = entry
                .analyzers_by_index
                .iter()
                .find(|(_, a)| *a == analyzer)
                .map(|(name, _)| name.clone())
                .unwrap_or_else(|| format!("{column}_ft_index"));
            catalog = catalog.with_column(i, index_name, min_n, max_n, fold);
        }
        catalog
    }

    pub(crate) fn table_schema(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .columns)
    }

    /// The table's block map (C++ `BlockIndex`), opened lazily from the
    /// C++-format mirror's `version` files. The decoded blocks go through the
    /// engine-wide `Context::buffer` LRU.
    pub(crate) fn block_index_for(
        &self,
        table: &str,
    ) -> Option<Arc<Mutex<storage::block_index::BlockIndex>>> {
        let table = self.table_key(table);
        let key = table.clone();
        // The cache check comes first: resolving the name through the catalog
        // reads the table's metadata from the store, and a scan asks for the
        // index once per block, so a lookup that never used the cache made a
        // ten-row result pay for a catalog read per block it touched.
        {
            let tables = self.tables.lock().unwrap();
            if let Some(index) = tables.get(&key).and_then(|e| e.block_index.clone()) {
                return Some(index);
            }
        }
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let db_id = self.database_id_of(&table);
        let index =
            storage::block_index::BlockIndex::open(self.blocks_root(), db_id, meta.id).ok()?;
        let shared = Arc::new(Mutex::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .block_index = Some(shared.clone());
        Some(shared)
    }

    /// The append/delete/import journals, rebuilt from the block files on the
    /// first use in this process.
    ///
    /// All five lists come out of one derivation so their lengths agree - the
    /// block `version` writer zips `totals` with `ts` and would otherwise write
    /// a `create` list that differs from the reference's.
    pub(crate) fn ensure_journals(&self, table: &str) -> Journals {
        let tkey = self.table_key(table);
        let cached = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).and_then(|entry| {
                Some((
                    entry.append_totals.clone()?,
                    entry.append_ts.clone()?,
                    entry.append_copy.clone()?,
                    entry.deleted_ids.clone()?,
                    entry.delete_ts.clone()?,
                ))
            })
        };
        if let Some((appends, append_ts, append_copy, deletes, delete_ts)) = cached {
            return Journals {
                appends,
                append_ts,
                append_copy,
                deletes,
                delete_ts,
            };
        }
        let (appends, append_ts, append_copy, deletes, delete_ts, imports) =
            self.journals_from_blocks(&tkey);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.append_totals = Some(appends);
        entry.append_ts = Some(append_ts);
        entry.append_copy = Some(append_copy);
        entry.deleted_ids = Some(deletes);
        // The journal was just rebuilt: a set built from the old one no longer
        // describes it.
        entry.deleted_set = None;
        entry.delete_ts = Some(delete_ts);
        entry.imports = Some(imports);
        Journals {
            appends: entry.append_totals.clone().unwrap_or_default(),
            append_ts: entry.append_ts.clone().unwrap_or_default(),
            append_copy: entry.append_copy.clone().unwrap_or_default(),
            deletes: entry.deleted_ids.clone().unwrap_or_default(),
            delete_ts: entry.delete_ts.clone().unwrap_or_default(),
        }
    }

    /// Stamps one append record with the commit ts that appended it.
    ///
    /// `settle_append_ts` (the "last `count` records" form) is only right while
    /// no other statement is appending; a commit says exactly which record it
    /// created (`BlockVersion::Append(commit_ts, rows)` sits on the append the
    /// transaction made, not on whatever landed last).
    pub(crate) fn settle_append_at(&self, table: &str, index: usize, ts: u64) {
        if index == usize::MAX {
            return;
        }
        let n = self.append_totals(table).len();
        if n == 0 || index >= n {
            return;
        }
        let mut log = self.append_ts(table);
        log.resize(n, 0);
        log[index] = ts;
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        tables.entry(tkey).or_default().append_ts = Some(log);
    }

    /// Rewrites the append log after a compaction merged `sources` into a new
    /// segment of `new_rows` rows.
    ///
    /// The log is cumulative over the table's rows in segment-id order, which
    /// is the order the mirror lays segments out in. A compaction removes the
    /// merged segments' rows and appends the new segment's, so every boundary
    /// after a removed segment shifts down by the rows removed before it, the
    /// boundaries *inside* a removed segment disappear, and the new segment's
    /// single append goes last. A merge of every segment therefore leaves one
    /// entry, and a partial merge keeps the untouched segments' `version`
    /// batches exactly as they were.
    ///
    /// Returns `(totals, ts, copy)` in the shape the three logs are stored in.
    pub(crate) fn rebase_appends(
        &self,
        table: &str,
        layout: &[(u32, u64)],
        sources: &std::collections::HashSet<u32>,
        new_rows: u64,
        new_ts: u64,
    ) -> (Vec<u64>, Vec<u64>, Vec<u64>) {
        let old_totals = self.append_totals(table);
        let old_ts = self.append_ts(table);
        let old_copy = self.append_copy(table);
        let mut out = (Vec::new(), Vec::new(), Vec::new());
        // Walk the old boundaries next to the layout that produced them, so a
        // boundary is dropped exactly when its segment was merged away.
        let mut segment_start = 0u64;
        let mut removed = 0u64;
        let mut i = 0usize;
        for (segment, rows) in layout {
            let segment_end = segment_start + rows;
            let merged = sources.contains(segment);
            while i < old_totals.len() && old_totals[i] <= segment_end {
                let total = old_totals[i];
                if !merged && total > segment_start {
                    let shifted = total - removed;
                    if out.0.last().is_none_or(|last| *last < shifted) {
                        out.0.push(shifted);
                        out.1.push(old_ts.get(i).copied().unwrap_or(0));
                        out.2.push(old_copy.get(i).copied().unwrap_or(0));
                    }
                }
                i += 1;
            }
            if merged {
                removed += rows;
            }
            segment_start = segment_end;
        }
        let base = out.0.last().copied().unwrap_or(0);
        out.0.push(base + new_rows);
        out.1.push(new_ts);
        out.2.push(1);
        out
    }

    /// The null bitmap's coverage for a block written by `COPY`/`IMPORT`.
    ///
    /// The reference creates an imported block's column vectors with a bitmask
    /// of exactly the imported row count, so the unused bits of its last byte
    /// stay clear and rows appended to the block later get no bit at all
    /// (measured: `IMPORT` 100 + `INSERT` 10 keeps the bitmap at 100 bits).
    /// A block opened by an `INSERT` gets every bit of every touched byte.
    ///
    /// `block_start` is the block's first row in the table-wide append order,
    /// which is what `append_totals` counts (a segment's rows do not restart
    /// it).
    pub(crate) fn block_bitmap_bits(
        &self,
        table: &str,
        block_start: usize,
        rows: usize,
    ) -> Option<usize> {
        let totals = self.append_totals(table);
        let kinds = self.append_copy(table);
        // The append that wrote the block's first row.
        let idx = totals.partition_point(|total| (*total as usize) <= block_start);
        if idx >= totals.len() || kinds.get(idx).copied() != Some(1) {
            return None;
        }
        let end = (totals[idx] as usize).min(block_start + rows);
        Some(end.saturating_sub(block_start))
    }

    /// How many rows the append log says were committed at or before `bound`.
    ///
    /// The records are cumulative and in row-id order, so the committed ones are
    /// a prefix of the table's ids - which is what lets a checkpoint write a
    /// block without a hole. A record with no timestamp yet belongs to a
    /// transaction that has not finished (`reserve_rows` without the ts, settled
    /// by its bottom half), and the count stops there.
    pub(crate) fn committed_rows(&self, table: &str, bound: u64) -> usize {
        let totals = self.append_totals(table);
        let ts = self.append_ts(table);
        let mut prev = 0u64;
        let mut rows = 0usize;
        for (i, total) in totals.iter().enumerate() {
            let batch = total.saturating_sub(prev);
            prev = *total;
            match ts.get(i).copied().unwrap_or(0) {
                0 => break,
                stamp if stamp > bound => break,
                _ => rows += batch as usize,
            }
        }
        rows
    }

    /// Stamps the newest append with the commit timestamp its transaction got.
    ///
    /// The row ids are reserved while the transaction is still being staged
    /// (`reserve_rows`), so the entry exists - with no timestamp yet - by the
    /// time the commit hands one out. A rolled-back transaction leaves the zero
    /// in place, and the mirror's fallback then treats the batch as one create.
    pub(crate) fn settle_append_ts(&self, table: &str, count: usize, ts: u64) {
        let n = self.append_totals(table).len();
        if count == 0 || n == 0 {
            return;
        }
        let mut log = self.append_ts(table);
        log.resize(n.saturating_sub(count), 0);
        log.resize(n, ts);
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        tables.entry(tkey).or_default().append_ts = Some(log);
    }

    /// The imported extent of `segment`, when it still holds exactly the rows a
    /// `COPY` wrote (`None` once an `INSERT` appended to it).
    pub(crate) fn imported_extent(&self, table: &str, segment: u32, rows: u64) -> Option<u64> {
        let journals = self.ensure_journals(table);
        let _ = journals;
        let tables = self.tables.lock().unwrap();
        let entry = tables.get(&self.table_key(table))?;
        let (_, imported, _) = entry
            .imports
            .as_ref()?
            .iter()
            .find(|(id, _, _)| *id == segment)?;
        (*imported == rows).then_some(*imported)
    }

    /// After a `COPY`: every segment whose row count grew was written by this
    /// statement, so its imported extent is its current row count.
    pub(crate) fn settle_import_extent(&self, table: &str, before: &[(u32, u64)]) {
        for (segment, rows) in self.segment_row_counts(table) {
            let previous = before
                .iter()
                .find(|(id, _)| *id == segment)
                .map(|(_, r)| *r)
                .unwrap_or(0);
            if rows > previous {
                self.note_import_rows(table, segment, rows);
            }
        }
    }

    /// Every row id deleted since the table was created.
    pub(crate) fn deleted_ids(&self, table: &str) -> Vec<u64> {
        self.load_deletes(table).0
    }

    /// [`Self::deleted_ids`] as a set, built once per journal load.
    ///
    /// A bulk `DELETE`/`UPDATE` asks whether each of its rows is already gone
    /// (`row_is_present` -> `is_deleted`), and scanning the journal for every
    /// one of them made the statement quadratic in the journal's length. The
    /// set is built on the first lookup, shared (`Arc`) by every later reader,
    /// and dropped when `ensure_journals` rebuilds the journal from the blocks.
    pub(crate) fn deleted_set(&self, table: &str) -> Arc<std::collections::HashSet<u64>> {
        let key = self.table_key(table);
        {
            let tables = self.tables.lock().unwrap();
            if let Some(set) = tables
                .get(&key)
                .and_then(|entry| entry.deleted_set.as_ref())
            {
                return Arc::clone(set);
            }
        }
        // Loading the journal takes the table lock itself, so it runs outside
        // it (`ensure_journals` re-reads the block `version` files).
        let set = Arc::new(
            self.deleted_ids(table)
                .into_iter()
                .collect::<std::collections::HashSet<u64>>(),
        );
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        match entry.deleted_set {
            // Another thread built it while this one loaded: keep the first.
            Some(ref existing) => Arc::clone(existing),
            None => {
                entry.deleted_set = Some(Arc::clone(&set));
                set
            }
        }
    }

    /// The deleted row ids and their delete timestamps, both loaded together.
    ///
    /// The block `version` files say which rows are gone and when
    /// (`BlockVersion::deleted_`); the KV delete log is a cache of the same
    /// history and only contributes ids the files do not have yet. Loading
    /// both here keeps `deleted_ids` and `delete_ts` aligned by construction
    /// and outside the table lock (the block read takes it).
    pub(crate) fn load_deletes(&self, table: &str) -> (Vec<u64>, Vec<u64>) {
        let journals = self.ensure_journals(table);
        (journals.deletes, journals.delete_ts)
    }

    /// Stamps `count` delete entries starting at `first` with their commit ts.
    pub(crate) fn settle_delete_at(&self, table: &str, first: usize, count: usize, ts: u64) {
        if count == 0 || first == usize::MAX {
            return;
        }
        // One journal load for both halves: a second call would clone the
        // whole cached journal again.
        let (ids, mut log) = self.load_deletes(table);
        let n = ids.len();
        if first >= n {
            return;
        }
        log.resize(n, 0);
        for value in &mut log[first..(first + count).min(n)] {
            *value = ts;
        }
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        tables.entry(tkey).or_default().delete_ts = Some(log);
    }

    /// Stamps the last `count` deletes with the commit timestamp their
    /// transaction got (`BlockVersion::Delete(row_id, commit_ts)`).
    pub(crate) fn settle_delete_ts(&self, table: &str, count: usize, ts: u64) {
        let (ids, mut log) = self.load_deletes(table);
        let n = ids.len();
        if count == 0 || n == 0 {
            return;
        }
        log.resize(n - count.min(n), 0);
        log.resize(n, ts);
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        tables.entry(tkey).or_default().delete_ts = Some(log);
    }

    pub(crate) fn reserve_rows(
        &self,
        table: &str,
        n: usize,
        new_segment: bool,
    ) -> Result<RowRange> {
        self.reserve_rows_from(table, n, new_segment, new_segment, None)
    }

    /// [`Self::reserve_rows`] for a statement that allocates inside its own
    /// commit, where the commit ts is already known: the append record is
    /// settled at once, so a checkpoint can tell it from an append still in
    /// flight.
    ///
    /// **Always inline, never through the allocator queue.** The allocator
    /// thread exists so that row ids follow **commit order** (its doc comment
    /// in `state.rs` says so). But every caller of this function runs inside
    /// `before_wal`, which the commit chain invokes while it holds the commit-ts
    /// turn - so at most one transaction allocates at a time, in commit order,
    /// and the turn already provides the ordering the queue was there for.
    ///
    /// Routing it through the queue instead is what forced a `block_on` on a
    /// runtime worker that was holding both the turn and `commit_lock`: a
    /// parked worker holding a `tokio::sync::Mutex` guard, waiting for a reply
    /// only another worker could make. See `docs/THREADS_TO_TOKIO.md` §1.2.
    pub(crate) fn reserve_rows_at(
        &self,
        table: &str,
        n: usize,
        new_segment: bool,
        ts: u64,
    ) -> Result<RowRange> {
        // `ts` is deliberately **not** recorded on the append record here. The
        // record exists (the row ids are taken), but the rows only become
        // readable in the commit's bottom half; stamping it now would let
        // `committed_rows` count rows that are still in flight. The caller
        // settles it with `settle_append_at` once they are published, which is
        // where the reference's block keeps `created_` and its data together.
        let _ = ts;
        self.reserve_rows_inline(table, n, new_segment, new_segment, None)
    }

    /// [`Self::reserve_rows_from`] in its **async** form, for the write paths
    /// that are already async.
    ///
    /// The allocator hands out row ids in commit order, so the request is a job
    /// on one FIFO and the reply is a `oneshot`. Both ends of that are tokio;
    /// the synchronous wrapper below exists only for the tests and for
    /// `EngineTxn::append`, whose callers are not async.
    pub(crate) async fn reserve_rows_from_async(
        &self,
        table: &str,
        n: usize,
        new_segment: bool,
        copy: bool,
        ts: Option<u64>,
    ) -> Result<RowRange> {
        let Some(allocator) = self.allocator() else {
            return self.reserve_rows_inline(table, n, new_segment, copy, ts);
        };
        let (reply, answer) = tokio::sync::oneshot::channel();
        let table = table.to_string();
        // The job body runs on the allocator task and only touches memory, so
        // sending it is a bounded queue push and awaiting the reply is a
        // `oneshot` - neither holds a thread.
        allocator
            .jobs
            .send(Box::new(move |engine| {
                let _ = reply.send(engine.reserve_rows_inline(&table, n, new_segment, copy, ts));
            }))
            .await
            .map_err(|_| Error::Txn("the row allocator stopped".into()))?;
        answer
            .await
            .map_err(|_| Error::Txn("the row allocator stopped".into()))?
    }

    /// [`Self::reserve_rows`] with the append's provenance stated separately:
    /// `copy` is true for `COPY`/`IMPORT` (which may also continue a segment
    /// it already opened, so it is not the same question as `new_segment`).
    pub(crate) fn reserve_rows_from(
        &self,
        table: &str,
        n: usize,
        new_segment: bool,
        copy: bool,
        ts: Option<u64>,
    ) -> Result<RowRange> {
        crate::scheduler::block_on(self.reserve_rows_from_async(table, n, new_segment, copy, ts))
    }

    /// The allocation itself, run on the allocator thread (`TxnAllocator::
    /// Process` -> `AllocateRowID`).
    pub(crate) fn reserve_rows_inline(
        &self,
        table: &str,
        n: usize,
        new_segment: bool,
        copy: bool,
        ts: Option<u64>,
    ) -> Result<RowRange> {
        let key = self.table_key(table);
        // The segment floor is read **before** the entry is locked:
        // `next_segment_id` walks the block layout and the catalog, and both
        // reach this same table entry.
        let floor = self.next_segment_id(table);
        // The allocator's counter, resolved **before** the entry is locked:
        // `next_row_seq` reads the block layout, which reaches
        // `block_index_for` and takes the same lock.
        let mut tables = self.tables.lock().unwrap();
        let start = match tables.entry(key.clone()).or_default().row_counter {
            Some(v) => v,
            None => {
                // Resolve the first counter without the lock - the block layout
                // has to be read - then take the lock again and re-check:
                // another writer may have opened the
                // segment while this thread was reading, and both must not hand
                // out the same ids.
                drop(tables);
                let next = self.next_row_seq(table)?;
                tables = self.tables.lock().unwrap();
                match tables.entry(key.clone()).or_default().row_counter {
                    Some(v) => v,
                    None => ((next >> 32) as u32, (next & 0xffff_ffff) as u32, false),
                }
            }
        };
        let entry = tables.entry(key).or_default();
        let (mut seg, mut off, sealed) = start;
        // The C++ row id is `segment_id << 32 | offset`. A `COPY` seals the
        // segment it writes, and an `INSERT` that follows one opens a new
        // segment, while consecutive `INSERT`s keep appending to the same one —
        // which is why two `COPY`s of 99 rows yield ids 0…98 and
        // 4294967296…4294967394, but two one-row `INSERT`s yield 0 and 1.
        //
        // A `COPY` that arrives in several batches is still **one** statement
        // (the reference's `Import` writes every block in one transaction), so
        // only its first batch opens a segment; the rest continue it.
        // `off > 0` is the "this segment already holds rows" test; a segment
        // that was *claimed* but not filled yet (an import that opened it and
        // has not written its first batch) is already taken as well, or a second
        // `COPY` would reuse its ids.
        // A new segment gets the id the catalog says comes next, never
        // `seg + 1`: a compaction retires segments, and reusing a retired id
        // would hand this write the row ids, chunk names and block directory of
        // a dead segment.
        let opens_segment = new_segment && (off > 0 || entry.segments.contains_key(&seg));
        let after_sealed_segment = !copy && sealed;
        if opens_segment || after_sealed_segment {
            // Opening a segment takes the next id the catalog knows of - never
            // a retired one (`MetaTableObject::GetNextSegmentID` reads the
            // segment map, which keeps a retired entry until cleanup).
            seg = (seg + 1).max(floor);
            off = 0;
        }
        // Exactly `Self::SEGMENT_CAPACITY` rows seal a segment
        // (`catalog_cache_impl.cpp:61`): the next append opens the following
        // one, which is how a table larger than 8,388,608 rows gets its second
        // segment without any compaction.
        if off as usize >= Self::SEGMENT_ROW_CAPACITY {
            seg = (seg + 1).max(floor);
            off = 0;
        }
        let base = ((seg as usize) << 32) | off as usize;
        off = off.saturating_add(n as u32);
        entry.row_counter = Some((seg, off, copy));
        entry
            .segments
            .entry(seg)
            .and_modify(|s| s.rows = s.rows.max(off))
            .or_insert(SegmentEntry { id: seg, rows: off });
        if n > 0 {
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
        }
        drop(tables);
        // One `create` per append in the mirror's `version` file.
        let append = self.note_append(table, n, copy, ts);
        Ok(RowRange { base, append })
    }

    /// The import's first batch: opens a segment of the statement's own and
    /// hands back its cursor (`TableMeta::GetSegmentIDs1` -> `ApplySegmentIDs`
    /// in the reference, which is what keeps another writer out of it).
    pub(crate) async fn begin_import(&self, table: &str) -> Result<ImportCursor> {
        // `ApplySegmentIDs`: the statement claims a fresh segment and owns every
        // offset in it. No append record is written yet - the whole import is
        // one transaction, and its records are only created when it commits.
        let range = self
            .reserve_rows_from_async(table, 0, true, true, None)
            .await?;
        Ok(ImportCursor {
            segment: (range.base >> 32) as u32,
            offset: 0,
            batches: Vec::new(),
        })
    }

    /// The import's next batch: continues **its own** segment.
    ///
    /// The table's `row_counter` is shared by every writer, so a concurrent
    /// `INSERT` moves it on; the import must not read it again or its blocks
    /// would start at another writer's offset.
    pub(crate) async fn reserve_import_rows(
        &self,
        table: &str,
        n: usize,
        cursor: ImportCursor,
    ) -> Result<(RowRange, ImportCursor)> {
        let Some(allocator) = self.allocator() else {
            return Ok(self.reserve_import_rows_inline(table, n, cursor));
        };
        let (reply, answer) = tokio::sync::oneshot::channel();
        let table = table.to_string();
        allocator
            .jobs
            .send(Box::new(move |engine| {
                let _ = reply.send(engine.reserve_import_rows_inline(&table, n, cursor));
            }))
            .await
            .map_err(|_| Error::Txn("the row allocator stopped".into()))?;
        answer
            .await
            .map_err(|_| Error::Txn("the row allocator stopped".into()))
    }

    pub(crate) fn reserve_import_rows_inline(
        &self,
        table: &str,
        n: usize,
        cursor: ImportCursor,
    ) -> (RowRange, ImportCursor) {
        let base = ((cursor.segment as usize) << 32) | cursor.offset as usize;
        let offset = cursor.offset.saturating_add(n as u32);
        {
            let key = self.table_key(table);
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            entry
                .segments
                .entry(cursor.segment)
                .and_modify(|s| s.rows = s.rows.max(offset))
                .or_insert(SegmentEntry {
                    id: cursor.segment,
                    rows: offset,
                });
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
        }
        let mut batches = cursor.batches.clone();
        batches.push(n);
        (
            RowRange {
                base,
                append: usize::MAX,
            },
            ImportCursor {
                segment: cursor.segment,
                offset,
                batches,
            },
        )
    }

    /// Records a write batch (`COPY`/`INSERT`) against `table`.
    pub(crate) fn bump_writes(&self, table: &str) {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .write_batches += 1;
    }

    /// True when `table` has been written in more than one batch.
    pub(crate) fn multi_segment(&self, table: &str) -> bool {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.write_batches)
            .unwrap_or(0)
            >= 2
    }

    pub(crate) fn table_meta(&self, table: &str) -> Result<catalog::TableMeta> {
        self.storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))
    }

    /// `(column, index name)` pairs of `table`, for the EXPLAIN regex rewrite.
    /// The analyzer recorded for the index named `index_name` on `table`, i.e.
    /// the analyzer of the column that index covers.
    pub(crate) fn analyzer_of_index(&self, table: &str, index_name: &str) -> Option<String> {
        let key = self.table_key(table);
        if let Some(a) = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.analyzers_by_index.get(index_name).cloned())
        {
            return Some(a);
        }
        let column = self
            .storage
            .catalog
            .index_by_name(table, index_name)
            .ok()??
            .column;
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.analyzers.get(&column).cloned())
    }

    /// `(column, min_n, max_n, fold)` for every full-text index whose analyzer
    /// is a sparse gram analyzer. Only these indexes can consume the literal
    /// runs of a `regex()` predicate (C++ `SparseGramParams`).
    pub(crate) fn sparsegram_index_params(
        &self,
        table: &str,
    ) -> Result<Vec<(String, u32, u32, bool)>> {
        let mut out = Vec::new();
        for im in self.storage.catalog.indexes_of_table(table)? {
            if !im.index_type.eq_ignore_ascii_case("FULLTEXT") {
                continue;
            }
            let Some(analyzer) = im
                .options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case("analyzer"))
                .map(|(_, v)| v)
            else {
                continue;
            };
            if let Some((min_n, max_n, fold)) =
                crate::regex_literal::parse_sparsegram_analyzer(analyzer)
            {
                out.push((im.column, min_n, max_n, fold));
            }
        }
        Ok(out)
    }

    /// Columns of `table` that carry any index, for the `EXPLAIN` index/leftover
    /// filter split.
    pub(crate) fn indexed_columns(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .storage
            .catalog
            .indexes_of_table(table)?
            .into_iter()
            .map(|i| i.column)
            .collect())
    }

    /// Begins a multi-statement transaction (`NewTxn`). Its writes are staged
    /// until [`EngineTxn::commit`].
    pub fn begin_txn(&self) -> EngineTxn<'_> {
        EngineTxn {
            engine: self,
            handle: self.engine_handle(),
            txn: Some(self.txn.begin()),
            appends: Vec::new(),
            deletes: Vec::new(),
            dumps: Vec::new(),
        }
    }

    /// `DESCRIBE <table>`: the same four columns as the reference's
    /// `SHOW TABLE … COLUMNS` (`name`, `type`, `default`, `comment`), in
    /// definition order.
    pub(crate) fn describe_table(&self, table: &str) -> Result<QueryResult> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let rows: Vec<Vec<String>> = (0..meta.columns.len())
            .map(|i| {
                let ty = meta.types.get(i).cloned().unwrap_or_default();
                vec![
                    meta.columns[i].clone(),
                    describe_type(&ty),
                    render_column_default(meta.defaults.get(i).and_then(|d| d.as_ref()), &ty),
                    meta.comments
                        .get(i)
                        .cloned()
                        .flatten()
                        .unwrap_or_else(|| "(empty)".to_string()),
                ]
            })
            .collect();
        Ok(QueryResult::Rows {
            columns: vec![
                "name".to_string(),
                "type".to_string(),
                "default".to_string(),
                "comment".to_string(),
            ],
            rows: text_rows(rows),
        })
    }

    /// `AppendInBlock` for the rows a statement just committed.
    ///
    /// The batch's values are handed to the block writer directly: the block is
    /// where they live, so there is no `row:` key to read them back from.
    pub(crate) fn publish_rows(
        &self,
        table: &str,
        base: usize,
        rows: &[Vec<Literal>],
    ) -> Result<()> {
        let defaults = self.table_defaults(table).unwrap_or_default();
        let types = self.column_types(table)?;
        let mut published: Vec<(usize, Vec<Value>)> = Vec::with_capacity(rows.len());
        for (k, row) in rows.iter().enumerate() {
            // Through the stored text form, which is what the block writer and
            // the reader agree on: an array literal's `{…}` is normalized to
            // the stored `[…]` only on that path.
            let text = self.encode_row_stored(table, row, &types)?;
            if let Ok(values) = self.parse_stored_row(table, &defaults, &types, text.as_bytes()) {
                published.push((base + k, values));
            }
        }
        self.refresh_block_mirror_extra(table, &published)
    }

    /// [`Self::publish_rows`] from the stored text form.
    ///
    /// An import streams its batches through one transaction, so it keeps the
    /// encoded `(row id, text)` pairs rather than a second copy of the parsed
    /// rows; they are decoded here, once the commit has finished with them.
    pub(crate) fn publish_encoded_rows<T: AsRef<[u8]>>(
        &self,
        table: &str,
        rows: &[(usize, T)],
    ) -> Result<()> {
        if rows.is_empty() {
            return Ok(());
        }
        let defaults = self.table_defaults(table).unwrap_or_default();
        let types = self.column_types(table).unwrap_or_default();
        // Batch by batch: holding the parsed values of the whole file next to
        // its stored text doubled the peak (4.6 GB resident for a 326 MB
        // import). Each batch is parsed, appended to the buffer and dropped.
        const CHUNK: usize = 8192;
        let mut published: Vec<(usize, Vec<Value>)> = Vec::with_capacity(CHUNK.min(rows.len()));
        for (seq, text) in rows {
            match self.parse_stored_row(table, &defaults, &types, text.as_ref()) {
                Ok(values) => published.push((*seq, values)),
                // Never drop a row because its text form would not parse: the
                // cells are simply that text, which is what the writer stores.
                Err(_) => {
                    let raw = String::from_utf8_lossy(text.as_ref()).into_owned();
                    let cells: Vec<Value> =
                        raw.split('\t').map(|c| Value::Str(c.to_string())).collect();
                    published.push((*seq, cells));
                }
            }
            if published.len() >= CHUNK {
                self.refresh_block_mirror_extra(table, &published)?;
                published.clear();
            }
        }
        if !published.is_empty() {
            self.refresh_block_mirror_extra(table, &published)?;
        }
        Ok(())
    }

    /// The `(db_id, table_id, table_key)` of a table from its catalog key.
    pub(crate) fn table_ident(&self, table: &str) -> Result<Option<(String, String, String)>> {
        let bare = table.rsplit('.').next().unwrap_or(table);
        for (k, v) in self.storage.catalog.store().scan_prefix(b"catalog|tbl|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            if parts.len() >= 5 && parts[3] == bare {
                return Ok(Some((
                    parts[2].to_string(),
                    String::from_utf8_lossy(&v).into_owned(),
                    key,
                )));
            }
        }
        Ok(None)
    }

    /// The table's own catalog key (`catalog|tbl|{db_id}|{name}|{ts}`), which
    /// the reference carries as `table_key` in `CreateIndexV2`. Empty when the
    /// table has no layout record yet.
    pub(crate) fn table_catalog_key(&self, table: &str) -> Result<String> {
        // `table` may be qualified (`default_db.wdldl`); the catalog key holds
        // the bare name.
        let bare = table.rsplit('.').next().unwrap_or(table);
        for (k, _v) in self.storage.catalog.store().scan_prefix(b"catalog|tbl|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            if parts.len() >= 5 && parts[3] == bare {
                return Ok(key);
            }
        }
        Ok(String::new())
    }

    /// Turns the captured batch into the reference's typed WAL command (`WalCmdCreateTableV2`,
    /// `WalCmdCreateIndexV2`, …). The catalog writes are still applied to the
    /// store by `commit_at`; replay deliberately skips typed DDL (the catalog
    /// is durable at commit), so this only changes the WAL's bytes.
    pub(crate) async fn ddl_commit_typed<T>(
        &self,
        op: TxnOp,
        f: impl FnOnce() -> Result<T>,
        typed_builder: impl FnOnce(&[(Vec<u8>, Option<Vec<u8>>)]) -> Option<storage::WalCmd>,
    ) -> Result<T> {
        // One DDL at a time: a second capture would interleave two statements'
        // layout rewrites, and only one capture slot exists.
        let _ddl = self.ddl_lock.lock().await;
        let (out, mut batch) = self.storage.catalog.capture(f)?;
        // A statement that changes nothing must not spend a timestamp. The
        // reference commits such a transaction through `GetReadCommitTS` — an
        // odd ts, no WAL entry — so `DROP TABLE IF EXISTS <missing>` leaves
        // every later ts where it was. The C++-layout rewrite always emits the
        // whole layout (the sweep deletes it, the rewrite writes it back), so
        // "nothing changed" has to be decided by comparing the batch with the
        // store: a put that would write the same bytes and a delete of a key
        // that is already absent are both no-ops.
        if batch.is_empty() || self.batch_is_noop(&batch)? {
            return Ok(out);
        }
        let turn = self.txn.reserve_commit_ts();
        let commit_ts = turn.ts();
        Catalog::restamp(&mut batch, commit_ts);
        let typed = typed_builder(&batch);
        let mut txn = self.txn.begin();
        // The reference's store type: the conflict matrix reads it, so a DDL
        // meets the appends/updates/compactions that overlap with it
        // (`NewTxn::CheckConflictTxnStore`).
        txn.set_op(op);
        for (key, value) in batch {
            match value {
                Some(value) => txn.put(key, value),
                None => txn.delete(key),
            }
        }
        if let Some(cmd) = typed {
            txn.set_typed_commands(vec![cmd]);
        }
        // A DDL's catalog method returns before its transaction commits (it
        // hands the batch back for the WAL and the commit), so the cache has to
        // be dropped again here: a reader between the two would otherwise
        // repopulate it with the pre-commit schema.
        let committed = self.txn.commit_at(txn, commit_ts).await;
        self.storage.catalog.invalidate_tables();
        committed?;
        // The catalog records this DDL wrote are made durable by the commit
        // (`apply_split` flushes the store the moment it takes a batch without
        // RocksDB's log). A directory with `checkpoint_interval = "0s"` keeps
        // its schema across a `kill -9` for that reason, as the reference's
        // does by replaying the typed DDL command.
        Ok(out)
    }

    /// [`Self::ddl_commit_typed`] for a statement whose data side effects must
    /// not exist before its transaction is published.
    ///
    /// `stage` computes the catalog batch **and** the plan for the effects; it
    /// runs inside the capture, so it sees the schema the statement is
    /// creating. `effects` runs after the conflict check and the WAL write, and
    /// before `finish_bottom` makes the commit visible: a statement that loses
    /// a conflict leaves no buffer cell and no `{id}.col` behind, which is what
    /// lets two DDLs commit without serialising their whole commit phase.
    pub(crate) async fn ddl_commit_with_effects<T, E, Fut>(
        &self,
        op: TxnOp,
        stage: impl FnOnce() -> Result<(T, E)>,
        effects: impl FnOnce(E) -> Fut,
        typed_builder: impl FnOnce(&[(Vec<u8>, Option<Vec<u8>>)]) -> Option<storage::WalCmd>,
    ) -> Result<T>
    where
        Fut: std::future::Future<Output = Result<()>>,
    {
        // Only the capture is serialised (`catalog.capture` has one overlay and
        // one slot); the commit phase is ordered by the transaction manager.
        let ((out, plan), mut batch) = {
            let _ddl = self.ddl_lock.lock().await;
            self.storage.catalog.capture(stage)?
        };
        if batch.is_empty() || self.batch_is_noop(&batch)? {
            return Ok(out);
        }
        let turn = self.txn.reserve_commit_ts();
        let commit_ts = turn.ts();
        Catalog::restamp(&mut batch, commit_ts);
        let typed = typed_builder(&batch);
        let mut txn = self.txn.begin();
        txn.set_op(op);
        for (key, value) in batch {
            match value {
                Some(value) => txn.put(key, value),
                None => txn.delete(key),
            }
        }
        if let Some(cmd) = typed {
            txn.set_typed_commands(vec![cmd]);
        }
        let committed = self.txn.commit_at_deferred(txn, commit_ts).await;
        self.storage.catalog.invalidate_tables();
        let ts = committed?;
        // The commit stays invisible until its data is in place; the publish
        // runs even when the effects fail, or the commit barrier would wait for
        // a transaction that already owns a timestamp.
        let effect_result = effects(plan).await;
        self.txn.finish_bottom(ts);
        effect_result?;
        Ok(out)
    }

    /// Registers a statement with the query registry and returns the guard that
    /// deregisters it. C++ `SessionManager::AddQueryRecord` + `RemoveQueryRecord`.
    pub(crate) fn begin_query<'a>(&'a self, stmt: &Statement, sql: &str) -> QueryGuard<'a> {
        let kind = diagnostics::statement_kind(stmt).to_string();
        let text = sql.trim().to_string();
        let started = std::time::Instant::now();
        let started_wall = std::time::SystemTime::now();
        let query_id = {
            let mut registry = self.queries.lock().unwrap();
            let query_id = registry.next_id();
            registry.add_running(diagnostics::RunningQuery {
                query_id,
                query_kind: kind.clone(),
                query_text: text.clone(),
                started,
                started_wall,
            });
            query_id
        };
        QueryGuard {
            engine: self,
            query_id,
            query_kind: kind,
            query_text: text,
            started,
        }
    }

    /// Executes a `SELECT` and returns its typed rows. The public
    /// [`QueryResult`] renders cells as text, but `INSERT … SELECT` and
    /// `CREATE TABLE … AS SELECT` must preserve values (and NULLs) before
    /// casting them to the target schema.
    pub(crate) fn run_select(&self, stmt: &Statement) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        self.run_select_with_ctes(stmt, &HashMap::new())
    }

    /// [`Self::run_select`] with the CTEs bound by an enclosing `WITH` clause.
    /// Each binding is materialised in order, so a later CTE (and the outer
    /// query) can scan an earlier one.
    pub(crate) fn run_select_with_ctes(
        &self,
        stmt: &Statement,
        outer_ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
    ) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        let mut cte_scope = outer_ctes.clone();
        if let Statement::Select { ctes, .. } = stmt {
            for cte in ctes {
                // Only the bindings that precede this CTE are visible inside
                // it, plus anything inherited from an enclosing query.
                let visible: Vec<String> = cte_scope.keys().cloned().collect();
                let query = self.qualify_tables_in(cte.query.as_ref(), &visible);
                let (mut columns, rows) = self.run_select_with_ctes(&query, &cte_scope)?;
                if !cte.columns.is_empty() {
                    if cte.columns.len() != columns.len() {
                        return Err(Error::Invalid(format!(
                            "WITH query '{}' has {} column(s) available but {} specified",
                            cte.alias,
                            columns.len(),
                            cte.columns.len()
                        )));
                    }
                    columns = cte.columns.clone();
                }
                cte_scope.insert(cte.alias.to_ascii_lowercase(), (columns, rows));
            }
        }
        let Statement::Select {
            items,
            from,
            join,
            where_pred,
            group_by,
            aliases,
            having,
            order_by,
            limit,
            offset,
            limit_expr,
            offset_expr,
            search,
            distinct,
            ..
        } = stmt
        else {
            return Err(Error::Invalid("run_select requires a SELECT".into()));
        };
        // A non-literal LIMIT/OFFSET is evaluated here, before the fast paths
        // below inspect the resolved values.
        let limit_value: Option<u64> = match (limit, limit_expr) {
            (Some(n), _) => Some(*n),
            (None, Some(e)) => Some(self.eval_limit_expr(e)?),
            _ => None,
        };
        let offset_value: Option<u64> = match (offset, offset_expr) {
            (Some(n), _) => Some(*n),
            (None, Some(e)) => Some(self.eval_limit_expr(e)?),
            _ => None,
        };
        // `SELECT count(*)` with no other clause is answered from the key
        // counts alone; the generic path materialises every row.
        if join.is_none()
            && where_pred.is_none()
            && group_by.is_empty()
            && having.is_none()
            && order_by.is_empty()
            && limit_value.is_none()
            && offset_value.is_none()
            && search.is_none()
            && !*distinct
            && items.len() == 1
            && aliases.len() <= 1
        {
            if let parser::SelectItem::Aggregate(a) = &items[0] {
                // `AggFunc` is not re-exported; the debug name is stable.
                if format!("{:?}", a.func) == "Count" && a.args.is_empty() && !a.distinct {
                    if let Some(t) = from.as_ref() {
                        let n = self.row_count(t)?;
                        let name = aliases
                            .first()
                            .cloned()
                            .flatten()
                            .unwrap_or_else(|| "count(star)".to_string());
                        return Ok((vec![name], vec![vec![Value::Int(n as i64)]]));
                    }
                }
            }
        }
        // `FROM (SELECT …)`: run the subquery first and scan its result. A
        // `FROM <cte>` uses the rows materialised above the same way.
        let derived: Option<(Vec<String>, Vec<Vec<Value>>)> = match from.as_ref() {
            Some(f) if f.trim_start().to_ascii_uppercase().starts_with("SELECT ") => {
                Some(self.derived_table(f.trim())?)
            }
            Some(f) => cte_scope.get(&f.to_ascii_lowercase()).cloned(),
            None => None,
        };
        // Scalar/EXISTS subqueries are rewritten before planning: the planner
        // has no access to the storage engine. The enclosing relation's columns
        // are the scope a nested query may correlate with - a derived table's
        // or a CTE's columns, else the base table's.
        let outer_columns = match (&derived, from.as_ref()) {
            (Some((columns, _)), _) => columns.clone(),
            (None, Some(table)) => self
                .table_schema(&self.table_key(table))
                .unwrap_or_default(),
            (None, None) => Vec::new(),
        };
        // A correlated subquery becomes a dependent join: every nested query
        // that reads this relation's columns is rewritten into a hidden scan
        // column, filled in below once the outer rows are materialised (see
        // `dependent.rs`). Uncorrelated nested queries keep going through the
        // constant folder.
        let dependent = crate::dependent::plan(self, stmt, &outer_columns, &cte_scope)?;
        let stmt: &Statement = dependent.as_ref().map(|d| &d.stmt).unwrap_or(stmt);
        let resolved = self.resolve_stmt_subqueries(stmt, &cte_scope, &outer_columns)?;
        let stmt: &Statement = resolved.as_ref().unwrap_or(stmt);
        // The rewritten statement is the one that gets planned; its clauses
        // shadow the parsed ones (only the nested queries changed).
        let Statement::Select {
            items,
            where_pred,
            group_by,
            having,
            order_by,
            ..
        } = stmt
        else {
            unreachable!("run_select checked the statement is a SELECT");
        };
        // `SELECT … UNNEST(col) …`: expand an array/JSON column into one
        // row per element, then let the normal planner handle WHERE,
        // GROUP BY and aggregates over the expanded relation.
        let mut where_pred = where_pred.clone();
        let mut group_by = group_by.to_vec();
        let mut having = having.clone();
        let mut items_owned: Vec<SelectItem> = items.to_vec();
        let mut unnest_expansion: Option<(String, Vec<Vec<Value>>)> = None;
        let unnest_idx = items_owned.iter().position(|it| {
            matches!(it, SelectItem::Func { name, .. }
                if name.eq_ignore_ascii_case("UNNEST"))
        });
        if let (Some(t), Some(ui)) = (from.as_ref(), unnest_idx) {
            let (arg_col, alias) = match &items_owned[ui] {
                SelectItem::Func { args, .. } => (
                    match args.first() {
                        Some(ExprAst::Column(c)) => Some(c.clone()),
                        _ => None,
                    },
                    aliases.get(ui).cloned().flatten(),
                ),
                _ => (None, None),
            };
            if let Some(col) = arg_col {
                let schema = self.table_schema(t)?;
                if let Some(ci) = schema.iter().position(|c| c == &col) {
                    let col_ty = self.column_types(t)?.get(ci).cloned().unwrap_or_default();
                    let float_elems = array_elem_is_float(&col_ty);
                    let is_json_col = is_json_type(&col_ty);
                    let new_name = alias.unwrap_or_else(|| format!("__unnest_{col}"));
                    let mut out: Vec<Vec<Value>> = Vec::new();
                    for (_seq, row) in self.fetch_rows_with_ids(t)? {
                        let mut elements: Vec<Value> = Vec::new();
                        // a JSON column's stored text may have been read as
                        // a number, so re-render it before parsing
                        let cell: Option<String> = match row.get(ci) {
                            Some(Value::Str(c)) => Some(c.clone()),
                            Some(other) if is_json_col => Some(other.to_slt_string()),
                            _ => None,
                        };
                        if let Some(cell) = cell {
                            if !is_json_col {
                                if let Some(elems) = brace_array_elems(&cell) {
                                    for e in elems {
                                        let text = if float_elems { fmt_float6(&e) } else { e };
                                        elements.push(parse_value(&text));
                                    }
                                }
                            } else {
                                // JSON elements keep their textual form
                                match expression::json::parse(&cell) {
                                    Some(expression::json::Json::Arr(items)) => {
                                        for v in items {
                                            elements.push(Value::Str(v.compact()));
                                        }
                                    }
                                    Some(other) => elements.push(Value::Str(other.compact())),
                                    None => {}
                                }
                            }
                        }
                        for v in elements {
                            let mut r = row.clone();
                            r.push(v);
                            out.push(r);
                        }
                    }
                    // Filters written against the output expression
                    // (`uc2 > 1` is sent as `unnest(c2) > 1`) must see
                    // the expanded column.
                    if let Some(p) = where_pred.as_ref() {
                        where_pred = Some(rewrite_unnest_pred(p, &col, &new_name));
                    }
                    if let Some(p) = having.as_ref() {
                        having = Some(Box::new(rewrite_unnest_pred(p, &col, &new_name)));
                    }
                    group_by = group_by
                        .iter()
                        .map(|g| rewrite_unnest_expr(g, &col, &new_name))
                        .collect();
                    items_owned[ui] = SelectItem::Column(new_name.clone());
                    unnest_expansion = Some((new_name, out));
                }
            }
        }
        let items: &[SelectItem] = &items_owned;
        // `FROM (SELECT …)`: run the subquery first and scan its result.
        // Its columns become a synthetic table so names resolve. A `FROM <cte>`
        // uses the rows materialised above the same way.
        let from_derived: Option<String> = if derived.is_some() {
            Some("__derived".to_string())
        } else {
            from.clone()
        };
        // The name a nested query uses for this relation: the CTE alias or the
        // base table (the derived-table alias is not in the AST, so a derived
        // table's columns are referenced unqualified).
        let outer_table_name = from.clone().unwrap_or_default();
        let from = &from_derived;
        // A WHERE clause must be a predicate: `WHERE c1` on a
        // non-boolean column is rejected by the reference.
        if let (Some(t), Some(p), None) = (from.as_ref(), where_pred.as_ref(), derived.as_ref()) {
            self.validate_where_predicate(t, p)?;
        }
        // SUM/AVG require a numeric argument.
        if let (Some(t), None) = (from.as_ref(), derived.as_ref()) {
            self.validate_aggregate_args(t, items)?;
        }
        // `AND`/`OR`/`NOT` take boolean operands: a projected
        // arithmetic expression such as `c1 + 0.1 and c2 - 1.0` is
        // invalid (the reference rejects it).
        for it in items {
            match it {
                SelectItem::Expr(e) => validate_boolean_expr(e)?,
                SelectItem::Func { args, .. } => {
                    for a in args {
                        validate_boolean_expr(a)?;
                    }
                }
                _ => {}
            }
        }
        if let (Some(_), Some(s)) = (from.as_ref(), search.as_deref()) {
            validate_search_pseudo_columns(s, items)?;
            // an outer WHERE cannot be combined with an in-subsearch filter
            if where_pred.is_some() && s.matches.iter().any(|m| m.filter.is_some()) {
                return Err(Error::Invalid(
                    "cannot have both a where clause and a subsearch filter".into(),
                ));
            }
        }
        let mut tables: Vec<(String, Vec<String>)> = Vec::new();
        if let Some(t) = from {
            let cols = match &derived {
                Some((cols, _)) => cols.clone(),
                None => self.table_schema(t)?,
            };
            tables.push((t.clone(), cols));
        }
        let mut unnest_rows: Option<Vec<Vec<Value>>> = None;
        if let Some((name, rows)) = unnest_expansion {
            if let Some(entry) = tables.first_mut() {
                entry.1.push(name);
            }
            unnest_rows = Some(rows);
        }
        if let Some(JoinClause { table, .. }) = join {
            tables.push((table.clone(), self.table_schema(table)?));
        }
        // SEARCH MATCH: run the search first, then expose its result rows
        // (plus hidden ROW_ID/SIMILARITY/DISTANCE/SCORE columns) as the scan.
        // A derived table's rows are the scan source from the start.
        let mut override_rows: Option<Vec<Vec<Value>>> = derived.map(|(_, rows)| rows);
        if let (Some(t), Some(s)) = (from.as_ref(), search.as_deref()) {
            let pred = match where_pred.as_ref() {
                Some(p) => Some(planner::predicate_expr(p, &tables[0].1)?),
                None => None,
            };
            let (rows, hidden) = self.run_search(t, s, pred.as_ref())?;
            if !hidden.is_empty() {
                if let Some(entry) = tables.first_mut() {
                    entry.1.extend(hidden);
                }
            }
            override_rows = Some(rows);
        }
        // Projection-only scan: read just the selected columns from the
        // C++ block mirror when that mirror is as fresh as the row
        // store (plan #87). `SELECT id, name` then avoids decoding the
        // wide columns (embeddings, text) altogether.
        // The projection-only scan reads just the selected columns, so a row it
        // returns has neither the dependent join's hidden columns nor the
        // width the hidden-value pass expects.
        if override_rows.is_none()
            && dependent.is_none()
            && join.is_none()
            && search.is_none()
            && where_pred.is_none()
            && group_by.is_empty()
            && having.is_none()
            && order_by.is_empty()
            && limit_value.is_none()
            && offset_value.is_none()
            && !*distinct
        {
            if let Some(t) = from.as_ref() {
                override_rows = self.try_projection(t, items)?;
            }
        }
        // `ROW_ID()` outside a SEARCH exposes the row's sequence number
        // as a hidden scan column.
        let mut rowid_col = false;
        if search.is_none() {
            if let Some(t) = from.as_ref() {
                if unnest_rows.is_none() && uses_row_id(items) {
                    if let Some(entry) = tables.first_mut() {
                        entry.1.push("#_row_id".into());
                    }
                    let rows: Vec<Vec<Value>> = self
                        .fetch_rows_with_ids(t)?
                        .into_iter()
                        .map(|(seq, mut r)| {
                            r.push(Value::Int(seq as i64));
                            r
                        })
                        .collect();
                    override_rows = Some(rows);
                    rowid_col = true;
                }
            }
        }
        let _ = rowid_col;
        // An ORDER BY key that names a select alias is rewritten to the
        // corresponding output label.
        let order_keys: Vec<parser::OrderKey> = order_by
            .iter()
            .map(|ok| {
                let mut ok = ok.clone();
                if let Some(i) = aliases
                    .iter()
                    .position(|a| a.as_deref() == Some(ok.column.as_str()))
                {
                    if let Some(item) = items.get(i) {
                        ok.column = planner::item_label(item);
                    }
                }
                ok
            })
            .collect();
        if unnest_rows.is_some() {
            override_rows = unnest_rows;
        }
        // The dependent join's value columns belong to the scanned relation, so
        // the statements rewritten by `dependent.rs` resolve them like any
        // other column (the same way `#_row_id` and the search columns work).
        if let Some(dep) = dependent.as_ref() {
            if let Some(entry) = tables.first_mut() {
                entry.1.extend(dep.column_names());
            }
        }
        // C++ `QueryBinder::BindSelect` is the entry point that turns a parsed
        // `SELECT` into a bound plan; it owns the clause ordering.
        let mut plan = planner::binder::QueryBinder::new()
            .bind_select(
                items,
                &tables,
                join.as_ref(),
                where_pred.as_ref(),
                &group_by,
                having.as_deref(),
                &order_keys,
                limit_value,
                offset_value,
            )?
            .plan;
        // `SELECT sum(c1) AS s` names the output column `s`.
        apply_select_aliases(&mut plan, aliases);
        // A full-text `threshold=` filter is answered by the index: the
        // score needs corpus statistics a row-local predicate cannot see,
        // so resolve the qualifying rows once and feed them down.
        if override_rows.is_none() && join.is_none() {
            if let (Some(t), Some(p)) = (from.as_ref(), where_pred.as_ref()) {
                if let Some((col, query, packed)) = fulltext_index_call(p) {
                    let (opts, hint) = packed.split_once('\u{1}').unwrap_or((&packed, ""));
                    // Only a `threshold=` filter (needs corpus stats) or a
                    // query targeted at a gram index (needs substring
                    // semantics) departs from the row-local term match.
                    let sparsegram = !hint.is_empty()
                        && self
                            .analyzer_of_index(t, hint)
                            .map(|a| a.to_ascii_lowercase().starts_with("sparsegram"))
                            .unwrap_or(false);
                    let has_threshold = opts
                        .split(';')
                        .any(|o| o.trim().to_ascii_lowercase().starts_with("threshold="));
                    if !sparsegram && !has_threshold {
                        override_rows = None;
                    } else if sparsegram {
                        let schema = self.table_schema(t)?;
                        let needle = query.trim().trim_matches('"').to_lowercase();
                        let keep: Vec<u64> = match schema.iter().position(|c| c == &col) {
                            None => Vec::new(),
                            Some(ci) => self
                                .fetch_rows_with_ids(t)?
                                .into_iter()
                                .filter(|(_, r)| match r.get(ci) {
                                    Some(Value::Str(s)) => s.to_lowercase().contains(&needle),
                                    Some(v) => v.to_slt_string().to_lowercase().contains(&needle),
                                    None => false,
                                })
                                .map(|(id, _)| id as u64)
                                .collect(),
                        };
                        let mut out = self.fetch_rows_by_ids(t, &keep)?;
                        if rowid_col {
                            for (r, id) in out.iter_mut().zip(keep.iter()) {
                                r.push(Value::Int(*id as i64));
                            }
                        }
                        override_rows = Some(out);
                    } else {
                        let m = MatchClause {
                            kind: "TEXT".into(),
                            column: col,
                            args: vec![
                                ExprAst::Literal(Literal::Str(query)),
                                ExprAst::Literal(Literal::Str(opts.to_string())),
                            ],
                            filter: None,
                        };
                        let (rows, _) = self.search_text_terms(t, &m, None, &[])?;
                        let mut ids: Vec<u64> = rows
                            .iter()
                            .filter_map(|r| match r.iter().rev().nth(3) {
                                Some(Value::Int(i)) => Some(*i as u64),
                                _ => None,
                            })
                            .collect();
                        // Table order, not score order: this is a WHERE filter.
                        ids.sort_unstable();
                        let mut out = self.fetch_rows_by_ids(t, &ids)?;
                        if rowid_col {
                            for (r, id) in out.iter_mut().zip(ids.iter()) {
                                r.push(Value::Int(*id as i64));
                            }
                        }
                        override_rows = Some(out);
                    }
                }
            }
        }
        // index scan: an equality predicate on an indexed single-table
        // column fetches only the matching rows.
        if override_rows.is_none() && join.is_none() {
            if let (Some(t), Some(pred)) = (from.as_ref(), where_pred.as_ref()) {
                if let Some((col, CmpOp::Eq, val)) = pred.as_column_literal() {
                    if let Some(ids) = self.index_lookup(t, col, val) {
                        let mut rows = self.fetch_rows_by_ids(t, &ids)?;
                        if rowid_col {
                            for (r, id) in rows.iter_mut().zip(ids.iter()) {
                                r.push(Value::Int(*id as i64));
                            }
                        }
                        override_rows = Some(rows);
                    }
                }
            }
        }
        // C++ `ApplyFastRoughFilter` then `IndexScanBuilder`, the first two
        // optimizer rules. Both need the table's index catalog, which the
        // planner does not own, so the executor binds it to this scan.
        let mut candidate = None;
        let mut unmatchable = false;
        let mut fulltext_substring = false;
        let mut rough_filter_done = false;
        // The scan's catalog, kept so the production optimizer can run the
        // reference's `ApplyFastRoughFilter` rule against the real indexes
        // instead of the empty one `FastRoughFilterRule::default()` carries.
        let mut scan_catalog: Option<planner::index_scan::IndexCatalog> = None;
        let scan_source = plan_scan_of(&plan);
        if join.is_none() && override_rows.is_none() {
            if let (Some(t), Some((_, scan_columns))) = (from.as_ref(), scan_source.as_ref()) {
                let catalog = self.index_catalog(t, scan_columns);
                scan_catalog = Some(catalog.clone());
                unmatchable = planner::index_scan::ApplyFastRoughFilter::build(&plan, &catalog)
                    .iter()
                    .any(|binding| {
                        binding.evaluator.tag()
                            == planner::index_scan::FastRoughFilterTag::AlwaysFalse
                    });
                if !unmatchable {
                    // The reference's first two rules, in its order
                    // (`optimizer_impl.cpp:40-41`): the rough filter, then the
                    // index scan. The rule rewrites the plan into an
                    // `IndexScan` node that owns the evaluator; the executor
                    // reads it back from there.
                    let regex = self.regex_index_catalog(t, scan_columns);
                    let mut index_optimizer = planner::optimize::Optimizer::new();
                    index_optimizer.add_rule(Box::new(
                        planner::optimize::FastRoughFilterRule::new(catalog.clone()),
                    ));
                    index_optimizer.add_rule(Box::new(planner::index_scan::IndexScanRule::new(
                        catalog.clone(),
                        Some(regex.clone()),
                    )));
                    plan = index_optimizer.optimize(plan)?;
                    rough_filter_done = true;
                    if let Some(pushed) = plan_index_candidate(&plan) {
                        // A sparse-gram index answers a literal run by
                        // substring; a term index answers by token.
                        fulltext_substring = fulltext_options(&pushed.index_filter)
                            .is_some_and(|opts| opts.to_ascii_lowercase().contains("index="))
                            && fulltext_column(&pushed.index_filter)
                                .is_some_and(|i| regex.columns.contains_key(&i));
                        candidate = Some((*pushed).clone());
                    }
                }
            }
        }
        // index scan: the planner already merged the predicate into per-column
        // intervals (`Secondary { ranges }`), so an integer range is answered
        // from the sealed chunks and the tail instead of materialising the
        // whole table. This has to run after `IndexScanBuilder` (the candidate
        // only exists from here on) and before `run_plan`; `=` never gets here
        // (see `index_scan_range_rows`).
        if override_rows.is_none() && join.is_none() {
            if let (Some(t), Some((_, scan_columns)), Some(candidate)) =
                (from.as_ref(), scan_source.as_ref(), candidate.as_ref())
            {
                if let Some(ids) = self.index_scan_range_rows(t, &candidate.evaluator, scan_columns)
                {
                    let mut rows = self.fetch_rows_by_ids(t, &ids)?;
                    if rowid_col {
                        for (r, id) in rows.iter_mut().zip(ids.iter()) {
                            r.push(Value::Int(*id as i64));
                        }
                    }
                    override_rows = Some(rows);
                }
            }
        }
        // The inner side of the dependent join runs once per distinct key over
        // the materialised outer relation; each row then carries the result as
        // an ordinary column, so `WHERE`, `HAVING`, `ORDER BY` and the
        // projection see it through the regular path.
        if let Some(dep) = dependent.as_ref() {
            // With a join the hidden columns belong to the FROM side, which is
            // what the join source consumes as its left input.
            let Some(table) = from.as_ref() else {
                return Err(Error::Invalid(
                    "correlated subquery needs a source relation".into(),
                ));
            };
            // `tables` already carries the enclosing relation's columns (a CTE
            // or a derived table has no catalog entry), and a derived table's
            // rows are the scan source from the start. The dependent join's
            // hidden names were appended last: the rows do not have them yet.
            let mut schema = tables
                .first()
                .map(|(_, columns)| columns.clone())
                .unwrap_or_default();
            schema.truncate(schema.len().saturating_sub(dep.column_names().len()));
            let mut rows = match override_rows.take() {
                Some(rows) => rows,
                None => self.fetch_rows(table)?,
            };
            dep.apply(self, &cte_scope, &outer_table_name, &schema, &mut rows)?;
            override_rows = Some(rows);
        }
        let source_columns = tables.first().map(|(_, columns)| columns.clone());
        let (columns, mut rows) = self.run_plan(
            plan,
            override_rows,
            source_columns,
            candidate,
            unmatchable,
            fulltext_substring,
            scan_catalog,
            rough_filter_done,
        )?;
        if *distinct {
            let mut seen = std::collections::HashSet::new();
            rows.retain(|r| {
                seen.insert(
                    r.iter()
                        .map(|v| v.to_slt_string())
                        .collect::<Vec<_>>()
                        .join("\u{1f}"),
                )
            });
        }
        Ok((columns, rows))
    }

    /// Answers `Projection(Scan)` and `Projection(Filter(Scan))` from the
    /// materialised rows, without the operator chain.
    ///
    /// The chain is generic: every row is pushed into a `DataChunk` (cloning
    /// each `Value`), the projection expression is evaluated per row by the
    /// interpreter and the result is copied back out. For a select list that is
    /// nothing but column references - the overwhelming majority of scans - all
    /// of that is a copy of the row. Measured on 40,000 `INT + VARCHAR` rows,
    /// the chain is ~15 ms of a ~28 ms `SELECT id, name`.
    ///
    /// `None` leaves the plan to the operators (an expression in the select
    /// list, an aggregate, a sort, an index scan, ...).
    pub(crate) fn direct_projection(
        &self,
        plan: &LogicalPlan,
        rows: &mut Option<Vec<Vec<Value>>>,
        source_columns: Option<&Vec<String>>,
    ) -> Option<(Vec<String>, Vec<Vec<Value>>)> {
        let LogicalPlan::Projection {
            input,
            exprs,
            names,
        } = plan
        else {
            return None;
        };
        let mut indices = Vec::with_capacity(exprs.len());
        for expr in exprs {
            match expr {
                Expr::Column(i) => indices.push(*i),
                _ => return None,
            }
        }
        // The indices are positions in the scan's column list. When the scan
        // was lowered against a *pruned* list while the rows were materialised
        // in the unpruned one (an index scan, a `LazyLoad` projection), the two
        // do not line up and the operators' remapping is what makes the answer
        // right - leave such a plan to them.
        let scan_columns = match &**input {
            LogicalPlan::Scan { columns, .. } => Some(columns),
            LogicalPlan::Filter { input, .. } => match &**input {
                LogicalPlan::Scan { columns, .. } => Some(columns),
                _ => None,
            },
            _ => None,
        };
        let scan_columns = scan_columns?;
        if indices.iter().any(|i| *i >= scan_columns.len()) {
            return None;
        }
        // The rows are laid out as `source_columns`; the plan's indices are
        // positions in `scan_columns` (the lowerer prunes the ones the query
        // does not read). Map one onto the other, and - when they differ -
        // compact each row to the scan layout once, so the projection and the
        // predicate index the same list.
        let pos: Vec<usize> = match source_columns {
            Some(full) if full != scan_columns => {
                let mut pos = Vec::with_capacity(scan_columns.len());
                for name in scan_columns {
                    pos.push(full.iter().position(|c| c == name)?);
                }
                pos
            }
            _ => (0..scan_columns.len()).collect(),
        };
        let (table, columns, predicate) = match &**input {
            LogicalPlan::Scan { table, columns } => (table, columns, None),
            LogicalPlan::Filter { input, predicate } => match &**input {
                LogicalPlan::Scan { table, columns } => (table, columns, Some(predicate)),
                _ => return None,
            },
            _ => return None,
        };
        // Only now that the shape is known is the materialised row set taken:
        // it is the caller's, and a plan this function declines must keep it.
        let mut filtered = false;
        let mut rows = match rows.take() {
            Some(rows) => rows,
            None => {
                let prune = predicate
                    .map(|pred| prune_conditions(pred, columns))
                    .unwrap_or_default();
                // Decode only the columns the projection and the predicate
                // read: on a wide row (an embedding, a tensor) the rest is not
                // even looked at. The reader keeps the row width, so every
                // index the plan carries still lines up.
                // The plan's indices are positions in the scan's (pruned) list;
                // the reader wants positions in the table's.
                let mut needed: Vec<usize> = indices
                    .iter()
                    .filter_map(|i| pos.get(*i).copied())
                    .collect();
                if let Some(pred) = predicate {
                    let mut pred_cols = Vec::new();
                    predicate_columns(pred, &mut pred_cols);
                    needed.extend(pred_cols.iter().filter_map(|i| pos.get(*i).copied()));
                }
                needed.sort_unstable();
                needed.dedup();
                // A predicate runs inside the reader, on the row buffer, so a
                // row it rejects is never built: the plan's indices are in the
                // scan's layout, so the predicate is rewritten to the row's.
                // A predicate is only run inside the reader when it is expected
                // to reject most rows (`=` against a constant): filtering there
                // saves the rejected rows' buffers, while a predicate that
                // matches everything would only add a copy of each row.
                let narrowed = if self.wide_text_columns(table) {
                    None
                } else if let Some(pred) = predicate.filter(|pred| is_equality_predicate(pred)) {
                    let predicate = remap_columns(pred, &pos);
                    let read = self
                        .read_blocks_filtered_values(table, &needed, &prune, &|row: &[Value]| {
                            predicate.eval_predicate(row)
                        })
                        .ok();
                    filtered = read.is_some();
                    read
                } else {
                    self.read_blocks_pruned_values(table, &needed, &prune).ok()
                };
                match narrowed {
                    Some((_, rows)) => {
                        // A delete reaches the block's `version` at the next
                        // checkpoint; until then it lives in the table's delete
                        // log. The block reader returns the tombstoned rows
                        // (the count guards in `try_rows` expect them), so this
                        // path - which does not run the row set through
                        // `try_rows` - drops them here.
                        let dead = self
                            .tombstone_set_at(table, crate::read_ts())
                            .unwrap_or_default();
                        rows.into_iter()
                            .filter(|(seq, _)| !dead.contains(&(*seq as u64)))
                            .map(|(_, cells)| cells)
                            .collect()
                    }
                    None => self.fetch_rows_pruned(table, &prune).ok()?,
                }
            }
        };
        if !pos.iter().copied().eq(0..pos.len()) {
            rows = rows
                .into_iter()
                .map(|row| {
                    pos.iter()
                        .map(|p| row.get(*p).cloned().unwrap_or(Value::Null))
                        .collect()
                })
                .collect();
        }
        if let (Some(pred), false) = (predicate, filtered) {
            rows.retain(|row| pred.eval_predicate(row));
        }
        // The scan already produced the selected columns in order: hand the rows
        // over instead of copying every cell out of them again.
        let identity = indices.iter().copied().eq(0..indices.len());
        let out = if identity {
            for row in &mut rows {
                row.truncate(indices.len());
            }
            rows
        } else {
            rows.into_iter()
                .map(|row| {
                    indices
                        .iter()
                        .map(|i| row.get(*i).cloned().unwrap_or(Value::Null))
                        .collect()
                })
                .collect()
        };
        Some((names.clone(), out))
    }

    pub(crate) fn fetch_rows(&self, table: &str) -> Result<Vec<Vec<Value>>> {
        Ok(self
            .fetch_rows_with_ids(table)?
            .into_iter()
            .map(|(_, r)| r)
            .collect())
    }

    /// [`Self::fetch_rows`] with block pruning: the scan hands over the    /// predicate's range/equality comparisons, and a block whose
    /// `blk|…|fast_rough_filter` record proves none of them can hold is not
    /// decoded at all. Falls back to the plain scan (which is always correct)
    /// when the mirror is absent.
    pub(crate) fn fetch_rows_pruned(
        &self,
        table: &str,
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<Vec<Vec<Value>>> {
        if prune.is_empty() {
            return self.fetch_rows(table);
        }
        if let Some(rows) = self.try_rows_pruned(table, prune)? {
            return Ok(rows.into_iter().map(|(_, r)| r).collect());
        }
        self.fetch_rows(table)
    }

    /// Next storage sequence id for `table` (max existing + 1).
    ///
    /// The ids in use are the blocks' (a directory whose rows live there) plus
    /// the append buffer's, which holds the commits above the last checkpoint.
    /// Handing out an id twice would overwrite a row the mirror already holds.
    pub(crate) fn next_row_seq(&self, table: &str) -> Result<usize> {
        // The highest `(segment, rows)` pair the rows already occupy. A write
        // continues in the segment it was in - the one the block layout and the
        // append buffer both point at - not in a fresh one; only an empty table
        // opens the id the catalog says comes next
        // (`MetaTableObject::GetNextSegmentID` reads the segment map, which
        // keeps a retired entry until cleanup).
        let mut max = 0usize;
        for (id, rows) in self.block_segment_row_counts(table) {
            max = max.max(((id as usize) << 32) | rows as usize);
        }
        for (seq, _) in self.pending_rows_of(table) {
            max = max.max(seq + 1);
        }
        if max == 0 {
            // Nothing written yet: the very first append opens the segment the
            // catalog says comes next, never a retired one
            // (`MetaTableObject::GetNextSegmentID` reads the segment map, which
            // keeps a retired entry until cleanup).
            max = (self.next_segment_id(table) as usize) << 32;
        }
        Ok(max)
    }

    /// Fills `ResultCacheGetter`'s cache with one scan node's rows
    /// (`ResultCacheSetter`): the key is the node shape plus the timestamp the
    /// rows were read at, so a later query at the same timestamp can reuse
    /// them. The write paths clear the cache.
    pub(crate) fn cache_scan_rows(&self, table: &str, columns: &[String], rows: &[Vec<Value>]) {
        if rows.is_empty() || !self.result_cache_active() {
            return;
        }
        // A scan of this shape is what the getter keys on.
        let node = planner::LogicalPlan::Scan {
            table: table.to_string(),
            columns: columns.to_vec(),
        };
        let key = planner::result_cache::cache_key(&node, self.current_ts());
        self.plan_cache.put(
            key,
            planner::result_cache::CacheContent {
                columns: columns.to_vec(),
                rows: rows.to_vec(),
            },
        );
    }

    /// Non-deleted rows with their storage sequence ids (tombstones are skipped).
    /// Rows shorter than the schema (e.g. after `ALTER TABLE ADD COLUMN`) are
    /// padded with their column defaults.
    pub(crate) fn fetch_rows_with_ids(&self, table: &str) -> Result<Vec<(usize, Vec<Value>)>> {
        let key = self.table_key(table);
        let (cached, live) = {
            let tables = self.tables.lock().unwrap();
            let entry = tables.get(&key);
            (
                entry.and_then(|e| e.row_cache.as_ref().map(|c| (c.rows.clone(), c.bytes))),
                entry.and_then(|e| e.live_rows),
            )
        };
        // The cache holds the latest state; a reader pinned to a timestamp has
        // to see the rows as of then, so it goes to the table itself.
        if crate::read_ts() == u64::MAX {
            if let Some((rows, bytes)) = cached {
                // The cache is authoritative only while it covers every live row.
                if live == Some(rows.len()) {
                    return Ok(rows);
                }
                let mut tables = self.tables.lock().unwrap();
                if let Some(entry) = tables.get_mut(&key) {
                    if entry.row_cache.as_ref().map(|c| c.bytes) == Some(bytes) {
                        entry.row_cache = None;
                    }
                }
            }
        }
        let rows = self.row_set_from(table)?;
        if crate::read_ts() == u64::MAX {
            self.cache_row_set(table, key, &rows);
        }
        Ok(rows)
    }

    /// Every live row of `table`, as a scan reads them.
    ///
    /// One place decides where a row comes from - the blocks, plus the append
    /// buffer for the tail above the last checkpoint - so the scan, the count
    /// and the compaction answer from the same set. A table no block can hold
    /// has only the buffer: the port keeps no row journal.
    pub(crate) fn row_set_from(&self, table: &str) -> Result<Vec<(usize, Vec<Value>)>> {
        // The block mirror is the authoritative value source once it covers every
        // live row; the buffer then only supplies the rows appended since.
        if let Some(rows) = self.try_rows(table)? {
            return Ok(rows);
        }
        let deleted = self.tombstone_set_at(table, crate::read_ts())?;
        Ok(self
            .pending_rows_of(table)
            .into_iter()
            .filter(|(seq, _)| !deleted.contains(&(*seq as u64)))
            .collect())
    }

    /// Keeps a decoded row set for the next scan, when it is small and scalar.
    ///
    /// The cache is only ever consulted while it covers every live row, so it
    /// can be dropped at any point without changing an answer.
    pub(crate) fn cache_row_set(&self, table: &str, key: String, rows: &[(usize, Vec<Value>)]) {
        let types = self.column_types(table).unwrap_or_default();
        if !scalar_only(&types) {
            return;
        }
        let bytes: usize = 24
            + rows
                .iter()
                .map(|(_, r)| r.iter().map(value_bytes).sum::<usize>())
                .sum::<usize>();
        if bytes > ROW_CACHE_BUDGET_BYTES {
            return;
        }
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        entry.live_rows = Some(rows.len());
        entry.row_cache = Some(RowCache {
            rows: rows.to_vec(),
            bytes,
        });
    }

    pub(crate) fn fetch_rows_including_deleted(
        &self,
        table: &str,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        self.fetch_rows_including_deleted_from(table, 0)
    }

    /// [`Self::fetch_rows_including_deleted`] from row id `first` on.
    ///
    /// Blocks below `first` are skipped without decoding them and the buffer
    /// starts at `first`, so a reopened engine indexes only the rows its
    /// sealed chunks do not cover.
    pub(crate) fn fetch_rows_including_deleted_from(
        &self,
        table: &str,
        first: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let name = self.table_key(table);
        // The blocks hold every row a segment ever held, tombstoned slots
        // included - which is what the index mirrors count. The append buffer
        // adds the rows above the last checkpoint, exactly as it does for a
        // visible scan.
        let mut rows: Vec<(usize, Vec<Value>)> = self
            .read_blocks_including_deleted_from(&name, first)
            .into_iter()
            .map(|(seq, cells)| (seq, cells.iter().map(|c| parse_value(c)).collect()))
            .collect();
        let seen: std::collections::HashSet<usize> = rows.iter().map(|(seq, _)| *seq).collect();
        rows.extend(
            self.pending_rows_of(&name)
                .into_iter()
                .filter(|(seq, _)| *seq >= first && !seen.contains(seq)),
        );
        rows.sort_by_key(|(seq, _)| *seq);
        Ok(rows)
    }

    /// Full rows from the C++ block mirror.
    ///
    /// The mirror is a snapshot of the rows that existed when it was written,
    /// so it serves every row whose id it recorded. Rows appended afterwards
    /// (INSERT/COPY/UPDATE, or a column rewrite) are read from the row journal,
    /// and rows deleted afterwards are hidden by their tombstones; no
    /// checkpoint is needed for the mirror to stay authoritative.
    ///
    /// Returns `None` when the table has no block mirror or the mirror does not
    /// match its recorded row ids, so the caller falls back to the KV row store.
    pub(crate) fn try_rows(&self, table: &str) -> Result<Option<TableRows>> {
        self.try_rows_pruned(table, &[])
    }

    /// [`Self::try_rows`] with block pruning: a block whose prefilter rules
    /// out the predicate is never decoded.
    pub(crate) fn try_rows_pruned(
        &self,
        table: &str,
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<Option<TableRows>> {
        let table = if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        };
        let table = table.as_str();
        let key = self.table_key(table);
        let schema = self.table_schema(table)?;
        if schema.is_empty() {
            return Ok(None);
        }
        // A mirror that is untouched since it was written is the table: the
        // blocks hold the row set and the values, so no journal scan follows.
        // That is what makes the block layout the storage of record - a
        // directory whose `row:` keys are gone still answers every scan (plan
        // C), and the read costs one pass over the blocks instead of one over
        // the blocks plus one over the KV.
        //
        // `mirror_dirty` is also what a replay sets when it applies a frame
        // above the checkpoint, so a directory that was merely reopened (no WAL
        // tail) takes this path with no snapshot at all: the decoded blocks are
        // the id list.
        let snapshot = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.block_rows.clone());
        let wanted: Vec<usize> = (0..schema.len()).collect();
        // All rows still buffered: read them as the `Value`s they already are
        // (see `buffered_rows_values`). The visibility rules are the ones the
        // block reader applies to the same rows.
        if !self.wide_text_columns(table) {
            if let Some(rows) = self.buffered_rows_values(table, None) {
                return Ok(Some(rows));
            }
        }
        let Ok((_, rows)) = self.read_blocks_pruned_values(table, &wanted, prune) else {
            return Ok(None);
        };
        // The snapshot and the scan both list the rows the mirror makes
        // visible (a tombstoned slot is in neither), so a different length
        // means the mirror was rewritten between the snapshot and the read.
        if let Some(snapshot) = &snapshot {
            // `snapshot` counts the rows the *files* hold; the append buffer
            // adds the ones the commits since the checkpoint wrote, so only a
            // short read means the mirror was rewritten under us.
            if prune.is_empty() && rows.len() < snapshot.len() {
                return Ok(None);
            }
        }
        // The block rows are the answer for everything the files hold, pruned
        // or not. A pruned read simply has fewer of them (the blocks the
        // prefilter ruled out are not decoded).
        let hidden = self.tombstone_set_at(table, crate::read_ts())?;
        let mut out: Vec<(usize, Vec<Value>)> = rows
            .into_iter()
            .filter(|(seq, _): &(usize, Vec<Value>)| !hidden.contains(&(*seq as u64)))
            .collect();
        // A mirror that is untouched since it was written is the whole table.
        // `mirror_dirty` also covers what a replay sets when it applies a frame
        // above the checkpoint, so a directory that was merely reopened takes
        // this path with no snapshot at all: the decoded blocks are the id list.
        if prune.is_empty() && !self.mirror_dirty(table) {
            return Ok(Some(out));
        }
        // The tail above the last checkpoint is the append buffer's: rows it
        // holds that the blocks do not, minus the ones deleted since.
        let have: std::collections::HashSet<usize> = out.iter().map(|(seq, _)| *seq).collect();
        out.extend(
            self.pending_rows_of(table)
                .into_iter()
                .filter(|(seq, _)| !hidden.contains(&(*seq as u64)) && !have.contains(seq)),
        );
        out.sort_by_key(|(seq, _)| *seq);
        Ok(Some(out))
    }

    pub(crate) fn try_projection(
        &self,
        table: &str,
        items: &[SelectItem],
    ) -> Result<Option<Vec<Vec<Value>>>> {
        let table = if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        };
        let table = table.as_str();
        let schema = self.table_schema(table)?;
        // A decoded row cache answers a projection faster than decoding the
        // block mirror; leave the scan to it.
        let key = self.table_key(table);
        let cached = if crate::read_ts() == u64::MAX {
            self.tables
                .lock()
                .unwrap()
                .get(&key)
                .and_then(|e| e.row_cache.as_ref().map(|c| c.rows.len()))
        } else {
            // A pinned reader cannot take the cache: it holds the latest state.
            None
        };
        if let Some(len) = cached {
            if len == self.live_row_count(table)? {
                return Ok(None);
            }
        }
        let mut wanted: Vec<usize> = Vec::new();
        for item in items {
            match item {
                SelectItem::Column(name) => {
                    let base = name.split('.').next_back().unwrap_or(name);
                    let Some(i) = schema.iter().position(|c| c == base) else {
                        return Ok(None);
                    };
                    wanted.push(i);
                }
                _ => return Ok(None),
            }
        }
        if wanted.is_empty() {
            return Ok(None);
        }
        let key = self.table_key(table);
        let snapshot = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.block_rows.clone());
        // No snapshot means nothing has written the mirror in this process (a
        // directory that was merely opened): the block files are still the row
        // set, and the guard below is only there to catch a rewrite that ran
        // under this read.

        // A table whose rows are all still in the append buffer (no checkpoint
        // yet) is read straight out of it: the block reader would render every
        // cell to text and this function would parse it back to the `Value` the
        // buffer already holds.
        if !self.wide_text_columns(table) {
            if let Some(rows) = self.buffered_rows_values(table, Some(&wanted)) {
                let deleted = self.tombstone_set(table)?;
                return Ok(Some(
                    rows.into_iter()
                        .filter(|(seq, _)| !deleted.contains(&(*seq as u64)))
                        .map(|(_, cells)| cells)
                        .collect(),
                ));
            }
        }
        let Ok((_, rows)) = self.read_blocks_pruned_values(table, &wanted, &[]) else {
            return Ok(None);
        };
        // The files alone hold `snapshot.len()` rows; the append buffer adds
        // the ones the commits since the checkpoint wrote. Fewer than the
        // snapshot means the mirror was rewritten under us.
        if snapshot.as_ref().is_some_and(|s| rows.len() < s.len()) {
            return Ok(None);
        }
        // The projected read already carries the append buffer's rows over the
        // blocks (`read_blocks_selected`), so the row set is the one the
        // full scan returns; only the tombstones the files do not mark yet have
        // to be dropped here.
        let deleted = self.tombstone_set(table)?;
        let out: Vec<Vec<Value>> = rows
            .into_iter()
            .filter(|(seq, _)| !deleted.contains(&(*seq as u64)))
            .map(|(_, cells)| cells)
            .collect();
        Ok(Some(out))
    }

    /// The sidecar holding a table's wide (blob-referenced) cells.
    /// Whether a table has a column whose stored text form is not stable across
    /// the write and scan paths (a multi-vector, a tensor, a sparse vector), so
    /// the buffered `Value` fast path must not answer for it and the read goes
    /// through the block decoder instead.
    pub(crate) fn wide_text_columns(&self, table: &str) -> bool {
        match self.storage.catalog.get_table_by_name(table) {
            Ok(Some(meta)) => meta.types.iter().any(|t| {
                let t = t.trim().to_ascii_uppercase();
                t.starts_with("MULTIVECTOR") || t.starts_with("TENSOR") || t.starts_with("SPARSE")
            }),
            _ => false,
        }
    }

    /// Decodes one stored cell of column `col`.
    ///
    /// A cell of a text column (`VARCHAR`/`JSON`) is returned as the text it
    /// was stored as; only the other columns go through [`parse_value`], whose
    /// inference would otherwise re-read `iNF` as the float `inf`.
    pub(crate) fn decode_cell(
        &self,
        _table: &str,
        types: &[String],
        col: usize,
        cell: &str,
    ) -> Result<Value> {
        if types
            .get(col)
            .map(|t| crate::blocks::keeps_cell_text(t))
            .unwrap_or(false)
        {
            // The stored form spells a NULL cell `null` and has no other way to
            // say it, so that one spelling still decodes as NULL.
            if cell.eq_ignore_ascii_case("null") {
                return Ok(Value::Null);
            }
            return Ok(Value::Str(cell.to_string()));
        }
        Ok(parse_value(cell))
    }

    /// The stored text form of one row, which the block writer and the reader
    /// agree on.
    pub(crate) fn encode_row_stored(
        &self,
        _table: &str,
        row: &[Literal],
        types: &[String],
    ) -> Result<String> {
        Ok(encode_row(row, types))
    }

    /// The stored text of the given row ids, from the blocks (or from the
    /// append buffer for a table the block cursor cannot serve).
    ///
    /// The text form is what the full-text and vector paths score, so they ask
    /// for it directly instead of going through a `Vec<Value>`.
    pub(crate) fn row_texts(&self, table: &str, ids: &[u64]) -> Result<Vec<(usize, Vec<u8>)>> {
        let mut out = Vec::with_capacity(ids.len());
        let mut __fk = 0usize;
        if let Ok(mut cursor) = self.block_cursor(table) {
            for &id in ids {
                let seq = id as usize;
                if let Some(text) = cursor.row(seq)? {
                    out.push((seq, text));
                }
            }
            return Ok(out);
        }
        for &id in ids {
            let seq = id as usize;
            if let Some(text) = self.pending_row_text(table, seq)? {
                out.push((seq, text));
            }
        }
        Ok(out)
    }

    pub(crate) fn fetch_some_rows_with_ids(
        &self,
        table: &str,
        ids: &[u64],
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut out = Vec::with_capacity(ids.len());
        // A point read: the id names its segment, block and slot, so the row
        // comes out of one block (or the append buffer, when it was written
        // after the last checkpoint). The cursor decodes each block the ids
        // touch once, so a bulk statement does not pay a block decode per row.
        if let Ok(mut cursor) = self.block_cursor(table) {
            return cursor.rows(ids);
        }
        // A table whose columns have no block layout lives in the buffer only.
        let types = self.column_types(table).unwrap_or_default();
        for &id in ids {
            let seq = id as usize;
            if let Some(text) = self.pending_row_text(table, seq)? {
                out.push((seq, self.parse_stored_row(table, &defaults, &types, &text)?));
            }
        }
        Ok(out)
    }

    pub(crate) fn vector_cache_for(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<SharedVectorCache> {
        let key = index_key(table, column);
        let map = self.vector_cache.lock().unwrap();
        // Answer from the cache when the table's published row count is the one
        // the cache was built for. Counting rows from scratch
        // (`block_visible_row_count`) walks every block's version file and every
        // slot, which cost 15 ms per query on a 20k-row table - it must not run
        // on the query path. Every write path maintains `live_rows`
        // (`note_live_rows`/`set_live_rows`), so a mismatch is the signal that
        // the cache is stale.
        if let Some(cache) = map.get(&key) {
            let published = self
                .tables
                .lock()
                .unwrap()
                .get(&self.table_key(table))
                .and_then(|e| e.live_rows);
            let live = cache.read().map(|g| g.live.len()).unwrap_or(0);
            if published.is_some_and(|rows| rows == live) {
                return Ok(cache.clone());
            }
        }
        let schema = self.table_schema(table)?;
        let col = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let mut cache = VectorCache {
            dims,
            rows: Vec::new(),
            live: Default::default(),
        };
        for (seq, cells) in self.fetch_rows_with_ids_projected(table, col)? {
            if let Some(Value::Str(text)) = cells.get(col) {
                let v = parse_vec_str(text);
                if v.len() == dims {
                    cache.rows.push((seq, v));
                    cache.live.insert(seq);
                }
            }
        }
        let built = cache.live.len();
        drop(map);
        let shared = Arc::new(std::sync::RwLock::new(cache));
        self.vector_cache
            .lock()
            .unwrap()
            .insert(key, shared.clone());
        // Record what the cache now holds, so the next lookup can compare
        // against the published count without recounting the blocks.
        let table_key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        if tables.get(&table_key).and_then(|e| e.live_rows).is_none() {
            tables.entry(table_key).or_default().live_rows = Some(built);
        }
        Ok(shared)
    }

    /// `IndexBuilder`: builds the HNSW index of `column` from every live row.
    /// The rows go through the same append hook as an insert, so the graph is
    /// the reference's in-memory segment; the next dump seals it into a chunk.
    pub(crate) fn build_hnsw_index(&self, table: &str, column: &str) -> Result<()> {
        self.build_hnsw_index_of(table, column, None)
    }

    /// [`Self::build_hnsw_index`] for one catalog record (see
    /// [`Self::hnsw_mem_segments_of`]).
    pub(crate) fn build_hnsw_index_of(
        &self,
        table: &str,
        column: &str,
        only: Option<u64>,
    ) -> Result<()> {
        let schema = self.table_schema(table)?;
        let Some(ci) = schema.iter().position(|c| c == column) else {
            return Ok(());
        };
        let profile = std::env::var_os("INFINITY_PROFILE_IMPORT").is_some();
        let t_read = std::time::Instant::now();
        // Read the column, then hand it to the memory segment as one batch: the
        // reference's `IndexBuilder` calls `InsertVecs` once for the table, and
        // `InsertVecs` builds it in parallel buckets. Feeding the rows one at a
        // time made `CREATE INDEX` a serial point-by-point build.
        let mut rows: Vec<(u64, Vec<f32>)> = Vec::new();
        for (seq, text) in self.read_block_column_including_deleted(table, ci) {
            let v: Vec<f32> = parse_vec_str(&text).iter().map(|x| *x as f32).collect();
            rows.push((seq as u64, v));
        }
        let read = t_read.elapsed();
        // The graph of **this** record only: a column may carry several HNSW
        // indexes and each owns its rows once (the reference's per-`MemIndex`
        // object), so feeding every graph on the column would double-count the
        // rows in the others.
        let segments = match only {
            Some(id) => self.hnsw_mem_segments_of(table, column, Some(id))?,
            None => self.hnsw_mem_segments(table, column)?,
        };
        let t_build = std::time::Instant::now();
        for mem in segments {
            if let Ok(mut mem) = mem.lock() {
                let batch: Vec<(u64, Vec<f32>)> = rows
                    .iter()
                    .filter(|(_, v)| v.len() == mem.dims)
                    .map(|(seq, v)| (*seq, v.clone()))
                    .collect();
                if !batch.is_empty() {
                    mem.insert_batch(&batch, self.hnsw_build_workers());
                }
            }
        }
        if profile {
            eprintln!(
                "hnsw build profile: read {read:?} ({} rows) build {:?}",
                rows.len(),
                t_build.elapsed()
            );
        }
        Ok(())
    }

    /// The memory graphs of `table`'s HNSW indexes, topped up from the catalog.
    ///
    /// `only` narrows the result to one index record: a build or a recovery
    /// step feeds rows to **that** index's graph, and the reference's `MemIndex`
    /// is keyed by index id, so a column carrying two HNSW indexes must not
    /// receive the same rows twice through one shared graph.
    pub(crate) fn hnsw_mem_segments_of(
        &self,
        table: &str,
        column: &str,
        only: Option<u64>,
    ) -> Result<Vec<Arc<Mutex<crate::hnsw_mem::HnswIndexInMem>>>> {
        let key = self.table_key(table);
        let schema = self.table_schema(table)?;
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        // Every HNSW **record** on the table owns a graph, so the registry is
        // topped up from the catalog on every call rather than only while it is
        // empty. The reference's `MemIndex` map is keyed by
        // `idx_seg|{db}|{tbl}|{index_id}|{seg}|mem_index`, so a second
        // `CREATE INDEX … USING HNSW` on a column the first one already covers
        // gets a graph of its own - gating on `hnsw_mem.is_empty()` left the
        // new index without one, and its dump then wrote nothing while the
        // first index's graph was fed by both build paths.
        {
            let indexes = self.storage.catalog.indexes_of_table(table)?;
            let types = self.column_types(table)?;
            for im in indexes {
                if !im.index_type.eq_ignore_ascii_case("HNSW") {
                    continue;
                }
                if entry.hnsw_mem.contains_key(&im.id) {
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
                entry.hnsw_mem.insert(
                    im.id,
                    Arc::new(Mutex::new(crate::hnsw_mem::HnswIndexInMem::new_with(
                        dims, m, efc, metric, encoding,
                    ))),
                );
            }
            entry.hnsw_probed = true;
        }
        // Every HNSW index on `column` owns its own graph.
        let ids: Vec<u64> = match only {
            Some(id) => vec![id],
            None => self.index_ids_on_column(table, column),
        };
        Ok(ids
            .into_iter()
            .filter_map(|id| entry.hnsw_mem.get(&id).cloned())
            .collect())
    }

    /// [`Self::hnsw_mem_segments_of`] over every HNSW index on `column`.
    pub(crate) fn hnsw_mem_segments(
        &self,
        table: &str,
        column: &str,
    ) -> Result<Vec<Arc<Mutex<crate::hnsw_mem::HnswIndexInMem>>>> {
        self.hnsw_mem_segments_of(table, column, None)
    }

    /// Builds (once) the inverted index for a text column from its live rows.
    ///
    /// The build walks the rows one batch at a time, so it can straddle a
    /// compaction - which renumbers every row and drops the cached indexes at a
    /// new `TableEntry::text_epoch`. A build therefore only publishes at the
    /// epoch it started at, and a cached index is only reused when it was
    /// published at the current one (see [`Self::publish_text_index`]). Without
    /// that, a build that read the pre-compaction ids could land *after* the
    /// compaction rebuilt the index: every hit would then name a row the merge
    /// moved, `fetch_some_rows_with_ids` drops those, and `MATCH` answers
    /// nothing on a table full of matches (`TestParallelFulltextIndexReadWrite`,
    /// on the query that runs after its writers stopped).
    pub(crate) fn text_index_for(
        &self,
        table: &str,
        column: &str,
        tokenizer: TextTokenizer,
    ) -> Result<SharedTextIndex> {
        let index_id = self.fulltext_index_id(table, column).ok_or_else(|| {
            Error::Invalid(format!(
                "fulltext index does not exist for column '{column}'"
            ))
        })?;
        self.text_index_for_id(table, index_id, column, tokenizer)
    }

    /// The catalog id of the full-text index on `column` - the first one when
    /// the column has several, which is what the reference's
    /// `TableIndexMeta::GetIndexBase` resolves for a plain `MATCH TEXT`.
    fn fulltext_index_id(&self, table: &str, column: &str) -> Option<u64> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .ok()?
            .into_iter()
            .find(|im| im.column == column && im.index_type.eq_ignore_ascii_case("FULLTEXT"))
            .map(|im| im.id)
            .filter(|id| *id > 0)
    }

    /// [`Self::text_index_build`] for one specific index id.
    pub(crate) fn text_index_for_id(
        &self,
        table: &str,
        index_id: u64,
        column: &str,
        tokenizer: TextTokenizer,
    ) -> Result<SharedTextIndex> {
        self.text_index_build(table, index_id, column, tokenizer)
    }

    /// Builds the index for one column from the rows (see [`Self::text_index_for`]).
    ///
    /// The build reads once, so its postings describe exactly the snapshot the
    /// statement sees; a merge that commits while it runs therefore invalidates
    /// the *cache*, never the answer - see [`TextPublish`].
    fn text_index_build(
        &self,
        table: &str,
        index_id: u64,
        column: &str,
        tokenizer: TextTokenizer,
    ) -> Result<SharedTextIndex> {
        let key = mem_index_key(table, index_id);
        let tkey = self.table_key(table);
        // A bulk load that bypasses the DML hooks would leave the index short of
        // rows; compare against the published row count and rebuild. Prefer the
        // counter the write paths maintain: `row_count` walks the blocks'
        // `version` files, which costs milliseconds on a table of a few blocks
        // and must not run on the query path.
        let reserved = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&tkey).and_then(|e| e.live_rows) {
                Some(n) => n,
                None => {
                    drop(tables);
                    self.row_count(table).unwrap_or(0)
                }
            }
        };
        // Clone the `Arc` and let go of `tables` before the index is locked.
        // A query holds the index's *read* guard while it fetches rows
        // (`search_text_terms`), and that fetch needs `tables`
        // (`visible_block_ids` -> `block_index_for`); taking the index lock
        // under `tables` here closes a cycle as soon as a writer (the appender)
        // is pending: nobody can make progress, and every later statement
        // queues behind the frozen `tables`.
        let (cached, epoch) = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&tkey) {
                // A cached index built before the last bump belongs to the
                // layout that bump retired, and one built before the last
                // visible compaction read the source segments: both name row
                // ids nothing owns any more.
                Some(entry) => (
                    entry
                        .text
                        .get(&index_id)
                        .filter(|_| {
                            entry.text_built.get(&index_id).is_some_and(
                                |(built_epoch, built_ts)| {
                                    *built_epoch == entry.text_epoch
                                        && *built_ts >= entry.text_min_ts
                                },
                            )
                        })
                        .cloned(),
                    entry.text_epoch,
                ),
                None => (None, 0),
            }
        };
        if let Some(idx) = cached {
            let (complete, segments, tail, overridden) = {
                let g = idx.read().unwrap_or_else(|e| e.into_inner());
                (
                    g.total_docs(),
                    g.reader.segments.len(),
                    g.tails().map(|m| m.pending_docs()).sum::<usize>(),
                    g.overridden.len(),
                )
            };
            if complete >= reserved {
                if ft_trace_enabled(&key) {
                    eprintln!(
                        "[FT] {key} reuse docs={complete} reserved={reserved} segs={segments} tail={tail} overridden={overridden}"
                    );
                }
                return Ok(idx);
            }
            if ft_trace_enabled(&key) {
                eprintln!(
                    "[FT] {key} rebuild: cached docs={complete} < reserved={reserved} segs={segments} tail={tail} overridden={overridden}"
                );
            }
        }
        let schema = self.table_schema(table)?;
        let col = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        // The chunk list the catalog holds, in the reference's layout
        // (`idx_{id}/seg_{n}/ft_{base:016x}`). It is the **only** input: the
        // reference's `ColumnIndexReader` walks `SegmentIndexIDs -> ChunkIDs`
        // and never lists a directory, which is what makes a compaction's
        // deprecated segments unreachable the moment their ids are dropped.
        let catalog_chunks = self
            .fulltext_catalog_chunks_of(table, index_id)
            .unwrap_or_default();
        // A chunk has no other place to be, so the index root follows the same
        // index the catalog named; the writer/sealer and the reader agree on
        // `idx_{id}/seg_{n}`.
        let dir = match self.fulltext_index_root_of(table, index_id) {
            Some(root) => root.to_string_lossy().into_owned(),
            // No FULLTEXT index record (a column whose index was just dropped,
            // or a bare name the catalog cannot resolve): nothing to read, and
            // the caller's build will publish into record absence anyway.
            None => self
                .dir
                .join("fulltext")
                .join(key.replace('/', "_"))
                .to_string_lossy()
                .into_owned(),
        };
        // Cloned because a rejected publish retries the whole build.
        let build_dir = dir.clone();
        let mut index = TextIndex::new(dir, tokenizer.clone());
        // Map the chunks a previous process sealed instead of re-indexing the
        // rows they already cover: a reopened engine pays for the unsealed tail
        // only, not for the whole table.
        let first = index.load_sealed_scoped(&catalog_chunks) as usize;
        if ft_trace_enabled(&key) {
            eprintln!(
                "[FT] {key} build dir={build_dir} segs_loaded={} first={first} catalog_chunks={}",
                index.reader.segments.len(),
                catalog_chunks.len()
            );
        }
        for (seq, cells) in self.fetch_rows_with_ids_projected_from(table, col, first)? {
            let text = match cells.get(col) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => String::new(),
            };
            index_row(&mut index, seq as u64, &text);
            index.maybe_seal(self.memindex_capacity());
        }
        let shared = Arc::new(std::sync::RwLock::new(index));
        #[cfg(test)]
        run_text_build_hook();
        match self.publish_text_index(&tkey, index_id, epoch, shared.clone()) {
            TextPublish::Cached => Ok(shared),
            // A compaction committed after the build's read. The build still
            // describes exactly the rows this statement sees (one read, one
            // snapshot), so its postings resolve for **this** statement; it just
            // may not be cached, because a statement above the merge would ask it
            // for ids the merge retired.
            //
            // Retrying here was the wrong answer twice over: the rebuild is fast
            // only when a cache is there to reuse, and while imports keep landing
            // the cache stays short - so a table under write pressure rebuilt the
            // whole column for every query, and a merge landing inside each
            // rebuild could keep one `MATCH` running for the whole gate timeout
            // (`TestParallelFulltextIndexReadWrite`, 15 s window, 427 s case).
            TextPublish::ReadTooOld | TextPublish::Renumbered => Ok(shared),
        }
    }

    /// Publishes a freshly built full-text index for `column`, unless the table's
    /// cached indexes were dropped (or a merge committed after the build's read)
    /// while it ran. See [`TextPublish`] for what the caller may do with it.
    pub(crate) fn publish_text_index(
        &self,
        tkey: &str,
        index_id: u64,
        epoch: u64,
        index: SharedTextIndex,
    ) -> TextPublish {
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey.to_string()).or_default();
        // The layout changed under the build: `TableEntry::text_epoch` is bumped
        // by the compaction that renumbers the rows, so the build read ids that
        // now name nothing.
        if entry.text_epoch != epoch {
            return TextPublish::Renumbered;
        }
        let read_ts = crate::read_ts();
        // A compaction committed after the build's read: the build still
        // describes the segments this statement sees, but a later statement, at
        // a ts above the merge, would resolve the ids it left behind.
        if read_ts < entry.text_min_ts {
            return TextPublish::ReadTooOld;
        }
        entry.text.insert(index_id, index);
        entry.text_built.insert(index_id, (epoch, read_ts));
        TextPublish::Cached
    }

    /// Moves a table's full-text caches onto the row ids a compaction just
    /// assigned.
    ///
    /// A merge renumbers every row it takes, so the postings of the kept caches
    /// stop naming live rows the moment it lands. The rows it moved are indexed
    /// here under their new ids - from the row bytes the merge already read, not
    /// from a scan - and the ids they left are marked overridden
    /// (`index_delete_row`). Overridden ids are what the sealed read paths skip
    /// while they iterate (`MergeCursor`), i.e. **before** the top-n cut: leaving
    /// them in the postings made `MATCH` fill its top-n with rows no fetch can
    /// resolve and answer "column is empty" on a table full of matches.
    ///
    /// The caches are not dropped for this. Doing that put a full rebuild of the
    /// column on the query path, and a table that keeps taking writes is
    /// compacted every couple of minutes - the rebuild outlived the next
    /// compaction, its publish was refused and it retried, which is what blocked
    /// one `MATCH` for the whole gate timeout
    /// (`TestParallelFulltextIndexReadWrite`).
    ///
    /// `built_ts` is the merge's commit ts: the caller raises
    /// `TableEntry::text_min_ts` to it, so the cache has to claim it.
    pub(crate) fn reindex_compacted_text(
        &self,
        table: &str,
        rows: &[(usize, Vec<u8>)],
        base: usize,
        built_ts: u64,
    ) {
        let tkey = self.table_key(table);
        // Clone the handles and let go of `tables` before an index is locked: a
        // query holds the index's read guard while it fetches rows, and that
        // fetch needs `tables`. Waiting for the write lock under it is the
        // reverse order and deadlocks the query.
        let (indexes, epoch) = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&tkey) {
                Some(entry) if !entry.text.is_empty() => {
                    let by_id: std::collections::HashMap<u64, String> = self
                        .storage
                        .catalog
                        .indexes_of_table(&self.qualified_table(table))
                        .unwrap_or_default()
                        .into_iter()
                        .map(|im| (im.id, im.column))
                        .collect();
                    (
                        entry
                            .text
                            .iter()
                            .filter_map(|(id, index)| {
                                Some((*id, by_id.get(id)?.clone(), index.clone()))
                            })
                            .collect::<Vec<_>>(),
                        entry.text_epoch,
                    )
                }
                // Nothing cached: the next query builds the merged layout.
                _ => return,
            }
        };
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let types = self.column_types(table).unwrap_or_default();
        let segment = (base as u64) >> 32;
        for (_index_id, column, shared) in &indexes {
            let Some(col) = schema.iter().position(|c| c == column) else {
                continue;
            };
            // Tokenise with no lock held: the analyzer is the expensive half of a
            // write (the DML path is split the same way).
            let prepared: Vec<PreparedDoc> = {
                let tokenizer = shared.read().unwrap().tokenizer.clone();
                // Every merged row is re-indexed, an unreadable one as an empty
                // document. Skipping it would leave the cache one document short
                // of `live_rows`, `text_index_build` would see a cache that never
                // catches up and rebuild the whole column on every query.
                rows.iter()
                    .enumerate()
                    .map(|(i, (_, bytes))| {
                        let text = std::str::from_utf8(bytes)
                            .ok()
                            .and_then(|row| row.split('\t').nth(col))
                            .map(|cell| match self.decode_cell(table, &types, col, cell) {
                                Ok(Value::Str(text)) => text,
                                Ok(other) => other.to_slt_string(),
                                Err(_) => String::new(),
                            })
                            .unwrap_or_default();
                        crate::engine::text_index::prepare_doc((base + i) as u64, &text, &tokenizer)
                    })
                    .collect()
            };
            // A tail spans one `ROW_ID` segment; the merged rows are a new one, so
            // write out whatever else is still unsealed first (no lock held).
            if rows.is_empty()
                || shared
                    .read()
                    .unwrap()
                    .unsealed_segment()
                    .is_some_and(|held| held != segment)
            {
                crate::engine::state::seal_text_index_unlocked(shared);
            }
            {
                let mut index = shared.write().unwrap();
                crate::engine::state::index_rows_prepared(&mut index, &prepared);
                // The reference's reader is built from the catalog's live
                // `SegmentIndexIDs`, so the segments this merge took are simply
                // not there. Drop them whole: the rows they held were renumbered
                // into `prepared` above, so nothing of them is owed.
                let live: std::collections::HashSet<u32> = self
                    .tables
                    .lock()
                    .unwrap()
                    .get(&tkey)
                    .map(|entry| entry.segments.keys().copied().collect())
                    .unwrap_or_default();
                index.drop_segments_not_in(&live);
            }
            // Persist the moved rows: the chunk names the new ids, so the next
            // build maps it instead of re-reading the column, and the chunks of
            // the retired segments are gone from the catalog.
            crate::engine::state::seal_text_index_unlocked(shared);
            self.record_sealed_text_chunks(table, column);
            if ft_trace_enabled(&self.table_key(table)) {
                let g = shared.read().unwrap_or_else(|e| e.into_inner());
                // The failure this trace exists for: a row is in the tail (so it
                // counts towards `total_docs`, which is what lets the cache be
                // reused) **and** marked overridden (so `merge` filters it out
                // of every posting). That combination answers 0 rows while the
                // counter says the index is complete.
                let shadowed = g
                    .tails()
                    .flat_map(|m| m.docs.keys().copied())
                    .filter(|row| g.overridden.contains(row))
                    .count();
                eprintln!(
                    "[FT] {tkey}.{column} reindexed {} rows at base={base} segs={} tail={} overridden={} shadowed_tail_rows={shadowed}",
                    rows.len(),
                    g.reader.segments.len(),
                    g.tails().map(|m| m.pending_docs()).sum::<usize>(),
                    g.overridden.len()
                );
            }
        }
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        if entry.text_epoch != epoch {
            // Another compaction renumbered the table while this ran; its own
            // pass owns the stamp.
            return;
        }
        for (index_id, _, _) in &indexes {
            entry.text_built.insert(*index_id, (epoch, built_ts));
        }
    }

    /// The sealed chunks the catalog lists for a table's full-text index on
    /// `column`, in the reference's layout.
    ///
    /// C++: `ColumnIndexReader::Open` (`column_index_reader_impl.cpp:71-105`)
    /// walks `TableIndexMeta` → segment ids → chunk ids → `ChunkIndexMetaInfo`,
    /// and the segment's directory comes from `SegmentIndexMeta::
    /// GetSegmentIndexDir()` (`db_{db}/tbl_{tbl}/idx_{idx}/seg_{seg}`). This is
    /// that walk: it is how a directory the reference wrote is read at all, and
    /// it is also the authoritative list for a directory the port wrote.
    /// `idx_{index_id}` for a full-text index - the root the seals write under
    /// and the catalog's `idx_chunk` records name.
    fn fulltext_index_root_of(&self, table: &str, index_id: u64) -> Option<std::path::PathBuf> {
        let qualified = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&qualified).ok()??;
        Some(self.blocks_root().join(format!(
            "db_{}/tbl_{}/idx_{index_id}",
            self.database_id_of(&qualified),
            meta.id
        )))
    }

    /// The chunks the catalog lists for one **specific** full-text index id,
    /// in the reference's layout.
    pub(crate) fn fulltext_catalog_chunks_of(
        &self,
        table: &str,
        index_id: u64,
    ) -> Result<Vec<CatalogChunk>> {
        // The catalog accepts both spellings depending on which record wrote it
        // (`im.table` is the bare name for an index created without a database,
        // the qualified one for the mirror), so both are tried.
        let qualified = self.qualified_table(table);
        let mut meta = self.storage.catalog.get_table_by_name(&qualified)?;
        if meta.is_none() && qualified != table {
            meta = self.storage.catalog.get_table_by_name(table)?;
        }
        let Some(meta) = meta else {
            return Ok(Vec::new());
        };
        let db_id = self.database_id_of(&qualified);
        let layers = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
        )?;
        let mut chunks = Vec::new();
        for (segment, entries) in layers {
            let dir = self
                .blocks_root()
                .join(format!(
                    "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                    meta.id
                ))
                .to_string_lossy()
                .into_owned();
            for (_, info) in entries {
                if info.base_name.is_empty() {
                    continue;
                }
                chunks.push(CatalogChunk {
                    dir: dir.clone(),
                    base_name: info.base_name,
                    base_row_id: info.base_row_id,
                });
            }
        }
        // Chunk-id order is dump order; the loader wants row order.
        chunks.sort_by_key(|chunk| chunk.base_row_id);
        Ok(chunks)
    }

    /// Builds (once) the multi-vector index of a column from its rows: every
    /// document goes to the memory tail and is dumped immediately, so later
    /// queries only map the segment files. Subsequent inserts append to the
    /// tail and `flush()` seals it (the reference's dump).
    /// Points a multi-vector index at its catalog chunk directory and takes its
    /// chunk counter from the catalog (`GetAndSetNextChunkID`).
    fn attach_mv_chunk_root(&self, table: &str, index_id: u64, index: &mut MultiVectorIndex) {
        let qualified = self.qualified_table(table);
        let Some(meta) = self
            .storage
            .catalog
            .get_table_by_name(&qualified)
            .ok()
            .flatten()
        else {
            return;
        };
        if index_id == 0 {
            return;
        }
        let db_id = self.database_id_of(&qualified);
        index.chunk_root = Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}", meta.id)),
        );
        // `GetAndSetNextChunkID` over every segment of this index.
        let mut next = 0u32;
        let Ok(layers) = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
        ) else {
            return;
        };
        for (segment, chunks) in layers {
            let from_tag = self
                .storage
                .catalog
                .next_chunk_id(&db_id.to_string(), &meta.id.to_string(), index_id, segment)
                .unwrap_or(0);
            let from_records = chunks
                .iter()
                .map(|(id, _)| *id as u32 + 1)
                .max()
                .unwrap_or(0);
            next = next.max(from_tag).max(from_records);
        }
        index.next_chunk = u64::from(next);
    }

    /// The multi-vector index a `MATCH TENSOR` on `column` resolves to: the
    /// column's first `PLAID`/`SMVE`/`EMVB` record, or an error when it has
    /// none (`PhysicalMatchTensorScan::PlanWithIndex` leaves such a column to
    /// the block scan).
    pub(crate) fn multivector_index_for(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<Arc<std::sync::RwLock<MultiVectorIndex>>> {
        let index_id = self
            .multivector_index_id(table, column)
            .ok_or_else(|| Error::NotFound(format!("no multi-vector index on '{column}'")))?;
        self.multivector_index_by_id(table, column, index_id, dims)
    }

    /// [`Self::multivector_index_for`] for one specific catalog record.
    ///
    /// The registry is keyed by the catalog index id: a column may carry
    /// several multi-vector indexes, each with its own tail, chunk list and
    /// options. The index's identity is its record - the reference's `MemIndex`
    /// is keyed by `idx_seg|{db}|{tbl}|{index_id}|{seg}|mem_index` - so a
    /// second `CREATE INDEX` on the same column must not rebuild the first.
    pub(crate) fn multivector_index_by_id(
        &self,
        table: &str,
        column: &str,
        index_id: u64,
        dims: usize,
    ) -> Result<Arc<std::sync::RwLock<MultiVectorIndex>>> {
        // The catalog and the index directory are keyed by the qualified name, so
        // a bare name (a test, or an SDK call without a database) would register
        // the index under a different key and rebuild it on every call.
        let qualified = self.qualified_table(table);
        let table = qualified.as_str();
        let key = mem_index_key(table, index_id);
        let tkey = self.table_key(table);
        // Prefer the counter the write paths maintain (live_rows): row_count
        // walks every block version file and every slot, which cost 0.67 ms of
        // a 1.6 ms multivector query on a 20k-row table.
        let reserved = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&tkey).and_then(|e| e.live_rows) {
                Some(n) => n,
                None => {
                    drop(tables);
                    self.row_count(table).unwrap_or(0)
                }
            }
        };
        // See `text_index_for`: `tables` is released before the index lock.
        let cached = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).and_then(|e| e.mv.get(&index_id)).cloned()
        };
        if let Some(idx) = cached {
            // `doc_count` counts parsed documents only: a column with a NULL or
            // unparseable tensor row would then never look complete and every
            // search would rebuild the index (PLAID training included). The
            // index tracks the rows it has *seen* instead.
            let complete = idx
                .read()
                .map(|index| index.is_complete(reserved))
                .unwrap_or(false);
            if complete {
                return Ok(idx);
            }
        }
        let schema = self.table_schema(table)?;
        let ci = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let dir = self.dir.join("multivector").join(key.replace('/', "_"));
        // The options, the kind and the chunk list all belong to this index's
        // own record: two indexes on one column are two `PlaidIndex`es, and a
        // `SMVE` and a `PLAID` beside each other keep separate configs.
        let record = self
            .storage
            .catalog
            .indexes_of_table(table)?
            .into_iter()
            .find(|im| im.id == index_id)
            .ok_or_else(|| Error::NotFound(format!("index {index_id} on '{column}'")))?;
        let index_name = record.name.clone();
        // A previous dump wrote the index out as C++ PLAID chunks; loading them
        // costs O(chunks), while the rebuild below retrains the centroids and
        // re-adds every embedding. The chunk's document order is the row order,
        // so a light column scan recovers the row ids.
        if record.index_type.eq_ignore_ascii_case("PLAID") {
            if let Ok(loaded) = self.read_plaid_index(table, &index_name) {
                let seqs = self.multivector_row_ids(table, ci, dims)?;
                if !seqs.is_empty() && loaded.doc_num() as usize == seqs.len() {
                    let mut index = MultiVectorIndex::new(
                        dir.clone(),
                        dims,
                        self.mv_plaid_config(table, &index_name),
                        self.mv_smve_config(table, &index_name),
                        self.mv_emvb_config(table, &index_name),
                    );
                    self.attach_mv_chunk_root(table, index_id, &mut index);
                    index.scanned_rows = reserved;
                    // The sealed scan is the fallback when a filter excludes
                    // every PLAID candidate; without the readers a filtered
                    // search would answer nothing at all.
                    index.load_sealed(&seqs);
                    index.plaid_seqs = seqs;
                    index.plaid = Some(loaded);
                    let shared = Arc::new(std::sync::RwLock::new(index));
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .mv
                        .insert(index_id, shared.clone());
                    return Ok(shared);
                }
            }
        }
        // A rebuild rewrites the chunk set; drop stale files so the readers
        // never mix two builds.
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                let p = entry.path();
                if p.extension().is_some_and(|x| x == "mvec" || x == "emvb") {
                    let _ = storage::chunk_store::remove_file(p);
                }
            }
        }
        let mut index = MultiVectorIndex::new(
            dir,
            dims,
            self.mv_plaid_config(table, &index_name),
            self.mv_smve_config(table, &index_name),
            self.mv_emvb_config(table, &index_name),
        );
        self.attach_mv_chunk_root(table, index_id, &mut index);
        let mut scanned = 0usize;
        for (seq, cells) in self.fetch_rows_with_ids_projected_all(table, ci)? {
            scanned += 1;
            let text = match cells.get(ci) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => continue,
            };
            if let Some(doc) = parse_multivector_doc(&text, dims) {
                index.insert(seq as u64, doc);
            }
        }
        // A row whose tensor does not parse still counts as seen: the reference
        // indexes the rows it is given, and the search falls back to the sealed
        // chunks for the rest.
        index.scanned_rows = scanned;
        // The reference trains `PlaidGlobalCentroids` and builds
        // `PlaidIndexInMem` while the embeddings are buffered, before the dump.
        index.build_plaid();
        index.build_smve();
        index.seal();
        let shared = Arc::new(std::sync::RwLock::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .mv
            .insert(index_id, shared.clone());
        Ok(shared)
    }

    /// The rows a `SEARCH MATCH …` filter admits. The reference computes a
    /// per-block bitmask (`CalculateFilterBitmask`) and passes it to the index;
    /// this evaluates the same predicates once over the rows.
    fn match_filter_rows(
        &self,
        table: &str,
        filters: &[&Expr],
    ) -> Result<std::collections::HashSet<u64>> {
        let mut out = std::collections::HashSet::new();
        if filters.is_empty() {
            return Ok(out);
        }
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            if filters.iter().all(|filter| filter.eval_predicate(&row)) {
                out.insert(seq as u64);
            }
        }
        Ok(out)
    }

    /// The row ids whose tensor parses, ascending: the document order a PLAID
    /// chunk was written in (`plaid_chunk` -> `column_docs`).
    ///
    /// `fetch_rows_with_ids` is the *live* population, which is what the chunk
    /// writer reads: a tombstoned row is not in a chunk, so listing it here
    /// would make the count disagree with `doc_num()` and every reopen would
    /// retrain the index instead of mapping it.
    fn multivector_row_ids(&self, table: &str, ci: usize, dims: usize) -> Result<Vec<u64>> {
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            let text = match row.get(ci) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => continue,
            };
            if parse_multivector_doc(&text, dims).is_some() {
                out.push(seq as u64);
            }
        }
        Ok(out)
    }

    /// `CREATE INDEX … USING PLAID WITH (nbits=…, n_centroids=…)`: the options
    /// of **that** index record.
    ///
    /// Keyed by the index, not the column: the reference's `MemIndex` holds one
    /// object per index, so `index3` on `col3` and a later `index7` on the same
    /// column are two different `PlaidIndex`es with their own options. The
    /// configs of the other kinds stay `None` - an index is exactly one kind.
    pub(crate) fn mv_plaid_config(&self, table: &str, index: &str) -> Option<PlaidConfig> {
        let im = self
            .storage
            .catalog
            .index_by_name(&self.qualified_table(table), index)
            .ok()??;
        if !im.index_type.eq_ignore_ascii_case("PLAID") {
            return None;
        }
        let mut cfg = PlaidConfig {
            nbits: 4,
            n_centroids: 0,
            colbertsar: false,
        };
        for (key, value) in &im.options {
            if key.eq_ignore_ascii_case("nbits") {
                if let Ok(v) = value.trim().parse() {
                    cfg.nbits = v;
                }
            } else if key.eq_ignore_ascii_case("n_centroids") {
                if let Ok(v) = value.trim().parse() {
                    cfg.n_centroids = v;
                }
            } else if key.eq_ignore_ascii_case("colbertsar_mode") {
                let v = value.trim();
                cfg.colbertsar = v.eq_ignore_ascii_case("true") || v == "1";
            }
        }
        Some(cfg)
    }

    /// Whether `table.column` carries an ANN index that the tensor scan can
    /// answer from (`PLAID`/`SMVE`; `EMVB` uses the same multi-vector path).
    pub(crate) fn tensor_has_ann_index(&self, table: &str, column: &str) -> bool {
        self.storage
            .catalog
            .indexes_of_table(table)
            .unwrap_or_default()
            .into_iter()
            .any(|im| {
                im.column == column
                    && (im.index_type.eq_ignore_ascii_case("PLAID")
                        || im.index_type.eq_ignore_ascii_case("SMVE")
                        || im.index_type.eq_ignore_ascii_case("EMVB"))
            })
    }

    /// `CREATE INDEX … USING EMVB WITH (pq_subspace_num=…, pq_subspace_bits=…)`
    /// on `table.column`.
    pub(crate) fn mv_emvb_config(&self, table: &str, index: &str) -> Option<EmvbConfig> {
        let im = self
            .storage
            .catalog
            .index_by_name(&self.qualified_table(table), index)
            .ok()??;
        if !im.index_type.eq_ignore_ascii_case("EMVB") {
            return None;
        }
        let mut cfg = EmvbConfig {
            subspace_num: 1,
            subspace_bits: 8,
        };
        for (key, value) in &im.options {
            if key.eq_ignore_ascii_case("pq_subspace_num") {
                if let Ok(v) = value.trim().parse() {
                    cfg.subspace_num = v;
                }
            } else if key.eq_ignore_ascii_case("pq_subspace_bits") {
                if let Ok(v) = value.trim().parse() {
                    cfg.subspace_bits = v;
                }
            }
        }
        Some(cfg)
    }

    /// `CREATE INDEX … USING SMVE WITH (width=…, topk=…)` on `table.column`.
    pub(crate) fn mv_smve_config(&self, table: &str, index: &str) -> Option<SmveConfig> {
        let im = self
            .storage
            .catalog
            .index_by_name(&self.qualified_table(table), index)
            .ok()??;
        if !im.index_type.eq_ignore_ascii_case("SMVE") {
            return None;
        }
        let mut cfg = SmveConfig {
            width: 2048,
            topk: 8,
        };
        for (key, value) in &im.options {
            if key.eq_ignore_ascii_case("width") {
                if let Ok(v) = value.trim().parse() {
                    cfg.width = v;
                }
            } else if key.eq_ignore_ascii_case("topk") {
                if let Ok(v) = value.trim().parse() {
                    cfg.topk = v;
                }
            }
        }
        Some(cfg)
    }

    /// Exact MaxSim of one row against `query`, read from the row
    /// (used to re-rank SMVE candidates the way the reference reads the column).
    pub(crate) fn exact_tensor_score(
        &self,
        table: &str,
        column: &str,
        dims: usize,
        query: &[Vec<f32>],
        seq: u64,
    ) -> Option<f32> {
        let schema = self.table_schema(table).ok()?;
        let ci = schema.iter().position(|c| c == column)?;
        let cells = self
            .fetch_some_rows_with_ids(table, &[seq])
            .ok()?
            .into_iter()
            .next()?
            .1;
        let text = match cells.get(ci)? {
            Value::Str(s) => s.clone(),
            other => other.to_slt_string(),
        };
        let doc = parse_multivector_doc(&text, dims)?;
        Some(maxsim_tokens(query, &doc))
    }

    /// Test-only: the id of the **first secondary** index on `column`.
    ///
    /// The registries are keyed by index id; a test names the column, so this
    /// resolves the one secondary record the test created.
    #[cfg(test)]
    pub(crate) fn test_secondary_id(&self, table: &str, column: &str) -> u64 {
        self.secondary_index_ids_on_column(table, column)
            .into_iter()
            .next()
            .unwrap_or(0)
    }

    /// Test-only: the id of the index on `column` whose type is `kind`.
    ///
    /// The registries are keyed by index id; a test names the column, so this
    /// resolves the one index of that kind the test created. A column can carry
    /// several indexes (`col3` holds both an IVF and an HNSW one), so the kind
    /// disambiguates.
    #[cfg(test)]
    pub(crate) fn test_index_id(&self, table: &str, column: &str, kind: &str) -> u64 {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .find(|im| im.column == column && im.index_type.eq_ignore_ascii_case(kind))
            .map(|im| im.id)
            .unwrap_or(0)
    }

    /// Every multi-vector index id on `column`.
    fn multivector_index_ids(&self, table: &str, column: &str) -> Vec<u64> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .filter(|im| {
                im.column == column
                    && (im.index_type.eq_ignore_ascii_case("PLAID")
                        || im.index_type.eq_ignore_ascii_case("SMVE")
                        || im.index_type.eq_ignore_ascii_case("EMVB")
                        || im.index_type.eq_ignore_ascii_case("MULTIVECTOR"))
            })
            .map(|im| im.id)
            .filter(|id| *id > 0)
            .collect()
    }

    /// The first multi-vector index id on `column` (what a plain
    /// `MATCH TENSOR` resolves to).
    fn multivector_index_id(&self, table: &str, column: &str) -> Option<u64> {
        self.multivector_index_ids(table, column).into_iter().next()
    }

    /// The catalog id of the BMP index on `column` (a column carries one).
    fn sparse_index_id(&self, table: &str, column: &str) -> Option<u64> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .ok()?
            .into_iter()
            .find(|im| im.column == column && im.index_type.eq_ignore_ascii_case("BMP"))
            .map(|im| im.id)
            .filter(|id| *id > 0)
    }

    /// Builds (once) the sparse BMP index of a column, dumping every segment so
    /// later queries only map the chunk files. Inserts append to the memory
    /// tail and `flush()` seals it.
    /// Points a sparse index at its catalog chunk directory and takes its chunk
    /// counter from the catalog (`GetAndSetNextChunkID`).
    fn attach_sparse_chunk_root(&self, table: &str, index_id: u64, index: &mut SparseIndex) {
        let qualified = self.qualified_table(table);
        let Some(meta) = self
            .storage
            .catalog
            .get_table_by_name(&qualified)
            .ok()
            .flatten()
        else {
            return;
        };
        if index_id == 0 {
            return;
        }
        let db_id = self.database_id_of(&qualified);
        index.chunk_root = Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}", meta.id)),
        );
        let mut next = 0u32;
        let Ok(layers) = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
        ) else {
            return;
        };
        for (segment, chunks) in layers {
            let from_tag = self
                .storage
                .catalog
                .next_chunk_id(&db_id.to_string(), &meta.id.to_string(), index_id, segment)
                .unwrap_or(0);
            let from_records = chunks
                .iter()
                .map(|(id, _)| *id as u32 + 1)
                .max()
                .unwrap_or(0);
            next = next.max(from_tag).max(from_records);
        }
        index.next_chunk = u64::from(next);
    }

    /// The BMP index a `MATCH SPARSE` on `column` resolves to: the column's
    /// `USING BMP` record, or an error when it has none
    /// (`PhysicalMatchSparseScan::PlanWithIndex` leaves such a column to the
    /// block scan).
    pub(crate) fn sparse_index_for(
        &self,
        table: &str,
        column: &str,
        width: storage::bmp::IdxWidth,
    ) -> Result<Arc<std::sync::RwLock<SparseIndex>>> {
        let index_id = self
            .sparse_index_id(table, column)
            .ok_or_else(|| Error::NotFound(format!("no BMP index on '{column}'")))?;
        self.sparse_index_by_id(table, column, index_id, width)
    }

    /// [`Self::sparse_index_for`] for one specific catalog record, so two BMP
    /// indexes on a column keep their own tails (the reference's per-`MemIndex`
    /// object).
    pub(crate) fn sparse_index_by_id(
        &self,
        table: &str,
        column: &str,
        index_id: u64,
        width: storage::bmp::IdxWidth,
    ) -> Result<Arc<std::sync::RwLock<SparseIndex>>> {
        let key = mem_index_key(table, index_id);
        let tkey = self.table_key(table);
        // The counter the write paths maintain, like `multivector_index_for`:
        // `row_count` walks every block version file and slot, which is a
        // visible slice of a small query's latency.
        let reserved = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&tkey).and_then(|e| e.live_rows) {
                Some(n) => n,
                None => {
                    drop(tables);
                    self.row_count(table).unwrap_or(0)
                }
            }
        };
        let schema = self.table_schema(table)?;
        let ci = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let store = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .and_then(|m| m.sparse_stores.get(ci).copied().flatten())
            .unwrap_or(CatalogSparseStore::Sort);
        // See `text_index_for`: `tables` is released before the index lock.
        let cached = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .and_then(|e| e.sparse.get(&index_id))
                .cloned()
        };
        if let Some(idx) = cached {
            // A changed catalog hint invalidates the cached index even though
            // the BMP bytes are identical. Coverage is measured by the rows the
            // index has *seen*, not by `doc_count`: a NULL sparse cell writes
            // no postings, so a table holding one never looked complete and was
            // rebuilt on every query.
            let reusable = idx
                .read()
                .map(|g| g.is_complete(reserved) && g.store == store)
                .unwrap_or(false);
            if reusable {
                return Ok(idx);
            }
        }
        let dir = self.dir.join("sparse").join(key.replace('/', "_"));
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                let p = entry.path();
                if p.extension().is_some_and(|x| x == "bmp") {
                    let _ = storage::chunk_store::remove_file(p);
                }
            }
        }
        let block_size = self
            .storage
            .catalog
            .indexes_of_table(table)?
            .into_iter()
            .find(|im| im.id == index_id)
            .map(|im| index_block_size(&im))
            .unwrap_or(index::sparse::BMP_BLOCK_SIZE);
        let mut index = SparseIndex::new(dir, width, store, block_size);
        self.attach_sparse_chunk_root(table, index_id, &mut index);
        // The column type carries the declared bound (`SPARSE(float,int8,8)`);
        // the reference uses it as the index's term space.
        index.terms = self
            .column_types(table)
            .ok()
            .and_then(|t| t.get(ci).cloned())
            .and_then(|text| sparse_declared_bound(&text))
            .unwrap_or(0);
        let mut scanned = 0usize;
        for (seq, cells) in self.fetch_rows_with_ids_projected_all(table, ci)? {
            scanned += 1;
            // A NULL sparse cell has no postings; only stored values are indexed.
            let Some(Value::Str(text)) = cells.get(ci) else {
                continue;
            };
            let entries: Vec<(i32, f32)> = parse_sparse_str(text)
                .into_iter()
                .map(|(i, x)| (i as i32, x as f32))
                .collect();
            index.insert(seq as u64, entries);
        }
        // A row with no postings still counts as covered: the check above asks
        // whether this index saw the table, not how many documents it holds.
        index.scanned_rows = scanned;
        index.seal();
        let shared = Arc::new(std::sync::RwLock::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .sparse
            .insert(index_id, shared.clone());
        Ok(shared)
    }

    /// Tombstoned sequence ids of a table; index hits are validated against it.
    pub(crate) fn tombstone_set(&self, table: &str) -> Result<std::collections::HashSet<u64>> {
        // A delete the checkpoint has not written yet is in the table's delete
        // log (`note_deleted_ids`, `DeleteInBlock`); the block's `version`
        // carries it after the flush. The set is the one the journal keeps for
        // single-row lookups, so building it is not repeated per query.
        Ok((*self.deleted_set(table)).clone())
    }

    /// [`Self::tombstone_set`] as of `read_ts`: a delete that committed after
    /// it has not happened yet for this reader.
    pub(crate) fn tombstone_set_at(
        &self,
        table: &str,
        read_ts: u64,
    ) -> Result<std::collections::HashSet<u64>> {
        if read_ts == u64::MAX {
            return self.tombstone_set(table);
        }
        // Both journal halves in one load: each `deleted_ids`/`delete_ts` call
        // clones the whole journal, and this is the read path of every query.
        let (ids, ts) = self.load_deletes(table);
        let mut dead: std::collections::HashSet<u64> = Default::default();
        for (at, id) in ids.iter().enumerate() {
            let deleted = ts.get(at).copied().unwrap_or(0);
            // An unsettled entry is a delete the commit already accepted but
            // has not stamped; it counts, exactly as `tombstone_set` does.
            if deleted == 0 || deleted <= read_ts {
                dead.insert(*id);
            }
        }
        Ok(dead)
    }

    /// Live rows with only column `idx` parsed. A vector scan over a wide table
    /// must not decode every column: the text of all rows is what made
    /// concurrent KNN queries exhaust memory.
    pub(crate) fn fetch_rows_with_ids_projected(
        &self,
        table: &str,
        idx: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        // Project one column. The blocks decode it on demand; the journal's
        // tail (rows above the last checkpoint) is projected the same way, and
        // the result keeps the shape the callers expect - a full-width row with
        // NULLs outside the wanted column.
        let project = |cells: Vec<Value>| -> Vec<Value> {
            let value = cells.get(idx).cloned().unwrap_or(Value::Null);
            let mut out = vec![Value::Null; idx + 1];
            out[idx] = value;
            out
        };
        if !self.mirror_dirty(table) {
            if let Ok((_, rows)) = self.read_blocks_pruned(table, &[idx], &[]) {
                return Ok(rows
                    .into_iter()
                    .map(|(seq, cells)| {
                        (seq, project(cells.iter().map(|c| parse_value(c)).collect()))
                    })
                    .collect());
            }
        }
        Ok(self
            .row_set_from(table)?
            .into_iter()
            .map(|(seq, cells)| (seq, project(cells)))
            .collect())
    }

    /// Like `fetch_rows_with_ids_projected`, but including tombstoned rows.
    /// Corpus statistics keep counting deleted documents (as the reference's
    /// segments do until compaction), so the index must see them too.
    pub(crate) fn fetch_rows_with_ids_projected_all(
        &self,
        table: &str,
        idx: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        self.fetch_rows_with_ids_projected_from(table, idx, 0)
    }

    /// [`Self::fetch_rows_with_ids_projected_all`] from row id `first` on.
    pub(crate) fn fetch_rows_with_ids_projected_from(
        &self,
        table: &str,
        idx: usize,
        first: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        Ok(self
            .fetch_rows_including_deleted_from(table, first)?
            .into_iter()
            .map(|(seq, cells)| {
                let value = cells.get(idx).cloned().unwrap_or(Value::Null);
                let mut out = vec![Value::Null; idx + 1];
                out[idx] = value;
                (seq, out)
            })
            .collect())
    }

    /// Rows matched by an equality on an indexed column, or the whole table
    /// when no such index applies. Point deletes/updates over a big table must
    /// not materialise every row.
    pub(crate) fn rows_for_predicate(
        &self,
        table: &str,
        where_pred: Option<&Predicate>,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        if let Some((col, CmpOp::Eq, val)) = where_pred.and_then(|p| p.as_column_literal()) {
            if let Some(ids) = self.index_lookup(table, col, val) {
                return self.fetch_some_rows_with_ids(table, &ids);
            }
        }
        // No index answered, but the predicate still names its rows: a
        // comparison on one column is checked against the block's bytes, so
        // only the matching rows are ever decoded (this is the scan half of an
        // `UPDATE`/`DELETE`, which materialised the whole table before).
        if let Some(bounds) = where_pred.and_then(|p| {
            let schema = self.table_schema(table).ok()?;
            crate::blocks::ColumnBounds::from_predicate(&schema, p)
        }) {
            if let Some(ids) = self.row_ids_in_bounds(table, &bounds) {
                let ids: Vec<u64> = ids.into_iter().map(|id| id as u64).collect();
                return self.fetch_some_rows_with_ids(table, &ids);
            }
        }
        self.fetch_rows_with_ids(table)
    }

    /// Parses a stored row, padding trailing cells from the column defaults.
    ///
    /// `types` are the table's declared column types: the stored text is
    /// re-read with them so a text column's cell stays text. Inferring the cell
    /// type instead rewrote the value - `str::parse::<f64>` accepts `inf`,
    /// `infinity` and `nan` in any case, so a stored `iNF` came back as `inf`
    /// (and `007` as `7`, `1.50` as `1.5`).
    pub(crate) fn parse_stored_row(
        &self,
        table: &str,
        defaults: &[Option<String>],
        types: &[String],
        v: &[u8],
    ) -> Result<Vec<Value>> {
        // The stored form is written from a `String` and read straight back, so
        // the bytes are already valid UTF-8: a lossy conversion here allocated
        // a `Cow` per row for 8.4 M rows.
        let text = match std::str::from_utf8(v) {
            Ok(t) => t,
            Err(_) => return self.parse_stored_row_lossy(table, defaults, types, v),
        };
        let mut cells: Vec<Value> = Vec::new();
        for cell in text.split('\t') {
            cells.push(self.decode_cell(table, types, cells.len(), cell)?);
        }
        while cells.len() < defaults.len() {
            let i = cells.len();
            cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                Some(d) => Value::from_literal(&default_literal(d)),
                None => Value::Null,
            });
        }
        Ok(cells)
    }

    /// [`Self::parse_stored_row`] for bytes that are not valid UTF-8 (a replay
    /// of a file written by an older build).
    fn parse_stored_row_lossy(
        &self,
        table: &str,
        defaults: &[Option<String>],
        types: &[String],
        v: &[u8],
    ) -> Result<Vec<Value>> {
        let text = String::from_utf8_lossy(v);
        let mut cells: Vec<Value> = Vec::new();
        for cell in text.split('\t') {
            cells.push(self.decode_cell(table, types, cells.len(), cell)?);
        }
        while cells.len() < defaults.len() {
            let i = cells.len();
            cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                Some(d) => Value::from_literal(&default_literal(d)),
                None => Value::Null,
            });
        }
        Ok(cells)
    }

    /// Per-column declared type text for `table`.
    pub(crate) fn column_types(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .types)
    }

    /// The names of the stored snapshots (`CREATE SNAPSHOT`).
    /// C++ `NewTxnManager::CurrentTS`: the timestamp of the last commit.
    pub fn current_ts(&self) -> u64 {
        self.txn.current_ts()
    }

    /// Takes the transaction id the reference's `BeginTxnShared` would take for
    /// one statement.
    ///
    /// Every statement spends an id, read-only ones included (`SELECT`, `USE`,
    /// `SHOW …`), and the WAL frame the statement writes carries it. The
    /// session handshake (`Connect`/`Disconnect`) is not a statement.
    pub fn begin_statement(&self) {
        self.txn.ensure_statement_id();
    }

    /// Ends the statement scope opened by [`Self::begin_statement`].
    pub fn end_statement(&self) {
        self.txn.end_statement();
    }

    /// C++ `NewTxnManager::PrepareCommitTS`: the high-water mark committers
    /// take from.
    pub fn prepare_commit_ts(&self) -> u64 {
        self.txn.prepare_commit_ts()
    }

    /// True unless `result_cache` is explicitly `off`.
    /// Rows a single imported segment can hold (`1024 * 8192`).
    pub const SEGMENT_ROW_CAPACITY: usize = 1024 * 8192;

    /// Rows committed per block while streaming a `COPY … FROM`.
    pub(crate) const COPY_BATCH_ROWS: usize = 8192;

    /// `Self::SEGMENT_CAPACITY` (`table_meta.cppm`): the rows one segment
    /// holds. A `COPY` that reaches it seals the segment and opens the next.
    pub(crate) const SEGMENT_CAPACITY: usize = 1024 * 8192;

    /// Number of segments `table` has: at least one per write batch, and more
    /// when a single import exceeds the segment capacity.
    /// The table's segments with their row counts, for `SHOW SEGMENTS`. A table
    /// restored from disk has no allocator history yet, so its rows report as
    /// segment 0.
    pub fn segments(&self, table: &str) -> Vec<(u32, u32)> {
        let key = self.table_key(table);
        let mut out: Vec<(u32, u32)> = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.segments.values().map(|s| (s.id, s.rows)).collect())
            .unwrap_or_default();
        if out.is_empty() {
            let rows = self.row_count(table).unwrap_or(0);
            if rows > 0 {
                out.push((0, rows as u32));
            }
        }
        out
    }

    pub fn segment_count(&self, table: &str) -> usize {
        let key = self.table_key(table);
        let rows = self.row_count(table).unwrap_or(0);
        let batches = match self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.write_batches)
        {
            Some(b) => b.max(1),
            // Never written in this process: an empty table has no segment,
            // a table loaded from disk has at least one.
            None => usize::from(rows > 0),
        };
        batches.max(rows.div_ceil(Self::SEGMENT_ROW_CAPACITY).max(1))
    }

    /// C++ `TableMeta::GetSegmentIDs1`: the ids of the segments holding rows.
    ///
    /// The allocator's registry is authoritative after a write; a table that
    /// was restored from a snapshot has no allocator history, so its segment
    /// ids are derived from the stored row ids (`segment_id << 32 | offset`).
    pub fn segment_ids(&self, table: &str) -> Vec<u32> {
        let key = self.table_key(table);
        // A table the catalog no longer knows has no segments: the allocator's
        // in-memory counters outlive a `DROP`, so asking them would report the
        // segments of a dropped table.
        if self
            .storage
            .catalog
            .get_table_by_name(&key)
            .ok()
            .flatten()
            .is_none()
        {
            return Vec::new();
        }
        let ids: Vec<u32> = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.segments.keys().copied().collect())
            .unwrap_or_default();
        if !ids.is_empty() {
            return ids;
        }
        let mut ids: Vec<u32> = self
            .fetch_rows_with_ids(&key)
            .unwrap_or_default()
            .iter()
            .map(|(seq, _)| (seq >> 32) as u32)
            .collect();
        ids.sort_unstable();
        ids.dedup();
        ids
    }

    /// Rows reserved in each of `table`'s segments, in segment id order.
    ///
    /// Deleted rows are included: the reference's index chunks keep them (a
    /// block is append-only), so the chunk's `row_count` counts every row the
    /// segment ever held, not the live ones.
    /// The next segment id for `table`: one past **every** id it has ever used.
    ///
    /// A compaction deprecates its source segments (`drop|seg|…`) and `Cleanup`
    /// later removes the records, so the newest live id alone is not a
    /// watermark: reusing a retired id would give a fresh segment the row ids,
    /// chunk names and block directories of a dead one. The reference reads the
    /// same set - its segment map keeps an entry until cleanup, and
    /// `MetaTableObject::GetNextSegmentID` takes the map's maximum.
    pub(crate) fn next_segment_id(&self, table: &str) -> u32 {
        let qualified = self.qualified_table(table);
        let live = self
            .segment_row_counts(&qualified)
            .into_iter()
            .map(|(id, _)| id)
            .max();
        let dropped = self
            .storage
            .catalog
            .get_table_by_name(&qualified)
            .ok()
            .flatten()
            .and_then(|meta| {
                self.storage
                    .catalog
                    .dropped_segments(
                        &self.database_id_of(&qualified).to_string(),
                        &meta.id.to_string(),
                    )
                    .ok()
            })
            .unwrap_or_default()
            .into_iter()
            .max();
        match live.max(dropped) {
            // A table that has not written a row yet opens segment **0**
            // (`TableMeta::InitSet` starts the map empty and the first append
            // takes id 0).
            None => 0,
            Some(id) => id + 1,
        }
    }

    pub fn segment_row_counts(&self, table: &str) -> Vec<(u32, u64)> {
        let key = self.table_key(table);
        let tracked: Vec<(u32, u64)> = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| {
                e.segments
                    .values()
                    .map(|s| (s.id, u64::from(s.rows)))
                    .collect()
            })
            .unwrap_or_default();
        if !tracked.is_empty() {
            let mut tracked = tracked;
            tracked.sort_unstable();
            return tracked;
        }
        // No allocator history (a directory loaded from disk): derive the
        // per-segment counts from the stored row ids, deleted rows included.
        let mut counts: std::collections::BTreeMap<u32, u64> = Default::default();
        for (seq, _) in self.fetch_rows_including_deleted(&key).unwrap_or_default() {
            *counts.entry((seq >> 32) as u32).or_default() += 1;
        }
        if counts.is_empty() {
            // Neither the allocator nor the journal has the layout (the rows
            // live in the blocks): the blocks say how far each segment was
            // written, which is the same number the allocator would.
            return self.block_segment_row_counts(&key);
        }
        counts.into_iter().collect()
    }

    /// C++ `SegmentMeta::GetBlockIDs1`: the block ids of `segment`, in order.
    /// Blocks exist once the C++-format mirror has been written
    /// ([`Self::write_blocks`] / [`Self::flush`]).
    ///
    /// A deprecated segment has no visible block: a compaction keeps its files
    /// until `Cleanup` and hides it with `drop|seg|…` only, so listing its
    /// blocks here would make this reader disagree with the scan.
    pub fn block_ids(&self, table: &str, segment: u32) -> Vec<u32> {
        let mut ids: Vec<u32> = self
            .visible_block_ids(table)
            .into_iter()
            .filter(|id| id.segment_id == segment)
            .map(|id| u32::from(id.block_id))
            .collect();
        ids.sort_unstable();
        ids
    }

    /// C++ `BlockMeta::GetRowCnt1`.
    pub fn block_row_count(&self, table: &str, segment: u32, block: u16) -> Option<usize> {
        if !self
            .visible_block_ids(table)
            .iter()
            .any(|id| id.segment_id == segment && u32::from(id.block_id) == u32::from(block))
        {
            return None;
        }
        let index = self.block_index_for(table)?;
        let index = index.lock().unwrap();
        index
            .get_block_meta(segment, block)
            .map(|meta| meta.row_count)
    }

    /// Every file under the database's C++ data directory
    /// (C++ `NewTxn::GetDBFilePaths`).
    pub fn db_files(&self, db: &str) -> Vec<PathBuf> {
        let dir = self
            .dir
            .join("data")
            .join(format!("db_{}", self.database_id(db)));
        files_under(&dir)
    }

    /// Every file under the table's C++ data directory
    /// (C++ `NewTxn::GetTableFilePaths`).
    pub fn table_files(&self, table: &str) -> Vec<PathBuf> {
        let table = self.table_key(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let dir = self.blocks_root().join(format!(
            "db_{}/tbl_{}",
            self.database_id_of(&table),
            meta.id
        ));
        files_under(&dir)
    }

    /// C++ `NewTxn::GetColumnFilePaths`: the block-mirror files that belong to
    /// one column (`<column_id>.col`, one per block). Columns are packed into
    /// these per-column files inside the table's mirror directory; a column
    /// dropped after a checkpoint leaves them behind for cleanup to reclaim.
    pub fn column_files(&self, table: &str, column: &str) -> Vec<PathBuf> {
        let table = self.table_key(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let Some(idx) = meta.columns.iter().position(|c| c == column) else {
            return Vec::new();
        };
        let Some(column_id) = meta.ids.get(idx) else {
            return Vec::new();
        };
        let file_name = format!("{column_id}.col");
        let outline_name = format!("col_{column_id}_out");
        let dir = self.blocks_root().join(format!(
            "db_{}/tbl_{}",
            self.database_id_of(&table),
            meta.id
        ));
        files_under(&dir)
            .into_iter()
            .filter(|p| {
                p.file_name()
                    .is_some_and(|n| n == file_name.as_str() || n == outline_name.as_str())
            })
            .collect()
    }

    /// Number of rows in the in-memory HNSW segment of `column`
    /// (C++ `SegmentIndexMeta::GetMemIndex() != nullptr` plus its size).
    pub fn mem_index_len(&self, table: &str, column: &str) -> Option<usize> {
        let key = self.table_key(table);
        // Every HNSW index on `column` keeps its own graph; the answer is the
        // one a plain `MATCH VECTOR` resolves to (the first by id).
        let index_id = self
            .storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .ok()?
            .into_iter()
            .find(|im| im.column == column && im.index_type.eq_ignore_ascii_case("HNSW"))
            .map(|im| im.id)?;
        // The handle is cloned and `tables` released before it is locked (see
        // `text_index_for`).
        let mem = {
            let tables = self.tables.lock().unwrap();
            tables.get(&key)?.hnsw_mem.get(&index_id)?.clone()
        };
        let len = mem.lock().unwrap().len();
        Some(len)
    }

    /// Configured byte budget of the in-memory index buffers (built in at
    /// 8 GiB; the reference's `[buffer] buffer_manager_size` is ignored).
    pub fn buffer_memory_limit(&self) -> usize {
        self.context
            .buffer_budget
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// Bytes this engine's block mappings cover. Nothing is charged to a buffer
    /// pool any more, so this is the page-cache footprint the buffer budget
    /// used to approximate for blocks (the index chunks are mapped too, and
    /// counted process-wide by `storage::mapped`).
    pub fn buffer_used_bytes(&self) -> usize {
        self.context
            .block_maps
            .lock()
            .unwrap()
            .values()
            .map(|block| block.bytes())
            .sum()
    }

    /// Begins a table restore that is only applied by
    /// [`PendingRestore::commit`].
    pub fn begin_restore_table_snapshot(&self, name: &str) -> Result<PendingRestore<'_>> {
        self.begin_restore(RestoreScope::Table, name)
    }

    /// Begins a database restore that is only applied by
    /// [`PendingRestore::commit`].
    pub fn begin_restore_database_snapshot(&self, name: &str) -> Result<PendingRestore<'_>> {
        self.begin_restore(RestoreScope::Database, name)
    }

    /// Begins a system restore that is only applied by
    /// [`PendingRestore::commit`].
    pub fn begin_restore_system_snapshot(&self, name: &str) -> Result<PendingRestore<'_>> {
        self.begin_restore(RestoreScope::System, name)
    }

    pub(crate) fn begin_restore(
        &self,
        scope: RestoreScope,
        name: &str,
    ) -> Result<PendingRestore<'_>> {
        if !self.snapshot_names()?.iter().any(|n| n == name) {
            return Err(Error::NotFound(format!("Snapshot: {name} not found")));
        }
        Ok(PendingRestore {
            engine: self,
            scope,
            name: name.to_string(),
        })
    }

    /// C++ `CompactionProcessor::NewNotifyCompact`
    /// (`compaction_process_impl.cpp:86`): walk every database and table and
    /// merge the segments `NewCompactionAlg` selects. Returns how many tables
    /// were compacted.
    ///    /// The reference lists the tables in a read transaction and then compacts
    /// each one in its own compact transaction; here the catalog read is the
    /// listing and [`Self::compact_table`] opens the compaction.
    pub fn notify_compact(&self) -> usize {
        let databases = self.storage.catalog.list_databases().unwrap_or_default();
        let tables = self.storage.catalog.list_tables().unwrap_or_default();
        let mut compacted = 0;
        for meta in tables {
            // The catalog keys a table by the name it was created with: a
            // qualified `db.t` when the DDL named the database, the bare name
            // otherwise (then the session's current database is the one it
            // lives in, as in `qualified_table`). Prefer the database whose
            // lookup returns this very table.
            let qualified = databases.iter().find_map(|db| {
                let candidate = format!("{db}.{}", meta.name);
                self.storage
                    .catalog
                    .get_table_by_name(&candidate)
                    .ok()
                    .flatten()
                    .filter(|found| found.id == meta.id)
                    .map(|_| candidate)
            });
            let target = qualified.unwrap_or_else(|| meta.name.clone());
            if self.compaction_candidates(&target).is_empty() {
                continue;
            }
            if self.compact_table(&target).is_ok() {
                compacted += 1;
            }
        }
        compacted
    }

    /// `NewCompactionAlg` (`DBTCompactionAlg`): the segments a manual or
    /// periodic compaction would merge.
    ///
    /// Each segment lands in `layer(row_count) = max(0, ln(row_count / 8192) /
    /// ln(4))`; the first layer holding at least `DBT_COMPACTION_M` (4)
    /// segments donates its first four, unless they together exceed a
    /// segment's capacity (`new_compaction_alg_impl.cpp:44-79`,
    /// `DBT_COMPACTION_M/C/S` in `default_values.cppm:190-192`).
    ///
    /// Three tiny segments therefore compact to nothing while four compact to
    /// one - which is why the gate that exercises compaction imports four
    /// times.
    pub(crate) fn compaction_candidates(&self, table: &str) -> Vec<u32> {
        const M: usize = 4;
        const S: u64 = storage::format::DEFAULT_BLOCK_CAPACITY as u64;
        // `CompactionProcessor` drops the table's **unsealed** segment from the
        // candidate list before asking the algorithm
        // (`compaction_process_impl.cpp:176`): the reference only seals a
        // segment when a `COPY` writes it or it reaches the capacity, so the
        // segment an `INSERT` is still appending to must not be merged.
        let unsealed = self
            .tables
            .lock()
            .unwrap()
            .get(&self.table_key(table))
            .and_then(|e| e.row_counter)
            .filter(|(_, _, sealed)| !*sealed)
            .map(|(segment, _, _)| segment);
        let mut layers: std::collections::BTreeMap<i32, Vec<(u32, u64)>> = Default::default();
        for (segment, rows) in self.segment_row_counts(table) {
            if Some(segment) == unsealed {
                continue;
            }
            // `DBTConfig::CalculateLayer`: `log(row_cnt / s_) / log(c)` with
            // **integer** division inside the log, truncated to `int` and
            // floored at 0 (`-inf` for a segment smaller than one block casts
            // to `INT_MIN` and floors to 0).
            let blocks = rows / S;
            let layer = if blocks <= 1 {
                0
            } else {
                ((blocks as f64).ln() / 4f64.ln()) as i32
            };
            layers
                .entry(layer.max(0))
                .or_default()
                .push((segment, rows));
        }
        for layer in layers.values() {
            if layer.len() < M {
                continue;
            }
            let picked = &layer[..M];
            if picked.iter().map(|(_, rows)| *rows).sum::<u64>() > Self::SEGMENT_ROW_CAPACITY as u64
            {
                return Vec::new();
            }
            return picked.iter().map(|(segment, _)| *segment).collect();
        }
        Vec::new()
    }

    /// C++ `WalSegmentInfo` (`wal_entry_impl.cpp:135`): one block per
    /// `DEFAULT_BLOCK_CAPACITY` rows, `outline_infos` = `(1, 0)` per column,
    /// `row_capacity` = the segment's capacity.
    pub(crate) fn wal_segment_info(
        &self,
        segment_id: u32,
        rows: usize,
        columns: usize,
        row_counts: bool,
    ) -> storage::wal_entry::WalSegmentInfo {
        let block_capacity = storage::format::DEFAULT_BLOCK_CAPACITY;
        let mut block_infos = Vec::new();
        let mut block_id = 0u16;
        let mut left = rows;
        while left > 0 {
            let in_block = left.min(block_capacity);
            block_infos.push(storage::wal_entry::WalBlockInfo {
                block_id,
                row_count: if row_counts { in_block as u16 } else { 0 },
                row_capacity: block_capacity as u16,
                outline_infos: vec![(1u32, 0u64); columns],
            });
            left -= in_block;
            block_id += 1;
        }
        storage::wal_entry::WalSegmentInfo {
            segment_id,
            column_count: columns as u64,
            row_count: if row_counts { rows as u64 } else { 0 },
            actual_row_count: 0,
            row_capacity: Self::SEGMENT_ROW_CAPACITY as u64,
            block_infos,
        }
    }

    /// `Cleanup` reclaims the segments a compaction deprecated.
    ///
    /// `NewTxn::Compact` only marks them (`drop|seg|…`); `NewCatalog::
    /// CleanSegment` is what removes the segment's blocks, its index chunks and
    /// the mark itself. The port therefore keeps the source directories while
    /// the table is merely compacted (`write_blocks`) and drops them here.
    pub(crate) fn reclaim_deprecated_segments(&self) -> Result<()> {
        self.reclaim_dropped_entities()?;
        for meta in self.storage.catalog.list_tables()? {
            let db_id = self.database_id_of(&meta.name);
            let dropped = self
                .storage
                .catalog
                .dropped_segments(&db_id.to_string(), &meta.id.to_string())?;
            if dropped.is_empty() {
                continue;
            }
            let index_ids: Vec<u64> = self
                .storage
                .catalog
                .indexes_of_table(&meta.name)?
                .iter()
                .map(|im| im.id)
                .collect();
            // The records being reclaimed live in the C++ key space, i.e. the
            // catalog store, not in the row store.
            let store = self.storage.catalog.store().clone();
            let root = self.blocks_root();
            for segment in &dropped {
                for prefix in [
                    format!("catalog|blk|{db_id}|{}|{segment}|", meta.id),
                    format!("seg|{db_id}|{}|{segment}|", meta.id),
                    format!("blk|{db_id}|{}|{segment}|", meta.id),
                ] {
                    for (k, _) in store.scan_prefix(prefix.as_bytes())? {
                        store.delete(&k)?;
                    }
                }
                store.delete(
                    storage::catalog_format::catalog_segment_key(
                        &db_id.to_string(),
                        &meta.id.to_string(),
                        *segment,
                    )
                    .as_bytes(),
                )?;
                for index_id in &index_ids {
                    let prefix = format!("idx_chunk|{db_id}|{}|{index_id}|{segment}|", meta.id);
                    for (k, _) in store.scan_prefix(prefix.as_bytes())? {
                        store.delete(&k)?;
                    }
                    store.delete(
                        catalog::layout::dropped_index_segment_key(
                            &db_id.to_string(),
                            &meta.id.to_string(),
                            *index_id,
                            *segment,
                        )
                        .as_bytes(),
                    )?;
                    storage::chunk_store::remove_tree(root.join(format!(
                        "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                        meta.id
                    )));
                }
                store.delete(
                    catalog::layout::dropped_segment_key(
                        &db_id.to_string(),
                        &meta.id.to_string(),
                        *segment,
                    )
                    .as_bytes(),
                )?;
                storage::chunk_store::remove_tree(
                    root.join(format!("db_{db_id}/tbl_{}/seg_{segment}", meta.id)),
                );
            }
        }
        // Directories of tables that no longer exist at all: `DROP TABLE` /
        // `DROP DATABASE` removes the catalog records, so the loop above never
        // hears about their segments and the files would stay behind for ever.
        // Walking the block root is what makes cleanup complete, the way the
        // reference's cleanup removes whatever the catalog does not reference.
        let live: std::collections::HashSet<(String, String)> = self
            .storage
            .catalog
            .list_tables()?
            .iter()
            .map(|m| (self.database_id_of(&m.name).to_string(), m.id.to_string()))
            .collect();
        let root = self.blocks_root();
        if let Ok(dbs) = std::fs::read_dir(&root) {
            for db in dbs.flatten() {
                let db_name = db.file_name().to_string_lossy().to_string();
                let Some(db_id) = db_name.strip_prefix("db_") else {
                    continue;
                };
                let Ok(tables) = std::fs::read_dir(db.path()) else {
                    continue;
                };
                for tbl in tables.flatten() {
                    let tbl_name = tbl.file_name().to_string_lossy().to_string();
                    let Some(table_id) = tbl_name.strip_prefix("tbl_") else {
                        continue;
                    };
                    if !live.contains(&(db_id.to_string(), table_id.to_string())) {
                        storage::chunk_store::remove_tree(tbl.path());
                    }
                }
            }
        }
        Ok(())
    }

    /// `Cleanup` reclaims what a `DROP` deprecated.
    ///
    /// `DROP DATABASE`, `DROP TABLE` and `DROP INDEX` are **markers** in the
    /// reference: `NewTxn::PrepareCommitDropDB` writes `drop|db|…`,
    /// `PrepareCommitDropTable` writes `drop|tbl|…` and `DropIndexByName` writes
    /// `drop|idx|…`, each holding the transaction's commit ts, and that is the
    /// whole statement - the catalog records, the segment layer and the files
    /// all stay. `NewTxn::Cleanup` is what reads every marker whose ts is at or
    /// below the visibility horizon (`NewCatalog::GetCleanedMeta`) and then
    /// removes the entity it deprecated (`CleanDB` / `CleanTable` /
    /// `CleanTableIndex`, `new_txn_impl.cpp:5172`).
    ///
    /// The port therefore keeps the directories while the entity is merely
    /// dropped and removes them here, and only here.
    pub(crate) fn reclaim_dropped_entities(&self) -> Result<()> {
        let store = self.storage.catalog.store().clone();
        let root = self.blocks_root();
        let prefix_keys = |prefix: &str| -> Result<Vec<Vec<u8>>> {
            Ok(store
                .scan_prefix(prefix.as_bytes())?
                .into_iter()
                .map(|(k, _)| k)
                .collect())
        };
        let delete_all = |keys: Vec<Vec<u8>>| -> Result<()> {
            if keys.is_empty() {
                return Ok(());
            }
            let batch: Vec<(Vec<u8>, Option<Vec<u8>>)> =
                keys.into_iter().map(|k| (k, None)).collect();
            store.atomic_batch(&batch)
        };

        // `drop|db|{name}/{create_ts}/{db_id}`
        for (key, _) in store.scan_prefix(b"drop|db|")? {
            let text = String::from_utf8_lossy(&key).to_string();
            let Some(rest) = text.strip_prefix("drop|db|") else {
                continue;
            };
            let parts: Vec<&str> = rest.split('/').collect();
            if parts.len() != 3 {
                continue;
            }
            let (name, create_ts, db_id) = (parts[0], parts[1], parts[2]);
            // `NewCatalog::CleanDB` walks the database's tables and cleans each
            // one, then takes the database's own record and tag away.
            for (tk, tv) in store.scan_prefix(format!("catalog|tbl|{db_id}|").as_bytes())? {
                let table_id = String::from_utf8_lossy(&tv).to_string();
                self.purge_table_records(&store, db_id, &table_id)?;
                store.delete(&tk)?;
            }
            let mut keys = prefix_keys(&format!("drop|tbl|{db_id}/"))?;
            keys.extend(prefix_keys(&format!("drop|idx|{db_id}/"))?);
            keys.extend(prefix_keys(&format!("db|{db_id}|"))?);
            keys.push(key);
            keys.push(
                storage::catalog_format::catalog_db_key(name, create_ts.parse().unwrap_or(0))
                    .into_bytes(),
            );
            delete_all(keys)?;
            storage::chunk_store::remove_tree(root.join(format!("db_{db_id}")));
        }

        // `drop|tbl|{db_id}/{name}/{create_ts}/{table_id}`
        for (key, _) in store.scan_prefix(b"drop|tbl|")? {
            let text = String::from_utf8_lossy(&key).to_string();
            let Some(rest) = text.strip_prefix("drop|tbl|") else {
                continue;
            };
            let parts: Vec<&str> = rest.split('/').collect();
            if parts.len() != 4 {
                continue;
            }
            let (db_id, name, create_ts, table_id) = (parts[0], parts[1], parts[2], parts[3]);
            self.purge_table_records(&store, db_id, table_id)?;
            let mut keys = prefix_keys(&format!("drop|idx|{db_id}/{table_id}/"))?;
            keys.push(key);
            keys.push(
                storage::catalog_format::catalog_table_key(
                    db_id,
                    name,
                    create_ts.parse().unwrap_or(0),
                )
                .into_bytes(),
            );
            delete_all(keys)?;
            storage::chunk_store::remove_tree(root.join(format!("db_{db_id}/tbl_{table_id}")));
        }

        // `drop|idx|{db_id}/{table_id}/{name}/{create_ts}/{index_id}`
        for (key, _) in store.scan_prefix(b"drop|idx|")? {
            let text = String::from_utf8_lossy(&key).to_string();
            let Some(rest) = text.strip_prefix("drop|idx|") else {
                continue;
            };
            let parts: Vec<&str> = rest.split('/').collect();
            if parts.len() != 5 {
                continue;
            }
            let (db_id, table_id, name, create_ts, index_id) =
                (parts[0], parts[1], parts[2], parts[3], parts[4]);
            self.purge_index_records(&store, db_id, table_id, name, create_ts, index_id)?;
            delete_all(vec![key])?;
            storage::chunk_store::remove_tree(
                root.join(format!("db_{db_id}/tbl_{table_id}/idx_{index_id}")),
            );
        }
        Ok(())
    }

    /// Every record below `db_id`.`table_id`: the column definitions, the
    /// table's tags, the segment and block layer, and the indexes' records.
    ///
    /// This is what `TableMeta::UninitSet` + `NewCatalog::CleanTable` remove
    /// (`table_meta_impl.cpp:339`, `new_catalog_static_impl.cpp:578`). The
    /// table's own `catalog|tbl|…` record is the caller's - it is keyed by the
    /// table's *name*, not its id.
    pub(crate) fn purge_table_records(
        &self,
        store: &Arc<dyn storage::MetaStore>,
        db_id: &str,
        table_id: &str,
    ) -> Result<()> {
        let mut keys: Vec<Vec<u8>> = Vec::new();
        for prefix in [
            format!("tbl|col|{db_id}|{table_id}|"),
            format!("col|{db_id}|{table_id}|"),
            format!("tbl|{db_id}|{table_id}|"),
            format!("catalog|seg|{db_id}|{table_id}|"),
            format!("seg|{db_id}|{table_id}|"),
            format!("catalog|blk|{db_id}|{table_id}|"),
            format!("blk|{db_id}|{table_id}|"),
            format!("blk_col|{db_id}|{table_id}|"),
            format!("catalog|idx|{db_id}|{table_id}|"),
            format!("idx|{db_id}|{table_id}|"),
            format!("idx_seg|{db_id}|{table_id}|"),
            format!("idx_chunk|{db_id}|{table_id}|"),
            format!("drop|seg|{db_id}/{table_id}/"),
            format!("drop|idx_seg|{db_id}/{table_id}/"),
            format!("drop|blk_col|{db_id}/{table_id}/"),
            format!("drop|tbl_col|{db_id}/{table_id}/"),
        ] {
            keys.extend(
                store
                    .scan_prefix(prefix.as_bytes())?
                    .into_iter()
                    .map(|(k, _)| k),
            );
        }
        if keys.is_empty() {
            return Ok(());
        }
        let batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = keys.into_iter().map(|k| (k, None)).collect();
        store.atomic_batch(&batch)
    }

    /// Every record of one index: its `catalog|idx|…` declaration (all
    /// versions), its `idx|…` tags, its `idx_seg|`/`idx_chunk|` layer and the
    /// `drop|idx_seg|` markers a compaction left.
    pub(crate) fn purge_index_records(
        &self,
        store: &Arc<dyn storage::MetaStore>,
        db_id: &str,
        table_id: &str,
        name: &str,
        create_ts: &str,
        index_id: &str,
    ) -> Result<()> {
        let mut keys: Vec<Vec<u8>> = Vec::new();
        keys.extend(
            store
                .scan_prefix(format!("catalog|idx|{db_id}|{table_id}|{name}|").as_bytes())?
                .into_iter()
                .map(|(k, _)| k),
        );
        keys.push(
            storage::catalog_format::catalog_index_key(
                db_id,
                table_id,
                name,
                create_ts.parse().unwrap_or(0),
            )
            .into_bytes(),
        );
        for prefix in [
            format!("idx|{db_id}|{table_id}|{index_id}|"),
            format!("idx_seg|{db_id}|{table_id}|{index_id}|"),
            format!("idx_chunk|{db_id}|{table_id}|{index_id}|"),
            format!("drop|idx_seg|{db_id}/{table_id}/{index_id}/"),
        ] {
            keys.extend(
                store
                    .scan_prefix(prefix.as_bytes())?
                    .into_iter()
                    .map(|(k, _)| k),
            );
        }
        let batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = keys.into_iter().map(|k| (k, None)).collect();
        store.atomic_batch(&batch)
    }

    /// Rows held by `segment` of `table` (used by `ShowBlocks`).
    pub fn segment_rows(&self, table: &str, segment: usize) -> usize {
        // Prefer the allocator's per-segment counts: several write batches can
        if let Some((_, rows)) = self
            .segments(table)
            .into_iter()
            .find(|(id, _)| *id as usize == segment)
        {
            return rows as usize;
        }
        let rows = self.row_count(table).unwrap_or(0);
        let by_size = rows.div_ceil(Self::SEGMENT_ROW_CAPACITY).max(1);
        if by_size == self.segment_count(table) {
            rows.saturating_sub(segment * Self::SEGMENT_ROW_CAPACITY)
                .min(Self::SEGMENT_ROW_CAPACITY)
        } else {
            rows / self.segment_count(table).max(1)
        }
    }

    /// The current value of a configuration knob (the `config` map filled by
    /// `SET CONFIG`), for the `ShowConfig` RPC. Known knobs fall back to the
    /// engine defaults the reference reports before any `SET CONFIG`.
    pub fn config_get(&self, key: &str) -> Option<String> {
        let key = key.to_ascii_lowercase();
        if let Some(v) = self.context.config.lock().unwrap().get(&key) {
            return Some(v.clone());
        }
        // `mem_index_capacity` is an `IntegerOption` in the reference and its
        // value comes from the running engine, not from the config map.
        if key == "mem_index_capacity" {
            return Some(self.memindex_capacity().to_string());
        }
        let default = match key.as_str() {
            "log_level" => "info",
            "result_cache" => "off",
            "cache_result_capacity" => "1024",
            "checkpoint_interval" => "0",
            "cleanup_interval" => "1000",
            _ => return None,
        };
        Some(default.to_string())
    }

    /// Every configuration knob's current value.
    pub fn config_all(&self) -> Vec<(String, String)> {
        self.context
            .config
            .lock()
            .unwrap()
            .iter()
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect()
    }

    pub(crate) fn result_cache_enabled(&self) -> bool {
        match self.context.config.lock().unwrap().get("result_cache") {
            Some(v) => !v.eq_ignore_ascii_case("off"),
            None => true,
        }
    }

    /// Whether new results are being cached (`off`/`suspend` disable it).
    pub(crate) fn result_cache_active(&self) -> bool {
        // C++ `DEFAULT_RESULT_CACHE = "off"` (`default_values.cppm:211`): the
        // result cache is opt-in, so an unset configuration means "do not
        // cache". This port used to default to on, which made the engine cache
        // results the reference would recompute.
        match self.context.config.lock().unwrap().get("result_cache") {
            Some(v) => !(v.eq_ignore_ascii_case("off") || v.eq_ignore_ascii_case("suspend")),
            None => false,
        }
    }

    /// `cache_result_capacity` as `SHOW VARIABLES` reports it.
    ///
    /// [`Self::result_cache_capacity`] returns `usize::MAX` as the internal
    /// "unbounded" sentinel, which used to leak into the variable table as
    /// 18446744073709551615. The reference reports the configured capacity
    /// (`ResultCacheManager::cache_num_capacity`, default
    /// `DEFAULT_CACHE_RESULT_CAPACITY`), so the sentinel must not be visible.
    pub(crate) fn reported_result_cache_capacity(&self) -> usize {
        self.context
            .config
            .lock()
            .unwrap()
            .get("cache_result_capacity")
            .and_then(|v| v.parse().ok())
            .unwrap_or(1024)
    }

    /// Maximum cached entries (`cache_result_capacity`); `usize::MAX` means
    /// unbounded. Only the internal cache check uses this sentinel — see
    /// [`Self::reported_result_cache_capacity`] for what a client sees.
    pub(crate) fn result_cache_capacity(&self) -> usize {
        self.context
            .config
            .lock()
            .unwrap()
            .get("cache_result_capacity")
            .and_then(|v| v.parse().ok())
            .unwrap_or(usize::MAX)
    }

    /// Validates `CREATE INDEX … USING <kind> WITH (…)`: the index kind must
    /// suit the column type and the options must be in range.
    pub(crate) fn validate_index(
        &self,
        table: &str,
        column: &str,
        index_type: &str,
        opts: &[(String, String)],
    ) -> Result<()> {
        let schema = self.table_schema(table)?;
        let Some(ci) = schema.iter().position(|c| c == column) else {
            // A functional-index target must be a call to a known scalar.
            // A plain arithmetic target (`c1 + 5`) is a valid functional index;
            // only a call `fn(...)` has to name a known scalar.
            let fname = column
                .split('(')
                .next()
                .unwrap_or("")
                .trim()
                .to_ascii_lowercase();
            if !column.contains('(') {
                return Ok(());
            }
            const KNOWN: &[&str] = &[
                "abs",
                "ceil",
                "char_length",
                "character_length",
                "datepart",
                "day",
                "exp",
                "floor",
                "length",
                "ln",
                "log",
                "log10",
                "log2",
                "lower",
                "ltrim",
                "md5",
                "month",
                "pow",
                "power",
                "reverse",
                "round",
                "rtrim",
                "sqrt",
                "substr",
                "substring",
                "trim",
                "trunc",
                "upper",
                "year",
            ];
            if !KNOWN.contains(&fname.as_str()) {
                return Err(Error::Invalid(format!(
                    "invalid function '{fname}' in an index expression"
                )));
            }
            return Ok(());
        };
        let ty = self
            .column_types(table)?
            .get(ci)
            .cloned()
            .unwrap_or_default()
            .to_ascii_uppercase();
        match index_type.to_ascii_uppercase().as_str() {
            "" => {}
            "BMP" => {
                if !is_sparse_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "BMP index requires a sparse column, got '{ty}'"
                    )));
                }
                for (k, v) in opts {
                    match k.as_str() {
                        "BLOCK_SIZE" => {
                            let n: i64 = v.parse().unwrap_or(-1);
                            if !(1..=256).contains(&n) {
                                return Err(Error::Invalid(format!(
                                    "block_size {v} is out of range 1..=256"
                                )));
                            }
                        }
                        "COMPRESS_TYPE" => {
                            let v = v.to_ascii_uppercase();
                            if v != "COMPRESS" && v != "RAWW" {
                                return Err(Error::Invalid(format!(
                                    "unsupported compress_type '{v}'"
                                )));
                            }
                        }
                        _ => {}
                    }
                }
            }
            "HNSW" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
                // LVQ encoding is unsupported for integer element types
                let int_elem = matches!(
                    tensor_elem_type(&ty),
                    ElemType::Int8
                        | ElemType::UInt8
                        | ElemType::Int16
                        | ElemType::Int32
                        | ElemType::Int64
                );
                let bit_elem = tensor_elem_type(&ty) == ElemType::Bit;
                for (k, v) in opts {
                    if !matches!(
                        k.as_str(),
                        "M" | "EF_CONSTRUCTION"
                            | "METRIC"
                            | "ENCODE"
                            | "BUILD_TYPE"
                            | "LSG_CONFIG"
                            | "BLOCK_SIZE"
                    ) {
                        return Err(Error::Invalid(format!(
                            "invalid index parameter: unsupported HNSW option '{k}'"
                        )));
                    }
                    if k == "ENCODE" && v.eq_ignore_ascii_case("lvq") && int_elem {
                        return Err(Error::Invalid(
                            "LVQ encoding is not supported for integer embeddings".into(),
                        ));
                    }
                    // `hamming` is only defined on bit vectors.
                    if k == "METRIC" && v.eq_ignore_ascii_case("hamming") && !bit_elem {
                        return Err(Error::Invalid(format!(
                            "invalid index parameter: metric 'hamming' requires a bit embedding, got '{ty}'"
                        )));
                    }
                }
            }
            "PLAID" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
                // `IndexPLAID::Make` (`index_plaid_impl.cpp:68-105`): the mode
                // flag takes `true`/`1`/`false`/`0` and nothing else, and a
                // ColBERTSaR index stores no residual, so its `nbits` is not
                // validated (it is unused).
                let mut colbertsar = false;
                for (k, v) in opts {
                    // `IndexPLAID::Make` rejects any parameter it does not know.
                    if !matches!(
                        k.to_ascii_uppercase().as_str(),
                        "NBITS" | "N_CENTROIDS" | "CENTROIDS" | "COLBERTSAR_MODE"
                    ) {
                        return Err(Error::Invalid(format!("Invalid index parameter type: {k}")));
                    }
                    if k.eq_ignore_ascii_case("COLBERTSAR_MODE") {
                        match v.trim().to_ascii_lowercase().as_str() {
                            "true" | "1" => colbertsar = true,
                            "false" | "0" => colbertsar = false,
                            _ => {
                                return Err(Error::Invalid(
                                    "colbertsar_mode must be 'true' or 'false'".into(),
                                ))
                            }
                        }
                    }
                }
                if !colbertsar {
                    for (k, v) in opts {
                        if k.eq_ignore_ascii_case("NBITS") {
                            let n: i64 = v.parse().unwrap_or(0);
                            if n != 2 && n != 4 {
                                return Err(Error::Invalid(format!("nbits {n} must be 2 or 4")));
                            }
                        }
                    }
                }
            }
            "IVF" | "FLAT" | "EMVB" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
                if index_type.eq_ignore_ascii_case("EMVB") {
                    // `IndexEMVB::ValidateColumnDataType` + `GetEMVBOPQ`:
                    // tensor-of-float only, both PQ options required, and the
                    // subspace count has to divide the dimension
                    // (`index_emvb_impl.cpp:36-53`, `product_quantizer_impl.cpp:447`).
                    if !ty.starts_with("TENSOR") || tensor_elem_type(&ty) != ElemType::Float32 {
                        return Err(Error::Invalid(format!(
                            "Attempt to create index on column: {column}, data type: {ty}."
                        )));
                    }
                    // `IndexEMVB::Make` checks the parameter *names* and their
                    // value sets only; the "dimension must divide the subspace
                    // count" test lives in `GetEMVBOPQ` (the PQ constructor,
                    // i.e. index-build time), so a DDL like
                    // `TENSOR(FLOAT,4) … pq_subspace_num = 8` is accepted by the
                    // reference and only blows up when the index is built.
                    let mut subspaces = None;
                    let mut bits = None;
                    for (k, v) in opts {
                        if k.eq_ignore_ascii_case("pq_subspace_num") {
                            let n: usize = v.trim().parse().unwrap_or(0);
                            if !matches!(n, 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128) {
                                return Err(Error::Invalid(
                                    "Invalid index parameter type: pq_subspace_num".into(),
                                ));
                            }
                            subspaces = Some(n);
                        } else if k.eq_ignore_ascii_case("pq_subspace_bits") {
                            let n: u32 = v.trim().parse().unwrap_or(0);
                            if n != 8 && n != 16 {
                                return Err(Error::Invalid(
                                    "Invalid index parameter type: pq_subspace_bits".into(),
                                ));
                            }
                            bits = Some(n);
                        } else {
                            return Err(Error::Invalid(format!(
                                "Invalid index parameter type: {k}"
                            )));
                        }
                    }
                    // The reference defaults both to 0 and then rejects them
                    // (`0` is not in either acceptable set), so the two options
                    // are effectively mandatory.
                    if subspaces.is_none() {
                        return Err(Error::Invalid(
                            "Invalid index parameter type: pq_subspace_num".into(),
                        ));
                    }
                    if bits.is_none() {
                        return Err(Error::Invalid(
                            "Invalid index parameter type: pq_subspace_bits".into(),
                        ));
                    }
                }
                if index_type.eq_ignore_ascii_case("IVF") {
                    // C++ `IndexIVF::Make`: `metric` is mandatory and the
                    // storage option decides which quantization parameters are
                    // required. Reject what the reference rejects.
                    let lowered: Vec<(String, String)> = opts
                        .iter()
                        .map(|(k, v)| (k.to_ascii_lowercase(), v.clone()))
                        .collect();
                    let params: Vec<(&str, &str)> = lowered
                        .iter()
                        .map(|(k, v)| (k.as_str(), v.as_str()))
                        .collect();
                    index::ivf_def::IndexIvf::make(
                        column,
                        "",
                        "",
                        vec![column.to_string()],
                        &params,
                    )
                    .map_err(|e| Error::Invalid(e.to_string()))?;
                }
            }
            "SECONDARY" | "SECONDARYFUNCTIONAL" => {
                // `DataType::CanBuildSecondaryIndex` (`data_type.h:99`) narrowed
                // to what the factories actually instantiate
                // (`secondary_index_data_impl.cpp:808-902`): every other type
                // - ARRAY, BLOB, UUID, FLOAT16, DECIMAL - has no ordered key and
                // the reference fails the index definition.
                const SECONDARY_TYPES: [&str; 13] = [
                    "BOOL",
                    "TINYINT",
                    "SMALLINT",
                    "INT",
                    "BIGINT",
                    "FLOAT",
                    "DOUBLE",
                    "DATE",
                    "TIME",
                    "DATETIME",
                    "TIMESTAMP",
                    "VARCHAR",
                    "JSON",
                ];
                if !SECONDARY_TYPES.iter().any(|t| ty.starts_with(t)) {
                    return Err(Error::Invalid(format!(
                        "Cannot build secondary index on data type: {ty}"
                    )));
                }
                // `IndexSecondary::ValidateColumnDataType`: a JSON index is
                // always low cardinality, so `cardinality = high` is an invalid
                // definition while `low` is accepted (and redundant).
                if is_json_type(&ty) {
                    if let Some((_, value)) = opts
                        .iter()
                        .find(|(key, _)| key.eq_ignore_ascii_case("CARDINALITY"))
                    {
                        let value = value.trim().trim_matches(|c| c == '\'' || c == '"');
                        if !value.eq_ignore_ascii_case("low") {
                            return Err(Error::Invalid(
                                "cardinality is always low for JSON column".into(),
                            ));
                        }
                    }
                }
            }
            "FULLTEXT" => {
                // A full-text index can only invert a text column.
                if !ty.starts_with("VARCHAR") {
                    return Err(Error::Invalid(format!(
                        "FULLTEXT index requires a varchar column, got '{ty}'"
                    )));
                }
                for (k, v) in opts {
                    if k.eq_ignore_ascii_case("ANALYZER") {
                        // C++ `IndexFullText::Make` hands the raw parameter to
                        // `AnalyzerPool::GetAnalyzer` and fails on its status,
                        // so the name is validated case sensitively and with
                        // the same per-family suffix rules (including the
                        // `strtol` prefix of `ngram-<n>`).
                        let a = v.trim().trim_matches(|c| c == '\'' || c == '"');
                        if !analyzer::is_constructible_analyzer_name(a) {
                            return Err(Error::Invalid(analyzer::unsupported_analyzer_message(a)));
                        }
                    }
                }
            }
            _ => {}
        }
        Ok(())
    }

    /// The rows a `SEARCH MATCH TEXT` may score, ascending: the `WHERE`, the
    /// `FILTER (…)` clause and the `filter=…` option evaluated once over the
    /// table, minus the tombstones. `None` when the query carries no filter.
    ///
    /// The result travels to the scorers as row ids rather than as a predicate
    /// closure because both of them **seek** with it: the block-max WAND moves
    /// every cursor past a rejected row and the streaming path advances every
    /// merge cursor, instead of scoring/fetching the row and dropping it.
    fn allowed_rows(
        &self,
        table: &str,
        pred: Option<&Expr>,
        in_filter: Option<&Expr>,
        opt_filter: Option<&Expr>,
        dead: &std::collections::HashSet<u64>,
    ) -> Result<Option<wand::DocFilter>> {
        if pred.is_none() && in_filter.is_none() && opt_filter.is_none() {
            return Ok(None);
        }
        let mut rows: Vec<u64> = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            let row_id = seq as u64;
            // A tombstone can never be scored, so it is not an admitted row
            // either: leaving it out makes the seek skip deletes too.
            if dead.contains(&row_id) {
                continue;
            }
            let ok = pred.map(|p| p.eval_predicate(&row)).unwrap_or(true)
                && in_filter.map(|p| p.eval_predicate(&row)).unwrap_or(true)
                && opt_filter.map(|p| p.eval_predicate(&row)).unwrap_or(true);
            if ok {
                rows.push(row_id);
            }
        }
        // The row cache the fetch may answer from is not ordered by row id, and
        // the pushdown needs `partition_point` to work.
        rows.sort_unstable();
        rows.dedup();
        Ok(Some(wand::DocFilter::new(rows)))
    }

    /// Selects rows for `SEARCH MATCH TEXT` by term presence. This reproduces
    /// the *row set* (any query term, `minimum_should_match`, `topn`); the exact
    /// BM25 ordering/threshold needs the iresearch scorer, so a query carrying
    /// `threshold` yields no rows rather than wrong ones.
    pub(crate) fn search_text_terms(
        &self,
        table: &str,
        m: &MatchClause,
        pred: Option<&Expr>,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        // A full-text scan holds the whole table's token data; bound how many
        // run at once so a handful of concurrent readers cannot exhaust memory.
        let _scan = self.context.scan_gate.enter();
        let out = self.search_text_terms_with_slot(table, m, pred, hidden)?;
        // An empty answer on a table full of matches is the failure this trace
        // exists for: record what the index held when it answered.
        if out.0.is_empty() {
            let tkey = self.table_key(table);
            if ft_trace_enabled(&tkey) {
                let Some(col) = match_columns(&m.column).first().cloned() else {
                    return Ok(out);
                };
                let Some(index_id) = self.fulltext_index_id(table, &col) else {
                    return Ok(out);
                };
                let shared = self
                    .tables
                    .lock()
                    .unwrap()
                    .get(&tkey)
                    .and_then(|e| e.text.get(&index_id).cloned());
                match shared {
                    Some(shared) => {
                        let g = shared.read().unwrap_or_else(|e| e.into_inner());
                        eprintln!(
                            "[FT] {tkey}.{} MATCH {:?} -> 0 rows; docs={} segs={} tail={} overridden={}",
                            m.column,
                            m.args.first().map(|a| format!("{a:?}")).unwrap_or_default(),
                            g.total_docs(),
                            g.reader.segments.len(),
                            g.tails().map(|x| x.pending_docs()).sum::<usize>(),
                            g.overridden.len()
                        );
                    }
                    None => eprintln!("[FT] {tkey}.{} MATCH -> 0 rows; no cached index", m.column),
                }
            }
        }
        Ok(out)
    }

    /// [`Self::search_text_terms`] for a caller that already holds the scan
    /// slot. The `default_field` fallback re-enters the statement with the
    /// field filled in, and taking a second slot on one thread is a deadlock:
    /// the gate admits two scans, so two concurrent statements doing that
    /// would each wait for the slot the other is holding.
    fn search_text_terms_with_slot(
        &self,
        table: &str,
        m: &MatchClause,
        pred: Option<&Expr>,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let schema = self.table_schema(table)?;
        // A full-text search runs through an index; without one the C++ engine
        // refuses the statement.
        if m.kind.eq_ignore_ascii_case("text") {
            let indexes = self.storage.catalog.list_indexes().unwrap_or_default();
            for col in match_columns(&m.column) {
                let has = indexes.iter().any(|im| {
                    im.table == table
                        && im.column == col
                        && im.index_type.eq_ignore_ascii_case("FULLTEXT")
                });
                if !has {
                    return Err(Error::Invalid(format!(
                        "fulltext index does not exist for column '{col}'"
                    )));
                }
            }
        }
        // `SEARCH MATCH TEXT (col, query [, topn] [, options])`: the reference
        // puts the row count in the third slot and the option string in the
        // fourth, so the arguments after the query are told apart by type.
        // Reading `args[1]` unconditionally used to take `topn` as the option
        // string, which silently disabled every option (`operator`,
        // `threshold`, `block_max`, `filter`, `similarity`, `rank_features`,
        // `minimum_should_match`, `bm25_param_*`).
        let mut topn_positional: Option<usize> = None;
        let mut options: Option<String> = None;
        for arg in m.args.iter().skip(1) {
            if let Some(n) = super::state::arg_usize(arg) {
                topn_positional = Some(n);
                continue;
            }
            if let ExprAst::Literal(Literal::Str(text)) = arg {
                options = Some(text.clone());
            }
        }
        let params = options.unwrap_or_default();
        // `threshold=<f>` keeps only documents scoring at least `f`
        let threshold: Option<f64> = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("threshold"))
            .and_then(|(_, v)| v.trim().parse().ok())
            // The reference's `threshold_ <= 0.0f` shortcut
            // (`term_doc_iterator_impl.cpp:118`): a non-positive threshold
            // filters nothing.
            .filter(|t| *t > 0.0);
        // `bm25_param_delta_term` / `bm25_param_delta_phrase` add
        // `idf * delta` to each matching unit: `TermDocIterator::InitBM25Info`
        // folds them into `f4 = delta / (k1 + 1)` which is added to the
        // frequency term and then scaled by `idf * (k1 + 1) * weight`.
        let bm25_param_opt = |name: &str| -> Option<f64> {
            params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case(name))
                .and_then(|(_, v)| v.trim().parse().ok())
        };
        let bm25_param = |name: &str| -> f64 { bm25_param_opt(name).unwrap_or(0.0) };
        let (bm25_k1, bm25_b) = parse_bm25_k1_b(&params)?;
        // `bm25_param_delta` is a shorthand that sets both
        // (`bound_select_statement_impl.cpp:256-257`).
        let delta_both = bm25_param("bm25_param_delta");
        let delta_term = bm25_param("bm25_param_delta_term") + delta_both;
        let delta_phrase = bm25_param("bm25_param_delta_phrase") + delta_both;
        // `operator=and` requires every query term (`or` is the default)
        let require_all = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("operator")
                        && v.trim().eq_ignore_ascii_case("and")
                })
                .unwrap_or(false)
        });
        let query = arg_str(m.args.first()).unwrap_or_default();
        // A query ending in a boolean keyword (`... OR NOT`) is incomplete.
        let ends_with_op = query
            .split_whitespace()
            .last()
            .map(|w| {
                w.eq_ignore_ascii_case("or")
                    || w.eq_ignore_ascii_case("and")
                    || w.eq_ignore_ascii_case("not")
            })
            .unwrap_or(false);
        // ... unless an explicit `operator=` supplies the default.
        let has_operator_opt = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, _)| k.trim().eq_ignore_ascii_case("operator"))
                .unwrap_or(false)
        });
        if ends_with_op && !has_operator_opt {
            return Err(Error::Invalid(
                "parser: incomplete boolean expression in the full-text query".into(),
            ));
        }
        let cols: Vec<usize> = match_columns(&m.column)
            .iter()
            .filter_map(|c| schema.iter().position(|s| s == c))
            .collect();
        if cols.is_empty() {
            // A column-less clause may still name fields inside the query.
            if let Some(groups) = parse_field_query(&query) {
                return self.search_field_query(table, &groups, &query, pred, &params, hidden);
            }
            // ...and otherwise falls back to `default_field=<col>`, which is
            // what the SDK's `match_text("", q, n, {"default_field": "body"})`
            // sends: an empty field list means "the default field", not "no
            // field". Without this the plain keyword below has nothing to be
            // searched in and the statement silently returned no rows.
            let fallback = super::state::search_option(&params, "default_field");
            match fallback.and_then(|name| schema.iter().position(|s| s == &name)) {
                Some(ci) => {
                    let name = schema[ci].clone();
                    // Re-dispatch with the field filled in, so the single-field
                    // path below does the tokenising, scoring and boosting.
                    // Carries this call's scan slot rather than taking another.
                    let mut m2 = m.clone();
                    m2.column = name;
                    return self.search_text_terms_with_slot(table, &m2, pred, hidden);
                }
                None => return Ok((Vec::new(), hidden.to_vec())),
            }
        }
        // `whitespace`/`keyword` split only on whitespace (so `2023-01-01` stays
        // one token); `whitespace-<chars>` splits **only** on the listed
        // delimiters (the C++ `WhitespaceAnalyzer` does not also split on
        // whitespace — see `whitespace_analyzer_impl.cpp`); a column indexed
        // with a real analyzer (`ik`, `rag-chinese`, `ngram`, …) tokenises with
        // that analyzer; anything else uses the standard tokenizer.
        let tokenizer: TextTokenizer = match_columns(&m.column)
            .iter()
            .map(|c| self.index_tokenizer(table, c))
            .find(|tok| tok.delims.is_some() || tok.analyzer.is_some())
            .unwrap_or_default();
        // One unit per quoted phrase / bare word. The analyzer decides what a
        // `"a b"` group tokenises to, so quotes are stripped first.
        let units: Vec<(bool, Vec<String>, f64, u32)> = query_units(&query)
            .into_iter()
            .map(|(phrase, text, boost, slop)| {
                (phrase, tokenize_text(&text, &tokenizer), boost, slop)
            })
            .filter(|(_, toks, _, _)| !toks.is_empty())
            .collect();
        // One entry per matched column: `(schema index, field boost, tokeniser)`.
        // `similarity=boolean` scores each field on its own, so a `keyword`
        // column next to a `standard` one must not share the other's analyzer.
        let field_specs: Vec<(usize, f64, TextTokenizer)> = m
            .column
            .split(',')
            .filter_map(|spec| {
                let name = spec
                    .split('^')
                    .next()
                    .unwrap_or(spec)
                    .split('@')
                    .next()
                    .unwrap_or(spec)
                    .trim();
                let boost = spec
                    .split('^')
                    .nth(1)
                    .and_then(|v| v.trim().parse::<f64>().ok())
                    .unwrap_or(1.0);
                let ci = schema.iter().position(|s| s == name)?;
                Some((ci, boost, self.index_tokenizer(table, name)))
            })
            .collect();
        let terms: Vec<String> = units
            .iter()
            .flat_map(|(_, u, _, _)| u.iter().cloned())
            .filter(|t| t != "or" && t != "and")
            .collect();
        if terms.is_empty() {
            return Ok((Vec::new(), hidden.to_vec()));
        }
        let min_should = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("minimum_should_match"))
            .map(|(_, v)| minimum_should_match(v.trim(), terms.len()))
            .unwrap_or(1);
        // With `operator=and`, each OR-separated group is a clause: a run of
        // several words must appear as a phrase (iresearch's `AndQuery`).
        let min_should = if require_all { units.len() } else { min_should };
        let topn = parse_topn_param(&params).or(topn_positional).unwrap_or(10);
        // `block_max=…`: the reference's `EarlyTermAlgo` switch
        // (`bound_select_statement_impl.cpp:168-183`). The port has one block-max
        // iterator, so every accepted value selects it except the no-pruning
        // ones (`false` / `batch`), which score every candidate. `compare` is the
        // reference's diagnostic three-way run; here it is the normal path.
        let naive_scan = match params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("block_max"))
            .map(|(_, v)| v.trim().to_ascii_lowercase())
            .as_deref()
        {
            None | Some("") | Some("auto") | Some("heap") | Some("heap_bmw") | Some("true")
            | Some("bmw") | Some("compare") => false,
            Some("false") | Some("batch") => true,
            Some(other) => {
                return Err(Error::Syntax(format!(
                    "block_max option must be empty, auto, heap_bmw, bmw, true, batch, false, or compare, got '{other}'"
                )))
            }
        };
        let in_filter = match m.filter.as_ref() {
            Some(p) => Some(planner::predicate_expr(p, &schema)?),
            None => None,
        };
        // `filter=…` in the MATCH TEXT options narrows the candidates.
        let opt_filter = match params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("filter"))
            .map(|(_, v)| v.trim().to_string())
            .filter(|v| !v.is_empty())
        {
            Some(f) => Some(planner::predicate_expr(
                &parse_filter_predicate(&f)?,
                &schema,
            )?),
            None => None,
        };
        let boolean = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("similarity")
                        && v.trim().eq_ignore_ascii_case("boolean")
                })
                .unwrap_or(false)
        });
        // The query-string grammar (`a AND b`, `a OR b`, `a AND NOT b`,
        // `(a OR b) AND c`, `column:term`, `"a b"~2`): build the reference's
        // query tree and evaluate it (`QueryBuilder::CreateSearch`). A plain
        // term list keeps the block-max WAND path below. The branch sits after the
        // option parsing so the predicate, `filter=` and `similarity` are known.
        // An explicit `operator=` selects the **term-list** grammar: the
        // reference's `FulltextQueryOperatorOption` is `kAnd`/`kOr` there and
        // `kInfinitySyntax` only when the option is absent
        // (`bound_select_statement_impl.cpp:200-210`), so the query string is
        // analysed into terms instead of being parsed. `TO BE OR NOT` with
        // `operator=and` is four ANDed terms, not a boolean expression.
        if !has_operator_opt && super::text_query::has_query_syntax(&query) {
            // The tree path scores each leaf on its own: it has no BooleanWeight,
            // batch-OR or rank-feature channel. Options only the WAND path
            // implements are rejected instead of being silently dropped -
            // answering a different question than the one asked is worse than an
            // error.
            let given = |name: &str| -> bool {
                params
                    .split(';')
                    .filter_map(|p| p.split_once('='))
                    .any(|(k, _)| k.trim().eq_ignore_ascii_case(name))
            };
            let unsupported = |name: &str| -> Error {
                Error::Invalid(format!(
                    "{name} is not supported with the query-string grammar \
                     (a AND b / a OR b / column:term)"
                ))
            };
            if boolean {
                return Err(unsupported("similarity=boolean"));
            }
            // `block_max` is not in this set: it only picks an early-termination
            // algorithm (`EarlyTermAlgo`, `bound_select_statement_impl.cpp:169-190`)
            // and every value yields the same result set. The tree path scores
            // every candidate, which is `batch`/`false`; accepting the option
            // keeps `compare` and the BMW spellings usable on a parsed query.
            for name in ["minimum_should_match", "rank_features"] {
                if given(name) {
                    return Err(unsupported(name));
                }
            }
            // The predicate / `filter=` / `IN (…)` narrow the candidates before
            // the tree ranks and truncates, exactly like the WAND path.
            let dead = self.tombstone_set(table)?;
            let allowed =
                self.allowed_rows(table, pred, in_filter.as_ref(), opt_filter.as_ref(), &dead)?;
            if let Some(hits) = self.search_text_query_tree(
                table,
                &m.column,
                &query,
                topn,
                delta_term,
                delta_phrase,
                threshold,
                (bm25_k1, bm25_b),
                allowed.as_ref(),
            )? {
                let wanted: Vec<u64> = hits.iter().map(|(seq, _)| *seq).collect();
                let mut by_id: HashMap<usize, Vec<Value>> = self
                    .fetch_some_rows_with_ids(table, &wanted)?
                    .into_iter()
                    .collect();
                let mut out = Vec::new();
                for (seq, score) in hits {
                    let Some(mut row) = by_id.remove(&(seq as usize)) else {
                        continue;
                    };
                    row.push(Value::Int(seq as i64));
                    row.push(Value::Float(score));
                    row.push(Value::Float(0.0));
                    row.push(Value::Float(score));
                    out.push(row);
                }
                // An empty tree result is the answer: falling through to the
                // plain-term path would answer a different question.
                return Ok((out, hidden.to_vec()));
            }
        }
        // `rank_features=<col>^<tag>^<weight>,…`: each document's `<col>` value is
        // a JSON array of `{"<tag>": <number>}` objects (its index uses the
        // `rankfeatures` analyzer), and the score gains `number * weight` for
        // every listed tag.
        let rank_features: Vec<(usize, Vec<(String, f64)>)> = {
            let spec = params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case("rank_features"))
                .map(|(_, v)| v.to_string());
            let mut groups: Vec<(usize, Vec<(String, f64)>)> = Vec::new();
            if let Some(spec) = spec {
                for item in spec.split(',') {
                    let parts: Vec<&str> = item.split('^').map(str::trim).collect();
                    let (Some(col), Some(tag)) = (parts.first(), parts.get(1)) else {
                        continue;
                    };
                    // The scoring column needs a `rankfeatures` full-text index.
                    let tkey = self.table_key(table);
                    let analyzer = self
                        .tables
                        .lock()
                        .unwrap()
                        .get(&tkey)
                        .and_then(|e| e.analyzers.get(*col).cloned())
                        .unwrap_or_default();
                    if !analyzer.eq_ignore_ascii_case("rankfeatures") {
                        if std::env::var_os("INF_DEBUG_SQL").is_some() {
                            eprintln!("[RANKF] table={table} col={col} analyzer={analyzer:?}");
                        }
                        return Err(Error::Invalid(format!(
                            "rank_features requires a rankfeatures index on '{col}'"
                        )));
                    }
                    let Some(ci) = schema.iter().position(|c| c == col) else {
                        continue;
                    };
                    let weight = parts.get(2).and_then(|w| w.parse().ok()).unwrap_or(1.0);
                    match groups.iter_mut().find(|(c, _)| *c == ci) {
                        Some((_, list)) => list.push(((*tag).to_string(), weight)),
                        None => groups.push((ci, vec![((*tag).to_string(), weight)])),
                    }
                }
            }
            groups
        };
        // Block-max WAND: an OR query over one or more fields never needs a
        // row's token vectors — the posting lists alone answer it. Filters
        // become a dead set, and `minimum_should_match` / `delta_term` /
        // boolean similarity are applied inside the scoring loop. BM25 scores
        // each field separately and sums, like `BM25Ranker`.
        if rank_features.is_empty()
            && !tokenizer.keyword()
            && !field_specs.is_empty()
            && units.iter().all(|(_, toks, _, _)| !toks.is_empty())
        {
            // One lazy posting stream per (field, term): nothing is materialised,
            // so the WAND cursors pay only for the documents they visit. The
            // guards stay alive for the whole scan because the streams borrow
            // the indexes.
            let mut indexes = Vec::with_capacity(field_specs.len());
            for (ci, boost, d) in &field_specs {
                let name = schema[*ci].clone();
                indexes.push((*boost, self.text_index_for(table, &name, d.clone())?));
            }
            // The guards borrow the arcs, so both stay alive for the scan.
            let mut held = Vec::with_capacity(indexes.len());
            for (boost, index) in &indexes {
                held.push((*boost, index.read().unwrap()));
            }
            let mut fields: Vec<WandField<'_>> = Vec::with_capacity(held.len());
            for (boost, guard) in &held {
                let text: &TextIndex = guard;
                let n = text.total_docs().max(1) as f64;
                let idf_of = |term: &str| {
                    let df = text.term_df(term).max(1) as f64;
                    (1.0 + (n - df + 0.5) / (df + 0.5)).ln()
                };
                let mut wand_units: Vec<WandUnit<'_>> = Vec::with_capacity(units.len());
                for (phrase, toks, uboost, slop) in &units {
                    // Boolean similarity ignores per-term boosts (one point per
                    // matched unit, weighted by the field boost).
                    let weight = if boolean { *boost } else { boost * uboost };
                    if boolean {
                        // `similarity=boolean` counts matched *terms*, not units:
                        // a phrase contributes one cursor per term, and the
                        // adjacency requirement does not apply.
                        for term in toks {
                            let cursor = text.term_stream(term);
                            if cursor.df() == 0 {
                                continue;
                            }
                            wand_units.push(WandUnit {
                                stream: WandStream::Term(cursor),
                                weight: *boost,
                                idf: 0.0,
                                delta: None,
                            });
                        }
                        continue;
                    }
                    if *phrase && toks.len() > 1 {
                        // One cursor per phrase: it lands only on documents
                        // where the terms are consecutive, and its block bound
                        // is the minimum over the terms' (BlockMaxPhraseScore).
                        let children: Vec<TermStream<'_>> = toks
                            .iter()
                            .map(|t| text.term_stream_capture(t, true))
                            .collect();
                        let cursor = PhraseCursor::new(children, *slop);
                        let idf: f64 = toks.iter().map(|t| idf_of(t)).sum();
                        wand_units.push(WandUnit {
                            stream: WandStream::Phrase(cursor),
                            weight,
                            idf,
                            delta: Some(delta_phrase),
                        });
                        continue;
                    }
                    let term = toks[0].clone();
                    let cursor = text.term_stream(&term);
                    if cursor.df() == 0 {
                        continue;
                    }
                    wand_units.push(WandUnit {
                        stream: WandStream::Term(cursor),
                        weight,
                        idf: idf_of(&term),
                        delta: None,
                    });
                }
                fields.push(WandField {
                    n_docs: text.total_docs(),
                    total_len: text.corpus_len(),
                    units: wand_units,
                });
            }
            // Tombstones still feed the corpus statistics (as the reference's
            // segments do until compaction) but never rank.
            let dead = self.tombstone_set(table)?;
            // A filter removes documents before ranking. Its answer is a row list
            // the iterator can seek with (`DocFilter`), which is what turns a
            // rejected candidate from "decode the block bound, score, drop" into
            // "hop every cursor past it".
            let filter =
                self.allowed_rows(table, pred, in_filter.as_ref(), opt_filter.as_ref(), &dead)?;
            let mode = if boolean {
                wand::WandMode::Boolean
            } else {
                wand::WandMode::Bm25
            };
            let mut iterator = wand::BlockMaxWandIterator::new(
                fields,
                topn.max(1),
                &dead,
                delta_term,
                min_should,
                mode,
            );
            if let Some(filter) = filter.as_ref() {
                iterator = iterator.with_filter(filter);
            }
            if bm25_k1 != 1.2 || bm25_b != 0.75 {
                iterator = iterator.with_bm25_params(bm25_k1, bm25_b);
            }
            // `threshold=` raises the pruning bar, like the reference's
            // `UpdateScoreThreshold` (`blockmax_wand_iterator_impl.cpp:233`).
            if let Some(t) = threshold {
                iterator = iterator.with_threshold(t);
            }
            if naive_scan {
                iterator = iterator.with_naive();
            }
            let hits = iterator.run();
            let scored: Vec<(usize, f64)> = hits
                .into_iter()
                .map(|(seq, score)| (seq as usize, score))
                .filter(|(_, score)| threshold.map(|t| *score > t).unwrap_or(true))
                .collect();
            // For plain OR terms this path is the whole answer: an empty result
            // is empty, and falling through would redo the same work on the
            // generic posting path.
            if scored.is_empty() {
                return Ok((Vec::new(), hidden.to_vec()));
            }
            {
                let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq as u64).collect();
                let mut by_id: HashMap<usize, Vec<Value>> = self
                    .fetch_some_rows_with_ids(table, &wanted)?
                    .into_iter()
                    .collect();
                let mut out = Vec::new();
                for (seq, score) in scored {
                    let Some(mut row) = by_id.remove(&seq) else {
                        continue;
                    };
                    row.push(Value::Int(seq as i64));
                    row.push(Value::Float(score));
                    row.push(Value::Float(0.0));
                    row.push(Value::Float(score));
                    out.push(row);
                }
                // An empty tree result is the answer: falling through to the
                // plain-term path would answer a different question.
                return Ok((out, hidden.to_vec()));
            }
        }
        // Everything below runs off the inverted index. The postings give the
        // candidate documents, each document's term frequency per field, the
        // corpus statistics (`n_docs`, average field length, df) and - through
        // the position lists - the phrase frequencies, so a multi-word or
        // phrase query never re-tokenises a row. A row is read only when the
        // query needs a value from it: a filter, or `rank_features`.
        let query_terms: Vec<String> = {
            let mut out: Vec<String> = Vec::new();
            for (_, unit, _, _) in &units {
                for term in unit {
                    if !out.contains(term) {
                        out.push(term.clone());
                    }
                }
            }
            out
        };
        // Streaming scoring: one merge cursor per (field, query term). A
        // cursor walks its term's postings in document order and serves the
        // positions of the document it is parked on, so neither the posting
        // lists nor a per-document position map is ever materialised.
        let mut indexes = Vec::with_capacity(field_specs.len());
        for (ci, boost, d) in &field_specs {
            let name = schema[*ci].clone();
            indexes.push((*boost, self.text_index_for(table, &name, d.clone())?));
        }
        let mut held = Vec::with_capacity(indexes.len());
        for (boost, index) in &indexes {
            held.push((*boost, index.read().unwrap()));
        }
        let mut field_texts: Vec<&TextIndex> = Vec::with_capacity(held.len());
        let mut field_cursors: Vec<Vec<MergeCursor<'_>>> = Vec::with_capacity(held.len());
        let mut field_stats: Vec<(usize, f64)> = Vec::with_capacity(held.len());
        let mut dfs: Vec<Vec<usize>> = Vec::with_capacity(held.len());
        for (_, guard) in &held {
            let text: &TextIndex = guard;
            let cursors: Vec<MergeCursor<'_>> =
                query_terms.iter().map(|t| text.term_cursor(t)).collect();
            let n = text.total_docs();
            let avg = if n == 0 {
                1.0
            } else {
                let a = text.corpus_len() as f64 / n as f64;
                if a <= 0.0 {
                    1.0
                } else {
                    a
                }
            };
            dfs.push(cursors.iter().map(|c| c.df()).collect());
            field_cursors.push(cursors);
            field_stats.push((n, avg));
            field_texts.push(text);
        }
        let n_docs = field_stats.first().map(|(n, _)| *n).unwrap_or(1).max(1);
        let avg_lens: Vec<f64> = field_stats.iter().map(|(_, a)| *a).collect();
        // A `keyword`/`whitespace` field scores constantly: one field-boost
        // point per matching document (not per query term).
        let keyword_field = tokenizer.keyword();
        // Deleted rows keep contributing to the corpus statistics (as the
        // reference's segments do until compaction) but never rank.
        let dead = self.tombstone_set(table)?;
        // The filters are evaluated once, into the row list the cursors seek
        // with; only `rank_features` still needs a row per candidate.
        let filter =
            self.allowed_rows(table, pred, in_filter.as_ref(), opt_filter.as_ref(), &dead)?;
        // `rank_features`: each listed tag's weight is the posting's doc-payload
        // column, written by the `rankfeatures` analyzer as
        // `SmallFloat::Float122ToUInt16`. Reading it from the index is what
        // `RankFeatureDocIterator` does; it keeps the reference's number (and
        // saves a row fetch plus a JSON parse per candidate).
        #[allow(clippy::type_complexity)]
        let mut rank_payloads: Vec<(
            usize,
            Vec<(String, f64, std::collections::BTreeMap<u64, u16>)>,
        )> = Vec::with_capacity(rank_features.len());
        if !rank_features.is_empty() {
            if let Some((_, _, decoded)) = field_specs.first() {
                for (ci, tags) in &rank_features {
                    let name = schema[*ci].clone();
                    let index = self.text_index_for(table, &name, decoded.clone())?;
                    let guard = index.read().unwrap();
                    let per_tag = tags
                        .iter()
                        .map(|(tag, weight)| (tag.clone(), *weight, guard.doc_payloads(tag)))
                        .collect();
                    rank_payloads.push((*ci, per_tag));
                }
            }
        }

        let mut scored: Vec<(usize, f64)> = Vec::new();
        // Reused per document: each phrase unit's terms' positions.
        let mut phrase_positions: Vec<Vec<u32>> = Vec::new();
        loop {
            let mut doc: Option<u64> = None;
            for field in field_cursors.iter_mut() {
                for cursor in field.iter_mut() {
                    if let Some(d) = cursor.doc() {
                        doc = Some(doc.map_or(d, |best: u64| best.min(d)));
                    }
                }
            }
            let Some(doc) = doc else { break };
            // A row the filter rejects can never be scored: push every cursor
            // past it instead of fetching and evaluating it one at a time.
            if let Some(filter) = filter.as_ref() {
                if !filter.contains(doc) {
                    let Some(next) = filter.next_ge(doc + 1) else {
                        break;
                    };
                    for field in field_cursors.iter_mut() {
                        for cursor in field.iter_mut() {
                            if cursor.doc().is_some_and(|d| d < next) {
                                cursor.advance(next);
                            }
                        }
                    }
                    continue;
                }
            }
            if !dead.contains(&doc) {
                // `rank_features=<col>^<tag>^<weight>` adds `weight_of_tag * weight`,
                // where `weight_of_tag` is the posting's doc payload decoded with
                // `SmallFloat::UInt16ToFloat122`.
                let rank_bonus: f64 = rank_payloads
                    .iter()
                    .map(|(_, per_tag)| {
                        per_tag
                            .iter()
                            .map(|(_, weight, rows)| {
                                rows.get(&doc)
                                    .map(|payload| {
                                        f64::from(analyzer::uint16_to_float122(*payload)) * weight
                                    })
                                    .unwrap_or(0.0)
                            })
                            .sum::<f64>()
                    })
                    .sum();
                // `similarity=boolean` scores each field on its own: a
                // `keyword`/`whitespace` field contributes its boost once, a
                // standard field one boost per matching term.
                if boolean {
                    let (mut bhits, mut bscore) = (0usize, 0.0f64);
                    for (f, (_, boost, d)) in field_specs.iter().enumerate() {
                        let mut matched = 0usize;
                        for cursor in field_cursors[f].iter_mut() {
                            if cursor.doc() == Some(doc) {
                                matched += 1;
                            }
                        }
                        if matched > 0 {
                            bhits += matched;
                            bscore += boost * if d.keyword() { 1.0 } else { matched as f64 };
                        }
                    }
                    let bscore = bscore + rank_bonus;
                    if bhits >= min_should
                        && bhits > 0
                        && threshold.map(|t| bscore > t).unwrap_or(true)
                    {
                        scored.push((doc as usize, bscore));
                    }
                } else {
                    let mut score = 0.0f64;
                    // Units matched in *any* field, for `minimum_should_match`.
                    let mut unit_hits = vec![false; units.len()];
                    for (f, (_, boost, _)) in field_specs.iter().enumerate() {
                        let len = field_texts[f].doc_len_opt(doc).unwrap_or(0) as f64;
                        let avg_len = avg_lens[f];
                        let idf_of = |ti: usize| -> f64 {
                            let df = dfs[f].get(ti).copied().unwrap_or(0).max(1) as f64;
                            (1.0 + (n_docs as f64 - df + 0.5) / (df + 0.5)).ln()
                        };
                        let cursors = &mut field_cursors[f];
                        let tf_of =
                            |cursors: &mut Vec<MergeCursor<'_>>, ti: usize| -> Option<u32> {
                                let c = cursors.get_mut(ti)?;
                                (c.doc() == Some(doc)).then(|| c.tf())
                            };
                        if keyword_field {
                            let mut any = false;
                            for (ui, (_, unit, _, _)) in units.iter().enumerate() {
                                let hit = unit.iter().any(|t| {
                                    query_terms
                                        .iter()
                                        .position(|q| q == t)
                                        .is_some_and(|ti| tf_of(cursors, ti).is_some())
                                });
                                if hit {
                                    unit_hits[ui] = true;
                                    any = true;
                                }
                            }
                            if any {
                                score += boost;
                            }
                            continue;
                        }
                        for (ui, (phrase, unit, uboost, slop)) in units.iter().enumerate() {
                            let uboost = *uboost;
                            if (*phrase || require_all) && unit.len() > 1 {
                                // A phrase scores like one term whose `tf` is the phrase
                                // frequency and whose `idf` is the sum over its terms
                                // (Lucene `PhraseWeight`); the frequency comes from the
                                // index's position lists, not from the row's text.
                                if phrase_positions.len() < unit.len() {
                                    phrase_positions.resize(unit.len(), Vec::new());
                                }
                                for (k, term) in unit.iter().enumerate() {
                                    phrase_positions[k].clear();
                                    let Some(ti) = query_terms.iter().position(|q| q == term)
                                    else {
                                        continue;
                                    };
                                    let Some(cursor) = cursors.get_mut(ti) else {
                                        continue;
                                    };
                                    if cursor.doc() == Some(doc) {
                                        phrase_positions[k].extend_from_slice(cursor.positions());
                                    }
                                }
                                // `"a b"~N`: a sloppy phrase scores with the
                                // fractional frequency the reference's
                                // `GetSloppyPhraseMatchData` produces.
                                let freq = if *slop == 0 {
                                    phrase_freq_lists(&phrase_positions[..unit.len()], unit.len())
                                        as f64
                                } else {
                                    phrase_freq_lists_sloppy(
                                        &phrase_positions[..unit.len()],
                                        unit.len(),
                                        *slop,
                                    )
                                };
                                if freq <= 0.0 {
                                    continue;
                                }
                                unit_hits[ui] = true;
                                let idf_sum: f64 = unit
                                    .iter()
                                    .map(|t| {
                                        let ti = query_terms
                                            .iter()
                                            .position(|q| q == t)
                                            .unwrap_or(usize::MAX);
                                        if ti == usize::MAX {
                                            0.0
                                        } else {
                                            idf_of(ti)
                                        }
                                    })
                                    .sum();
                                let tf = freq;
                                let norm = (bm25_k1 + 1.0) * tf
                                    / (tf + bm25_k1 * (1.0 - bm25_b + bm25_b * len / avg_len));
                                score += idf_sum * (norm + delta_phrase) * boost * uboost;
                                continue;
                            }
                            for term in unit {
                                let Some(ti) = query_terms.iter().position(|q| q == term) else {
                                    continue;
                                };
                                let Some(tf) = tf_of(cursors, ti) else {
                                    continue;
                                };
                                unit_hits[ui] = true;
                                let idf = idf_of(ti);
                                let tf = tf as f64;
                                let norm = (bm25_k1 + 1.0) * tf
                                    / (tf + bm25_k1 * (1.0 - bm25_b + bm25_b * len / avg_len));
                                score += idf * (norm + delta_term) * boost * uboost;
                            }
                        }
                    }
                    let hits: usize = if require_all {
                        // one count per required clause
                        unit_hits.iter().filter(|m| **m).count()
                    } else {
                        units
                            .iter()
                            .zip(unit_hits.iter())
                            .filter(|(_, m)| **m)
                            .map(|((_, u, _, _), _)| u.len())
                            .sum()
                    };
                    score += rank_bonus;
                    if hits >= min_should
                        && hits > 0
                        && threshold.map(|t| score > t).unwrap_or(true)
                    {
                        scored.push((doc as usize, score));
                    }
                }
            }
            // Every cursor parked on this document moves past it.
            for field in field_cursors.iter_mut() {
                for cursor in field.iter_mut() {
                    if cursor.doc() == Some(doc) {
                        cursor.advance(doc + 1);
                    }
                }
            }
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq as u64).collect();
        let mut by_id: HashMap<usize, Vec<Value>> = self
            .fetch_some_rows_with_ids(table, &wanted)?
            .into_iter()
            .collect();
        let mut out = Vec::new();
        for (seq, score) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(score));
            row.push(Value::Float(0.0));
            row.push(Value::Float(score));
            out.push(row);
        }
        Ok((out, hidden.to_vec()))
    }

    /// Scores a column-less `MATCH TEXT ('', '(doc:…)^w …')` clause. With
    /// `similarity=boolean` each group multiplies its boost by the sum of its
    /// fields' boolean scores (a `keyword`/`whitespace` field counts once, a
    /// standard field once per matching term); otherwise the query is parsed
    /// into a normal query tree and scored with BM25, like a named column.
    #[allow(clippy::type_complexity)]
    /// Evaluates a query-string query (`a AND b`, `a OR b`, `a AND NOT b`,
    /// `(a OR b) AND c`, `column:term`, `"a b"~2`) through the reference's
    /// `QueryNode` tree (`query_builder_impl.cpp`).
    ///
    /// `Ok(None)` when the text does not build a tree (a `:` that is not a
    /// field qualifier, an empty analysis), in which case the caller keeps its
    /// plain-term fast path.
    #[allow(clippy::too_many_arguments)]
    fn search_text_query_tree(
        &self,
        table: &str,
        columns: &str,
        query: &str,
        topn: usize,
        delta_term: f64,
        delta_phrase: f64,
        threshold: Option<f64>,
        bm25: (f64, f64),
        // Rows the predicate / `filter=` / `IN (…)` admit, applied before the
        // tree ranks and truncates - the WAND path's rule.
        allowed: Option<&crate::engine::wand::DocFilter>,
    ) -> Result<Option<Vec<(u64, f64)>>> {
        use crate::invertedindex::search::query_builder::SearchDriver;
        let tkey = self.table_key(table);
        // Load or refresh every field's index *before* taking the `tables` guard:
        // opening one may seal the tail or rebuild it from rows, and
        // `text_index_for` takes the table guard itself. This also makes sure the
        // index covers the published row count before the tree is built.
        for spec in columns.split(',') {
            let name = spec
                .split('^')
                .next()
                .unwrap_or(spec)
                .split('@')
                .next()
                .unwrap_or(spec)
                .trim();
            if name.is_empty() {
                continue;
            }
            let tokenizer = self.index_tokenizer(table, name);
            let _ = self.text_index_for(table, name, tokenizer)?;
        }
        // The registry is keyed by index id; the query names columns, so build
        // the `column -> shared index` view once from the catalog.
        let (indexes, analyzers) = {
            let by_id: std::collections::HashMap<u64, String> = self
                .storage
                .catalog
                .indexes_of_table(&self.qualified_table(table))
                .unwrap_or_default()
                .into_iter()
                .map(|im| (im.id, im.column))
                .collect();
            let tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get(&tkey) else {
                return Ok(None);
            };
            let by_column: HashMap<String, super::state::SharedTextIndex> = entry
                .text
                .iter()
                .filter_map(|(id, v)| Some((by_id.get(id)?.clone(), v.clone())))
                .collect();
            (by_column, entry.analyzers.clone())
        };
        if indexes.is_empty() {
            return Ok(None);
        }
        let mut field2analyzer: HashMap<String, HashMap<String, String>> = HashMap::new();
        let mut lookup: HashMap<String, super::state::SharedTextIndex> = HashMap::new();
        let mut default_field: Option<String> = None;
        for spec in columns.split(',') {
            let name = spec
                .split('^')
                .next()
                .unwrap_or(spec)
                .split('@')
                .next()
                .unwrap_or(spec)
                .trim();
            let Some(shared) = indexes.get(name) else {
                continue;
            };
            // `SearchDriver::analyzer_name` reads the first entry of the inner
            // map when the field carries no `index` suffix, which is how the
            // reference looks a column up.
            let mut per_index: HashMap<String, String> = HashMap::new();
            per_index.insert(
                String::new(),
                analyzers.get(name).cloned().unwrap_or_default(),
            );
            field2analyzer.insert(name.to_string(), per_index);
            lookup.insert(name.to_string(), shared.clone());
            default_field.get_or_insert_with(|| name.to_string());
        }
        let Some(default_field) = default_field else {
            return Ok(None);
        };
        let driver = SearchDriver::new(field2analyzer, default_field);
        let Ok(tree) = driver.parse_single_with_fields(columns, query) else {
            // A `column:term` naming a column outside `columns` is not "no
            // syntax": falling back would search `title:foo` as ordinary text and
            // answer a different question. The reference resolves field names
            // against every index of the table, so such a leaf finds no postings -
            // match nothing instead of guessing.
            let outside = query_field_qualifiers(query)
                .into_iter()
                .any(|field| !columns.split(',').any(|spec| column_of(spec) == field));
            if outside {
                return Ok(Some(Vec::new()));
            }
            return Ok(None);
        };
        let dead = self.tombstone_set(table)?;
        let Some(hits) = super::text_query::run_query_tree(
            tree,
            lookup,
            topn,
            &dead,
            allowed,
            threshold,
            delta_term,
            delta_phrase,
            bm25,
        ) else {
            return Ok(None);
        };
        // `run_query_tree` already dropped the rows below `threshold`, before its
        // sort and `topn` cut; filtering again here would only re-check them.
        Ok(Some(hits))
    }

    pub(crate) fn search_field_query(
        &self,
        table: &str,
        groups: &[(f64, Vec<(String, String)>)],
        query: &str,
        pred: Option<&Expr>,
        params: &str,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let boolean = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("similarity")
                        && v.trim().eq_ignore_ascii_case("boolean")
                })
                .unwrap_or(false)
        });
        if !boolean {
            return self.search_field_query_bm25(table, query, pred, params, hidden);
        }
        let schema = self.table_schema(table)?;
        let mut fields: Vec<(String, usize, TextTokenizer)> = Vec::new();
        for (_, terms) in groups {
            for (f, _) in terms {
                if f.is_empty() || fields.iter().any(|(n, _, _)| n == f) {
                    continue;
                }
                if let Some(ci) = schema.iter().position(|s| s == f) {
                    let d = self.index_tokenizer(table, f);
                    fields.push((f.clone(), ci, d));
                }
            }
        }
        // `minimum_should_match` counts *matched clauses*, where a quoted phrase
        // (`doc:"a b"`) is one clause that matches only when all of its tokens
        // are present.
        let units_total: usize = groups.iter().map(|(_, terms)| terms.len()).sum();
        let min_should = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("minimum_should_match"))
            .map(|(_, v)| minimum_should_match(v.trim(), units_total))
            .unwrap_or(1);
        let topn = parse_topn_param(params).unwrap_or(10);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let stored_types = self.column_types(table).unwrap_or_default();
        // The index decides which documents can hold one of the group's terms;
        // only those rows are read and tokenised. This syntax has no negation
        // (`parse_field_query` keeps a `-term` as a literal term), so the union
        // of the leaves' postings cannot drop a matching document.
        let mut candidates: std::collections::BTreeSet<u64> = Default::default();
        for (_, terms) in groups {
            for (name, text) in terms {
                let Some((field_name, _, d)) =
                    fields.iter().find(|(n, _, _)| n == name || name.is_empty())
                else {
                    continue;
                };
                let toks = tokenize_index_text(text, d);
                if toks.is_empty() {
                    continue;
                }
                let index = self.text_index_for(table, field_name, d.clone())?;
                let guard = index.read().unwrap();
                let merged = guard.merge(&toks);
                for list in merged.postings.values() {
                    candidates.extend(list.iter().map(|(doc, _)| *doc));
                }
            }
        }
        let mut scored: Vec<(usize, f64)> = Vec::new();
        let candidate_ids: Vec<u64> = candidates.into_iter().collect();
        // One cursor over the blocks: a candidate the index still lists but the
        // table no longer shows (a tombstoned row) simply has no text.
        for (seq, v) in self.row_texts(table, &candidate_ids)? {
            let mut row = self.parse_stored_row(table, &defaults, &stored_types, &v)?;
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            if let Some(p) = pred {
                if !p.eval_predicate(&row) {
                    continue;
                }
            }
            #[cfg(test)]
            MATCH_ROWS_TOKENIZED.with(|n| n.set(n.get() + 1));
            let tokens: Vec<std::collections::HashSet<String>> = fields
                .iter()
                .map(|(_, ci, d)| {
                    let text = match row.get(*ci) {
                        Some(Value::Str(s)) => s.clone(),
                        Some(o) => o.to_slt_string(),
                        None => String::new(),
                    };
                    tokenize_index_text(&text, d).into_iter().collect()
                })
                .collect();
            let mut score = 0.0f64;
            // `units_matched` drives `minimum_should_match`: a phrase clause
            // counts only when every token it expands to is present.
            let mut units_matched = 0usize;
            for (boost, terms) in groups {
                let mut gscore = 0.0f64;
                for (f, t) in terms {
                    let Some(fi) = fields
                        .iter()
                        .position(|(name, _, _)| f == name || f.is_empty())
                    else {
                        continue;
                    };
                    let d = &fields[fi].2;
                    let sub = tokenize_text(t, d);
                    if !sub.is_empty() && sub.iter().all(|x| tokens[fi].contains(x)) {
                        units_matched += 1;
                    }
                }
                for (fi, (name, _, d)) in fields.iter().enumerate() {
                    let kw = d.keyword();
                    let mut matched = 0usize;
                    for (f, t) in terms {
                        if f == name || f.is_empty() {
                            // A quoted phrase is split by the field's own
                            // analyzer (one token for `keyword`).
                            matched += tokenize_text(t, d)
                                .iter()
                                .filter(|x| tokens[fi].contains(*x))
                                .count();
                        }
                    }
                    if matched > 0 {
                        gscore += if kw { 1.0 } else { matched as f64 };
                    }
                }
                score += boost * gscore;
            }
            if units_matched >= min_should && units_matched > 0 {
                scored.push((seq, score));
            }
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        let mut by_id: HashMap<usize, Vec<Value>> =
            self.fetch_rows_with_ids(table)?.into_iter().collect();
        let mut out = Vec::new();
        for (seq, score) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(score));
            row.push(Value::Float(0.0));
            row.push(Value::Float(score));
            out.push(row);
        }
        Ok((out, hidden.to_vec()))
    }

    /// BM25 path for a column-less field-qualified query. The query string is
    /// parsed by the shared [`SearchDriver`] into the same query tree the
    /// reference builds, then every leaf is scored with the named-column
    /// formula: per-field corpus statistics (document count, sum of token
    /// counts, per-term document frequency) and per-document term frequency
    /// and length, `k1 = 1.2`, `b = 0.75`, and the leaf's `^boost` weight.
    #[allow(clippy::type_complexity)]
    pub(crate) fn search_field_query_bm25(
        &self,
        table: &str,
        query: &str,
        pred: Option<&Expr>,
        params: &str,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let schema = self.table_schema(table)?;
        // `column2analyzer`: the driver only needs the column's analyzer name.
        let field2analyzer: HashMap<String, HashMap<String, String>> = {
            let key = self.table_key(table);
            let guard = self.tables.lock().unwrap();
            guard
                .get(&key)
                .map(|entry| {
                    entry
                        .analyzers
                        .iter()
                        .map(|(column, analyzer)| {
                            (
                                column.clone(),
                                HashMap::from([(String::new(), analyzer.clone())]),
                            )
                        })
                        .collect()
                })
                .unwrap_or_default()
        };
        let default_field = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("default_field"))
            .map(|(_, v)| v.trim().to_string())
            .unwrap_or_default();
        let tree = SearchDriver::new(field2analyzer, default_field)
            .parse_single(query)
            .map_err(|error| Error::Invalid(format!("{error}")))?;
        let mut columns: Vec<String> = Vec::new();
        collect_query_columns(&tree, &mut columns);
        let mut fields: Vec<FieldGroupField> = columns
            .iter()
            .filter_map(|name| {
                Some(FieldGroupField {
                    ci: schema.iter().position(|s| s == name)?,
                    name: name.clone(),
                    tokenizer: self.index_tokenizer(table, name),
                    df: HashMap::new(),
                    avg_len: 1.0,
                })
            })
            .collect();
        let defaults = self.table_defaults(table).unwrap_or_default();
        // Corpus statistics come from the inverted index and the index also
        // says which documents can possibly match, so only those are tokenised.
        // A tree that negates (`NOT doc:x` selects documents without a term)
        // constrains nothing and keeps the full scan.
        let leaves = query_leaf_terms(&tree);
        let mut candidates: Option<std::collections::BTreeSet<u64>> = None;
        let mut n_docs = 0usize;
        for field in fields.iter_mut() {
            let terms: Vec<String> = leaves
                .as_ref()
                .map(|leaves| {
                    leaves
                        .iter()
                        .filter(|(column, _)| column == &field.name)
                        .map(|(_, term)| term.clone())
                        .collect()
                })
                .unwrap_or_default();
            let index = self.text_index_for(table, &field.name, field.tokenizer.clone())?;
            let guard = index.read().unwrap();
            let merged = guard.merge(&terms);
            n_docs = n_docs.max(merged.n_docs);
            field.avg_len = if merged.n_docs == 0 {
                1.0
            } else {
                let avg = merged.total_len as f64 / merged.n_docs as f64;
                if avg <= 0.0 {
                    1.0
                } else {
                    avg
                }
            };
            for term in &terms {
                field.df.insert(
                    term.clone(),
                    merged.postings.get(term).map(Vec::len).unwrap_or(0),
                );
            }
            if leaves.is_some() {
                let set = candidates.get_or_insert_with(Default::default);
                for list in merged.postings.values() {
                    set.extend(list.iter().map(|(doc, _)| *doc));
                }
            }
        }
        let n_docs = n_docs.max(1);
        // `(seq, live, passes the filter, per-field (term, position) pairs)`.
        // Deleted rows still reach the index (its statistics count them), but
        // they never rank.
        #[allow(clippy::type_complexity)]
        let mut docs: Vec<(usize, bool, bool, Vec<Vec<(String, u32)>>)> = Vec::new();
        // The corpus statistics count every row the segments ever held, so the
        // scan reads the blocks tombstoned slots included and marks which of
        // them are still live.
        let wanted: Option<std::collections::HashSet<u64>> =
            candidates.as_ref().map(|ids| ids.iter().copied().collect());
        let dead = self.tombstone_set(table)?;
        let scan: Vec<(usize, Vec<Value>)> = self
            .fetch_rows_including_deleted(table)?
            .into_iter()
            .filter(|(seq, _)| {
                wanted
                    .as_ref()
                    .map(|set| set.contains(&(*seq as u64)))
                    .unwrap_or(true)
            })
            .collect();
        for (seq, mut row) in scan {
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            let live = !dead.contains(&(seq as u64));
            let passes = pred.map(|p| p.eval_predicate(&row)).unwrap_or(true);
            #[cfg(test)]
            MATCH_ROWS_TOKENIZED.with(|n| n.set(n.get() + 1));
            let tokens: Vec<Vec<(String, u32)>> = fields
                .iter()
                .map(|field| {
                    let text = match row.get(field.ci) {
                        Some(Value::Str(s)) => s.clone(),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    };
                    tokenize_index_positions(&text, &field.tokenizer)
                })
                .collect();
            docs.push((seq, live, passes, tokens));
        }
        let bm25_param = |name: &str| -> f64 {
            params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case(name))
                .and_then(|(_, v)| v.trim().parse().ok())
                .unwrap_or(0.0)
        };
        let (bm25_k1, bm25_b) = parse_bm25_k1_b(params)?;
        // `bm25_param_delta` is shorthand for both, as in
        // `bound_select_statement_impl.cpp:256-257`.
        let delta_both = bm25_param("bm25_param_delta");
        let delta_term = bm25_param("bm25_param_delta_term") + delta_both;
        let delta_phrase = bm25_param("bm25_param_delta_phrase") + delta_both;
        let threshold: Option<f64> = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("threshold"))
            .and_then(|(_, v)| v.trim().parse().ok())
            // The reference's `threshold_ <= 0.0f` shortcut
            // (`term_doc_iterator_impl.cpp:118`): a non-positive threshold
            // filters nothing.
            .filter(|t| *t > 0.0);
        let total_terms = query_term_count(&tree);
        let min_should = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("minimum_should_match"))
            .map(|(_, v)| minimum_should_match(v.trim(), total_terms))
            .unwrap_or(1);
        let topn = parse_topn_param(params).unwrap_or(10);
        let mut scored: Vec<(usize, f64)> = Vec::new();
        for (seq, live, passes, tokens) in &docs {
            if !live || !passes {
                continue;
            }
            let Some((score, units)) = eval_field_query_node(
                &tree,
                &fields,
                tokens,
                n_docs,
                delta_term,
                delta_phrase,
                (bm25_k1, bm25_b),
            ) else {
                continue;
            };
            if units >= min_should && units > 0 && threshold.map(|t| score > t).unwrap_or(true) {
                scored.push((*seq, score));
            }
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq as u64).collect();
        let mut by_id: HashMap<usize, Vec<Value>> = self
            .fetch_some_rows_with_ids(table, &wanted)?
            .into_iter()
            .collect();
        let mut out = Vec::new();
        for (seq, score) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(score));
            row.push(Value::Float(0.0));
            row.push(Value::Float(score));
            out.push(row);
        }
        Ok((out, hidden.to_vec()))
    }

    /// Rejects `SUM`/`AVG` over a non-numeric column.
    /// A WHERE term used on its own (`WHERE c1`) must itself be boolean; the
    /// reference rejects a numeric column or an arithmetic expression there.
    pub(crate) fn validate_where_predicate(
        &self,
        table: &str,
        p: &parser::Predicate,
    ) -> Result<()> {
        use parser::Predicate;
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        fn bool_column(schema: &[String], types: &[String], name: &str) -> bool {
            schema
                .iter()
                .position(|c| c == name)
                .and_then(|i| types.get(i))
                .map(|t| t.trim().to_ascii_uppercase().starts_with("BOOL"))
                .unwrap_or(false)
        }
        match p {
            Predicate::Bool(a, _, b) => {
                self.validate_where_predicate(table, a)?;
                self.validate_where_predicate(table, b)
            }
            Predicate::Not(a) => self.validate_where_predicate(table, a),
            Predicate::IsTrue(e) => match e {
                ExprAst::Column(c) if bool_column(&schema, &types, c) => Ok(()),
                ExprAst::Column(_) => Err(Error::Invalid(
                    "filter requires a boolean expression".into(),
                )),
                // A function call may yield a boolean (CONTAINS, FILTER_TEXT…);
                // its own validation applies. The numeric pseudo-columns do not.
                ExprAst::Func { name, .. }
                    if ["ROW_ID", "SCORE", "DISTANCE", "SIMILARITY"]
                        .iter()
                        .any(|n| name.eq_ignore_ascii_case(n)) =>
                {
                    Err(Error::Invalid(
                        "filter requires a boolean expression".into(),
                    ))
                }
                ExprAst::Func { .. } => Ok(()),
                _ => Err(Error::Invalid(
                    "filter requires a boolean expression".into(),
                )),
            },
            _ => Ok(()),
        }
    }

    pub(crate) fn validate_aggregate_args(&self, table: &str, items: &[SelectItem]) -> Result<()> {
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        for it in items {
            let SelectItem::Aggregate(a) = it else {
                continue;
            };
            if !matches!(a.func, AggFunc::Sum | AggFunc::Avg) {
                continue;
            }
            for (j, name) in a.args.iter().enumerate() {
                // a computed argument is validated by its own expression path
                if a.arg_exprs.get(j).map(|o| o.is_some()).unwrap_or(false) {
                    continue;
                }
                let Some(ci) = schema.iter().position(|c| c == name) else {
                    continue;
                };
                let ty = types
                    .get(ci)
                    .cloned()
                    .unwrap_or_default()
                    .to_ascii_uppercase();
                let numeric = matches!(
                    ty.as_str(),
                    "TINYINT"
                        | "SMALLINT"
                        | "INT"
                        | "INTEGER"
                        | "BIGINT"
                        | "FLOAT"
                        | "DOUBLE"
                        | "REAL"
                        | "TINYINT8"
                        | "UINT8"
                        | "FLOAT16"
                        | "BFLOAT16"
                );
                if !numeric {
                    return Err(Error::Invalid(format!(
                        "{}({name}): argument type '{ty}' is not numeric",
                        a.func.as_str()
                    )));
                }
            }
        }
        Ok(())
    }

    /// Per-column defaults (as `.slt` text) for `table`.
    pub(crate) fn table_defaults(&self, table: &str) -> Result<Vec<Option<String>>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .defaults)
    }

    /// Writes the chunk files of every secondary index on `table`, the way
    /// `NewTxn::Import` populates them (`PopulateIndex(…, DumpIndexCause::
    /// kImport)` per segment). A `COPY` is an import to the reference, so it
    /// must leave the index readable without a later dump.
    pub(crate) fn populate_secondary_indexes(&self, table: &str) {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if im.table != table && im.table != self.qualified_table(table) {
                continue;
            }
            if !im.index_type.eq_ignore_ascii_case("SECONDARY") {
                continue;
            }
            let _ = self.build_index(&im.table, &im.column);
        }
    }

    /// The postings the in-memory full-text segment of `column` holds — what
    /// `ChunkIndexMetaInfo::term_count_` reports for a dump of it (it counts
    /// `(term, row)` pairs, not distinct terms: 100 rows of `aaa bbb` is 200).
    pub(crate) fn text_term_count(&self, table: &str, column: &str) -> u32 {
        let key = self.table_key(table);
        let Some(index_id) = self.fulltext_index_id(table, column) else {
            return 0;
        };
        let shared = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.text.get(&index_id).cloned());
        let Some(shared) = shared else {
            return 0;
        };
        let text = shared.read().unwrap();
        // Spilled runs keep their postings on disk; their term dictionary is
        // loaded with the reader, so `postings` covers the memory tail only.
        text.mem
            .postings
            .iter()
            .map(|postings| postings.list.len())
            .sum::<usize>() as u32
    }

    /// Runs `SEARCH …`. A single `MATCH` clause ranks its own column; several
    /// clauses combined with `FUSION(…)` return the union of the sub-searches.
    pub(crate) fn run_search(
        &self,
        table: &str,
        s: &SearchClause,
        pred: Option<&Expr>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let hidden: Vec<String> = ["#_row_id", "#_similarity", "#_distance", "#_score"]
            .iter()
            .map(|x| x.to_string())
            .collect();
        // A `FUSION(…)` tail always routes through the fusion operator, even
        // when it has a single child (e.g. `FUSION('match_tensor', …)` re-ranks
        // the one sub-search's candidates).
        if s.matches.len() > 1 || s.fusion.is_some() {
            return self.fuse_matches(table, s, pred, hidden);
        }
        match s.matches.first() {
            Some(m) => self.run_one_match(
                table,
                m,
                s.using_index.as_deref(),
                s.with_opts.as_deref(),
                pred,
            ),
            None => Ok((Vec::new(), hidden)),
        }
    }

    /// `FUSION(…)`: runs every sub-search and combines the per-document child
    /// scores. Reproduces `PhysicalFusion::ExecuteRRFWeighted`
    /// (`physical_fusion_impl.cpp`): `rrf` is rank-based with a rank constant of
    /// 60, `weighted_sum`/`max` combine scores normalised by `none` (default for
    /// ...), `minmax` (the default), `l2` or `atan`, with per-child weights.
    pub(crate) fn fuse_matches(
        &self,
        table: &str,
        s: &SearchClause,
        pred: Option<&Expr>,
        hidden: Vec<String>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let method = s
            .fusion
            .clone()
            .unwrap_or_default()
            .trim()
            .to_ascii_lowercase();
        let params = s.fusion_params.clone().unwrap_or_default();
        let opt = |key: &str| -> Option<String> {
            params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case(key))
                .map(|(_, v)| v.trim().to_string())
        };
        let num_children = s.matches.len();
        // Run each sub-search (best-first) and note whether its score is a
        // similarity (higher is better) or a distance (lower is better).
        let mut children: Vec<Vec<Vec<Value>>> = Vec::with_capacity(num_children);
        let mut min_heap: Vec<bool> = Vec::with_capacity(num_children);
        for m in &s.matches {
            let (rows, _) = self.run_one_match(
                table,
                m,
                s.using_index.as_deref(),
                s.with_opts.as_deref(),
                pred,
            )?;
            min_heap.push(match_is_min_heap(m));
            children.push(rows);
        }
        // `match_tensor` ignores the children's scores entirely: it reranks the
        // union of the rows by a tensor similarity.
        if method == "match_tensor" {
            return self.fuse_match_tensor(table, &opt, children, hidden);
        }
        // Per-document child scores, in first-seen order (fragment 0 first, then
        // each document in rank order).
        struct Doc {
            scores: Vec<f64>,
            mask: Vec<bool>,
            row: Vec<Value>,
        }
        let mut docs: Vec<Doc> = Vec::new();
        let mut index: HashMap<i64, usize> = HashMap::new();
        for (ci, rows) in children.iter().enumerate() {
            for (rank, row) in rows.iter().enumerate() {
                let id = match row.iter().rev().nth(3) {
                    Some(Value::Int(i)) => *i,
                    _ => continue,
                };
                let score = match row.last() {
                    Some(Value::Float(f)) => *f,
                    Some(Value::Int(i)) => *i as f64,
                    _ => 0.0,
                };
                let di = *index.entry(id).or_insert_with(|| {
                    docs.push(Doc {
                        scores: vec![0.0; num_children],
                        mask: vec![false; num_children],
                        row: row.clone(),
                    });
                    docs.len() - 1
                });
                docs[di].mask[ci] = true;
                docs[di].scores[ci] = if method == "rrf" {
                    (rank + 1) as f64
                } else {
                    score
                };
            }
        }
        let topn: usize = opt("topn")
            .or_else(|| opt("window_size"))
            .and_then(|v| v.parse::<usize>().ok())
            .filter(|n| *n >= 1)
            .unwrap_or(100);
        // `FUSION('match_tensor', …)` re-ranks the candidates its children
        // produced by the MaxSim of a tensor column against a query tensor
        // (`physical_fusion_impl.cpp`): the fusion score replaces the child
        // scores rather than combining them.
        if method == "match_tensor"
            && opt("match_method")
                .map(|m| m.eq_ignore_ascii_case("maxsim"))
                .unwrap_or(true)
        {
            let schema = self.table_schema(table)?;
            let types = self.column_types(table)?;
            let col_name = opt("column_name").unwrap_or_default();
            let col_idx = schema.iter().position(|c| c == &col_name);
            let q: Vec<Vec<f64>> = opt("search_tensor")
                .map(|s| parse_tensor_leaves(&s))
                .unwrap_or_default();
            let col_elem = col_idx
                .and_then(|i| types.get(i))
                .map(|t| tensor_elem_type(t))
                .unwrap_or(ElemType::Float32);
            let qry_elem = opt("tensor_data_type")
                .map(|s| elem_type_of(&s))
                .unwrap_or(ElemType::Float32);
            for doc in &mut docs {
                let score = match col_idx.and_then(|i| doc.row.get(i)) {
                    Some(Value::Str(v)) => tensor_score(
                        &q,
                        v,
                        col_elem,
                        qry_elem,
                        col_idx
                            .and_then(|i| types.get(i))
                            .map(|t| t.to_ascii_uppercase().starts_with("TENSORARRAY"))
                            .unwrap_or(false),
                    ),
                    _ => 0.0,
                };
                set_score(&mut doc.row, score);
            }
            let mut docs = docs;
            docs.sort_by(|a, b| {
                row_score(&b.row)
                    .partial_cmp(&row_score(&a.row))
                    .unwrap_or(std::cmp::Ordering::Equal)
            });
            docs.truncate(topn);
            return Ok((docs.into_iter().map(|d| d.row).collect(), hidden));
        }
        if method == "rrf" {
            let rank_constant: f64 = opt("rank_constant")
                .and_then(|v| v.parse::<f64>().ok())
                .filter(|n| *n >= 1.0)
                .unwrap_or(60.0);
            for doc in &mut docs {
                let mut score = 0.0;
                for (ci, rank) in doc.scores.iter().enumerate() {
                    if doc.mask[ci] && *rank >= 1.0 {
                        score += 1.0 / (rank_constant + rank);
                    }
                }
                set_score(&mut doc.row, score);
            }
        } else {
            // `weights` (default 1.0 each) and `normalize` (default `minmax`).
            let mut weights: Vec<f64> = opt("weights")
                .map(|w| {
                    w.split(',')
                        .filter_map(|x| x.trim().parse::<f64>().ok())
                        .collect()
                })
                .unwrap_or_default();
            weights.resize(num_children, 1.0);
            let normalize = opt("normalize")
                .map(|v| v.to_ascii_lowercase())
                .unwrap_or_else(|| "minmax".into());
            let mut min_scores = vec![f64::INFINITY; num_children];
            let mut max_scores = vec![f64::NEG_INFINITY; num_children];
            let mut l2_sums = vec![0.0f64; num_children];
            for doc in &docs {
                for ci in 0..num_children {
                    if !doc.mask[ci] {
                        continue;
                    }
                    let score = doc.scores[ci];
                    match normalize.as_str() {
                        "minmax" => {
                            min_scores[ci] = min_scores[ci].min(score);
                            max_scores[ci] = max_scores[ci].max(score);
                        }
                        "l2" | "l2_norm" => l2_sums[ci] += score * score,
                        _ => {}
                    }
                }
            }
            for doc in &mut docs {
                let mut fusion = if method == "max" {
                    f64::NEG_INFINITY
                } else {
                    0.0
                };
                for ci in 0..num_children {
                    if !doc.mask[ci] {
                        continue;
                    }
                    let mut score = doc.scores[ci];
                    match normalize.as_str() {
                        "atan" => score = score.atan() / std::f64::consts::PI + 0.5,
                        "minmax" => {
                            score = if max_scores[ci] != min_scores[ci] {
                                (score - min_scores[ci]) / (max_scores[ci] - min_scores[ci])
                            } else {
                                0.0
                            }
                        }
                        "l2" | "l2_norm" => {
                            let norm = l2_sums[ci].sqrt();
                            if norm > 0.0 {
                                score /= norm;
                            }
                        }
                        _ => {}
                    }
                    if !min_heap[ci] {
                        if normalize == "none" {
                            score = -score;
                        } else {
                            score = 1.0 - score;
                        }
                    }
                    if method == "max" {
                        fusion = fusion.max(score);
                    } else {
                        fusion += weights[ci] * score;
                    }
                }
                set_score(&mut doc.row, fusion);
            }
        }
        let mut docs: Vec<Doc> = docs;
        docs.sort_by(|a, b| {
            let x = row_score(&a.row);
            let y = row_score(&b.row);
            y.partial_cmp(&x).unwrap_or(std::cmp::Ordering::Equal)
        });
        docs.truncate(topn);
        Ok((docs.into_iter().map(|d| d.row).collect(), hidden))
    }

    /// `FUSION('match_tensor', …)`: reranks the **union** of the sub-search rows
    /// by the `search_tensor` option's similarity against `column_name`,
    /// reproducing `PhysicalFusion::ExecuteMatchTensor`. The children's own
    /// scores are not used — only the row set they produced.
    pub(crate) fn fuse_match_tensor(
        &self,
        table: &str,
        opt: &impl Fn(&str) -> Option<String>,
        children: Vec<Vec<Vec<Value>>>,
        hidden: Vec<String>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let column = opt("column_name").unwrap_or_default();
        let search_tensor = opt("search_tensor").unwrap_or_default();
        let data_type = opt("tensor_data_type").unwrap_or_else(|| "float".into());
        let topn = opt("topn")
            .and_then(|v| v.parse::<usize>().ok())
            .filter(|n| *n > 0)
            .unwrap_or(10);
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        let col_idx = schema
            .iter()
            .position(|c| *c == column)
            .ok_or_else(|| Error::Invalid(format!("no column '{column}' for match_tensor")))?;
        // Mirror the `MATCH TENSOR` query path: reshape by the declared
        // dimension before parsing into per-tensor leaves.
        let qtext = match type_dim(&types[col_idx]) {
            Some(d) => reshape_tensor(&search_tensor, d),
            None => search_tensor.clone(),
        };
        let q = parse_tensor_leaves(&qtext);
        let col_elem = tensor_elem_type(types.get(col_idx).map(String::as_str).unwrap_or(""));
        let qry_elem = elem_type_of(&data_type);

        // Deduplicate the union by row id, keeping the first occurrence's row.
        let mut seen: std::collections::HashSet<i64> = Default::default();
        let mut candidates: Vec<(i64, Vec<Value>)> = Vec::new();
        for child in children {
            for row in child {
                let id = match row.iter().rev().nth(3) {
                    Some(Value::Int(i)) => *i,
                    _ => continue,
                };
                if seen.insert(id) {
                    candidates.push((id, row));
                }
            }
        }
        let mut scored: Vec<(i64, f64, Vec<Value>)> = Vec::new();
        for (id, row) in candidates {
            let cell = match row.get(col_idx) {
                Some(Value::Str(s)) => s.clone(),
                Some(v) => v.to_slt_string(),
                None => continue,
            };
            let score = tensor_score(
                &q,
                &cell,
                col_elem,
                qry_elem,
                types
                    .get(col_idx)
                    .map(|t| t.to_ascii_uppercase().starts_with("TENSORARRAY"))
                    .unwrap_or(false),
            );
            scored.push((id, score, row));
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        Ok((
            scored
                .into_iter()
                .map(|(_, score, mut row)| {
                    set_score(&mut row, score);
                    row
                })
                .collect(),
            hidden,
        ))
    }

    /// Runs a single `MATCH` clause by brute force, returning the matching rows
    /// with hidden `ROW_ID/SIMILARITY/DISTANCE/SCORE` columns.
    pub(crate) fn run_one_match(
        &self,
        table: &str,
        m: &MatchClause,
        using_index: Option<&str>,
        with_opts: Option<&str>,
        pred: Option<&Expr>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let hidden: Vec<String> = ["#_row_id", "#_similarity", "#_distance", "#_score"]
            .iter()
            .map(|x| x.to_string())
            .collect();
        let is_sparse = m.kind.eq_ignore_ascii_case("SPARSE");
        let is_tensor = m.kind.eq_ignore_ascii_case("TENSOR");
        if m.kind.eq_ignore_ascii_case("TEXT") {
            return self.search_text_terms(table, m, pred, &hidden);
        }
        if !m.kind.eq_ignore_ascii_case("VECTOR") && !is_sparse && !is_tensor {
            return Ok((Vec::new(), hidden));
        }
        // A vector/sparse/tensor scan also reads most of the table; share the
        // same bound as full-text scans (acquired after the TEXT early return,
        // which takes the slot itself).
        let _scan = self.context.scan_gate.enter();
        // A trailing index name in `MATCH VECTOR (col, vec, 'float', 'l2', k,
        // 'idx')` must reference an existing index.
        if !is_tensor && !is_sparse {
            if let Some(parser::ExprAst::Literal(parser::Literal::Str(name))) = m.args.get(4) {
                let name = name.trim();
                // A bare identifier is an index hint; anything with `=` is a
                // search option (`threshold=200`).
                if !name.is_empty()
                    && !name.contains('=')
                    && !name.contains(';')
                    && !self
                        .storage
                        .catalog
                        .list_indexes()?
                        .iter()
                        .any(|im| im.name == name)
                {
                    return Err(Error::NotFound(format!("index '{name}'")));
                }
            }
        }
        // `USING INDEX (name)` must reference an existing index
        if let Some(list) = using_index {
            for name in list
                .trim()
                .trim_start_matches('(')
                .trim_end_matches(')')
                .split(',')
                .map(|n| n.trim())
                .filter(|n| !n.is_empty())
            {
                if !self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == name)
                {
                    return Err(Error::Invalid(format!("index '{name}' does not exist")));
                }
            }
        }
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        // An in-parenthesis `WHERE` filter also prunes candidates before ranking.
        let in_filter = match m.filter.as_ref() {
            Some(p) => Some(planner::predicate_expr(p, &schema)?),
            None => None,
        };
        let col_idx = match match_columns(&m.column)
            .iter()
            .find_map(|c| schema.iter().position(|s| s == c))
        {
            Some(i) => i,
            None => return Ok((Vec::new(), hidden)),
        };
        let decl = types.get(col_idx).map(String::as_str).unwrap_or("");
        if !is_embedding_type(decl) && !is_tensor_type(decl) && !is_sparse_type(decl) {
            return Err(Error::Invalid(format!(
                "syntax: column '{}' is not a vector column",
                m.column
            )));
        }
        let query_items = match m.args.first() {
            Some(ExprAst::Literal(Literal::Array(items))) => items.clone(),
            _ => Vec::new(),
        };
        // A plain (multi-)vector column only accepts a query vector of its own
        // dimension; a `MULTIVECTOR` compares against each sub-vector.
        if !is_sparse && !is_tensor {
            let decl = types.get(col_idx).map(String::as_str).unwrap_or("");
            let is_multi = decl.to_ascii_uppercase().starts_with("MULTIVECTOR");
            if !is_multi {
                if let Some(d) = type_dim(decl) {
                    // A bit vector accepts either one bit per item or a packed
                    // form with eight bits per item.
                    let bit_col = tensor_elem_type(decl) == ElemType::Bit;
                    let n = query_items.len();
                    let ok = !query_items.is_empty() && (n == d || (bit_col && n * 8 == d));
                    if !ok {
                        return Err(Error::Invalid(format!(
                            "query embedding has {} dimensions, but column '{}' expects {}",
                            n, m.column, d
                        )));
                    }
                }
            }
        }
        // A VECTOR column only accepts a query of its own element type; a bit
        // column also accepts a float query (the bits are read as 0/1). A
        // tensor query is cast to the stored element type, so it is exempt.
        if !is_sparse && !is_tensor {
            let col_elem = tensor_elem_type(decl);
            let q_elem = elem_type_of_optional(m.args.get(1)).unwrap_or(ElemType::Float32);
            let ok = if col_elem == ElemType::Bit {
                matches!(q_elem, ElemType::Bit | ElemType::Float32)
            } else {
                q_elem == col_elem
            };
            if !ok {
                return Err(Error::Invalid(
                    "query element type does not match the column's".into(),
                ));
            }
        }
        let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
            .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
            .to_ascii_lowercase();
        // A zero-dimension sparse column cannot be searched.
        if is_sparse {
            let up = decl.to_ascii_uppercase();
            if up.contains(", 0)") || up.contains(",0)") {
                return Err(Error::Invalid(format!(
                    "invalid sparse dimension for column '{}'",
                    m.column
                )));
            }
        }
        // Sparse vectors only support inner product.
        if is_sparse && metric != "ip" {
            return Err(Error::Invalid(format!(
                "invalid distance type '{metric}' for a sparse column"
            )));
        }
        // A `MATCH VECTOR` names the query element type; an unknown one is an
        // error (the reference reports `INVALID_EMBEDDING_DATA_TYPE`).
        if !is_tensor && !is_sparse {
            if let Some(s) = arg_str(m.args.get(1)) {
                if elem_type_named(&s).is_none() {
                    return Err(Error::Invalid(format!("invalid embedding data type '{s}'")));
                }
            }
        }
        // `hamming` is defined on bit vectors only.
        if metric == "hamming" && tensor_elem_type(decl) != ElemType::Bit {
            return Err(Error::Invalid(
                "not supported: hamming distance requires a bit vector".into(),
            ));
        }
        let topn = if is_tensor {
            // topn lives in the trailing parameter string; the default is 10.
            // The reference multiplies it by 100 into a u32, so anything
            // above `u32::MAX / 100` would wrap; it is rejected.
            let raw = arg_str(m.args.get(3)).unwrap_or_default();
            let value = raw
                .split(';')
                .filter_map(|part| {
                    let (k, v) = part.split_once('=')?;
                    k.trim().eq_ignore_ascii_case("topn").then_some(v.trim())
                })
                .next_back();
            // The SDK sends the count positionally; `WITH (topn=…)` is the
            // option spelling. Default 10.
            let bare = raw.trim();
            match value {
                None => match bare.parse::<i64>() {
                    Ok(n) if n > 0 => n as usize,
                    _ => 10,
                },
                Some(v) => match v.parse::<i64>() {
                    Ok(n) if n > 0 && n <= i64::from(u32::MAX / 100) => n as usize,
                    Ok(_) => {
                        return Err(Error::Invalid(
                            "invalid parameter: topn must be a positive integer".into(),
                        ))
                    }
                    Err(_) => {
                        return Err(Error::Invalid(
                            "invalid parameter: topn must be a positive integer".into(),
                        ))
                    }
                },
            }
        } else {
            let n = m.args.iter().rev().find_map(arg_int).unwrap_or(10);
            if n <= 0 {
                return Err(Error::Invalid(
                    "invalid parameter: topn must be a positive integer".into(),
                ));
            }
            n as usize
        };
        // `MATCH VECTOR` carries its options after `topn`; `SPARSE`/`TENSOR`
        // carry them in the last argument (the reference's `opt_params` list).
        // The SQL form may omit `topn`, in which case the options are the final
        // string — a bare `'ip'` has no `=` and is not an option bag.
        let params_idx = if is_tensor || is_sparse { 3 } else { 4 };
        let params = arg_str(m.args.get(params_idx)).unwrap_or_else(|| {
            if is_tensor || is_sparse {
                m.args
                    .iter()
                    .rev()
                    .find_map(|arg| match arg {
                        ExprAst::Literal(Literal::Str(s)) if s.contains('=') => Some(s.clone()),
                        _ => None,
                    })
                    .unwrap_or_default()
            } else {
                String::new()
            }
        });
        // `MATCH SPARSE` accepts exactly the names `ExpressionBinder::BuildSearchExpr`
        // allows (`expression_binder_impl.cpp:1140-1146`): `alpha`, `beta`,
        // `tail`, `threshold`. `index_name` / `ignore_index` are consumed by the
        // service layer and `filter` is a separate field, so the port's string
        // transport may carry them too. Note `tail` is accepted and **ignored**
        // by the reference — `BMPUtil::ParseBmpSearchOptions` only reads
        // `use_tail`, which never gets past the binder.
        let sparse_known = |key: &str| {
            matches!(
                key,
                // The reference's `opt_params` names.
                "alpha"
                    | "beta"
                    | "tail"
                    | "threshold"
                    // Consumed by its service layer / separate fields.
                    | "index_name"
                    | "ignore_index"
                    | "filter"
                    // The port's SQL form may carry `topn` in the option bag
                    // (`MATCH SPARSE (col, vec, 'ip', 'topn=5;alpha=0.5')`); the
                    // reference takes it as the positional 4th argument.
                    | "topn"
            )
        };
        let sources: Vec<&str> = if is_sparse {
            [Some(params.as_str()), with_opts]
                .into_iter()
                .flatten()
                .collect()
        } else {
            vec![params.as_str()]
        };
        for source in sources {
            for part in source.split(';').filter(|p| !p.trim().is_empty()) {
                let key = part
                    .split('=')
                    .next()
                    .unwrap_or("")
                    .trim()
                    .to_ascii_lowercase();
                if key.is_empty() {
                    continue;
                }
                if is_sparse {
                    if !sparse_known(&key) {
                        return Err(Error::Syntax(format!(
                            "Unsupported optional parameter: {key}"
                        )));
                    }
                    continue;
                }
                let known = matches!(
                    key.as_str(),
                    "topn"
                        | "block_max"
                        | "threshold"
                        | "ef"
                        | "ef_search"
                        | "nprobe"
                        | "io_limit"
                        | "distance"
                        | "similarity"
                        | "alpha"
                        | "beta"
                        // `BMPUtil::ParseBmpSearchOptions`'s tail switch.
                        | "use_tail"
                        // index hints the SDK passes through; the planner picks the
                        // index itself, so they are accepted and ignored.
                        | "index_name"
                        | "ignore_index"
                        | "index_type"
                        // `MATCH TEXT`/`MATCH SPARSE` carry a per-search filter.
                        | "filter"
                        // `ParseMatchTensorExpr`'s EMVB pruning thresholds; the
                        // port's EMVB search scores candidates exactly and does
                        // not prune on them yet.
                        | "emvb_threshold_first"
                        | "emvb_threshold_final"
                        // `plaid_search_options` reads these four; an override
                        // the reference would reject keeps the default, so they
                        // have to reach it instead of failing here.
                        | "plaid_n_ivf_probe"
                        | "plaid_centroid_score_threshold"
                        | "plaid_n_doc_to_score"
                        | "plaid_n_full_scores"
                );
                if !known {
                    return Err(Error::Invalid(format!(
                        "invalid parameter: unknown option '{key}'"
                    )));
                }
            }
        }
        // `index_name=…` selects a specific index; naming one that does not
        // exist is an error, and combining it with `ignore_index=true` is
        // contradictory.
        let option_value = |name: &str| {
            params.split(';').find_map(|p| {
                let (k, v) = p.split_once('=')?;
                k.trim()
                    .eq_ignore_ascii_case(name)
                    .then(|| v.trim().to_string())
            })
        };
        if option_value("index_name").is_some()
            && option_value("ignore_index")
                .map(|v| v.eq_ignore_ascii_case("true"))
                .unwrap_or(false)
        {
            return Err(Error::Invalid(
                "syntax: cannot combine index_name with ignore_index".into(),
            ));
        }
        if let Some(want) = option_value("index_name") {
            let exists = self.storage.catalog.index_by_name(table, &want)?.is_some();
            if !exists {
                return Err(Error::NotFound(format!("index '{want}'")));
            }
        }
        // C++ `KnnScanSharedData` / `KnnSearchOption`: one object for the
        // `WITH` and legacy option strings.
        let knn_opts = knn_scan::KnnScanOptions::parse(with_opts, &params);
        let threshold = knn_opts.threshold;
        // `filter=…` narrows the candidates of this search (the SDK passes the
        // same expression it would put in WHERE).
        let search_filter = match option_value("filter") {
            Some(f) if !f.is_empty() => {
                let p = parse_filter_predicate(&f)?;
                Some(planner::predicate_expr(&p, &self.table_schema(table)?)?)
            }
            _ => None,
        };
        // A per-search `filter=` and an outer WHERE are mutually exclusive (the
        // reference rejects the combination).
        if search_filter.is_some() && pred.is_some() {
            return Err(Error::Invalid(
                "cannot have both a filter option and a where clause".into(),
            ));
        }
        // Multi-vector: answer from the write-time index (sealed chunks + the
        // memory tail) when no predicate needs the other columns. The index
        // covers every live row, so this path does not scan the table.
        // A `filter=` option or an in-parenthesis `WHERE` narrows the candidates
        // before ranking; the reference bakes it into the search bitmask
        // (`CalculateFilterBitmask`) instead of scanning, so the index answers it
        // here too. The outer `WHERE` still takes the scan path: it may hold
        // constructs this evaluator does not model.
        if is_tensor && pred.is_none() {
            let decl_up = types
                .get(col_idx)
                .map(|t| t.to_ascii_uppercase())
                .unwrap_or_default();
            let column = match_columns(&m.column)
                .first()
                .cloned()
                .unwrap_or_default();
            let ann_indexed = self.tensor_has_ann_index(table, &column);
            if (decl_up.starts_with("MULTIVECTOR") || ann_indexed)
                && tensor_elem_type(&decl_up) != ElemType::Bit
            {
                if let Some(dims) = type_dim(&decl_up) {
                    let q_flat: Vec<f32> = query_items
                        .iter()
                        .filter_map(|x| x.trim().parse::<f32>().ok())
                        .collect();
                    if dims > 0 && !q_flat.is_empty() && q_flat.len().is_multiple_of(dims) {
                        let query: Vec<Vec<f32>> =
                            q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
                        // No catalog index on the column: the reference's
                        // `PlanWithIndex` simply leaves the index out and the
                        // scan reads the blocks, so a missing record falls
                        // through to the exact scan below.
                        let index = self.multivector_index_for(table, &column, dims).ok();
                        if let Some(index) = index {
                            let dead = self.tombstone_set(table)?;
                            let rank = |seq: u64| {
                                self.exact_tensor_score(table, &column, dims, &query, seq)
                            };
                            let plaid_options = crate::engine::state::plaid_search_options(
                                |key| option_value(key),
                                topn,
                            );
                            let filters: Vec<&Expr> = [in_filter.as_ref(), search_filter.as_ref()]
                                .into_iter()
                                .flatten()
                                .collect();
                            let allowed: Option<std::collections::HashSet<u64>> =
                                if filters.is_empty() {
                                    None
                                } else {
                                    Some(self.match_filter_rows(table, &filters)?)
                                };
                            let hits = index.read().unwrap().search(
                                &query,
                                topn.max(1),
                                &dead,
                                Some(&rank),
                                &plaid_options,
                                allowed.as_ref(),
                            );
                            let scored: Vec<(u64, f64)> = hits
                                .into_iter()
                                .map(|(seq, score)| (seq, f64::from(score)))
                                .filter(|(_, score)| threshold.map(|t| *score >= t).unwrap_or(true))
                                .collect();
                            let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq).collect();
                            let mut by_id: HashMap<usize, Vec<Value>> = self
                                .fetch_some_rows_with_ids(table, &wanted)?
                                .into_iter()
                                .collect();
                            let mut out = Vec::new();
                            for (seq, score) in scored {
                                let Some(mut row) = by_id.remove(&(seq as usize)) else {
                                    continue;
                                };
                                row.push(Value::Int(seq as i64));
                                row.push(Value::Float(score));
                                row.push(Value::Float(-score));
                                row.push(Value::Float(score));
                                out.push(row);
                            }
                            return Ok((out, hidden));
                        }
                    }
                }
            }
        }

        // Sparse: answer from the write-time BMP index when no predicate needs
        // the other columns; it already covers every live row.
        if is_sparse
            && !types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false)
            && pred.is_none()
            && in_filter.is_none()
            && search_filter.is_none()
        {
            let width = sparse_idx_width(types.get(col_idx).map(String::as_str).unwrap_or(""));
            let qv: Vec<(i32, f32)> = parse_sparse(&query_items)
                .into_iter()
                .map(|(i, v)| (i as i32, v as f32))
                .collect();
            if !qv.is_empty() {
                let column = match_columns(&m.column)
                    .first()
                    .cloned()
                    .unwrap_or_default();
                // A column without a BMP index is answered by the exact scan
                // (the reference's `PlanWithIndex` skips a column whose index
                // the catalog does not list).
                let index = self.sparse_index_for(table, &column, width).ok();
                if let Some(index) = index {
                    let dead = self.tombstone_set(table)?;
                    let bmp_options = parse_bmp_search_options(with_opts, &params);
                    let hits = index
                        .read()
                        .unwrap()
                        .search(&qv, topn.max(1), &dead, bmp_options);
                    let scored: Vec<(u64, f64)> = hits
                        .into_iter()
                        .map(|(seq, score)| (seq, f64::from(score)))
                        .filter(|(_, score)| threshold.map(|t| *score >= t).unwrap_or(true))
                        .collect();
                    let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq).collect();
                    let mut by_id: HashMap<usize, Vec<Value>> = self
                        .fetch_some_rows_with_ids(table, &wanted)?
                        .into_iter()
                        .collect();
                    let mut out = Vec::new();
                    for (seq, score) in scored {
                        let Some(mut row) = by_id.remove(&(seq as usize)) else {
                            continue;
                        };
                        row.push(Value::Int(seq as i64));
                        row.push(Value::Float(score));
                        row.push(Value::Float(-score));
                        row.push(Value::Float(score));
                        out.push(row);
                    }
                    return Ok((out, hidden));
                }
            }
        }

        // Multi-vector: answer from the write-time EMVB index (sealed chunks
        // plus the memory tail) when no predicate needs the other columns.
        // C++: `PhysicalMatchTensorScan` over `EMVBIndexInMem`/segment readers.
        if is_tensor
            && types
                .get(col_idx)
                .map(|t| t.to_ascii_uppercase().starts_with("MULTIVECTOR"))
                .unwrap_or(false)
            && pred.is_none()
        {
            let dims = types.get(col_idx).and_then(|t| type_dim(t));
            if let Some(dims) = dims.filter(|d| *d > 0) {
                let q_flat: Vec<f32> = query_items
                    .iter()
                    .filter_map(|x| x.trim().parse::<f32>().ok())
                    .collect();
                if !q_flat.is_empty() && q_flat.len().is_multiple_of(dims) {
                    let query: Vec<Vec<f32>> =
                        q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
                    let column = match_columns(&m.column)
                        .first()
                        .cloned()
                        .unwrap_or_default();
                    let tkey = self.table_key(table);
                    let index_id = self.multivector_index_id(table, &column);
                    let index = index_id.and_then(|id| {
                        self.tables
                            .lock()
                            .unwrap()
                            .get(&tkey)
                            .and_then(|e| e.mv.get(&id))
                            .cloned()
                    });
                    if let Some(index) = index {
                        let dead = self.tombstone_set(table)?;
                        let rank =
                            |seq: u64| self.exact_tensor_score(table, &column, dims, &query, seq);
                        let plaid_options = crate::engine::state::plaid_search_options(
                            |key| option_value(key),
                            topn,
                        );
                        let filters: Vec<&Expr> = [in_filter.as_ref(), search_filter.as_ref()]
                            .into_iter()
                            .flatten()
                            .collect();
                        let allowed: Option<std::collections::HashSet<u64>> = if filters.is_empty()
                        {
                            None
                        } else {
                            Some(self.match_filter_rows(table, &filters)?)
                        };
                        let hits: Vec<(u64, f32)> = index
                            .read()
                            .unwrap()
                            .search(
                                &query,
                                topn.max(1),
                                &dead,
                                Some(&rank),
                                &plaid_options,
                                allowed.as_ref(),
                            )
                            .into_iter()
                            .filter(|(_, score)| {
                                threshold.map(|t| f64::from(*score) >= t).unwrap_or(true)
                            })
                            .collect();
                        let wanted: Vec<u64> = hits.iter().map(|(seq, _)| *seq).collect();
                        let mut by_id: HashMap<usize, Vec<Value>> = self
                            .fetch_some_rows_with_ids(table, &wanted)?
                            .into_iter()
                            .collect();
                        let mut out = Vec::new();
                        for (seq, score) in hits {
                            let Some(mut row) = by_id.remove(&(seq as usize)) else {
                                continue;
                            };
                            let score = f64::from(score);
                            row.push(Value::Int(seq as i64));
                            row.push(Value::Float(score));
                            row.push(Value::Float(-score));
                            row.push(Value::Float(score));
                            out.push(row);
                        }
                        return Ok((out, hidden));
                    }
                }
            }
        }

        let cache: Option<SharedVectorCache> = (|| {
            if is_sparse || is_tensor {
                return None;
            }
            if pred.is_some() || in_filter.is_some() || search_filter.is_some() {
                return None;
            }
            let decl = types.get(col_idx)?.to_ascii_uppercase();
            if decl.contains("MULTI") || !decl.contains("FLOAT") || decl.contains("INT") {
                return None;
            }
            if !matches!(metric.as_str(), "l2" | "cosine" | "ip") {
                return None;
            }
            let dims = type_dim(types.get(col_idx)?)?;
            if dims == 0 {
                return None;
            }
            let col = match_columns(&m.column).first()?.clone();
            self.vector_cache_for(table, &col, dims).ok()
        })();
        // Rows come from whichever consumer needs them: an index answers with a
        // candidate set and only those rows are decoded; the exact scan needs
        // the table. `rows` therefore starts empty and is filled once the
        // candidate set is known.
        let has_filter = pred.is_some() || in_filter.is_some() || search_filter.is_some();
        let mut rows: Vec<(usize, Vec<Value>)> = Vec::new();
        // The cache path scores from `score_cache` and never touches `rows`.
        // A `bit` sparse column scores and renders as an integer.
        let bit_sparse = is_sparse && types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false);
        // Walk a persisted HNSW graph instead of every row when one is loaded
        // for this column (plan #70). The candidates are still scored exactly
        // below, so the ranking equals the exhaustive scan whenever the graph
        // yields the whole top-`k` — `k` is generous and capped by the row
        // count, which fully covers a small table.
        let vector_candidates: Option<std::collections::HashSet<usize>> = (|| {
            if is_sparse || is_tensor {
                return None;
            }
            let col_name = match_columns(&m.column).first()?.clone();
            let col_type = types.get(col_idx)?.to_ascii_uppercase();
            if col_type.starts_with("MULTIVECTOR") || is_bit_type(&col_type) {
                return None; // a single-vector index
            }
            let qv: Vec<f32> = query_items
                .iter()
                .filter_map(|x| x.parse::<f64>().ok())
                .map(|x| x as f32)
                .collect();
            if qv.is_empty() {
                return None;
            }
            // A predicate can exclude rows the graph returned, so the graph alone
            // cannot decide the top-k: the reference re-scores every row the
            // filter admits. Keep the exact scan for filtered queries.
            if has_filter {
                return None;
            }
            // The index this scan reads: the one the statement named, else the
            // column's first HNSW/IVF index.
            let index_id = self
                .storage
                .catalog
                .indexes_of_table(&self.qualified_table(table))
                .unwrap_or_default()
                .into_iter()
                .find(|im| {
                    im.column == col_name
                        && (im.index_type.eq_ignore_ascii_case("HNSW")
                            || im.index_type.eq_ignore_ascii_case("IVF"))
                })
                .map(|im| im.id);
            self.knn_candidates(&knn_scan::KnnCandidateScan {
                table,
                index_id,
                query: &qv,
                rows: &rows,
                live_rows: None,
                cache: cache.as_ref(),
                topn,
                metric: &metric,
                options: &knn_opts,
            })
        })();
        // The candidate set is known: decode only the rows it kept, or the whole
        // table when no index answered.
        if cache.is_none() {
            rows = match &vector_candidates {
                // A predicate reads other columns, so a filtered query needs the
                // whole row; an unfiltered one is scored from the vector column
                // alone and gets its output rows from the block pass below.
                Some(cands) if !cands.is_empty() => {
                    let wanted: &[usize] = if has_filter { &[] } else { &[col_idx] };
                    let (_, got) = self.read_blocks_candidates_values(table, wanted, cands)?;
                    got
                }
                Some(_) => Vec::new(),
                None => self.fetch_rows_with_ids(table)?,
            };
        }
        // One BMP index per query over the scanned rows: sparse vectors use the
        // ported block-max scoring; other kinds leave this empty.
        let bmp_scores: HashMap<usize, f64> = (|| {
            if !is_sparse || bit_sparse || pred.is_some() || in_filter.is_some() {
                return HashMap::new();
            }
            let qv: Vec<(u32, f32)> = parse_sparse(&query_items)
                .into_iter()
                .map(|(i, v)| (i as u32, v as f32))
                .collect();
            // Per-segment BMP index written in the reference's byte layout
            // (`BMPAlg::SaveToPtr`) and queried through mmap.
            let width = sparse_idx_width(types.get(col_idx).map(String::as_str).unwrap_or(""));
            #[allow(clippy::type_complexity)]
            let mut segments: HashMap<u64, Vec<(u64, Vec<(i32, f32)>)>> = HashMap::new();
            for (seq, row) in &rows {
                let Some(Value::Str(v)) = row.get(col_idx) else {
                    continue;
                };
                let entries: Vec<(i32, f32)> = parse_sparse_str(v)
                    .into_iter()
                    .map(|(i, x)| (i as i32, x as f32))
                    .collect();
                let segment = (*seq as u64) >> 32;
                segments
                    .entry(segment)
                    .or_default()
                    .push((*seq as u64 & 0xffff_ffff, entries));
            }
            let mut best: HashMap<usize, f64> = HashMap::new();
            let key = self.table_key(table);
            let version = self
                .tables
                .lock()
                .unwrap()
                .get(&key)
                .map(|e| e.write_batches)
                .unwrap_or(0);
            let query_i32: Vec<(i32, f32)> = qv.iter().map(|(t, v)| (*t as i32, *v)).collect();
            // The query's `alpha` prunes the on-disk scan (`BmpSearchOptions`).
            let bmp_options = parse_bmp_search_options(with_opts, &params);
            for (segment, mut docs) in segments {
                docs.sort_by_key(|(row, _)| *row);
                let term_num = docs
                    .iter()
                    .flat_map(|(_, e)| e.iter().map(|(t, _)| *t + 1))
                    .max()
                    .unwrap_or(0)
                    .max(1) as usize;
                let dir = self.dir.join("sparse").join(
                    index_key(
                        table,
                        &match_columns(&m.column)
                            .first()
                            .cloned()
                            .unwrap_or_default(),
                    )
                    .replace('/', "_"),
                );
                let path = dir.join(format!("seg_{segment}_{}_{version}.bmp", docs.len()));
                if storage::bmp::write_bmp_file(
                    &path,
                    index::sparse::BMP_BLOCK_SIZE,
                    width,
                    &docs,
                    term_num,
                )
                .is_err()
                {
                    continue;
                }
                let Ok(reader) = storage::bmp::BmpReader::open(&path, width) else {
                    continue;
                };
                for (row, score) in reader.search(&query_i32, topn.max(1), bmp_options) {
                    let id = ((segment << 32) | row as u64) as usize;
                    let score = score as f64;
                    best.entry(id)
                        .and_modify(|s| {
                            if score > *s {
                                *s = score
                            }
                        })
                        .or_insert(score);
                }
            }
            best
        })();
        // Multi-vector: one `MultiVectorIndex` per segment, MaxSim + inner
        // top-n, merged across segments (the reference's handler shape).
        let mv_scores: HashMap<usize, f64> = (|| {
            if !is_tensor || bit_sparse {
                return HashMap::new();
            }
            let Some(decl) = types.get(col_idx).map(|t| t.to_ascii_uppercase()) else {
                return HashMap::new();
            };
            if !decl.starts_with("MULTIVECTOR") {
                return HashMap::new();
            }
            let Some(dims) = type_dim(&decl) else {
                return HashMap::new();
            };
            if dims == 0 {
                return HashMap::new();
            }
            let q_flat: Vec<f32> = query_items
                .iter()
                .filter_map(|x| x.trim().parse::<f32>().ok())
                .collect();
            if q_flat.is_empty() || !q_flat.len().is_multiple_of(dims) {
                return HashMap::new();
            }
            let query: Vec<Vec<f32>> = q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
            // Exact MaxSim over the scanned rows. The persisted segments and
            // residual codes are built at write/seal time (`MultiVectorIndex`);
            // a scan never writes index files.
            let mut best: HashMap<usize, f64> = HashMap::new();
            for (seq, row) in &rows {
                let Some(Value::Str(v)) = row.get(col_idx) else {
                    continue;
                };
                if let Some(doc) = parse_multivector_doc(v, dims) {
                    best.insert(*seq, f64::from(maxsim_tokens(&query, &doc)));
                }
            }
            best
        })();
        // C++ `PhysicalKnnScan`: exact scoring/ranking of the scanned rows
        // (filters live in the filter-scan base; the index probe above is
        // `KnnCandidateScan`).
        let keep = |row: &[Value]| {
            pred.is_none_or(|p| p.eval_predicate(row))
                && in_filter.as_ref().is_none_or(|p| p.eval_predicate(row))
                && search_filter.as_ref().is_none_or(|p| p.eval_predicate(row))
        };
        let column_decl = types
            .get(col_idx)
            .map(|t| t.to_ascii_uppercase())
            .unwrap_or_default();
        let query_elem_arg = arg_str(m.args.get(1));
        let scan = knn_scan::PhysicalKnnScan {
            column_index: col_idx,
            column_type: &column_decl,
            is_sparse,
            is_tensor,
            is_bit_sparse: bit_sparse,
            query_items: &query_items,
            query_elem_type: query_elem_arg.as_deref(),
            topn,
            metric: &metric,
            threshold,
            rows: &rows,
            cache: cache.as_ref(),
            vector_candidates,
        };
        // Scoring only ever reads the vector column, so the rows it decoded are
        // projections of it. The result rows are read separately (all columns),
        // which is why this scan cannot reuse them.
        let state = scan.finish(scan.execute(&keep, &bmp_scores, &mv_scores)?);
        let use_sim = state.use_sim;
        let scored = state.scored;

        // The result rows are read from the blocks in one pass. The scoring pass
        // decoded only the vector column (a projection), so it cannot supply
        // them, and a point read per row took the journal/KV path at ~3 ms a
        // row - ten result rows spent 28 ms after the query had finished in
        // 0.2 ms.
        let mut by_id: HashMap<usize, Vec<Value>> = HashMap::new();
        let missing: Vec<u64> = scored
            .iter()
            .map(|(seq, _, _)| *seq as u64)
            .filter(|seq| !by_id.contains_key(&(*seq as usize)))
            .collect();
        if !missing.is_empty() {
            let set: std::collections::HashSet<usize> =
                missing.iter().map(|seq| *seq as usize).collect();
            // One pass over the blocks that hold any of the wanted rows, not a
            // point read per row: a point read took the journal/KV path and cost
            // ~3 ms per row on this table, so ten result rows spent 28 ms after
            // the query itself had finished in 0.2 ms.
            by_id.extend(self.read_blocks_candidates_values(table, &[], &set)?.1);
        }
        let num = |f: f64| {
            if bit_sparse {
                Value::Int(f as i64)
            } else {
                Value::Float(f)
            }
        };
        let mut out = Vec::new();
        for (seq, sim, dist) in scored {
            // A stale sequence id (a row re-keyed by DROP COLUMN or compacted)
            // has no stored row; emitting an empty row would add a phantom hit.
            let Some(mut row) = by_id.remove(&seq) else {
                continue;
            };
            row.push(Value::Int(seq as i64));
            row.push(num(sim));
            row.push(Value::Float(dist));
            row.push(num(if use_sim { sim } else { dist }));
            out.push(row);
        }
        Ok((out, hidden))
    }

    pub fn catalog(&self) -> &Catalog {
        &self.storage.catalog
    }

    /// The underlying row/meta store (exposed for tests and tooling that need to
    /// inspect the on-disk key space).
    pub fn store(&self) -> &std::sync::Arc<dyn MetaStore> {
        self.storage.catalog.store()
    }

    /// TRUE when `table` exists (the name is qualified with the session's
    /// database when unqualified).
    pub fn table_exists(&self, table: &str) -> bool {
        let db = self.db();
        let name = if table.contains('.') {
            table.to_ascii_lowercase()
        } else {
            format!("{db}.{}", table.to_ascii_lowercase())
        };
        matches!(self.storage.catalog.get_table_by_name(&name), Ok(Some(_)))
    }

    /// Number of live (non-deleted) rows in `table`.
    pub fn row_count(&self, table: &str) -> Result<usize> {
        let db = self.db();
        let name = if table.contains('.') {
            table.to_string()
        } else {
            format!("{db}.{table}")
        };
        let key = self.table_key(&name);
        let dirty = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|entry| entry.mirror_dirty)
            .unwrap_or(false);
        // The blocks are the table when a mirror exists and nothing has touched
        // it since it was written; counted from the same `version` files a scan
        // reads (an incremental counter misses an UPDATE's re-inserted rows).
        if !dirty && self.has_block_mirror(&name) {
            let rows = self.block_visible_row_count(&name);
            self.tables
                .lock()
                .unwrap()
                .entry(key)
                .or_default()
                .live_rows = Some(rows);
            return Ok(rows);
        }
        // A table the block format covers is counted from the blocks: they are
        // the only row copy there is.
        if self.has_block_layout(&name) {
            let rows = self.block_visible_row_count(&name);
            self.tables
                .lock()
                .unwrap()
                .entry(key)
                .or_default()
                .live_rows = Some(rows);
            return Ok(rows);
        }
        // No block layout means no row files either; the count is nothing.
        Ok(0)
    }

    /// The storage's lifecycle mode (C++ `Storage::GetStorageMode`).
    pub fn storage_mode(&self) -> StorageMode {
        self.storage.get_storage_mode()
    }

    /// Whether every column of `table` has a C++ block layout.
    ///
    /// A table it does not cover (a bit tensor array, say) has no block files at
    /// all, so its rows keep the journal as their home - the only tables that
    /// still write `row:` keys.
    pub(crate) fn has_block_layout(&self, table: &str) -> bool {
        match self.storage.catalog.get_table_by_name(table) {
            Ok(Some(meta)) => meta
                .types
                .iter()
                .all(|t| crate::blocks::column_type_of(t).is_some()),
            _ => false,
        }
    }

    /// Rejects a user write unless the storage is `kWritable`.
    ///
    /// The reference gates every write operator on the mode: an
    /// `kUnInitialized` storage is `UnrecoverableError("Uninitialized storage
    /// mode")`, anything but `kWritable` is
    /// `Status::InvalidNodeRole("Attempt to write on non-writable node")`
    /// (8007, e.g. `physical_insert_impl.cpp:117-128`).
    pub(crate) fn ensure_writable(&self) -> Result<()> {
        match self.storage_mode() {
            StorageMode::Writable => Ok(()),
            StorageMode::UnInitialized => {
                Err(Error::Unrecoverable("Uninitialized storage mode".into()))
            }
            _ => Err(Error::InvalidNodeRole(
                "Attempt to write on non-writable node".into(),
            )),
        }
    }

    /// Rejects a cleanup unless the storage is `kWritable` or `kReadable`
    /// (`background_process_impl.cpp:109`).
    pub(crate) fn ensure_cleanup_allowed(&self) -> Result<()> {
        match self.storage_mode() {
            StorageMode::UnInitialized => {
                Err(Error::Unrecoverable("Uninitialized storage mode".into()))
            }
            mode if mode.accepts_cleanup() => Ok(()),
            _ => Err(Error::InvalidNodeRole(
                "Attempt to write on non-writable node".into(),
            )),
        }
    }

    /// A checkpoint (`PhysicalFlush`). The reference only serves it from a
    /// writable node (`physical_flush_impl.cpp:42-52`).
    ///
    /// With `[storage] flush_dumps_mem_index = true` this also releases every
    /// memory index: after the checkpoint (which must finish first - a dump
    /// takes `flush_lock` internally, and the lock is not re-entrant) each
    /// index is sealed and published by its own `DumpMemIndex` transaction.
    /// That spends a commit ts per non-empty tail, which is why it is off by
    /// default: the reference's flush leaves the tails in memory.
    pub async fn flush(&self) -> Result<()> {
        self.checkpoint().await?;
        if self
            .context
            .flush_dumps_mem_index
            .load(std::sync::atomic::Ordering::Relaxed)
        {
            self.dump_all_memory_indexes().await;
            // The dumps wrote new chunk files; drop their mappings and reload
            // the readers so the released memory actually leaves RSS.
            self.drop_index_caches();
            self.reload_vector_indexes();
        }
        Ok(())
    }

    /// The checkpoint alone: `flush_storage` and nothing else.
    ///
    /// The periodic trigger uses this, not [`Self::flush`]: the reference's
    /// `PeriodicTriggerThread` only checkpoints, so
    /// `[storage] flush_dumps_mem_index` must not turn every interval into a
    /// dump of every index (each non-empty tail spends a commit ts).
    pub async fn checkpoint(&self) -> Result<()> {
        self.ensure_writable()?;
        self.flush_storage().await
    }

    /// Attaches a `BGTaskProcessor` (`Storage::dump_index_processor`). While
    /// attached, exceeding the memindex quota queues a real
    /// [`DumpMemIndexTask`] instead of dumping synchronously.
    pub fn attach_bg_task_processor(self: &Arc<Self>, processor: Arc<BGTaskProcessor>) {
        *self.self_weak.lock().unwrap() = Arc::downgrade(self);
        *self.bg_processor.lock().unwrap() = Some(processor);
    }

    /// Waits for every queued background dump to finish.
    pub fn drain_background_tasks(&self) {
        let processor = self.bg_processor.lock().unwrap().clone();
        if let Some(processor) = processor {
            processor.drain();
        }
    }

    /// Queues a [`CleanupTask`]. Returns `false` when no processor is attached
    /// or it is shutting down.
    pub fn submit_cleanup(&self) -> bool {
        self.submit_task(|engine| Box::new(CleanupTask { engine }))
    }

    /// Queues an [`OptimizeTask`]. Returns `false` when no processor is
    /// attached or it is shutting down.
    pub fn submit_optimize_indexes(&self) -> bool {
        self.submit_task(|engine| Box::new(OptimizeTask { engine }))
    }

    pub(crate) fn submit_task(
        &self,
        make: impl FnOnce(std::sync::Weak<Engine>) -> Box<dyn BgTask>,
    ) -> bool {
        let Some(processor) = self.bg_processor.lock().unwrap().clone() else {
            return false;
        };
        let engine = self.self_weak.lock().unwrap().clone();
        processor.submit(make(engine))
    }

    /// Whether a transaction committed since the last cleanup.
    ///
    /// C++ `CleanupPeriodicTrigger::CreateCleanupTask`
    /// (`periodic_trigger_impl.cpp:48-59`): the trigger only builds a cleanup
    /// task when the oldest alive timestamp moved past the last cleanup
    /// timestamp, so an idle server does not checkpoint on every tick.
    pub fn needs_cleanup(&self) -> bool {
        self.txn.current_ts()
            > self
                .context
                .last_cleanup_ts
                .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// Starts the background task processor and the periodic trigger thread.
    ///
    /// C++: `Storage::AdminToWriter` starts `BGTaskProcessor`, the compaction /
    /// optimization / dump-index processors and `PeriodicTriggerThread`
    /// (`src/storage/storage_impl.cpp:357-430`); the trigger thread then wakes
    /// every second and fires each trigger whose interval has elapsed
    /// (`periodic_trigger_thread_impl.cpp:49-74`).
    ///
    /// Only the CLI calls this: [`Engine::open_with_options`] stays
    /// deterministic for tests and embedded callers.
    ///
    /// `compact_interval` drives [`Self::notify_compact`], the reference's
    /// periodic compaction (`compaction_process_impl.cpp:86`): every database
    /// and table is visited, and the segments `NewCompactionAlg` picks are
    /// merged. `Duration::ZERO` disables a trigger.
    pub fn start_background_tasks(
        self: &Arc<Self>,
        checkpoint_interval: std::time::Duration,
        cleanup_interval: std::time::Duration,
        compact_interval: std::time::Duration,
        optimize_interval: std::time::Duration,
    ) {
        let processor = Arc::new(BGTaskProcessor::new(1));
        processor.start();
        self.attach_bg_task_processor(processor);
        self.attach_bottom_pool();
        // The bottom halves run on the shared blocking pool, one job per table
        // at a time (`Engine::submit_bottom`): the reference's fixed pool with
        // `crc32(table_id) % n` routing serialises any two tables that hash
        // alike, which is the wrong limit for a directory of 100k tables.
        // `Storage::Start` also starts the memory-index appender
        // (`storage_impl.cpp` / `mem_index_appender_impl.cpp:52`).
        // The trigger is a **task**, not a thread: every step it performs is an
        // `.await` or a pure enqueue, so between ticks it costs a future
        // instead of a parked OS thread. It runs on the engine's own runtime,
        // which is the same one the statements and the fan-outs use.
        let engine = Arc::downgrade(self);
        crate::scheduler::scheduler().spawn_task(async move {
            trigger_loop(
                engine,
                checkpoint_interval,
                cleanup_interval,
                compact_interval,
                optimize_interval,
            )
            .await
        });
    }

    /// `ChunkIndexMetaInfo` of the sealed secondary chunks: `(base row id,
    /// row count)` in dump order.
    #[allow(dead_code)]
    pub(crate) fn secondary_chunk_meta(&self, table: &str, column: &str) -> Vec<(u64, usize)> {
        let key = self.table_key(table);
        let id = self
            .secondary_index_ids_on_column(table, column)
            .into_iter()
            .next();
        let Some(id) = id else {
            return Vec::new();
        };
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|entry| entry.secondary.chunk_meta.get(&id))
            .cloned()
            .unwrap_or_default()
    }

    pub(crate) fn query_count(&self) -> u64 {
        self.context
            .query_count
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// One session variable's value; the names come from
    /// `PhysicalShow::ExecuteShowSessionVariable`. `None` for a variable the
    /// reference does not register.
    pub(crate) fn session_variable_value(&self, var: variables::SessionVariable) -> Option<String> {
        Some(match var {
            variables::SessionVariable::QueryCount => self.query_count().to_string(),
            variables::SessionVariable::TotalCommitCount => self.total_commit_count(),
            variables::SessionVariable::TotalRollbackCount => {
                self.txn.total_rollbacked_txn_count().to_string()
            }
            variables::SessionVariable::ConnectedTime => self.context.connected_at.clone(),
        })
    }

    pub(crate) fn total_commit_count(&self) -> String {
        self.txn.total_committed_txn_count().to_string()
    }

    /// One global variable's value; the names come from
    /// `PhysicalShow::ExecuteShowGlobalVariable`.
    ///
    /// Every value is read from the subsystem that owns it — the buffer cache,
    /// the transaction manager, the background processor, the query registry,
    /// `/proc` — so `SHOW VARIABLE` and `GET /variables/*` report engine state
    /// instead of a constant. The two variables this port cannot answer are
    /// `running_compact_task` / `running_optimize_task`: the reference runs them
    /// on the compaction/optimization processors, and this engine has one
    /// [`BGTaskProcessor`] and no compact/optimize processor at all, so no task
    /// of those kinds can be running.
    pub(crate) fn global_variable_value(&self, var: variables::GlobalVariable) -> Result<String> {
        use crate::variables::GlobalVariable as G;
        Ok(match var {
            G::QueryCount => self.query_count().to_string(),
            G::SessionCount => "1".to_string(),
            // Mapped bytes over the configured budget: the reference reports
            // what its buffer objects charge the pool, the port reports what
            // the mappings cover (nothing is copied out of them).
            G::BufferPoolUsage => format!(
                "{}/{}",
                self.buffer_used_bytes(),
                self.buffer_memory_limit()
            ),
            G::SchedulePolicy => "round robin".to_string(),
            G::NextTxnId => self.txn.current_transaction_id().to_string(),
            // C++ `BufferManager::BufferedObjectCount`: the reference counts
            // the payloads it holds; the port counts the block mappings it
            // shares (index chunks are mapped too, but they belong to the
            // index structures, not to this pool).
            G::BufferedObjectCount => self.context.block_maps.lock().unwrap().len().to_string(),
            // C++ `BufferManager::WaitingGCObjectCount`: a mapping is either in
            // use or gone, so nothing waits for the GC.
            G::UnusedBufferObjectCount => "0".to_string(),
            G::ActiveTxnCount => self.txn.active_txn_count().to_string(),
            G::CurrentTs => self.txn.current_ts().to_string(),
            G::TotalCommitCount => self.total_commit_count(),
            G::TotalRollbackCount => self.txn.total_rollbacked_txn_count().to_string(),
            G::ActiveWalFilename => self
                .txn
                .wal_path()
                .file_name()
                .map(|n| n.to_string_lossy().into_owned())
                .unwrap_or_default(),
            G::ProfileRecordCapacity => self.profile_capacity().to_string(),
            // C++ `BGTaskProcessor::RunningTaskCount` for the dump-index
            // processor. Both names report it: `bg_task_count` is the
            // aggregate (the reference sums its four processors) and
            // `running_bg_task` the dump-index one.
            G::BackgroundTaskCount | G::RunningBgTask => self.bg_task_count().to_string(),
            // No compaction/optimization processor exists in this port, so
            // neither can have a running task (see the doc comment).
            G::RunningCompactTask | G::RunningOptimizeTask => "0".to_string(),
            G::SystemMemoryUsage => process_rss_bytes().to_string(),
            G::OpenFileCount => open_file_count().to_string(),
            // C++ `SystemInfo::CPUUsage() * 100` (see `Self::cpu_usage`).
            G::CpuUsage => format!("{}", self.cpu_usage() * 100.0),
            G::JeProf => "on".to_string(),
            G::FollowerNum => "0".to_string(),
            G::ResultCache => self
                .context
                .config
                .lock()
                .unwrap()
                .get("result_cache")
                .cloned()
                .unwrap_or_else(|| "off".to_string()),
            G::CacheResultCapacity => self.reported_result_cache_capacity().to_string(),
            G::CacheResultNum => self.result_cache.lock().unwrap().len().to_string(),
            // C++ reads the same two counters from the buffer manager; both
            // names report `misses/requests` over the block mappings.
            G::MemoryCacheMiss | G::DiskCacheMiss => {
                use std::sync::atomic::Ordering;
                let misses = self.context.block_map_misses.load(Ordering::Relaxed);
                let hits = self.context.block_map_hits.load(Ordering::Relaxed);
                format!("{}/{}", misses, hits + misses)
            }
            G::EnableProfile => self
                .profile_enabled
                .load(std::sync::atomic::Ordering::Relaxed)
                .to_string(),
        })
    }

    /// Size of the finished-statement history (`profile_record_capacity`).
    pub(crate) fn profile_capacity(&self) -> usize {
        self.queries.lock().unwrap().capacity()
    }

    /// Process CPU usage since the previous call, as a fraction of one CPU.
    ///
    /// C++ `SystemInfo::CPUUsage` (`src/metrics/system_info_impl.cpp:335`)
    /// samples `utime + stime` (fields 14/15 of `/proc/self/stat`) and the
    /// machine's total CPU time 200 ms apart and returns
    /// `Δprocess / Δtotal * hardware_concurrency`. Blocking the caller for
    /// 200 ms is not acceptable for an accessor, so the denominator is the
    /// wall clock since the previous call instead:
    ///
    /// ```text
    /// cpu_usage = (Δticks / CLOCK_TCK) / Δseconds / online_cpus
    /// ```
    ///
    /// i.e. the share of the machine's CPUs this process used over the
    /// interval, on the same 0..1 scale. The first call has no previous sample
    /// and returns 0.
    pub(crate) fn cpu_usage(&self) -> f64 {
        let Some(ticks) = process_cpu_ticks() else {
            return 0.0;
        };
        let now = std::time::Instant::now();
        let mut sample = self.cpu_sample.lock().unwrap();
        let previous = sample.replace(CpuSample { ticks, at: now });
        let Some(previous) = previous else {
            return 0.0;
        };
        let elapsed = now.duration_since(previous.at).as_secs_f64();
        if elapsed <= 0.0 {
            return 0.0;
        }
        let cpu_seconds = ticks.saturating_sub(previous.ticks) as f64 / CLOCK_TICKS_PER_SEC;
        (cpu_seconds / elapsed / online_cpu_count() as f64).clamp(0.0, 1.0)
    }

    /// Every variable at `scope`, as `SHOW VARIABLES` lists it.
    ///
    /// C++: `PhysicalShow::ExecuteShowGlobalVariables` /
    /// `ExecuteShowSessionVariables` (`physical_show_*variables_impl.cpp`),
    /// which walk `VarUtil::global_name_map_` / `session_name_map_`. The rows
    /// are `(name, value)`; the reference's third column (a description) is
    /// not part of the HTTP envelope, which reads only the first two.
    ///
    /// Session scope is per-`Engine`: this engine serves one session, so its
    /// session counters are the engine's own `query_count` and transaction
    /// counters rather than a per-connection record.
    pub fn variables(&self, scope: Scope) -> Vec<(String, String)> {
        match scope {
            Scope::Global => {
                // C++ `ExecuteShowGlobalVariables` breaks out of the
                // `cache_result_capacity` arm when the cache is off
                // (`physical_show_global_variables_impl.cpp:56-60`), so the row
                // is absent rather than zero.
                let skip_capacity = !self.result_cache_active();
                variables::ALL_GLOBAL_NAMES
                    .iter()
                    .filter_map(|name| {
                        if skip_capacity && name.eq_ignore_ascii_case("cache_result_capacity") {
                            return None;
                        }
                        let var = variables::GlobalVariable::from_name(name)?;
                        let value = self.global_variable_value(var).ok()?;
                        Some(((*name).to_string(), value))
                    })
                    .collect()
            }
            Scope::Session => variables::ALL_SESSION_NAMES
                .iter()
                .filter_map(|name| {
                    let var = variables::SessionVariable::from_name(name)?;
                    let value = self.session_variable_value(var)?;
                    Some(((*name).to_string(), value))
                })
                .collect(),
        }
    }

    /// `SHOW BUFFERS`: one row per sealed block this process has mapped.
    ///
    /// C++: `PhysicalShow::ExecuteShowBuffer` (`physical_show_buffer_impl.cpp`)
    /// over `BufferManager::GetBufferObjectsInfo`. A reference row describes an
    /// owned buffer object; here the sealed blocks are mapped and shared, so a
    /// row describes one mapping: the block directory as the path, the bytes
    /// its files cover, and `Loaded`/`Mmap`/`data` for the state fields.
    pub fn buffer_objects(&self) -> Vec<storage::mapped_block::BufferObjectInfo> {
        use storage::mapped_block::{BufferObjectInfo, BufferStatus, BufferType, FileWorkerType};
        let root = self.blocks_root();
        let mut rows: Vec<BufferObjectInfo> = self
            .context
            .block_maps
            .lock()
            .unwrap()
            .values()
            .map(|block| BufferObjectInfo {
                object_path: block.dir(&root),
                buffered_status: BufferStatus::Loaded,
                buffered_type: BufferType::Mmap,
                file_type: FileWorkerType::DataFile,
                object_size: block.bytes(),
            })
            .collect();
        rows.sort_by(|a, b| a.object_path.cmp(&b.object_path));
        rows
    }

    /// `SHOW OBJECT <name>`: the mapped block whose path is `name`.
    pub fn buffer_object(&self, name: &str) -> Option<storage::mapped_block::BufferObjectInfo> {
        self.buffer_objects()
            .into_iter()
            .find(|row| row.object_path.to_string_lossy() == name)
    }

    /// `SHOW FILES`: every file under the engine directory with its size.
    ///
    /// C++: `PhysicalShow::ExecuteShowPersistenceFiles`
    /// (`physical_show_persistence_files_impl.cpp`) over
    /// `PersistenceManager::GetAllFiles`. The port keeps its files on the local
    /// disk and has no persistence manager, so the walk over the data directory
    /// is the equivalent source: each row is `(absolute path, size in bytes)`,
    /// sorted for a stable response.
    pub fn all_files(&self) -> Vec<(PathBuf, usize)> {
        let mut out = Vec::new();
        let mut stack = vec![self.dir.clone()];
        while let Some(dir) = stack.pop() {
            let Ok(entries) = std::fs::read_dir(&dir) else {
                continue;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                match entry.file_type() {
                    Ok(t) if t.is_dir() => stack.push(path),
                    Ok(t) if t.is_file() => {
                        let size = entry.metadata().map(|m| m.len() as usize).unwrap_or(0);
                        out.push((path, size));
                    }
                    _ => {}
                }
            }
        }
        out.sort();
        out
    }

    /// `SHOW MEMINDEX`: the in-memory part of each index, with its row count.
    ///
    /// C++: `PhysicalShow::ExecuteShowMemIndex`
    /// (`physical_show_mem_index_impl.cpp`) over
    /// `MemIndexTracer::GetMemIndexTracerInfo`. The reference lists its HNSW
    /// tracer only (and returns `NotSupport` under the new catalog); this port
    /// walks the catalog's indexes and reports the rows each index family keeps
    /// unsealed in memory, which is the same information for a superset of
    /// index types.
    pub fn memory_indexes(&self) -> Vec<diagnostics::MemoryIndexRow> {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return Vec::new();
        };
        /// The cached index of one column, cloned out of `tables` so it can be
        /// read once the table lock is released (a scan holds one of these
        /// guards while it waits for `tables`).
        enum Handle {
            Hnsw(Arc<Mutex<crate::hnsw_mem::HnswIndexInMem>>),
            Text(SharedTextIndex),
            Sparse(Arc<std::sync::RwLock<SparseIndex>>),
            Mv(Arc<std::sync::RwLock<MultiVectorIndex>>),
            Ivf(usize),
        }
        let mut out = Vec::new();
        for meta in indexes {
            let kind = meta.index_type.to_ascii_uppercase();
            let handle = {
                let tables = self.tables.lock().unwrap();
                let Some(entry) = tables.get(&meta.table) else {
                    continue;
                };
                match kind.as_str() {
                    "HNSW" => entry
                        .hnsw_mem
                        .get(&meta.id)
                        .map(|h| Handle::Hnsw(h.clone())),
                    "FULLTEXT" => entry.text.get(&meta.id).map(|t| Handle::Text(t.clone())),
                    "SPARSE" | "BMP" => entry
                        .sparse
                        .get(&meta.id)
                        .map(|s| Handle::Sparse(s.clone())),
                    "MULTIVECTOR" => entry.mv.get(&meta.id).map(|m| Handle::Mv(m.clone())),
                    "IVF" => entry
                        .ivf_mem
                        .get(&meta.id)
                        .map(|rows| Handle::Ivf(rows.len())),
                    _ => None,
                }
            };
            let (row_count, mem_used) = match handle {
                Some(Handle::Hnsw(h)) => {
                    let h = h.lock().unwrap();
                    (h.len(), h.mem_usage())
                }
                Some(Handle::Text(t)) => {
                    let t = t.read().unwrap();
                    // The rows still unsealed: the resident tail plus the one a
                    // seal is writing right now (it keeps answering from memory,
                    // so it must not disappear from the report mid-seal).
                    let inflight = t
                        .inflight
                        .as_ref()
                        .map(|inflight| inflight.mem.doc_count())
                        .unwrap_or(0);
                    (t.mem.doc_count() + inflight, t.mem.mem_used())
                }
                Some(Handle::Sparse(s)) => {
                    let s = s.read().unwrap();
                    (s.doc_count(), 0)
                }
                Some(Handle::Mv(m)) => {
                    let m = m.read().unwrap();
                    (m.doc_count(), 0)
                }
                Some(Handle::Ivf(n)) => (n, 0),
                None => (0, 0),
            };
            // A secondary index keeps no in-memory rows in this port: its rows
            // are written straight into the sealed chunk.
            if row_count == 0 && mem_used == 0 {
                continue;
            }
            out.push(diagnostics::MemoryIndexRow {
                table: meta.table.clone(),
                index: meta.name.clone(),
                index_type: kind,
                row_count,
                mem_used,
            });
        }
        out.sort_by(|a, b| (&a.table, &a.index).cmp(&(&b.table, &b.index)));
        out
    }

    /// Health of the full-text indexes of `table`: chunk count, unsealed tail,
    /// spill runs, rows a seal is writing right now, stale copies, and the last
    /// seal's duration ([`diagnostics::FulltextStats`]).
    ///
    /// This is the data a slow full-text query is diagnosed with. It reads the
    /// column under its **read** guard, which a seal never holds while writing,
    /// so asking for stats cannot be blocked behind an fsync.
    pub fn fulltext_stats(&self, table: &str) -> Vec<diagnostics::FulltextStats> {
        let key = self.table_key(table);
        // Clone the handles out of `tables`, then drop the guard: the index lock
        // is taken after it everywhere (see `dml::batch_gate`).
        // The stats name the column each index was built on, so the id-keyed
        // registry is joined with the catalog once.
        let by_id: std::collections::HashMap<u64, String> = self
            .storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .map(|im| (im.id, im.column))
            .collect();
        let handles: Vec<(String, SharedTextIndex)> = {
            let tables = self.tables.lock().unwrap();
            match tables.get(&key) {
                Some(entry) => entry
                    .text
                    .iter()
                    .filter_map(|(id, shared)| Some((by_id.get(id)?.clone(), shared.clone())))
                    .collect(),
                None => Vec::new(),
            }
        };
        let mut out: Vec<diagnostics::FulltextStats> = handles
            .into_iter()
            .map(|(column, shared)| {
                let index = shared.read().unwrap();
                diagnostics::FulltextStats {
                    table: table.to_string(),
                    index: column,
                    chunks: index.reader.segments.len(),
                    chunk_rows: index
                        .reader
                        .segments
                        .iter()
                        .map(|segment| segment.docs as usize)
                        .sum(),
                    tail_rows: index.mem.pending_docs(),
                    spill_runs: index.mem.runs.len(),
                    inflight_rows: index
                        .inflight
                        .as_ref()
                        .map(|inflight| inflight.mem.pending_docs())
                        .unwrap_or(0),
                    overridden_rows: index.overridden.len(),
                    tail_bytes: index.mem.mem_used(),
                    last_seal_us: index.last_seal_us,
                }
            })
            .collect();
        out.sort_by(|a, b| a.index.cmp(&b.index));
        out
    }

    /// `SHOW TRANSACTIONS`: the live transactions, ordered by id.
    ///
    /// C++: `PhysicalShow::ExecuteShowTransactions` over
    /// `NewTxnManager::GetTxnInfoArray`.
    pub fn active_transactions(&self) -> Vec<txn::TxnInfo> {
        self.txn.get_txn_info_array()
    }

    /// `SHOW TRANSACTION <id>`.
    ///
    /// C++: `NewTxnManager::GetTxnInfoByID`; `None` when no such transaction is
    /// live (the reference's `SessionNotFound`/not-found path).
    pub fn transaction_detail(&self, id: u64) -> Option<txn::TxnInfo> {
        self.txn.get_txn_info_by_id(id)
    }

    /// `SHOW QUERIES`: the statements executing right now.
    ///
    /// C++: `PhysicalShow::ExecuteShowQueries` over
    /// `SessionManager::QueryRecords`.
    pub fn running_queries(&self) -> Vec<diagnostics::QueryRow> {
        self.queries.lock().unwrap().running()
    }

    /// `SHOW QUERY <id>`: a running or just-finished statement.
    pub fn query_profile(&self, id: u64) -> Option<diagnostics::QueryRow> {
        self.queries.lock().unwrap().query(id)
    }

    /// `SHOW PROFILES`: the finished statements recorded while `profile` is on.
    ///
    /// C++: `PhysicalShow::ExecuteShowProfiles` over
    /// `NewCatalog::GetProfileRecords`. The reference keeps one row per query
    /// phase; this port records the statement's wall-clock duration, the only
    /// phase timing that exists here.
    pub fn profiles(&self) -> Vec<diagnostics::ProfileRow> {
        self.queries.lock().unwrap().profiles()
    }

    /// `Cleanup`: drops the files a previous drop or optimize made garbage,
    /// matching what the reference's cleanup command removes.
    ///
    /// **No checkpoint.** `PhysicalCommand::Execute` for `kCleanup` runs
    /// `new_txn->Cleanup()` and nothing else (`physical_command_impl.cpp:390`),
    /// so a cleanup is one transaction - one commit ts, one `CLEANUP` entry.
    /// Flushing here made it two (a checkpoint frame plus the cleanup entry),
    /// which shifted every later statement's ts by one checkpoint.
    pub async fn cleanup(&self) -> Result<()> {
        // The cleanup task runs in `kWritable` and `kReadable`
        // (`background_process_impl.cpp:109`); only `kAdmin` is rejected.
        self.ensure_cleanup_allowed()?;
        // `WalManager::Cleanup`: the sealed `wal.log.<ts>` segments whose
        // records the KV store holds are reclaimed here, not at a checkpoint.
        if let Ok(applied) = self.txn.applied_ts() {
            let _ = self.txn.drop_sealed_wal_up_to(applied);
        }
        self.reclaim_deprecated_segments()?;
        // The column files an `ALTER TABLE … DROP COLUMN` left behind: the
        // drop writes a marker per block and `Cleanup` removes the file, the
        // way the reference does (its marker stays).
        let _ = self.reclaim_dropped_block_columns();
        let cleaned = self.cleanup_index_chunks();
        // `BGTaskProcessor` records the cleanup timestamp so the trigger can
        // skip ticks with nothing new to reclaim
        // (`background_process_impl.cpp:126-138`, `last_cleanup_ts_`).
        self.context
            .last_cleanup_ts
            .store(self.txn.current_ts(), std::sync::atomic::Ordering::Relaxed);
        // `NewTxn::Cleanup` is a transaction: its commit writes one `CLEANUP`
        // command carrying the begin ts (`CleanupTxnStore::ToWalEntry`), which
        // the reference's replay ignores. Without it a directory that ran a
        // cleanup has one frame less than the reference's.
        self.txn.append_cleanup().await?;
        cleaned
    }
}

/// `bm25_param_k1` / `bm25_param_b`, validated the way
/// `bound_select_statement_impl.cpp:239-252` validates them before writing the
/// values into `BM25Params`. Defaults: k1 = 1.2, b = 0.75.
fn parse_bm25_k1_b(params: &str) -> Result<(f64, f64)> {
    let get = |name: &str| -> Option<f64> {
        params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case(name))
            .and_then(|(_, v)| v.trim().parse().ok())
    };
    let k1 = match get("bm25_param_k1") {
        Some(v) if v < 0.0 => {
            return Err(Error::Syntax(
                "bm25_param_k1 must be a non-negative float. default value: 1.2".into(),
            ))
        }
        Some(v) => v,
        None => 1.2,
    };
    let b = match get("bm25_param_b") {
        Some(v) if !(0.0..=1.0).contains(&v) => {
            return Err(Error::Syntax(
                "bm25_param_b must be in the range [0.0f, 1.0f]. default value: 0.75".into(),
            ))
        }
        Some(v) => v,
        None => 0.75,
    };
    Ok((k1, b))
}

/// The column a `MATCH TEXT` column spec names: `col`, `col^2`, `col@index`.
fn column_of(spec: &str) -> &str {
    spec.split('^')
        .next()
        .unwrap_or(spec)
        .split('@')
        .next()
        .unwrap_or(spec)
        .trim()
}

/// The field qualifiers (`col:term`) a query string names, in order.
///
/// Used to tell "the grammar did not parse" apart from "the grammar parsed but
/// named a column outside the searched ones": the first is a reason to fall back
/// to the plain-term path, the second must not be, because the fallback would
/// search `title:foo` as ordinary text.
fn query_field_qualifiers(query: &str) -> Vec<&str> {
    let mut out = Vec::new();
    for token in query.split(|c: char| c.is_whitespace() || c == '(' || c == ')') {
        let token = token.trim_matches('"');
        let Some((field, _)) = token.split_once(':') else {
            continue;
        };
        if field.is_empty()
            || !field.chars().all(|c| c.is_alphanumeric() || c == '_')
            || field.starts_with(|c: char| c.is_ascii_digit())
        {
            continue;
        }
        out.push(field);
    }
    out
}

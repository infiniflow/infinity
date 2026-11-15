use super::*;

/// Background `DumpMemIndexTask` (`src/storage/bg_task/bg_task.cppm`).
///
/// Submitted by [`Engine`] when the in-memory quota is exceeded while a
/// `BGTaskProcessor` is attached. It upgrades the weak engine handle when it
/// runs, so the dump sees every row buffered by the time the worker starts —
/// the reference's proposed-dump coalescing.
pub struct DumpMemIndexTask {
    pub(crate) engine: std::sync::Weak<Engine>,
    pub(crate) pending: Arc<std::sync::Mutex<MemIndexPending>>,
    /// Whether every tail must go, or only the largest one: an explicit quota
    /// check asks for everything, a write-triggered one takes the largest.
    pub(crate) drain_all: bool,
    /// The table whose batch raised the charge. A write-triggered dump picks
    /// its victim among **that** table's indexes: choosing across the engine
    /// let a table that merely held more bytes absorb every breach, and the
    /// table that had just grown never drained. `None` for an explicit check,
    /// which drains everything anyway.
    pub(crate) table: Option<String>,
}

/// Background `CleanupTask` (`src/storage/bg_task/bg_task.cppm:107-114`).
///
/// The trigger submits it once a transaction has committed since the last
/// cleanup ([`Engine::needs_cleanup`]); the task itself runs
/// [`Engine::cleanup`], which records the new timestamp. A failure is reported
/// rather than propagated: the reference calls `UnrecoverableError`, and the
/// port has no process-wide abort for a background task.
pub struct CleanupTask {
    pub(crate) engine: std::sync::Weak<Engine>,
}

/// Background `NotifyOptimizeTask` (`src/storage/bg_task/bg_task.cppm:136-143`),
/// the periodic index optimizer: `NewNotifyOptimize` runs
/// `txn->OptimizeAllIndexes()` (`optimization_process_impl.cpp:91-145`), which
/// [`Engine::optimize_indexes`] mirrors for the index mirrors it ports.
pub struct OptimizeTask {
    pub(crate) engine: std::sync::Weak<Engine>,
}

/// The reference's `PeriodicTriggerThread::Run`
/// (`periodic_trigger_thread_impl.cpp:49-74`): wake every second and fire each
/// trigger whose interval elapsed.
///
/// This is a **tokio task**, not a thread. Every step it performs is either an
/// `.await` (the checkpoint's WAL swap and block mirror) or a pure enqueue, so
/// the loop costs a future between ticks instead of a parked thread. The one
/// synchronous wait left is `notify_compact`, which walks the tables and takes
/// each one's compaction lock; it is called inside `block_in_place` so the
/// runtime worker it happens to run on hands its core to a replacement first.
///
/// The engine is upgraded per tick and dropped before the sleep, so the task
/// stops as soon as the last `Arc<Engine>` goes away instead of keeping the
/// engine alive through a strong handle.
pub(crate) async fn trigger_loop(
    engine: std::sync::Weak<Engine>,
    checkpoint_interval: std::time::Duration,
    cleanup_interval: std::time::Duration,
    compact_interval: std::time::Duration,
    optimize_interval: std::time::Duration,
) {
    pub(crate) const TICK: std::time::Duration = std::time::Duration::from_secs(1);
    let mut last_checkpoint = std::time::Instant::now();
    let mut last_cleanup = std::time::Instant::now();
    let mut last_compact = std::time::Instant::now();
    let mut last_optimize = std::time::Instant::now();
    loop {
        {
            let Some(engine) = engine.upgrade() else {
                return;
            };
            // `PeriodicTriggerThread::Run`: the checkpoint trigger lives on the
            // same 1s tick as the other three; a checkpoint rotates the WAL and
            // folds the memory blocks into the block mirror.
            if !checkpoint_interval.is_zero() && last_checkpoint.elapsed() >= checkpoint_interval {
                last_checkpoint = std::time::Instant::now();
                engine.checkpoint_wal_async().await;
                // A periodic checkpoint is not the operator's `FLUSH DATA`:
                // it never dumps the memory indexes, even with
                // `flush_dumps_mem_index` on.
                let _ = engine.checkpoint().await;
            }
            if !optimize_interval.is_zero() && last_optimize.elapsed() >= optimize_interval {
                last_optimize = std::time::Instant::now();
                engine.submit_optimize_indexes();
            }
            if !compact_interval.is_zero() && last_compact.elapsed() >= compact_interval {
                last_compact = std::time::Instant::now();
                // `CompactionProcessor::NewNotifyCompact`: every table, the
                // segments `NewCompactionAlg` picks. `notify_compact` is still a
                // synchronous walk (it takes each table's compaction lock), so
                // it runs with the worker's core handed off rather than parking
                // the tick.
                let engine = engine.clone();
                let _ = tokio::task::block_in_place(move || engine.notify_compact());
            }
            if !cleanup_interval.is_zero()
                && last_cleanup.elapsed() >= cleanup_interval
                && engine.needs_cleanup()
            {
                last_cleanup = std::time::Instant::now();
                engine.submit_cleanup();
            }
        }
        tokio::time::sleep(TICK).await;
    }
}

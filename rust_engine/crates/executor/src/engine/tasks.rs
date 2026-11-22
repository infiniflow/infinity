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

/// What one reclaim slot does, given the ledger and the two configured numbers.
///
/// Split out of [`trigger_loop`] so the priority rule is a value a test can
/// assert instead of a control-flow shape hidden in an async loop.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum ReclaimTick {
    /// The interval has not elapsed, or it is disabled (`0`).
    Skip,
    /// Over a waterline: drain until the ledger is back below `soft`.
    Pressure(u64),
    /// Under every waterline but a sweep is due: drain tables idle past `age`.
    Stale(u64),
}

/// The reclaim decision for one slot.
///
/// **Pressure wins, and both targets share the one cadence.** The write path
/// checks the quota on every statement, so the slot only has to catch what
/// writes cannot see: a table that stopped being written while holding memory.
/// That is the staleness sweep - but a slot that finds the engine over a
/// waterline must drain bytes first, because that is the case the operator can
/// feel, and it may have been reached with no writes at all (rows replayed, a
/// long import that just ended).
///
/// `due` is the caller's interval check; `pressure` is the ledger's level,
/// which must come from the read-only `Watermarks::pressure` (a timer may not
/// retune the waterlines).
pub(crate) fn reclaim_tick(
    due: bool,
    pressure: crate::engine::maintain::Pressure,
    soft: u64,
    idle_age: u64,
) -> ReclaimTick {
    if !due {
        return ReclaimTick::Skip;
    }
    match pressure {
        crate::engine::maintain::Pressure::Normal => ReclaimTick::Stale(idle_age),
        _ => ReclaimTick::Pressure(soft),
    }
}

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

/// A spann segment the checkpoint declined to rebuild on its own thread.
///
/// The checkpoint publishes deltas, which is `O(new rows)`. A rebuild is
/// `O(rows · log(rows/max) · dims)` - minutes on a large segment - so the two
/// states that force one (a chain at the layer cap, or a segment whose
/// published chain is mostly dead rows) are handed here instead: the old chain
/// keeps answering until the new one is installed, and the checkpoint does not
/// stall. `publish_index_chunks_forced` is the rebuild, and it runs on the
/// blocking pool so a query keeps its runtime worker.
pub struct SpannRebuildTask {
    pub(crate) engine: std::sync::Weak<Engine>,
    pub(crate) table: String,
    pub(crate) table_id: u64,
    pub(crate) index: String,
    /// The dump timestamp the new chunk's records carry.
    pub(crate) ts: u64,
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
    reclaim_interval: std::time::Duration,
) {
    pub(crate) const TICK: std::time::Duration = std::time::Duration::from_secs(1);
    let mut last_checkpoint = std::time::Instant::now();
    let mut last_cleanup = std::time::Instant::now();
    let mut last_compact = std::time::Instant::now();
    let mut last_optimize = std::time::Instant::now();
    // The one clock the reclaim loop runs on. Its cadence is
    // `unsealed_reclaim_interval`, **not** the tick: the sweep walks every
    // table, so running it once a second would pay that walk on every tick for
    // an engine whose writes already check the quota themselves.
    let mut last_reclaim = std::time::Instant::now();
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
            // The memory ceiling is checked on the tick, **not** behind
            // `needs_cleanup`: a read-only server grows its page cache without a
            // single transaction, and that process is the one an OOM killer
            // finds. Every lever here is `madvise`/`malloc_trim` - no I/O, no
            // lock on a hot path - so it is safe to run on the tick itself.
            engine.memory_tick();
            if !optimize_interval.is_zero() && last_optimize.elapsed() >= optimize_interval {
                last_optimize = std::time::Instant::now();
                engine.submit_optimize_indexes();
            }
            if !compact_interval.is_zero() && last_compact.elapsed() >= compact_interval {
                last_compact = std::time::Instant::now();
                // `CompactionProcessor::NewNotifyCompact`: every table, the
                // segments `NewCompactionAlg` picks. Awaited like the
                // checkpoint above - bridging it with `block_in_place` parked
                // the tick's worker for the whole merge.
                let _ = engine.notify_compact().await;
            }
            if !cleanup_interval.is_zero()
                && last_cleanup.elapsed() >= cleanup_interval
                && engine.needs_cleanup()
            {
                last_cleanup = std::time::Instant::now();
                engine.submit_cleanup();
            }
            // Periodic reclaim of unsealed memory (`docs/MEMORY_RECLAIM.md`
            // §4.7). **One cadence for both targets.** The write path already
            // checks the byte quota on every statement, so the tick's job was
            // only ever the case "nobody is writing and memory stays held" -
            // which is the staleness sweep. Merging them means the sweep's
            // interval also bounds how long an over-waterline engine can go
            // unnoticed, and the per-tick `budget_bytes()` walk disappears.
            if !reclaim_interval.is_zero() && last_reclaim.elapsed() >= reclaim_interval {
                last_reclaim = std::time::Instant::now();
                let now_ms = crate::memory::now_millis();
                let (pressure, soft) = engine.unsealed_pressure();
                let idle_age = engine.unsealed_idle_age();
                match reclaim_tick(true, pressure, soft, idle_age) {
                    ReclaimTick::Skip => {}
                    ReclaimTick::Pressure(soft) => {
                        let _ = engine
                            .reclaim_unsealed(now_ms, crate::engine::reclaim::Target::Bytes(soft))
                            .await;
                    }
                    ReclaimTick::Stale(age) => {
                        let _ = engine
                            .reclaim_unsealed(now_ms, crate::engine::reclaim::Target::Stale(age))
                            .await;
                    }
                }
            }
            // The insert path takes a couple of queued splits per row and
            // leaves the rest of a bulk load's backlog behind it; this is the
            // pass that catches up. It is bounded, and a tail with nothing
            // queued costs one short read, so an idle engine does no work here.
            // Rebalancing a tail is pure CPU (it holds the tail's lock and
            // splits/merges its nodes), so it goes to the blocking pool rather
            // than parking a worker in place: `AGENTS.md` §7.8, and it keeps
            // the per-tick wait an `.await` like every other step here.
            if engine.has_spann_fixups() {
                let engine = engine.clone();
                let _ = tokio::task::spawn_blocking(move || {
                    engine.spann_drain_fixups(crate::spann_mem::BACKGROUND_FIXUP_BUDGET)
                })
                .await;
            }
        }
        tokio::time::sleep(TICK).await;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::engine::maintain::Pressure;

    /// The interval is the only cadence: a slot that is not due does nothing,
    /// whatever the ledger says.
    #[test]
    fn a_slot_that_is_not_due_does_nothing() {
        for pressure in [Pressure::Normal, Pressure::Soft, Pressure::Hard] {
            assert_eq!(reclaim_tick(false, pressure, 1024, 600), ReclaimTick::Skip);
        }
    }

    /// Over a waterline the slot drains bytes; under it, the slot is the
    /// staleness sweep. `soft` and `idle_age` are passed through unchanged, so
    /// the loop names the same number the ledger was judged by.
    #[test]
    fn pressure_takes_priority_over_the_staleness_sweep() {
        assert_eq!(
            reclaim_tick(true, Pressure::Soft, 4096, 600),
            ReclaimTick::Pressure(4096)
        );
        assert_eq!(
            reclaim_tick(true, Pressure::Hard, 4096, 600),
            ReclaimTick::Pressure(4096)
        );
        assert_eq!(
            reclaim_tick(true, Pressure::Normal, 4096, 600),
            ReclaimTick::Stale(600)
        );
    }

    /// `idle_age = 0` is still a sweep: "no age threshold" means every table
    /// with unsealed data is a candidate, not that the sweep is off. Disabling
    /// is the interval's job (`0`), which `due` already encodes.
    #[test]
    fn a_zero_idle_age_is_a_sweep_not_a_skip() {
        assert_eq!(
            reclaim_tick(true, Pressure::Normal, 4096, 0),
            ReclaimTick::Stale(0)
        );
    }
}

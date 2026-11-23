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
            // Reclaim of unsealed memory (`docs/MEMORY_RECLAIM.md` §4.7). The
            // **only** place a drain runs.
            //
            // Two things ask for it and they share one gate here:
            // * the interval, for a table that stopped being written while
            //   still holding memory (writes cannot see that case);
            // * the write path's flag, raised when a batch pushed the ledger
            //   over the quota.
            //
            // Short-circuiting both through this one slot is what keeps the
            // spacing between two drains at least one tick, whichever of the
            // two asked. `reclaim_unsealed` re-checks the target itself, so a
            // flag raised and then satisfied by another path costs one atomic
            // read.
            let due_interval =
                !reclaim_interval.is_zero() && last_reclaim.elapsed() >= reclaim_interval;
            let requested = engine
                .context
                .unsealed_reclaim_requested
                .swap(false, std::sync::atomic::Ordering::Relaxed);
            if due_interval || requested {
                last_reclaim = std::time::Instant::now();
                let _ = engine.reclaim_unsealed().await;
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

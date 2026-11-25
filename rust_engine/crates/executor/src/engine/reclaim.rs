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

//! Reclaim of unsealed memory (`docs/MEMORY_RECLAIM.md`).
//!
//! Both memory forms - the append buffer and every in-memory index tail - are
//! bounded by **one** budget, `[buffer] unsealed_memory_quota`. Crossing it
//! drains, and the drain is the same code whoever calls it:
//!
//! * the write path, right after a batch grew a table, and
//! * the periodic sweep, for tables nobody writes any more.
//!
//! That is deliberate. Two triggers but one implementation means the two cannot
//! disagree about what "over" means, which table pays, or when to stop; a second
//! copy would drift the first time one of them was tuned.
//!
//! The unit is a **table**. A round sorts every table holding unsealed memory by
//! two dimensions (size and age), fuses the ranks ([`rrf_order`]), and dumps
//! tables in that order until the ledger is back under **half the quota**.
//!
//! Age is measured in **commit timestamps**, not wall-clock milliseconds: a
//! table's idleness is the distance between the engine's current commit ts and
//! the ts of its last append. The ts sequence is allocated in commit order and
//! never rewinds, so it orders tables the way a clock would, while staying
//! meaningful across a restart - a wall clock would make every table look idle
//! the moment the process restarted and put them all at the head of the first
//! sweep.

use std::sync::atomic::Ordering;

use super::maintain::{rrf_order, ReclaimCandidate};
use crate::engine::state::Engine;

/// Milliseconds on a clock that never goes backwards, for retry deadlines.
///
/// The ranking's age is in commit ts on purpose (it has to survive a restart),
/// but a *retry gate* must not be: a failed round may not commit anything, so a
/// deadline expressed in commit ts is only reached when an unrelated statement
/// happens to advance the clock. An `Instant`-based deadline fires on time even
/// on an idle, failing server.
pub(crate) fn monotonic_now_ms() -> u64 {
    use std::sync::OnceLock;
    static START: OnceLock<std::time::Instant> = OnceLock::new();
    // Never 0: the value doubles as "an attempt was recorded", so a stamp taken
    // in the first millisecond of the process must not read as "never tried".
    (START
        .get_or_init(std::time::Instant::now)
        .elapsed()
        .as_millis() as u64)
        .max(1)
}

/// The monotonic deadline `wait` from now.
fn monotonic_deadline(wait: std::time::Duration) -> u64 {
    monotonic_now_ms().saturating_add(wait.as_millis() as u64)
}

/// The trigger loop's tick, in milliseconds. A drain only runs on a tick, so a
/// retry deadline shorter than one could not be observed.
const TICK_MS: u64 = 1000;

/// A table as the round's first pass sees it.
struct TableStat {
    key: String,
    /// Buffered rows plus index tails, for the size rank. Cached index bytes
    /// only affect *ordering*, never the decision to reclaim.
    footprint: u64,
    /// Commit ts of the table's last append, or 0 when it has none this process.
    last_ts: u64,
    /// When the loop last tried and failed, in **monotonic milliseconds**. The
    /// per-table retry gate compares it against the same clock, so a failed dump
    /// becomes due on time even when nothing commits in between.
    last_attempt_ts: u64,
    dump_failures: u32,
}

impl Engine {
    /// One reclaim round. Returns how many tables it dumped.
    ///
    /// Two clocks, each where it belongs. **Age** is the engine's commit
    /// timestamp - it orders appends and dumps already, survives a restart, and
    /// a test drives ageing by committing rows instead of sleeping. **Retry
    /// gates** are monotonic elapsed time, because a failed round may not commit
    /// anything at all and a commit-ts deadline would then never come due.
    ///
    /// The round runs only while the ledger is over **half the quota** and stops
    /// the moment it is back under. `budget_bytes()` is re-read before every
    /// dump: a dump may release nothing (an import in flight, another path got
    /// there first) and concurrent writes can push the total back up, so an
    /// incremental count would drift.
    pub(crate) async fn reclaim_unsealed(&self) -> usize {
        // `unsealed_memory_quota == 0` disables the budget (and, with it, this
        // loop): a zero target would otherwise read as "drain everything".
        if self.context.unsealed_memory_quota.load(Ordering::Relaxed) == 0 {
            return 0;
        }
        let _guard = self.context.unsealed_reclaim_lock.lock().await;
        // The retry gate is a monotonic deadline, not a commit ts: ageing the
        // ranking on commit ts is right, but no commit has to happen for a
        // backed-off round to become due again.
        if monotonic_now_ms() < self.context.unsealed_backoff_until.load(Ordering::Relaxed) {
            return 0;
        }
        let target = self.reclaim_target();
        if self.reclaim_target_met(target) {
            return 0;
        }
        self.reclaim_unsealed_until(false, || self.reclaim_target_met(target))
            .await
    }

    /// Reclaims unsealed tables (append buffers + in-memory index tails) under
    /// process RSS pressure, reusing the same RRF ranking and `dump_table` path
    /// until `process_rss_bytes() <= target_rss` or no eligible tables remain.
    pub(crate) async fn reclaim_unsealed_under_pressure(&self, target_rss: u64) -> usize {
        if self.budget_bytes() == 0 {
            return 0;
        }
        let _guard = self.context.unsealed_reclaim_lock.lock().await;
        if crate::engine::state::process_rss_bytes() <= target_rss {
            return 0;
        }
        self.reclaim_unsealed_until(true, || {
            storage::trim_allocator();
            crate::engine::state::process_rss_bytes() <= target_rss || self.budget_bytes() == 0
        })
        .await
    }

    async fn reclaim_unsealed_until<F>(&self, ignore_backoff: bool, mut stop: F) -> usize
    where
        F: FnMut() -> bool,
    {
        let now_ts = self.current_ts();
        let cap = self
            .context
            .unsealed_max_tables_per_round
            .load(Ordering::Relaxed);

        if stop() {
            return 0;
        }

        let stats = self.unsealed_table_stats();
        if stats.is_empty() {
            return 0;
        }
        let candidates: Vec<ReclaimCandidate> = stats
            .iter()
            .filter(|stat| {
                if ignore_backoff {
                    !self.import_in_flight(&stat.key) && self.table_has_unsealed(&stat.key)
                } else {
                    self.reclaim_candidate_eligible(stat)
                }
            })
            .map(|stat| ReclaimCandidate {
                key: stat.key.clone(),
                bytes: stat.footprint,
                // Pure commit-ts age: how long since the table last changed.
                // A failing table recedes through its own backoff (which keeps
                // it out of the candidate set), not by pretending it is young -
                // mixing a monotonic stamp in here would compare two clocks.
                age_ms: now_ts.saturating_sub(stat.last_ts),
            })
            .collect();
        if candidates.is_empty() {
            return 0;
        }
        let order = rrf_order(&candidates);

        let mut dumped = 0usize;
        let mut progressed = false;
        // `attempted` counts visits, `dumped` counts successes: the cap is on
        // the former, because a table whose dump releases nothing still spent
        // block I/O and an index catalog scan. Counting only successes let a
        // widespread storage failure walk every eligible table in one tick.
        for (attempted, at) in order.into_iter().enumerate() {
            let key = candidates[at].key.clone();
            if stop() {
                break;
            }
            if cap != 0 && attempted >= cap {
                break;
            }
            if self.dump_table(&key).await {
                progressed = true;
                dumped += 1;
            }
        }
        // A round that released nothing must not be repeated on the next slot: it
        // would walk the same hopeless candidates and pay one `list_indexes()`
        // per table, for ever.
        if !ignore_backoff {
            if progressed {
                self.context
                    .unsealed_backoff_until
                    .store(0, Ordering::Relaxed);
            } else {
                // A **monotonic deadline**, not a commit ts. A failed round may not
                // advance `current_ts()` at all (a block write that errors before
                // its commit), and a deadline expressed in commit ts would then
                // never be reached: once storage recovered, every later tick still
                // failed the gate until an unrelated commit moved the clock.
                //
                // The floor is the tick: a drain only runs on one, so a shorter
                // deadline could not be observed anyway. It also keeps an interval
                // of `0` (sweep off, write-triggered drains only) from retrying a
                // failure every tick.
                let interval = self
                    .context
                    .unsealed_reclaim_interval
                    .load(Ordering::Relaxed);
                let wait = std::time::Duration::from_millis(interval.max(TICK_MS));
                self.context
                    .unsealed_backoff_until
                    .store(monotonic_deadline(wait), Ordering::Relaxed);
            }
        }
        dumped
    }

    /// How long a failed table waits before it is a candidate again.
    ///
    /// Duration, not commit ts: the per-table backoff has the same problem the
    /// round's gate has - a table whose dump keeps failing may never see a
    /// commit, so a commit-ts deadline would only expire when some unrelated
    /// statement advanced the clock, instead of on time.
    fn table_backoff(&self, failures: u32) -> std::time::Duration {
        // The interval is the base; `2^failures` stretches it. The cap at 2^6
        // keeps the wait finite, while the tick floor keeps an interval of `0`
        // (sweep off) from turning into a per-tick retry.
        let base = self
            .context
            .unsealed_reclaim_interval
            .load(Ordering::Relaxed)
            .max(TICK_MS);
        std::time::Duration::from_millis(base.saturating_mul(1u64 << failures.min(6)))
    }

    /// The ledger level this round drains to: **half the quota**.
    ///
    /// Half, not the quota itself: with no headroom the next batch of writes
    /// crosses the line again immediately and the loop chases the write path
    /// forever.
    pub(crate) fn reclaim_target(&self) -> u64 {
        self.context.unsealed_memory_quota.load(Ordering::Relaxed) / 2
    }

    /// Whether the round's goal is reached.
    fn reclaim_target_met(&self, target: u64) -> bool {
        self.budget_bytes() as u64 <= target
    }

    /// Whether a table should enter this round's candidate set.
    fn reclaim_candidate_eligible(&self, stat: &TableStat) -> bool {
        // A table an `IMPORT` is still staging into is not reclaimable: its rows
        // are not committed, so a dump would publish a chunk over rows the table
        // may never have.
        if self.import_in_flight(&stat.key) {
            return false;
        }
        if !self.table_has_unsealed(&stat.key) {
            return false;
        }
        if stat.dump_failures == 0 {
            return true;
        }
        // No stamp means no attempt was recorded, so there is nothing to wait
        // for. Only reachable when the counter outlives its stamp (a table entry
        // rebuilt from the catalog); waiting from process start would keep the
        // table in memory for a failure this process never had.
        if stat.last_attempt_ts == 0 {
            return true;
        }
        // The table's own backoff. `last_attempt_ts` is a monotonic millisecond
        // stamp, so this fires on time on a failing server; the commit ts stays
        // in `age_ms`, where its meaning is "how long since the table last
        // changed" and a restart cannot reset it.
        let wait = self.table_backoff(stat.dump_failures).as_millis() as u64;
        monotonic_now_ms().saturating_sub(stat.last_attempt_ts) >= wait
    }

    /// One table's unsealed footprint snapshot, for every table.
    ///
    /// One `tables` critical section: the mark map and the buffer are both plain
    /// fields there. Index tails are read through the atomic marks rather than
    /// each index's own lock - see
    /// [`crate::engine::state::TableEntry::unsealed_marks`].
    fn unsealed_table_stats(&self) -> Vec<TableStat> {
        let tables = self.tables.lock().unwrap();
        tables
            .iter()
            .filter_map(|(key, entry)| {
                if entry.unsealed_bytes() == 0 && entry.unsealed_marks_are_empty() {
                    return None;
                }
                let marks: usize = entry
                    .unsealed_marks
                    .values()
                    .map(|mark| mark.load(Ordering::Relaxed) as usize)
                    .sum();
                let footprint = entry.unsealed_bytes()
                    + if marks > 0 {
                        // The cached figure is what ordering uses; fall back to
                        // the mark count when the cache has not caught up (a
                        // `CREATE INDEX` never refreshes it).
                        entry.memindex_bytes.max(marks)
                    } else {
                        entry.memindex_bytes
                    };
                // The newest append's commit ts is the table's last write.
                let last_ts = entry
                    .append_ts
                    .as_ref()
                    .and_then(|ts| ts.last().copied())
                    .unwrap_or(0);
                Some(TableStat {
                    key: key.clone(),
                    footprint: footprint as u64,
                    last_ts,
                    last_attempt_ts: entry.last_attempt_ts,
                    dump_failures: entry.dump_failures,
                })
            })
            .collect()
    }

    /// Whether one table has anything the loop could release: buffered rows or a
    /// non-zero index mark.
    pub(crate) fn table_has_unsealed(&self, key: &str) -> bool {
        let tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get(key) else {
            return false;
        };
        entry.unsealed_bytes() > 0 || !entry.unsealed_marks_are_empty()
    }

    /// Dumps one table's unsealed data. Returns whether it released anything.
    ///
    /// **Two phases, and the order matters.** The block write needs the table's
    /// bottom gate and the index dump needs `flush_lock`; the checkpoint takes
    /// `flush_lock` and *then* the gate (`admin.rs` -> `flush_table`). Doing both
    /// in one bottom half would make this thread hold the gate while waiting for
    /// `flush_lock` - the reverse of the checkpoint's order - which parks both
    /// without an error.
    ///
    /// The progress signal is [`Self::unsealed_live_of`] before and after:
    /// buffered rows plus the index bytes computed now. Neither is a cache, which
    /// matters because `memindex_bytes` is never refreshed by `CREATE INDEX` and
    /// would report "nothing released" for a table it just emptied.
    async fn dump_table(&self, key: &str) -> bool {
        let before = self.unsealed_live_of(key);
        // Phase 1: fold the buffered rows into the block files, under the gate.
        //
        // The bound is the table's own newest append ts, not `checkpoint_ts()`.
        // `checkpoint_ts()` is the last **completed** commit's ts, and the write
        // path calls this from inside the very commit that just allocated a
        // larger one - so a checkpoint bound would skip exactly the rows the
        // caller is trying to drain, and every write-triggered round would
        // report "released nothing" and back off. A batch of rows with
        // `ts <= latest append ts` is what this table owns; the gate we hold
        // keeps any later one from arriving while the write runs.
        let table = key.to_string();
        let bound_ts = self.reclaim_bound_ts(key);
        let work = table.clone();
        let _ = self
            .run_commit_bottom(key, move |engine: &Engine| {
                Box::pin(async move {
                    engine
                        .write_blocks_bounded_locked(&work, bound_ts)
                        .map(|_| ())
                })
            })
            .await;
        // Phase 2: seal every index tail, with no gate held.
        self.dump_every_table_index(Some(key)).await;
        // Only this table's charge, not a full reconciliation: the round's stop
        // check re-reads the ledger before every dump, and a round of 100 dumps
        // would otherwise walk every key (and read every index's state) 100
        // times. The checkpoint's `recompute_memindex_total` stays the safety
        // net for drift.
        self.refresh_memindex_charge(key);

        let after = self.unsealed_live_of(key);
        let released = after < before;
        let still_unsealed = self.table_has_unsealed(key);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(key) {
            if released {
                // Partial success must not be recorded as a failure: the next
                // round should come back to it, not back off from it.
                entry.last_attempt_ts = 0;
                entry.dump_failures = 0;
            } else {
                // A monotonic stamp: the per-table gate compares it against a
                // monotonic clock, so a failed dump retries when its wait is
                // over even if nothing commits in between.
                entry.last_attempt_ts = monotonic_now_ms();
                entry.dump_failures = entry.dump_failures.saturating_add(1);
            }
            let _ = still_unsealed;
        }
        released
    }

    /// The commit ts up to which this table's rows may be written out.
    ///
    /// `max(checkpoint_ts(), newest append ts)`. The first keeps the historical
    /// behaviour when the table has nothing newer; the second is what the write
    /// path needs, since it drains from inside a commit whose ts has not been
    /// published yet and would otherwise be excluded.
    fn reclaim_bound_ts(&self, key: &str) -> u64 {
        let newest = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(key)
                .and_then(|e| e.append_ts.as_ref().and_then(|ts| ts.last().copied()))
                .unwrap_or(0)
        };
        self.checkpoint_ts().max(newest)
    }

    /// `unsealed_bytes() + memindex_bytes_of(key)`, both read now.
    ///
    /// The plan calls this `unsealed_live`; the suffix is only there to keep it
    /// distinct from the `TableEntry` accessors it composes.
    fn unsealed_live_of(&self, key: &str) -> usize {
        let buffered = {
            let tables = self.tables.lock().unwrap();
            tables.get(key).map(|e| e.unsealed_bytes()).unwrap_or(0)
        };
        buffered + self.memindex_bytes_of(key)
    }
}

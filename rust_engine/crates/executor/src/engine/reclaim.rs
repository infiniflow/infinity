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

//! Periodic reclaim of unsealed memory (`docs/MEMORY_RECLAIM.md`).
//!
//! Both memory forms - the append buffer and every in-memory index tail - were
//! only checked on the **write** path. A table that stopped being written was
//! therefore never looked at again: its rows and its index tails stayed in
//! memory until the process ended. This module adds the sweep that closes that
//! gap.
//!
//! The unit is a **table**, not an index. A table's footprint is its buffered
//! rows plus its index tails, and one round sorts every table by two dimensions
//! (size and idleness), fuses the ranks ([`rrf_order`]), and dumps tables in
//! that order until the round's target is met.
//!
//! Two targets, one loop, **one cadence** (`unsealed_reclaim_interval`): every
//! slot reads the ledger once and picks one of these, pressure first.
//!
//! * `Target::Bytes(soft)`: the engine is over a waterline, so drain until
//!   `budget_bytes()` is below the soft line again.
//! * `Target::Stale(idle_age)`: drain tables whose last write is older than the
//!   threshold, however much memory the engine is using.

use std::sync::atomic::Ordering;

use super::maintain::{rrf_order, Pressure, ReclaimCandidate};
use crate::engine::state::Engine;

/// What one round is trying to achieve.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum Target {
    /// Drain until the engine-wide byte ledger is at or below this line.
    Bytes(u64),
    /// Drain every candidate whose age is at least this many milliseconds.
    Stale(u64),
}

/// A table as the round's first pass sees it.
struct TableStat {
    key: String,
    /// Buffered rows plus index tails, for the size rank. Cached index bytes
    /// only affect *ordering*, never the decision to reclaim.
    footprint: u64,
    /// When the table was last written, or the process start if never.
    last_write_ms: u64,
    /// When the loop last tried and failed, so a failing table's age is measured
    /// from the attempt rather than from the write (the pressure target only).
    last_attempt_ms: u64,
    dump_failures: u32,
}

impl Engine {
    /// One reclaim round. Returns how many tables it dumped.
    ///
    /// `now_ms` is the caller's monotonic clock reading, passed in so tests can
    /// drive ageing without sleeping. See `docs/MEMORY_RECLAIM.md` §4.
    pub(crate) async fn reclaim_unsealed(&self, now_ms: u64, target: Target) -> usize {
        if self.context.unsealed_backoff_until.load(Ordering::Relaxed) > now_ms {
            return 0;
        }
        let cap = self
            .context
            .unsealed_max_tables_per_round
            .load(Ordering::Relaxed);
        let idle_age = self.context.unsealed_idle_age.load(Ordering::Relaxed);
        // The backoff base: `idle_age == 0` means "no age threshold", and a zero
        // base would make `base * 2^n` zero and disable the backoff entirely, so
        // the interval is the floor. Both are intervals, so `max` is the honest
        // "how long to wait before retrying".
        let base = idle_age.max(
            self.context
                .unsealed_reclaim_interval
                .load(Ordering::Relaxed),
        );

        let stats = self.unsealed_table_stats();
        if stats.is_empty() {
            return 0;
        }
        let candidates: Vec<ReclaimCandidate> = stats
            .iter()
            .filter(|stat| self.reclaim_candidate_eligible(stat, now_ms, target, base))
            .map(|stat| ReclaimCandidate {
                key: stat.key.clone(),
                bytes: stat.footprint,
                age_ms: now_ms.saturating_sub(match target {
                    Target::Bytes(_) => stat.last_write_ms.max(stat.last_attempt_ms),
                    Target::Stale(_) => stat.last_write_ms,
                }),
            })
            .collect();
        if candidates.is_empty() {
            return 0;
        }
        let order = rrf_order(&candidates);

        let mut dumped = 0usize;
        let mut progressed = false;
        for at in order {
            let key = candidates[at].key.clone();
            if self.reclaim_target_met(&target) {
                break;
            }
            if self.dump_table(&key).await {
                progressed = true;
                dumped += 1;
                if cap != 0 && dumped >= cap {
                    break;
                }
            }
        }
        // A round that released nothing must not be repeated on the next tick:
        // every tick would then walk the same hopeless candidates and pay one
        // `list_indexes()` per table. One timer covers both targets because
        // "no progress" means the same thing to each.
        if progressed {
            self.context
                .unsealed_backoff_until
                .store(0, Ordering::Relaxed);
        } else {
            let interval = self
                .context
                .unsealed_reclaim_interval
                .load(Ordering::Relaxed);
            if interval > 0 {
                self.context
                    .unsealed_backoff_until
                    .store(now_ms.saturating_add(interval), Ordering::Relaxed);
            }
        }
        dumped
    }

    /// Whether a table should enter this round's candidate set.
    fn reclaim_candidate_eligible(
        &self,
        stat: &TableStat,
        now_ms: u64,
        target: Target,
        base: u64,
    ) -> bool {
        // A table an `IMPORT` is still staging into is not "idle"; dropping it
        // from the set is better than letting `dump_table` return `false` every
        // round and spending the round's budget on it.
        if self.import_in_flight(&stat.key) {
            return false;
        }
        if !self.table_has_unsealed(&stat.key) {
            return false;
        }
        match target {
            // Only tables that are actually old enough; a write resets the age,
            // so a table being written right now is naturally excluded.
            Target::Stale(idle) => now_ms.saturating_sub(stat.last_write_ms) >= idle,
            // The pressure path wants any table with data, including the one
            // that just caused the breach. Only a table that has failed before
            // waits, and it waits exponentially.
            Target::Bytes(_) => {
                if stat.dump_failures == 0 {
                    return true;
                }
                // Exponential backoff from the same `sort_base` the ranking
                // uses: a table that keeps failing must not hold a slot in every
                // round. The cap at 2^6 keeps the wait finite (about 10.7 h at
                // the defaults) instead of overflowing.
                let wait = base.saturating_mul(1u64 << stat.dump_failures.min(6));
                let since = now_ms.saturating_sub(stat.last_write_ms.max(stat.last_attempt_ms));
                since >= wait
            }
        }
    }

    /// Whether the round's goal is reached.
    fn reclaim_target_met(&self, target: &Target) -> bool {
        match target {
            // Re-read the ledger: a dump may have released nothing (an import in
            // flight, another path got there first) and concurrent writes can
            // push it back up, so an incremental count would drift.
            Target::Bytes(soft) => (self.budget_bytes() as u64) <= *soft,
            // The staleness sweep is a single ordered pass over the candidates it
            // selected by age; there is nothing left to satisfy.
            Target::Stale(_) => false,
        }
    }

    /// One table's unsealed footprint snapshot, for every table.
    ///
    /// One `tables` critical section: the mark map and the buffer are both
    /// plain fields there. Index tails are read through the atomic marks rather
    /// than each index's own lock - see [`crate::engine::state::TableEntry::unsealed_marks`].
    fn unsealed_table_stats(&self) -> Vec<TableStat> {
        let unsealed: Vec<usize> = {
            let tables = self.tables.lock().unwrap();
            tables
                .values()
                .map(|entry| entry.unsealed_bytes() + entry.memindex_bytes)
                .collect()
        };
        let _ = unsealed;
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
                Some(TableStat {
                    key: key.clone(),
                    footprint: footprint as u64,
                    last_write_ms: entry.last_write_ms,
                    last_attempt_ms: entry.last_attempt_ms,
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
    /// without an error. `dump_under_pressure` is already two-phase for this
    /// reason.
    ///
    /// The progress signal is [`crate::engine::state::TableEntry::unsealed_live`]
    /// before and after: buffered rows plus the index bytes computed now. Neither
    /// is a cache, which matters because `memindex_bytes` is never refreshed by
    /// `CREATE INDEX` and would report "nothing released" for a table it just
    /// emptied.
    async fn dump_table(&self, key: &str) -> bool {
        let before = self.unsealed_live_of(key);
        // Phase 1: fold the buffered rows into the block files, under the gate.
        let table = key.to_string();
        let checkpoint_ts = self.checkpoint_ts();
        let work = table.clone();
        let _ = self
            .run_commit_bottom(key, move |engine: &Engine| {
                Box::pin(async move {
                    engine
                        .write_blocks_bounded_locked(&work, checkpoint_ts)
                        .map(|_| ())
                })
            })
            .await;
        // Phase 2: seal every index tail, with no gate held.
        self.dump_every_table_index(Some(key)).await;
        self.recompute_memindex_total();

        let after = self.unsealed_live_of(key);
        let released = after < before;
        let still_unsealed = self.table_has_unsealed(key);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(key) {
            if released && !still_unsealed {
                // Fully drained: the table is no longer a candidate, and its
                // age restarts so the staleness sweep leaves it alone.
                entry.last_write_ms = crate::memory::now_millis();
                entry.last_attempt_ms = 0;
                entry.dump_failures = 0;
            } else if released {
                // Partial success must not be recorded as a failure: the next
                // round should come back to it, not back off from it.
                entry.last_attempt_ms = 0;
                entry.dump_failures = 0;
            } else {
                entry.last_attempt_ms = crate::memory::now_millis();
                entry.dump_failures = entry.dump_failures.saturating_add(1);
            }
        }
        released
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

    /// The soft waterline, read without retuning it.
    ///
    /// `seal_pressure()` calls `retune` first, which halves `soft` while the
    /// engine is above 80% of its budget. That is driven by writes and belongs
    /// there; a once-a-second timer that did the same would take 64 MiB to one
    /// byte in about six seconds and then demand draining the whole pool.
    pub(crate) fn unsealed_pressure(&self) -> (Pressure, u64) {
        let budget = self.budget_bytes() as u64;
        let marks = self.context.watermarks.lock().unwrap();
        (marks.pressure(budget), marks.soft())
    }
}

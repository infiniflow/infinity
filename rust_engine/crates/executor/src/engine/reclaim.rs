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
//! The unit is a **table**, not an index. One round sorts every table that holds
//! unsealed memory by two dimensions (size and idleness), fuses the ranks
//! ([`rrf_order`]), and dumps tables in that order until the ledger is back under
//! **half the quota**.
//!
//! There is deliberately **one** goal. An earlier version had two - "over a
//! waterline" and "older than an age" - but both asked the same question ("which
//! table should give memory back first?") and answered it with the same ranking;
//! only the stopping rule differed. One goal is simpler to reason about and one
//! rule to tune: the loop starts when the ledger passes half the quota and stops
//! once it is back under.

use std::sync::atomic::Ordering;

use super::maintain::{rrf_order, ReclaimCandidate};
use crate::engine::state::Engine;

/// A table as the round's first pass sees it.
struct TableStat {
    key: String,
    /// Buffered rows plus index tails, for the size rank. Cached index bytes
    /// only affect *ordering*, never the decision to reclaim.
    footprint: u64,
    /// When the table was last written, or the process start if never.
    last_write_ms: u64,
    /// When the loop last tried and failed. The age the ranking uses is measured
    /// from `max(last_write_ms, last_attempt_ms)`, so a table that keeps failing
    /// drifts to the back instead of sitting at the front for ever.
    last_attempt_ms: u64,
    dump_failures: u32,
}

impl Engine {
    /// One reclaim round. Returns how many tables it dumped.
    ///
    /// `now_ms` is the caller's monotonic clock reading, passed in so tests can
    /// drive ageing without sleeping. See `docs/MEMORY_RECLAIM.md` §4.
    ///
    /// The round runs only while the ledger is **over half the quota** and stops
    /// the moment it is back under. `budget_bytes()` is re-read before every
    /// dump: a dump may release nothing (an import in flight, another path got
    /// there first) and concurrent writes can push the total back up, so an
    /// incremental count would drift.
    pub(crate) async fn reclaim_unsealed(&self, now_ms: u64) -> usize {
        // `memindex_quota == 0` disables the byte quota (and, with it, this
        // loop): a zero target would otherwise read as "drain everything".
        if self.context.memindex_quota.load(Ordering::Relaxed) == 0 {
            return 0;
        }
        if self.context.unsealed_backoff_until.load(Ordering::Relaxed) > now_ms {
            return 0;
        }
        let cap = self
            .context
            .unsealed_max_tables_per_round
            .load(Ordering::Relaxed);
        // The backoff base is the interval: `idle_age` is gone, and a zero base
        // would make `base * 2^n` zero and disable the backoff entirely.
        let base = self
            .context
            .unsealed_reclaim_interval
            .load(Ordering::Relaxed)
            .max(1);

        let target = self.reclaim_target();
        if self.reclaim_target_met(target) {
            return 0;
        }

        let stats = self.unsealed_table_stats();
        if stats.is_empty() {
            return 0;
        }
        let candidates: Vec<ReclaimCandidate> = stats
            .iter()
            .filter(|stat| self.reclaim_candidate_eligible(stat, now_ms, base))
            .map(|stat| ReclaimCandidate {
                key: stat.key.clone(),
                bytes: stat.footprint,
                // Age counts from the last write *or* the last attempt, so a
                // table that keeps failing recedes instead of camping at the
                // head of the order.
                age_ms: now_ms.saturating_sub(stat.last_write_ms.max(stat.last_attempt_ms)),
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
            if self.reclaim_target_met(target) {
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
        // A round that released nothing must not be repeated on the next slot:
        // it would walk the same hopeless candidates and pay one `list_indexes()`
        // per table, for ever.
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

    /// The ledger level this round drains to: **half the quota**.
    ///
    /// One number, not two. "Over the soft waterline" and "older than an age"
    /// both meant "give memory back now", and the ranking already decided which
    /// table pays; a second threshold only added a way for the two to disagree.
    /// Half the quota is comfortably below the write path's own `soft` line, so a
    /// round leaves headroom instead of immediately tripping the quota again.
    ///
    /// `memindex_quota == 0` disables the byte quota mechanism entirely, and with
    /// it this loop.
    pub(crate) fn reclaim_target(&self) -> u64 {
        self.context.memindex_quota.load(Ordering::Relaxed) / 2
    }

    /// Whether the round's goal is reached.
    fn reclaim_target_met(&self, target: u64) -> bool {
        self.budget_bytes() as u64 <= target
    }

    /// Whether a table should enter this round's candidate set.
    fn reclaim_candidate_eligible(&self, stat: &TableStat, now_ms: u64, base: u64) -> bool {
        // A table an `IMPORT` is still staging into is not reclaimable: its rows
        // are not committed, so a dump would publish a chunk over rows the table
        // may never have. Dropping it from the set is better than letting
        // `dump_table` return `false` every round and spending the round on it.
        if self.import_in_flight(&stat.key) {
            return false;
        }
        if !self.table_has_unsealed(&stat.key) {
            return false;
        }
        if stat.dump_failures == 0 {
            return true;
        }
        // Exponential backoff from the same `sort_base` the ranking uses: a table
        // that keeps failing must not hold a slot in every round. The cap at 2^6
        // keeps the wait finite instead of overflowing.
        let wait = base.saturating_mul(1u64 << stat.dump_failures.min(6));
        let since = now_ms.saturating_sub(stat.last_write_ms.max(stat.last_attempt_ms));
        since >= wait
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
    /// The progress signal is [`Self::unsealed_live_of`] before and after:
    /// buffered rows plus the index bytes computed now. Neither is a cache,
    /// which matters because `memindex_bytes` is never refreshed by
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
}

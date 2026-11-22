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

/// A table as the round's first pass sees it.
struct TableStat {
    key: String,
    /// Buffered rows plus index tails, for the size rank. Cached index bytes
    /// only affect *ordering*, never the decision to reclaim.
    footprint: u64,
    /// Commit ts of the table's last append, or 0 when it has none this process.
    last_ts: u64,
    /// When the loop last tried and failed, in commit ts. The age the ranking
    /// uses is measured from `max(last_ts, last_attempt_ts)`, so a table that
    /// keeps failing drifts to the back instead of sitting at the front for ever.
    last_attempt_ts: u64,
    dump_failures: u32,
}

impl Engine {
    /// One reclaim round. Returns how many tables it dumped.
    ///
    /// The clock is the engine's own commit timestamp: there is no `now` to pass
    /// in and no wall clock to read. `current_ts()` is the order appends and
    /// dumps already use, and a test drives ageing by committing rows rather
    /// than by sleeping.
    ///
    /// The round runs only while the ledger is over **half the quota** and stops
    /// the moment it is back under. `budget_bytes()` is re-read before every
    /// dump: a dump may release nothing (an import in flight, another path got
    /// there first) and concurrent writes can push the total back up, so an
    /// incremental count would drift.
    pub(crate) async fn reclaim_unsealed(&self) -> usize {
        // `unsealed_memory_quota == 0` disables the budget (and, with it, this
        // loop): a zero target would otherwise read as "drain everything".
        if self.context.memindex_quota.load(Ordering::Relaxed) == 0 {
            return 0;
        }
        let now_ts = self.current_ts();
        if self.context.unsealed_backoff_until.load(Ordering::Relaxed) > now_ts {
            return 0;
        }
        let cap = self
            .context
            .unsealed_max_tables_per_round
            .load(Ordering::Relaxed);
        let base = self.backoff_base_ts();

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
            .filter(|stat| self.reclaim_candidate_eligible(stat, now_ts, base))
            .map(|stat| ReclaimCandidate {
                key: stat.key.clone(),
                bytes: stat.footprint,
                // Age counts from the last append *or* the last attempt, so a
                // table that keeps failing recedes instead of camping at the
                // head of the order.
                age_ms: now_ts.saturating_sub(stat.last_ts.max(stat.last_attempt_ts)),
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
        // A round that released nothing must not be repeated on the next slot: it
        // would walk the same hopeless candidates and pay one `list_indexes()`
        // per table, for ever.
        if progressed {
            self.context
                .unsealed_backoff_until
                .store(0, Ordering::Relaxed);
        } else {
            self.context
                .unsealed_backoff_until
                .store(now_ts.saturating_add(base), Ordering::Relaxed);
        }
        dumped
    }

    /// How many commit ts a failed round waits before trying again.
    ///
    /// `unsealed_reclaim_interval` is a duration and the age is in commit ts, so
    /// the two cannot be compared directly. The conversion here is one commit per
    /// interval minute, which makes the base a **commit count** - the unit the
    /// ranking actually uses. An interval of `0` (sweep off) falls back to 1 so a
    /// failed write-triggered round still backs off instead of spinning.
    fn backoff_base_ts(&self) -> u64 {
        let minutes = self
            .context
            .unsealed_reclaim_interval
            .load(Ordering::Relaxed)
            / 60_000;
        minutes.max(1)
    }

    /// The ledger level this round drains to: **half the quota**.
    ///
    /// Half, not the quota itself: with no headroom the next batch of writes
    /// crosses the line again immediately and the loop chases the write path
    /// forever.
    pub(crate) fn reclaim_target(&self) -> u64 {
        self.context.memindex_quota.load(Ordering::Relaxed) / 2
    }

    /// Whether the round's goal is reached.
    fn reclaim_target_met(&self, target: u64) -> bool {
        self.budget_bytes() as u64 <= target
    }

    /// Whether a table should enter this round's candidate set.
    fn reclaim_candidate_eligible(&self, stat: &TableStat, now_ts: u64, base: u64) -> bool {
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
        // Exponential backoff from the same base the ranking uses: a table that
        // keeps failing must not hold a slot in every round. The cap at 2^6 keeps
        // the wait finite instead of overflowing.
        let wait = base.saturating_mul(1u64 << stat.dump_failures.min(6));
        let since = now_ts.saturating_sub(stat.last_ts.max(stat.last_attempt_ts));
        since >= wait
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
        self.recompute_memindex_total();

        let after = self.unsealed_live_of(key);
        let released = after < before;
        let still_unsealed = self.table_has_unsealed(key);
        let now_ts = self.current_ts();
        if let Some(entry) = self.tables.lock().unwrap().get_mut(key) {
            if released {
                // Partial success must not be recorded as a failure: the next
                // round should come back to it, not back off from it.
                entry.last_attempt_ts = 0;
                entry.dump_failures = 0;
            } else {
                entry.last_attempt_ts = now_ts;
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

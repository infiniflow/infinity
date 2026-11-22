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

//! Index maintenance accounting and scheduling (`SPANN_DESIGN.md` §9.5, §9.6).
//!
//! Two pieces live here, both of them **pure policy**: they read numbers the
//! engine already keeps and decide *when* to seal and *which* tail to seal.
//! The sealing itself (a dump transaction) stays where it was, so this module
//! adds no locks around file work.
//!
//! * [`Watermarks`] turns the one byte ledger (`memindex_quota` against
//!   `memindex_total` plus the unsealed rows) into the three-level pressure of
//!   §9.5: below `soft` nothing happens, above `soft` a background seal is
//!   proposed, above `hard` the writing thread seals inline. That is the "one
//!   ledger" of §12.1 - rows (`mem_index_capacity`) no longer decide *when* to
//!   dump, they only bound how much a single index keeps.
//! * [`rrf_order`] merges the two ranks the periodic reclaim loop needs - size
//!   and age - into one order. The write path does not use it: a breach names
//!   one table, so it only has to choose among that table's indexes, which is
//!   `pick_largest_index`.

/// Which waterline the pending bytes are above (§9.5).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum Pressure {
    Normal,
    /// `tail_bytes > soft`: queue a seal behind the writers.
    Soft,
    /// `tail_bytes > hard`: the writing thread seals before returning.
    Hard,
}

/// `soft = min(64 MiB, budget/4)`, `hard = min(256 MiB, budget/2)` (§9.5), with
/// an AIMD trim on `soft` while the engine sits above 80% of its budget.
///
/// The budget is the atomic the quota check already reads (`memindex_quota`);
/// this type only derives the two lower waterlines from it, instead of every
/// caller inventing its own threshold.
#[derive(Debug)]
pub(crate) struct Watermarks {
    budget: u64,
    soft: u64,
    hard: u64,
}

const SOFT_CAP: u64 = 64 << 20;
const HARD_CAP: u64 = 256 << 20;

impl Watermarks {
    pub(crate) fn new(budget: u64) -> Self {
        let mut out = Self {
            budget: 0,
            soft: 0,
            hard: 0,
        };
        out.set_budget(budget);
        out
    }

    /// Recomputes the waterlines for a new budget. `budget == 0` disables the
    /// mechanism, exactly like `memindex_quota == 0` does.
    pub(crate) fn set_budget(&mut self, budget: u64) {
        self.budget = budget;
        if budget == 0 {
            self.soft = 0;
            self.hard = 0;
            return;
        }
        // Both lines are fractions of the budget (§9.5), so a tiny budget gets
        // tiny lines instead of collapsing onto the budget itself - a floor here
        // would make `soft == budget` and disable the soft level entirely.
        let base = (budget / 4).clamp(1, SOFT_CAP);
        self.hard = (budget / 2).clamp(1, HARD_CAP).max(base);
        self.soft = base;
    }

    pub(crate) fn budget(&self) -> u64 {
        self.budget
    }

    /// The lower two waterlines. `soft` is read by the periodic reclaim loop
    /// to name its byte target, so it is not test-only.
    pub(crate) fn soft(&self) -> u64 {
        self.soft
    }

    /// The hard line, only the tests pin it: production reads it through
    /// [`Self::level`], which is what the waterlines exist to feed.
    #[cfg(test)]
    pub(crate) fn hard(&self) -> u64 {
        self.hard
    }

    /// The level `pending` bytes sit at, **without** retuning the waterlines.
    ///
    /// The write path uses [`Self::level`] through `Engine::seal_pressure`,
    /// which calls [`Self::retune`] first: the AIMD halving is driven by writes,
    /// so its frequency is the write frequency. The periodic loop runs every
    /// second, and calling `retune` there would halve `soft` once a second while
    /// the engine stayed above 80% with no writes at all - 64 MiB down to one
    /// byte in about six seconds, after which the byte target would demand
    /// draining the whole pool. A read-only level check is what a timer may do.
    pub(crate) fn pressure(&self, pending: u64) -> Pressure {
        self.level(pending)
    }

    /// The level `pending` bytes sit at.
    pub(crate) fn level(&self, pending: u64) -> Pressure {
        if self.budget == 0 {
            return Pressure::Normal;
        }
        if pending > self.hard {
            Pressure::Hard
        } else if pending > self.soft {
            Pressure::Soft
        } else {
            Pressure::Normal
        }
    }

    /// AIMD on `soft` (§9.5 row 3): above 80% of the budget, halve it (down to
    /// one mebibyte); below half the budget, restore it. `hard` follows the
    /// budget only - it is the memory-safety line, not a tuning knob.
    pub(crate) fn retune(&mut self, pending: u64) {
        if self.budget == 0 {
            return;
        }
        let base = (self.budget / 4).clamp(1, SOFT_CAP);
        let over = pending as f64 > self.budget as f64 * 0.8;
        let under = (pending as f64) < self.budget as f64 / 2.0;
        if over {
            self.soft = (self.soft / 2).max(1);
        } else if under || self.soft == 0 {
            self.soft = base;
        }
        self.soft = self.soft.clamp(1, self.hard.max(1));
    }
}

/// One table the periodic reclaim loop may dump, as the fusion sees it.
#[derive(Clone, Debug)]
pub(crate) struct ReclaimCandidate {
    /// `db.table`, the identity the winner is reported by.
    pub key: String,
    /// Unsealed bytes right now: the size dimension.
    pub bytes: u64,
    /// How long the table has been idle: the age dimension.
    pub age_ms: u64,
}

/// Reciprocal-rank fusion of the size and age ranks (`K = 3`).
///
/// The two dimensions are **not** summed: bytes scale with table size while age
/// is a duration, so any weighted sum needs a coefficient that only makes sense
/// for one deployment. Ranks do not: `score = 1/(K + size_rank) +
/// 1/(K + age_rank)`.
///
/// `K = 3` so age dominates. The loop's purpose is that a table nobody writes
/// stops holding memory (§1 gap A). A large table that is still growing is
/// visible to the pressure path on every tick; an idle one has no other path
/// that would ever look at it. With `K = 3` the ranks score 0.500/0.269/0.269/
/// 0.154, so being the oldest is worth as much as being both biggest and
/// second-oldest - and rank 1 on either axis beats rank 2 on both, which is
/// what keeps a huge inactive table from being buried under a merely busy one.
/// (`K = 60`, RRF's usual value, flattens this to 0.0328/0.0255/0.0255/0.0286
/// and lets size win outright.)
///
/// **Ties break toward age, not size.** The fused score is symmetric: a table
/// ranked `(1, 4)` scores exactly what one ranked `(4, 1)` does, because
/// `1/(K+a) + 1/(K+b)` does not care which axis produced which rank. So *every*
/// "biggest but newest" / "smallest but oldest" pair ties, and a byte-descending
/// tie-break would then hand the pair to the big table - reversing the very
/// preference `K = 3` exists to express, and letting a busy large table push an
/// idle small one past the round's cap. Age descending is therefore the first
/// tie-break, bytes descending the second, and the key the last, so the order is
/// still a pure function of the inputs and a test can assert *which* table leads
/// instead of relying on sort stability.
pub(crate) fn rrf_order(candidates: &[ReclaimCandidate]) -> Vec<usize> {
    const K: usize = 3;
    if candidates.is_empty() {
        return Vec::new();
    }
    let mut by_size: Vec<usize> = (0..candidates.len()).collect();
    by_size.sort_by(|a, b| {
        candidates[*b]
            .bytes
            .cmp(&candidates[*a].bytes)
            .then_with(|| candidates[*a].key.cmp(&candidates[*b].key))
    });
    let mut by_age: Vec<usize> = (0..candidates.len()).collect();
    by_age.sort_by(|a, b| {
        candidates[*b]
            .age_ms
            .cmp(&candidates[*a].age_ms)
            .then_with(|| candidates[*a].key.cmp(&candidates[*b].key))
    });
    let mut size_rank = vec![0usize; candidates.len()];
    for (rank, at) in by_size.iter().enumerate() {
        size_rank[*at] = rank + 1;
    }
    let mut age_rank = vec![0usize; candidates.len()];
    for (rank, at) in by_age.iter().enumerate() {
        age_rank[*at] = rank + 1;
    }
    // `1/a + 1/b` is `(a + b) / (a * b)` with `a = K + size_rank`,
    // `b = K + age_rank`. Comparing two such scores by cross-multiplication is
    // exact integer arithmetic, so no rounding can reorder a pair.
    let score = |i: usize| -> (u128, u128) {
        let a = (K + size_rank[i]) as u128;
        let b = (K + age_rank[i]) as u128;
        (a + b, a * b)
    };
    let mut order: Vec<usize> = (0..candidates.len()).collect();
    order.sort_by(|x, y| {
        let (num_x, den_x) = score(*x);
        let (num_y, den_y) = score(*y);
        (num_y * den_x)
            .cmp(&(num_x * den_y))
            // Symmetric scores: prefer the older table (see the doc comment).
            .then_with(|| candidates[*y].age_ms.cmp(&candidates[*x].age_ms))
            .then_with(|| candidates[*y].bytes.cmp(&candidates[*x].bytes))
            .then_with(|| candidates[*x].key.cmp(&candidates[*y].key))
    });
    order
}

/// The index a write-path breach should dump: the one holding the most bytes,
/// with ties going to whichever was dumped longest ago.
///
/// The write path names one table, so the candidate set is that table's indexes
/// and there is nothing to be fair *between* - the earlier per-tenant scheduler
/// degenerated to pure rotation here, because a breach is always raised by one
/// table and its candidate set therefore has one tenant. Bytes descending is
/// what actually releases the most per dump; the `last_dump` tie-break is what
/// keeps two equally sized indexes from starving each other (without it a
/// strict `bytes > seen` picks the first of the pair on every breach, and the
/// other one's chunk never appears).
pub(crate) fn pick_largest_index(held: &[(u64, u64, u64)]) -> Option<usize> {
    let mut best: Option<(usize, u64, u64)> = None;
    for (at, (bytes, last_dump, _id)) in held.iter().enumerate() {
        let better = match best {
            None => true,
            Some((_, seen_bytes, seen_dump)) => {
                *bytes > seen_bytes || (*bytes == seen_bytes && *last_dump < seen_dump)
            }
        };
        if better {
            best = Some((at, *bytes, *last_dump));
        }
    }
    best.map(|(at, _, _)| at)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn watermarks_split_the_budget_like_the_design() {
        // The 8 GiB default: soft caps at 64 MiB, hard at 256 MiB.
        let w = Watermarks::new(8 << 30);
        assert_eq!(w.soft(), SOFT_CAP);
        assert_eq!(w.hard(), HARD_CAP);
        assert_eq!(w.level(0), Pressure::Normal);
        assert_eq!(w.level(SOFT_CAP), Pressure::Normal);
        assert_eq!(w.level(SOFT_CAP + 1), Pressure::Soft);
        assert_eq!(w.level(HARD_CAP), Pressure::Soft);
        assert_eq!(w.level(HARD_CAP + 1), Pressure::Hard);

        // A tight budget keeps them proportional.
        let w = Watermarks::new(16 << 20);
        assert_eq!(w.soft(), 4 << 20);
        assert_eq!(w.hard(), 8 << 20);

        // A budget below the floor still yields usable (identical) lines, so a
        // tiny test quota never turns every write into an inline seal.
        let w = Watermarks::new(1024);
        assert!(w.soft() >= 1 && w.soft() <= w.hard());
    }

    #[test]
    fn zero_budget_disables_the_mechanism() {
        let mut w = Watermarks::new(0);
        assert_eq!(w.level(u64::MAX), Pressure::Normal);
        w.set_budget(1 << 30);
        assert_eq!(w.level(1 << 29), Pressure::Hard);
        w.set_budget(0);
        assert_eq!(w.level(1 << 29), Pressure::Normal);
    }

    #[test]
    fn soft_waterline_halves_under_pressure_and_recovers() {
        let mut w = Watermarks::new(1 << 30); // base soft = 64 MiB
        let base = w.soft();
        assert_eq!(base, SOFT_CAP);
        w.retune(900 << 20); // above 80% of the budget
        assert_eq!(w.soft(), base / 2);
        w.retune(950 << 20);
        assert_eq!(w.soft(), base / 4);
        for _ in 0..40 {
            w.retune(1023 << 20); // never zero, however long the pressure lasts
        }
        assert!(w.soft() >= 1);
        w.retune(100 << 20); // back under half the budget
        assert_eq!(w.soft(), base);
    }

    fn cand(key: &str, bytes: u64, age_ms: u64) -> ReclaimCandidate {
        ReclaimCandidate {
            key: key.to_string(),
            bytes,
            age_ms,
        }
    }

    #[test]
    fn rrf_prefers_a_table_that_is_both_large_and_old() {
        // Big-and-new, small-and-old, and middling-and-middling. The fused order
        // must not collapse to either single rank: the two extremes tie, and the
        // middling table loses both dimensions and comes last.
        let list = [
            cand("db.big_new", 1_000, 10),
            cand("db.small_old", 10, 10_000),
            cand("db.mid", 100, 1_000),
        ];
        let order = rrf_order(&list);
        assert_eq!(order.len(), 3);
        // Both single-axis winners outrank the middling one.
        assert!(
            order[0] == 0 || order[0] == 1,
            "a single-axis winner must lead: {order:?}"
        );
        assert!(
            order[1] == 0 || order[1] == 1,
            "the other single-axis winner must be second: {order:?}"
        );
        assert_eq!(order[2], 2, "middling in both dimensions comes last");
    }

    #[test]
    fn rrf_breaks_a_symmetric_tie_toward_age() {
        // The fused score is symmetric, so "biggest but newest" ties "smallest
        // but oldest" exactly (`1/4 + 1/5` either way). Breaking that tie by
        // bytes would hand it to the fresh table and undo the whole point of the
        // age axis, so the tie-break prefers age: the idle table leads.
        let list = [
            cand("db.huge_fresh", 1_000_000_000, 1),
            cand("db.tiny_stale", 1, 6_000_000),
        ];
        let order = rrf_order(&list);
        assert_eq!(order[0], 1, "the idle table goes first: {order:?}");

        // Not merely a two-element accident: with a third table lowest on both
        // axes it stays last, and the tie between the extremes still resolves
        // toward age.
        let list = [
            cand("db.huge_fresh", 1_000_000_000, 1),
            cand("db.tiny_stale", 1, 6_000_000),
            cand("db.mid", 500_000, 3_000_000),
        ];
        let keys: Vec<&str> = rrf_order(&list)
            .iter()
            .map(|i| list[*i].key.as_str())
            .collect();
        assert_eq!(keys[0], "db.tiny_stale", "age wins the tie: {keys:?}");
        assert_eq!(
            keys[2], "db.mid",
            "middling on both axes goes last: {keys:?}"
        );
    }

    #[test]
    fn rrf_ties_are_broken_by_age_then_bytes_then_name() {
        // Same age, different bytes: bytes decide.
        let list = [
            cand("db.c", 500, 7),
            cand("db.b", 500, 7),
            cand("db.a", 900, 7),
        ];
        let order = rrf_order(&list);
        let keys: Vec<&str> = order.iter().map(|i| list[*i].key.as_str()).collect();
        assert_eq!(keys, vec!["db.a", "db.b", "db.c"]);

        // Same bytes and same age: the name is the final tie-break.
        let list = [cand("db.z", 10, 10), cand("db.y", 10, 10)];
        let keys: Vec<String> = rrf_order(&list)
            .iter()
            .map(|i| list[*i].key.clone())
            .collect();
        assert_eq!(keys, vec!["db.y", "db.z"]);
    }

    #[test]
    fn rrf_of_an_empty_set_is_empty() {
        assert!(rrf_order(&[]).is_empty());
    }

    #[test]
    fn the_largest_index_wins_and_ties_go_to_the_older_dump() {
        // (bytes, last_dump, id)
        let held = [(10, 5, 1), (900, 7, 2), (900, 3, 3)];
        assert_eq!(
            pick_largest_index(&held),
            Some(2),
            "equal bytes, older dump"
        );
        let held = [(900, 3, 3), (900, 7, 2)];
        assert_eq!(pick_largest_index(&held), Some(0));
        let held = [(5, 0, 1), (7, 0, 2)];
        assert_eq!(pick_largest_index(&held), Some(1), "bytes decide first");
        assert_eq!(pick_largest_index(&[]), None);
    }
}

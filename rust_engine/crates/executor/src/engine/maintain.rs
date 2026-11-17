//! Index maintenance accounting and scheduling (`CSPANN_DESIGN.md` §9.5, §9.6).
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
//! * [`SealScheduler`] is the §9.6 fair pick: weighted by pending bytes per
//!   database, at most `max_inflight` seals per tenant, and a tail that has
//!   waited past the starvation bound goes first and is counted
//!   (`index_seal_starvation`).

use std::collections::HashMap;

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

    /// The lower two waterlines, for the tests that pin them.
    #[cfg(test)]
    pub(crate) fn soft(&self) -> u64 {
        self.soft
    }

    #[cfg(test)]
    pub(crate) fn hard(&self) -> u64 {
        self.hard
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

/// How long a tail may go without a dump before it is escalated as starved.
///
/// The engine has no per-task clock on this path, so the bound is counted in
/// commit-ts ticks (one tick per commit): 1000 commits is the design's "2 s" for
/// a busy engine, and it is what makes a table that loses every weighted round
/// still get sealed.
pub(crate) const SEAL_STARVE_TICKS: u64 = 1000;

/// One tail that could be sealed, as the scheduler sees it.
#[derive(Clone, Debug)]
pub(crate) struct SealCandidate {
    /// `db.table`, the tenant the fair share is computed over.
    pub tenant: String,
    /// Bytes this tail holds right now (the scheduling weight).
    pub bytes: u64,
    /// Commit ts of its last dump, `0` when never: the starvation input.
    pub last_dump: u64,
}

/// §9.6's fair pick, narrowed to the one maintenance task kind the engine has
/// (seal a memory index into its chunk).
///
/// Weighted fair queueing over tenants: every `pick` credits each tenant by its
/// pending bytes and debits the winner by the whole round's credit, so a database
/// holding 10x the bytes is picked about 10x as often instead of one big table
/// absorbing every breach. Two bounds sit on top:
///
/// * at most `max_inflight` seals per tenant (a tenant cannot occupy the whole
///   dump pool);
/// * a candidate whose tail has not been dumped for `starved_after` ticks jumps
///   the queue and increments [`Self::starvation`] - the design's
///   `index_seal_starvation`, which used to be a silent starvation.
#[derive(Debug)]
pub(crate) struct SealScheduler {
    credit: HashMap<String, i128>,
    inflight: HashMap<String, usize>,
    max_inflight: usize,
    starvation: u64,
}

impl SealScheduler {
    pub(crate) fn new(max_inflight: usize) -> Self {
        Self {
            credit: HashMap::new(),
            inflight: HashMap::new(),
            max_inflight: max_inflight.max(1),
            starvation: 0,
        }
    }

    pub(crate) fn starvation(&self) -> u64 {
        self.starvation
    }

    pub(crate) fn inflight(&self, tenant: &str) -> usize {
        self.inflight.get(tenant).copied().unwrap_or(0)
    }

    /// A seal for `tenant` finished (or was dropped): release its slot.
    pub(crate) fn done(&mut self, tenant: &str) {
        if let Some(count) = self.inflight.get_mut(tenant) {
            *count = count.saturating_sub(1);
            if *count == 0 {
                self.inflight.remove(tenant);
            }
        }
    }

    /// The candidate to seal, or `None` when every tenant is at its in-flight
    /// bound (or there is nothing to seal).
    pub(crate) fn pick(
        &mut self,
        candidates: &[SealCandidate],
        now_ts: u64,
        starved_after: u64,
    ) -> Option<usize> {
        if candidates.is_empty() {
            return None;
        }
        let mut weight: HashMap<&str, i128> = HashMap::new();
        for c in candidates {
            *weight.entry(c.tenant.as_str()).or_insert(0) += c.bytes as i128;
        }
        let round: i128 = weight.values().sum();
        for (tenant, w) in &weight {
            *self.credit.entry((*tenant).to_string()).or_insert(0) += *w;
        }

        let open = |scheduler: &Self, c: &SealCandidate| {
            scheduler.inflight(&c.tenant) < scheduler.max_inflight
        };
        // A tail whose previous dump is older than the bound goes first; among
        // those, the longest wait wins, so a starved queue drains in order.
        let mut starved: Option<(u64, usize)> = None;
        for (i, c) in candidates.iter().enumerate() {
            if !open(self, c) {
                continue;
            }
            // Never dumped is normal (a fresh tail is ordered by weight); the
            // counter is for a tail that *had* its turn and did not get another.
            if c.last_dump == 0 {
                continue;
            }
            let age = now_ts.saturating_sub(c.last_dump);
            if age > starved_after && starved.is_none_or(|(seen, _)| age > seen) {
                starved = Some((age, i));
            }
        }
        let chosen = match starved {
            Some((_, i)) => {
                self.starvation += 1;
                i
            }
            None => {
                let mut best: Option<(i128, u64, usize)> = None;
                for (i, c) in candidates.iter().enumerate() {
                    if !open(self, c) {
                        continue;
                    }
                    let score = self.credit.get(&c.tenant).copied().unwrap_or(0);
                    let waited = now_ts.saturating_sub(c.last_dump);
                    let better = match best {
                        None => true,
                        Some((seen, seen_wait, _)) => {
                            score > seen || (score == seen && waited > seen_wait)
                        }
                    };
                    if better {
                        best = Some((score, waited, i));
                    }
                }
                best?.2
            }
        };
        let tenant = candidates[chosen].tenant.clone();
        *self.credit.entry(tenant.clone()).or_insert(0) -= round;
        *self.inflight.entry(tenant).or_insert(0) += 1;
        Some(chosen)
    }
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

    fn candidate(tenant: &str, bytes: u64, last_dump: u64) -> SealCandidate {
        SealCandidate {
            tenant: tenant.to_string(),
            bytes,
            last_dump,
        }
    }

    #[test]
    fn the_fair_pick_follows_the_bytes() {
        // db2 holds 10x the bytes, so it wins about 10 of 11 rounds.
        let mut s = SealScheduler::new(1);
        let small = candidate("db1", 1 << 20, 10);
        let big = candidate("db2", 10 << 20, 10);
        let (mut one, mut two) = (0usize, 0usize);
        for _ in 0..11 {
            let list = [small.clone(), big.clone()];
            match s.pick(&list, 100, 1000) {
                Some(0) => one += 1,
                Some(1) => two += 1,
                other => panic!("unexpected pick {other:?}"),
            }
            // The caller would have sealed it already: release the slot.
            s.done(if one > two { "db1" } else { "db2" });
        }
        assert_eq!(two, 10, "the heavy tenant gets its share ({one}/{two})");
        assert_eq!(one, 1);
    }

    #[test]
    fn a_tenant_cannot_hold_every_slot() {
        let mut s = SealScheduler::new(1);
        let a = candidate("db1", 100, 10);
        let b = candidate("db2", 1, 10);
        let list = [a, b];
        assert_eq!(s.pick(&list, 100, 1_000_000), Some(0));
        // db1 is at its bound, so the pick must come from db2 even though db1
        // carries all the weight.
        assert_eq!(s.pick(&list, 100, 1_000_000), Some(1));
        assert_eq!(s.pick(&list, 100, 1_000_000), None);
        s.done("db1");
        assert_eq!(s.pick(&list, 100, 1_000_000), Some(0));
    }

    #[test]
    fn a_starved_tail_jumps_the_queue_and_is_counted() {
        let mut s = SealScheduler::new(4);
        let fresh = candidate("db1", 100 << 20, 999);
        let stale = candidate("db2", 1, 10);
        let list = [fresh, stale];
        // `now - last_dump = 990 > 900`: the small but stale tail goes first.
        assert_eq!(s.pick(&list, 1000, 900), Some(1));
        assert_eq!(s.starvation(), 1);
        // Immediately re-picking the same stale tail would starve again; the
        // caller records the new dump ts, which is what we model here.
        let list = [candidate("db1", 100 << 20, 999), candidate("db2", 1, 1000)];
        assert_eq!(s.pick(&list, 1000, 900), Some(0));
        assert_eq!(s.starvation(), 1);
        // A never-dumped tail is not "starved" - it is simply new, and the
        // weight decides. Counting it would make the counter meaningless.
        let never = candidate("db3", 1, 0);
        let list = [candidate("db1", 100 << 20, 999), never];
        assert_eq!(s.pick(&list, 1000, 900), Some(0));
        assert_eq!(s.starvation(), 1);
    }
}

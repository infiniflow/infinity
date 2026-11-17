//! Recall monitoring and the query-time budget knobs (`SPANN_DESIGN.md` §11.3
//! items 4 and 5, §11.4).
//!
//! The index answers an *approximate* top-k, so the only honest way to keep a
//! recall promise is to measure it: a small fraction of the queries are also
//! answered exactly, and the gap between the two drives the search width.
//!
//! * window: the estimate is the mean of the samples the last seven days saw
//!   (§11.3 item 4), so one pathological query cannot move the beam;
//! * widen: `est < target` -> `beam *= 1.5` (up to `4x` the base) and
//!   `rebuild_suggested = true` - a beam that has to grow this far means the
//!   tree itself aged, and §9.4's rebuild is the real fix;
//! * shrink: `est > target + 0.03` **and** p99 over the latency budget ->
//!   `beam *= 0.9`, never below the base. Latency alone does not narrow the
//!   search: trading recall for speed is what §11.3 item 3 forbids.
//!
//! Item 5 (`WITH (recall_target = …, latency_budget_ms = …)`) overrides the
//! monitor's target and budget for one statement.

use std::collections::VecDeque;

/// The sliding window: seven days.
const WINDOW_SECS: u64 = 7 * 24 * 60 * 60;
/// One query in a hundred is answered exactly as well.
pub(crate) const DEFAULT_SAMPLE_RATE: u64 = 100;
/// Latency samples the p99 is taken over.
const LATENCY_SAMPLES: usize = 128;
/// §11.3 item 4's ceiling on the widening: `base * 4`.
const WIDEN_CAP: usize = 4;
/// How much better than the target the estimate has to be before narrowing.
const SHRINK_MARGIN: f32 = 0.03;

/// Per-index recall state.
///
/// Kept in memory (`MemoryContext::recall`): the target is a policy that a
/// restart re-learns in a few sampled queries, while a per-index catalog field
/// would be a format change for a number that tunes one search parameter.
#[derive(Debug)]
pub(crate) struct RecallMonitor {
    target: f32,
    /// `latency_budget_ms` of the last statement that named one (`None` = never
    /// shrink, only widen).
    budget_ms: Option<f32>,
    sample_rate: u64,
    seen: u64,
    window: VecDeque<(u64, f32)>,
    latency: VecDeque<f32>,
    base_beam: usize,
    beam: usize,
    max_beam: usize,
    rebuild_suggested: bool,
}

impl RecallMonitor {
    pub(crate) fn new(base_beam: usize) -> Self {
        let base = base_beam.max(1);
        Self {
            target: 0.95,
            budget_ms: None,
            sample_rate: DEFAULT_SAMPLE_RATE,
            seen: 0,
            window: VecDeque::new(),
            latency: VecDeque::new(),
            base_beam: base,
            beam: base,
            max_beam: base * WIDEN_CAP,
            rebuild_suggested: false,
        }
    }

    /// `WITH (recall_target = …)`.
    pub(crate) fn set_target(&mut self, target: f32) {
        if target > 0.0 && target <= 1.0 {
            self.target = target;
        }
    }

    /// `WITH (latency_budget_ms = …)`.
    pub(crate) fn set_budget(&mut self, budget_ms: f32) {
        if budget_ms > 0.0 {
            self.budget_ms = Some(budget_ms);
        }
    }

    #[cfg(test)]
    pub(crate) fn target(&self) -> f32 {
        self.target
    }

    /// The search width this query should use.
    pub(crate) fn beam(&self) -> usize {
        self.beam
    }

    #[cfg(test)]
    pub(crate) fn base_beam(&self) -> usize {
        self.base_beam
    }

    pub(crate) fn rebuild_suggested(&self) -> bool {
        self.rebuild_suggested
    }

    /// Whether this query is one of the sampled ones. Advances the counter, so
    /// exactly one in `sample_rate` queries takes the expensive exact path.
    pub(crate) fn sample(&mut self) -> bool {
        self.seen += 1;
        self.seen.is_multiple_of(self.sample_rate)
    }

    /// Feeds one sampled query. Returns the new beam when it changed.
    pub(crate) fn observe(&mut self, now_secs: u64, est: f32, latency_ms: f32) -> Option<usize> {
        let cutoff = now_secs.saturating_sub(WINDOW_SECS);
        while self.window.front().is_some_and(|(ts, _)| *ts < cutoff) {
            self.window.pop_front();
        }
        self.window.push_back((now_secs, est.clamp(0.0, 1.0)));
        self.latency.push_back(latency_ms);
        while self.latency.len() > LATENCY_SAMPLES {
            self.latency.pop_front();
        }
        let estimate = self.estimate()?;
        let before = self.beam;
        if estimate < self.target {
            // 1.5x, rounded up so it always grows; capped at 4x the base.
            self.beam = (self.beam * 3).div_ceil(2).min(self.max_beam);
            self.rebuild_suggested = true;
        } else if estimate > self.target + SHRINK_MARGIN {
            if let (Some(budget), Some(p99)) = (self.budget_ms, self.p99()) {
                if p99 > budget {
                    self.beam = (self.beam * 9 / 10).max(self.base_beam);
                }
            }
        }
        (self.beam != before).then_some(self.beam)
    }

    /// The window's mean estimate, `None` before the first sample.
    pub(crate) fn estimate(&self) -> Option<f32> {
        if self.window.is_empty() {
            return None;
        }
        let sum: f32 = self.window.iter().map(|(_, est)| *est).sum();
        Some(sum / self.window.len() as f32)
    }

    /// The p99 of the sampled end-to-end latencies.
    pub(crate) fn p99(&self) -> Option<f32> {
        if self.latency.is_empty() {
            return None;
        }
        let mut sorted: Vec<f32> = self.latency.iter().copied().collect();
        sorted.sort_by(f32::total_cmp);
        let at = ((sorted.len() as f32 * 0.99).ceil() as usize).saturating_sub(1);
        sorted.get(at.min(sorted.len() - 1)).copied()
    }

    #[cfg(test)]
    pub(crate) fn samples(&self) -> usize {
        self.window.len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn samples_one_query_in_a_hundred() {
        let mut m = RecallMonitor::new(8);
        let hits = (0..1000).filter(|_| m.sample()).count();
        assert_eq!(hits, 10);
    }

    #[test]
    fn a_low_estimate_widens_the_beam_and_suggests_a_rebuild() {
        let mut m = RecallMonitor::new(8);
        assert_eq!(m.base_beam(), 8);
        assert_eq!(m.beam(), 8);
        for i in 0..4 {
            m.observe(1000 + i, 0.80, 1.0);
        }
        assert_eq!(m.estimate(), Some(0.8));
        assert!(m.beam() > 8, "beam {} did not widen", m.beam());
        assert!(m.rebuild_suggested());
        // The ceiling is 4x the base, so a beam that keeps losing stops growing.
        for i in 0..40 {
            m.observe(2000 + i, 0.5, 1.0);
        }
        assert_eq!(m.beam(), 32);
    }

    #[test]
    fn shrinking_needs_both_a_good_estimate_and_a_missed_budget() {
        let mut m = RecallMonitor::new(16);
        // A good estimate and a terrible latency, but no budget named: raw
        // latency must never be allowed to narrow the search.
        for i in 0..10 {
            m.observe(1000 + i, 1.0, 9_999.0);
        }
        assert_eq!(m.beam(), 16);

        // Unreachable target widens it, then a reached target with a missed
        // budget narrows it back - but never below the base.
        m.set_target(0.99);
        for i in 0..5 {
            m.observe(2000 + i, 0.90, 1.0);
        }
        let widened = m.beam();
        assert!(widened > 16, "beam {widened} did not widen");

        m.set_target(0.5);
        m.set_budget(1.0);
        for i in 0..30 {
            m.observe(3000 + i, 1.0, 9.0);
        }
        assert_eq!(m.beam(), 16);
    }

    #[test]
    fn the_window_forgets_older_than_seven_days() {
        let mut m = RecallMonitor::new(8);
        m.observe(0, 0.10, 1.0);
        assert_eq!(m.samples(), 1);
        // Eight days later the old sample is out of the window.
        m.observe(WINDOW_SECS + 1, 0.99, 1.0);
        assert_eq!(m.samples(), 1);
        assert_eq!(m.estimate(), Some(0.99));
    }

    #[test]
    fn out_of_range_targets_and_budgets_are_ignored() {
        let mut m = RecallMonitor::new(8);
        m.set_target(0.0);
        m.set_target(1.5);
        assert_eq!(m.target(), 0.95);
        m.set_budget(-1.0);
        m.observe(1, 1.0, 100.0);
        // No budget named -> no narrowing, whatever the latency.
        assert_eq!(m.beam(), 8);
    }
}

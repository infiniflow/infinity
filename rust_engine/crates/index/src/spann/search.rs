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

//! The candidate set and the beam.
//!
//! `SearchSet` mirrors Cockroach's `search_set.go`: entries sort by
//! `(distance, errorBound, key)`, the heap only prunes once it is full, and
//! `maybe_closer` decides whether a *new* candidate can displace the current
//! worst one. `SPANN_DESIGN.md` §8.4 fixes the thresholds; the pruning rule is
//! what keeps a probe over a 128-slot partition from allocating per candidate.
//!
//! Scores are "smaller is better" for every metric: L2 stores the squared
//! distance, ip stores `-⟨o,q⟩`, cosine stores `-cos`. The executor re-scores
//! the candidates with `f64` anyway (`D1`), so this ordering only decides which
//! rows survive.

use std::collections::BinaryHeap;
use std::collections::HashSet;

use super::PartitionKey;

/// One result slot. `score` is the estimate, `err` the bound on its error.
#[derive(Clone, Copy, Debug)]
struct Entry {
    score: f32,
    err: f32,
    id: u64,
}

impl PartialEq for Entry {
    fn eq(&self, other: &Self) -> bool {
        self.cmp(other) == std::cmp::Ordering::Equal
    }
}

impl Eq for Entry {}

impl PartialOrd for Entry {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Entry {
    /// The heap is a **max**-heap of the worst entry: largest `score`, ties
    /// broken by the larger error bound, then by id (deterministic across runs).
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.score
            .total_cmp(&other.score)
            .then_with(|| self.err.total_cmp(&other.err))
            .then_with(|| self.id.cmp(&other.id))
    }
}

/// One partition waiting to be expanded, ordered so the heap pops the *nearest*
/// first.
///
/// Best-first is what makes recall independent of tree depth: a node at depth 3
/// competes directly with one at depth 7 on its lower bound, instead of the two
/// spending separate per-level budgets. With a level-synchronous beam a tree
/// grown by 2-way splits (one level per ~CAP rows) dilutes the beam to
/// `beam / 2^height` by the time it reaches the leaves - measured at 0.23 recall
/// on 100k x 1536d / height 9 with beam 32, against 1.0 once the walk is
/// best-first. This is Cockroach's `BestFirstSearch` (`cspann/search.go`), and
/// `beam` here is its `beamSize`: a budget on **leaf partitions scanned**, not a
/// per-level width.
#[derive(Clone, Copy, Debug)]
pub struct Candidate {
    /// Distance from the query to this partition's **centroid**: the ordering
    /// key. Every partition holds exactly the rows of its own Voronoi cell, so
    /// this is the tight, meaningful rank.
    pub bound: f32,
    /// `max(0, bound - radius)`: a true lower bound on any row in the subtree,
    /// used only to *skip* a subtree, never to order or to stop the walk. With
    /// `bound - radius` as the heap key the rank collapses: a merged or sparse
    /// partition has a radius larger than its distance, every such node ties at
    /// 0, and the beam walks them in key order (measured: 0.85 recall at 200k x
    /// 1536d versus 1.0 once the rank is the centroid distance).
    pub lower: f32,
    /// Widened to `u64`: the incremental tree keys partitions by their DFS path,
    /// the published tree by a `u32` DFS ordinal.
    pub key: u64,
}

impl PartialEq for Candidate {
    fn eq(&self, other: &Self) -> bool {
        self.cmp(other) == std::cmp::Ordering::Equal
    }
}

impl Eq for Candidate {}

impl PartialOrd for Candidate {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Candidate {
    /// Reversed on purpose: `BinaryHeap` is a max-heap, and the smallest lower
    /// bound has to come out first. Ties break on the key so a query is
    /// reproducible.
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        other
            .bound
            .total_cmp(&self.bound)
            .then_with(|| other.key.cmp(&self.key))
    }
}

/// `max(0, distance - radius)` for L2, else `distance` (§8.3: the radius bounds
/// a partition's rows only under L2).
pub fn radius_lower(distance: f32, radius: f32, metric: crate::vector::Metric) -> f32 {
    if metric.is_l2() {
        (distance - radius).max(0.0)
    } else {
        distance
    }
}

/// The top-k set of one query, with the dedupe every multi-chunk merge needs.
#[derive(Debug)]
pub struct SearchSet {
    k: usize,
    /// Once the heap holds this many entries it starts pruning: a candidate
    /// that cannot beat the worst is rejected without touching the heap
    /// (`pruning_threshold` in the reference).
    capacity: usize,
    heap: BinaryHeap<Entry>,
    seen: HashSet<u64>,
    /// Candidates offered, for the metrics in §16.4.
    offered: usize,
}

impl SearchSet {
    pub fn new(k: usize, extra: usize) -> Self {
        let k = k.max(1);
        let capacity = k + extra;
        Self {
            k,
            capacity,
            heap: BinaryHeap::with_capacity(capacity + 1),
            seen: HashSet::new(),
            offered: 0,
        }
    }

    /// §8.4: `pruning_threshold = max(1024, (k + extra) * 2)`.
    pub fn with_default_capacity(k: usize) -> Self {
        Self::new(k, k.max(1024))
    }

    pub fn len(&self) -> usize {
        self.heap.len()
    }

    pub fn is_empty(&self) -> bool {
        self.heap.is_empty()
    }

    pub fn offered(&self) -> usize {
        self.offered
    }

    /// Whether `(score, err)` could still land in the final top-k.
    pub fn maybe_closer(&self, score: f32, err: f32) -> bool {
        if self.heap.len() < self.capacity {
            return true;
        }
        match self.heap.peek() {
            None => true,
            Some(worst) => score - err <= worst.score + worst.err,
        }
    }

    /// The worst surviving entry's upper bound, for the early-stop test
    /// (`d_k+ = dist_k + err_k`).
    pub fn kth_upper(&self) -> Option<f32> {
        if self.heap.len() < self.k {
            None
        } else {
            self.heap.peek().map(|w| w.score + w.err)
        }
    }

    pub fn add(&mut self, score: f32, err: f32, id: u64) {
        if !score.is_finite() {
            return;
        }
        self.offered += 1;
        if !self.seen.insert(id) {
            return;
        }
        if self.heap.len() < self.capacity {
            self.heap.push(Entry { score, err, id });
            return;
        }
        let Some(worst) = self.heap.peek() else {
            self.heap.push(Entry { score, err, id });
            return;
        };
        if score - err > worst.score + worst.err {
            return;
        }
        self.heap.pop();
        self.heap.push(Entry { score, err, id });
    }

    /// The surviving entries, best first, truncated to `k`.
    pub fn into_sorted(mut self) -> Vec<(u64, f32)> {
        let mut out: Vec<(u64, f32)> = Vec::with_capacity(self.heap.len());
        while let Some(e) = self.heap.pop() {
            out.push((e.id, e.score));
        }
        out.reverse();
        out.truncate(self.k);
        out
    }

    /// Best-first without consuming (the tree descent reads the frontier).
    pub fn best(&self) -> Option<(u64, f32)> {
        self.heap
            .iter()
            .map(|e| (e.id, e.score))
            .min_by(|a, b| a.1.total_cmp(&b.1))
    }
}

/// The beam width at `level` (Cockroach's `beamSize /= 2^(level-LeafLevel)`).
///
/// Only the leaf level keeps the caller's width, because only the leaf scan
/// decides recall. Every level above it holds exponentially fewer partitions,
/// so widening the beam up there buys nothing and costs centroid distances;
/// keeping it at the leaf width is what made a `beam = 32` query pay the root's
/// whole level for no answer.
pub fn level_width(base: usize, level: u32) -> usize {
    let base = base.max(1);
    if level <= 1 {
        return base;
    }
    (base >> (level - 1).min(31)).max(1)
}

/// Whether a partition whose L2 **distance** lower bound is `lower_bound` can
/// still beat an incumbent k-th result scored as a **squared** distance.
///
/// The two numbers live in different units: `Partition::lower_bound` returns
/// `‖q−c‖ − R`, while every L2 score in [`SearchSet`] is `‖q−o‖²`. Comparing
/// them directly (the first version did) prunes a partition whenever its bound
/// crosses the *squared* k-th distance, which for normalised vectors - where
/// `d² < d < 1` is the common case - drops partitions that genuinely hold the
/// answer. Squaring the bound restores the inequality `d ≥ lb ⟹ d² ≥ lb²`.
pub fn l2_can_beat(lower_bound: f32, kth_squared: f32) -> bool {
    lower_bound * lower_bound <= kth_squared
}

/// A fixed-width beam of partitions, ordered by their lower bound.
#[derive(Debug)]
pub struct Beam {
    width: usize,
    items: Vec<(f32, PartitionKey)>,
}

impl Beam {
    pub fn new(width: usize) -> Self {
        Self {
            width: width.max(1),
            items: Vec::new(),
        }
    }

    pub fn with_root(root: PartitionKey) -> Self {
        Self {
            width: 1,
            items: vec![(0.0, root)],
        }
    }

    pub fn push(&mut self, lower_bound: f32, key: PartitionKey) {
        self.items.push((lower_bound, key));
        // Sort only when the buffer doubled, so a 65k-way level does not
        // re-sort 65k times.
        if self.items.len() >= self.width.saturating_mul(2).max(8) {
            self.prune();
        }
    }

    pub fn prune(&mut self) {
        self.items
            .sort_by(|a, b| a.0.total_cmp(&b.0).then_with(|| a.1.cmp(&b.1)));
        self.items.truncate(self.width);
    }

    pub fn into_keys(mut self) -> Vec<PartitionKey> {
        self.prune();
        self.items.into_iter().map(|(_, key)| key).collect()
    }

    /// [`Self::into_keys`] with the bounds kept: the leaf scan tests them
    /// against the incumbent k-th result, and recomputing a centroid distance
    /// per partition at the leaf level is pure waste.
    pub fn into_items(mut self) -> Vec<(f32, PartitionKey)> {
        self.prune();
        self.items
    }

    pub fn keys(&self) -> Vec<PartitionKey> {
        self.items.iter().map(|(_, key)| *key).collect()
    }

    pub fn len(&self) -> usize {
        self.items.len()
    }

    pub fn is_empty(&self) -> bool {
        self.items.is_empty()
    }

    pub fn width(&self) -> usize {
        self.width
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn keeps_the_k_best_and_dedupes() {
        let mut set = SearchSet::new(3, 0);
        for (id, score) in [(1u64, 0.5f32), (2, 0.1), (3, 0.9), (4, 0.05), (2, 0.0)] {
            set.add(score, 0.0, id);
        }
        let got = set.into_sorted();
        assert_eq!(
            got.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
            vec![4, 2, 1],
            "the second offer of id 2 is a duplicate and is ignored (first one wins)"
        );
    }

    #[test]
    fn error_bound_is_used_when_pruning() {
        let mut set = SearchSet::new(1, 0);
        set.add(1.0, 0.0, 10);
        // 1.4 - 0.5 = 0.9 <= 1.0 + 0.0: could still be closer, admitted.
        set.add(1.4, 0.5, 11);
        // 2.5 - 0.1 = 2.4 > 1.4 + 0.5: cannot beat the incumbent, pruned.
        set.add(2.5, 0.1, 12);
        let ids: Vec<u64> = set.into_sorted().into_iter().map(|(id, _)| id).collect();
        assert_eq!(ids, vec![11]);
    }

    #[test]
    fn kth_upper_is_none_until_the_set_is_full() {
        let mut set = SearchSet::new(2, 0);
        set.add(0.5, 0.1, 1);
        assert!(set.kth_upper().is_none());
        set.add(0.7, 0.2, 2);
        // The worst surviving entry is id 2: 0.7 + 0.2.
        assert!((set.kth_upper().unwrap() - 0.9).abs() < 1e-6);
    }

    #[test]
    fn beam_keeps_the_smallest_bounds() {
        let mut beam = Beam::new(2);
        for (lb, key) in [(5.0f32, 1u32), (1.0, 2), (3.0, 3), (0.5, 4)] {
            beam.push(lb, key);
        }
        assert_eq!(beam.into_keys(), vec![4, 2]);
    }

    #[test]
    fn into_items_keeps_the_bound_of_each_key() {
        let mut beam = Beam::new(2);
        for (lb, key) in [(5.0f32, 1u32), (1.0, 2), (0.5, 4)] {
            beam.push(lb, key);
        }
        assert_eq!(beam.into_items(), vec![(0.5, 4), (1.0, 2)]);
    }

    /// Cockroach's `beamSize /= 2^(level-LeafLevel)`: only the leaf level keeps
    /// the caller's width, every level above it is halved again.
    #[test]
    fn level_width_narrows_interior_levels() {
        assert_eq!(level_width(32, 1), 32);
        assert_eq!(level_width(32, 2), 16);
        assert_eq!(level_width(32, 3), 8);
        assert_eq!(level_width(32, 4), 4);
        // Never zero, even for a beam of 1 at a tall tree.
        assert_eq!(level_width(1, 9), 1);
        assert_eq!(level_width(0, 3), 1);
    }

    /// The bound is a distance and the k-th result is a squared distance; the
    /// first version compared them directly, which pruned every partition whose
    /// bound fell between `kth` and `√kth` - i.e. most of them once vectors are
    /// normalised.
    #[test]
    fn squared_bound_is_compared_in_the_right_units() {
        // lb = 0.3 puts members at d² ≥ 0.09, inside kth = 0.25: keep it.
        assert!(l2_can_beat(0.3, 0.25));
        // lb = 0.6 puts members at d² ≥ 0.36, outside kth = 0.25: prune it.
        assert!(!l2_can_beat(0.6, 0.25));
        // A bound of exactly √kth is still admissible (ties are kept).
        assert!(l2_can_beat(0.5, 0.25));
    }
}

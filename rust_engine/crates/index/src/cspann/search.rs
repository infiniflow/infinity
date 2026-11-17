//! The candidate set and the beam.
//!
//! `SearchSet` mirrors Cockroach's `search_set.go`: entries sort by
//! `(distance, errorBound, key)`, the heap only prunes once it is full, and
//! `maybe_closer` decides whether a *new* candidate can displace the current
//! worst one. `CSPANN_DESIGN.md` §8.4 fixes the thresholds; the pruning rule is
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
}

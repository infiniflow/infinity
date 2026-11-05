//! The reference's KNN result merger: `MergeKnn` over `HeapResultHandler`.
//!
//! Every index scan feeds scored `(distance, RowID)` pairs from each sealed
//! chunk and from the memory segment into one merger, which keeps the global
//! top-k per query.
//!
//! C++: `src/storage/knn_index/merge_knn.cppm` and
//! `src/storage/knn_index/result_handler.cppm`.

use std::cmp::Ordering;
use std::collections::{BTreeSet, BinaryHeap};

use crate::RowID;

/// Which end of the distance axis wins, i.e. the reference's
/// `CompareMax` / `CompareMin`.
///
/// `Max` keeps the smallest distances (L2 and Hamming); `Min` keeps the
/// largest (cosine and inner product, whose distances are negated before
/// they reach the merger).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Compare {
    Max,
    Min,
}

impl Compare {
    /// C++ `Compare::Compare(d, threshold)`: `true` when `d` is dropped.
    fn outside(self, d: f32, threshold: f32) -> bool {
        match self {
            Compare::Max => d > threshold,
            Compare::Min => d < threshold,
        }
    }

    /// The heap key. Larger is worse so the heap root is the eviction
    /// candidate: the largest distance (`Max`) or the smallest (`Min`).
    fn key(self, d: f32) -> f32 {
        match self {
            Compare::Max => d,
            Compare::Min => -d,
        }
    }
}

/// A `(distance, row id)` slot ordered so that [`BinaryHeap`]'s max is the
/// worst entry: worse distance first and, on a tie, the larger row id.
/// C++: `CompareMax::Compare(a, b, i, j)`.
#[derive(Clone, Copy, Debug)]
struct Entry {
    dist: f32,
    key: f32,
    row_id: RowID,
}

impl PartialEq for Entry {
    fn eq(&self, other: &Self) -> bool {
        self.cmp(other) == Ordering::Equal
    }
}

impl Eq for Entry {}

impl PartialOrd for Entry {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Entry {
    fn cmp(&self, other: &Self) -> Ordering {
        self.key
            .total_cmp(&other.key)
            .then(self.row_id.cmp(&other.row_id))
    }
}

/// Accumulates per-segment KNN results into one top-k per query.
///
/// C++: `MergeKnn<QueryElemType, C, DistType>`.
pub struct MergeKnn {
    compare: Compare,
    query_count: usize,
    top_k: usize,
    threshold: Option<f32>,
    total_count: i64,
    begun: bool,
    heaps: Vec<BinaryHeap<Entry>>,
    /// Materialized slots, valid after [`MergeKnn::end`] /
    /// [`MergeKnn::end_without_sort`].
    results: Vec<Vec<(f32, RowID)>>,
    result_size: Option<usize>,
}

impl MergeKnn {
    /// C++: `MergeKnn(query_count, topk, knn_threshold)`.
    pub fn new(query_count: usize, top_k: usize, compare: Compare, threshold: Option<f32>) -> Self {
        Self {
            compare,
            query_count,
            top_k,
            threshold,
            total_count: 0,
            begun: false,
            heaps: (0..query_count).map(|_| BinaryHeap::new()).collect(),
            results: vec![Vec::new(); query_count],
            result_size: None,
        }
    }

    /// C++: `Begin`. Re-initializes the per-query heaps.
    pub fn begin(&mut self) {
        if self.begun || self.query_count == 0 {
            return;
        }
        for heap in &mut self.heaps {
            heap.clear();
        }
        self.begun = true;
    }

    /// C++: `MergeKnnResultHandlerT::AddResult` — applies the threshold and
    /// keeps a bounded heap of the best `top_k` entries.
    pub fn add_result(&mut self, query_id: usize, dist: f32, row_id: RowID) {
        debug_assert!(query_id < self.query_count);
        if let Some(t) = self.threshold {
            if self.compare.outside(dist, t) {
                return;
            }
        }
        let entry = Entry {
            dist,
            key: self.compare.key(dist),
            row_id,
        };
        self.heaps[query_id].push(entry);
        if self.heaps[query_id].len() > self.top_k {
            self.heaps[query_id].pop();
        }
    }

    /// C++: `Search(size_t query_id, const DistType *, const RowID *, count)`.
    pub fn search(&mut self, query_id: usize, hits: &[(f32, RowID)]) {
        if query_id == 0 {
            self.total_count += hits.len() as i64;
        }
        for (dist, row_id) in hits {
            self.add_result(query_id, *dist, *row_id);
        }
    }

    /// C++: `End` — settles the heap and sorts each query best-first.
    pub fn end(&mut self) {
        if !self.begun {
            return;
        }
        for (q, heap) in self.heaps.iter_mut().enumerate() {
            let mut slots: Vec<Entry> = heap.drain().collect();
            // The key is "larger is worse", so ascending order is best-first.
            slots.sort();
            self.results[q] = slots.into_iter().map(|e| (e.dist, e.row_id)).collect();
        }
        self.result_size = Some(self.results[0].len().min(self.top_k));
        self.begun = false;
    }

    /// C++: `EndWithoutSort` — materializes the heap order, which is enough
    /// when the caller only needs the candidate ids.
    pub fn end_without_sort(&mut self) {
        if !self.begun {
            return;
        }
        for (q, heap) in self.heaps.iter_mut().enumerate() {
            self.results[q] = heap.drain().map(|e| (e.dist, e.row_id)).collect();
        }
        self.result_size = Some(self.results[0].len().min(self.top_k));
        self.begun = false;
    }

    /// C++: `GetSize` (`min(topk, result_handler_->GetSize(0))` once ended).
    pub fn get_size(&self, query_id: usize) -> usize {
        match self.result_size {
            Some(n) => n.min(self.top_k),
            None => self.heaps[query_id].len(),
        }
    }

    /// C++: `GetDistancesByIdx`.
    pub fn distances_by_idx(&self, query_id: usize) -> Vec<f32> {
        self.results[query_id].iter().map(|(d, _)| *d).collect()
    }

    /// C++: `GetIDsByIdx`.
    pub fn ids_by_idx(&self, query_id: usize) -> Vec<RowID> {
        self.results[query_id].iter().map(|(_, r)| *r).collect()
    }

    /// C++: `GetUniqueIDs` — the candidate set across every query, ordered
    /// like the reference's `std::set<RowID>`.
    pub fn unique_ids(&self) -> Vec<RowID> {
        let mut ids = BTreeSet::new();
        for slots in &self.results {
            for (_, row_id) in slots {
                ids.insert(*row_id);
            }
        }
        ids.into_iter().collect()
    }

    /// C++: `total_input_count`.
    pub fn total_input_count(&self) -> i64 {
        self.total_count
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn row(segment: u32, offset: u32) -> RowID {
        ((segment as u64) << 32) | offset as u64
    }

    #[test]
    fn merges_segments_into_the_global_top_k() {
        let mut merge = MergeKnn::new(1, 3, Compare::Max, None);
        merge.begin();
        merge.search(0, &[(0.1, row(0, 10)), (0.5, row(0, 11))]);
        merge.search(
            0,
            &[(0.05, row(1, 20)), (0.4, row(1, 21)), (0.9, row(1, 22))],
        );
        merge.end();
        assert_eq!(
            merge.ids_by_idx(0),
            vec![row(1, 20), row(0, 10), row(1, 21)]
        );
        assert_eq!(merge.distances_by_idx(0), vec![0.05, 0.1, 0.4]);
        assert_eq!(merge.get_size(0), 3);
        assert_eq!(merge.total_input_count(), 5);
    }

    #[test]
    fn ties_drop_the_larger_row_id() {
        let mut merge = MergeKnn::new(1, 2, Compare::Max, None);
        merge.begin();
        merge.search(0, &[(1.0, 5), (1.0, 3), (1.0, 9)]);
        merge.end();
        // Row 9 is the larger id, so it is the worst of three equal distances.
        assert_eq!(merge.ids_by_idx(0), vec![3, 5]);
    }

    #[test]
    fn threshold_is_applied_against_the_stored_distance() {
        let mut dist = MergeKnn::new(1, 4, Compare::Max, Some(0.5));
        dist.begin();
        dist.search(0, &[(0.4, 1), (0.6, 2)]);
        dist.end();
        assert_eq!(dist.ids_by_idx(0), vec![1]);

        // Cosine / inner product store negated distances and keep the largest.
        let mut sim = MergeKnn::new(1, 4, Compare::Min, Some(-0.9));
        sim.begin();
        sim.search(0, &[(-0.95, 1), (-0.5, 2)]);
        sim.end();
        assert_eq!(sim.ids_by_idx(0), vec![2]);
        assert_eq!(sim.distances_by_idx(0), vec![-0.5]);
    }

    #[test]
    fn queries_have_independent_slots() {
        let mut merge = MergeKnn::new(2, 1, Compare::Max, None);
        merge.begin();
        merge.add_result(0, 0.2, row(0, 1));
        merge.add_result(0, 0.1, row(0, 2));
        merge.add_result(1, 0.3, row(0, 3));
        merge.add_result(1, 0.4, row(0, 4));
        merge.end();
        assert_eq!(merge.ids_by_idx(0), vec![row(0, 2)]);
        assert_eq!(merge.ids_by_idx(1), vec![row(0, 3)]);
        assert_eq!(merge.unique_ids(), vec![row(0, 2), row(0, 3)]);
    }

    #[test]
    fn end_without_sort_keeps_every_candidate() {
        let mut merge = MergeKnn::new(1, 3, Compare::Max, None);
        merge.begin();
        merge.search(0, &[(0.3, row(0, 1)), (0.1, row(0, 2)), (0.2, row(0, 3))]);
        merge.end_without_sort();
        let mut ids = merge.ids_by_idx(0);
        ids.sort();
        assert_eq!(ids, vec![row(0, 1), row(0, 2), row(0, 3)]);
        assert_eq!(merge.get_size(0), 3);
    }

    #[test]
    fn cosine_merge_sorts_best_first() {
        let mut merge = MergeKnn::new(1, 2, Compare::Min, None);
        merge.begin();
        // Cosine arrives as `cos - 1` (similarity - 1): larger is better.
        merge.search(
            0,
            &[(-0.2, row(0, 1)), (-0.8, row(0, 2)), (-0.5, row(0, 3))],
        );
        merge.end();
        assert_eq!(merge.ids_by_idx(0), vec![row(0, 1), row(0, 3)]);
        assert_eq!(merge.distances_by_idx(0), vec![-0.2, -0.5]);
    }
}

//! Multi-vector retrieval: `MaxSim` scoring with the reference's inner top-n
//! aggregation (`src/storage/knn_index/multivector/`,
//! `MultiVectorResultHandler`).
//!
//! A document is a bag of token vectors. `MaxSim(q, d) = Σ_i max_j <q_i, d_j>`;
//! the reference truncates each token's contribution to the document's inner
//! top-n (`topn` option) before the sum. This module keeps one index per
//! segment (`ROW_ID` high bits) and merges the per-segment results.

use std::collections::HashMap;

/// Per-segment multi-vector index.
#[derive(Debug, Default)]
pub struct MultiVectorIndex {
    dims: usize,
    /// Document token vectors, in insertion order.
    docs: Vec<Vec<Vec<f32>>>,
}

impl MultiVectorIndex {
    pub fn new(dims: usize) -> Self {
        Self {
            dims,
            docs: Vec::new(),
        }
    }

    /// Appends a document's token vectors; returns its dense id.
    pub fn add(&mut self, doc: Vec<Vec<f32>>) -> usize {
        debug_assert!(doc.iter().all(|v| v.len() == self.dims));
        self.docs.push(doc);
        self.docs.len() - 1
    }

    pub fn len(&self) -> usize {
        self.docs.len()
    }

    pub fn is_empty(&self) -> bool {
        self.docs.is_empty()
    }

    /// `MaxSim` of one document. `inner_topn == 0` keeps every token.
    pub fn maxsim(&self, query: &[Vec<f32>], doc: usize, inner_topn: usize) -> f32 {
        let Some(tokens) = self.docs.get(doc) else {
            return 0.0;
        };
        let mut total = 0.0f32;
        for q in query {
            let mut scores: Vec<f32> = tokens.iter().map(|d| dot(q, d)).collect();
            if inner_topn > 0 && scores.len() > inner_topn {
                scores.sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
                scores.truncate(inner_topn);
            }
            // The reference sums the per-query-token best match; when `topn`
            // keeps several, their sum is still the token's contribution.
            total += scores.iter().copied().fold(f32::NEG_INFINITY, f32::max);
        }
        total
    }

    /// Top-k documents by `MaxSim`, ascending by document id on ties.
    pub fn search(&self, query: &[Vec<f32>], k: usize, inner_topn: usize) -> Vec<(usize, f32)> {
        let mut scored: Vec<(usize, f32)> = (0..self.docs.len())
            .map(|d| (d, self.maxsim(query, d, inner_topn)))
            .collect();
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(k);
        scored
    }
}

/// Merges per-segment results (`Vec<(row, score)>`) into one top-k, keeping the
/// best score per row (the reference's `MergeKnn` over segment handlers).
pub fn merge_topk(per_segment: Vec<Vec<(usize, f32)>>, k: usize) -> Vec<(usize, f32)> {
    let mut best: HashMap<usize, f32> = HashMap::new();
    for hits in per_segment {
        for (row, score) in hits {
            best.entry(row)
                .and_modify(|s| {
                    if score > *s {
                        *s = score
                    }
                })
                .or_insert(score);
        }
    }
    let mut out: Vec<(usize, f32)> = best.into_iter().collect();
    out.sort_by(|a, b| {
        b.1.partial_cmp(&a.1)
            .unwrap_or(std::cmp::Ordering::Equal)
            .then(a.0.cmp(&b.0))
    });
    out.truncate(k);
    out
}

fn dot(a: &[f32], b: &[f32]) -> f32 {
    a.iter().zip(b).map(|(x, y)| x * y).sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn maxsim_matches_definition() {
        let mut index = MultiVectorIndex::new(2);
        let d0 = vec![vec![1.0, 0.0], vec![0.0, 1.0]];
        let d1 = vec![vec![0.5, 0.5]];
        index.add(d0.clone());
        index.add(d1.clone());
        let q = vec![vec![1.0, 0.0], vec![0.0, 1.0]];
        // d0: 1 + 1 = 2; d1: 0.5 + 0.5 = 1.
        assert!((index.maxsim(&q, 0, 0) - 2.0).abs() < 1e-6);
        assert!((index.maxsim(&q, 1, 0) - 1.0).abs() < 1e-6);
        let hits = index.search(&q, 2, 0);
        assert_eq!(hits[0].0, 0);
        assert_eq!(hits[1].0, 1);
    }

    #[test]
    fn merge_keeps_best_per_row() {
        let merged = merge_topk(vec![vec![(7, 0.5), (9, 0.2)], vec![(7, 0.9), (3, 0.4)]], 2);
        assert_eq!(merged, vec![(7, 0.9), (3, 0.4)]);
    }
}

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
    fn pq_prefilter_keeps_the_true_topk() {
        let dims = 16usize;
        let mut state = 7u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        let docs: Vec<Vec<Vec<f32>>> = (0..300)
            .map(|_| {
                (0..4)
                    .map(|_| {
                        (0..dims)
                            .map(|_| ((next() % 1000) as f32) / 500.0 - 1.0)
                            .collect()
                    })
                    .collect()
            })
            .collect();
        let all: Vec<&[f32]> = docs.iter().flatten().map(|t| t.as_slice()).collect();
        let pq = PqCodebook::train(dims, 4, 8, &all);
        let mut index = MultiVectorIndex::new(dims);
        for d in &docs {
            index.add(d.clone());
        }
        let query = vec![(0..dims)
            .map(|i| (i as f32) / 16.0 - 0.5)
            .collect::<Vec<f32>>()];
        let exact: Vec<usize> = index
            .search(&query, 5, 0)
            .into_iter()
            .map(|(d, _)| d)
            .collect();
        let cands = pq.prefilter(&query, &docs, 40);
        let hits = exact.iter().filter(|d| cands.contains(d)).count();
        assert!(hits >= 4, "recall {hits}/5, candidates {cands:?}");
    }

    #[test]
    fn merge_keeps_best_per_row() {
        let merged = merge_topk(vec![vec![(7, 0.5), (9, 0.2)], vec![(7, 0.9), (3, 0.4)]], 2);
        assert_eq!(merged, vec![(7, 0.9), (3, 0.4)]);
    }
}

/// Product-quantization codebook for the first search stage of the reference's
/// EMVB index (`emvb_index.cppm`: residual PQ with `subspace_num` / `subspace_bits`).
///
/// Each document token is encoded as one centroid id per subspace; a query
/// scores a document by the centroid interactions of its tokens, which yields a
/// candidate set that is then re-scored with the exact `MaxSim`. This is the
/// "step 1 / step 2" structure of `EMVBIndex::Search` without the residual
/// stage.
#[derive(Debug, Default)]
pub struct PqCodebook {
    dims: usize,
    subspace_num: usize,
    /// `centroids[s][c]` = the c-th centroid of subspace `s`.
    centroids: Vec<Vec<Vec<f32>>>,
}

impl PqCodebook {
    /// Trains `1 << bits` centroids per subspace with deterministic k-means.
    pub fn train(dims: usize, subspace_num: usize, bits: u32, tokens: &[&[f32]]) -> Self {
        let subspace_num = subspace_num.max(1).min(dims.max(1));
        let width = dims.div_ceil(subspace_num);
        let k = 1usize << bits.min(8);
        let mut centroids = Vec::with_capacity(subspace_num);
        for s in 0..subspace_num {
            let lo = s * width;
            let hi = (lo + width).min(dims);
            // Evenly spaced initial centroids from the sampled tokens.
            let mut cs: Vec<Vec<f32>> = Vec::with_capacity(k);
            for c in 0..k {
                let idx = (c * tokens.len()) / k.max(1);
                let t = tokens.get(idx.min(tokens.len().saturating_sub(1))).copied();
                let mut v = vec![0.0f32; hi.saturating_sub(lo)];
                if let Some(t) = t {
                    let n = v.len().min(t.len().saturating_sub(lo));
                    v[..n].copy_from_slice(&t[lo..lo + n]);
                }
                cs.push(v);
            }
            // Lloyd iterations.
            for _ in 0..8 {
                let mut sums = vec![vec![0.0f32; hi.saturating_sub(lo)]; k];
                let mut counts = vec![0usize; k];
                for t in tokens {
                    let v = &t[lo..hi.min(t.len())];
                    if v.is_empty() {
                        continue;
                    }
                    let mut best = 0usize;
                    let mut best_d = f32::INFINITY;
                    for (ci, c) in cs.iter().enumerate() {
                        let d: f32 = c.iter().zip(v).map(|(a, b)| (a - b) * (a - b)).sum();
                        if d < best_d {
                            best_d = d;
                            best = ci;
                        }
                    }
                    counts[best] += 1;
                    for (acc, x) in sums[best].iter_mut().zip(v) {
                        *acc += x;
                    }
                }
                for ci in 0..k {
                    if counts[ci] == 0 {
                        continue;
                    }
                    for (acc, c) in sums[ci].iter().zip(cs[ci].iter_mut()) {
                        *c = acc / counts[ci] as f32;
                    }
                }
            }
            centroids.push(cs);
        }
        Self {
            dims,
            subspace_num,
            centroids,
        }
    }

    /// Encodes one token as one centroid id per subspace.
    pub fn encode(&self, token: &[f32]) -> Vec<u8> {
        let width = self.dims.div_ceil(self.subspace_num);
        let mut out = Vec::with_capacity(self.subspace_num);
        for s in 0..self.subspace_num {
            let lo = s * width;
            let hi = (lo + width).min(self.dims);
            if lo >= token.len() {
                out.push(0);
                continue;
            }
            let v = &token[lo..hi.min(token.len())];
            let mut best = 0usize;
            let mut best_d = f32::INFINITY;
            for (ci, c) in self.centroids[s].iter().enumerate() {
                let d: f32 = c.iter().zip(v).map(|(a, b)| (a - b) * (a - b)).sum();
                if d < best_d {
                    best_d = d;
                    best = ci;
                }
            }
            out.push(best as u8);
        }
        out
    }

    /// Stage 1: ranks documents by centroid interaction and keeps the best
    /// `n_candidates` (`centroid_nprobe` / `n_doc_to_score` in the reference).
    pub fn prefilter(
        &self,
        query: &[Vec<f32>],
        docs: &[Vec<Vec<f32>>],
        n_candidates: usize,
    ) -> Vec<usize> {
        let q_codes: Vec<Vec<u32>> = query
            .iter()
            .map(|q| self.encode(q).into_iter().map(u32::from).collect())
            .collect();
        // Score of a (query token, centroid id) pair per subspace.
        let mut scored: Vec<(usize, f32)> = Vec::with_capacity(docs.len());
        for (d, doc) in docs.iter().enumerate() {
            let mut total = 0.0f32;
            for qc in &q_codes {
                let mut best = f32::NEG_INFINITY;
                for token in doc {
                    let code = self.encode(token);
                    let mut s = 0.0f32;
                    for (sub, cid) in code.iter().enumerate() {
                        let c = &self.centroids[sub][*cid as usize];
                        let q = &self.centroids[sub][qc[sub] as usize];
                        s += q.iter().zip(c).map(|(a, b)| a * b).sum::<f32>();
                    }
                    if s > best {
                        best = s;
                    }
                }
                total += best.max(0.0);
            }
            scored.push((d, total));
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(n_candidates);
        scored.into_iter().map(|(d, _)| d).collect()
    }
}

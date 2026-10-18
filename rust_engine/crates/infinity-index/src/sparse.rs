//! Sparse retrieval ported from the reference's BMP index
//! (`src/storage/knn_index/sparse/{bmp_ivt,bmp_fwd,bmp_alg}.cppm`).
//!
//! The index splits the document space into blocks of [`BMP_BLOCK_SIZE`] rows.
//! For every term it keeps, per block, the posting offsets/scores and the
//! block's maximum score, plus the term's k-th largest score. A query then:
//!
//!   1. derives `threshold = max(term_score * kth(topk))`,
//!   2. accumulates a per-block upper bound from the terms' block maxima
//!      (`Calculate2`),
//!   3. visits blocks with `upper_bound >= threshold`, highest bound first,
//!   4. scores the block exactly and stops once `upper_bound * alpha` is below
//!      the current k-th score,
//!   5. optionally scans the tail rows that do not fill a whole block.
//!
//! This replaces the executor's brute-force "dot product over every row".

use std::collections::{BinaryHeap, HashMap};

/// `BMP_BLOCK_SIZE` (`src/common/default_values.cppm:184`).
pub const BMP_BLOCK_SIZE: usize = 16;

/// One term's postings, grouped by block.
#[derive(Debug, Default, Clone)]
struct TermPostings {
    /// `(block id, [(offset in block, score)])`.
    blocks: Vec<(u32, Vec<(u16, f32)>)>,
    /// Maximum score per block, parallel to `blocks`.
    block_max: Vec<f32>,
    /// Scores across all documents, descending (for `kth(topk)`).
    sorted_scores: Vec<f32>,
}

impl TermPostings {
    fn kth(&self, topk: usize) -> f32 {
        if topk == 0 {
            return 0.0;
        }
        self.sorted_scores.get(topk - 1).copied().unwrap_or(0.0)
    }
}

/// A sparse vector: `(index, value)` pairs sorted by index.
pub type SparseVec = Vec<(u32, f32)>;

/// Block-max sparse index over a fixed document set.
#[derive(Debug, Default)]
pub struct BmpIndex {
    block_size: usize,
    docs: Vec<SparseVec>,
    terms: HashMap<u32, TermPostings>,
}

impl BmpIndex {
    pub fn builder(block_size: usize) -> Self {
        Self {
            block_size: block_size.max(1),
            docs: Vec::new(),
            terms: HashMap::new(),
        }
    }

    /// Appends a document (`docs.len()` is its id).
    pub fn add(&mut self, doc: &[(u32, f32)]) -> usize {
        let doc_id = self.docs.len();
        let block = (doc_id / self.block_size) as u32;
        let offset = (doc_id % self.block_size) as u16;
        for &(term, score) in doc {
            let entry = self.terms.entry(term).or_default();
            match entry.blocks.last_mut() {
                Some((b, list)) if *b == block => list.push((offset, score)),
                _ => {
                    entry.blocks.push((block, vec![(offset, score)]));
                    entry.block_max.push(score);
                }
            }
            let max = entry.block_max.last_mut().expect("just pushed");
            if score > *max {
                *max = score;
            }
            entry.sorted_scores.push(score);
        }
        self.docs.push(doc.to_vec());
        doc_id
    }

    pub fn len(&self) -> usize {
        self.docs.len()
    }

    pub fn is_empty(&self) -> bool {
        self.docs.is_empty()
    }

    /// The stored sparse vector of a document (for persisting a segment).
    pub fn doc(&self, doc: usize) -> &[(u32, f32)] {
        self.docs.get(doc).map(Vec::as_slice).unwrap_or(&[])
    }

    /// Finalises the per-term k-th score table.
    pub fn build(&mut self) {
        for entry in self.terms.values_mut() {
            entry
                .sorted_scores
                .sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
        }
    }

    /// `BMPAlgBase::SearchKnn` (`bmp_alg.cppm:53`).
    ///
    /// `alpha` is the block-pruning factor, `beta` keeps the top `beta` share of
    /// the query terms (1.0 = all), `use_tail` scans the rows after the last
    /// full block.
    pub fn search(
        &self,
        query: &[(u32, f32)],
        topk: usize,
        alpha: f32,
        beta: f32,
        use_tail: bool,
    ) -> Vec<(usize, f32)> {
        if topk == 0 || self.docs.is_empty() {
            return Vec::new();
        }
        let topk = topk.min(self.docs.len());

        // `beta < 1` keeps the strongest query terms.
        let mut q: Vec<(u32, f32)> = query.to_vec();
        if beta < 1.0 {
            let keep = ((query.len() as f32) * beta).ceil() as usize;
            q.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
            q.truncate(keep.max(1));
            q.sort_by_key(|(term, _)| *term);
        }

        let block_num = self.docs.len() / self.block_size;
        let mut upper_bounds = vec![0.0f32; block_num.max(1)];
        let mut threshold = 0.0f32;
        for &(term, q_score) in &q {
            let Some(postings) = self.terms.get(&term) else {
                continue;
            };
            threshold = threshold.max(q_score * postings.kth(topk));
            for (i, (block, _)) in postings.blocks.iter().enumerate() {
                if (*block as usize) < upper_bounds.len() {
                    upper_bounds[*block as usize] += q_score * postings.block_max[i];
                }
            }
        }

        // A block whose bound equals the threshold within float rounding still
        // holds candidates (`>=` in `bmp_alg.cppm`), so compare with an epsilon.
        let cutoff = threshold * (1.0 - 1e-6);
        let mut block_scores: Vec<(f32, u32)> = upper_bounds
            .iter()
            .enumerate()
            .filter(|(_, ub)| **ub >= cutoff)
            .map(|(id, ub)| (*ub, id as u32))
            .collect();
        block_scores.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));

        // Min-heap of the current best (score, doc); `Reverse` keeps the worst
        // on top, matching `HeapResultHandler<CompareMin>`.
        let mut best: BinaryHeap<std::cmp::Reverse<OrdF32>> = BinaryHeap::new();

        let mut stopped = false;
        for (ub_score, block_id) in block_scores {
            let start = block_id as usize * self.block_size;
            for doc in start..(start + self.block_size).min(self.docs.len()) {
                let score = dot(query, &self.docs[doc]);
                push_topk(&mut best, score, doc, topk);
            }
            if best.len() == topk {
                let min_score = best.peek().map(|r| (r.0).0).unwrap_or(0.0);
                // Same float-rounding tolerance as the block filter: a block
                // whose bound only ties the k-th score can still hold it.
                if ub_score * alpha < min_score * (1.0 - 1e-6) {
                    stopped = true;
                    break;
                }
            }
        }
        if use_tail && !stopped {
            for doc in block_num * self.block_size..self.docs.len() {
                let score = dot(query, &self.docs[doc]);
                push_topk(&mut best, score, doc, topk);
            }
        }

        let mut out: Vec<(usize, f32)> = best.into_iter().map(|r| ((r.0).1, (r.0).0)).collect();
        out.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        out
    }
}

/// `HeapResultHandler::AddResult`: keep the largest `topk` scores.
fn push_topk(
    best: &mut BinaryHeap<std::cmp::Reverse<OrdF32>>,
    score: f32,
    doc: usize,
    topk: usize,
) {
    best.push(std::cmp::Reverse(OrdF32(score, doc)));
    if best.len() > topk {
        best.pop();
    }
}

/// `(score, doc)` ordered by score then doc id, for the heap.
#[derive(Debug, Clone, Copy, PartialEq)]
struct OrdF32(f32, usize);
impl Eq for OrdF32 {}
impl PartialOrd for OrdF32 {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}
impl Ord for OrdF32 {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.0
            .partial_cmp(&other.0)
            .unwrap_or(std::cmp::Ordering::Equal)
            .then(self.1.cmp(&other.1))
    }
}

/// Inner product of two index-sorted sparse vectors.
fn dot(a: &[(u32, f32)], b: &[(u32, f32)]) -> f32 {
    let (mut i, mut j, mut s) = (0usize, 0usize, 0.0f32);
    while i < a.len() && j < b.len() {
        match a[i].0.cmp(&b[j].0) {
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
            std::cmp::Ordering::Equal => {
                s += a[i].1 * b[j].1;
                i += 1;
                j += 1;
            }
        }
    }
    s
}

#[cfg(test)]
mod tests {
    use super::*;

    fn dataset(n: usize, nnz: usize, seed: u64) -> Vec<SparseVec> {
        let mut state = seed | 1;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        (0..n)
            .map(|_| {
                let mut terms: Vec<(u32, f32)> = (0..nnz)
                    .map(|_| ((next() % 64) as u32, ((next() % 100) as f32) / 100.0))
                    .collect();
                terms.sort_by_key(|(t, _)| *t);
                terms.dedup_by_key(|(t, _)| *t);
                terms
            })
            .collect()
    }

    #[test]
    fn matches_brute_force() {
        let data = dataset(500, 8, 7);
        let mut index = BmpIndex::builder(BMP_BLOCK_SIZE);
        for d in &data {
            index.add(d);
        }
        index.build();

        let mut state = 99u64;
        for _ in 0..20 {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            // One term: the block bound equals the term's max, so pruning is
            // exact and the result must match the brute-force top-k.
            let query: Vec<(u32, f32)> =
                vec![((state % 64) as u32, ((state % 90) as f32 + 10.0) / 100.0)];
            let mut query = query;
            query.sort_by_key(|(t, _)| *t);
            query.dedup_by_key(|(t, _)| *t);

            let mut exact: Vec<(usize, f32)> = data
                .iter()
                .enumerate()
                .map(|(i, d)| (i, dot(&query, d)))
                .filter(|(_, s)| *s > 0.0)
                .collect();
            exact.sort_by(|a, b| {
                b.1.partial_cmp(&a.1)
                    .unwrap_or(std::cmp::Ordering::Equal)
                    .then(a.0.cmp(&b.0))
            });
            exact.truncate(10);

            let got: Vec<(usize, f32)> = index
                .search(&query, 10, 1.0, 1.0, true)
                .into_iter()
                .filter(|(_, s)| *s > 0.0)
                .collect();
            // Block pruning may resolve equal-score ties differently from the
            // brute-force order (both are valid), so compare the score profile.
            assert_eq!(got.len(), exact.len());
            for (g, e) in got.iter().zip(exact.iter()) {
                assert!((g.1 - e.1).abs() < 1e-5, "got={got:?} exact={exact:?}");
            }
        }
    }

    #[test]
    fn beta_keeps_the_strongest_terms() {
        let data = dataset(200, 4, 11);
        let mut index = BmpIndex::builder(BMP_BLOCK_SIZE);
        for d in &data {
            index.add(d);
        }
        index.build();
        let query = vec![(1u32, 0.9f32), (2, 0.1), (3, 0.2)];
        let all = index.search(&query, 5, 0.9, 1.0, true);
        // beta = 0.34 keeps only the first term; the result must still be a
        // prefix-consistent top-k over that reduced query.
        let reduced = index.search(&query, 5, 0.9, 0.34, true);
        assert!(reduced.len() <= all.len());
        assert!(reduced
            .iter()
            .all(|(doc, _)| all.iter().any(|(d, _)| d == doc)));
    }
}

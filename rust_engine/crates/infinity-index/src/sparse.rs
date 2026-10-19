//! Block-max sparse retrieval, ported class by class from the reference
//! (`src/storage/knn_index/sparse/{bmp_fwd,bmp_ivt,bmp_alg}.cppm`).
//!
//! [`BlockFwd`] groups documents into blocks of [`BMP_BLOCK_SIZE`] and keeps,
//! per block, every term with its within-block offsets and scores.
//! [`BMPIvt`] is the inverted index over completed blocks: per term, the block
//! ids, the block maxima and the postings. [`BmpAlg`] (= C++ `BMPAlg`) wires
//! the two together and implements `SearchKnn`.
//!
//! The reference's mmap variants (`BMPOwnMem::kFalse`) are represented by
//! `infinity-storage::bmp_cpp`, which reads the same byte layout.
//!
//! A query:
//!
//!   1. derives `threshold = max(term_score * kth(topk))`,
//!   2. accumulates a per-block upper bound from the terms' block maxima
//!      (`Calculate2`),
//!   3. visits blocks with `upper_bound >= threshold`, highest bound first,
//!   4. scores the block from the forward index (`GetScores`) and stops once
//!      `upper_bound * alpha` is below the current k-th score,
//!   5. optionally scores the tail rows that do not fill a whole block.

use std::collections::{BinaryHeap, HashMap};

/// `BMP_BLOCK_SIZE` (`src/common/default_values.cppm:184`).
pub const BMP_BLOCK_SIZE: usize = 16;

/// A sparse vector: `(index, value)` pairs sorted by index.
pub type SparseVec = Vec<(u32, f32)>;

/// `BMPAlgBase::SearchKnn` options.
/// C++: `BmpSearchOptions` (`bmp_util.cppm:104`).
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct BmpSearchOptions {
    pub alpha: f32,
    pub beta: f32,
    pub use_tail: bool,
}

impl Default for BmpSearchOptions {
    fn default() -> Self {
        Self {
            alpha: 1.0,
            beta: 1.0,
            use_tail: true,
        }
    }
}

/// One term's postings inside a block.
/// C++: an entry of `BlockTerms<..., kTrue>` / `BlockTermsIter::Value`.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct BlockTerm {
    pub term_id: u32,
    /// Offsets of the matching documents inside the block.
    pub block_offsets: Vec<u16>,
    /// Score of each document, parallel to `block_offsets`.
    pub values: Vec<f32>,
}

/// Documents buffered until a block fills.
/// C++: `TailFwd<DataType, IdxType>` (`bmp_fwd.cppm`).
#[derive(Clone, Debug, Default)]
pub struct TailFwd {
    docs: Vec<SparseVec>,
}

impl TailFwd {
    pub fn new() -> Self {
        Self::default()
    }

    /// C++: `TailFwd::AddDoc`; returns the number of buffered documents.
    pub fn add_doc(&mut self, doc: &[(u32, f32)]) -> usize {
        self.docs.push(doc.to_vec());
        self.docs.len()
    }

    pub fn len(&self) -> usize {
        self.docs.len()
    }

    pub fn is_empty(&self) -> bool {
        self.docs.is_empty()
    }

    /// C++: `GetTailTerms`.
    pub fn get_tail_terms(&self) -> &[SparseVec] {
        &self.docs
    }

    /// C++: `ToBlockFwd` — groups the buffered documents by term. The
    /// reference sorts by term only; ties are ordered by document offset here
    /// so the block layout is deterministic.
    pub fn to_block_fwd(&self) -> Vec<BlockTerm> {
        let mut pairs: Vec<(u32, u16, f32)> = Vec::new();
        for (offset, doc) in self.docs.iter().enumerate() {
            for &(term, value) in doc {
                pairs.push((term, offset as u16, value));
            }
        }
        pairs.sort_by(|a, b| a.0.cmp(&b.0).then(a.1.cmp(&b.1)));
        let mut out: Vec<BlockTerm> = Vec::new();
        for (term, offset, value) in pairs {
            match out.last_mut() {
                Some(last) if last.term_id == term => {
                    last.block_offsets.push(offset);
                    last.values.push(value);
                }
                _ => out.push(BlockTerm {
                    term_id: term,
                    block_offsets: vec![offset],
                    values: vec![value],
                }),
            }
        }
        out
    }

    /// C++: `GetScores` — dot product of each buffered document with the
    /// query, in document order.
    pub fn get_scores(&self, query: &[(u32, f32)]) -> Vec<f32> {
        let mut res = vec![0.0f32; self.docs.len()];
        for (offset, doc) in self.docs.iter().enumerate() {
            let mut j = 0;
            for &(term, q_score) in query {
                while j < doc.len() && doc[j].0 < term {
                    j += 1;
                }
                if j == doc.len() {
                    break;
                }
                if doc[j].0 == term {
                    res[offset] += q_score * doc[j].1;
                    j += 1;
                }
            }
        }
        res
    }
}

/// The forward index: per block, its terms sorted by term id.
/// C++: `BlockFwd<DataType, IdxType, BMPOwnMem::kTrue>` (`bmp_fwd.cppm`).
#[derive(Clone, Debug)]
pub struct BlockFwd {
    block_size: usize,
    blocks: Vec<Vec<BlockTerm>>,
    tail: TailFwd,
}

impl BlockFwd {
    /// C++: `BlockFwd(block_size)`.
    pub fn new(block_size: usize) -> Self {
        Self {
            block_size: block_size.max(1),
            blocks: Vec::new(),
            tail: TailFwd::new(),
        }
    }

    /// C++: `AddDoc` — buffers a document and returns the documents of a block
    /// that just completed (the caller feeds them to the inverted index).
    pub fn add_doc(&mut self, doc: &[(u32, f32)]) -> Option<TailFwd> {
        if self.tail.add_doc(doc) < self.block_size {
            return None;
        }
        let done = std::mem::take(&mut self.tail);
        self.blocks.push(done.to_block_fwd());
        Some(done)
    }

    /// C++: `GetBlockTerms`.
    pub fn get_block_terms(&self, block_id: usize) -> &[BlockTerm] {
        self.blocks.get(block_id).map(Vec::as_slice).unwrap_or(&[])
    }

    /// C++: `GetScoresTail`.
    pub fn get_scores_tail(&self, query: &[(u32, f32)]) -> Vec<f32> {
        self.tail.get_scores(query)
    }

    /// C++: `Prefetch` (a SIMD hint; nothing to do here).
    pub fn prefetch(&self, _block_id: usize) {}

    /// C++: `GetTailFwd`.
    pub fn take_tail_fwd(&mut self) -> TailFwd {
        std::mem::take(&mut self.tail)
    }

    pub fn block_size(&self) -> usize {
        self.block_size
    }

    /// Number of *complete* blocks; the partial block lives in the tail.
    pub fn block_num(&self) -> usize {
        self.blocks.len()
    }
}

/// Per-term postings over completed blocks: the block ids with their maximum
/// score, plus every score for the top-k threshold.
/// C++: `BlockPostings` + `BlockData<..., kCompressed, kTrue>` (`bmp_ivt.cppm`).
#[derive(Clone, Debug, Default)]
pub struct Postings {
    block_ids: Vec<u32>,
    block_max: Vec<f32>,
    /// Scores across all documents, descending (for `kth(topk)`).
    sorted_scores: Vec<f32>,
}

impl Postings {
    /// C++: `BlockPostings::kth` — the k-th largest score of the term.
    pub fn kth(&self, topk: usize) -> f32 {
        if topk == 0 {
            return 0.0;
        }
        self.sorted_scores.get(topk - 1).copied().unwrap_or(0.0)
    }

    /// C++: `BlockData::block_ids`.
    pub fn block_ids(&self) -> &[u32] {
        &self.block_ids
    }

    /// C++: `BlockData::max_scores`.
    pub fn max_scores(&self) -> &[f32] {
        &self.block_max
    }
}

/// The inverted index over completed blocks.
/// C++: `BMPIvt<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>`.
#[derive(Clone, Debug, Default)]
pub struct BMPIvt {
    postings: HashMap<u32, Postings>,
    term_num: usize,
}

impl BMPIvt {
    /// C++: `BMPIvt(term_num)`.
    pub fn new(term_num: usize) -> Self {
        Self {
            postings: HashMap::new(),
            term_num,
        }
    }

    /// C++: `AddBlock` — records, for every term present in the block, the
    /// block id and the term's maximum score in that block.
    pub fn add_block(&mut self, block_id: u32, docs: &[SparseVec]) {
        // C++ `std::unordered_map<IdxType, DataType> max_scores`.
        let mut max_scores: HashMap<u32, f32> = HashMap::new();
        for doc in docs {
            for &(term, score) in doc {
                let entry = max_scores.entry(term).or_insert(score);
                if score > *entry {
                    *entry = score;
                }
            }
        }
        for (term, max) in max_scores {
            let entry = self.postings.entry(term).or_default();
            entry.block_ids.push(block_id);
            entry.block_max.push(max);
            for doc in docs {
                for &(t, score) in doc {
                    if t == term {
                        entry.sorted_scores.push(score);
                    }
                }
            }
        }
    }

    /// C++: `GetPostings`.
    pub fn get_postings(&self, term: u32) -> Option<&Postings> {
        self.postings.get(&term)
    }

    /// C++: `Optimize(topk, ivt_scores)` — settles the k-th score table.
    /// The reference keeps one k-th value per term; we keep the full ordered
    /// list so any `topk` can be answered.
    pub fn build(&mut self) {
        for postings in self.postings.values_mut() {
            postings
                .sorted_scores
                .sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
        }
    }

    /// C++: `term_num`.
    pub fn term_num(&self) -> usize {
        self.term_num.max(self.postings.len())
    }
}

/// Block-max pruning index: `BMPIvt` + `BlockFwd` + the document ids.
/// C++: `BMPAlg<DataType, IdxType, BMPCompressType::kCompressed, kTrue>`.
#[derive(Clone, Debug)]
pub struct BmpAlg {
    ivt: BMPIvt,
    fwd: BlockFwd,
    docs: Vec<SparseVec>,
}

impl Default for BmpAlg {
    fn default() -> Self {
        Self::new(BMP_BLOCK_SIZE)
    }
}

impl BmpAlg {
    /// C++: `BMPAlg(term_num, block_size)`.
    pub fn new(block_size: usize) -> Self {
        Self {
            ivt: BMPIvt::new(0),
            fwd: BlockFwd::new(block_size),
            docs: Vec::new(),
        }
    }

    /// C++: `AddDoc` — appends a document (`docs.len()` is its id) and, when a
    /// block completes, feeds the block to the inverted index.
    pub fn add_doc(&mut self, doc: &[(u32, f32)]) -> usize {
        let doc_id = self.docs.len();
        self.docs.push(doc.to_vec());
        if let Some(block) = self.fwd.add_doc(doc) {
            let block_id = self.fwd.block_num() as u32 - 1;
            self.ivt.add_block(block_id, block.get_tail_terms());
        }
        doc_id
    }

    /// C++: `BMPIvt::Optimize` — must run before [`BmpAlg::search_knn`].
    pub fn build(&mut self) {
        self.ivt.build();
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

    /// `BMPAlgBase::SearchKnn` (`bmp_alg.cppm:53`).
    pub fn search_knn(
        &self,
        query: &[(u32, f32)],
        topk: usize,
        options: BmpSearchOptions,
    ) -> Vec<(usize, f32)> {
        self.search_knn_filtered(query, topk, options, None)
    }

    /// C++: `BMPAlgBase::SearchKnn` with the reference's per-document filter
    /// (`KnnFilter`/`BitmaskFilter`): only ids where `allowed[id]` is true are
    /// scored into the result heap.
    pub fn search_knn_filtered(
        &self,
        query: &[(u32, f32)],
        topk: usize,
        options: BmpSearchOptions,
        allowed: Option<&[bool]>,
    ) -> Vec<(usize, f32)> {
        if topk == 0 || self.docs.is_empty() {
            return Vec::new();
        }
        let topk = topk.min(self.docs.len());

        // `beta < 1` keeps the strongest query terms.
        let mut q: Vec<(u32, f32)> = query.to_vec();
        if options.beta < 1.0 {
            let keep = ((query.len() as f32) * options.beta).ceil() as usize;
            q.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
            q.truncate(keep.max(1));
            q.sort_by_key(|(term, _)| *term);
        }

        let block_size = self.fwd.block_size();
        let block_num = self.fwd.block_num();
        let mut upper_bounds = vec![0.0f32; block_num.max(1)];
        let mut threshold = 0.0f32;
        for &(term, q_score) in &q {
            let Some(postings) = self.ivt.get_postings(term) else {
                continue;
            };
            threshold = threshold.max(q_score * postings.kth(topk));
            for (i, block) in postings.block_ids().iter().enumerate() {
                if (*block as usize) < upper_bounds.len() {
                    upper_bounds[*block as usize] += q_score * postings.max_scores()[i];
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
            self.fwd.prefetch(block_id as usize);
            let terms = self.fwd.get_block_terms(block_id as usize);
            let scores = get_scores(terms, &q, block_size);
            let start = block_id as usize * block_size;
            for (offset, score) in scores.into_iter().enumerate() {
                let doc = start + offset;
                if doc < self.docs.len()
                    && allowed.is_none_or(|a| a.get(doc).copied().unwrap_or(false))
                {
                    push_topk(&mut best, score, doc, topk);
                }
            }
            if best.len() == topk {
                let min_score = best.peek().map(|r| (r.0).0).unwrap_or(0.0);
                // Same float-rounding tolerance as the block filter: a block
                // whose bound only ties the k-th score can still hold it.
                if ub_score * options.alpha < min_score * (1.0 - 1e-6) {
                    stopped = true;
                    break;
                }
            }
        }
        if options.use_tail && !stopped {
            for (i, score) in self.fwd.get_scores_tail(&q).into_iter().enumerate() {
                let doc = block_num * block_size + i;
                if allowed.is_none_or(|a| a.get(doc).copied().unwrap_or(false)) {
                    push_topk(&mut best, score, doc, topk);
                }
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

/// C++: `BMPAlgBase::GetScores` — accumulates each block term's postings into
/// the block's per-document scores.
fn get_scores(terms: &[BlockTerm], query: &[(u32, f32)], block_size: usize) -> Vec<f32> {
    let mut res = vec![0.0f32; block_size];
    let mut i = 0;
    for term in terms {
        while i < query.len() && query[i].0 < term.term_id {
            i += 1;
        }
        if i == query.len() {
            break;
        }
        if query[i].0 == term.term_id {
            let q_score = query[i].1;
            for (offset, value) in term.block_offsets.iter().zip(&term.values) {
                res[*offset as usize] += q_score * value;
            }
        }
    }
    res
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
#[cfg(test)]
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
        let mut next = || -> u64 {
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

    /// Ported from `test_bmp_fwd_ut.cpp`: every document's terms must be
    /// recoverable from the block terms, and only complete blocks are kept.
    #[test]
    fn block_fwd_groups_docs_into_blocks() {
        let data = dataset(1000, 5, 3);
        let mut fwd = BlockFwd::new(BMP_BLOCK_SIZE);
        let mut completed = 0;
        for doc in &data {
            if fwd.add_doc(doc).is_some() {
                completed += 1;
            }
        }
        assert_eq!(fwd.block_size(), BMP_BLOCK_SIZE);
        assert_eq!(fwd.block_num(), 1000 / BMP_BLOCK_SIZE);
        assert_eq!(completed, fwd.block_num());

        for block_id in 0..fwd.block_num() {
            let terms = fwd.get_block_terms(block_id);
            let mut rebuilt: Vec<SparseVec> = vec![Vec::new(); BMP_BLOCK_SIZE];
            for term in terms {
                for (offset, value) in term.block_offsets.iter().zip(&term.values) {
                    rebuilt[*offset as usize].push((term.term_id, *value));
                }
            }
            for (offset, doc) in rebuilt.iter_mut().enumerate() {
                doc.sort_by_key(|(t, _)| *t);
                let src = &data[block_id * BMP_BLOCK_SIZE + offset];
                assert_eq!(doc, src, "block {block_id} offset {offset}");
            }
            // Terms are sorted so `GetScores` can walk the query once.
            assert!(terms.windows(2).all(|w| w[0].term_id < w[1].term_id));
        }
        // The partial block stays in the tail.
        assert_eq!(fwd.take_tail_fwd().len(), 1000 % BMP_BLOCK_SIZE);
    }

    /// Ported from `test_bmp_ivt_ut.cpp`: per term the postings keep the block
    /// ids, the block maxima and the k-th score.
    /// C++: `BMPIndexTest.test2` (`test_bmp_index_ut.cpp`): six documents in
    /// blocks of two; the first document has the strongest match.
    #[test]
    fn bmp_index_ranks_documents_across_blocks() {
        let options = BmpSearchOptions {
            alpha: 1.0,
            beta: 1.0,
            use_tail: true,
        };
        let mut index = BmpAlg::new(2);
        index.add_doc(&[(0, 3.0)]);
        index.add_doc(&[(1, 1.0)]);
        for _ in 0..4 {
            index.add_doc(&[(0, 1.0), (1, 1.0)]);
        }
        index.build();
        let hits = index.search_knn(&[(0, 1.0), (1, 1.0)], 5, options);
        let ids: Vec<usize> = hits.iter().map(|(doc, _)| *doc).collect();
        let scores: Vec<f32> = hits.iter().map(|(_, score)| *score).collect();
        assert_eq!(ids, vec![0, 2, 3, 4, 5]);
        assert_eq!(scores, vec![3.0, 2.0, 2.0, 2.0, 2.0]);
    }

    #[test]
    fn bmp_ivt_keeps_block_maxima_and_kth() {
        let data = dataset(1000, 5, 5);
        let mut alg = BmpAlg::new(BMP_BLOCK_SIZE);
        for doc in &data {
            alg.add_doc(doc);
        }
        alg.build();

        // Recompute the expected per-term block maxima from the raw documents.
        let block_num = alg.fwd.block_num();
        let mut expected: HashMap<u32, Vec<f32>> = HashMap::new();
        for block_id in 0..block_num {
            let mut max: HashMap<u32, f32> = HashMap::new();
            for doc in &data[block_id * BMP_BLOCK_SIZE..(block_id + 1) * BMP_BLOCK_SIZE] {
                for &(term, score) in doc {
                    let e = max.entry(term).or_insert(score);
                    if score > *e {
                        *e = score;
                    }
                }
            }
            for (term, score) in max {
                expected.entry(term).or_default().push(score);
            }
        }
        for (term, maxima) in expected {
            let postings = alg.ivt.get_postings(term).expect("term present");
            assert_eq!(postings.block_ids().len(), maxima.len());
            assert_eq!(postings.max_scores(), maxima.as_slice());
            // `kth` is the k-th largest term score across the *complete* blocks
            // (`Optimize` is fed `GetIvtScores`, which excludes the tail).
            let mut all: Vec<f32> = data[..block_num * BMP_BLOCK_SIZE]
                .iter()
                .map(|d| {
                    d.iter()
                        .find(|(t, _)| *t == term)
                        .map(|(_, s)| *s)
                        .unwrap_or(0.0)
                })
                .collect();
            all.sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
            for k in [1usize, 10, 100] {
                assert_eq!(postings.kth(k), all[k - 1], "term {term} k {k}");
            }
        }
    }

    #[test]
    fn matches_brute_force() {
        let data = dataset(500, 8, 7);
        let mut index = BmpAlg::new(BMP_BLOCK_SIZE);
        for d in &data {
            index.add_doc(d);
        }
        index.build();

        let mut state = 99u64;
        for _ in 0..20 {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            // One term: the block bound equals the term's max, so pruning is
            // exact and the result must match the brute-force top-k.
            let mut query: Vec<(u32, f32)> =
                vec![((state % 64) as u32, ((state % 90) as f32 + 10.0) / 100.0)];
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
                .search_knn(&query, 10, BmpSearchOptions::default())
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
        let mut index = BmpAlg::new(BMP_BLOCK_SIZE);
        for d in &data {
            index.add_doc(d);
        }
        index.build();
        let query = vec![(1u32, 0.9f32), (2, 0.1), (3, 0.2)];
        let all = index.search_knn(&query, 5, BmpSearchOptions::default());
        // beta = 0.34 keeps only the first term; the result must still be a
        // prefix-consistent top-k over that reduced query.
        let reduced = index.search_knn(
            &query,
            5,
            BmpSearchOptions {
                beta: 0.34,
                ..Default::default()
            },
        );
        assert!(reduced.len() <= all.len());
        assert!(reduced
            .iter()
            .all(|(doc, _)| all.iter().any(|(d, _)| d == doc)));
    }

    #[test]
    fn tail_documents_are_scored_when_the_block_does_not_fill() {
        let data = dataset(BMP_BLOCK_SIZE + 3, 4, 13);
        let mut index = BmpAlg::new(BMP_BLOCK_SIZE);
        for d in &data {
            index.add_doc(d);
        }
        index.build();
        assert_eq!(index.fwd.block_num(), 1);

        let query = vec![(7u32, 1.0f32)];
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
        let got = index.search_knn(&query, exact.len(), BmpSearchOptions::default());
        assert_eq!(got.len(), exact.len());
        for (g, e) in got.iter().zip(exact.iter()) {
            assert_eq!(g.0, e.0);
            assert!((g.1 - e.1).abs() < 1e-6);
        }
    }
}

#[cfg(test)]
mod bmp_reference_suite {
    //! Ports of `BMPIndexTest.test1` (`test_bmp_index_ut.cpp`): search
    //! accuracy against the exact ground truth, before and after a disk
    //! round trip. The C++ test instantiates four type/compression
    //! combinations; the Rust implementation is type-erased, so the two block
    //! layouts are exercised instead.

    use super::*;
    use infinity_storage::bmp_cpp::{write_bmp_file, BmpCppReader, IdxWidth};

    fn dataset(rows: usize, cols: usize, sparsity: f32) -> Vec<Vec<(u32, f32)>> {
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut next = || -> u64 {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        (0..rows)
            .map(|_| {
                let count = (cols as f32 * sparsity) as usize;
                let mut seen = std::collections::HashSet::new();
                let mut row = Vec::with_capacity(count);
                while row.len() < count {
                    let term = (next() % cols as u64) as u32;
                    if seen.insert(term) {
                        let value = (next() % 10_000) as f32 / 1000.0;
                        row.push((term, value));
                    }
                }
                row.sort_by_key(|(term, _)| *term);
                row
            })
            .collect()
    }

    fn groundtruth(
        dataset: &[Vec<(u32, f32)>],
        query: &[(u32, f32)],
        topk: usize,
        cols: usize,
    ) -> Vec<u32> {
        let mut scores = vec![0.0f32; dataset.len()];
        for &(term, value) in query {
            let _ = cols;
            for (row, doc) in dataset.iter().enumerate() {
                if let Some((_, doc_value)) = doc.iter().find(|(t, _)| *t == term) {
                    scores[row] += value * doc_value;
                }
            }
        }
        let mut hits: Vec<(u32, f32)> = scores
            .into_iter()
            .enumerate()
            .map(|(row, score)| (row as u32, score))
            .collect();
        hits.sort_by(|a, b| b.1.total_cmp(&a.1));
        hits.truncate(topk);
        hits.into_iter().map(|(row, _)| row).collect()
    }

    fn assert_accuracy(hits: &[(usize, f32)], gt: &[u32]) -> (usize, usize) {
        let got: std::collections::HashSet<u32> = hits.iter().map(|(doc, _)| *doc as u32).collect();
        (gt.iter().filter(|row| got.contains(row)).count(), gt.len())
    }

    /// C++: `BMPIndexTest.test1`.
    #[test]
    fn bmp_index_accuracy_and_disk_roundtrip() {
        let rows = 1000;
        let cols = 1000;
        let corpus = dataset(rows, cols, 0.05);
        let queries = dataset(100, cols, 0.05);
        let gt: Vec<Vec<u32>> = queries
            .iter()
            .map(|q| groundtruth(&corpus, q, 10, cols))
            .collect();

        for block_size in [8usize, 64] {
            let options = BmpSearchOptions {
                alpha: 1.0,
                beta: 1.0,
                use_tail: true,
            };
            let mut index = BmpAlg::new(block_size);
            for doc in &corpus {
                index.add_doc(doc);
            }
            index.build();
            let mut hit = 0;
            let mut total = 0;
            for (q, gt) in queries.iter().zip(&gt) {
                let query: Vec<(u32, f32)> = q.clone();
                let hits = index.search_knn(&query, 10, options);
                let (h, t) = assert_accuracy(&hits, gt);
                hit += h;
                total += t;
            }
            assert!(
                hit as f32 >= total as f32 * 0.9,
                "block {block_size}: {hit}/{total}"
            );

            // Disk round trip: the same corpus through the C++ BMP layout.
            let dir = tempfile::tempdir().unwrap();
            let path = dir.path().join("bmp.idx");
            let docs: Vec<infinity_storage::bmp_cpp::SparseDoc> = corpus
                .iter()
                .enumerate()
                .map(|(row, doc)| {
                    (
                        row as u64,
                        doc.iter().map(|(t, v)| (*t as i32, *v)).collect(),
                    )
                })
                .collect();
            write_bmp_file(&path, block_size, IdxWidth::I32, &docs, cols).unwrap();
            let reader = BmpCppReader::open(&path, IdxWidth::I32).unwrap();
            assert_eq!(reader.doc_count(), rows);
            let mut hit = 0;
            let mut total = 0;
            for (q, gt) in queries.iter().zip(&gt) {
                let query: Vec<(i32, f32)> = q.iter().map(|(t, v)| (*t as i32, *v)).collect();
                let hits = reader.search(&query, 10, 1.0);
                let (h, t) = assert_accuracy(&hits, gt);
                hit += h;
                total += t;
            }
            assert!(
                hit as f32 >= total as f32 * 0.9,
                "disk block {block_size}: {hit}/{total}"
            );
        }
    }
}

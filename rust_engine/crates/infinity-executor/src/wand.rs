//! The reference's BM25 ranker and block-max WAND iterator, ported as classes
//! so the retrieval control flow stays 1:1 with C++ (plan §3.6):
//!
//! * BM25Ranker — src/storage/invertedindex/search/bm25_ranker_impl.cpp
//! * BlockMaxLeafIterator — blockmax_leaf_iterator.cppm, realised over the
//!   merged posting streams. A block is the same fixed run of documents the
//!   reference's DocListEncoder summarises: MAX_DOC_PER_RECORD (128) entries of
//!   one term's posting list, bounded by the largest tf and the largest
//!   tf/doc-length ratio they contain (doc_list_encoder_impl.cpp:69-71,161-168,
//!   consumed by TermDocIterator::BlockMaxBM25Score, term_doc_iterator_impl.cpp:91-102).
//! * BlockMaxWandIterator — src/storage/invertedindex/search/blockmax_wand_iterator_impl.cpp:
//!   FindPivotOptimized (:88), NextShallow (:348-377), the block-skip path of
//!   Next (:449-524) and incremental Next(RowID) (:248).
//!
//! The iterator drives TermStreams instead of materialised posting lists: every
//! cursor reads its documents through SeekDoc, one 128-posting block at a time,
//! from the memory tail, the spilled runs or the sealed segments' doc lists. A
//! query therefore pays for the documents it visits, not for the terms' whole
//! document frequency.

use std::cmp::Reverse;
use std::collections::{BinaryHeap, HashSet};

#[cfg(test)]
use super::WAND_BLOCK;
use super::{WandField, WandStream, WandUnit};

/// MAX_DOC_PER_RECORD (index_defines.cppm:43): the fixed run of documents that
/// forms one posting block, i.e. the granularity of block_max_tf /
/// block_max_percentage. SkipListReader returns one entry per block.
#[cfg(test)]
const BLOCK_SIZE: usize = WAND_BLOCK;

/// numeric_limits<u16>::max(): block_max_percentage is stored as a u16 fraction
/// of the block's largest tf/doc-length ratio (doc_list_encoder_impl.cpp:166).
const PERCENTAGE_SCALE: f64 = u16::MAX as f64;

/// BM25Ranker (bm25_ranker_impl.cpp:25-34): k1 = 1.2, b = 0.75,
/// idf = ln(1 + (N - df + 0.5) / (df + 0.5)), plus the optional delta_term added
/// to the tf norm.
pub(crate) struct Bm25Ranker {
    pub k1: f64,
    pub b: f64,
    pub delta: f64,
}

impl Bm25Ranker {
    pub fn new(delta: f64) -> Self {
        Self {
            k1: 1.2,
            b: 0.75,
            delta,
        }
    }

    pub fn idf(&self, n_docs: f64, df: f64) -> f64 {
        (1.0 + (n_docs - df + 0.5) / (df + 0.5)).ln()
    }

    /// idf * (tf_norm + delta) * weight, with
    /// tf_norm = (k1+1)*tf / (tf + k1*((1-b) + b*dl/avgdl)).
    #[cfg(test)]
    pub fn score(&self, tf: f64, dl: f64, avgdl: f64, idf: f64, weight: f64) -> f64 {
        self.score_with(tf, dl, avgdl, idf, weight, self.delta)
    }

    /// [`Self::score`] with the unit's own `delta` (`delta_term` for a term,
    /// `delta_phrase` for a phrase).
    pub fn score_with(
        &self,
        tf: f64,
        dl: f64,
        avgdl: f64,
        idf: f64,
        weight: f64,
        delta: f64,
    ) -> f64 {
        let norm = (self.k1 + 1.0) * tf
            / (tf + self.k1 * ((1.0 - self.b) + self.b * dl / avgdl.max(1e-9)));
        idf * (norm + delta) * weight
    }

    /// `f4 = delta / (k1 + 1)` for an arbitrary per-unit delta.
    pub fn f4_with(delta: f64) -> f64 {
        delta / 2.2
    }

    /// tf_norm <= k1+1, so this is a safe upper bound for pruning.
    pub fn upper_bound(&self, idf: f64, weight: f64) -> f64 {
        idf * (self.k1 + 1.0 + self.delta.max(0.0)) * weight
    }

    /// bm25_common_score_ (term_doc_iterator_impl.cpp:71):
    /// weight * idf * (k1 + 1). Both the whole-term bound and every block bound
    /// are this factor times a tf-only term.
    pub fn common(&self, idf: f64, weight: f64) -> f64 {
        weight * idf * (self.k1 + 1.0)
    }

    /// f1 = k1 * (1 - b) (term_doc_iterator_impl.cpp:73).
    pub fn f1(&self) -> f64 {
        self.k1 * (1.0 - self.b)
    }

    /// f3 = k1 * b / avgdl * u16::MAX (term_doc_iterator_impl.cpp:74-75), the
    /// tf/doc-length ratio term of BlockMaxBM25Score.
    pub fn f3(&self, avgdl: f64) -> f64 {
        self.k1 * self.b / avgdl.max(1e-9) * PERCENTAGE_SCALE
    }
}

/// similarity=boolean scores one point per matched term (per field); the default
/// is BM25. Both are handled inside the same iterator.
#[derive(Clone, Copy, PartialEq, Eq)]
pub(crate) enum WandMode {
    Bm25,
    Boolean,
}

/// One (field, term) posting cursor: the reference's TermDocIterator.
struct Cursor<'a> {
    /// Position of this cursor in creation order (field-major, then term).
    /// Scores are summed in this order so the result never depends on how the
    /// doc-id sort permuted the cursors.
    ord: usize,
    term: usize,
    weight: f64,
    /// bm25_common_score_; zero for similarity=boolean.
    common: f64,
    /// BM25ScoreUpperBound() of the whole term.
    upper_bound: f64,
    /// f1, f3, f4 of TermDocIterator::InitBM25Info.
    f1: f64,
    f3: f64,
    f4: f64,
    /// Field statistics of this unit's BM25 score.
    avg: f64,
    idf: f64,
    /// `delta_term` for a term, `delta_phrase` for a phrase.
    delta: f64,
    /// The posting stream, read one block at a time.
    stream: WandStream<'a>,
    /// iter_->SeekDoc position, cached so the cursors can be sorted. `None`
    /// with `done == false` means a `seek_block` still has to be resolved.
    cur: Option<u64>,
    /// The unit is exhausted.
    done: bool,
    /// BlockMaxBM25Score() cache: the block it was computed for, its bound and
    /// the block's last document (used by the block-skip path).
    block_index: u64,
    block_max: f64,
    block_last_doc: u64,
}

impl<'a> Cursor<'a> {
    fn refresh(&mut self) {
        self.cur = self.stream.doc();
        self.done = self.cur.is_none();
    }

    /// Resolves a cursor that was only block-seeked.
    fn ensure(&mut self) {
        if !self.done && self.cur.is_none() {
            self.refresh();
        }
    }

    /// PostingIterator::SeekDoc at the block level: the unit may still have to
    /// verify its own positions, which `ensure` pays for - so the range the
    /// WAND skips over is never resolved (`NextShallow`).
    fn seek(&mut self, doc: u64) {
        self.stream.seek_block(doc);
        self.cur = None;
        self.done = false;
    }

    /// BlockLastDocID(): the last document of the current block.
    fn block_last(&mut self) -> u64 {
        self.block_last_doc
    }

    /// BlockMaxBM25Score() (term_doc_iterator_impl.cpp:91-102): the block's
    /// largest tf and largest tf/doc-length ratio, fed through the same BM25
    /// shape as the exact score. Cached per block like the reference's
    /// block_max_bm25_score_cache_end_id_.
    fn block_max(&mut self, mode: WandMode) -> f64 {
        let id = self.stream.block_id();
        if self.block_index == id {
            return self.block_max;
        }
        // `BlockMaxPhraseScore` for a phrase unit: the minimum over its terms',
        // since a phrase can occur no more often than any one of them.
        let (last, max_tf, max_pct) = self.stream.block_stats();
        let value = match mode {
            // One point per matched term, independent of tf and length.
            WandMode::Boolean => self.weight,
            WandMode::Bm25 => {
                let pct_u16 = (max_pct * PERCENTAGE_SCALE).ceil().max(1.0);
                let norm = 1.0 / (1.0 + self.f1 / max_tf.max(1.0) + self.f3 / pct_u16);
                (self.common * (norm + self.f4)).min(self.upper_bound)
            }
        };
        self.block_index = id;
        self.block_max = value;
        self.block_last_doc = last;
        value
    }
}

/// BlockMaxWandIterator over the merged per-field streams. Terms are single-unit
/// OR terms (no phrase / require-all).
pub(crate) struct BlockMaxWandIterator<'a> {
    fields: usize,
    dead: &'a HashSet<u64>,
    k: usize,
    min_should: usize,
    mode: WandMode,
    ranker: Bm25Ranker,
    cursors: Vec<Cursor<'a>>,
    /// The live cursors ordered by document id. The reference keeps a child
    /// array it re-sorts after every reposition; a doc-ordered min-heap gives
    /// FindPivotOptimized the same prefix while only the cursors that actually
    /// move are touched (O(log n) per move instead of O(n log n) per round).
    heap: BinaryHeap<Reverse<(u64, usize)>>,
    /// Sum of upper_bound over the cursors in the heap, i.e. the whole query's
    /// upper bound minus everything already retired.
    total_upper: f64,
    /// The cursors the last pivot parked on the candidate, in doc order. This is
    /// the reference's [0, pivot_] slice: what offer() counts matches over and
    /// what the next round repositions.
    prefix: Vec<usize>,
    /// Score every candidate instead of pruning (the reference's
    /// `EarlyTermAlgo::kNaive`, i.e. `block_max=false`): no pivot, no block
    /// bound, no early termination. It exists to measure what the pruning is
    /// worth, and is never selected unless asked for.
    naive: bool,
    /// Top-k, sorted by score (desc) then doc id (asc), like the scan path.
    best: Vec<(u64, f64)>,
    threshold: f64,
    /// doc_id_ / bm25_score_cache_ of the reference: the last candidate Next()
    /// stopped on and its exact score.
    doc_id: Option<u64>,
    candidate_score: f64,
    /// Instrumentation: exact per-document score decodes (calc_score_cnt_) and
    /// the deepest block any of them came from.
    score_evaluations: usize,
    max_scored_block: u64,
}

impl<'a> BlockMaxWandIterator<'a> {
    pub fn new(
        fields: Vec<WandField<'a>>,
        k: usize,
        dead: &'a HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: WandMode,
    ) -> Self {
        let ranker = Bm25Ranker::new(delta);
        let mut cursors = Vec::new();
        let field_count = fields.len();
        for field in fields {
            let WandField {
                n_docs,
                total_len,
                units,
            } = field;
            let n = n_docs.max(1) as f64;
            let avg = (total_len as f64 / n).max(1e-9);
            for (ui, unit) in units.into_iter().enumerate() {
                let WandUnit {
                    mut stream,
                    weight: w,
                    idf,
                    delta: unit_delta,
                } = unit;
                if stream.doc().is_none() {
                    continue;
                }
                let unit_delta = unit_delta.unwrap_or(delta);
                let (upper_bound, common, f1, f3, f4) = match mode {
                    WandMode::Bm25 => (
                        ranker.upper_bound(idf, w),
                        ranker.common(idf, w),
                        ranker.f1(),
                        ranker.f3(avg),
                        Bm25Ranker::f4_with(unit_delta),
                    ),
                    WandMode::Boolean => (w, 0.0, 0.0, 0.0, 0.0),
                };
                cursors.push(Cursor {
                    ord: cursors.len(),
                    term: ui,
                    weight: w,
                    common,
                    upper_bound,
                    f1,
                    f3,
                    f4,
                    avg,
                    idf,
                    delta: unit_delta,
                    stream,
                    cur: None,
                    done: false,
                    block_index: u64::MAX,
                    block_max: 0.0,
                    block_last_doc: u64::MAX,
                });
                cursors.last_mut().unwrap().refresh();
            }
        }
        let mut heap = BinaryHeap::with_capacity(cursors.len());
        let mut total_upper = 0.0;
        for cursor in &cursors {
            if let Some(doc) = cursor.cur {
                heap.push(Reverse((doc, cursor.ord)));
                total_upper += cursor.upper_bound;
            }
        }
        Self {
            fields: field_count,
            dead,
            k,
            min_should,
            mode,
            ranker,
            cursors,
            heap,
            total_upper,
            prefix: Vec::new(),
            naive: false,
            best: Vec::new(),
            threshold: f64::NEG_INFINITY,
            doc_id: None,
            candidate_score: 0.0,
            score_evaluations: 0,
            max_scored_block: 0,
        }
    }

    /// `block_max=false`: score every document that any unit matches, in
    /// document order, and let only the top-k survive. Same answers as the
    /// pruned path, without the pruning.
    pub fn with_naive(mut self) -> Self {
        self.naive = true;
        self
    }

    /// Drives the reference's Next() loop until the cursors are exhausted or the
    /// remaining upper bound cannot beat the k-th score.
    pub fn run(mut self) -> Vec<(u64, f64)> {
        self.drive();
        self.best
    }

    /// The reference's driver: Next(doc_id + 1) until exhaustion, offering each
    /// surviving candidate to the top-k.
    fn drive(&mut self) {
        if self.k == 0 || self.fields == 0 || self.cursors.is_empty() {
            return;
        }
        let mut target = None;
        while self.next(target) {
            let doc_id = self.doc_id.expect("Next() sets doc_id");
            let score = self.candidate_score;
            self.offer(doc_id, score);
            target = Some(doc_id + 1);
        }
        // How much the pruning actually saved: exact score decodes against the
        // cursors' whole document frequency.
        if std::env::var_os("INFINITY_WAND_STATS").is_some() {
            let df: u64 = self.cursors.iter().map(|c| c.stream.df()).sum();
            eprintln!(
                "[WAND] scored={} cursors={} df_total={}",
                self.score_evaluations,
                self.cursors.len(),
                df
            );
        }
    }

    /// Test-only view of the pruning counters.
    #[cfg(test)]
    fn drive_with_stats(mut self) -> (Vec<(u64, f64)>, usize, u64) {
        self.drive();
        (self.best, self.score_evaluations, self.max_scored_block)
    }

    /// Hands a cursor the last round popped back to the heap, resolving the
    /// block cursor it was seek-ed to. A cursor that ran out is retired: its
    /// whole upper bound leaves the search.
    fn push_back(&mut self, i: usize) {
        let cursor = &mut self.cursors[i];
        cursor.ensure();
        match cursor.cur {
            Some(doc) => self.heap.push(Reverse((doc, cursor.ord))),
            None => self.total_upper -= cursor.upper_bound,
        }
    }

    /// Keeps the candidate if it survives minimum_should_match, the tombstone
    /// filter and the k-th score (ties broken by the smaller doc id).
    fn offer(&mut self, doc_id: u64, score: f64) {
        // minimum_should_match: BM25 counts distinct query units across fields;
        // boolean counts every (field, term) match.
        let matched = match self.mode {
            WandMode::Bm25 => {
                let mut seen = HashSet::new();
                for &i in &self.prefix {
                    seen.insert(self.cursors[i].term);
                }
                seen.len()
            }
            WandMode::Boolean => self.prefix.len(),
        };
        if matched < self.min_should.max(1) || self.dead.contains(&doc_id) {
            return;
        }
        let better = if self.best.len() < self.k {
            true
        } else {
            match self.best.last() {
                Some((worst, worst_score)) => {
                    score > *worst_score || (score == *worst_score && doc_id < *worst)
                }
                None => true,
            }
        };
        if !better {
            return;
        }
        self.best.push((doc_id, score));
        self.best.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        self.best.truncate(self.k);
        if self.best.len() == self.k {
            self.threshold = self.best[self.k - 1].1;
        }
    }

    /// BlockMaxWandIterator::Next(RowID) (blockmax_wand_iterator_impl.cpp:248):
    /// stop on the next document whose block-max bound can still reach the k-th
    /// score, decoding the exact score only for that document. Returns false and
    /// clears doc_id when the iterator is exhausted.
    fn next(&mut self, target: Option<u64>) -> bool {
        if self.naive {
            return self.next_naive(target);
        }
        if let Some(target) = target {
            if self.doc_id.is_some_and(|current| current >= target) {
                return true;
            }
            // Reposition the cursors the last pivot parked on the candidate.
            // Everything else still in the heap already sits above doc_id, so
            // this is the whole of the reference's "lists[0] .. lists[pivot]".
            for i in std::mem::take(&mut self.prefix) {
                let cursor = &mut self.cursors[i];
                if cursor.cur.is_some_and(|doc| doc < target) {
                    cursor.seek(target);
                }
                self.push_back(i);
            }
        }
        loop {
            self.prefix.clear();
            // '<' (not '<='): a document that ties the k-th score can still win
            // on the smaller doc id, and the heap is already doc-ordered, so
            // this needs no extra bookkeeping.
            if self.total_upper < self.threshold {
                break;
            }
            // FindPivotOptimized: the pivot is the first prefix whose upper
            // bound reaches the threshold, so pop just that prefix.
            let mut sum = 0.0;
            let mut pivot = None;
            while let Some(Reverse((doc, i))) = self.heap.pop() {
                sum += self.cursors[i].upper_bound;
                self.prefix.push(i);
                if sum >= self.threshold {
                    pivot = Some(doc);
                    break;
                }
            }
            let Some(doc) = pivot else {
                break;
            };
            // Every other cursor tied on the pivot document joins the prefix.
            while let Some(&Reverse((tied, i))) = self.heap.peek() {
                if tied != doc {
                    break;
                }
                self.heap.pop();
                self.prefix.push(i);
            }
            // If lists[0] is not on the pivot document yet, another list owns it:
            // move the prefix up to it and re-pivot.
            if self.cursors[self.prefix[0]].cur != Some(doc) {
                for i in std::mem::take(&mut self.prefix) {
                    let cursor = &mut self.cursors[i];
                    if cursor.cur.is_some_and(|cur| cur < doc) {
                        cursor.seek(doc);
                    }
                    self.push_back(i);
                }
                continue;
            }
            // NextShallow + BlockMaxBM25Score over the prefix. Every prefix cursor
            // sits on doc, so the sum bounds doc's exact score.
            let mut block_bound = 0.0;
            for k in 0..self.prefix.len() {
                let i = self.prefix[k];
                block_bound += self.cursors[i].block_max(self.mode);
            }
            if block_bound >= self.threshold {
                // Only now is the exact score decoded.
                let score = self.score_prefix();
                self.doc_id = Some(doc);
                self.candidate_score = score;
                return true;
            }
            self.skip_blocks(doc);
            for i in std::mem::take(&mut self.prefix) {
                self.push_back(i);
            }
        }
        self.doc_id = None;
        false
    }

    /// block_max=false: hand back the smallest document any unit matches, with
    /// every cursor parked on it scored. Those cursors leave the heap for the
    /// round exactly like the pruned path's prefix, so nothing is scored twice
    /// and nothing is skipped.
    fn next_naive(&mut self, target: Option<u64>) -> bool {
        if let Some(target) = target {
            if self.doc_id.is_some_and(|current| current >= target) {
                return true;
            }
            for i in std::mem::take(&mut self.prefix) {
                let cursor = &mut self.cursors[i];
                if cursor.cur.is_some_and(|doc| doc < target) {
                    cursor.seek(target);
                }
                self.push_back(i);
            }
        }
        self.prefix.clear();
        let Some(&Reverse((doc, _))) = self.heap.peek() else {
            self.doc_id = None;
            return false;
        };
        while let Some(&Reverse((tied, i))) = self.heap.peek() {
            if tied != doc {
                break;
            }
            self.heap.pop();
            self.prefix.push(i);
        }
        self.candidate_score = self.score_prefix();
        self.doc_id = Some(doc);
        true
    }

    /// The reference's block-skip path (`blockmax_wand_iterator_impl.cpp:449-524`):
    /// while the prefix's block bound cannot reach the k-th score, step every
    /// prefix cursor to the end of its block and try again. Nothing is resolved
    /// here, so the skipped ranges never decode document offsets (`NextShallow`).
    fn skip_blocks(&mut self, mut from: u64) {
        loop {
            // `block_last_doc` of the prefix is fresh: either the caller just
            // computed the block bound, or the loop below did. A block never
            // runs past the next cursor in doc order.
            let up_to = self.heap.peek().map(|&Reverse((doc, _))| doc);
            let mut block_last = u64::MAX;
            for k in 0..self.prefix.len() {
                block_last = block_last.min(self.cursors[self.prefix[k]].block_last());
            }
            if block_last == u64::MAX {
                return;
            }
            let skip_to = match up_to {
                Some(up_to) => (block_last + 1).min(up_to),
                None => block_last + 1,
            };
            if skip_to <= from {
                return;
            }
            for k in 0..self.prefix.len() {
                let i = self.prefix[k];
                self.cursors[i].seek(skip_to);
            }
            // The bound of the blocks the prefix cursors now sit on.
            let mut bound = 0.0;
            let mut last = u64::MAX;
            for k in 0..self.prefix.len() {
                let i = self.prefix[k];
                bound += self.cursors[i].block_max(self.mode);
                last = last.min(self.cursors[i].block_last());
            }
            if bound >= self.threshold || last == u64::MAX {
                return;
            }
            from = skip_to;
        }
    }

    /// Sums the exact BM25Score() of the prefix cursors, all parked on the same
    /// document.
    fn score_prefix(&mut self) -> f64 {
        let ranker = Bm25Ranker::new(self.ranker.delta);
        let mut score = 0.0;
        // Sum in cursor creation order (field-major, then term) so the value is
        // independent of the document order the heap handed us, matching the
        // scan path bit for bit. Doc order is not needed after this point.
        self.prefix.sort_by_key(|&i| self.cursors[i].ord);
        match self.mode {
            WandMode::Bm25 => {
                for k in 0..self.prefix.len() {
                    let i = self.prefix[k];
                    let cursor = &mut self.cursors[i];
                    let tf = f64::from(cursor.stream.tf());
                    let dl = f64::from(cursor.stream.len());
                    score += ranker.score_with(
                        tf,
                        dl,
                        cursor.avg,
                        cursor.idf,
                        cursor.weight,
                        cursor.delta,
                    );
                }
            }
            // similarity=boolean: one point per matched term.
            WandMode::Boolean => {
                for k in 0..self.prefix.len() {
                    score += self.cursors[self.prefix[k]].weight;
                }
            }
        }
        self.score_evaluations += 1;
        for k in 0..self.prefix.len() {
            let block = self.cursors[self.prefix[k]].block_index;
            self.max_scored_block = self.max_scored_block.max(block);
        }
        score
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::TermStream;
    use std::collections::HashMap;

    /// A one-field WAND input over materialised lists (the tests' corpus).
    fn field<'a>(
        postings: &[(&str, &'a [(u64, u32)])],
        doc_len: &'a [(u64, u32)],
        n: usize,
        weights: Vec<f64>,
    ) -> WandField<'a> {
        let ranker = Bm25Ranker::new(0.0);
        let units = postings
            .iter()
            .enumerate()
            .map(|(i, (_term, list))| WandUnit {
                stream: WandStream::Term(TermStream::from_slice(list, doc_len)),
                weight: weights.get(i).copied().unwrap_or(1.0),
                idf: ranker.idf(n.max(1) as f64, list.len() as f64),
                delta: None,
            })
            .collect();
        WandField {
            n_docs: n,
            total_len: doc_len.iter().map(|(_, l)| *l as u64).sum(),
            units,
        }
    }

    #[test]
    fn ranker_matches_the_reference_formula() {
        let r = Bm25Ranker::new(0.0);
        let idf = r.idf(4.0, 3.0);
        assert!((idf - (1.0f64 + 1.5 / 3.5).ln()).abs() < 1e-12);
        // tf=1, dl=2, avg=2.25
        let expected = idf * (2.2 * 1.0 / (1.0 + 1.2 * (0.25 + 0.75 * 2.0 / 2.25)));
        assert!((r.score(1.0, 2.0, 2.25, idf, 1.0) - expected).abs() < 1e-12);
        // delta adds to the norm
        let rd = Bm25Ranker::new(1.0);
        assert!((rd.score(1.0, 2.0, 2.25, idf, 1.0) - (expected + idf)).abs() < 1e-12);
    }

    #[test]
    fn wand_matches_brute_force() {
        let alpha: &[(u64, u32)] = &[(0, 1), (1, 1), (3, 2)];
        let beta: &[(u64, u32)] = &[(2, 1), (3, 1)];
        let lens: &[(u64, u32)] = &[(0, 2), (1, 2), (2, 2), (3, 3)];
        let dead = HashSet::new();
        let weights = vec![1.0, 1.0];
        let make = || {
            vec![field(
                &[("alpha", alpha), ("beta", beta)],
                lens,
                4,
                weights.clone(),
            )]
        };

        let got = BlockMaxWandIterator::new(make(), 4, &dead, 0.0, 1, WandMode::Bm25).run();
        assert_eq!(got.len(), 4);
        assert_eq!(got[0].0, 3); // both terms + length boost
        assert_eq!(got[1].0, 2); // only beta (higher idf than alpha)

        let boolean = BlockMaxWandIterator::new(make(), 4, &dead, 0.0, 1, WandMode::Boolean).run();
        assert_eq!(boolean[0], (3, 2.0));
        assert_eq!(boolean[1].1, 1.0);
        // minimum_should_match=2 keeps only the doc with both terms
        let both = BlockMaxWandIterator::new(make(), 4, &dead, 0.0, 2, WandMode::Bm25).run();
        assert_eq!(both.len(), 1);
        assert_eq!(both[0].0, 3);
    }

    /// The reference's BlockMaxBM25Score() must bound every exact score in the
    /// block, otherwise pruning would silently drop documents.
    #[test]
    fn block_maximum_bounds_every_score_in_the_block() {
        let doc_len: Vec<(u64, u32)> = (0..400).map(|d| (d, 40 + (d % 31) as u32)).collect();
        let postings: Vec<(u64, u32)> = (0..400)
            .map(|d| (d, 1 + (d % 11).min(d % 31) as u32))
            .collect();
        let lens: HashMap<u64, u32> = doc_len.iter().copied().collect();
        let dead = HashSet::new();
        let ranker = Bm25Ranker::new(0.0);
        let n = 400.0f64;
        let total_len: u64 = doc_len.iter().map(|(_, l)| *l as u64).sum();
        let avg = total_len as f64 / n;
        let idf = ranker.idf(n, postings.len() as f64);
        let dl = |row: u64| *lens.get(&row).unwrap_or(&1);
        let mut it = BlockMaxWandIterator::new(
            vec![field(&[("alpha", &postings)], &doc_len, 400, vec![1.7])],
            1,
            &dead,
            0.0,
            1,
            WandMode::Bm25,
        );
        let mut cursor = it.cursors.remove(0);
        let mut from = 0u64;
        loop {
            if cursor.stream.doc().is_none() {
                break;
            }
            let (last, _, _) = cursor.stream.block_stats();
            let bound = cursor.block_max(WandMode::Bm25);
            for (doc, tf) in postings.iter().filter(|(d, _)| *d >= from && *d <= last) {
                let exact = ranker.score(*tf as f64, f64::from(dl(*doc)), avg, idf, 1.7);
                assert!(exact <= bound + 1e-12, "doc {doc}: {exact} > {bound}");
            }
            from = last + 1;
            cursor.seek(from);
        }
    }

    /// Block maxima exist to stop the scan early: when the whole top-k lives in
    /// the first posting block, no later block may be scored at all.
    #[test]
    fn top_k_in_the_first_block_never_scores_later_blocks() {
        // alpha hits all 2000 docs. The first block is term-dense, the rest are
        // single-occurrence, so once the cut-off settles on the first block every
        // later block's maximum falls below it.
        let docs = 2000u64;
        let doc_len: Vec<(u64, u32)> = (0..docs).map(|d| (d, 100)).collect();
        let postings: Vec<(u64, u32)> = (0..docs)
            .map(|d| (d, if (d as usize) < BLOCK_SIZE { 20 } else { 1 }))
            .collect();
        let dead = HashSet::new();
        let make = || {
            vec![field(
                &[("alpha", &postings)],
                &doc_len,
                docs as usize,
                vec![1.0],
            )]
        };

        let mut it = BlockMaxWandIterator::new(make(), 10, &dead, 0.0, 1, WandMode::Bm25);
        assert!(it.next(None));
        let deepest = it.max_scored_block;
        let first_block_evals = it.score_evaluations;
        assert_eq!(deepest, 0, "scored a block past the first one");

        let (best, evaluations, deepest_block) =
            BlockMaxWandIterator::new(make(), 10, &dead, 0.0, 1, WandMode::Bm25).drive_with_stats();
        assert_eq!(best.len(), 10);
        assert!(best.iter().all(|(doc, _)| (*doc as usize) < BLOCK_SIZE));
        assert_eq!(deepest_block, 0, "scored a block past the first one");
        assert_eq!(
            evaluations, BLOCK_SIZE,
            "only the first block may be decoded, got {evaluations}"
        );
        assert!(first_block_evals > 0);

        let (all, full_evaluations, _) =
            BlockMaxWandIterator::new(make(), docs as usize, &dead, 0.0, 1, WandMode::Bm25)
                .drive_with_stats();
        assert_eq!(all.len(), docs as usize);
        assert_eq!(full_evaluations, docs as usize);
        assert!(evaluations * 10 < full_evaluations);
    }

    fn lcg(state: &mut u64) -> u64 {
        *state = state
            .wrapping_mul(6364136223846793005)
            .wrapping_add(1442695040888963407);
        *state >> 33
    }

    /// Exhaustive evaluation of the same query: no pivot, no pruning.
    #[allow(clippy::too_many_arguments)]
    fn full_scan(
        postings: &HashMap<String, Vec<(u64, u32)>>,
        lens: &HashMap<u64, u32>,
        n: usize,
        total_len: u64,
        weights: &[f64],
        terms: &[String],
        k: usize,
        dead: &HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: WandMode,
    ) -> Vec<(u64, f64)> {
        let ranker = Bm25Ranker::new(delta);
        let mut candidates: Vec<u64> = Vec::new();
        for term in terms {
            if let Some(list) = postings.get(term) {
                candidates.extend(list.iter().map(|p| p.0));
            }
        }
        candidates.sort_unstable();
        candidates.dedup();
        let avg = (total_len as f64 / n.max(1) as f64).max(1e-9);
        let mut scored = Vec::new();
        for doc in candidates {
            let mut score = 0.0;
            let mut seen_terms = HashSet::new();
            let mut matched = 0;
            for (ti, term) in terms.iter().enumerate() {
                let Some(list) = postings.get(term) else {
                    continue;
                };
                let Ok(pos) = list.binary_search_by_key(&doc, |p| p.0) else {
                    continue;
                };
                matched += 1;
                seen_terms.insert(ti);
                let w = weights.get(ti).copied().unwrap_or(1.0);
                match mode {
                    WandMode::Bm25 => {
                        let tf = list[pos].1 as f64;
                        let dl = f64::from(*lens.get(&doc).unwrap_or(&1));
                        let idf = ranker.idf(n.max(1) as f64, list.len() as f64);
                        score += ranker.score(tf, dl, avg, idf, w);
                    }
                    WandMode::Boolean => score += w,
                }
            }
            let hits = match mode {
                WandMode::Bm25 => seen_terms.len(),
                WandMode::Boolean => matched,
            };
            if hits < min_should.max(1) || dead.contains(&doc) {
                continue;
            }
            scored.push((doc, score));
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(k);
        scored
    }

    /// Random corpus: the pivot search and the block-max pruning must return
    /// exactly what the full scan returns, scores included.
    #[test]
    fn wand_matches_full_scan_on_a_random_corpus() {
        const DOCS: u64 = 2000;
        let mut rng = 0x5eed_1234_abcd_0001u64;
        let terms: Vec<String> = ["alpha", "beta", "gamma", "delta", "epsilon"]
            .iter()
            .map(|t| t.to_string())
            .collect();
        let mut lens = HashMap::new();
        for doc in 0..DOCS {
            lens.insert(doc, 5 + (lcg(&mut rng) % 200) as u32);
        }
        let mut postings: HashMap<String, Vec<(u64, u32)>> = HashMap::new();
        for (ti, term) in terms.iter().enumerate() {
            let density = 30 - ti as u64 * 4;
            let mut list = Vec::new();
            for doc in 0..DOCS {
                if lcg(&mut rng) % 100 < density {
                    let len = lens[&doc];
                    let tf = 1 + (lcg(&mut rng) % len as u64) as u32;
                    list.push((doc, tf));
                }
            }
            postings.insert(term.clone(), list);
        }
        let total_len: u64 = lens.values().map(|l| *l as u64).sum();
        let weights = vec![1.0, 0.5, 1.0, 2.0, 1.0];
        let dead: HashSet<u64> = (0..DOCS)
            .filter(|_| lcg(&mut rng).is_multiple_of(20))
            .collect();

        let lens_slice: Vec<(u64, u32)> = lens.iter().map(|(d, l)| (*d, *l)).collect();
        let make = || {
            let ranker = Bm25Ranker::new(0.0);
            let units = terms
                .iter()
                .enumerate()
                .map(|(i, t)| WandUnit {
                    stream: WandStream::Term(TermStream::from_slice(
                        postings[t].as_slice(),
                        &lens_slice,
                    )),
                    weight: weights.get(i).copied().unwrap_or(1.0),
                    idf: ranker.idf(DOCS as f64, postings[t].len() as f64),
                    delta: None,
                })
                .collect();
            vec![WandField {
                n_docs: DOCS as usize,
                total_len,
                units,
            }]
        };

        for k in [1usize, 10, 100] {
            for (delta, min_should) in [(0.0, 1usize), (0.5, 1), (0.0, 2)] {
                for mode in [WandMode::Bm25, WandMode::Boolean] {
                    let want = full_scan(
                        &postings,
                        &lens,
                        DOCS as usize,
                        total_len,
                        &weights,
                        &terms,
                        k,
                        &dead,
                        delta,
                        min_should,
                        mode,
                    );
                    let got =
                        BlockMaxWandIterator::new(make(), k, &dead, delta, min_should, mode).run();
                    assert_eq!(
                        got,
                        want,
                        "k={k} delta={delta} min_should={min_should} mode={}",
                        mode == WandMode::Bm25
                    );
                }
            }
        }

        // Block maxima must also pay off on the random corpus.
        let mut candidates: Vec<u64> = Vec::new();
        for term in &terms {
            candidates.extend(postings[term].iter().map(|p| p.0));
        }
        candidates.sort_unstable();
        candidates.dedup();
        let (best, evaluations, _) =
            BlockMaxWandIterator::new(make(), 10, &dead, 0.0, 1, WandMode::Bm25).drive_with_stats();
        assert_eq!(
            best,
            full_scan(
                &postings,
                &lens,
                DOCS as usize,
                total_len,
                &weights,
                &terms,
                10,
                &dead,
                0.0,
                1,
                WandMode::Bm25
            )
        );
        assert!(
            evaluations * 2 < candidates.len(),
            "{evaluations} exact scores for {} candidates: no pruning happened",
            candidates.len()
        );
    }

    /// The heap must not change a single answer. Several corpora, each mixing
    /// dense lists (deep prefixes, block skips) with sparse ones (document gaps
    /// between cursors, which is the "lists[0] is not on the pivot document"
    /// jump), compared against the naive OR scan on document ids *and* scores.
    #[test]
    fn heap_wand_matches_naive_scan_on_many_corpora() {
        const DOCS: u64 = 400;
        for seed in [
            0x1234_5678_9abc_def1u64,
            0x0f0f_0f0f_1111_2222,
            0xdead_beef_cafe_0001,
        ] {
            let mut rng = seed;
            let terms: Vec<String> = (0..6).map(|i| format!("t{i}")).collect();
            let mut lens = HashMap::new();
            for doc in 0..DOCS {
                lens.insert(doc, 3 + (lcg(&mut rng) % 97) as u32);
            }
            let mut postings: HashMap<String, Vec<(u64, u32)>> = HashMap::new();
            for (ti, term) in terms.iter().enumerate() {
                let density = if ti % 2 == 0 { 55 } else { 7 };
                let mut list = Vec::new();
                for doc in 0..DOCS {
                    if lcg(&mut rng) % 100 < density {
                        let len = lens[&doc];
                        let tf = 1 + (lcg(&mut rng) % len as u64) as u32;
                        list.push((doc, tf));
                    }
                }
                postings.insert(term.clone(), list);
            }
            let total_len: u64 = lens.values().map(|l| *l as u64).sum();
            let weights = vec![1.0, 0.5, 2.0, 1.0, 1.5, 0.25];
            let dead: HashSet<u64> = (0..DOCS).filter(|_| lcg(&mut rng) % 13 == 0).collect();
            let lens_slice: Vec<(u64, u32)> = lens.iter().map(|(d, l)| (*d, *l)).collect();
            let make = || {
                let ranker = Bm25Ranker::new(0.0);
                let units = terms
                    .iter()
                    .enumerate()
                    .map(|(i, t)| WandUnit {
                        stream: WandStream::Term(TermStream::from_slice(
                            postings[t].as_slice(),
                            &lens_slice,
                        )),
                        weight: weights[i],
                        idf: ranker.idf(DOCS as f64, postings[t].len() as f64),
                        delta: None,
                    })
                    .collect();
                vec![WandField {
                    n_docs: DOCS as usize,
                    total_len,
                    units,
                }]
            };

            for k in [1usize, 3, 25, DOCS as usize] {
                for (delta, min_should) in [(0.0, 1usize), (0.3, 3)] {
                    for mode in [WandMode::Bm25, WandMode::Boolean] {
                        let want = full_scan(
                            &postings,
                            &lens,
                            DOCS as usize,
                            total_len,
                            &weights,
                            &terms,
                            k,
                            &dead,
                            delta,
                            min_should,
                            mode,
                        );
                        let got =
                            BlockMaxWandIterator::new(make(), k, &dead, delta, min_should, mode)
                                .run();
                        assert_eq!(
                            got, want,
                            "seed={seed:x} k={k} delta={delta} min_should={min_should}"
                        );
                    }
                }
            }
        }
    }

    /// block_max=false must not change a single answer: it only scores every
    /// candidate instead of pruning. Dense and sparse lists interleaved, so the
    /// pivot, the document-gap jump and the block skip all fire on the pruned path.
    #[test]
    fn naive_and_pruned_agree() {
        const DOCS: u64 = 3000;
        let doc_len: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 10 + (d % 90) as u32)).collect();
        let (mut a, mut b, mut c) = (Vec::new(), Vec::new(), Vec::new());
        let mut rng = 0xabcd_1234_5678_9ef1u64;
        for doc in 0..DOCS {
            if lcg(&mut rng) % 100 < 40 {
                a.push((doc, 1 + (lcg(&mut rng) % 5) as u32));
            }
            if lcg(&mut rng) % 100 < 15 {
                b.push((doc, 1 + (lcg(&mut rng) % 9) as u32));
            }
            if lcg(&mut rng) % 100 < 3 {
                c.push((doc, 1 + (lcg(&mut rng) % 3) as u32));
            }
        }
        let dead = HashSet::new();
        let make = || {
            vec![field(
                &[("a", &a), ("b", &b), ("c", &c)],
                &doc_len,
                DOCS as usize,
                vec![1.0, 0.5, 2.0],
            )]
        };
        for k in [1usize, 5, 100, DOCS as usize] {
            for mode in [WandMode::Bm25, WandMode::Boolean] {
                let pruned = BlockMaxWandIterator::new(make(), k, &dead, 0.0, 1, mode).run();
                let naive = BlockMaxWandIterator::new(make(), k, &dead, 0.0, 1, mode)
                    .with_naive()
                    .run();
                assert_eq!(pruned, naive, "k={k}");
            }
        }
    }

    /// A phrase unit's block bound is the minimum over its terms', so blocks
    /// with no phrase occurrence are skipped without verifying each document.
    #[test]
    fn phrase_unit_prunes_blocks_without_phrase_matches() {
        const DOCS: u64 = 2000;
        // Documents 0..128 carry the phrase 20 times (`a` at even offsets, `b`
        // right after); the rest carry `a` and `b` far apart, so the phrase
        // never occurs there.
        let mut a: Vec<(u64, u32, Vec<u32>)> = Vec::new();
        let mut b: Vec<(u64, u32, Vec<u32>)> = Vec::new();
        for doc in 0..DOCS {
            if doc < BLOCK_SIZE as u64 {
                let pa: Vec<u32> = (0..20).map(|i| i * 2).collect();
                let pb: Vec<u32> = (0..20).map(|i| i * 2 + 1).collect();
                a.push((doc, 20, pa));
                b.push((doc, 20, pb));
            } else {
                a.push((doc, 1, vec![0]));
                b.push((doc, 1, vec![10]));
            }
        }
        let lens: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 100)).collect();
        let ranker = Bm25Ranker::new(0.0);
        let n = DOCS as f64;
        let idf_a = ranker.idf(n, a.len() as f64);
        let idf_b = ranker.idf(n, b.len() as f64);
        let make = || {
            let cursor = crate::PhraseCursor::new(vec![
                TermStream::from_postings(a.clone(), &lens),
                TermStream::from_postings(b.clone(), &lens),
            ]);
            vec![WandField {
                n_docs: DOCS as usize,
                total_len: DOCS * 100,
                units: vec![WandUnit {
                    stream: WandStream::Phrase(cursor),
                    weight: 1.0,
                    idf: idf_a + idf_b,
                    delta: None,
                }],
            }]
        };
        let dead = HashSet::new();
        let (best, evaluations, deepest) =
            BlockMaxWandIterator::new(make(), 10, &dead, 0.0, 1, WandMode::Bm25).drive_with_stats();
        assert_eq!(best.len(), 10);
        assert!(best.iter().all(|(doc, _)| *doc < BLOCK_SIZE as u64));
        assert_eq!(deepest, 0, "scored a block past the first one");
        assert!(
            evaluations <= BLOCK_SIZE,
            "{evaluations} exact scores: the later blocks were not pruned"
        );
    }

    /// `seek_block` is the reference's `NextShallow`: it repositions the unit
    /// without decoding any offsets, so a block the WAND skips never pays for
    /// the phrase verification.
    #[test]
    fn phrase_seek_block_defers_the_offset_check() {
        let mut a = Vec::new();
        let mut b = Vec::new();
        for doc in 0..400u64 {
            if doc < 128 {
                a.push((doc, 2, vec![0u32, 2]));
                b.push((doc, 2, vec![1u32, 3]));
            } else {
                a.push((doc, 1, vec![0u32]));
                b.push((doc, 1, vec![9u32]));
            }
        }
        let lens: Vec<(u64, u32)> = (0..400u64).map(|d| (d, 10)).collect();
        let mut cursor = crate::PhraseCursor::new(vec![
            TermStream::from_postings(a, &lens),
            TermStream::from_postings(b, &lens),
        ]);
        assert_eq!(cursor.doc(), Some(0));
        let before = cursor.verified();
        cursor.seek_block(200);
        assert_eq!(
            cursor.verified(),
            before,
            "seek_block decoded offsets for documents it skipped"
        );
        assert_eq!(cursor.doc(), None);
        assert!(cursor.verified() > before, "resolve must verify");
    }
}

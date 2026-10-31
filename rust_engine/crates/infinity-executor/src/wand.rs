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

use std::collections::HashSet;

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
    field: usize,
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
    stream: Box<dyn WandStream + 'a>,
    /// iter_->SeekDoc position, cached so the cursors can be sorted.
    cur: Option<u64>,
    /// BlockMaxBM25Score() cache: the block it was computed for, its bound and
    /// the block's last document (used by the block-skip path).
    block_index: u64,
    block_max: f64,
    block_last_doc: u64,
}

impl<'a> Cursor<'a> {
    fn refresh(&mut self) {
        self.cur = self.stream.doc();
    }

    /// PostingIterator::SeekDoc: position on the first document >= doc. Only
    /// decodes the block the seek lands in, never a score.
    fn seek(&mut self, doc: u64) {
        self.stream.advance(doc);
        self.refresh();
    }

    /// BlockLastDocID(): the last document of the current block.
    fn block_last(&mut self) -> u64 {
        self.block_last_doc
    }

    /// BlockMaxBM25Score() (term_doc_iterator_impl.cpp:91-102): the block's
    /// largest tf and largest tf/doc-length ratio, fed through the same BM25
    /// shape as the exact score. Cached per block like the reference's
    /// block_max_bm25_score_cache_end_id_.
    fn block_max(&mut self, doc_len: &dyn Fn(u64) -> u32, mode: WandMode) -> f64 {
        let id = self.stream.block_id();
        if self.block_index == id {
            return self.block_max;
        }
        // `BlockMaxPhraseScore` for a phrase unit: the minimum over its terms',
        // since a phrase can occur no more often than any one of them.
        let (last, max_tf, max_pct) = self.stream.block_stats(doc_len);
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
    /// One document-length provider per field (GetColumnLength).
    doc_lens: Vec<Box<dyn Fn(u64) -> u32 + 'a>>,
    /// Top-k, sorted by score (desc) then doc id (asc), like the scan path.
    best: Vec<(u64, f64)>,
    threshold: f64,
    /// doc_id_ / pivot_ / bm25_score_cache_ of the reference: the last candidate
    /// Next() stopped on and the prefix that matched it.
    doc_id: Option<u64>,
    pivot: usize,
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
        let mut doc_lens = Vec::new();
        let field_count = fields.len();
        for (fi, field) in fields.into_iter().enumerate() {
            let WandField {
                n_docs,
                total_len,
                units,
                doc_len,
            } = field;
            doc_lens.push(doc_len);
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
                    field: fi,
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
                    block_index: u64::MAX,
                    block_max: 0.0,
                    block_last_doc: u64::MAX,
                });
                cursors.last_mut().unwrap().refresh();
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
            doc_lens,
            best: Vec::new(),
            threshold: f64::NEG_INFINITY,
            doc_id: None,
            pivot: 0,
            candidate_score: 0.0,
            score_evaluations: 0,
            max_scored_block: 0,
        }
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
    }

    /// Test-only view of the pruning counters.
    #[cfg(test)]
    fn drive_with_stats(mut self) -> (Vec<(u64, f64)>, usize, u64) {
        self.drive();
        (self.best, self.score_evaluations, self.max_scored_block)
    }

    /// Keeps the candidate if it survives minimum_should_match, the tombstone
    /// filter and the k-th score (ties broken by the smaller doc id).
    fn offer(&mut self, doc_id: u64, score: f64) {
        // minimum_should_match: BM25 counts distinct query units across fields;
        // boolean counts every (field, term) match.
        let matched = match self.mode {
            WandMode::Bm25 => {
                let mut seen = HashSet::new();
                for cursor in &self.cursors[..=self.pivot] {
                    seen.insert(cursor.term);
                }
                seen.len()
            }
            WandMode::Boolean => self.pivot + 1,
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
        match target {
            // doc_id_ == INVALID_ROWID: initialize the children via Next(0).
            None => {
                for cursor in &mut self.cursors {
                    cursor.seek(0);
                }
            }
            Some(target) => {
                if let Some(current) = self.doc_id {
                    if current >= target {
                        return true;
                    }
                }
                // Move all pointers from lists[0] to lists[pivot] to target.
                let last = self.pivot.min(self.cursors.len().saturating_sub(1));
                for i in 0..=last {
                    let cursor = &mut self.cursors[i];
                    if cursor.cur.is_some_and(|doc| doc < target) {
                        cursor.seek(target);
                    }
                }
            }
        }
        loop {
            // Next() keeps the cursors sorted by doc id and drops the exhausted
            // tail, subtracting its upper bound from the total.
            self.cursors.sort_by_key(|c| c.cur.unwrap_or(u64::MAX));
            while self.cursors.last().is_some_and(|c| c.cur.is_none()) {
                self.cursors.pop();
            }
            if self.cursors.is_empty() {
                break;
            }
            let total: f64 = self.cursors.iter().map(|c| c.upper_bound).sum();
            // '<' (not '<='): a document that ties the k-th score can still win on
            // the smaller doc id.
            if total < self.threshold {
                break;
            }
            // FindPivotOptimized: pivot = first prefix whose upper bound reaches
            // the threshold.
            let mut pivot = self.cursors.len();
            let mut prefix = 0.0;
            for (pos, cursor) in self.cursors.iter().enumerate() {
                prefix += cursor.upper_bound;
                if prefix >= self.threshold {
                    pivot = pos;
                    break;
                }
            }
            if pivot >= self.cursors.len() {
                break;
            }
            let Some(doc) = self.cursors[pivot].cur else {
                break;
            };
            while pivot + 1 < self.cursors.len() && self.cursors[pivot + 1].cur == Some(doc) {
                pivot += 1;
            }
            // If lists[0] is not on the pivot document yet, another list owns it:
            // move the prefix up to it and re-pivot.
            if self.cursors[0].cur != Some(doc) {
                for i in 0..pivot {
                    let cursor = &mut self.cursors[i];
                    if cursor.cur.is_some_and(|cur| cur < doc) {
                        cursor.seek(doc);
                    }
                }
                continue;
            }
            // NextShallow + BlockMaxBM25Score over the prefix. Every prefix cursor
            // sits on doc, so the sum bounds doc's exact score.
            let mut block_bound = 0.0;
            for i in 0..=pivot {
                let field = self.cursors[i].field;
                let bound = {
                    let cursor = &mut self.cursors[i];
                    cursor.block_max(&*self.doc_lens[field], self.mode)
                };
                block_bound += bound;
            }
            if block_bound >= self.threshold {
                // Only now is the exact score decoded.
                let score = self.score_prefix(pivot);
                self.doc_id = Some(doc);
                self.pivot = pivot;
                self.candidate_score = score;
                return true;
            }
            self.skip_block_range(doc, pivot);
        }
        self.doc_id = None;
        false
    }

    /// The reference's block-skip path (blockmax_wand_iterator_impl.cpp:449-524):
    /// every document in [doc, skip_to) is bounded by the prefix's block maxima,
    /// which stayed below the k-th score, so the whole run is stepped over in one
    /// seek per cursor. The run is capped at the next document any cursor is
    /// already sitting on, which keeps documents that a later pivot still needs
    /// reachable.
    fn skip_block_range(&mut self, doc: u64, pivot: usize) {
        let up_to = self.cursors.get(pivot + 1).and_then(|c| c.cur);
        let mut block_last = u64::MAX;
        for cursor in &mut self.cursors[..=pivot] {
            block_last = block_last.min(cursor.block_last());
        }
        let skip_to = match up_to {
            Some(up_to) => (block_last + 1).min(up_to),
            None => block_last + 1,
        };
        debug_assert!(skip_to > doc);
        for i in 0..=pivot {
            let cursor = &mut self.cursors[i];
            if cursor.cur.is_some_and(|cur| cur < skip_to) {
                cursor.seek(skip_to);
            }
        }
    }

    /// Sums the exact BM25Score() of the prefix cursors, all parked on the same
    /// document.
    fn score_prefix(&mut self, pivot: usize) -> f64 {
        let ranker = Bm25Ranker::new(self.ranker.delta);
        let mut score = 0.0;
        // Sum in cursor creation order (field-major, then term) so the value is
        // independent of the doc-id sort, matching the scan path bit for bit.
        let mut prefix: Vec<usize> = (0..=pivot).collect();
        prefix.sort_by_key(|&i| self.cursors[i].ord);
        match self.mode {
            WandMode::Bm25 => {
                for &i in &prefix {
                    let cursor = &mut self.cursors[i];
                    let doc = cursor.cur.unwrap_or(0);
                    let tf = f64::from(cursor.stream.tf());
                    let dl = f64::from(self.doc_lens[cursor.field](doc));
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
                for &i in &prefix {
                    score += self.cursors[i].weight;
                }
            }
        }
        self.score_evaluations += 1;
        for cursor in &self.cursors[..=pivot] {
            self.max_scored_block = self.max_scored_block.max(cursor.block_index);
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
        let lens: HashMap<u64, u32> = doc_len.iter().copied().collect();
        let ranker = Bm25Ranker::new(0.0);
        let units = postings
            .iter()
            .enumerate()
            .map(|(i, (_term, list))| WandUnit {
                stream: Box::new(TermStream::from_slice(list)),
                weight: weights.get(i).copied().unwrap_or(1.0),
                idf: ranker.idf(n.max(1) as f64, list.len() as f64),
                delta: None,
            })
            .collect();
        WandField {
            n_docs: n,
            total_len: doc_len.iter().map(|(_, l)| *l as u64).sum(),
            units,
            doc_len: Box::new(move |row| *lens.get(&row).unwrap_or(&1)),
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
            let (last, _, _) = cursor.stream.block_stats(&dl);
            let bound = cursor.block_max(&dl, WandMode::Bm25);
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

        let make = || {
            let lens_map = lens.clone();
            let ranker = Bm25Ranker::new(0.0);
            let units = terms
                .iter()
                .enumerate()
                .map(|(i, t)| WandUnit {
                    stream: Box::new(TermStream::from_slice(postings[t].as_slice())),
                    weight: weights.get(i).copied().unwrap_or(1.0),
                    idf: ranker.idf(DOCS as f64, postings[t].len() as f64),
                    delta: None,
                })
                .collect();
            vec![WandField {
                n_docs: DOCS as usize,
                total_len,
                units,
                doc_len: Box::new(move |row| *lens_map.get(&row).unwrap_or(&1)),
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
        let lens: HashMap<u64, u32> = (0..DOCS).map(|d| (d, 100)).collect();
        let ranker = Bm25Ranker::new(0.0);
        let n = DOCS as f64;
        let idf_a = ranker.idf(n, a.len() as f64);
        let idf_b = ranker.idf(n, b.len() as f64);
        let make = || {
            let cursor = crate::PhraseCursor::new(vec![
                TermStream::from_postings(a.clone()),
                TermStream::from_postings(b.clone()),
            ]);
            let lens = lens.clone();
            vec![WandField {
                n_docs: DOCS as usize,
                total_len: DOCS * 100,
                units: vec![WandUnit {
                    stream: Box::new(cursor),
                    weight: 1.0,
                    idf: idf_a + idf_b,
                    delta: None,
                }],
                doc_len: Box::new(move |row| *lens.get(&row).unwrap_or(&1)),
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
}

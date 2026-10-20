//! The reference's BM25 ranker and block-max WAND iterator, ported as classes
//! so the retrieval control flow stays 1:1 with C++ (plan §3.6):
//!
//! * `BM25Ranker` — `src/storage/invertedindex/search/bm25_ranker_impl.cpp`
//! * `BlockMaxLeafIterator` — `blockmax_leaf_iterator.cppm`, realised over the
//!   in-memory postings. A block is the same fixed run of documents the
//!   reference's `DocListEncoder` summarises: `MAX_DOC_PER_RECORD` (128) entries
//!   of one term's posting list, bounded by the largest tf and the largest
//!   tf/doc-length ratio they contain (`doc_list_encoder_impl.cpp:69-71,161-168`,
//!   consumed by `TermDocIterator::BlockMaxBM25Score`,
//!   `term_doc_iterator_impl.cpp:91-102`).
//! * `BlockMaxWandIterator` — `src/storage/invertedindex/search/blockmax_wand_iterator_impl.cpp`:
//!   `FindPivotOptimized` (`:88`), `NextShallow` (`:348-377`), the block-skip
//!   path of `Next` (`:449-524`) and incremental `Next(RowID)` (`:248`).
//!
//! The iterator owns the cursors, the current candidate, the top-k and the
//! pruning threshold. `run()` is a thin wrapper driving the reference's
//! `Next()` loop to exhaustion, so the executor's entry point is unchanged.

use std::collections::{HashMap, HashSet};

use super::MergedIndex;

/// `MAX_DOC_PER_RECORD` (`index_defines.cppm:43`): the fixed run of documents
/// that forms one posting block, i.e. the granularity of `block_max_tf` /
/// `block_max_percentage`. `SkipListReader` returns one entry per block.
const BLOCK_SIZE: usize = 128;

/// `numeric_limits<u16>::max()`: `block_max_percentage` is stored as a u16
/// fraction of the block's largest tf/doc-length ratio
/// (`doc_list_encoder_impl.cpp:166`).
const PERCENTAGE_SCALE: f64 = u16::MAX as f64;

/// `BM25Ranker` (`bm25_ranker_impl.cpp:25-34`): k1 = 1.2, b = 0.75,
/// `idf = ln(1 + (N - df + 0.5) / (df + 0.5))`, plus the optional `delta_term`
/// added to the tf norm.
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

    /// `idf * (tf_norm + delta) * weight`, with
    /// `tf_norm = (k1+1)*tf / (tf + k1*((1-b) + b*dl/avgdl))`.
    pub fn score(&self, tf: f64, dl: f64, avgdl: f64, idf: f64, weight: f64) -> f64 {
        let norm = (self.k1 + 1.0) * tf
            / (tf + self.k1 * ((1.0 - self.b) + self.b * dl / avgdl.max(1e-9)));
        idf * (norm + self.delta) * weight
    }

    /// `tf_norm <= k1+1`, so this is a safe upper bound for pruning.
    pub fn upper_bound(&self, idf: f64, weight: f64) -> f64 {
        idf * (self.k1 + 1.0 + self.delta.max(0.0)) * weight
    }

    /// `bm25_common_score_` (`term_doc_iterator_impl.cpp:71`):
    /// `weight * idf * (k1 + 1)`. Both the whole-term bound and every block
    /// bound are this factor times a tf-only term.
    pub fn common(&self, idf: f64, weight: f64) -> f64 {
        weight * idf * (self.k1 + 1.0)
    }

    /// `f4 = delta / (k1 + 1)` (`term_doc_iterator_impl.cpp:76`).
    pub fn f4(&self) -> f64 {
        self.delta / (self.k1 + 1.0)
    }

    /// `f1 = k1 * (1 - b)` (`term_doc_iterator_impl.cpp:73`).
    pub fn f1(&self) -> f64 {
        self.k1 * (1.0 - self.b)
    }

    /// `f3 = k1 * b / avgdl * u16::MAX` (`term_doc_iterator_impl.cpp:74-75`),
    /// the tf/doc-length ratio term of `BlockMaxBM25Score`.
    pub fn f3(&self, avgdl: f64) -> f64 {
        self.k1 * self.b / avgdl.max(1e-9) * PERCENTAGE_SCALE
    }
}

/// `similarity=boolean` scores one point per matched term (per field); the
/// default is BM25. Both are handled inside the same iterator.
#[derive(Clone, Copy, PartialEq, Eq)]
pub(crate) enum WandMode {
    Bm25,
    Boolean,
}

/// One `(field, term)` posting cursor: the reference's `TermDocIterator`.
///
/// `pos` is the document cursor (`iter_->SeekDoc`) and the block cursor is
/// derived from it (`pos / BLOCK_SIZE`), since a block is a positional run of
/// the posting list.
struct Cursor {
    /// Position of this cursor in creation order (field-major, then term).
    /// Scores are summed in this order so the result never depends on how the
    /// doc-id sort permuted the cursors.
    ord: usize,
    field: usize,
    term: usize,
    weight: f64,
    /// `bm25_common_score_`; zero for `similarity=boolean`.
    common: f64,
    /// `BM25ScoreUpperBound()` of the whole term.
    upper_bound: f64,
    /// `f1`, `f3`, `f4` of `TermDocIterator::InitBM25Info`.
    f1: f64,
    f3: f64,
    f4: f64,
    /// Index of the current posting (`iter_->SeekDoc`); `list.len()` when
    /// exhausted (`INVALID_ROWID`).
    pos: usize,
    /// `BlockMaxBM25Score()` cache keyed by the block it was computed for.
    cached_block: usize,
    block_max: f64,
}

impl Cursor {
    fn doc(&self, list: &[(u64, u32)]) -> Option<u64> {
        list.get(self.pos).map(|p| p.0)
    }

    /// `PostingIterator::SeekDoc`: position on the first document `>= doc`.
    /// Only decodes skip-list style bounds, never a score.
    fn seek(&mut self, list: &[(u64, u32)], doc: u64) {
        if self.pos >= list.len() {
            return;
        }
        self.pos = list.partition_point(|p| p.0 < doc).max(self.pos);
    }

    /// `BlockLastDocID()`: the last document of the block holding `pos`.
    fn block_last(&self, list: &[(u64, u32)]) -> u64 {
        let end = ((self.pos / BLOCK_SIZE + 1) * BLOCK_SIZE).min(list.len());
        list.get(end.saturating_sub(1))
            .map(|p| p.0)
            .unwrap_or(u64::MAX)
    }

    /// `BlockMaxBM25Score()` (`term_doc_iterator_impl.cpp:91-102`): the block's
    /// largest tf and largest tf/doc-length ratio, fed through the same BM25
    /// shape as the exact score. Cached per block like the reference's
    /// `block_max_bm25_score_cache_end_id_`.
    fn block_max(
        &mut self,
        list: &[(u64, u32)],
        doc_len: &HashMap<u64, u32>,
        mode: WandMode,
    ) -> f64 {
        let block = self.pos / BLOCK_SIZE;
        if self.cached_block == block {
            return self.block_max;
        }
        let mut max_tf = 0.0f64;
        let mut max_pct = 0.0f64;
        let start = block * BLOCK_SIZE;
        let end = (start + BLOCK_SIZE).min(list.len());
        for post in &list[start..end] {
            let tf = post.1 as f64;
            max_tf = max_tf.max(tf);
            let dl = *doc_len.get(&post.0).unwrap_or(&1) as f64;
            max_pct = max_pct.max(tf / dl.max(1.0));
        }
        let value = match mode {
            // One point per matched term, independent of tf and length.
            WandMode::Boolean => self.weight,
            WandMode::Bm25 => {
                let pct_u16 = (max_pct * PERCENTAGE_SCALE).ceil().max(1.0);
                let norm = 1.0 / (1.0 + self.f1 / max_tf.max(1.0) + self.f3 / pct_u16);
                (self.common * (norm + self.f4)).min(self.upper_bound)
            }
        };
        self.cached_block = block;
        self.block_max = value;
        value
    }
}

fn doc_of(c: &Cursor, fields: &[(MergedIndex, Vec<f64>)], terms: &[String]) -> Option<u64> {
    c.doc(&fields[c.field].0.postings[&terms[c.term]])
}

fn list_of<'a>(
    c: &Cursor,
    fields: &'a [(MergedIndex, Vec<f64>)],
    terms: &[String],
) -> &'a [(u64, u32)] {
    &fields[c.field].0.postings[&terms[c.term]]
}

/// `BlockMaxWandIterator` over the merged per-field postings. Terms are
/// single-unit OR terms (no phrase / require-all).
pub(crate) struct BlockMaxWandIterator<'a> {
    fields: &'a [(MergedIndex, Vec<f64>)],
    terms: &'a [String],
    dead: &'a HashSet<u64>,
    k: usize,
    min_should: usize,
    mode: WandMode,
    ranker: Bm25Ranker,
    cursors: Vec<Cursor>,
    /// Top-k, sorted by score (desc) then doc id (asc), like the scan path.
    best: Vec<(u64, f64)>,
    threshold: f64,
    /// `doc_id_` / `pivot_` / `bm25_score_cache_` of the reference: the last
    /// candidate `Next()` stopped on and the prefix that matched it.
    doc_id: Option<u64>,
    pivot: usize,
    candidate_score: f64,
    /// Instrumentation: exact per-document score decodes (`calc_score_cnt_`) and
    /// the deepest block any of them came from.
    score_evaluations: usize,
    max_scored_block: usize,
}

impl<'a> BlockMaxWandIterator<'a> {
    pub fn new(
        fields: &'a [(MergedIndex, Vec<f64>)],
        terms: &'a [String],
        k: usize,
        dead: &'a HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: WandMode,
    ) -> Self {
        let ranker = Bm25Ranker::new(delta);
        let mut cursors = Vec::new();
        for (fi, (index, weights)) in fields.iter().enumerate() {
            let n = index.n_docs.max(1) as f64;
            let avg = (index.total_len as f64 / n).max(1e-9);
            for (ti, term) in terms.iter().enumerate() {
                let Some(list) = index.postings.get(term) else {
                    continue;
                };
                if list.is_empty() {
                    continue;
                }
                let idf = ranker.idf(n, list.len() as f64);
                let w = weights.get(ti).copied().unwrap_or(1.0);
                let (upper_bound, common, f1, f3, f4) = match mode {
                    WandMode::Bm25 => (
                        ranker.upper_bound(idf, w),
                        ranker.common(idf, w),
                        ranker.f1(),
                        ranker.f3(avg),
                        ranker.f4(),
                    ),
                    WandMode::Boolean => (w, 0.0, 0.0, 0.0, 0.0),
                };
                cursors.push(Cursor {
                    ord: cursors.len(),
                    field: fi,
                    term: ti,
                    weight: w,
                    common,
                    upper_bound,
                    f1,
                    f3,
                    f4,
                    pos: 0,
                    cached_block: usize::MAX,
                    block_max: 0.0,
                });
            }
        }
        Self {
            fields,
            terms,
            dead,
            k,
            min_should,
            mode,
            ranker,
            cursors,
            best: Vec::new(),
            threshold: f64::NEG_INFINITY,
            doc_id: None,
            pivot: 0,
            candidate_score: 0.0,
            score_evaluations: 0,
            max_scored_block: 0,
        }
    }

    /// Drives the reference's `Next()` loop until the cursors are exhausted or
    /// the remaining upper bound cannot beat the k-th score.
    pub fn run(mut self) -> Vec<(u64, f64)> {
        self.drive();
        self.best
    }

    /// The reference's driver: `Next(doc_id + 1)` until exhaustion, offering
    /// each surviving candidate to the top-k.
    fn drive(&mut self) {
        if self.k == 0 || self.fields.is_empty() || self.cursors.is_empty() {
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
    fn drive_with_stats(mut self) -> (Vec<(u64, f64)>, usize, usize) {
        self.drive();
        (self.best, self.score_evaluations, self.max_scored_block)
    }

    /// Keeps the candidate if it survives `minimum_should_match`, the tombstone
    /// filter and the k-th score (ties broken by the smaller doc id).
    fn offer(&mut self, doc_id: u64, score: f64) {
        // `minimum_should_match`: BM25 counts distinct query units across
        // fields; boolean counts every (field, term) match.
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

    /// `BlockMaxWandIterator::Next(RowID)` (`blockmax_wand_iterator_impl.cpp:248`):
    /// stop on the next document whose block-max bound can still reach the
    /// k-th score, decoding the exact score only for that document. Returns
    /// false and clears `doc_id` when the iterator is exhausted.
    fn next(&mut self, target: Option<u64>) -> bool {
        let fields = self.fields;
        let terms = self.terms;
        match target {
            // `doc_id_ == INVALID_ROWID`: initialize the children via Next(0).
            None => {
                for cursor in &mut self.cursors {
                    cursor.pos = 0;
                }
            }
            Some(target) => {
                if let Some(current) = self.doc_id {
                    if current >= target {
                        return true;
                    }
                }
                // Move all pointers from lists[0] to lists[pivot] to `target`.
                if !self.cursors.is_empty() {
                    let last = self.pivot.min(self.cursors.len() - 1);
                    for i in 0..=last {
                        let cursor = &mut self.cursors[i];
                        let list = list_of(cursor, fields, terms);
                        if cursor.doc(list).is_some_and(|doc| doc < target) {
                            cursor.seek(list, target);
                        }
                    }
                }
            }
        }
        loop {
            // `Next()` keeps the cursors sorted by doc id and drops the
            // exhausted tail, subtracting its upper bound from the total.
            self.cursors
                .sort_by_key(|c| doc_of(c, fields, terms).unwrap_or(u64::MAX));
            while self
                .cursors
                .last()
                .is_some_and(|c| doc_of(c, fields, terms).is_none())
            {
                self.cursors.pop();
            }
            if self.cursors.is_empty() {
                break;
            }
            let total: f64 = self.cursors.iter().map(|c| c.upper_bound).sum();
            // `<` (not `<=`): a document that ties the k-th score can still win
            // on the smaller doc id.
            if total < self.threshold {
                break;
            }
            // `FindPivotOptimized`: pivot = first prefix whose upper bound
            // reaches the threshold.
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
            let Some(doc) = doc_of(&self.cursors[pivot], fields, terms) else {
                break;
            };
            while pivot + 1 < self.cursors.len()
                && doc_of(&self.cursors[pivot + 1], fields, terms) == Some(doc)
            {
                pivot += 1;
            }
            // If lists[0] is not on the pivot document yet, another list owns
            // it: move the prefix up to it and re-pivot.
            if doc_of(&self.cursors[0], fields, terms) != Some(doc) {
                for i in 0..pivot {
                    let cursor = &mut self.cursors[i];
                    let list = list_of(cursor, fields, terms);
                    if cursor.doc(list).is_some_and(|cur| cur < doc) {
                        cursor.seek(list, doc);
                    }
                }
                continue;
            }
            // `NextShallow` + `BlockMaxBM25Score` over the prefix. Every prefix
            // cursor sits on `doc`, so the sum bounds `doc`'s exact score.
            let mut block_bound = 0.0;
            for i in 0..=pivot {
                let list = list_of(&self.cursors[i], fields, terms);
                let doc_len = &fields[self.cursors[i].field].0.doc_len;
                block_bound += self.cursors[i].block_max(list, doc_len, self.mode);
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

    /// The reference's block-skip path (`blockmax_wand_iterator_impl.cpp:449-524`):
    /// every document in `[doc, skip_to)` is bounded by the prefix's block
    /// maxima, which stayed below the k-th score, so the whole run is stepped
    /// over in one seek per cursor. The run is capped at the next document any
    /// cursor is already sitting on, which keeps documents that a later pivot
    /// still needs reachable.
    fn skip_block_range(&mut self, doc: u64, pivot: usize) {
        let fields = self.fields;
        let terms = self.terms;
        let up_to = self
            .cursors
            .get(pivot + 1)
            .and_then(|c| doc_of(c, fields, terms));
        let mut block_last = u64::MAX;
        for cursor in &self.cursors[..=pivot] {
            block_last = block_last.min(cursor.block_last(list_of(cursor, fields, terms)));
        }
        let skip_to = match up_to {
            Some(up_to) => (block_last + 1).min(up_to),
            None => block_last + 1,
        };
        debug_assert!(skip_to > doc);
        for i in 0..=pivot {
            let cursor = &mut self.cursors[i];
            let list = list_of(cursor, fields, terms);
            if cursor.doc(list).is_some_and(|cur| cur < skip_to) {
                cursor.seek(list, skip_to);
            }
        }
    }

    /// Sums the exact `BM25Score()` of the prefix cursors, all parked on the
    /// same document.
    fn score_prefix(&mut self, pivot: usize) -> f64 {
        let fields = self.fields;
        let terms = self.terms;
        let ranker = Bm25Ranker::new(self.ranker.delta);
        // Sum in cursor creation order (field-major, then term) so the value is
        // independent of the doc-id sort, matching the scan path bit for bit.
        let mut prefix: Vec<usize> = (0..=pivot).collect();
        prefix.sort_by_key(|&i| self.cursors[i].ord);
        let mut score = 0.0;
        match self.mode {
            WandMode::Bm25 => {
                for &i in &prefix {
                    let cursor = &self.cursors[i];
                    let (index, weights) = &fields[cursor.field];
                    let n = index.n_docs.max(1) as f64;
                    let avg = (index.total_len as f64 / n).max(1e-9);
                    let post = &index.postings[&terms[cursor.term]][cursor.pos];
                    let tf = post.1 as f64;
                    let dl = *index.doc_len.get(&post.0).unwrap_or(&1) as f64;
                    let df = index.postings[&terms[cursor.term]].len() as f64;
                    let idf = ranker.idf(n, df);
                    let w = weights.get(cursor.term).copied().unwrap_or(1.0);
                    score += ranker.score(tf, dl, avg, idf, w);
                }
            }
            // `similarity=boolean`: one point per matched term.
            WandMode::Boolean => {
                for &i in &prefix {
                    let cursor = &self.cursors[i];
                    score += fields[cursor.field]
                        .1
                        .get(cursor.term)
                        .copied()
                        .unwrap_or(1.0);
                }
            }
        }
        self.score_evaluations += 1;
        for cursor in &self.cursors[..=pivot] {
            self.max_scored_block = self.max_scored_block.max(cursor.pos / BLOCK_SIZE);
        }
        score
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn index(postings: &[(&str, &[(u64, u32)])], doc_len: &[(u64, u32)], n: usize) -> MergedIndex {
        let mut map = HashMap::new();
        for (term, list) in postings {
            map.insert(term.to_string(), list.to_vec());
        }
        MergedIndex {
            n_docs: n,
            total_len: doc_len.iter().map(|(_, l)| *l as u64).sum(),
            postings: map,
            doc_len: doc_len.iter().copied().collect(),
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
        // Two terms over four docs; BM25 ranking by hand-checkable scores.
        let idx = index(
            &[
                ("alpha", &[(0, 1), (1, 1), (3, 2)]),
                ("beta", &[(2, 1), (3, 1)]),
            ],
            &[(0, 2), (1, 2), (2, 2), (3, 3)],
            4,
        );
        let terms = vec!["alpha".to_string(), "beta".to_string()];
        let weights = vec![1.0, 1.0];
        let fields = vec![(idx, weights.clone())];
        let dead = HashSet::new();
        let got =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 1, WandMode::Bm25).run();
        assert_eq!(got.len(), 4);
        assert_eq!(got[0].0, 3); // both terms + length boost
        assert_eq!(got[1].0, 2); // only "beta" (higher idf than "alpha")

        let boolean =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 1, WandMode::Boolean).run();
        assert_eq!(boolean[0], (3, 2.0));
        assert_eq!(boolean[1].1, 1.0);
        // minimum_should_match=2 keeps only the doc with both terms
        let both =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 2, WandMode::Bm25).run();
        assert_eq!(both.len(), 1);
        assert_eq!(both[0].0, 3);
    }

    /// The reference's `BlockMaxBM25Score()` must bound every exact score in the
    /// block, otherwise pruning would silently drop documents.
    #[test]
    fn block_maximum_bounds_every_score_in_the_block() {
        let doc_len: Vec<(u64, u32)> = (0..400).map(|d| (d, 40 + (d % 31) as u32)).collect();
        let postings: Vec<(u64, u32)> = (0..400)
            .map(|d| (d, 1 + (d % 11).min(d % 31) as u32))
            .collect();
        let idx = index(&[("alpha", &postings)], &doc_len, 400);
        let terms = vec!["alpha".to_string()];
        let fields = vec![(idx, vec![1.7])];
        let dead = HashSet::new();
        let list = fields[0].0.postings["alpha"].clone();
        let doc_len = fields[0].0.doc_len.clone();
        let ranker = Bm25Ranker::new(0.0);
        let n = fields[0].0.n_docs as f64;
        let avg = fields[0].0.total_len as f64 / n;
        let idf = ranker.idf(n, list.len() as f64);
        let mut cursor =
            BlockMaxWandIterator::new(&fields, &terms, 1, &dead, 0.0, 1, WandMode::Bm25)
                .cursors
                .remove(0);
        for (block, chunk) in list.chunks(BLOCK_SIZE).enumerate() {
            cursor.pos = block * BLOCK_SIZE;
            cursor.cached_block = usize::MAX;
            let bound = cursor.block_max(&list, &doc_len, WandMode::Bm25);
            for post in chunk {
                let dl = *doc_len.get(&post.0).unwrap() as f64;
                let exact = ranker.score(post.1 as f64, dl, avg, idf, 1.7);
                assert!(exact <= bound + 1e-12, "block {block}: {exact} > {bound}");
            }
        }
    }

    /// Block maxima exist to stop the scan early: when the whole top-k lives in
    /// the first posting block, no later block may be scored at all.
    #[test]
    fn top_k_in_the_first_block_never_scores_later_blocks() {
        // "alpha" hits all 2000 docs. The first block is term-dense, the rest
        // are single-occurrence, so once the cut-off settles on the first block
        // every later block's maximum falls below it.
        let docs = 2000u64;
        let doc_len: Vec<(u64, u32)> = (0..docs).map(|d| (d, 100)).collect();
        let postings: Vec<(u64, u32)> = (0..docs)
            .map(|d| (d, if (d as usize) < BLOCK_SIZE { 20 } else { 1 }))
            .collect();
        let idx = index(&[("alpha", &postings)], &doc_len, docs as usize);
        let terms = vec!["alpha".to_string()];
        let fields = vec![(idx, vec![1.0])];
        let dead = HashSet::new();

        let mut it = BlockMaxWandIterator::new(&fields, &terms, 10, &dead, 0.0, 1, WandMode::Bm25);
        let top = it.next(None);
        assert!(top);
        let deepest = it.max_scored_block;
        let first_block_evals = it.score_evaluations;
        assert_eq!(deepest, 0, "scored a block past the first one");

        // The same query driven to completion keeps the top-k inside the first
        // block and never decodes a later block.
        let (best, evaluations, deepest_block) =
            BlockMaxWandIterator::new(&fields, &terms, 10, &dead, 0.0, 1, WandMode::Bm25)
                .drive_with_stats();
        assert_eq!(best.len(), 10);
        assert!(best.iter().all(|(doc, _)| (*doc as usize) < BLOCK_SIZE));
        assert_eq!(deepest_block, 0, "scored a block past the first one");
        assert_eq!(
            evaluations, BLOCK_SIZE,
            "only the first block may be decoded, got {evaluations}"
        );
        assert!(first_block_evals > 0);

        // The full-scan path decodes every document; block maxima cut the
        // exact-score work from `docs` to a single block.
        let full = BlockMaxWandIterator::new(
            &fields,
            &terms,
            docs as usize,
            &dead,
            0.0,
            1,
            WandMode::Bm25,
        );
        let (all, full_evaluations, _) = full.drive_with_stats();
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
    fn full_scan(
        fields: &[(MergedIndex, Vec<f64>)],
        terms: &[String],
        k: usize,
        dead: &HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: WandMode,
    ) -> Vec<(u64, f64)> {
        let ranker = Bm25Ranker::new(delta);
        let mut candidates: Vec<u64> = Vec::new();
        for (index, _) in fields {
            for term in terms {
                if let Some(list) = index.postings.get(term) {
                    candidates.extend(list.iter().map(|p| p.0));
                }
            }
        }
        candidates.sort_unstable();
        candidates.dedup();
        let mut scored = Vec::new();
        for doc in candidates {
            let mut score = 0.0;
            let mut seen_terms = HashSet::new();
            let mut matched = 0;
            for (index, weights) in fields {
                let n = index.n_docs.max(1) as f64;
                let avg = (index.total_len as f64 / n).max(1e-9);
                for (ti, term) in terms.iter().enumerate() {
                    let Some(list) = index.postings.get(term) else {
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
                            let dl = *index.doc_len.get(&doc).unwrap_or(&1) as f64;
                            let idf = ranker.idf(n, list.len() as f64);
                            score += ranker.score(tf, dl, avg, idf, w);
                        }
                        WandMode::Boolean => score += w,
                    }
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
        let mut doc_len = HashMap::new();
        for doc in 0..DOCS {
            doc_len.insert(doc, 5 + (lcg(&mut rng) % 200) as u32);
        }
        let mut postings = HashMap::new();
        for (ti, term) in terms.iter().enumerate() {
            let density = 30 - ti as u64 * 4;
            let mut list = Vec::new();
            for doc in 0..DOCS {
                if lcg(&mut rng) % 100 < density {
                    let len = doc_len[&doc];
                    let tf = 1 + (lcg(&mut rng) % len as u64) as u32;
                    list.push((doc, tf));
                }
            }
            postings.insert(term.clone(), list);
        }
        let total_len: u64 = doc_len.values().map(|l| *l as u64).sum();
        let index = MergedIndex {
            n_docs: DOCS as usize,
            total_len,
            postings,
            doc_len: doc_len.clone(),
        };
        let dead: HashSet<u64> = (0..DOCS)
            .filter(|_| lcg(&mut rng).is_multiple_of(20))
            .collect();
        let fields = vec![(index, vec![1.0, 0.5, 1.0, 2.0, 1.0])];

        for k in [1usize, 10, 100] {
            for (delta, min_should) in [(0.0, 1usize), (0.5, 1), (0.0, 2)] {
                for mode in [WandMode::Bm25, WandMode::Boolean] {
                    let want = full_scan(&fields, &terms, k, &dead, delta, min_should, mode);
                    let got = BlockMaxWandIterator::new(
                        &fields, &terms, k, &dead, delta, min_should, mode,
                    )
                    .run();
                    assert_eq!(
                        got,
                        want,
                        "k={k} delta={delta} min_should={min_should} mode={}",
                        mode == WandMode::Bm25
                    );
                }
            }
        }

        // Block maxima must also pay off on the random corpus: the exhaustive
        // path touches every candidate, the pivot search only those whose block
        // bound can still beat the k-th score.
        let mut candidates: Vec<u64> = Vec::new();
        for (index, _) in &fields {
            for term in &terms {
                if let Some(list) = index.postings.get(term) {
                    candidates.extend(list.iter().map(|post| post.0));
                }
            }
        }
        candidates.sort_unstable();
        candidates.dedup();
        let (best, evaluations, _) =
            BlockMaxWandIterator::new(&fields, &terms, 10, &dead, 0.0, 1, WandMode::Bm25)
                .drive_with_stats();
        assert_eq!(
            best,
            full_scan(&fields, &terms, 10, &dead, 0.0, 1, WandMode::Bm25)
        );
        assert!(
            evaluations * 2 < candidates.len(),
            "{evaluations} exact scores for {} candidates: no pruning happened",
            candidates.len()
        );
    }
}

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
use crate::WAND_BLOCK;
use crate::{WandField, WandStream, WandUnit};

/// MAX_DOC_PER_RECORD (index_defines.cppm:43): the fixed run of documents that
/// forms one posting block, i.e. the granularity of block_max_tf /
/// block_max_percentage. SkipListReader returns one entry per block.
#[cfg(test)]
const BLOCK_SIZE: usize = WAND_BLOCK;

/// numeric_limits<u16>::max(): block_max_percentage is stored as a u16 fraction
/// of the block's largest tf/doc-length ratio (doc_list_encoder_impl.cpp:166).
const PERCENTAGE_SCALE: f64 = u16::MAX as f64;

/// BM25Ranker (bm25_ranker_impl.cpp:25-34): k1 = 1.2, b = 0.75 by default,
/// idf = ln(1 + (N - df + 0.5) / (df + 0.5)), plus the optional delta_term added
/// to the tf norm. `bm25_param_k1` / `bm25_param_b` override the two constants
/// (`bound_select_statement_impl.cpp:239-252`).
///
/// Every quantity is `f32`, like the reference: `k1`, `b`, `delta`, and the
/// derived `bm25_common_score_` / `f1` / `f2` / `f3` / `f4` / `avg_column_len_`
/// are all `float` in `TermDocIterator` and `PhraseDocIterator`
/// (`term_doc_iterator_impl.cpp:55-77`, `phrase_doc_iterator_impl.cpp:41-56`),
/// and every accumulator in the WAND is `float` too
/// (`blockmax_wand_iterator_impl.cpp:328,349,402`). Computing in `f64` and
/// rounding once at the end does not reproduce that: the difference is ~1e-3
/// on a score of 15, enough to swap two documents at the top-n boundary.
#[derive(Clone)]
pub(crate) struct Bm25Ranker {
    pub k1: f32,
    pub b: f32,
    pub delta: f32,
}

impl Bm25Ranker {
    /// The reference defaults (`kBM25K1 = 1.2`, `kBM25B = 0.75`).
    pub fn new(delta: f32) -> Self {
        Self::with_params(1.2, 0.75, delta)
    }

    /// `bm25_param_k1=<f>` / `bm25_param_b=<f>`: the values
    /// `bound_select_statement_impl.cpp:239-252` writes into `BM25Params`.
    pub fn with_params(k1: f32, b: f32, delta: f32) -> Self {
        Self { k1, b, delta }
    }

    /// `smooth_idf = log1p((total_df - doc_freq + 0.5F) / (doc_freq + 0.5F))`
    /// (`term_doc_iterator_impl.cpp:70`): the division is `float`, then
    /// `std::log1p` on the float promotes to `double`, and the result is stored
    /// back into a `float`.
    pub fn idf(&self, n_docs: f32, df: f32) -> f32 {
        let ratio = ((n_docs - df) + 0.5) / (df + 0.5);
        (1.0f64 + f64::from(ratio)).ln() as f32
    }

    /// idf * (tf_norm + delta) * weight, with
    /// tf_norm = (k1+1)*tf / (tf + k1*((1-b) + b*dl/avgdl)).
    #[cfg(test)]
    pub fn score(&self, tf: f32, dl: f32, avgdl: f32, idf: f32, weight: f32) -> f32 {
        self.score_with(tf, dl, avgdl, idf, weight, self.delta)
    }

    /// [`Self::score`] with the unit's own `delta` (`delta_term` for a term,
    /// `delta_phrase` for a phrase).
    ///
    /// `TermDocIterator::BM25Score` (`term_doc_iterator_impl.cpp:154-175`)
    /// evaluates `p = f1 + f2 * doc_len` and then
    /// `bm25_common_score_ * (tf / (tf + p) + f4)` — every step in `float`, and
    /// `f1`/`f2`/`f4` are the precomputed members, not a re-derivation. The
    /// shape is reproduced here with the same intermediate values so the
    /// rounding matches.
    pub fn score_with(
        &self,
        tf: f32,
        dl: f32,
        avgdl: f32,
        idf: f32,
        weight: f32,
        delta: f32,
    ) -> f32 {
        let f1 = self.f1();
        let f2 = self.f2(avgdl);
        let f4 = self.f4(delta);
        let common = self.common(idf, weight);
        let p = f1 + f2 * dl;
        common * (tf / (tf + p) + f4)
    }

    /// `f4 = delta / (k1 + 1)` for an arbitrary per-unit delta
    /// (`term_doc_iterator_impl.cpp:76`).
    pub fn f4(&self, delta: f32) -> f32 {
        delta / (self.k1 + 1.0)
    }

    /// tf_norm <= k1+1, so this is a safe upper bound for pruning.
    pub fn upper_bound(&self, idf: f32, weight: f32) -> f32 {
        idf * (self.k1 + 1.0 + self.delta.max(0.0)) * weight
    }

    /// bm25_common_score_ (term_doc_iterator_impl.cpp:71):
    /// weight * idf * (k1 + 1). Both the whole-term bound and every block bound
    /// are this factor times a tf-only term.
    pub fn common(&self, idf: f32, weight: f32) -> f32 {
        weight * idf * (self.k1 + 1.0)
    }

    /// f1 = k1 * (1 - b) (term_doc_iterator_impl.cpp:73).
    pub fn f1(&self) -> f32 {
        self.k1 * (1.0 - self.b)
    }

    /// f2 = k1 * b / avgdl (term_doc_iterator_impl.cpp:74).
    pub fn f2(&self, avgdl: f32) -> f32 {
        self.k1 * self.b / avgdl.max(1e-9)
    }

    /// f3 = k1 * b / avgdl * u16::MAX (term_doc_iterator_impl.cpp:74-75), the
    /// tf/doc-length ratio term of BlockMaxBM25Score.
    pub fn f3(&self, avgdl: f32) -> f32 {
        self.f2(avgdl) * (PERCENTAGE_SCALE as f32)
    }
}

/// The rows a query may score, ascending: the `FILTER (…)` / `IN (…)` /
/// `filter=…` answer, evaluated once over the table.
///
/// The iterator consumes it as a **seek target**. Without it a rejected
/// candidate is scored (its block bound is decoded) and then dropped by
/// `offer`, one pivot per rejected document; with it the cursors hop to the
/// next admitted row, so a selective filter costs one seek per surviving row
/// instead of one pivot per rejected one.
pub(crate) struct DocFilter {
    allowed: Vec<u64>,
}

impl DocFilter {
    /// `rows` must be sorted and deduplicated (see `Engine::allowed_rows`).
    pub(crate) fn new(rows: Vec<u64>) -> Self {
        Self { allowed: rows }
    }

    pub(crate) fn contains(&self, doc: u64) -> bool {
        self.allowed.binary_search(&doc).is_ok()
    }

    /// The first admitted row `>= doc`.
    pub(crate) fn next_ge(&self, doc: u64) -> Option<u64> {
        let at = self.allowed.partition_point(|row| *row < doc);
        self.allowed.get(at).copied()
    }

    #[cfg(test)]
    pub(crate) fn len(&self) -> usize {
        self.allowed.len()
    }
}

/// similarity=boolean scores one point per matched term (per field); the default
/// is BM25. Both are handled inside the same iterator.
#[derive(Clone, Copy, PartialEq, Eq)]
pub(crate) enum WandMode {
    Bm25,
    Boolean,
}

/// The reference's `BlockMaxWandStats`: what one scan cost.
#[cfg(test)]
#[derive(Clone, Copy, Default, Debug)]
pub(crate) struct WandStats {
    pub iterations: u64,
    pub docs_scored: usize,
    pub child_advances: u64,
    pub block_seeks: u64,
    pub heap_ops: u64,
}

/// One (field, term) posting cursor: the reference's TermDocIterator.
struct Cursor<'a> {
    /// Position of this cursor in creation order (field-major, then term).
    /// Scores are summed in this order so the result never depends on how the
    /// doc-id sort permuted the cursors.
    ord: usize,
    term: usize,
    weight: f32,
    /// bm25_common_score_; zero for similarity=boolean.
    common: f32,
    /// BM25ScoreUpperBound() of the whole term.
    upper_bound: f32,
    /// f1, f3, f4 of TermDocIterator::InitBM25Info.
    f1: f32,
    f3: f32,
    f4: f32,
    /// Field statistics of this unit's BM25 score.
    avg: f32,
    idf: f32,
    /// `delta_term` for a term, `delta_phrase` for a phrase.
    delta: f32,
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
    block_max: f32,
    block_last_doc: u64,
    /// `(last, max_tf, pct)` of the block `block_index` names, so `block_last`
    /// and `block_max` agree and neither re-reads the stream.
    block_stats_cache: (u64, f32, u16),
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
        // The cursor left the block its bound was computed for; drop the cached
        // pair so `block_last()`/`block_max()` answer for the block it lands in.
        // `block_bounds` recomputes whenever `block_index` no longer matches the
        // stream's block id, and `u64::MAX` can never match one.
        self.block_index = u64::MAX;
    }

    /// BlockLastDocID(): the last document of the current block.
    ///
    /// Refreshes on first use after a `seek`: the cursor may have moved and the
    /// cached pair belongs to the block it left.
    fn block_last(&mut self) -> u64 {
        self.block_bounds();
        self.block_last_doc
    }

    /// The block's `(last, max_tf, pct)`, recomputed whenever the cursor is in a
    /// block whose bounds are not cached yet. `block_last()` and `block_max()`
    /// both read through here, so a cursor that was `seek`-ed can never answer
    /// with the bounds of the block it left.
    fn block_bounds(&mut self) -> (u64, f32, u16) {
        let id = self.stream.block_id();
        if self.block_index != id {
            let (last, max_tf, pct) = self.stream.block_stats();
            self.block_index = id;
            self.block_last_doc = last;
            self.block_stats_cache = (last, max_tf as f32, pct);
        }
        self.block_stats_cache
    }

    /// BlockMaxBM25Score() (term_doc_iterator_impl.cpp:91-102): the block's
    /// largest tf and largest tf/doc-length ratio, fed through the same BM25
    /// shape as the exact score. Cached per block like the reference's
    /// block_max_bm25_score_cache_end_id_.
    ///
    /// The cache key is the block **identity**, which `seek` invalidates: a
    /// block-level seek left the old block's pair in place until the next
    /// `load`, and the WAND reads `block_last()` *before* it asks for the bound
    /// to decide whether a cursor still covers a candidate. Serving the
    /// previous block's pair there made a cursor that had moved to a later block
    /// keep claiming it covered rows it had already passed.
    fn block_max(&mut self, mode: WandMode) -> f32 {
        let (_, max_tf, pct) = self.block_bounds();
        let value = match mode {
            // One point per matched term, independent of tf and length.
            WandMode::Boolean => self.weight,
            WandMode::Bm25 => {
                // `bm25_common_score_ * (1/(1 + f1/block_max_tf +
                // f3/block_max_percentage) + f4)` with the stored `u16`
                // percentage (`term_doc_iterator_impl.cpp:98-100`), in `float`.
                let norm = 1.0 / (1.0 + self.f1 / max_tf.max(1.0) + self.f3 / (pct.max(1) as f32));
                (self.common * (norm + self.f4)).min(self.upper_bound)
            }
        };
        self.block_max = value;
        value
    }
}

/// BlockMaxWandIterator over the merged per-field streams. Terms are single-unit
/// OR terms (no phrase / require-all).
pub(crate) struct BlockMaxWandIterator<'a> {
    fields: usize,
    dead: &'a common::RowBitmap,
    /// Rows a `FILTER(…)` admits; `None` scores every row the postings reach.
    /// See [`DocFilter`] for why it is a row list and not a predicate closure.
    filter: Option<&'a DocFilter>,
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
    ///
    /// Kept in `f64` although every `upper_bound` is an `f32`: this number is a
    /// *pruning* ledger, and pruning may only ever be too generous. The ledger
    /// is maintained by adding a cursor's bound at birth and subtracting the
    /// same bound at retirement, and an `f32` sum does not cancel - adding
    /// `16777216f32` and `1f32` yields `16777216f32`, so retiring the big
    /// cursor leaves `0` where `1` is still owed and the search stops before a
    /// live document that qualifies on its own. Summing the (exact) `f32`
    /// bounds in `f64` makes the addition and the subtraction exact for any
    /// realistic cursor count, so a retired cursor removes exactly what it
    /// contributed.
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
    threshold: f32,
    /// doc_id_ / bm25_score_cache_ of the reference: the last candidate Next()
    /// stopped on and its exact score.
    doc_id: Option<u64>,
    candidate_score: f64,
    /// Instrumentation: exact per-document score decodes (calc_score_cnt_), the
    /// deepest block any of them came from, and the control-structure work
    /// (pivot rounds, heap operations, child advances).
    score_evaluations: usize,
    max_scored_block: u64,
    rounds: u64,
    jumps: u64,
    heap_ops: u64,
    child_advances: u64,
}

impl<'a> BlockMaxWandIterator<'a> {
    pub fn new(
        fields: Vec<WandField<'a>>,
        k: usize,
        dead: &'a common::RowBitmap,
        delta: f32,
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
            let n = n_docs.max(1) as f32;
            // `avg_column_len_ = doc_len * 1.0 / doc_cnt` into a `float`
            // (term_doc_iterator_impl.cpp:66).
            let avg = (total_len as f32 / n).max(1e-9);
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
                        ranker.f4(unit_delta),
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
                    block_stats_cache: (u64::MAX, 0.0, 0),
                });
                cursors.last_mut().unwrap().refresh();
            }
        }
        let mut heap = BinaryHeap::with_capacity(cursors.len());
        let mut total_upper: f64 = 0.0;
        for cursor in &cursors {
            if let Some(doc) = cursor.cur {
                heap.push(Reverse((doc, cursor.ord)));
                total_upper += f64::from(cursor.upper_bound);
            }
        }
        Self {
            fields: field_count,
            dead,
            filter: None,
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
            threshold: f32::NEG_INFINITY,
            doc_id: None,
            candidate_score: 0.0,
            score_evaluations: 0,
            max_scored_block: 0,
            rounds: 0,
            jumps: 0,
            heap_ops: 0,
            child_advances: 0,
        }
    }

    /// `block_max=false`: score every document that any unit matches, in
    /// document order, and let only the top-k survive. Same answers as the
    /// pruned path, without the pruning.
    pub fn with_naive(mut self) -> Self {
        self.naive = true;
        self
    }

    /// `bm25_param_k1=<f>` / `bm25_param_b=<f>`: the reference folds both into
    /// `BM25Params` before any iterator is built
    /// (`bound_select_statement_impl.cpp:239-252`), so every coefficient derived
    /// from k1/b - the whole-term bound, `common`, f1, f3, f4 - is recomputed
    /// here. `similarity=boolean` never touches BM25 and is left alone.
    pub fn with_bm25_params(mut self, k1: f32, b: f32) -> Self {
        if self.mode == WandMode::Boolean {
            return self;
        }
        let ranker = Bm25Ranker::with_params(k1, b, self.ranker.delta);
        for cursor in &mut self.cursors {
            cursor.upper_bound = ranker.upper_bound(cursor.idf, cursor.weight);
            cursor.common = ranker.common(cursor.idf, cursor.weight);
            cursor.f1 = ranker.f1();
            cursor.f3 = ranker.f3(cursor.avg);
            cursor.f4 = ranker.f4(cursor.delta);
        }
        // The pruning bound is the sum over the cursors that still have a
        // document; a retired cursor contributes nothing.
        self.total_upper = self
            .cursors
            .iter()
            .filter(|c| c.cur.is_some())
            .map(|c| f64::from(c.upper_bound))
            .sum();
        self.ranker = ranker;
        self
    }

    /// `threshold=<f>`: the reference raises the block-max WAND's pruning bar to
    /// the requested score (`UpdateScoreThreshold`,
    /// `blockmax_wand_iterator_impl.cpp:233-246`, fed by
    /// `physical_match_impl.cpp`), so documents whose bound cannot reach it are
    /// skipped instead of decoded and then dropped. `t <= 0` means "no
    /// threshold" - the reference's `threshold_ <= 0.0f` shortcut.
    pub fn with_threshold(mut self, t: f32) -> Self {
        if t > 0.0 {
            self.threshold = self.threshold.max(t);
        }
        self
    }

    /// Restricts scoring to `filter`'s rows - the `FILTER (…)` / `IN (…)` /
    /// `filter=…` pushdown. Without it every row the postings reach is a
    /// candidate and the rejected ones are dropped after being scored.
    pub fn with_filter(mut self, filter: &'a DocFilter) -> Self {
        self.filter = Some(filter);
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
            let (blocks, postings) = crate::take_decode_stats();
            eprintln!(
                "[WAND] scored={} iterations={} jumps={} child_advances={} heap_ops={} retired={}                  blocks={} postings={} cursors={} df_total={}",
                self.score_evaluations,
                self.rounds,
                self.jumps,
                self.child_advances,
                self.heap_ops,
                self.cursors.len() - self.heap.len(),
                blocks,
                postings,
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

    /// [`Self::run`] plus the counters of the scan it just finished.
    #[cfg(test)]
    pub(crate) fn run_with_stats(mut self) -> (Vec<(u64, f64)>, WandStats) {
        self.drive();
        let stats = self.stats();
        (self.best, stats)
    }

    /// `Next(target)` without the top-k wrapper: the caller sees every candidate
    /// the pruned path stops on, which is what a `topn=0` scan does.
    ///
    /// `min_should` is applied here too. `offer` is the top-k wrapper's
    /// eligibility check, and the full-scan path never reaches it, so without
    /// this a `topn=0` scan of `"a b"` with `min_should=2` returned a document
    /// matching only `a` - and a test that compares the pruned scan against the
    /// naive one would agree while both violated `minimum_should_match`.
    #[cfg(test)]
    pub(crate) fn step(&mut self, target: Option<u64>) -> bool {
        let mut target = target;
        loop {
            if !self.next(target) {
                return false;
            }
            let Some(doc) = self.doc_id else {
                return false;
            };
            let eligible =
                self.matched_units() >= self.min_should.max(1) && !self.dead.contains(doc);
            if eligible {
                return true;
            }
            // Rejected: resume immediately after this document.
            target = Some(doc + 1);
        }
    }

    /// How many query units the current candidate matched, in [`Self::offer`]'s
    /// definition: distinct terms for BM25, `(field, term)` pairs for boolean.
    #[cfg(test)]
    fn matched_units(&self) -> usize {
        match self.mode {
            WandMode::Bm25 => {
                let mut seen = HashSet::new();
                for &i in &self.prefix {
                    seen.insert(self.cursors[i].term);
                }
                seen.len()
            }
            WandMode::Boolean => self.prefix.len(),
        }
    }

    /// The document and exact score `step` stopped on.
    #[cfg(test)]
    pub(crate) fn current(&self) -> Option<(u64, f64)> {
        self.doc_id.map(|doc| (doc, self.candidate_score))
    }

    /// The reference's `BlockMaxWandStats`.
    #[cfg(test)]
    pub(crate) fn stats(&self) -> WandStats {
        WandStats {
            iterations: self.rounds,
            docs_scored: self.score_evaluations,
            child_advances: self.child_advances,
            // `jumps` counts the rounds that moved the prefix to a cursor that
            // still sat before the pivot; the reference counts the same rounds
            // as block seeks (`block_seeks_`).
            block_seeks: self.jumps,
            heap_ops: self.heap_ops,
        }
    }

    /// Hands a cursor the last round popped back to the heap, resolving the
    /// block cursor it was seek-ed to. A cursor that ran out is retired: its
    /// whole upper bound leaves the search.
    fn push_back(&mut self, i: usize) {
        let cursor = &mut self.cursors[i];
        cursor.ensure();
        self.child_advances += 1;
        match cursor.cur {
            Some(doc) => {
                self.heap_ops += 1;
                self.heap.push(Reverse((doc, cursor.ord)))
            }
            None => self.total_upper -= f64::from(cursor.upper_bound),
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
        if matched < self.min_should.max(1) || self.dead.contains(doc_id) {
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
            // `UpdateScoreThreshold` only ever raises the bar
            // (`blockmax_wand_iterator_impl.cpp:233-246`), so a user
            // `threshold=` survives a weak k-th score.
            self.threshold = self.threshold.max(self.best[self.k - 1].1 as f32);
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
            // Start the reposition at the next row the filter admits: a rejected
            // row can never be scored, so no cursor needs to stop there.
            let Some(target) = self.lift(target) else {
                self.doc_id = None;
                return false;
            };
            // Reposition every cursor below the target: the ones the last pivot
            // parked on the candidate, and every cursor still in the heap that
            // sits before it. A cursor the last round never parked on the
            // candidate was left at its own document, and without this the next
            // pivot would stop there and hand back a document *before* the one
            // requested. The reference's heap variant does the same
            // (`while (!head_.empty() && ctx_[head_.front()]->DocID() < target)
            // PushBack(PopHead(), target)`, heap_blockmax_wand_iterator_impl.cpp:379).
            self.skip_to(target);
        }
        loop {
            self.rounds += 1;
            self.prefix.clear();
            // '<' (not '<='): a document that ties the k-th score can still win
            // on the smaller doc id, and the heap is already doc-ordered, so
            // this needs no extra bookkeeping. Both sides are widened to `f64`:
            // the ledger is `f64` (see `total_upper`), and comparing it against
            // the `f32` threshold in `f32` would round the ledger down.
            if self.total_upper < f64::from(self.threshold) {
                break;
            }
            // (1) FindPivotOptimized: the pivot is the first prefix whose upper
            // bound reaches the threshold, plus everything tied on its document.
            let mut sum: f32 = 0.0;
            let mut pivot = None;
            while let Some(Reverse((doc, i))) = self.heap.pop() {
                self.heap_ops += 1;
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
            while let Some(&Reverse((tied, i))) = self.heap.peek() {
                if tied != doc {
                    break;
                }
                self.heap.pop();
                self.heap_ops += 1;
                self.prefix.push(i);
            }
            // The pivot must land on a row the filter admits. Hopping the cursors
            // here is what makes the filter a seek instead of a post-score drop:
            // a rejected candidate would otherwise be decoded, bounded and
            // offered only to be thrown away, one pivot per rejected row.
            if let Some(filter) = self.filter {
                if !filter.contains(doc) {
                    let Some(next) = filter.next_ge(doc + 1) else {
                        break;
                    };
                    self.skip_to(next);
                    continue;
                }
            }
            // (2) Upper bound at the candidate. A cursor parked on the document
            // or inside a block that covers it contributes that block's bound; a
            // cursor whose block ends before the candidate contributes nothing
            // (it has no posting there); one that has to move first gets a
            // block-level seek, which then decides which of the two it is.
            let mut sum_at_doc: f32 = 0.0;
            for k in 0..self.prefix.len() {
                let i = self.prefix[k];
                if self.cursors[i].cur != Some(doc) {
                    if self.cursors[i].block_last() >= doc {
                        sum_at_doc += self.cursors[i].block_max(self.mode);
                        continue;
                    }
                    self.cursors[i].seek(doc);
                    self.resolve(i);
                    if self.cursors[i].cur != Some(doc) {
                        continue;
                    }
                }
                sum_at_doc += self.cursors[i].block_max(self.mode);
            }
            // (3) Decision, the same shape as the reference's: score only when
            // every prefix cursor is parked on the candidate, and only return a
            // document whose exact score beats the k-th.
            if sum_at_doc > self.threshold {
                if self
                    .prefix
                    .iter()
                    .all(|&i| self.cursors[i].cur == Some(doc))
                {
                    let score = self.score_prefix();
                    if score > f64::from(self.threshold) {
                        self.doc_id = Some(doc);
                        self.candidate_score = score;
                        return true;
                    }
                } else {
                    // A cursor still sits before the candidate: advance the
                    // prefix to it and re-pivot.
                    self.jumps += 1;
                    self.advance_prefix(doc);
                    continue;
                }
            }
            self.skip_blocks(doc);
            for i in std::mem::take(&mut self.prefix) {
                self.push_back(i);
            }
        }
        self.doc_id = None;
        false
    }

    /// Moves the prefix cursors that sit before doc up to it and hands them back
    /// to the heap.
    fn advance_prefix(&mut self, doc: u64) {
        for i in std::mem::take(&mut self.prefix) {
            let cursor = &mut self.cursors[i];
            if cursor.cur.is_some_and(|cur| cur < doc) {
                cursor.seek(doc);
            }
            self.push_back(i);
        }
    }

    /// The first row at or after `doc` the filter admits; `doc` itself when the
    /// query has no filter.
    fn lift(&self, doc: u64) -> Option<u64> {
        match self.filter {
            Some(filter) => filter.next_ge(doc),
            None => Some(doc),
        }
    }

    /// Moves every cursor that still sits before `next` up to it: the pivot
    /// prefix (already out of the heap) and the heap entries below it. Rows in
    /// between are ones the filter rejected, so no cursor needs to stop there.
    fn skip_to(&mut self, next: u64) {
        self.jumps += 1;
        self.advance_prefix(next);
        let mut moved: Vec<usize> = Vec::new();
        while let Some(&Reverse((doc, i))) = self.heap.peek() {
            if doc >= next {
                break;
            }
            self.heap.pop();
            self.heap_ops += 1;
            moved.push(i);
        }
        for i in moved {
            self.cursors[i].seek(next);
            self.push_back(i);
        }
    }

    /// Resolves a cursor the round moved: its document, and the bound of the
    /// block it landed on (which also refreshes block_last_doc).
    fn resolve(&mut self, i: usize) {
        self.cursors[i].ensure();
        self.child_advances += 1;
        if self.cursors[i].cur.is_some() {
            self.cursors[i].block_max(self.mode);
        }
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
            let Some(target) = self.lift(target) else {
                self.doc_id = None;
                return false;
            };
            // Same repositioning as the pruned path: the parked prefix and every
            // heap cursor left below the target move up to it.
            self.skip_to(target);
        }
        loop {
            self.prefix.clear();
            let Some(&Reverse((doc, _))) = self.heap.peek() else {
                self.doc_id = None;
                return false;
            };
            // Same pushdown as the pruned path: a document the filter rejects is
            // never scored, so the cursors hop to the next admitted row.
            if let Some(filter) = self.filter {
                if !filter.contains(doc) {
                    let Some(next) = filter.next_ge(doc + 1) else {
                        self.doc_id = None;
                        return false;
                    };
                    self.skip_to(next);
                    continue;
                }
            }
            while let Some(&Reverse((tied, i))) = self.heap.peek() {
                if tied != doc {
                    break;
                }
                self.heap.pop();
                self.heap_ops += 1;
                self.prefix.push(i);
            }
            self.candidate_score = self.score_prefix();
            self.doc_id = Some(doc);
            return true;
        }
    }

    /// The reference's block-skip path (SkipUncompetitiveRange,
    /// heap_blockmax_wand_iterator_impl.cpp:196-341), in its three phases: move
    /// block cursors only, prove range by range that nothing in there can
    /// compete while moving the cursor whose block ends first, then position the
    /// prefix so the round can hand the cursors back to the heap.
    fn skip_blocks(&mut self, from: u64) {
        let start = from + 1;
        let up_to = self.heap.peek().map(|&Reverse((doc, _))| doc);

        // (1) Block-level reposition (NextShallow): a cursor whose current block
        // already covers start stays where it is.
        for k in 0..self.prefix.len() {
            let i = self.prefix[k];
            if self.cursors[i].cur.is_some_and(|cur| cur >= start) {
                continue;
            }
            if self.cursors[i].block_last() >= start {
                continue;
            }
            self.cursors[i].seek(start);
            self.resolve(i);
        }

        // (2) Repeat "this block range cannot compete" and push the cursor whose
        // block ends first forward by one block.
        let mut next_doc = start;
        loop {
            let mut sum_range: f32 = 0.0;
            let mut min_last = u64::MAX;
            let mut earliest = None;
            for k in 0..self.prefix.len() {
                let i = self.prefix[k];
                if self.cursors[i].cur.is_none() {
                    continue;
                }
                let last = self.cursors[i].block_last();
                if last < next_doc {
                    // Its block ends before the range: it contributes nothing.
                    continue;
                }
                sum_range += self.cursors[i].block_max(self.mode);
                if last < min_last {
                    min_last = last;
                    earliest = Some(i);
                }
            }
            let Some(i) = earliest else { break };
            // '>' like the reference: a range that can only tie cannot produce a
            // document that beats the k-th (ties never win on a larger doc id).
            if sum_range > self.threshold {
                break;
            }
            let skip_to = match up_to {
                Some(up_to) => (min_last + 1).min(up_to),
                None => min_last + 1,
            };
            if skip_to <= next_doc {
                break;
            }
            self.cursors[i].seek(skip_to);
            self.resolve(i);
            next_doc = skip_to;
        }

        // (3) Position every prefix cursor at or after next_doc.
        for k in 0..self.prefix.len() {
            let i = self.prefix[k];
            if self.cursors[i].cur.is_some_and(|cur| cur >= next_doc) {
                continue;
            }
            self.cursors[i].seek(next_doc);
            self.resolve(i);
        }
    }

    /// Sums the exact BM25Score() of the prefix cursors, all parked on the same
    /// document.
    fn score_prefix(&mut self) -> f64 {
        let ranker = self.ranker.clone();
        // The reference's `float sum_score = 0.0f` (`blockmax_wand_iterator_impl.
        // cpp:402`): the terms' scores are accumulated in `float`, one at a
        // time, and only the total leaves the iterator. Accumulating in `f64`
        // and rounding once gives a different value, which is enough to swap
        // documents at a top-n boundary.
        let mut score: f32 = 0.0;
        // Sum in cursor creation order (field-major, then term) so the value is
        // independent of the document order the heap handed us, matching the
        // scan path bit for bit. Doc order is not needed after this point.
        self.prefix.sort_by_key(|&i| self.cursors[i].ord);
        match self.mode {
            WandMode::Bm25 => {
                for k in 0..self.prefix.len() {
                    let i = self.prefix[k];
                    let cursor = &mut self.cursors[i];
                    let tf = cursor.stream.tf();
                    let dl = cursor.stream.len() as f32;
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
        f64::from(score)
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
        weights: Vec<f32>,
    ) -> WandField<'a> {
        let ranker = Bm25Ranker::new(0.0);
        let units = postings
            .iter()
            .enumerate()
            .map(|(i, (_term, list))| WandUnit {
                stream: WandStream::Term(TermStream::from_slice(list, doc_len)),
                weight: weights.get(i).copied().unwrap_or(1.0),
                idf: ranker.idf(n.max(1) as f32, list.len() as f32),
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
        assert!((idf - ((1.0f64 + 1.5 / 3.5).ln() as f32)).abs() < 1e-6);
        // tf=1, dl=2, avg=2.25
        let expected = idf * (2.2 * 1.0 / (1.0 + 1.2 * (0.25 + 0.75 * 2.0 / 2.25)));
        assert!((r.score(1.0, 2.0, 2.25, idf, 1.0) - expected).abs() < 1e-6);
        // delta adds to the norm
        let rd = Bm25Ranker::new(1.0);
        assert!((rd.score(1.0, 2.0, 2.25, idf, 1.0) - (expected + idf)).abs() < 1e-6);
    }

    #[test]
    fn wand_matches_brute_force() {
        let alpha: &[(u64, u32)] = &[(0, 1), (1, 1), (3, 2)];
        let beta: &[(u64, u32)] = &[(2, 1), (3, 1)];
        let lens: &[(u64, u32)] = &[(0, 2), (1, 2), (2, 2), (3, 3)];
        let dead = common::RowBitmap::new();
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
        let dead = common::RowBitmap::new();
        let ranker = Bm25Ranker::new(0.0);
        let n = 400.0f32;
        let total_len: u64 = doc_len.iter().map(|(_, l)| *l as u64).sum();
        let avg = total_len as f32 / n;
        let idf = ranker.idf(n, postings.len() as f32);
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
                let exact = ranker.score(*tf as f32, dl(*doc) as f32, avg, idf, 1.7);
                assert!(exact <= bound + 1e-4, "doc {doc}: {exact} > {bound}");
            }
            from = last + 1;
            cursor.seek(from);
        }
    }

    /// A rounded threshold must not make the pruning switch change the answer.
    ///
    /// `threshold=0.99999999` rounds to `1.0f32`, exactly the score of a
    /// document matching one boolean term. The pruned and the naive path must
    /// still agree on which documents survive.
    ///
    /// This covers the iterator only; the end-to-end version, where the
    /// caller's filter is the second half of the bug, is
    /// `threshold_is_a_strict_lower_bound_and_prunes_the_scan` in `tests_ut`.
    #[test]
    fn a_rounded_threshold_prunes_and_filters_identically() {
        let doc_len: Vec<(u64, u32)> = (0..2).map(|d| (d, 10)).collect();
        let dead = common::RowBitmap::new();
        let t: &[(u64, u32)] = &[(0, 1), (1, 1)];
        // Two identical fields: a document matching both scores `2.0`, and one
        // matching a single field scores `1.0` - exactly the rounded bar.
        let make = || {
            vec![
                field(&[("t", t)], &doc_len, 2, vec![1.0]),
                field(&[("t", t)], &doc_len, 2, vec![1.0]),
            ]
        };
        let bar = 0.99999999f32;
        assert_eq!(bar, 1.0, "the bar must round up to 1.0 for this test");
        let pruned = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(bar)
            .run();
        let naive = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(bar)
            .with_naive()
            .run();
        assert_eq!(pruned, naive, "block_max must not change the result set");
        assert_eq!(pruned.len(), 2, "score 2.0 clears the rounded bar");
    }

    /// A retired cursor must not shrink the remaining-bound ledger below the
    /// true bound of the cursors that are still live.
    ///
    /// `total_upper` is maintained incrementally: it starts as the sum of every
    /// cursor's bound and subtracts a cursor's bound when that cursor runs out.
    /// With `f32` the sum rounds, and subtracting the already-rounded addend
    /// does not cancel exactly - `16777216f32 + 1f32` is `16777216f32`, so the
    /// retirement of the big cursor leaves `0` where `1` is still owed.
    ///
    /// Boost `16777216` is reachable: a `BOOST` factor goes through `f32`.
    /// Field `a` then has bound `16777216` and field `b` has `1`; `a` matches
    /// only deleted row 0, `b` matches live row 1. Once `a` is exhausted the
    /// ledger must still hold `1` for `b`, otherwise the search stops before
    /// scoring row 1 - which qualifies on its own.
    #[test]
    fn retiring_a_cursor_keeps_the_remaining_bound() {
        let doc_len: Vec<(u64, u32)> = (0..2).map(|d| (d, 10)).collect();
        let dead = {
            let mut d = common::RowBitmap::new();
            d.insert(0);
            d
        };
        let a: &[(u64, u32)] = &[(0, 1)];
        let b: &[(u64, u32)] = &[(1, 1)];
        let make = || {
            vec![
                field(&[("a", a)], &doc_len, 2, vec![16_777_216.0]),
                field(&[("b", b)], &doc_len, 2, vec![1.0]),
            ]
        };
        let pruned = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(0.5)
            .run();
        let naive = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(0.5)
            .with_naive()
            .run();
        assert_eq!(
            pruned, naive,
            "pruning dropped a live document the naive scan keeps"
        );
        assert_eq!(pruned.len(), 1, "row 1 must survive");
        assert_eq!(pruned[0].0, 1);
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
        let dead = common::RowBitmap::new();
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

    /// The filtered pushdown returns exactly what a post-filtered scan does, and
    /// only scores admitted rows.
    #[test]
    fn filter_pushdown_matches_a_post_filtered_scan() {
        const DOCS: u64 = 2000;
        let doc_len: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 20 + (d % 37) as u32)).collect();
        let postings: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 1 + (d % 5) as u32)).collect();
        let dead = common::RowBitmap::new();
        // Every fifth row survives the filter.
        let allowed: Vec<u64> = (0..DOCS).step_by(5).collect();
        let filter = DocFilter::new(allowed.clone());
        assert_eq!(filter.len(), allowed.len());
        assert_eq!(filter.next_ge(3), Some(5));
        assert!(filter.contains(10));
        assert!(!filter.contains(11));
        assert_eq!(filter.next_ge(DOCS), None);

        let make = || {
            vec![field(
                &[("alpha", &postings)],
                &doc_len,
                DOCS as usize,
                vec![1.0],
            )]
        };
        let k = allowed.len();
        let got = BlockMaxWandIterator::new(make(), k, &dead, 0.0, 1, WandMode::Bm25)
            .with_filter(&filter)
            .run();
        assert_eq!(got.len(), k, "every admitted row is a candidate");
        // The same query answered without the pushdown, filtered afterwards.
        let want: Vec<(u64, f64)> =
            BlockMaxWandIterator::new(make(), DOCS as usize, &dead, 0.0, 1, WandMode::Bm25)
                .run()
                .into_iter()
                .filter(|(doc, _)| filter.contains(*doc))
                .take(k)
                .collect();
        assert_eq!(got.len(), want.len());
        for (a, b) in got.iter().zip(&want) {
            assert_eq!(a.0, b.0, "different document order");
            assert!((a.1 - b.1).abs() < 1e-12, "score of doc {}", a.0);
        }
    }

    /// A rejected candidate costs a seek, not a scoring pass: the pruned path
    /// scores at most one document per admitted row, where the unfiltered scan
    /// scores every document the postings hold.
    #[test]
    fn filter_pushdown_scores_only_admitted_rows() {
        const DOCS: u64 = 2000;
        let doc_len: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 50)).collect();
        let postings: Vec<(u64, u32)> = (0..DOCS).map(|d| (d, 1)).collect();
        let dead = common::RowBitmap::new();
        let allowed: Vec<u64> = (0..DOCS).step_by(20).collect();
        let filter = DocFilter::new(allowed.clone());
        let make = || {
            vec![field(
                &[("alpha", &postings)],
                &doc_len,
                DOCS as usize,
                vec![1.0],
            )]
        };
        let (best, evaluations, _) =
            BlockMaxWandIterator::new(make(), allowed.len(), &dead, 0.0, 1, WandMode::Bm25)
                .with_filter(&filter)
                .drive_with_stats();
        assert_eq!(best.len(), allowed.len());
        assert!(
            evaluations <= allowed.len(),
            "scored {evaluations} documents for {} admitted rows",
            allowed.len()
        );
        let (_, without, _) =
            BlockMaxWandIterator::new(make(), DOCS as usize, &dead, 0.0, 1, WandMode::Bm25)
                .drive_with_stats();
        assert_eq!(
            without, DOCS as usize,
            "the unfiltered scan scores every row"
        );
        assert!(without >= evaluations * 10, "the pushdown skipped nothing");
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
        weights: &[f32],
        terms: &[String],
        k: usize,
        dead: &common::RowBitmap,
        delta: f32,
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
        let avg = (total_len as f32 / n.max(1) as f32).max(1e-9);
        let mut scored = Vec::new();
        for doc in candidates {
            let mut score = 0.0f32;
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
                        let tf = list[pos].1 as f32;
                        let dl = *lens.get(&doc).unwrap_or(&1) as f32;
                        let idf = ranker.idf(n.max(1) as f32, list.len() as f32);
                        score += ranker.score(tf, dl, avg, idf, w);
                    }
                    WandMode::Boolean => score += w,
                }
            }
            let hits = match mode {
                WandMode::Bm25 => seen_terms.len(),
                WandMode::Boolean => matched,
            };
            if hits < min_should.max(1) || dead.contains(doc) {
                continue;
            }
            scored.push((doc, f64::from(score)));
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
        let dead: common::RowBitmap = (0..DOCS)
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
                    idf: ranker.idf(DOCS as f32, postings[t].len() as f32),
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
            let dead: common::RowBitmap = (0..DOCS).filter(|_| lcg(&mut rng) % 13 == 0).collect();
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
                        idf: ranker.idf(DOCS as f32, postings[t].len() as f32),
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
        let dead = common::RowBitmap::new();
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
        let n = DOCS as f32;
        let idf_a = ranker.idf(n, a.len() as f32);
        let idf_b = ranker.idf(n, b.len() as f32);
        let make = || {
            let cursor = crate::PhraseCursor::new(
                vec![
                    TermStream::from_postings(a.clone(), &lens),
                    TermStream::from_postings(b.clone(), &lens),
                ],
                0,
            );
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
        let dead = common::RowBitmap::new();
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
        let mut cursor = crate::PhraseCursor::new(
            vec![
                TermStream::from_postings(a, &lens),
                TermStream::from_postings(b, &lens),
            ],
            0,
        );
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

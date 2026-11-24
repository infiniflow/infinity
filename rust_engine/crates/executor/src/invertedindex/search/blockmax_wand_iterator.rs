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

use std::collections::HashSet;

#[cfg(test)]
use crate::WAND_BLOCK;
use crate::{WandField, WandStream, WandUnit};

/// The next `f32` above `x` (toward `+inf`).
fn round_up32(x: f32, ulps: u32) -> f32 {
    if !x.is_finite() || x == f32::INFINITY || x == 0.0 {
        return x;
    }
    let bits = x.to_bits();
    if x.is_sign_negative() {
        f32::from_bits(bits - ulps)
    } else {
        f32::from_bits(bits + ulps)
    }
}

/// The next `f64` above `x` (toward `+inf`), used where a bound must never be
/// rounded down. `x.next_up()` is not stable, and the step only has to cover one
/// ulp, so the bits are walked directly.
fn round_up(x: f64) -> f64 {
    if !x.is_finite() || x == f64::INFINITY {
        return x;
    }
    let bits = x.to_bits();
    // Sign decides the direction, not `x >= 0.0`: `-0.0` compares equal to
    // `0.0` but stepping its bits up moves toward zero. The sums here are never
    // negative, so this only keeps the helper honest for its own sake.
    if x.is_sign_negative() {
        f64::from_bits(bits - 1)
    } else {
        f64::from_bits(bits + 1)
    }
}

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

    /// The whole-term bound: `tf_norm <= k1+1`, so this covers every score the
    /// term can produce.
    ///
    /// In `f32` it does not, quite: `score_with` evaluates `common * (tf/(tf+p)
    /// + f4)` and this evaluates `idf * (k1 + 1 + delta) * weight`, and the two
    /// round differently. With `df = 1`, `n = 1000`, `delta = 3` and
    /// `tf = dl = avg = 2^25` the score is `23.663778305` against a bound of
    /// `23.663772583`. Pruning uses this bound for the termination ledger and
    /// for the pivot, so a document above a user threshold could be skipped.
    /// The result carries the same `1 + 2^-20` margin the block bound uses.
    pub fn upper_bound(&self, idf: f32, weight: f32) -> f32 {
        let bound = idf * (self.k1 + 1.0 + self.delta.max(0.0)) * weight;
        round_up32(bound * (1.0 + 2f32.powi(-20)), 1)
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
#[repr(C)]
struct Cursor<'a> {
    /// iter_->SeekDoc position, cached so the cursors can be sorted. `None`
    /// with `done == false` means a `seek_block` still has to be resolved.
    cur: Option<u64>,
    /// The unit is exhausted.
    done: bool,
    /// Cached shallow skip-list state when `cur.is_none()` and `!done`, so
    /// repeated `block_last_shallow` / `block_max_shallow_or` calls in Step 2
    /// and `skip_blocks` Phase 2 do not re-dispatch through `WandStream` or
    /// recompute `bound_from` (2 `f32` divisions).
    shallow_valid: bool,
    /// BlockMaxBM25Score() cache: the block it was computed for, its bound and
    /// the block's last document (used by the block-skip path).
    block_index: u64,
    block_max_index: u64,
    block_last_doc: u64,
    shallow_last: u64,
    shallow_max: Option<f32>,
    block_max: f32,
    /// Precomputed `common * (1 + 2^-20)` and `round_up32(upper_bound * (1 + 2^-20), 1)`
    /// for `bound_from`.
    common_margin: f32,
    capped_upper_bound: f32,
    /// f1, f3, f4 of TermDocIterator::InitBM25Info.
    f1: f32,
    f3: f32,
    f4: f32,
    weight: f32,
    /// Position of this cursor in creation order (field-major, then term).
    /// Scores are summed in this order so the result never depends on how the
    /// doc-id sort permuted the cursors.
    ord: usize,
    term: usize,
    /// `(last, max_tf, pct)` of the block `block_index` names, so `block_last`
    /// and `block_max` agree and neither re-reads the stream.
    block_stats_cache: (u64, f32, u16),
    /// bm25_common_score_; zero for similarity=boolean.
    common: f32,
    /// BM25ScoreUpperBound() of the whole term.
    upper_bound: f32,
    /// Field statistics of this unit's BM25 score.
    avg: f32,
    idf: f32,
    /// `delta_term` for a term, `delta_phrase` for a phrase.
    delta: f32,
    /// The posting stream, read one block at a time (placed last so hot cursor
    /// metadata stays in the first two cache lines).
    stream: WandStream<'a>,
}

impl<'a> Cursor<'a> {
    #[inline(always)]
    fn refresh(&mut self) {
        self.cur = self.stream.doc();
        self.done = self.cur.is_none();
        self.shallow_valid = false;
    }

    /// Resolves a cursor that was only block-seeked.
    #[inline(always)]
    fn ensure(&mut self) {
        if !self.done && self.cur.is_none() {
            self.refresh();
        }
    }

    /// Seeks to `target` and immediately resolves the cursor's `cur` document
    /// in a single pass, avoiding a second loop over `order` and `cursors`.
    #[inline(always)]
    fn seek_and_ensure(&mut self, target: u64) -> Option<u64> {
        if self.done {
            return None;
        }
        if let Some(cur) = self.cur {
            if cur >= target {
                return Some(cur);
            }
        }
        self.stream.seek_block(target);
        self.shallow_valid = false;
        self.block_index = u64::MAX;
        self.block_max_index = u64::MAX;
        let doc = self.stream.doc();
        self.cur = doc;
        self.done = doc.is_none();
        doc
    }

    /// PostingIterator::SeekDoc at the block level: the unit may still have to
    /// verify its own positions, which `ensure` pays for - so the range the
    /// WAND skips over is never resolved (`NextShallow`).
    #[inline(always)]
    fn seek(&mut self, doc: u64) {
        self.stream.seek_block(doc);
        self.cur = None;
        self.done = false;
        self.shallow_valid = false;
        // The cursor left the block its bound was computed for; drop the cached
        // pair so `block_last()`/`block_max()` answer for the block it lands in.
        // `block_bounds` recomputes whenever `block_index` no longer matches the
        // stream's block id, and `u64::MAX` can never match one.
        self.block_index = u64::MAX;
        self.block_max_index = u64::MAX;
    }

    /// `NextShallow(target)` (`term_doc_iterator_impl.cpp:135`): move the block
    /// cursor to the block that holds `target`, leaving the postings undecoded.
    ///
    /// The reference's phase 1 and 2 call this and then read
    /// `BlockLastDocID()` / `BlockMaxBM25Score()`, which its leaf answers from
    /// the skip-list item the hop landed on. `cur` stays `None` so this port's
    /// accessors take the same path (`block_last_shallow`,
    /// `block_max_shallow_or`) instead of decoding.
    ///
    /// `false` once the unit is exhausted.
    #[inline(always)]
    fn seek_shallow(&mut self, doc: u64) -> bool {
        match self.stream.skip_block_shallow(doc) {
            Some(true) => {
                self.cur = None;
                self.done = false;
                self.shallow_valid = false;
                // The cached bounds belong to the block the cursor left.
                self.block_index = u64::MAX;
                self.block_max_index = u64::MAX;
                true
            }
            Some(false) => false,
            None => {
                self.cur = None;
                self.done = true;
                self.shallow_valid = false;
                self.block_index = u64::MAX;
                self.block_max_index = u64::MAX;
                false
            }
        }
    }

    #[inline(always)]
    fn ensure_shallow_cache(&mut self, mode: WandMode) -> Option<(u64, Option<f32>)> {
        if self.cur.is_some() {
            return None;
        }
        if !self.shallow_valid {
            let last = self.stream.block_last_shallow()?;
            let max = self
                .stream
                .block_max_shallow()
                .map(|(max_tf, pct)| self.bound_from(mode, max_tf as f32, pct));
            self.shallow_last = last;
            self.shallow_max = max;
            self.shallow_valid = true;
        }
        Some((self.shallow_last, self.shallow_max))
    }

    /// `BlockMaxBM25Score()` from the skip list alone.
    #[inline(always)]
    fn block_max_shallow_or(&mut self, mode: WandMode) -> Option<f32> {
        self.ensure_shallow_cache(mode)?.1
    }

    /// `BlockLastDocID()` preferring the skip list, falling back to the decoded
    /// block for a cursor that is already cached.
    #[inline(always)]
    fn block_last_at_or_after(&mut self) -> Option<u64> {
        if self.cur.is_some() {
            Some(self.block_last())
        } else if self.shallow_valid {
            Some(self.shallow_last)
        } else {
            self.stream.block_last_shallow()
        }
    }

    /// BlockLastDocID(): the last document of the current block.
    ///
    /// Refreshes on first use after a `seek`: the cursor may have moved and the
    /// cached pair belongs to the block it left.
    #[inline(always)]
    fn block_last(&mut self) -> u64 {
        self.block_bounds();
        self.block_last_doc
    }

    /// The block's `(last, max_tf, pct)`, recomputed whenever the cursor is in a
    /// block whose bounds are not cached yet. `block_last()` and `block_max()`
    /// both read through here, so a cursor that was `seek`-ed can never answer
    /// with the bounds of the block it left.
    #[inline(always)]
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
    #[inline(always)]
    fn block_max(&mut self, mode: WandMode) -> f32 {
        let id = self.stream.block_id();
        if self.block_max_index != id {
            if self.block_index != id {
                let (last, max_tf, pct) = self.stream.block_stats();
                self.block_index = id;
                self.block_last_doc = last;
                self.block_stats_cache = (last, max_tf as f32, pct);
            }
            let (_, max_tf, pct) = self.block_stats_cache;
            self.block_max = self.bound_from(mode, max_tf, pct);
            self.block_max_index = id;
        }
        self.block_max
    }

    /// The BM25 (or boolean) bound from a block's `(max tf, ratio u16)` pair.
    ///
    /// Split out of `block_max` so [`Self::block_max_shallow_or`] computes the
    /// same value from a skip-list item without decoding the block.
    fn bound_from(&self, mode: WandMode, max_tf: f32, pct: u16) -> f32 {
        match mode {
            // One point per matched term, independent of tf and length.
            WandMode::Boolean => self.weight,
            WandMode::Bm25 => {
                // `bm25_common_score_ * (1/(1 + f1/block_max_tf +
                // f3/block_max_percentage) + f4)` with the stored `u16`
                // percentage (`term_doc_iterator_impl.cpp:98-100`), in `float`.
                //
                // Rounded up: the bound and the exact score are different
                // expressions, and in `f32` the bound can land a couple of ulps
                // *below* the score it covers (with `tf = dl = avg = 1216` and
                // `pct = 65535` the score is `1.4293134`, the bound
                // `1.4293131`), which would prune a document that clears the
                // threshold.
                // A coefficient can overflow: `bm25_param_k1=2e35` with
                // `avg = 20` makes `f3` infinite (the parameter is only checked
                // for being non-negative), `f3 / pct` is infinite too, and the
                // norm collapses to `0` - a zero bound for a block whose scores
                // are finite. Fall back to the whole-term bound, which is the
                // largest value any block can reach.
                if !self.f3.is_finite() || !self.f1.is_finite() {
                    return self.upper_bound;
                }
                let norm = 1.0 / (1.0 + self.f1 / max_tf.max(1.0) + self.f3 / (pct.max(1) as f32));
                // The margin must survive the cap. In exact arithmetic
                // `norm + f4 <= 1 + delta/(k1+1)`, so `common * (norm + f4)`
                // never exceeds `upper_bound`; with the margin the left side can
                // pass it by a rounding step (a large `delta` does it by more),
                // and a plain `min(upper_bound)` would then clip exactly the
                // margin that keeps the bound above the score. Both sides are
                // widened by the same factor, so the `min` can only pick the
                // larger of two bounds.
                let bound = self.common_margin * (norm + self.f4);
                bound.min(self.capped_upper_bound)
            }
        }
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
    /// L1-resident copy of `cursors[i].upper_bound` as `f64`, indexed by cursor id `i`,
    /// so `FindPivotOptimized` reads sequentially without touching `Cursor` structs
    /// or converting `f32 -> f64` per iteration.
    upper_bounds: Vec<f64>,
    /// The live cursors kept sorted by `(doc_id, cursor_ord)` packed into a
    /// `u128` (`(doc << 32) | ord`) so cursor indices above 65,535 never
    /// overflow into `doc_id`. Keeping an L1-resident sorted array lets
    /// `FindPivotOptimized` scan sequentially and re-merge only the moved
    /// prefix after each round.
    order: Vec<u128>,
    order_scratch: Vec<u128>,
    scratch_blocks: Vec<(u64, f32, usize)>,
    /// Number of leading entries in `order` that form the current round's
    /// pivot prefix.
    prefix_len: usize,
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
            let n = n_docs.max(1);
            // `avg_column_len_ = doc_len * 1.0 / doc_cnt` into a `float`
            // (term_doc_iterator_impl.cpp:66): the reference divides in
            // `double` and rounds the quotient once. Dividing two `f32`s
            // instead loses precision in the *numerator* first once
            // `total_len` passes 2^24 - `16777217 / 5000` is
            // `3355.443115` that way and `3355.443359` through `double`.
            let avg = ((total_len as f64 / n as f64) as f32).max(1e-9);
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
                let margin = 1.0 + 2f32.powi(-20);
                cursors.push(Cursor {
                    ord: cursors.len(),
                    term: ui,
                    weight: w,
                    common,
                    upper_bound,
                    common_margin: common * margin,
                    capped_upper_bound: round_up32(upper_bound * margin, 1),
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
                    block_max_index: u64::MAX,
                    block_max: 0.0,
                    block_last_doc: u64::MAX,
                    block_stats_cache: (u64::MAX, 0.0, 0),
                    shallow_valid: false,
                    shallow_last: u64::MAX,
                    shallow_max: None,
                });
                cursors.last_mut().unwrap().refresh();
            }
        }
        let mut order = Vec::with_capacity(cursors.len());
        let mut upper_bounds = Vec::with_capacity(cursors.len());
        let mut total_upper: f64 = 0.0;
        for cursor in &cursors {
            let ub = f64::from(cursor.upper_bound);
            upper_bounds.push(ub);
            if let Some(doc) = cursor.cur {
                order.push((u128::from(doc) << 32) | (cursor.ord as u128));
                total_upper = round_up(total_upper + ub);
            }
        }
        order.sort_unstable();
        let order_scratch = vec![0u128; order.len()];
        let num_cursors = cursors.len();
        Self {
            fields: field_count,
            dead,
            filter: None,
            k,
            min_should,
            mode,
            ranker,
            cursors,
            upper_bounds,
            order,
            order_scratch,
            scratch_blocks: Vec::with_capacity(num_cursors),
            prefix_len: 0,
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
        let margin = 1.0 + 2f32.powi(-20);
        for (idx, cursor) in self.cursors.iter_mut().enumerate() {
            cursor.upper_bound = ranker.upper_bound(cursor.idf, cursor.weight);
            self.upper_bounds[idx] = f64::from(cursor.upper_bound);
            cursor.common = ranker.common(cursor.idf, cursor.weight);
            cursor.common_margin = cursor.common * margin;
            cursor.capped_upper_bound = round_up32(cursor.upper_bound * margin, 1);
            cursor.f1 = ranker.f1();
            cursor.f3 = ranker.f3(cursor.avg);
            cursor.f4 = ranker.f4(cursor.delta);
        }
        // The pruning bound is the sum over the cursors that still have a
        // document; a retired cursor contributes nothing.
        self.recompute_upper_bound();
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
                self.cursors.len() - self.order.len(),
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
                for &entry in &self.order[..self.prefix_len] {
                    let i = (entry & 0xffff_ffff) as usize;
                    seen.insert(self.cursors[i].term);
                }
                seen.len()
            }
            WandMode::Boolean => self.prefix_len,
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

    /// Seeks the first `count` cursors in `self.order` to `target` (if not
    /// already at or past it), resolves them in one pass, retires exhausted
    /// ones, and restores sorted order.
    fn reposition_prefix_to(&mut self, count: usize, target: u64) {
        if count == 0 {
            return;
        }
        let mut retired = false;
        let mut write = 0usize;
        let mut advances = 0u64;
        let mut ops = 0u64;
        for read in 0..count {
            let i = (self.order[read] & 0xffff_ffff) as usize;
            let cursor = &mut self.cursors[i];
            let doc_opt = cursor.seek_and_ensure(target);
            advances += 1;
            if let Some(doc) = doc_opt {
                ops += 1;
                self.order[write] = (u128::from(doc) << 32) | (i as u128);
                write += 1;
            } else {
                retired = true;
            }
        }
        self.child_advances += advances;
        self.heap_ops += ops;
        self.finish_reposition(count, write, retired);
    }

    fn finish_reposition(&mut self, count: usize, write: usize, retired: bool) {
        if write < count {
            self.order.copy_within(count.., write);
            self.order.truncate(self.order.len() - (count - write));
        }
        if retired {
            self.recompute_upper_bound();
        }
        if write == 0 {
            return;
        }
        let total_len = self.order.len();
        if write == 1 {
            if total_len > 1 && self.order[0] > self.order[1] {
                let val = self.order[0];
                let pos = 1 + self.order[1..].partition_point(|&e| e < val);
                self.order.copy_within(1..pos, 0);
                self.order[pos - 1] = val;
            }
            return;
        }
        self.order[..write].sort_unstable();
        if write < total_len && self.order[write - 1] > self.order[write] {
            let left_max = self.order[write - 1];
            let right_end = self.order[write..].partition_point(|&e| e < left_max);
            if write + right_end < total_len / 2 {
                if self.order_scratch.len() < write {
                    self.order_scratch.resize(write, 0);
                }
                self.order_scratch[..write].copy_from_slice(&self.order[..write]);
                let mut l = 0usize;
                let mut r = write;
                let r_end = write + right_end;
                let mut out = 0usize;
                while l < write && r < r_end {
                    let lv = self.order_scratch[l];
                    let rv = self.order[r];
                    if lv <= rv {
                        self.order[out] = lv;
                        l += 1;
                    } else {
                        self.order[out] = rv;
                        r += 1;
                    }
                    out += 1;
                }
                if l < write {
                    self.order[out..out + (write - l)]
                        .copy_from_slice(&self.order_scratch[l..write]);
                }
            } else {
                if self.order_scratch.len() < total_len {
                    self.order_scratch.resize(total_len, 0);
                }
                let (left, right) = self.order.split_at(write);
                let scratch = &mut self.order_scratch[..total_len];
                let mut l = 0usize;
                let mut r = 0usize;
                let mut out = 0usize;
                while l < left.len() && r < right.len() {
                    let lv = left[l];
                    let rv = right[r];
                    if lv <= rv {
                        scratch[out] = lv;
                        l += 1;
                    } else {
                        scratch[out] = rv;
                        r += 1;
                    }
                    out += 1;
                }
                if l < left.len() {
                    let rem = left.len() - l;
                    scratch[out..out + rem].copy_from_slice(&left[l..]);
                } else if r < right.len() {
                    let rem = right.len() - r;
                    scratch[out..out + rem].copy_from_slice(&right[r..]);
                }
                self.order_scratch.truncate(total_len);
                std::mem::swap(&mut self.order, &mut self.order_scratch);
            }
        }
    }

    /// Recomputes the live bound from the cursors still in play.
    ///
    /// Retiring a cursor used to subtract its bound from the running ledger,
    /// which loses precision in the direction that matters: `2^53 + 1` is
    /// `2^53` even in `f64`, so removing the `2^53` bound left `0` while
    /// another cursor still contributed `1`, and the search stopped early.
    /// Summing what is left avoids the subtraction entirely; each addition is
    /// rounded up so the total can only ever be too large, never too small.
    fn recompute_upper_bound(&mut self) {
        let mut total = 0.0f64;
        for cursor in &self.cursors {
            if cursor.cur.is_none() {
                continue;
            }
            total = round_up(total + f64::from(cursor.upper_bound));
        }
        self.total_upper = total;
    }

    /// Keeps the candidate if it survives minimum_should_match, the tombstone
    /// filter and the k-th score (ties broken by the smaller doc id).
    fn offer(&mut self, doc_id: u64, score: f64) {
        // minimum_should_match: BM25 counts distinct query units across fields;
        // boolean counts every (field, term) match.
        let matched = match self.mode {
            WandMode::Bm25 => {
                let mut seen = HashSet::new();
                for &entry in &self.order[..self.prefix_len] {
                    let i = (entry & 0xffff_ffff) as usize;
                    seen.insert(self.cursors[i].term);
                }
                seen.len()
            }
            WandMode::Boolean => self.prefix_len,
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
            self.prefix_len = 0;
            // '<' (not '<='): a document that ties the k-th score can still win
            // on the smaller doc id, and the heap is already doc-ordered, so
            // this needs no extra bookkeeping.
            //
            // The ledger is the *exact* sum of the `f32` bounds, but the score
            // this bar stands in for is accumulated in `f32` (see
            // `score_prefix`), and a chain of `f32` additions can end above the
            // exact sum - those roundings go both ways, and stopping on the
            // exact value would then cut a document whose real score clears the
            // bar. Each of the `n` additions can raise the value by at most one
            // ulp (`f32::EPSILON`), so the bar is the exact sum widened by
            // `n * EPSILON`: an upper bound on any order of accumulation.
            let eps_f64 = f64::from(f32::EPSILON);
            let thresh_f64 = f64::from(self.threshold);
            let live = self.order.len() as f64;
            let bound = self.total_upper * (1.0 + live * eps_f64);
            if bound < thresh_f64 {
                break;
            }
            // (1) FindPivotOptimized: the pivot is the first prefix whose upper
            // bound reaches the threshold, plus everything tied on its document.
            //
            // The running sum is widened by the same `n * EPSILON` factor as the
            // termination bar, because this loop adds the bounds in **document**
            // order while `score_prefix` adds the same numbers in **creation**
            // order; the two orders round differently, and a partial sum that
            // reads below the bar here can still be a score above it. Stopping
            // the pivot too early is what loses the document: no pivot means the
            // round ends and that candidate is never scored.
            let mut sum: f64 = 0.0;
            let mut pivot = None;
            let mut idx = 0usize;
            let thresh_conserv = if thresh_f64 > 0.0 {
                thresh_f64 * (1.0 - live * eps_f64)
            } else {
                thresh_f64
            };
            for (pos, &entry) in self.order.iter().enumerate() {
                let i = (entry & 0xffff_ffff) as usize;
                sum += self.upper_bounds[i];
                if sum >= thresh_conserv {
                    let count = pos + 1;
                    let widened = sum * (1.0 + (count as f64) * eps_f64);
                    if widened >= thresh_f64 {
                        pivot = Some((entry >> 32) as u64);
                        idx = count;
                        break;
                    }
                }
            }
            let Some(doc) = pivot else {
                break;
            };
            while idx < self.order.len() && ((self.order[idx] >> 32) as u64) == doc {
                idx += 1;
            }
            self.heap_ops += idx as u64;
            self.prefix_len = idx;
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
            // Summed in `f64` with each step rounded up: this is the same
            // document-order sum as the pivot and, like it, stands in for a
            // score that `score_prefix` accumulates in creation order.
            let mut sum_at_doc: f64 = 0.0;
            for k in 0..self.prefix_len {
                let entry = self.order[k];
                let i = (entry & 0xffff_ffff) as usize;
                if ((entry >> 32) as u64) != doc {
                    if let Some(last) = self.cursors[i].block_last_at_or_after() {
                        if last >= doc {
                            let bound = if self.cursors[i].cur.is_some() {
                                self.cursors[i].block_max(self.mode)
                            } else {
                                self.cursors[i]
                                    .block_max_shallow_or(self.mode)
                                    .unwrap_or_else(|| self.cursors[i].block_max(self.mode))
                            };
                            sum_at_doc += f64::from(bound);
                            continue;
                        }
                    }
                    if self.cursors[i].seek_shallow(doc) {
                        let bound = self.cursors[i]
                            .block_max_shallow_or(self.mode)
                            .unwrap_or_else(|| self.cursors[i].block_max(self.mode));
                        sum_at_doc += f64::from(bound);
                        continue;
                    }
                    if self.cursors[i].done {
                        continue;
                    }
                    self.cursors[i].seek(doc);
                    self.resolve(i);
                    if self.cursors[i].cur != Some(doc) {
                        continue;
                    }
                }
                let bound = self.cursors[i].block_max(self.mode);
                sum_at_doc += f64::from(bound);
            }
            // (3) Decision, the same shape as the reference's: score only when
            // every prefix cursor is parked on the candidate, and only return a
            // document whose exact score beats the k-th.
            //
            // `sum_at_doc` adds the bounds in document order; the exact score
            // `score_prefix` produces adds the same numbers in creation order.
            // Those two orders round differently in `f32`, so a candidate whose
            // score clears the bar can still read below it here, and acting on
            // that (returning false, or skipping the document) loses it. The
            // comparison is widened by `n * EPSILON` for the same reason as the
            // termination bar; the bounds themselves are exact `f32` values.
            let widened_at_doc = sum_at_doc * (1.0 + self.prefix_len as f64 * eps_f64);
            if widened_at_doc > thresh_f64 {
                if self.order[..self.prefix_len]
                    .iter()
                    .all(|&e| self.cursors[(e & 0xffff) as usize].cur == Some(doc))
                {
                    let score = self.score_prefix();
                    if score > thresh_f64 {
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
            let next_doc = self.skip_blocks(doc);
            let prefix_len = self.prefix_len;
            self.prefix_len = 0;
            self.reposition_prefix_to(prefix_len, next_doc);
        }
        self.doc_id = None;
        false
    }

    /// Moves the prefix cursors that sit before doc up to it and restores order.
    fn advance_prefix(&mut self, doc: u64) {
        let mut move_count = self.prefix_len;
        while move_count > 0 && ((self.order[move_count - 1] >> 32) as u64) == doc {
            move_count -= 1;
        }
        self.prefix_len = 0;
        self.reposition_prefix_to(move_count, doc);
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
    /// prefix and any entries after it below `next`. Rows in between are ones
    /// the filter rejected, so no cursor needs to stop there.
    fn skip_to(&mut self, next: u64) {
        self.jumps += 1;
        let mut end = self.prefix_len;
        while end < self.order.len() && ((self.order[end] >> 32) as u64) < next {
            self.heap_ops += 1;
            end += 1;
        }
        self.prefix_len = 0;
        self.reposition_prefix_to(end, next);
    }

    /// Resolves a cursor the round moved: its document, and the bound of the
    /// block it landed on (which also refreshes block_last_doc).
    #[inline(always)]
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
            self.prefix_len = 0;
            let Some(first) = self.order.first().copied() else {
                self.doc_id = None;
                return false;
            };
            let doc = (first >> 32) as u64;
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
            for &entry in &self.order {
                if ((entry >> 32) as u64) != doc {
                    break;
                }
                self.heap_ops += 1;
                self.prefix_len += 1;
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
    fn skip_blocks(&mut self, from: u64) -> u64 {
        let start = from + 1;
        let prefix_len = self.prefix_len;
        let up_to = self.order.get(prefix_len).map(|&e| (e >> 32) as u64);

        // (1) + (2) init: Reposition the block cursor only (phase 1) and collect
        // `(last, bound, cursor_idx)` for phase 2 in a single pass over `0..prefix_len`.
        let mut next_doc = start;
        self.scratch_blocks.clear();
        for k in 0..prefix_len {
            let i = (self.order[k] & 0xffff_ffff) as usize;
            if !self.cursors[i].cur.is_some_and(|cur| cur >= start)
                && !self.cursors[i]
                    .block_last_at_or_after()
                    .is_some_and(|last| last >= start)
                && !self.cursors[i].seek_shallow(start)
                && !self.cursors[i].done
            {
                self.cursors[i].seek(start);
                self.cursors[i].ensure();
                self.child_advances += 1;
            }
            let last = match self.cursors[i].block_last_at_or_after() {
                Some(last) => last,
                None => continue,
            };
            if last < next_doc {
                continue;
            }
            let bound = match self.cursors[i].block_max_shallow_or(self.mode) {
                Some(bound) => bound,
                None => self.cursors[i].block_max(self.mode),
            };
            self.scratch_blocks.push((last, bound, i));
        }

        // (2) Repeat "the current block range cannot compete" and push every
        // cursor whose block ends at `min_last` forward by one block.
        let eps_f64 = f64::from(f32::EPSILON);
        let thresh_f64 = f64::from(self.threshold);
        loop {
            let mut sum_range: f64 = 0.0;
            let mut live_range = 0usize;
            let mut min_last = u64::MAX;
            for &(last, bound, _) in &self.scratch_blocks {
                if last < next_doc {
                    continue;
                }
                sum_range += f64::from(bound);
                live_range += 1;
                if last < min_last {
                    min_last = last;
                }
            }
            if live_range == 0 {
                break;
            }
            // '>' like the reference: a range that can only tie cannot produce a
            // document that beats the k-th (ties never win on a larger doc id).
            //
            // Widened by `n * EPSILON` for the same reason as the termination
            // bar: this sum is in document order, the score it protects is
            // summed in creation order, and skipping a range whose real score
            // clears the threshold drops the documents in it. The bounds here
            // are exact `f32` values, so a `f64` sum plus the accumulation
            // allowance covers every order.
            let widened = sum_range * (1.0 + live_range as f64 * eps_f64);
            if widened > thresh_f64 {
                break;
            }
            // The reference advances every child whose block ends at
            // `min_block_last`, not just the one picked as `earliest`.
            let clamp_to_up_to = up_to.is_some_and(|up_to| min_last >= up_to);
            if clamp_to_up_to {
                // The range runs to the heap top, so everything below it can be
                // crossed.
                next_doc = next_doc.max(up_to.expect("checked above"));
                break;
            }
            let skip_to = min_last + 1;
            if skip_to <= next_doc {
                break;
            }
            let mut moved = false;
            for sb_idx in 0..self.scratch_blocks.len() {
                if self.scratch_blocks[sb_idx].0 != min_last {
                    continue;
                }
                let j = self.scratch_blocks[sb_idx].2;
                if !self.cursors[j].seek_shallow(skip_to) && !self.cursors[j].done {
                    self.cursors[j].seek(skip_to);
                    self.cursors[j].ensure();
                    self.child_advances += 1;
                }
                if let Some(last) = self.cursors[j].block_last_at_or_after() {
                    let bound = match self.cursors[j].block_max_shallow_or(self.mode) {
                        Some(bound) => bound,
                        None => self.cursors[j].block_max(self.mode),
                    };
                    self.scratch_blocks[sb_idx].0 = last;
                    self.scratch_blocks[sb_idx].1 = bound;
                } else {
                    self.scratch_blocks[sb_idx].0 = 0;
                    self.scratch_blocks[sb_idx].1 = 0.0;
                }
                moved = true;
            }
            if !moved {
                break;
            }
            next_doc = skip_to;
        }

        next_doc
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
        self.prefix.clear();
        for &entry in &self.order[..self.prefix_len] {
            self.prefix.push((entry & 0xffff_ffff) as usize);
        }
        self.prefix.sort_unstable();
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
        let avg = (total_len as f64 / n as f64) as f32;
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

    /// The pruning ledger must bound the score the iterator will actually
    /// produce, which is accumulated in `f32`.
    ///
    /// Summing the exact `f32` bounds in `f64` keeps each addend exact, but the
    /// score is still summed in `f32` (`sum_score` in the reference), and a
    /// chain of `f32` additions can end up *above* the exact sum. Six boolean
    /// fields with weights `[1, s, s, s, s, s]` (`s = 3 * 2^-25`) sum to
    /// `1 + 3.75 * 2^-23` exactly while the `f32` accumulation rounds up to
    /// `1 + 5 * 2^-23`; a threshold in between then stops the search on a
    /// document whose real score clears it.
    #[test]
    fn the_ledger_bounds_the_f32_score_it_is_compared_against() {
        let doc_len: Vec<(u64, u32)> = vec![(0, 10)];
        let dead = common::RowBitmap::new();
        let s = 3.0f32 * 2f32.powi(-25);
        let weights = [1.0f32, s, s, s, s, s];
        let threshold = 1.0f32 + 4.0 * 2f32.powi(-23);
        // All six fields match document 0.
        let t: &[(u64, u32)] = &[(0, 1)];
        let make = || {
            weights
                .iter()
                .map(|w| field(&[("t", t)], &doc_len, 1, vec![*w]))
                .collect::<Vec<_>>()
        };
        let pruned = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(threshold)
            .run();
        let naive = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(threshold)
            .with_naive()
            .run();
        assert_eq!(
            pruned, naive,
            "the ledger stopped a document whose f32 score clears the threshold"
        );
        assert_eq!(pruned.len(), 1, "document 0 qualifies");
    }

    /// Retiring a cursor must not lose a bound that `f64` cannot represent
    /// beside the retired one.
    ///
    /// The ledger is maintained by subtracting the retired cursor's bound, and
    /// even `f64` rounds: `2^53 + 1` is `2^53`, so removing the `2^53` bound
    /// leaves `0` where the other cursor still contributes `1`. A `BOOST` far
    /// from 1 is what makes the large bound reachable.
    #[test]
    fn retiring_a_huge_cursor_keeps_the_remaining_bound() {
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
                field(&[("a", a)], &doc_len, 2, vec![2f32.powi(53)]),
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
            "the ledger lost the small bound when the huge cursor retired"
        );
        assert_eq!(pruned.len(), 1, "row 1 must survive");
        assert_eq!(pruned[0].0, 1);
    }

    /// The pivot sum has to be as conservative as the score it stands in for.
    ///
    /// `score_prefix` sums the prefix in *creation* order, but the pivot loop
    /// sums the cursors the heap hands it, which is *document* order, and it
    /// stops at the first partial sum that reaches the bar. Two orders of the
    /// same addends give different `f32` totals, so a prefix the pivot never
    /// completes can still score above the threshold when the document is
    /// finally scored - and then no pivot is found and the document is lost.
    ///
    /// Eight boolean fields, weights `[1, s, s, s, s, s, s, s]` with
    /// `s = 3 * 2^-25`: the first field matches only row 1, the rest match rows
    /// 0 and 1, row 0 is deleted. The document-order partial sum reaches only
    /// `1 + 5 * 2^-23` where creation order gives `1 + 7 * 2^-23`, so a bar of
    /// `1 + 6 * 2^-23` is cleared by the score and missed by the pivot.
    #[test]
    fn the_pivot_bound_covers_the_scoring_order() {
        let doc_len: Vec<(u64, u32)> = vec![(0, 10), (1, 10)];
        let dead = {
            let mut d = common::RowBitmap::new();
            d.insert(0);
            d
        };
        let s = 3.0f32 * 2f32.powi(-25);
        // Field 0 (weight 1) hits only row 1; the rest (weight s) hit both rows.
        let only_1: &[(u64, u32)] = &[(1, 1)];
        let both: &[(u64, u32)] = &[(0, 1), (1, 1)];
        let make = || {
            let mut fields = vec![field(&[("t", only_1)], &doc_len, 2, vec![1.0])];
            for _ in 0..7 {
                fields.push(field(&[("t", both)], &doc_len, 2, vec![s]));
            }
            fields
        };
        let threshold = 1.0f32 + 6.0 * 2f32.powi(-23);
        let pruned = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(threshold)
            .run();
        let naive = BlockMaxWandIterator::new(make(), 5, &dead, 0.0, 1, WandMode::Boolean)
            .with_threshold(threshold)
            .with_naive()
            .run();
        assert_eq!(
            pruned, naive,
            "the pivot sum missed a document the score clears the bar for"
        );
        assert_eq!(pruned.len(), 1, "row 1 qualifies");
        assert_eq!(pruned[0].0, 1);
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

    /// An overflowing `f3` must not collapse the block bound to zero.
    ///
    /// `bm25_param_k1` is only checked for being non-negative, so `2e35` is
    /// accepted; with `b = 0.75` and `avg = 20` the `k1*b/avg*u16::MAX` that
    /// becomes `f3` overflows to infinity, `f3/pct` is infinite, and the norm
    /// `1/(1 + ...)` is `0`. A block bound of zero prunes every document in it.
    #[test]
    fn an_overflowing_coefficient_falls_back_to_the_term_bound() {
        let ranker = Bm25Ranker::with_params(2e35, 0.75, 0.0);
        let idf = ranker.idf(2.0, 2.0);
        let avg = 20.0f32;
        // The block's statistics: one occurrence, the corpus average length.
        let f3 = ranker.f3(avg);
        assert!(
            !f3.is_finite(),
            "f3 must overflow for this test to mean anything"
        );
        let postings: Vec<(u64, u32)> = vec![(0, 1), (1, 10)];
        let doc_len: Vec<(u64, u32)> = vec![(0, 20), (1, 20)];
        let field = field(&[("t", &postings)], &doc_len, 2, vec![1.0]);
        let dead = common::RowBitmap::new();
        let mut it = BlockMaxWandIterator::new(vec![field], 1, &dead, 0.0, 1, WandMode::Bm25)
            .with_bm25_params(2e35, 0.75);
        let cursor = &mut it.cursors[0];
        let id = cursor.stream.block_id();
        cursor.block_index = id;
        cursor.block_stats_cache = (1, 1.0, 65535);
        let bound = cursor.block_max(WandMode::Bm25);
        assert!(bound > 0.0, "the block bound collapsed to {bound}");
        assert!(
            bound >= cursor.upper_bound * 0.5,
            "the fallback is the whole-term bound, not a fraction of it"
        );
        let _ = idf;
    }

    /// The whole-term bound must cover the term's score, which `f32` rounding
    /// makes false for the plain formula.
    ///
    /// `upper_bound` multiplies `idf * (k1 + 1 + delta) * weight`; `score_with`
    /// multiplies `common * (tf/(tf+p) + f4)`. With `df = 1`, `n = 1000`,
    /// `delta = 3`, `tf = dl = avg = 2^25` and weight `0.6997575163841248` the
    /// score is `23.663778305` and the plain bound `23.663772583`, so pruning
    /// would stop on a document that clears a threshold of `23.663776398`.
    #[test]
    fn the_term_bound_covers_the_term_score() {
        let ranker = Bm25Ranker::new(3.0);
        let (n, df) = (1000.0f32, 1.0f32);
        let idf = ranker.idf(n, df);
        let tf = 2f32.powi(25);
        for w in [0.699_757_5_f32, 0.1, 1.0] {
            let score = ranker.score(tf, tf, tf, idf, w);
            let bound = ranker.upper_bound(idf, w);
            assert!(score <= bound, "w={w}: score {score} > upper bound {bound}");
        }
    }

    /// `BlockMaxBM25Score` must not fall below the exact score of the block it
    /// describes - through the production path, cap included.
    ///
    /// The bound evaluates a different expression than `score_with`
    /// (`1/(1 + f1/max_tf + f3/pct)` against `tf/(tf + f1 + f2*dl)`), so the two
    /// round differently: at `tf = dl = avg = 1216` with a full percentage the
    /// exact score is `1.4293134` and the raw bound `1.4293131`, three ulps
    /// below. A document whose score just clears the threshold would then be
    /// pruned as uncompetitive.
    ///
    /// The `2^24` rows are the ones where the raw bound passes `upper_bound` by
    /// a rounding step and `.min` binds - without widening the cap, the margin
    /// is removed there and the bound drops back below the score.
    ///
    /// `pct = 65535` claims the block's longest document is the corpus average,
    /// which only bounds the block when `dl >= avg`; that is the point sampled.
    #[test]
    fn block_maximum_never_falls_below_the_exact_score() {
        // `delta = 100` is where the raw bound passes `upper_bound` by more
        // than a rounding step, so `.min` genuinely binds there.
        for delta in [0.0f32, 0.5, 3.0, 100.0] {
            let ranker = Bm25Ranker::new(delta);
            let idf = ranker.idf(1000.0, 1.0);
            for (avg, tf) in [(1216.0f32, 1216.0f32), (16_777_216.0, 1_048_576.0)] {
                for w in [0.1f32, 0.7, 1.7] {
                    let postings: Vec<(u64, u32)> = vec![(0, tf as u32)];
                    let doc_len: Vec<(u64, u32)> = vec![(0, tf as u32)];
                    let mut field = field(&[("t", &postings)], &doc_len, 1, vec![w]);
                    field.total_len = (avg * 1000.0) as u64;
                    field.n_docs = 1000;
                    if let Some(unit) = field.units.first_mut() {
                        unit.idf = idf;
                    }
                    let dead = common::RowBitmap::new();
                    let mut it =
                        BlockMaxWandIterator::new(vec![field], 1, &dead, delta, 1, WandMode::Bm25);
                    let cursor = &mut it.cursors[0];
                    // Pin the block statistics the bound reads: the largest tf
                    // the block holds, and a full percentage (the block's
                    // longest document is the corpus average).
                    let id = cursor.stream.block_id();
                    cursor.block_index = id;
                    cursor.block_stats_cache = (0, tf, 65535);
                    let exact = ranker.score(tf, tf, avg, idf, w);
                    let bound = cursor.block_max(WandMode::Bm25);
                    assert!(
                        exact <= bound,
                        "delta={delta} avg={avg} w={w}: exact {exact} > block_max {bound}"
                    );
                }
            }
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
        let avg = ((total_len as f64 / n.max(1) as f64) as f32).max(1e-9);
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

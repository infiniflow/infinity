// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module;

#include <cassert>
#include <cstdio>
#include <cstdlib>

module infinity_core:heap_blockmax_wand_iterator.impl;

import :heap_blockmax_wand_iterator;
import :index_defines;
import :blockmax_leaf_iterator;
import :multi_doc_iterator;
import :logger;
import :infinity_exception;

import third_party;
import internal_types;

namespace infinity {

namespace {
std::atomic<HeapBlockMaxWandIterator::AdvanceMode> g_advance_mode{HeapBlockMaxWandIterator::AdvanceMode::kThresholdSkip};

// Diagnostic switch: disables block skipping (falls back to one-doc-at-a-time), used to pin a recall
// miss on the block-skip path.
bool DisableBlockSkip() {
    static const bool disabled = std::getenv("INFINITY_BMW_IT_NOSKIP") != nullptr;
    return disabled;
}

// Targeted tracing: when a block skip also skips doc == INFINITY_BMW_IT_TRACE_DOC, dumps the whole
// state used for that non-competitiveness proof to stderr so the block bounds can be checked by hand.
RowID TraceTargetDoc() {
    static const RowID target = [] {
        const char *raw = std::getenv("INFINITY_BMW_IT_TRACE_DOC");
        if (raw == nullptr || *raw == '\0') {
            return INVALID_ROWID;
        }
        return RowID(static_cast<u64>(std::strtoull(raw, nullptr, 10)));
    }();
    return target;
}
} // namespace

void HeapBlockMaxWandIterator::SetAdvanceMode(AdvanceMode mode) { g_advance_mode.store(mode, std::memory_order_relaxed); }

HeapBlockMaxWandIterator::AdvanceMode HeapBlockMaxWandIterator::GetAdvanceMode() { return g_advance_mode.load(std::memory_order_relaxed); }

void HeapBlockMaxWandIterator::AdvanceChild(u32 idx, RowID target) {
    if (GetAdvanceMode() == AdvanceMode::kPlainSeek) {
        return static_cast<void>(ctx_[idx]->NextNoSkip(target));
    }
    return static_cast<void>(ctx_[idx]->Next(target));
}

HeapBlockMaxWandIterator::HeapBlockMaxWandIterator(std::vector<std::unique_ptr<DocIterator>> &&children) : MultiDocIterator(std::move(children)) {
    const size_t num_children = children_.size();
    ctx_.reserve(num_children);
    bm25_score_upper_bound_ = 0.0f;
    estimate_iterate_cost_ = {};
    for (size_t i = 0; i < num_children; ++i) {
        auto *child = dynamic_cast<BlockMaxLeafIterator *>(children_[i].get());
        if (child == nullptr) {
            UnrecoverableError("HeapBlockMaxWandIterator only supports BlockMaxLeafIterator");
        }
        ctx_.push_back(child);
        // bm25_score_upper_bound_ is recomputed over the live children in Init(); the full sum here
        // keeps UpdateScoreThreshold safe when it is called before Next().
        bm25_score_upper_bound_ += child->BM25ScoreUpperBound();
        estimate_iterate_cost_ += child->GetEstimateIterateCost();
    }
    retired_.assign(num_children, 0);
    head_.reserve(num_children);
    prefix_.reserve(num_children);
}

void HeapBlockMaxWandIterator::Init() {
    head_.clear();
    prefix_.clear();
    std::fill(retired_.begin(), retired_.end(), 0);
    score_cached_ = false;
    score_cache_ = 0.0f;
    bm25_score_upper_bound_ = 0.0f;
    for (u32 i = 0; i < ctx_.size(); ++i) {
        if (ctx_[i]->Next(0)) {
            head_.push_back(i);
            bm25_score_upper_bound_ += ctx_[i]->BM25ScoreUpperBound();
        } else {
            retired_[i] = 1;
        }
    }
    std::make_heap(head_.begin(), head_.end(), [this](u32 lhs, u32 rhs) { return HeadLess(lhs, rhs); });
}

bool HeapBlockMaxWandIterator::Finish() {
    doc_id_ = INVALID_ROWID;
    prefix_.clear();
    score_cached_ = false;
    score_cache_ = 0.0f;
    return false;
}

void HeapBlockMaxWandIterator::Retire(u32 idx) {
    if (retired_[idx] != 0) {
        return;
    }
    retired_[idx] = 1;
    ++stats_.retired_children_;
    bm25_score_upper_bound_ -= ctx_[idx]->BM25ScoreUpperBound();
    if (bm25_score_upper_bound_ < 0.0f) {
        // Guard against float drift on repeated subtraction: a negative bound would terminate early.
        bm25_score_upper_bound_ = 0.0f;
    }
}

void HeapBlockMaxWandIterator::PushHead(u32 idx) {
    ++stats_.heap_ops_;
    head_.push_back(idx);
    std::push_heap(head_.begin(), head_.end(), [this](u32 lhs, u32 rhs) { return HeadLess(lhs, rhs); });
}

u32 HeapBlockMaxWandIterator::PopHead() {
    ++stats_.heap_ops_;
    std::pop_heap(head_.begin(), head_.end(), [this](u32 lhs, u32 rhs) { return HeadLess(lhs, rhs); });
    const u32 idx = head_.back();
    head_.pop_back();
    return idx;
}

void HeapBlockMaxWandIterator::PushBack(u32 idx, RowID target) {
    if (retired_[idx] != 0) {
        return;
    }
    ++stats_.child_advances_;
    AdvanceChild(idx, target);
    if (ctx_[idx]->DocID() == INVALID_ROWID) {
        Retire(idx);
        return;
    }
    PushHead(idx);
}

void HeapBlockMaxWandIterator::AdvanceWithin(u32 idx, RowID target) {
    if (retired_[idx] != 0) {
        return;
    }
    // "doc_id_ >= target" is not enough: after NextShallow() the block cursor has moved past the
    // stored doc without re-decoding it, so the iterator is stale and must be re-seeked before it can
    // be scored (see BlockMaxLeafIterator::IsCurrentDocDecoded).
    if (ctx_[idx]->DocID() >= target && ctx_[idx]->IsCurrentDocDecoded()) {
        return;
    }
    ++stats_.child_advances_;
    AdvanceChild(idx, target);
    if (ctx_[idx]->DocID() == INVALID_ROWID) {
        Retire(idx);
    }
}

void HeapBlockMaxWandIterator::RestorePrefix() {
    for (const u32 idx : prefix_) {
        if (retired_[idx] == 0) {
            PushHead(idx);
        }
    }
    prefix_.clear();
}

void HeapBlockMaxWandIterator::SkipUncompetitiveRange(RowID from_doc) {
    const RowID start = from_doc + 1;
    const RowID up_to = head_.empty() ? INVALID_ROWID : ctx_[head_.front()]->DocID();
    const RowID trace_doc = TraceTargetDoc();

    // Phase 1: only reposition the block cursor to the block covering start (NextShallow decodes the
    // skip list only; it does not touch doc_id_).
    //
    // Careful: BlockMinPossibleDocID() must NOT be used to advance next_doc. "child i has no posting
    // in [start, block_min)" only means that child contributes 0; other children may still hit docs in
    // the range (typical case: the last posting of a term in segment 0 is at 3469 and its next block
    // starts at segment 1, so block_min = 2^32 -- skipping the tail of segment 0 that way loses
    // documents). "The whole range is non-competitive" can only be proven by the prefix block-bound
    // sum in phase 2.
    RowID next_doc = start;
    for (const u32 idx : prefix_) {
        if (retired_[idx] != 0) {
            continue;
        }
        if (ctx_[idx]->BlockLastDocID() >= start) {
            continue;
        }
        ++stats_.block_seeks_;
        if (!ctx_[idx]->NextShallow(start)) {
            Retire(idx);
            continue;
        }
    }

    // Phase 2: repeatedly prove "the current block range cannot compete" and push the child with the
    // earliest block end forward by one block.
    //
    // Upper bound of [next_doc, min_block_last] = sum of block_max_i over prefix children covering
    // next_doc:
    //   * the range lies inside every counted child's current block (min_block_last <= block_last_i),
    //     so the block bounds are valid;
    //   * children with block_last_i < next_doc contribute 0 on the range and are not counted;
    //   * when the range end is clamped by up_to (heap top), only prefix children can hit docs in it.
    for (;;) {
        float sum_range = 0.0f;
        RowID min_block_last = INVALID_ROWID;
        for (const u32 idx : prefix_) {
            if (retired_[idx] != 0) {
                continue;
            }
            BlockMaxLeafIterator *child = ctx_[idx];
            const RowID block_last = child->BlockLastDocID();
            if (block_last == INVALID_ROWID || block_last < next_doc) {
                continue;
            }
            sum_range += child->BlockMaxBM25Score();
            if (min_block_last == INVALID_ROWID || block_last < min_block_last) {
                min_block_last = block_last;
            }
        }
        if (min_block_last == INVALID_ROWID) {
            // No prefix child's current block covers anything past next_doc: let the main loop
            // reposition. next_doc must not become INVALID_ROWID here, otherwise phase 3 would advance
            // every child once and cross a range that was never proven.
            break;
        }
        if (sum_range > threshold_) {
            // The range may still contain a competitive doc: stop at next_doc and let the main loop
            // handle docs one by one.
            break;
        }
        // Range actually skipped: [next_doc, up_to) on the up_to branch, otherwise [next_doc, min_block_last].
        const bool clamp_to_up_to = (up_to != INVALID_ROWID && min_block_last >= up_to);
        const bool trace_hit = trace_doc != INVALID_ROWID && trace_doc > from_doc && trace_doc >= next_doc &&
                               (clamp_to_up_to ? (trace_doc < up_to) : (trace_doc <= min_block_last));
        if (trace_hit) {
            std::fprintf(stderr,
                         "[BMW-SKIP] stage=2 from=%s start=%s next=%s up_to=%s min_block_last=%s sum_range=%.6f thr=%.6f "
                         "ub_sum=%.6f prefix=%zu\n",
                         from_doc.ToString().c_str(),
                         start.ToString().c_str(),
                         next_doc.ToString().c_str(),
                         up_to.ToString().c_str(),
                         min_block_last.ToString().c_str(),
                         sum_range,
                         threshold_,
                         bm25_score_upper_bound_,
                         prefix_.size());
            for (const u32 idx : prefix_) {
                if (retired_[idx] != 0) {
                    std::fprintf(stderr, "[BMW-SKIP]   child=%u retired\n", idx);
                    continue;
                }
                BlockMaxLeafIterator *child = ctx_[idx];
                std::fprintf(stderr,
                             "[BMW-SKIP]   child=%u doc=%s block_min=%s block_last=%s block_max=%.6f ub=%.6f local_thr=%.6f\n",
                             idx,
                             child->DocID().ToString().c_str(),
                             child->BlockMinPossibleDocID().ToString().c_str(),
                             child->BlockLastDocID().ToString().c_str(),
                             child->BlockMaxBM25Score(),
                             child->BM25ScoreUpperBound(),
                             child->Threshold());
            }
        }
        if (clamp_to_up_to) {
            // Passed the heap top (children outside the prefix start participating here), so the whole
            // range up to up_to can be skipped.
            if (up_to > next_doc) {
                next_doc = up_to;
            }
            break;
        }
        next_doc = min_block_last + 1;
        bool moved = false;
        for (const u32 idx : prefix_) {
            if (retired_[idx] != 0) {
                continue;
            }
            if (ctx_[idx]->BlockLastDocID() == min_block_last) {
                ++stats_.block_seeks_;
                if (!ctx_[idx]->NextShallow(next_doc)) {
                    Retire(idx);
                }
                moved = true;
            }
        }
        if (!moved) {
            break;
        }
    }

    // Phase 3: actually position at next_doc and restore doc_id_ (the only step that decodes tf).
    // next_doc is always valid (starts at start and only moves forward in place), so this goes through
    // AdvanceWithin.
    for (const u32 idx : prefix_) {
        if (retired_[idx] != 0) {
            continue;
        }
        AdvanceWithin(idx, next_doc);
    }
    std::erase_if(prefix_, [this](u32 idx) { return retired_[idx] != 0; });
}

void HeapBlockMaxWandIterator::SkipUncompetitiveRangeOrAdvanceOne(RowID from_doc) {
    if (!DisableBlockSkip()) {
        SkipUncompetitiveRange(from_doc);
        return;
    }
    // Block skipping disabled: each prefix child advances one doc, matching a plain doc-by-doc scan.
    for (const u32 idx : prefix_) {
        AdvanceWithin(idx, from_doc + 1);
    }
}

bool HeapBlockMaxWandIterator::Next(RowID target) {
    assert(target != INVALID_ROWID);

    if (doc_id_ == INVALID_ROWID) {
        // Init() resets the score cache itself.
        Init();
    } else {
        if (doc_id_ >= target) {
            // The iterator does not move: doc_id_ (and therefore score_cache_) still describes the
            // current document. Keep the cache, otherwise a caller that re-asks for the current doc
            // (ScoreThresholdIterator-style pull loops, or a Next(t) that does not advance) would see
            // Score() == 0 for a doc that was already scored.
            return true;
        }
        // About to leave doc_id_: the cached score no longer describes the current document.
        score_cached_ = false;
        // On the previous successful return every prefix child was positioned on doc_id_; push them to
        // >= target.
        for (const u32 idx : prefix_) {
            PushBack(idx, target);
        }
        prefix_.clear();
    }
    if (head_.empty()) {
        return Finish();
    }
    // Children still below target: advance them to target (PushBack re-heapifies or retires them).
    while (!head_.empty() && ctx_[head_.front()]->DocID() < target) {
        PushBack(PopHead(), target);
    }

    for (;;) {
        ++stats_.iterations_;
        if (head_.empty()) {
            return Finish();
        }
        // Even the sum of all live upper bounds cannot reach the threshold: no competitive doc is left.
        if (bm25_score_upper_bound_ <= threshold_) {
            return Finish();
        }

        // ------------------------------------------------------------------
        // (1) pivot: pop the heap in doc order, accumulating global upper bounds until the prefix sum
        //     exceeds the threshold. Same pivot definition as the sort-based version, just without
        //     sorting the whole array first.
        // ------------------------------------------------------------------
        prefix_.clear();
        float prefix_upper_bound = 0.0f;
        while (!head_.empty()) {
            const u32 idx = PopHead();
            prefix_.push_back(idx);
            prefix_upper_bound += ctx_[idx]->BM25ScoreUpperBound();
            if (prefix_upper_bound > threshold_) {
                break;
            }
        }
        assert(!prefix_.empty());
        const RowID pivot_doc = ctx_[prefix_.back()]->DocID();
        if (pivot_doc == INVALID_ROWID) [[unlikely]] {
            // Cannot happen in theory (an exhausted child is retired immediately); defensive cleanup.
            RestorePrefix();
            return Finish();
        }
        // Children tied with the pivot doc (they must still be at the heap top) join the prefix.
        while (!head_.empty() && ctx_[head_.front()]->DocID() == pivot_doc) {
            prefix_.push_back(PopHead());
        }

        // ------------------------------------------------------------------
        // (2) Upper bound sum_at_doc of the candidate pivot_doc.
        //     Never call NextShallow on a child already positioned on pivot_doc: it only moves the
        //     block cursor without updating doc_id_, and Score() would then read a tf=0 mis-score or the
        //     cached score of the previous doc.
        // ------------------------------------------------------------------
        float sum_at_doc = 0.0f;
        bool any_retired = false;
        for (const u32 idx : prefix_) {
            BlockMaxLeafIterator *child = ctx_[idx];
            if (child->DocID() == pivot_doc) {
                sum_at_doc += child->BlockMaxBM25Score();
            } else if (child->BlockLastDocID() >= pivot_doc) {
                // The candidate lies in its current block: the block bound is the candidate's bound.
                sum_at_doc += child->BlockMaxBM25Score();
            } else {
                // The candidate lies beyond the current block: a block-level seek is needed, which the
                // statistics count regardless of whether the range-skipping path is enabled.
                ++stats_.block_seeks_;
                if (!child->NextShallow(pivot_doc)) {
                    Retire(idx);
                    any_retired = true;
                } else if (child->BlockMinPossibleDocID() > pivot_doc) {
                    // The block cursor was skipped past the candidate: this child contributes nothing there.
                } else {
                    sum_at_doc += child->BlockMaxBM25Score();
                }
            }
        }
        if (any_retired) {
            std::erase_if(prefix_, [this](u32 idx) { return retired_[idx] != 0; });
            if (prefix_.empty()) {
                continue;
            }
        }

        // All prefix children must sit on pivot_doc *and* be decoded there. Checking only the front
        // would rely on the implicit "prefix_ is doc-ascending" invariant; if that ever breaks (or a
        // child is left stale by a block-level seek), scoring would read a mismatched posting block.
        // This is exactly the failure mode of 473f4fa0, so verify it explicitly every round.
        bool all_at_pivot_doc = true;
        for (const u32 idx : prefix_) {
            if (ctx_[idx]->DocID() != pivot_doc || !ctx_[idx]->IsCurrentDocDecoded()) {
                all_at_pivot_doc = false;
                break;
            }
        }

        // ------------------------------------------------------------------
        // (3) Decision
        // ------------------------------------------------------------------
        if (sum_at_doc > threshold_) {
            if (all_at_pivot_doc) {
                // Every prefix child is positioned on pivot_doc: compute the exact score.
                ++stats_.docs_scored_;
                float score = 0.0f;
                u32 match_count = 0;
                for (const u32 idx : prefix_) {
                    score += ctx_[idx]->Score();
                    match_count += ctx_[idx]->MatchCount();
                }
                // Passing the upper bound is necessary only; the exact score must also beat the threshold.
                if (score > threshold_ && (minimum_should_match_hint_ == 0 || match_count >= minimum_should_match_hint_)) {
                    doc_id_ = pivot_doc;
                    score_cache_ = score;
                    score_cached_ = true;
                    return true;
                }
                // Scoring failed: pivot_doc is not competitive, so skip blocks instead of advancing one doc.
                SkipUncompetitiveRangeOrAdvanceOne(pivot_doc);
                RestorePrefix();
                continue;
            }
            // Some child still sits before pivot_doc: advance to pivot_doc and recompute the pivot next round.
            for (const u32 idx : prefix_) {
                AdvanceWithin(idx, pivot_doc);
            }
            RestorePrefix();
            continue;
        }

        // sum_at_doc <= threshold: pivot_doc itself cannot compete (same for the range).
        // Hand it to the block skip, which jumps as many blocks as possible with NextShallow and only
        // falls back to one-doc steps when individual docs really have to be examined.
        SkipUncompetitiveRangeOrAdvanceOne(pivot_doc);
        RestorePrefix();
    }
}

float HeapBlockMaxWandIterator::Score() { return score_cached_ ? score_cache_ : 0.0f; }

void HeapBlockMaxWandIterator::UpdateScoreThreshold(float threshold) {
    if (threshold <= threshold_) {
        return;
    }
    threshold_ = threshold;
    // Each child gets the tightened local threshold: threshold - (upper bound sum of the others).
    const float base_threshold = threshold - bm25_score_upper_bound_;
    for (u32 i = 0; i < ctx_.size(); ++i) {
        if (retired_[i] != 0) {
            continue;
        }
        ctx_[i]->UpdateScoreThreshold(base_threshold + ctx_[i]->BM25ScoreUpperBound());
    }
}

u32 HeapBlockMaxWandIterator::MatchCount() const {
    if (doc_id_ == INVALID_ROWID) {
        return 0;
    }
    u32 count = 0;
    for (const u32 idx : prefix_) {
        if (retired_[idx] == 0 && ctx_[idx]->DocID() == doc_id_) {
            count += ctx_[idx]->MatchCount();
        }
    }
    return count;
}

void HeapBlockMaxWandIterator::SetMinimumShouldMatchHint(u32 minimum_should_match) { minimum_should_match_hint_ = minimum_should_match; }

void HeapBlockMaxWandIterator::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "HeapBlockMaxWandIterator";
    os << " (threshold: " << threshold_ << ")";
    os << " (bm25_score_upper_bound: " << bm25_score_upper_bound_ << ")";
    os << " (doc_id: " << doc_id_.ToUint64() << ")";
    os << '\n';
    for (size_t i = 0; i < children_.size(); ++i) {
        children_[i]->PrintTree(os, prefix + (is_final ? "    " : "│   "), i + 1 == children_.size());
    }
}

} // namespace infinity

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

#include <algorithm>
#include <cassert>
#include <numeric>

module infinity_core:block_at_a_time_iterator.impl;

import :block_at_a_time_iterator;
import :index_defines;
import :doc_iterator;
import :blockmax_leaf_iterator;
import :term_doc_iterator;
import :multi_doc_iterator;
import :logger;
import :infinity_exception;
import :simd_functions;

import third_party;
import internal_types;

namespace infinity {

static constexpr u32 BAAT_BATCH_SIZE = MAX_DOC_PER_RECORD; // 128, already 8-aligned for AVX2

BlockAtATimeIterator::BlockAtATimeIterator(std::vector<std::unique_ptr<DocIterator>> &&iterators, const u32 topn)
    : MultiDocIterator(std::move(iterators)), topn_(topn) {
    bm25_score_upper_bound_ = 0.0f;
    estimate_iterate_cost_ = {};
    term_params_.reserve(children_.size());
    bool needs_simd_buffer = false;
    for (auto &child : children_) {
        auto *leaf = dynamic_cast<BlockMaxLeafIterator *>(child.get());
        if (leaf == nullptr) {
            UnrecoverableError("BlockAtATimeIterator only supports BlockMaxLeafIterator");
        }
        TermBM25Params params;
        params.leaf_iter = leaf;
        params.score_upper_bound = leaf->BM25ScoreUpperBound();
        bm25_score_upper_bound_ += params.score_upper_bound;
        estimate_iterate_cost_ += leaf->GetEstimateIterateCost();
        if (auto *tdi = dynamic_cast<TermDocIterator *>(leaf)) {
            params.is_term_doc = true;
            needs_simd_buffer = true;
            std::tie(params.f1, params.f2, params.bm25_common_score) = tdi->Get_f1_f2_bm25_common_score();
        } else {
            params.f1 = 0.0f;
            params.f2 = 0.0f;
            params.bm25_common_score = leaf->BM25ScoreUpperBound();
        }
        term_params_.emplace_back(std::move(params));
    }
    if (needs_simd_buffer) {
        AllocateBatchBuffers();
    }
    SortTermsByIDF();
    topk_results_.reserve(topn_);
}

BlockAtATimeIterator::~BlockAtATimeIterator() {
    std::free(aligned_buffer_);
    if (SHOULD_LOG_TRACE()) {
        LOG_TRACE(fmt::format("BlockAtATimeIterator: docs={} blk_skip={} blk_dec={} thr={}",
                              total_docs_processed_,
                              total_blocks_skipped_,
                              total_blocks_decoded_,
                              threshold_));
    }
}

void BlockAtATimeIterator::AllocateBatchBuffers() {
    const u32 buf_size = BAAT_BATCH_SIZE * (sizeof(u32) + sizeof(u32) + sizeof(u32) + sizeof(f32));
    aligned_buffer_ = std::aligned_alloc(64, buf_size);
    if (!aligned_buffer_) {
        UnrecoverableError(fmt::format("{}: Out of memory!", __func__));
    }
    auto *base = static_cast<u8 *>(aligned_buffer_);
    batch_tf_ = reinterpret_cast<u32 *>(base);
    base += BAAT_BATCH_SIZE * sizeof(u32);
    batch_doc_len_ = reinterpret_cast<u32 *>(base);
    base += BAAT_BATCH_SIZE * sizeof(u32);
    batch_match_cnt_ = reinterpret_cast<u32 *>(base);
    base += BAAT_BATCH_SIZE * sizeof(u32);
    batch_score_sum_ = reinterpret_cast<f32 *>(base);
}

void BlockAtATimeIterator::SortTermsByIDF() {
    sorted_term_order_.resize(term_params_.size());
    std::iota(sorted_term_order_.begin(), sorted_term_order_.end(), 0u);
    // Descending score_upper_bound → rare terms first
    std::sort(sorted_term_order_.begin(), sorted_term_order_.end(), [this](u32 a, u32 b) {
        return term_params_[a].score_upper_bound > term_params_[b].score_upper_bound;
    });
}

void BlockAtATimeIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold > threshold_) {
        threshold_ = threshold;
        for (auto &p : term_params_) {
            p.leaf_iter->UpdateScoreThreshold(threshold);
        }
    }
}

// Rebuild running top-k heap from current accumulator, then update threshold_.
// Called after each term so subsequent terms benefit from block-level pruning.
void BlockAtATimeIterator::UpdateThresholdFromAccumulator() {
    if (accumulator_.size() < topn_) {
        return;
    }
    // Clear and rebuild
    while (!running_topk_heap_.empty()) {
        running_topk_heap_.pop();
    }
    for (const auto &[doc_id, entry] : accumulator_) {
        if (entry.score < threshold_)
            continue;
        if (running_topk_heap_.size() < topn_) {
            running_topk_heap_.emplace(doc_id, entry.score);
        } else if (entry.score >= running_topk_heap_.top().second) {
            running_topk_heap_.pop();
            running_topk_heap_.emplace(doc_id, entry.score);
        }
    }
    if (running_topk_heap_.size() >= topn_) {
        threshold_ = running_topk_heap_.top().second;
    }
}

bool BlockAtATimeIterator::Next(RowID doc_id) {
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id) {
        score_cached_ = false;
        return true;
    }
    if (topk_results_.empty()) {
        // Run BAAT pipeline once
        while (!running_topk_heap_.empty()) {
            running_topk_heap_.pop();
        }

        for (auto &p : term_params_) {
            p.leaf_iter->Next(0);
        }

        // Process terms in IDF order; after each term, update threshold
        for (u32 sort_idx = 0; sort_idx < sorted_term_order_.size(); ++sort_idx) {
            ProcessTerm(sorted_term_order_[sort_idx]);
            // After processing a rare term, update threshold so subsequent
            // common terms can skip entire blocks.
            if (sort_idx < sorted_term_order_.size() - 1) {
                UpdateThresholdFromAccumulator();
            }
        }

        ExtractTopK();
        // Sort by doc_id ascending so linear cursor scan returns docs in increasing order.
        std::sort(topk_results_.begin(), topk_results_.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
        topk_cursor_ = 0;
    }
    while (topk_cursor_ < topk_results_.size()) {
        const auto &[cd, cs] = topk_results_[topk_cursor_];
        if (cd < doc_id) {
            ++topk_cursor_;
            continue;
        }
        ++topk_cursor_;
        if (cs >= threshold_) {
            doc_id_ = cd;
            bm25_score_cache_ = cs;
            score_cached_ = true;
            return true;
        }
    }
    doc_id_ = INVALID_ROWID;
    return false;
}

// ------------------------------------------------------------------
// ProcessTerm: process one term's entire posting list
// ------------------------------------------------------------------
void BlockAtATimeIterator::ProcessTerm(const u32 term_idx) {
    auto &params = term_params_[term_idx];
    auto *leaf = params.leaf_iter;
    auto *tdi = params.is_term_doc ? dynamic_cast<TermDocIterator *>(leaf) : nullptr;

    // Do not use the term-level threshold filter — we want ALL docs from this term
    // because even a low individual score adds to the accumulated total.
    leaf->UpdateScoreThreshold(0.0f);

    RowID target = 0;
    while (true) {
        // Step 1: Advance block cursor with NextShallow (skiplist only, no doc decode)
        if (!leaf->NextShallow(target)) {
            break;
        }
        RowID block_last = leaf->BlockLastDocID();
        RowID block_min = leaf->BlockMinPossibleDocID();
        if (block_min == INVALID_ROWID) {
            break;
        }

        // Step 2: Block-level pruning.
        // If this term's max possible contribution in this block cannot reach
        // the current top-k threshold, skip the entire block without decoding.
        if (threshold_ > 0.0f && leaf->BlockMaxBM25Score() <= threshold_) {
            ++total_blocks_skipped_;
            target = block_last + 1;
            continue;
        }
        ++total_blocks_decoded_;

        // Step 3: Decode first document in the block (also positions doc cursor)
        if (!leaf->Next(target)) {
            break;
        }

        if (tdi != nullptr && batch_tf_ != nullptr) {
            ProcessBlockSIMD(tdi, block_min, block_last, term_idx);
            target = block_last + 1;
        } else {
            // Scalar fallback (PhraseDocIterator etc.)
            RowID current = leaf->DocID();
            while (current <= block_last) {
                ++total_docs_processed_;
                const float score = leaf->Score();
                AccumulateDoc(current, score, 1);
                target = current + 1;
                if (!leaf->Next(target))
                    return;
                current = leaf->DocID();
            }
        }
    }
}

// ------------------------------------------------------------------
// ProcessBlockSIMD: decode one entire block with BatchDecodeTo,
// compute all scores with SIMD BatchBM25, then accumulate.
// ------------------------------------------------------------------
void BlockAtATimeIterator::ProcessBlockSIMD(TermDocIterator *tdi, const RowID block_min, const RowID block_last, const u32 term_idx) {
    const u32 batch_len = static_cast<u32>(block_last - block_min) + 1;
    const u32 aligned_len = ((batch_len + 7) / 8) * 8; // round up to 8 for AVX2

    std::memset(batch_tf_, 0, aligned_len * sizeof(u32));
    std::memset(batch_doc_len_, 0, aligned_len * sizeof(u32));
    std::memset(batch_match_cnt_, 0, aligned_len * sizeof(u32));
    std::memset(batch_score_sum_, 0, aligned_len * sizeof(f32));

    // BatchDecodeTo iterates SeekDoc from current position to block_last+1,
    // filling tf and doc_len at offset (iter_doc_id - block_min) for each match.
    tdi->BatchDecodeTo(block_min, block_last + 1, batch_tf_, batch_doc_len_);

    // SIMD batch BM25 for single keyword (children_num = 1)
    const auto &tp = term_params_[term_idx];
    GetSIMD_FUNCTIONS()
        .BatchBM25_func_ptr_(aligned_len, 1u, &tp.f1, &tp.f2, &tp.bm25_common_score, batch_tf_, batch_doc_len_, batch_match_cnt_, batch_score_sum_);

    // Accumulate
    for (u32 i = 0; i < batch_len; ++i) {
        if (batch_match_cnt_[i] > 0) {
            const RowID doc_id = block_min + i;
            const float score = batch_score_sum_[i];
            ++total_docs_processed_;
            AccumulateDoc(doc_id, score, 1);
        }
    }
}

// ------------------------------------------------------------------
// AccumulateDoc: add score to the accumulator's entry for doc_id,
// and keep the running top-k heap up to date for progressive threshold.
// ------------------------------------------------------------------
void BlockAtATimeIterator::AccumulateDoc(const RowID doc_id, const float score, const u32 match) {
    auto it = accumulator_.find(doc_id);
    float new_score;
    if (it != accumulator_.end()) {
        it->second.score += score;
        it->second.match_count += match;
        new_score = it->second.score;
    } else {
        accumulator_.emplace(doc_id, AccumEntry{score, match});
        new_score = score;
    }

    // Maintain running top-k heap for progressive threshold
    if (new_score >= threshold_) {
        running_topk_heap_.emplace(doc_id, new_score);
        // Lazy cleanup: only pop stale entries when heap is oversized
        if (running_topk_heap_.size() >= topn_ * 3u) {
            while (running_topk_heap_.size() > topn_ && running_topk_heap_.top().second < threshold_) {
                running_topk_heap_.pop();
            }
        }
        if (running_topk_heap_.size() >= topn_) {
            // Pop stale low-score entries from the top
            while (running_topk_heap_.top().second < threshold_) {
                running_topk_heap_.pop();
                if (running_topk_heap_.empty())
                    break;
            }
            if (running_topk_heap_.size() >= topn_) {
                threshold_ = running_topk_heap_.top().second;
            }
        }
    }
}

// ------------------------------------------------------------------
// ExtractTopK: rebuild clean heap from accumulator and sort into vector.
// ------------------------------------------------------------------
void BlockAtATimeIterator::ExtractTopK() {
    topk_results_.clear();
    while (!running_topk_heap_.empty()) {
        running_topk_heap_.pop();
    }
    for (const auto &[doc_id, entry] : accumulator_) {
        if (entry.score >= threshold_) {
            if (running_topk_heap_.size() < topn_) {
                running_topk_heap_.emplace(doc_id, entry.score);
            } else if (entry.score >= running_topk_heap_.top().second) {
                running_topk_heap_.pop();
                running_topk_heap_.emplace(doc_id, entry.score);
            }
        }
    }

    topk_results_.resize(running_topk_heap_.size());
    auto idx = running_topk_heap_.size();
    while (!running_topk_heap_.empty()) {
        topk_results_[--idx] = running_topk_heap_.top();
        running_topk_heap_.pop();
    }
}

float BlockAtATimeIterator::BM25Score() const { return score_cached_ ? bm25_score_cache_ : 0.0f; }

u32 BlockAtATimeIterator::MatchCount() const {
    if (doc_id_ == INVALID_ROWID)
        return 0;
    auto it = accumulator_.find(doc_id_);
    return (it != accumulator_.end()) ? it->second.match_count : 0;
}

void BlockAtATimeIterator::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "BlockAtATimeIterator (topn: " << topn_ << ", threshold: " << threshold_ << ")";
    os << '\n';
    const std::string next_prefix = prefix + (is_final ? "    " : "│   ");
    for (u32 i = 0; i + 1 < children_.size(); ++i) {
        children_[i]->PrintTree(os, next_prefix, false);
    }
    if (!children_.empty()) {
        children_.back()->PrintTree(os, next_prefix, true);
    }
}

} // namespace infinity

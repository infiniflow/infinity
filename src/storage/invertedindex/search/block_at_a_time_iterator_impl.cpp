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

BlockAtATimeIterator::BlockAtATimeIterator(std::vector<std::unique_ptr<DocIterator>> &&iterators, const u32 topn)
    : MultiDocIterator(std::move(iterators)), topn_(topn) {
    bm25_score_upper_bound_ = 0.0f;
    estimate_iterate_cost_ = {};
    size_t num_iterators = children_.size();
    term_params_.reserve(num_iterators);
    for (size_t i = 0; i < num_iterators; i++) {
        BlockMaxLeafIterator *leaf = dynamic_cast<BlockMaxLeafIterator *>(children_[i].get());
        if (leaf == nullptr) {
            UnrecoverableError("BlockAtATimeIterator only supports BlockMaxLeafIterator");
        }
        TermBM25Params params;
        params.leaf_iter = leaf;
        // Extract BM25 parameters (f1, f2, bm25_common_score) from the TermDocIterator
        const auto tdi = dynamic_cast<TermDocIterator *>(leaf);
        if (tdi != nullptr) {
            const auto [f1, f2, bm25_common_score] = tdi->Get_f1_f2_bm25_common_score();
            params.f1 = f1;
            params.f2 = f2;
            params.bm25_common_score = bm25_common_score;
        } else {
            // PhraseDocIterator fallback: use BlockMaxLeafIterator upper bound
            params.f1 = 0.0f;
            params.f2 = 0.0f;
            params.bm25_common_score = leaf->BM25ScoreUpperBound();
        }
        params.score_upper_bound = leaf->BM25ScoreUpperBound();
        bm25_score_upper_bound_ += params.score_upper_bound;
        estimate_iterate_cost_ += leaf->GetEstimateIterateCost();
        term_params_.emplace_back(std::move(params));
    }
    SortTermsByIDF();
    topk_results_.reserve(topn_);
}

BlockAtATimeIterator::~BlockAtATimeIterator() {
    if (SHOULD_LOG_TRACE()) {
        LOG_TRACE(fmt::format("BlockAtATimeIterator stats: docs_processed={}, blocks_skipped={}, blocks_decoded={}",
                              total_docs_processed_,
                              total_blocks_skipped_,
                              total_blocks_decoded_));
    }
}

void BlockAtATimeIterator::SortTermsByIDF() {
    const size_t n = term_params_.size();
    sorted_term_order_.resize(n);
    std::iota(sorted_term_order_.begin(), sorted_term_order_.end(), 0u);

    // Sort by IDF descending: rare terms first establish high threshold faster.
    // IDF ≈ log(1 + (N - df + 0.5) / (df + 0.5))
    // score_upper_bound is proportional to IDF * (k1 + 1), so we sort by it.
    std::sort(sorted_term_order_.begin(), sorted_term_order_.end(), [this](u32 a, u32 b) {
        return term_params_[a].score_upper_bound > term_params_[b].score_upper_bound;
    });
}

void BlockAtATimeIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold > threshold_) {
        threshold_ = threshold;
        // Propagate threshold to children for their internal BlockMax pruning
        for (auto &p : term_params_) {
            p.leaf_iter->UpdateScoreThreshold(threshold);
        }
    }
}

bool BlockAtATimeIterator::Next(RowID doc_id) {
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id) {
        score_cached_ = false;
        return true;
    }
    // If topk_results_ is not yet populated, run the full BAAT pipeline
    if (topk_results_.empty()) {
        // Initialize all term iterators
        for (size_t i = 0; i < term_params_.size(); i++) {
            term_params_[i].leaf_iter->Next(0);
        }

        // Run block-at-a-time for all terms
        for (u32 sort_idx = 0; sort_idx < sorted_term_order_.size(); ++sort_idx) {
            const u32 term_idx = sorted_term_order_[sort_idx];
            ProcessTerm(term_idx);
        }

        // Extract top-k from the accumulator
        ExtractTopK();
        topk_cursor_ = 0;
    }

    // Iterate through topk_results_ starting from the requested doc_id
    while (topk_cursor_ < topk_results_.size()) {
        const auto &[candidate_doc, candidate_score] = topk_results_[topk_cursor_];
        ++topk_cursor_;
        if (candidate_doc >= doc_id && candidate_score > threshold_) {
            doc_id_ = candidate_doc;
            bm25_score_cache_ = candidate_score;
            score_cached_ = true;
            return true;
        }
    }

    doc_id_ = INVALID_ROWID;
    return false;
}

void BlockAtATimeIterator::ProcessTerm(const u32 term_idx) {
    auto *leaf = term_params_[term_idx].leaf_iter;

    RowID target = 0;
    while (true) {
        // Advance the iterator. If its own threshold filters it out, we still need to
        // process filtered docs because the accumulator may have partial scores from
        // previously processed (rarer) terms. Set leaf threshold to 0 for full traversal.
        leaf->UpdateScoreThreshold(0.0f);

        if (!leaf->Next(target)) {
            break; // exhausted
        }

        // --- Block-level pruning ---
        RowID block_last = leaf->BlockLastDocID();
        RowID block_min = leaf->BlockMinPossibleDocID();

        // If block_min is INVALID_ROWID (no block loaded), skip
        if (block_min == INVALID_ROWID) {
            break;
        }

        float block_max_score = leaf->BlockMaxBM25Score();

        // Compute the best possible additional score from remaining (unprocessed) terms
        // for docs in accumulator. This is a conservative estimate - we use the
        // current block's max score as the potential contribution.
        if (threshold_ > 0.0f && block_max_score <= threshold_ * 0.01f) {
            // Block max is negligible relative to threshold, skip entire block
            ++total_blocks_skipped_;
            target = block_last + 1;
            continue;
        }

        ++total_blocks_decoded_;

        // --- Process all docs in this block ---
        RowID current_doc = leaf->DocID();
        while (current_doc <= block_last) {
            ++total_docs_processed_;
            const float score = leaf->Score();

            // Accumulate
            auto it = accumulator_.find(current_doc);
            if (it != accumulator_.end()) {
                it->second.score += score;
                it->second.match_count += 1;
            } else {
                accumulator_.emplace(current_doc, AccumEntry{score, 1});
            }

            // Advance to next doc in this block
            target = current_doc + 1;
            if (!leaf->Next(target)) {
                return; // exhausted
            }
            current_doc = leaf->DocID();
            if (current_doc > block_last) {
                target = current_doc;
                break;
            }
        }
    }
}

void BlockAtATimeIterator::ExtractTopK() {
    topk_results_.clear();

    // Minimum heap of size topn_ for efficient top-k extraction
    auto cmp = [](const std::pair<RowID, float> &a, const std::pair<RowID, float> &b) {
        return a.second > b.second; // min-heap by score
    };
    std::priority_queue<std::pair<RowID, float>, std::vector<std::pair<RowID, float>>, decltype(cmp)> min_heap(cmp);

    for (const auto &[doc_id, entry] : accumulator_) {
        if (entry.score <= threshold_) {
            continue;
        }
        if (min_heap.size() < topn_) {
            min_heap.emplace(doc_id, entry.score);
        } else if (entry.score > min_heap.top().second) {
            min_heap.pop();
            min_heap.emplace(doc_id, entry.score);
        }
    }

    // Extract from heap in descending score order
    topk_results_.resize(min_heap.size());
    size_t idx = min_heap.size();
    while (!min_heap.empty()) {
        --idx;
        topk_results_[idx] = min_heap.top();
        min_heap.pop();
    }

    LOG_TRACE(fmt::format("BlockAtATimeIterator::ExtractTopK: accumulator size={}, topk size={}", accumulator_.size(), topk_results_.size()));
}

float BlockAtATimeIterator::BM25Score() const {
    if (score_cached_) {
        return bm25_score_cache_;
    }
    return 0.0f;
}

u32 BlockAtATimeIterator::MatchCount() const {
    if (doc_id_ == INVALID_ROWID) {
        return 0;
    }
    auto it = accumulator_.find(doc_id_);
    if (it != accumulator_.end()) {
        return it->second.match_count;
    }
    return 0;
}

} // namespace infinity

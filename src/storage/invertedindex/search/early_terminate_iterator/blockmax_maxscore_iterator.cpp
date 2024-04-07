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
#include <tuple>
#include <vector>
module blockmax_maxscore_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

BlockMaxMaxscoreIterator::BlockMaxMaxscoreIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators) : sorted_iterators_(std::move(iterators)) {
    std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) {
        return a->BM25ScoreUpperBound() < b->BM25ScoreUpperBound();
    });
    // init df
    doc_freq_ = std::accumulate(sorted_iterators_.begin(),
                                sorted_iterators_.end(),
                                u32{},
                                [](const u32 sum, const UniquePtr<EarlyTerminateIterator> &iter) -> u32 { return sum + iter->DocFreq(); });
    common_block_max_bm25_score_until_.resize(sorted_iterators_.size());
    bm25_scores_upper_bound_until_.resize(sorted_iterators_.size());
    float sum_scores_upper_bound = 0.0f;
    for (u32 i = 0; i < sorted_iterators_.size(); ++i) {
        sum_scores_upper_bound += sorted_iterators_[i]->BM25ScoreUpperBound();
        bm25_scores_upper_bound_until_[i] = sum_scores_upper_bound;
    }
    bm25_score_upper_bound_ = sum_scores_upper_bound;
}

// inherited from EarlyTerminateIterator
bool BlockMaxMaxscoreIterator::BlockSkipTo(RowID doc_id, float threshold) {
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
        RowID next_candidate = INVALID_ROWID;
        float sum_block_max_bm25_score = 0.0f;
        bool match_any = false;
        for (u32 i = 0; i < sorted_iterators_.size(); ++i) {
            if (const auto &it = sorted_iterators_[i]; it->BlockSkipTo(doc_id, 0)) {
                // success in block skip
                if (const RowID lowest_possible = it->BlockMinPossibleDocID(); lowest_possible <= doc_id) {
                    match_any = true;
                    sum_block_max_bm25_score += it->BlockMaxBM25Score();
                    next_candidate = std::min(next_candidate, it->BlockLastDocID() + 1);
                } else {
                    next_candidate = std::min(next_candidate, lowest_possible);
                }
            }
            common_block_max_bm25_score_until_[i] = sum_block_max_bm25_score;
        }
        if (match_any and sum_block_max_bm25_score >= threshold) {
            common_block_min_possible_doc_id_ = doc_id;
            common_block_last_doc_id_ = next_candidate - 1;
            common_block_max_bm25_score_ = sum_block_max_bm25_score;
            return true;
        }
        if (next_candidate == INVALID_ROWID) {
            return false;
        }
        doc_id = next_candidate;
    }
}

Tuple<bool, float, RowID> BlockMaxMaxscoreIterator::SeekInBlockRange(RowID doc_id, float threshold, RowID doc_id_no_beyond) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    assert((doc_id >= BlockMinPossibleDocID()));
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, 0.0F, INVALID_ROWID};
        }
        RowID next_candidate = INVALID_ROWID;
        float leftover_threshold = threshold;
        for (u32 j = sorted_iterators_.size(); j > pivot; --j) {
            const auto &it = sorted_iterators_[j - 1];
            // TODO: does not need score if id != doc_id
            auto [success, score, id] = it->SeekInBlockRange(doc_id, 0, block_end);
            if (success) {
                if (id == doc_id) {
                    leftover_threshold -= score;
                    // TODO: does not need score here
                    auto [success2, score2, id2] = it->SeekInBlockRange(doc_id + 1, 0, block_end);
                    if (success2) {
                        next_candidate = std::min(next_candidate, id2);
                    }
                } else {
                    next_candidate = std::min(next_candidate, id);
                }
            }
        }
        for (u32 i = pivot; i > 0; --i) {
            if (leftover_threshold > common_block_max_bm25_score_until_[i - 1]) {
                // switch to next candidate
                break;
            }
            // TODO: add a "SeekTrue" method
            auto [success, score, id] = sorted_iterators_[i - 1]->SeekInBlockRange(doc_id, 0, block_end);
            if (success and id == doc_id) {
                leftover_threshold -= score;
            }
        }
        if (leftover_threshold <= 0) {
            doc_id_ = doc_id;
            return {true, threshold - leftover_threshold, doc_id};
        }
        doc_id = next_candidate;
    }
}

Pair<RowID, float> BlockMaxMaxscoreIterator::NextWithThreshold(float threshold) {
    // TODO:
    return {};
}

Pair<RowID, float> BlockMaxMaxscoreIterator::BlockNextWithThreshold(float threshold) {
    for (RowID next_skip = doc_id_ + 1;;) {
        if (!BlockSkipTo(next_skip, threshold)) [[unlikely]] {
            return {INVALID_ROWID, 0.0F};
        }
        next_skip = std::max(next_skip, BlockMinPossibleDocID());
        auto [success, score, id] = SeekInBlockRange(next_skip, threshold, BlockLastDocID());
        if (success) {
            // success in SeekInBlockRange, inner doc_id_ is updated
            return {id, score};
        }
        next_skip = BlockLastDocID() + 1;
    }
}

void BlockMaxMaxscoreIterator::UpdateScoreThreshold(float threshold) {
    const float base_threshold = threshold - BM25ScoreUpperBound();
    for (const auto &it : sorted_iterators_) {
        it->UpdateScoreThreshold(base_threshold + it->BM25ScoreUpperBound());
    }
    // update pivot
    while (pivot < sorted_iterators_.size() and bm25_scores_upper_bound_until_[pivot] < threshold) {
        ++pivot;
    }
}

} // namespace infinity
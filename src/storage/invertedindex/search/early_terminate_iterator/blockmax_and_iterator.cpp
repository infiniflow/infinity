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
#include <vector>
module blockmax_and_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;
import third_party;

namespace infinity {

BlockMaxAndIterator::BlockMaxAndIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators) : sorted_iterators_(std::move(iterators)) {
    std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) { return a->DocFreq() < b->DocFreq(); });
    // init df
    doc_freq_ = sorted_iterators_.front()->DocFreq();
    common_block_max_bm25_score_parts_.resize(sorted_iterators_.size());
    leftover_scores_upper_bound_.resize(sorted_iterators_.size());
    for (u32 i = sorted_iterators_.size() - 1; i > 0; --i) {
        leftover_scores_upper_bound_[i - 1] = leftover_scores_upper_bound_[i] + sorted_iterators_[i]->BM25ScoreUpperBound();
    }
    bm25_score_upper_bound_ = leftover_scores_upper_bound_[0] + sorted_iterators_[0]->BM25ScoreUpperBound();
}

void BlockMaxAndIterator::UpdateScoreThreshold(float threshold) {
    if (threshold < 0) {
        return;
    }
    const float base_threshold = threshold - BM25ScoreUpperBound();
    for (const auto &it : sorted_iterators_) {
        it->UpdateScoreThreshold(base_threshold + it->BM25ScoreUpperBound());
    }
}

bool BlockMaxAndIterator::BlockSkipTo(RowID doc_id, float threshold) {
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
        RowID common_block_last_doc_id = INVALID_ROWID;
        u32 i = 0;
        for (; i < sorted_iterators_.size(); ++i) {
            const auto &it = sorted_iterators_[i];
            if (!it->BlockSkipTo(doc_id, 0)) {
                // no more possible results
                return false;
            }
            // success in block skip
            const RowID lowest_possible = it->BlockMinPossibleDocID();
            if (lowest_possible > common_block_last_doc_id) {
                // need to update doc_id, restart from the first iterator
                break;
            }
            // now lowest_possible <= common_block_last_doc_id
            if (lowest_possible > doc_id) {
                doc_id = lowest_possible;
            }
            common_block_last_doc_id = std::min(common_block_last_doc_id, it->BlockLastDocID());
            assert((doc_id <= common_block_last_doc_id));
        }
        if (i == sorted_iterators_.size()) {
            float sum_score = 0.0f;
            for (u32 j = sorted_iterators_.size(); j > 0; --j) {
                const float prev_sum_score = sum_score;
                sum_score += sorted_iterators_[j - 1]->BlockMaxBM25Score();
                common_block_max_bm25_score_parts_[j - 1] = prev_sum_score;
            }
            fmt::print("sum_score: {}, bm25_score_upper_bound_: {}\n", sum_score, bm25_score_upper_bound_);
            // assert((sum_score <= bm25_score_upper_bound_));
            if (sum_score >= threshold) {
                common_block_max_bm25_score_ = sum_score;
                common_block_min_possible_doc_id_ = doc_id;
                common_block_last_doc_id_ = common_block_last_doc_id;
                return true;
            }
        }
        // continue loop
        doc_id = common_block_last_doc_id + 1;
    }
}

float BlockMaxAndIterator::BM25Score() {
    if (bm25_score_cached_) {
        return bm25_score_cache_;
    }
    float sum_score = 0.0f;
    for (const auto &it : sorted_iterators_) {
        sum_score += it->BM25Score();
    }
    bm25_score_cached_ = true;
    bm25_score_cache_ = sum_score;
    return sum_score;
}

Pair<bool, RowID> BlockMaxAndIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond) {
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, INVALID_ROWID};
        }
        // special case: first iterator, threshold will not change
        auto [success1, id1] = sorted_iterators_[0]->SeekInBlockRange(doc_id, block_end);
        if (!success1) {
            return {false, INVALID_ROWID};
        }
        doc_id = id1;
        u32 i = 1;
        for (; i < sorted_iterators_.size(); ++i) {
            const auto [success2, id2] = sorted_iterators_[i]->SeekInBlockRange(doc_id, block_end);
            if (id2 != doc_id) {
                // need to update doc_id, restart from the first iterator
                doc_id = id2;
                break;
            }
        }
        if (i == sorted_iterators_.size()) {
            doc_id_ = doc_id;
            bm25_score_cached_ = false;
            return {true, doc_id};
        }
    }
}

Tuple<bool, float, RowID> BlockMaxAndIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond, const float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, 0.0F, INVALID_ROWID};
        }
        // special case: first iterator, threshold will not change
        auto [success1, id1] = sorted_iterators_[0]->SeekInBlockRange(doc_id, block_end);
        if (!success1) {
            return {false, 0.0F, INVALID_ROWID};
        }
        doc_id = id1;
        u32 i = 1;
        for (; i < sorted_iterators_.size(); ++i) {
            const auto [success2, id2] = sorted_iterators_[i]->SeekInBlockRange(doc_id, block_end);
            if (id2 != doc_id) {
                // need to update doc_id, restart from the first iterator
                doc_id = id2;
                break;
            }
        }
        if (i == sorted_iterators_.size()) {
            float leftover_threshold = threshold;
            bool valid = true;
            for (u32 i = 0; i < sorted_iterators_.size(); ++i) {
                leftover_threshold -= sorted_iterators_[i]->BM25Score();
                if (leftover_threshold > common_block_max_bm25_score_parts_[i]) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                doc_id_ = doc_id;
                const float score_result = threshold - leftover_threshold;
                bm25_score_cached_ = true;
                bm25_score_cache_ = score_result;
                return {true, score_result, doc_id};
            }
            ++doc_id;
        }
    }
}

// TODO: simplify this
Pair<bool, RowID> BlockMaxAndIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    RowID seek_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > seek_end) [[unlikely]] {
            return {false, INVALID_ROWID};
        }
        // special case: first iterator, threshold will not change
        auto [success1, id1] = sorted_iterators_[0]->PeekInBlockRange(doc_id, seek_end);
        if (!success1) {
            return {false, INVALID_ROWID};
        }
        assert((id1 <= seek_end));
        assert((id1 >= doc_id));
        doc_id = id1;
        u32 i = 1;
        for (; i < sorted_iterators_.size(); ++i) {
            auto [success2, id2] = sorted_iterators_[i]->PeekInBlockRange(doc_id, doc_id);
            if (!success2) {
                // need to update doc_id, restart from the first iterator
                ++doc_id;
                break;
            }
            assert((id2 == doc_id));
        }
        if (i == sorted_iterators_.size()) {
            return {true, doc_id};
        }
    }
}

bool BlockMaxAndIterator::NotPartCheckExist(RowID doc_id) {
    if (doc_id_ > doc_id) {
        return false;
    }
    if (doc_id_ == doc_id) {
        return true;
    }
    for (const auto &it : sorted_iterators_) {
        if (!it->NotPartCheckExist(doc_id)) {
            return false;
        }
    }
    doc_id_ = doc_id;
    return true;
}

} // namespace infinity
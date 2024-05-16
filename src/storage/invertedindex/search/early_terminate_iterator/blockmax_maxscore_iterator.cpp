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
#include <iostream>
#include <tuple>
#include <vector>
module blockmax_maxscore_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;
import logger;

namespace infinity {

BlockMaxMaxscoreIterator::~BlockMaxMaxscoreIterator() {
    OStringStream oss;
    oss << "BlockMaxMaxscoreIterator: Debug Info:\n    inner_pivot_loop_cnt: " << inner_pivot_loop_cnt
        << " inner_must_have_loop_cnt: " << inner_must_have_loop_cnt_ << " use_prev_candidate_cnt: " << use_prev_candidate_cnt_
        << " not_use_prev_candidate_cnt: " << not_use_prev_candidate_cnt_ << "\n";
    oss << "    pivot_history:\n";
    for (const auto &p : pivot_history_) {
        oss << "    pivit value: " << p.first << " at doc_id: " << p.second << '\n';
    }
    oss << "    must_have_history:\n";
    for (const auto &p : must_have_history_) {
        oss << "    must_have value: " << p.first << " at doc_id: " << p.second << '\n';
    }
    LOG_TRACE(std::move(oss).str());
}

BlockMaxMaxscoreIterator::BlockMaxMaxscoreIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators) : sorted_iterators_(std::move(iterators)) {
    std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) {
        return a->BM25ScoreUpperBound() > b->BM25ScoreUpperBound();
    });
    // init df
    doc_freq_ = std::accumulate(sorted_iterators_.begin(),
                                sorted_iterators_.end(),
                                u32{},
                                [](const u32 sum, const UniquePtr<EarlyTerminateIterator> &iter) -> u32 { return sum + iter->DocFreq(); });
    common_block_max_bm25_score_parts_.resize(sorted_iterators_.size());
    leftover_scores_upper_bound_.resize(sorted_iterators_.size());
    for (u32 i = sorted_iterators_.size() - 1; i > 0; --i) {
        leftover_scores_upper_bound_[i - 1] = leftover_scores_upper_bound_[i] + sorted_iterators_[i]->BM25ScoreUpperBound();
    }
    bm25_score_upper_bound_ = leftover_scores_upper_bound_[0] + sorted_iterators_[0]->BM25ScoreUpperBound();
}

// inherited from EarlyTerminateIterator
bool BlockMaxMaxscoreIterator::BlockSkipTo(RowID doc_id, float threshold) {
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
        RowID next_candidate = INVALID_ROWID;
        bool match_any = false;
        u32 i = 0;
        for (float min_leftover_threshold = threshold; i < sorted_iterators_.size(); ++i) {
            if (const auto &it = sorted_iterators_[i]; it->BlockSkipTo(doc_id, 0)) {
                // success in block skip
                if (const RowID lowest_possible = it->BlockMinPossibleDocID(); lowest_possible <= doc_id) {
                    match_any = true;
                    min_leftover_threshold -= it->BlockMaxBM25Score();
                    next_candidate = std::min(next_candidate, it->BlockLastDocID() + 1);
                } else {
                    next_candidate = std::min(next_candidate, lowest_possible);
                }
            }
            if (min_leftover_threshold > leftover_scores_upper_bound_[i]) {
                // no need to check the rest
                break;
            }
        }
        if (i == sorted_iterators_.size() and match_any) {
            for (u32 j = sorted_iterators_.size() - 1; j > 0; --j) {
                common_block_max_bm25_score_parts_[j - 1] = common_block_max_bm25_score_parts_[j] + sorted_iterators_[j]->BlockMaxBM25Score();
            }
            common_block_max_bm25_score_ = common_block_max_bm25_score_parts_[0] + sorted_iterators_[0]->BlockMaxBM25Score();
            common_block_min_possible_doc_id_ = doc_id;
            common_block_last_doc_id_ = next_candidate - 1;
            assert((common_block_max_bm25_score_ >= threshold));
            return true;
        }
        if (next_candidate == INVALID_ROWID) {
            return false;
        }
        doc_id = next_candidate;
    }
}

float BlockMaxMaxscoreIterator::BM25Score() {
    if (bm25_score_cached_) [[unlikely]] {
        return bm25_score_cache_;
    }
    u32 seek_start = sorted_iterators_.size();
    if (need_seek_after_must_) {
        seek_start = must_have_before_;
    } else if (need_seek_after_pivot_) {
        seek_start = pivot_;
    }
    const RowID doc_id = doc_id_;
    for (u32 i = seek_start; i < sorted_iterators_.size(); ++i) {
        sorted_iterators_[i]->SeekInBlockRange(doc_id, doc_id);
    }
    float sum_score = 0.0F;
    for (const auto &it : sorted_iterators_) {
        if (it->DocID() == doc_id) {
            sum_score += it->BM25Score();
        }
    }
    bm25_score_cached_ = true;
    bm25_score_cache_ = sum_score;
    return sum_score;
}

Pair<bool, RowID> BlockMaxMaxscoreIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond) {
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    if (doc_id > block_end) {
        return {false, INVALID_ROWID};
    }
    if (must_have_before_) {
        // use must_have_before_
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
                need_seek_after_must_ = true;
                need_seek_after_pivot_ = false;
                return {true, doc_id};
            }
        }
    }
    // use pivot
    bool match_any = false;
    RowID next_candidate = INVALID_ROWID;
    for (u32 j = 0; j < pivot_; ++j) {
        if (auto [success, id] = sorted_iterators_[j]->SeekInBlockRange(doc_id, block_end); success) {
            match_any = true;
            next_candidate = std::min(next_candidate, id);
        }
    }
    if (match_any) {
        doc_id_ = next_candidate;
        bm25_score_cached_ = false;
        need_seek_after_must_ = false;
        need_seek_after_pivot_ = true;
    }
    return {match_any, next_candidate};
}

Tuple<bool, float, RowID> BlockMaxMaxscoreIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond, const float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    assert((doc_id >= BlockMinPossibleDocID()));
    if (const RowID prev_next_candidate = prev_next_candidate_; prev_next_candidate != INVALID_ROWID and doc_id < prev_next_candidate) {
        ++use_prev_candidate_cnt_;
        doc_id = prev_next_candidate;
        prev_next_candidate_ = INVALID_ROWID;
    } else {
        ++not_use_prev_candidate_cnt_;
    }
    if (must_have_before_) {
        // use must_have_before_
        while (true) {
            while (true) {
                ++inner_must_have_loop_cnt_;
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
                for (; i < must_have_before_; ++i) {
                    const auto [success2, id2] = sorted_iterators_[i]->SeekInBlockRange(doc_id, block_end);
                    if (id2 != doc_id) {
                        // need to update doc_id, restart from the first iterator
                        doc_id = id2;
                        break;
                    }
                }
                if (i == must_have_before_) {
                    // have candidate
                    break;
                }
            }
            // now check doc_id
            float leftover_threshold = threshold;
            u32 i = 0;
            for (; i < must_have_before_; ++i) {
                leftover_threshold -= sorted_iterators_[i]->BM25Score();
                if (leftover_threshold > common_block_max_bm25_score_parts_[i]) {
                    // no need to check the rest
                    break;
                }
            }
            if (i == must_have_before_) {
                for (; i < sorted_iterators_.size(); ++i) {
                    const auto [success, score, id] = sorted_iterators_[i]->SeekInBlockRange(doc_id, doc_id, 0);
                    if (success) {
                        leftover_threshold -= score;
                    }
                    if (leftover_threshold > common_block_max_bm25_score_parts_[i]) {
                        // no need to check the rest
                        break;
                    }
                }
            }
            if (i == sorted_iterators_.size()) {
                doc_id_ = doc_id;
                const float bm25_score = threshold - leftover_threshold;
                bm25_score_cached_ = true;
                bm25_score_cache_ = bm25_score;
                prev_next_candidate_ = INVALID_ROWID;
                return {true, bm25_score, doc_id};
            }
            ++doc_id;
        }
        // return before here
    }
    // now must_have_before_ == 0, pivot > 1. use pivot.
    assert(pivot_ > 1);
    while (true) {
        ++inner_pivot_loop_cnt;
        if (doc_id > block_end) [[unlikely]] {
            return {false, 0.0F, INVALID_ROWID};
        }
        RowID next_candidate = INVALID_ROWID;
        float leftover_threshold = threshold;
        bool match_any = false;
        for (u32 j = 0; j < pivot_; ++j) {
            const auto &it = sorted_iterators_[j];
            const auto [success, id] = it->SeekInBlockRange(doc_id, doc_id);
            if (success) {
                assert((id == doc_id));
                match_any = true;
                leftover_threshold -= it->BM25Score();
            }
            auto [success2, id2] = it->PeekInBlockRange(doc_id + 1, block_end);
            if (success2) {
                next_candidate = std::min(next_candidate, id2);
            }
        }
        if (match_any) {
            for (u32 i = pivot_; i < sorted_iterators_.size() and leftover_threshold <= common_block_max_bm25_score_parts_[i - 1]; ++i) {
                auto [success, id] = sorted_iterators_[i]->SeekInBlockRange(doc_id, doc_id);
                if (success) {
                    leftover_threshold -= sorted_iterators_[i]->BM25Score();
                }
            }
            if (leftover_threshold <= 0) {
                doc_id_ = doc_id;
                const float bm25_score = threshold - leftover_threshold;
                bm25_score_cached_ = true;
                bm25_score_cache_ = bm25_score;
                prev_next_candidate_ = next_candidate;
                return {true, bm25_score, doc_id};
            }
        }
        assert((doc_id < next_candidate));
        doc_id = next_candidate;
    }
}

Pair<bool, RowID> BlockMaxMaxscoreIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    RowID seek_end = std::min(doc_id_no_beyond, BlockLastDocID());
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    bool match_any = false;
    RowID next_candidate = INVALID_ROWID;
    for (u32 j = 0; j < pivot_; ++j) {
        if (auto [success, id] = sorted_iterators_[j]->PeekInBlockRange(doc_id, seek_end); success) {
            match_any = true;
            next_candidate = std::min(next_candidate, id);
        }
    }
    return {match_any, next_candidate};
}

void BlockMaxMaxscoreIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold < 0) {
        return;
    }
    const float base_threshold = threshold - BM25ScoreUpperBound();
    for (const auto &it : sorted_iterators_) {
        it->UpdateScoreThreshold(base_threshold + it->BM25ScoreUpperBound());
    }
    // update pivot
    while (pivot_ > 0 and threshold > (pivot_ > 1 ? leftover_scores_upper_bound_[pivot_ - 2] : BlockMaxBM25Score())) {
        --pivot_;
        pivot_history_.emplace_back(pivot_, doc_id_.ToUint64());
    }
    // update must have
    while (must_have_before_ < sorted_iterators_.size() and
           threshold > must_have_total_upper_bound_score_ + leftover_scores_upper_bound_[must_have_before_]) {
        must_have_total_upper_bound_score_ += sorted_iterators_[must_have_before_++]->BM25ScoreUpperBound();
        must_have_history_.emplace_back(must_have_before_, doc_id_.ToUint64());
    }
}

bool BlockMaxMaxscoreIterator::NotPartCheckExist(RowID doc_id) {
    if (doc_id_ > doc_id) {
        return false;
    }
    if (doc_id_ == doc_id) {
        return true;
    }
    for (const auto &it : sorted_iterators_) {
        if (it->NotPartCheckExist(doc_id)) {
            return true;
        }
    }
    return false;
}

} // namespace infinity
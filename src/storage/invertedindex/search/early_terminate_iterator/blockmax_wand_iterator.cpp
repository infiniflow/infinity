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

module blockmax_wand_iterator;
import stl;
import third_party;
import index_defines;
import early_terminate_iterator;
import internal_types;
import logger;
import infinity_exception;

namespace infinity {

BlockMaxWandIterator::~BlockMaxWandIterator() {
    String msg = "BlockMaxWandIterator pivot_history: ";
    SizeT num_history = pivot_history_.size();
    for (SizeT i=0; i<num_history; i++) {
        auto &p = pivot_history_[i];
        u32 pivot = std::get<0>(p);
        u64 row_id = std::get<1>(p);
        float score = std::get<2>(p);
        //oss << " (" << pivot << ", " << row_id << ", " << score << ")";
        msg += fmt::format(" ({}, {}, {:6f})", pivot, row_id, score);
    }
    msg += fmt::format("\nnext_sort_cnt_ {}, next_it0_docid_mismatch_cnt_ {}, next_sum_score_low_cnt_ {}, next_sum_score_bm_low_cnt_ {}", next_sort_cnt_, next_it0_docid_mismatch_cnt_, next_sum_score_low_cnt_, next_sum_score_bm_low_cnt_);
    LOG_TRACE(msg);
}

BlockMaxWandIterator::BlockMaxWandIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators) : sorted_iterators_(std::move(iterators)), pivot_(sorted_iterators_.size()) {
    backup_iterators_.reserve(sorted_iterators_.size());
    bm25_score_upper_bound_ = 0.0f;
    SizeT num_iterators = sorted_iterators_.size();
    for (SizeT i = 0; i < num_iterators; i++){
        bm25_score_upper_bound_ += sorted_iterators_[i]->BM25ScoreUpperBound();
    }
    next_sum_score_bm_low_cnt_dist_.resize(100, 0);
}

void BlockMaxWandIterator::UpdateScoreThreshold(const float threshold) {
    EarlyTerminateIterator::UpdateScoreThreshold(threshold);
    const float base_threshold = threshold - BM25ScoreUpperBound();
    SizeT num_iterators = sorted_iterators_.size();
    for (SizeT i = 0; i < num_iterators; i++){
        auto &it = sorted_iterators_[i];
        float new_threshold = base_threshold + it->BM25ScoreUpperBound();
        it->UpdateScoreThreshold(new_threshold);
    }
}

bool BlockMaxWandIterator::NextShallow(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    assert(backup_iterators_.empty());
    RowID common_block_last_doc_id = INVALID_ROWID;
    SizeT num_iterators = sorted_iterators_.size();
    SizeT gap = num_iterators;
    for (SizeT i = 0; i < num_iterators; i++){
        auto &it = sorted_iterators_[i];
        bool ok = it->NextShallow(doc_id);
        if (ok)[[likely]] {
            if (common_block_last_doc_id > it->BlockLastDocID())
                common_block_last_doc_id = it->BlockLastDocID();
            if(gap < num_iterators)[[unlikely]] {
                sorted_iterators_[gap] = std::move(it);
                gap++;
            }
        } else{
            it.reset();
            if(gap == num_iterators){
                gap = i;
            }
        }
    }
    if(gap != num_iterators)[[unlikely]] {
        sorted_iterators_.resize(gap);
    }
    if (sorted_iterators_.empty())[[unlikely]] {
        common_block_min_possible_doc_id_ = INVALID_ROWID;
        common_block_last_doc_id_ = INVALID_ROWID;
        doc_id_ = INVALID_ROWID;
        return false;
    } else {
        common_block_min_possible_doc_id_ = doc_id;
        common_block_last_doc_id_ = common_block_last_doc_id;
        return true;
    }
}

bool BlockMaxWandIterator::Next(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    bm25_score_cached_ = false;
    SizeT num_iterators = sorted_iterators_.size();
    if (doc_id_ == INVALID_ROWID) {
        // Initialize children once.
        for (SizeT i = 0; i < num_iterators; i++) {
            sorted_iterators_[i]->Next(0);
        }
    } else {
        assert(pivot_ < num_iterators);
        assert(doc_id_ < doc_id);
        // Move all pointers from lists[0] to lists[p] by calling Next(list, d + 1)
        for (SizeT i = 0; i <= pivot_ && sorted_iterators_[i]->DocID() < doc_id; i++) {
            sorted_iterators_[i]->Next(doc_id);
        }
    }
    while(1){
        // sort the lists by current docIDs
        next_sort_cnt_++;
        std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) {
            return a->DocID() < b->DocID();
        });
        // remove exhausted lists
        for (int i = int(num_iterators) - 1; i >= 0 && sorted_iterators_[i]->DocID() == INVALID_ROWID; i--) {
            if (SHOULD_LOG_DEBUG()) {
                OStringStream oss;
                sorted_iterators_[i]->PrintTree(oss, "Exhaused: ", true);
                LOG_DEBUG(oss.str());
            }
            bm25_score_upper_bound_ -= sorted_iterators_[i]->BM25ScoreUpperBound();
            sorted_iterators_.pop_back();
            num_iterators --;
        }
        if (bm25_score_upper_bound_ <= threshold_) [[unlikely]] {
            doc_id_ = INVALID_ROWID;
            return false;
        }

        // same "pivoting" as in WAND using the max impact for the whole lists, use p to denote the pivot
        SizeT pivot = num_iterators;
        float sum_score_ub = 0.0f;
        for (SizeT i = 0; i < num_iterators; i++) {
            sum_score_ub += sorted_iterators_[i]->BM25ScoreUpperBound();
            if (sum_score_ub > threshold_) {
                pivot = i;
                break;
            }
        }
        if (pivot >= num_iterators) [[unlikely]] {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        RowID d = sorted_iterators_[pivot]->DocID();
        while(pivot + 1 < num_iterators && sorted_iterators_[pivot + 1]->DocID() == d){
            pivot++;
        }

        // NextShallow iterators 0..pivot to d, sum the blockmax score, and purge exhausted lists
        float sum_score_bm = 0.0f;
        bool found_exhausted_it = false;
        for(SizeT i=0; i<=pivot; i++){
            bool ok = sorted_iterators_[i]->NextShallow(d);
            if (ok) [[likely]] {
                sum_score_bm += sorted_iterators_[i]->BlockMaxBM25Score();
            } else {
                if (SHOULD_LOG_DEBUG()) {
                    OStringStream oss;
                    sorted_iterators_[i]->PrintTree(oss, "Exhausted: ", true);
                    LOG_DEBUG(oss.str());
                }
                sorted_iterators_.erase(sorted_iterators_.begin() + i);
                num_iterators = sorted_iterators_.size();
                found_exhausted_it = true;
                break;
            }
        }
        if (found_exhausted_it) [[unlikely]] {
            continue;
        }

        if (sum_score_bm > threshold_) {
            if (sorted_iterators_[0]->DocID() == d) {
                // EvaluatePartial(d , p);
                float sum_score = 0.0f;
                for(SizeT i=0; i<=pivot; i++){
                    sum_score += sorted_iterators_[i]->BM25Score();
                }
                if(sum_score > threshold_){
                    pivot_ = pivot;
                    doc_id_ = d;
                    bm25_score_cache_ = sum_score;
                    bm25_score_cached_ = true;
                    pivot_history_.emplace_back(pivot_, doc_id_.ToUint64(), sum_score);
                    return true;
                } else {
                    next_sum_score_low_cnt_++;
                    for (SizeT i = 0; i <= pivot; i++) {
                        sorted_iterators_[i]->Next(d+1);
                    }
                }
            } else {
                next_it0_docid_mismatch_cnt_++;
                for (SizeT i = 0; i < pivot && sorted_iterators_[i]->DocID() < d; i++) {
                    sorted_iterators_[i]->Next(d);
                }
            }
        } else {
            next_sum_score_bm_low_cnt_++;
            // d′ = GetNewCandidate();
            // Choose one list from the lists before and including lists[p] with the largest IDF, move it by calling Next(list, d′)
            RowID up_to = INVALID_ROWID;
            if (pivot + 1 < num_iterators)
                up_to = sorted_iterators_[pivot + 1]->DocID();
            RowID shallowed_did = INVALID_ROWID;
            std::partial_sort(sorted_iterators_.begin(),
                              sorted_iterators_.begin() + 1,
                              sorted_iterators_.begin() + pivot + 1,
                              [](const auto &a, const auto &b) { return a->BlockLastDocID() < b->BlockLastDocID(); });
            // sum_score_bm_0 is the sum of block_max of [1..pivot]
            float sum_score_bm_0 = sum_score_bm - sorted_iterators_[0]->BlockMaxBM25Score();
            RowID block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
            SizeT cnt_next_shallow = 0;
            while (block_last_doc_id_0 < up_to) {
                shallowed_did = block_last_doc_id_0 + 1;
                cnt_next_shallow++;
                bool ok = sorted_iterators_[0]->NextShallow(block_last_doc_id_0 + 1);
                if (!ok)
                    break;
                block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
                if (block_last_doc_id_0 == INVALID_ROWID)
                    break;
                sum_score_bm = sum_score_bm_0 + sorted_iterators_[0]->BlockMaxBM25Score();
                if (sum_score_bm > threshold_)
                    break;
                std::partial_sort(sorted_iterators_.begin(),
                                  sorted_iterators_.begin() + 1,
                                  sorted_iterators_.begin() + pivot + 1,
                                  [](const auto &a, const auto &b) { return a->BlockLastDocID() < b->BlockLastDocID(); });
                // sum_score_bm_0 is the sum of block_max of [1..pivot]
                sum_score_bm_0 = sum_score_bm - sorted_iterators_[0]->BlockMaxBM25Score();
                block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
            }
            if (cnt_next_shallow >= 100) [[unlikely]] {
                next_sum_score_bm_low_cnt_dist_[99]++;
            } else {
                next_sum_score_bm_low_cnt_dist_[cnt_next_shallow]++;
            }
            if (shallowed_did != INVALID_ROWID) {
                for (SizeT i = 0; i <= pivot; i++) {
                    if (sorted_iterators_[i]->DocID() < shallowed_did)
                        sorted_iterators_[i]->Next(shallowed_did);
                }
            } else {
                for (SizeT i = 0; i <= pivot; i++) {
                    sorted_iterators_[i]->Next(up_to);
                }
            }
        }
    }

    String error_message = "Should not reach here!";
    UnrecoverableError(error_message);
    return false;
}

// inherited from EarlyTerminateIterator
bool BlockMaxWandIterator::BlockSkipTo(RowID doc_id, float threshold) {
    return false;
}

float BlockMaxWandIterator::BM25Score() {
    if (bm25_score_cached_) [[unlikely]] {
        return bm25_score_cache_;
    }
    SizeT num_iterators = sorted_iterators_.size();
    if (doc_id_ == INVALID_ROWID || pivot_ >= num_iterators) [[unlikely]] {
        return 0.0f;
    }
    float sum_score = 0.0f;
    RowID d = sorted_iterators_[pivot_]->DocID();
    for(SizeT i=0; i<num_iterators && sorted_iterators_[i]->DocID() == d; i++){
        sum_score += sorted_iterators_[i]->BM25Score();
    }
    bm25_score_cache_ = sum_score;
    bm25_score_cached_ = true;
    return sum_score;
}

Pair<bool, RowID> BlockMaxWandIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond) {
    return {false, INVALID_ROWID};
}

Tuple<bool, float, RowID> BlockMaxWandIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond, const float threshold) {
    return {false, 0.0F, INVALID_ROWID};
}

Pair<bool, RowID> BlockMaxWandIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    return {false, INVALID_ROWID};
}

bool BlockMaxWandIterator::NotPartCheckExist(RowID doc_id) {
    return false;
}

} // namespace infinity
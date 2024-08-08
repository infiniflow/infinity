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
module blockmax_maxscore_iterator;
import stl;
import index_defines;
import term_doc_iterator;
import multi_doc_iterator;
import internal_types;
import logger;
import third_party;

namespace infinity {

BlockMaxMaxscoreIterator::~BlockMaxMaxscoreIterator() {
    OStringStream oss;
    oss << "BlockMaxMaxscoreIterator: Debug Info:\n    inner_pivot_loop_cnt: " << inner_pivot_loop_cnt
        << " inner_must_have_loop_cnt: " << inner_must_have_loop_cnt_ << " use_prev_candidate_cnt: " << use_prev_candidate_cnt_
        << " not_use_prev_candidate_cnt: " << not_use_prev_candidate_cnt_ << "\n";
    oss << "    pivot_history:\n";
    for (const auto &p : pivot_history_) {
        oss << "    pivot value: " << p.first << " at doc_id: " << p.second << '\n';
    }
    oss << "    must_have_history:\n";
    for (const auto &p : must_have_history_) {
        oss << "    must_have value: " << p.first << " at doc_id: " << p.second << '\n';
    }
    LOG_DEBUG(std::move(oss).str());
}

BlockMaxMaxscoreIterator::BlockMaxMaxscoreIterator(Vector<UniquePtr<DocIterator>> &&iterators) : MultiDocIterator(std::move(iterators)) {
    SizeT num_iterators = children_.size();
    for (SizeT i = 0; i < num_iterators; i++) {
        TermDocIterator *tdi = dynamic_cast<TermDocIterator *>(children_[i].get());
        if (tdi == nullptr)
            continue;
        bm25_score_upper_bound_ += tdi->BM25ScoreUpperBound();
        doc_freq_ += tdi->GetDF();
        sorted_iterators_.push_back(tdi);
    }
    std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) {
        return a->BM25ScoreUpperBound() > b->BM25ScoreUpperBound();
    });
    Init();
}

void BlockMaxMaxscoreIterator::Init() {
    common_block_max_bm25_score_parts_.resize(sorted_iterators_.size());
    leftover_scores_upper_bound_.resize(sorted_iterators_.size());
    for (u32 i = sorted_iterators_.size() - 1; i > 0; --i) {
        leftover_scores_upper_bound_[i - 1] = leftover_scores_upper_bound_[i] + sorted_iterators_[i]->BM25ScoreUpperBound();
    }
    bm25_score_upper_bound_ = leftover_scores_upper_bound_[0] + sorted_iterators_[0]->BM25ScoreUpperBound();
}

bool BlockMaxMaxscoreIterator::Next(RowID doc_id) {
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
    return true;
}

float BlockMaxMaxscoreIterator::BM25Score() {
    if (bm25_score_cached_) [[unlikely]] {
        return bm25_score_cache_;
    }
    float sum_score = 0.0f;
    bm25_score_cached_ = true;
    bm25_score_cache_ = sum_score;
    return sum_score;
}

void BlockMaxMaxscoreIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold <= threshold_)
        return;
    threshold_ = threshold;
}

} // namespace infinity
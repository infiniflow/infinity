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

module or_iterator;
import internal_types;

import stl;
import index_defines;
import multi_doc_iterator;
import doc_iterator;
namespace infinity {

void DocIteratorHeap::BuildHeap() {
    for (SizeT i = (1 + iterator_heap_.size()) / 2; i > 0; --i) {
        AdjustDown(i);
    }
}

void DocIteratorHeap::AdjustDown(SizeT idx) {
    assert(idx >= 1 && idx < iterator_heap_.size());
    SizeT min = idx;
    SizeT end = iterator_heap_.size();
    do {
        idx = min;
        SizeT left = idx << 1;
        SizeT right = left + 1;
        if (left < end && iterator_heap_[left].doc_id_ < iterator_heap_[min].doc_id_) {
            min = left;
        }
        if (right < end && iterator_heap_[right].doc_id_ < iterator_heap_[min].doc_id_) {
            min = right;
        }
        if (min != idx) {
            std::swap(iterator_heap_[idx], iterator_heap_[min]);
        }
    } while (min != idx);
}

OrIterator::OrIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiDocIterator(std::move(iterators)) {
    bm25_score_upper_bound_ = 0.0f;
    estimate_iterate_cost_ = {};
    for (const auto &child : children_) {
        bm25_score_upper_bound_ += child->BM25ScoreUpperBound();
        estimate_iterate_cost_ += child->GetEstimateIterateCost();
    }
}

bool OrIterator::Next(const RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ == INVALID_ROWID) {
        for (u32 i = 0; i < children_.size(); ++i) {
            children_[i]->Next();
            DocIteratorEntry entry = {children_[i]->DocID(), i};
            heap_.AddEntry(entry);
        }
        heap_.BuildHeap();
        doc_id_ = heap_.TopEntry().doc_id_;
    }
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    while (doc_id > heap_.TopEntry().doc_id_) {
        DocIterator *top = GetDocIterator(heap_.TopEntry().entry_id_);
        top->Next(doc_id);
        heap_.TopEntry().doc_id_ = top->DocID();
        heap_.AdjustDown(1);
    }
    doc_id_ = heap_.TopEntry().doc_id_;
    return doc_id_ != INVALID_ROWID;
}

float OrIterator::Score() {
    if (score_cache_docid_ == doc_id_) {
        return score_cache_;
    }
    float sum_score = 0;
    for (const auto &child : children_) {
        if (child->DocID() == doc_id_) {
            sum_score += child->Score();
        }
    }
    score_cache_docid_ = doc_id_;
    score_cache_ = sum_score;
    return sum_score;
}

void OrIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold <= threshold_)
        return;
    threshold_ = threshold;
    const float base_threshold = threshold - BM25ScoreUpperBound();
    for (const auto &child : children_) {
        const float new_threshold = std::max(0.0f, base_threshold + child->BM25ScoreUpperBound());
        child->UpdateScoreThreshold(new_threshold);
    }
}

u32 OrIterator::MatchCount() const {
    u32 count = 0;
    if (const auto current_doc_id = DocID(); current_doc_id != INVALID_ROWID) {
        for (const auto &child : children_) {
            if (child->DocID() == current_doc_id) {
                count += child->MatchCount();
            }
        }
    }
    return count;
}

} // namespace infinity
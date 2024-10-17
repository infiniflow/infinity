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
#include <vector>
module minimum_should_match_iterator;
import stl;
import index_defines;
import multi_doc_iterator;
import internal_types;
import logger;
import infinity_exception;

namespace infinity {

MinimumShouldMatchIterator::MinimumShouldMatchIterator(Vector<UniquePtr<DocIterator>> &&iterators, const u32 minimum_should_match)
    : MultiDocIterator(std::move(iterators)), minimum_should_match_(minimum_should_match) {
#ifndef NDEBUG
    // validate children
    for (const auto &child : children_) {
        switch (child->GetType()) {
            case DocIteratorType::kTermDocIterator:
            case DocIteratorType::kPhraseIterator: {
                // acceptable
                break;
            }
            default: {
                UnrecoverableError("MinimumShouldMatchIterator can only accept TermDocIterator or PhraseIterator as children");
            }
        }
    }
#endif
    if (minimum_should_match_ <= 1u) {
        UnrecoverableError("MinimumShouldMatchIterator should have minimum_should_match > 1");
    }
    tail_heap_.resize(minimum_should_match_ - 1u);
    bm25_score_cache_docid_ = INVALID_ROWID;
}

MinimumShouldMatchIterator::~MinimumShouldMatchIterator() {}

void MinimumShouldMatchIterator::UpdateScoreThreshold(float threshold) { UnrecoverableError("Unreachable code"); }

bool MinimumShouldMatchIterator::Next(RowID doc_id) {
    if (doc_id_ == INVALID_ROWID) {
        // Initialize once.
        lead_.resize(children_.size());
        std::iota(lead_.begin(), lead_.end(), 0u);
    } else if (doc_id_ >= doc_id) {
        return true;
    }
    for (;; ++doc_id) {
        for (const auto idx : lead_) {
            if (const auto [have_pop, pop_idx] = PushToTailHeap(idx); have_pop) {
                if (children_[pop_idx]->Next(doc_id)) {
                    PushToHeadHeap(pop_idx);
                }
            }
        }
        lead_.clear();
        while (HeadHeapTop() < doc_id) {
            const auto idx = PopFromHeadHeap();
            if (const auto [have_pop, pop_idx] = PushToTailHeap(idx); have_pop) {
                if (children_[pop_idx]->Next(doc_id)) {
                    PushToHeadHeap(pop_idx);
                }
            }
        }
        if (tail_size_ + head_heap_.size() < minimum_should_match_) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        doc_id = HeadHeapTop();
        assert(doc_id != INVALID_ROWID);
        while (HeadHeapTop() == doc_id) {
            lead_.push_back(PopFromHeadHeap());
        }
        if (lead_.size() >= minimum_should_match_) {
            doc_id_ = doc_id;
            return true;
        }
        while (lead_.size() + tail_size_ >= minimum_should_match_) {
            // advance tail
            if (const auto tail_idx = PopFromTailHeap(); children_[tail_idx]->Next(doc_id)) {
                if (children_[tail_idx]->DocID() > doc_id) {
                    PushToHeadHeap(tail_idx);
                } else if (lead_.push_back(tail_idx); lead_.size() >= minimum_should_match_) {
                    doc_id_ = doc_id;
                    return true;
                }
            }
        }
    }
}

float MinimumShouldMatchIterator::BM25Score() {
    if (bm25_score_cache_docid_ == doc_id_) {
        return bm25_score_cache_;
    }
    while (tail_size_) {
        // advance tail
        if (const auto tail_idx = PopFromTailHeap(); children_[tail_idx]->Next(doc_id_)) {
            if (children_[tail_idx]->DocID() == doc_id_) {
                lead_.push_back(tail_idx);
            } else {
                PushToHeadHeap(tail_idx);
            }
        }
    }
    float sum_score = 0;
    for (const auto idx : lead_) {
        sum_score += children_[idx]->BM25Score();
    }
    bm25_score_cache_docid_ = doc_id_;
    bm25_score_cache_ = sum_score;
    return sum_score;
}

u32 MinimumShouldMatchIterator::MatchCount() const {
    UnrecoverableError("Unreachable code");
    return {};
}

RowID MinimumShouldMatchIterator::HeadHeapTop() const {
    if (head_heap_.empty()) {
        return INVALID_ROWID;
    }
    return children_[head_heap_.front()]->DocID();
}

void MinimumShouldMatchIterator::PushToHeadHeap(const u32 idx) {
    head_heap_.push_back(idx);
    std::push_heap(head_heap_.begin(), head_heap_.end(), [&](const u32 lhs, const u32 rhs) {
        return children_[lhs]->DocID() > children_[rhs]->DocID();
    });
}

u32 MinimumShouldMatchIterator::PopFromHeadHeap() {
    assert(!head_heap_.empty());
    std::pop_heap(head_heap_.begin(), head_heap_.end(), [&](const u32 lhs, const u32 rhs) {
        return children_[lhs]->DocID() > children_[rhs]->DocID();
    });
    const u32 idx = head_heap_.back();
    head_heap_.pop_back();
    return idx;
}

Pair<bool, u32> MinimumShouldMatchIterator::PushToTailHeap(const u32 idx) {
    auto comp = [&](const u32 lhs, const u32 rhs) { return children_[lhs]->GetDF() > children_[rhs]->GetDF(); };
    if (tail_size_ < tail_heap_.size()) {
        tail_heap_[tail_size_++] = idx;
        std::push_heap(tail_heap_.begin(), tail_heap_.begin() + tail_size_, comp);
        return {false, std::numeric_limits<u32>::max()};
    }
    if (children_[idx]->GetDF() <= children_[tail_heap_.front()]->GetDF()) {
        return {true, idx};
    }
    const auto result = tail_heap_.front();
    std::pop_heap(tail_heap_.begin(), tail_heap_.end(), comp);
    tail_heap_.back() = idx;
    std::push_heap(tail_heap_.begin(), tail_heap_.end(), comp);
    return {true, result};
}

u32 MinimumShouldMatchIterator::PopFromTailHeap() {
    assert(tail_size_ > 0);
    std::pop_heap(tail_heap_.begin(), tail_heap_.begin() + tail_size_, [&](const u32 lhs, const u32 rhs) {
        return children_[lhs]->GetDF() > children_[rhs]->GetDF();
    });
    return tail_heap_[--tail_size_];
}

} // namespace infinity

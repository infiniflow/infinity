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
module and_iterator;

import stl;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

AndIterator::AndIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiDocIterator(std::move(iterators)) {
    std::sort(children_.begin(), children_.end(), [](const auto &lhs, const auto &rhs) { return lhs->GetDF() < rhs->GetDF(); });
    // init df
    doc_freq_ = std::numeric_limits<u32>::max();
    bm25_score_upper_bound_ = 0.0f;
    for (SizeT i = 0; i < children_.size(); i++) {
        const auto &it = children_[i];
        doc_freq_ = std::min(doc_freq_, it->GetDF());
        bm25_score_upper_bound_ += children_[i]->BM25ScoreUpperBound();
    }
}

bool AndIterator::Next(RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    RowID target_doc_id = doc_id;
    do {
        for (SizeT i = 0; i < children_.size(); i++) {
            const auto &it = children_[i];
            bool ok = it->Next(target_doc_id);
            if (!ok) {
                doc_id_ = INVALID_ROWID;
                return false;
            }
            target_doc_id = it->DocID();
        }
        if (target_doc_id == INVALID_ROWID) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        if (target_doc_id == children_[0]->DocID()) {
            if (threshold_ <= 0.0f) {
                doc_id_ = target_doc_id;
                return true;
            }
            float sum_score = 0.0f;
            for (SizeT i = 0; i < children_.size(); i++) {
                const auto &it = children_[i];
                sum_score += it->BM25Score();
            }
            if (sum_score > threshold_) {
                doc_id_ = target_doc_id;
                bm25_score_cache_ = sum_score;
                bm25_score_cache_docid_ = doc_id_;
                return true;
            }
            target_doc_id++;
        }
    } while (1);
}

float AndIterator::BM25Score() {
    if (bm25_score_cache_docid_ == doc_id_) {
        return bm25_score_cache_;
    }
    float sum_score = 0.0f;
    for (SizeT i = 0; i < children_.size(); i++) {
        const auto &it = children_[i];
        sum_score += it->BM25Score();
    }
    bm25_score_cache_docid_ = doc_id_;
    bm25_score_cache_ = sum_score;
    return sum_score;
}

void AndIterator::UpdateScoreThreshold(float threshold) {
    if (threshold <= threshold_)
        return;
    threshold_ = threshold;
    const float base_threshold = threshold - BM25ScoreUpperBound();
    for (SizeT i = 0; i < children_.size(); i++) {
        const auto &it = children_[i];
        float new_threshold = std::max(0.0f, base_threshold + it->BM25ScoreUpperBound());
        it->UpdateScoreThreshold(new_threshold);
    }
}

} // namespace infinity

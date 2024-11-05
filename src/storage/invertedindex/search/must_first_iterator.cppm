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

export module must_first_iterator;

import stl;
import index_defines;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

export class MustFirstIterator final : public MultiDocIterator {
public:
    MustFirstIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiDocIterator(std::move(iterators)) {
        bm25_score_upper_bound_ = {};
        estimate_iterate_cost_ = children_[0]->GetEstimateIterateCost();
        for (u32 i = 0; i < children_.size(); ++i) {
            bm25_score_upper_bound_ += children_[i]->BM25ScoreUpperBound();
        }
    }

    DocIteratorType GetType() const override { return DocIteratorType::kMustFirstIterator; }

    String Name() const override { return "MustFirstIterator"; }

    bool Next(const RowID doc_id) override {
        children_[0]->Next(doc_id);
        const auto current_doc_id = children_[0]->DocID();
        if (current_doc_id == INVALID_ROWID) [[unlikely]] {
            doc_id_ = current_doc_id;
            return false;
        }
        // now current_doc_id < INVALID_ROWID
        for (u32 i = 1; i < children_.size(); ++i) {
            children_[i]->Next(current_doc_id);
        }
        doc_id_ = current_doc_id;
        return true;
    }

    float Score() override {
        if (score_cache_docid_ == doc_id_) {
            return score_cache_;
        }
        float sum_score = 0;
        for (u32 i = 0; i < children_.size(); ++i) {
            const auto &child = children_[i];
            if (child->DocID() == doc_id_) {
                sum_score += child->Score();
            }
        }
        score_cache_docid_ = doc_id_;
        score_cache_ = sum_score;
        return sum_score;
    }

    void UpdateScoreThreshold(const float threshold) override {
        if (threshold <= threshold_)
            return;
        threshold_ = threshold;
        const float base_threshold = threshold - BM25ScoreUpperBound();
        for (u32 i = 0; i < children_.size(); ++i) {
            const auto &child = children_[i];
            const float new_threshold = std::max(0.0f, base_threshold + child->BM25ScoreUpperBound());
            child->UpdateScoreThreshold(new_threshold);
        }
    }

    u32 MatchCount() const override {
        u32 count = 0;
        if (const auto current_doc_id = DocID(); current_doc_id != INVALID_ROWID) {
            for (u32 i = 0; i < children_.size(); ++i) {
                const auto &child = children_[i];
                if (child->DocID() == current_doc_id) {
                    count += child->MatchCount();
                }
            }
        }
        return count;
    }

private:
    // score cache
    RowID score_cache_docid_ = INVALID_ROWID;
    float score_cache_ = 0.0f;
};

} // namespace infinity

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

export module keyword_iterator;

import stl;
import index_defines;
import doc_iterator;
import multi_doc_iterator;
import or_iterator;
import internal_types;

namespace infinity {

export class KeywordIterator final : public MultiDocIterator {
public:
    KeywordIterator(Vector<UniquePtr<DocIterator>> iterators, const float weight) : MultiDocIterator(std::move(iterators)), weight_(weight) {}

    DocIteratorType GetType() const override { return DocIteratorType::kKeywordIterator; }

    String Name() const override { return "KeywordIterator"; }

    /* pure virtual methods implementation */
    bool Next(const RowID doc_id) override {
        if (doc_id_ == INVALID_ROWID) {
            for (u32 i = 0; i < children_.size(); ++i) {
                children_[i]->Next();
                DocIteratorEntry entry = {children_[i]->DocID(), i};
                heap_.AddEntry(entry);
            }
            heap_.BuildHeap();
            doc_id_ = heap_.TopEntry().doc_id_;
        }
        if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id) {
            return true;
        }
        while (doc_id > heap_.TopEntry().doc_id_) {
            DocIterator *top = children_[heap_.TopEntry().entry_id_].get();
            top->Next(doc_id);
            heap_.TopEntry().doc_id_ = top->DocID();
            heap_.AdjustDown(1);
        }
        doc_id_ = heap_.TopEntry().doc_id_;
        return doc_id_ != INVALID_ROWID;
    }

    float Score() override { return weight_; }

    void UpdateScoreThreshold(float threshold) override { /* do nothing */ }

    u32 MatchCount() const override { return 0; }

private:
    const float weight_ = 1.0f;
    DocIteratorHeap heap_{};
};

} // namespace infinity

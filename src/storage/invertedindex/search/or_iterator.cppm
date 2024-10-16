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

export module or_iterator;

import stl;
import index_defines;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

export struct DocIteratorEntry {
    RowID doc_id_{INVALID_ROWID};
    u32 entry_id_{0};
};

export struct DocIteratorHeap {
public:
    DocIteratorHeap() { iterator_heap_.resize(1); }

    void AddEntry(const DocIteratorEntry &entry) { iterator_heap_.push_back(entry); }

    void BuildHeap();

    void AdjustDown(SizeT idx);

    DocIteratorEntry &TopEntry() { return iterator_heap_[1]; }

public:
    Vector<DocIteratorEntry> iterator_heap_; // children begin with 1
};

export class OrIterator : public MultiDocIterator {
public:
    OrIterator(Vector<UniquePtr<DocIterator>> iterators);

    DocIteratorType GetType() const override { return DocIteratorType::kOrIterator; }

    String Name() const override { return "OrIterator"; }

    /* pure virtual methods implementation */
    bool Next(RowID doc_id) override;

    float BM25Score() override;

    void UpdateScoreThreshold(float threshold) override;

    u32 MatchCount() const override;

private:
    DocIterator *GetDocIterator(u32 i) { return children_[i].get(); }

    const DocIterator *GetDocIterator(u32 i) const { return children_[i].get(); }

    DocIteratorHeap heap_;
    // bm25 score cache
    RowID bm25_score_cache_docid_ = INVALID_ROWID;
    float bm25_score_cache_ = 0.0f;
};
} // namespace infinity
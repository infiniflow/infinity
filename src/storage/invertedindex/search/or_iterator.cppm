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
import posting_iterator;
import index_defines;
import doc_iterator;
import multi_query_iterator;
import priority_queue;
namespace infinity {

export class OrIterator : public MultiQueryDocIterator {
public:
    class DocIteratorHeap : public PriorityQueue<DocIterator *> {
    public:
        DocIteratorHeap(u32 size) { Initialize(size); }

    protected:
        bool LessThan(DocIterator *iter1, DocIterator *iter2) override { return iter1->Doc() < iter2->Doc(); }
    };

    struct DocIteratorEntry {
        docid_t doc_id_{INVALID_DOCID};
        u32 entry_id_{0};
    };

    OrIterator(Vector<UniquePtr<DocIterator>> iterators);

    virtual ~OrIterator();

    bool IsOr() const override { return true; }

    void DoSeek(docid_t doc_id) override;

    u32 GetDF() const override;

private:
    DocIterator *GetDocIterator(u32 i) { return children_[i].get(); }

    void AdjustDown() {
        DocIteratorEntry *heap = iterator_heap_.data();
        u32 idx = 1;
        u32 min = idx;
        do {
            idx = min;
            u32 left = idx << 1;
            u32 right = left + 1;
            if (left <= count_ && heap[left].doc_id_ < heap[min].doc_id_) {
                min = left;
            }
            if (right <= count_ && heap[right].doc_id_ < heap[min].doc_id_) {
                min = right;
            }
            if (min != idx) {
                std::swap(heap[idx], heap[min]);
            }
        } while (min != idx);
    }

    Vector<DocIteratorEntry> iterator_heap_;
    u32 count_{0};
};
} // namespace infinity
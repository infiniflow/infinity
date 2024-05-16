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

module or_iterator;
import internal_types;

import stl;
import index_defines;
import multi_query_iterator;
import doc_iterator;
namespace infinity {
OrIterator::OrIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiQueryDocIterator(std::move(iterators)) {
    count_ = children_.size();
    iterator_heap_.resize(children_.size() + 1);
    for (u32 i = 0; i < children_.size(); ++i) {
        iterator_heap_[i + 1].doc_id_ = children_[i]->Doc();
        iterator_heap_[i + 1].entry_id_ = i;
    }
    // Build the heap
    for (u32 i = children_.size() / 2; i > 0; --i) {
        AdjustDown(i);
    }
    doc_id_ = iterator_heap_[1].doc_id_;
    // init df
    or_iterator_df_ = std::accumulate(children_.begin(), children_.end(), 0, [](u32 sum, const UniquePtr<DocIterator> &iter) -> u32 {
        return sum + iter->GetDF();
    });
}

void OrIterator::DoSeek(RowID id) {
    while (id > iterator_heap_[1].doc_id_) {
        DocIterator *top = GetDocIterator(iterator_heap_[1].entry_id_);
        top->Seek(id);
        iterator_heap_[1].doc_id_ = top->Doc();
        AdjustDown(1);
    }
    doc_id_ = iterator_heap_[1].doc_id_;
}

} // namespace infinity
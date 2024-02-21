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

import stl;
import index_defines;
import index_config;
import multi_query_iterator;
import doc_iterator;
namespace infinity {
OrIterator::OrIterator(Vector<UniquePtr<DocIterator>> iterators) { children_ = std::move(iterators); }

OrIterator::~OrIterator() {}

void OrIterator::DoSeek(docid_t id) {
    docid_t doc_id = INVALID_DOCID;
    do {
        DocIterator *top = GetDocIterator(iterator_heap_[1].entry_id_);
        top->Seek(id);
        doc_id = top->Doc();
        iterator_heap_[1].doc_id_ = doc_id;
        AdjustDown(1, count_, iterator_heap_.data());
    } while (id > iterator_heap_[1].doc_id_);
    doc_id_ = iterator_heap_[1].doc_id_;
}
} // namespace infinity
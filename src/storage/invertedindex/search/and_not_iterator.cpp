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

module and_not_iterator;

import stl;
import index_defines;
import index_config;
import multi_query_iterator;
import doc_iterator;

namespace infinity {

AndNotIterator::AndNotIterator(Vector<UniquePtr<DocIterator>> iterators) { children_ = std::move(iterators); }

AndNotIterator::~AndNotIterator() {}

void AndNotIterator::DoSeek(docid_t doc_id) {
    if (!children_[0]->Seek(doc_id)) {
        // not match in positive child
        return;
    }
    for (u32 i = 1; i < children_.size(); ++i) {
        if (children_[i]->Seek(doc_id)) {
            // match in negative child
            return;
        }
    }
    doc_id_ = doc_id;
}

u32 AndNotIterator::GetDF() const { return children_[0]->GetDF(); }
} // namespace infinity
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
import multi_query_iterator;
import doc_iterator;
import internal_types;

namespace infinity {

AndNotIterator::AndNotIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiQueryDocIterator(std::move(iterators)) {
    // initialize doc_id_ to first valid doc
    DoSeek(0);
}

void AndNotIterator::DoSeek(RowID doc_id) {
    bool next_loop = false;
    do {
        children_[0]->Seek(doc_id);
        doc_id = children_[0]->Doc();
        if (doc_id == INVALID_ROWID) [[unlikely]] {
            break;
        }
        // now doc_id < INVALID_ROWID
        next_loop = false;
        for (u32 i = 1; i < children_.size(); ++i) {
            children_[i]->Seek(doc_id);
            if (RowID doc = children_[i]->Doc(); doc == doc_id) {
                ++doc_id;
                next_loop = true;
                break;
            }
        }
    } while (next_loop);
    doc_id_ = doc_id;
}

u32 AndNotIterator::GetDF() const { return children_[0]->GetDF(); }
} // namespace infinity
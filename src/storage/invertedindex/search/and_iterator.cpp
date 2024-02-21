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

module and_iterator;

import stl;
import doc_iterator;

namespace infinity {

AndIterator::AndIterator(Vector<UniquePtr<DocIterator>> iterators) {
    children_ = std::move(iterators);
    sorted_iterators_.reserve(children_.size());
    for (u32 i = 0; i < children_.size(); ++i) {
        sorted_iterators_.push_back(children_[i].get());
    }
}

AndIterator::~AndIterator() {}

void AndIterator::DoSeek(docid_t doc_id) {
    DocIterator **first_iter = &sorted_iterators_[0];
    DocIterator **current_iter = first_iter;
    DocIterator **end_iter = first_iter + sorted_iterators_.size();
    docid_t current = doc_id;
    do {
        docid_t tmp_id = INVALID_DOCID;
        bool ret = (*current_iter)->Seek(current);
        tmp_id = doc_id_;
        if (tmp_id == INVALID_DOCID) {
            current = tmp_id;
            break;
        } else if (tmp_id != current) {
            current = tmp_id;
            current_iter = first_iter;
        } else {
            current_iter++;
            if (current_iter >= end_iter) {
                current++;
                current_iter = first_iter;
            }
        }

    } while (true);
    doc_id_ = current;
}
} // namespace infinity

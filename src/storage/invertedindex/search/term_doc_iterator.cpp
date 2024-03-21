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

module term_doc_iterator;

import stl;
import memory_pool;
import posting_iterator;
import index_defines;
import term_meta;
import doc_iterator;
import internal_types;

namespace infinity {
TermDocIterator::TermDocIterator(UniquePtr<PostingIterator> &&iter, u64 column_id, float weight)
    : column_id_(column_id), iter_(std::move(iter)), weight_(weight) {
    DoSeek(0);
}

TermDocIterator::~TermDocIterator() {}

void TermDocIterator::DoSeek(RowID doc_id) { doc_id_ = iter_->SeekDoc(doc_id); }

} // namespace infinity
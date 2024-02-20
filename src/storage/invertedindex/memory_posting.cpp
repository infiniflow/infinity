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

#include <vector>

module memory_posting;

import stl;
import memory_pool;
import file_writer;
import posting_list_format;
import index_defines;
import lock_free_vector;
import third_party;

namespace infinity {

template <bool REALTIME>
MemoryPosting<REALTIME>::MemoryPosting(MemoryPool *pool, PostingFormatOption posting_option) : doc_ids_(pool), term_freqs_(pool), positions_(pool) {}

template <bool REALTIME>
MemoryPosting<REALTIME>::~MemoryPosting() {}

template <bool REALTIME>
void MemoryPosting<REALTIME>::AddPosition(pos_t pos) {
    positions_.PushBack(pos);
    cur_tf_++;
}

template <bool REALTIME>
void MemoryPosting<REALTIME>::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    term_freqs_.PushBack(cur_tf_);
    doc_ids_.PushBack(doc_id);
    cur_tf_ = 0;
}

template class MemoryPosting<true>;
template class MemoryPosting<false>;
} // namespace infinity

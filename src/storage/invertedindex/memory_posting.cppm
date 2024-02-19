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

export module memory_posting;
import stl;
import memory_pool;
import file_writer;
import posting_list_format;
import index_defines;
import lock_free_vector;

namespace infinity {

template <bool REALTIME>
struct PostingTraits;

template <>
struct PostingTraits<false> {
    using DocIDVector = SPLockFreeVector<u32>;
    using TermFreqVector = SPLockFreeVector<u32>;
    using PositionVector = SPLockFreeVector<u32>;
};

template <>
struct PostingTraits<true> {
    using DocIDVector = MPLockFreeVector<u32>;
    using TermFreqVector = MPLockFreeVector<u32>;
    using PositionVector = MPLockFreeVector<u32>;
};

export template <bool REALTIME = false>
class MemoryPosting : public PostingTraits<REALTIME> {
public:
    MemoryPosting(MemoryPool *pool, PostingFormatOption posting_option);

    ~MemoryPosting();

    void AddPosition(pos_t pos);

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

private:
    typename PostingTraits<REALTIME>::DocIDVector doc_ids_;
    typename PostingTraits<REALTIME>::TermFreqVector term_freqs_;
    typename PostingTraits<REALTIME>::PositionVector positions_;
    u32 cur_tf_{0};
};

} // namespace infinity

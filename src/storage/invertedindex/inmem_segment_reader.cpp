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
// limitations under the License.module;

module;

module inmem_index_segment_reader;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import index_defines;
import posting_writer;
import memory_indexer;

namespace infinity {
InMemIndexSegmentReader::InMemIndexSegmentReader(MemoryIndexer *column_indexer) : posting_table_(column_indexer->GetPostingTable()) {}

PostingWriter *InMemIndexSegmentReader::GetPostingWriter(const String &term) const {
    MemoryIndexer::PostingTable::iterator iter = posting_table_->find(term);
    if (iter != posting_table_->end())
        return iter->second.get();
    else
        return nullptr;
}

bool InMemIndexSegmentReader::GetSegmentPosting(const String &term,
                                                docid_t base_doc_id,
                                                SegmentPosting &seg_posting,
                                                MemoryPool *session_pool) const {
    PostingWriter *posting_writer = GetPostingWriter(term);
    if (posting_writer == nullptr) {
        return false;
    }
    seg_posting.Init(base_doc_id, posting_writer);
    return true;
}

} // namespace infinity
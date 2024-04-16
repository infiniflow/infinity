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
import third_party;

namespace infinity {
InMemIndexSegmentReader::InMemIndexSegmentReader(MemoryIndexer *memory_indexer)
    : posting_table_(memory_indexer->GetPostingTable()), base_row_id_(memory_indexer->GetBaseRowId()) {}

bool InMemIndexSegmentReader::GetSegmentPosting(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool,  bool fetch_position) const {
    SharedPtr<PostingWriter> writer;
    bool found = posting_table_->store_.Get(term, writer);
    if (found) {
        seg_posting.Init(base_row_id_, writer);
        return true;
    }
    return false;
}

} // namespace infinity
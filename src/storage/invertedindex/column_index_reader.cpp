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

module column_index_reader;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import disk_index_segment_reader;
import inmem_index_segment_reader;
import memory_indexer;
import dict_reader;
import posting_list_format;
import internal_types;

namespace infinity {
ColumnIndexReader::ColumnIndexReader() {}

void ColumnIndexReader::Open(const String &index_dir,
                             const Vector<String> &base_names,
                             const Vector<RowID> &base_rowids,
                             optionflag_t flag,
                             MemoryIndexer *memory_indexer) {
    flag_ = flag;
    for (SizeT i = 0; i < base_names.size(); i++) {
        SharedPtr<DiskIndexSegmentReader> segment_reader = MakeShared<DiskIndexSegmentReader>(index_dir, base_names[i], base_rowids[i], flag);
        segment_readers_.push_back(segment_reader);
    }
    if (memory_indexer != nullptr && memory_indexer->GetDocCount() != 0) {
        SharedPtr<InMemIndexSegmentReader> segment_reader = MakeShared<InMemIndexSegmentReader>(memory_indexer);
        segment_readers_.push_back(segment_reader);
    }
}

PostingIterator *ColumnIndexReader::Lookup(const String &term, MemoryPool *session_pool) {
    SharedPtr<Vector<SegmentPosting>> seg_postings = MakeShared<Vector<SegmentPosting>>();
    for (u32 i = 0; i < segment_readers_.size(); ++i) {
        SegmentPosting seg_posting;
        auto ret = segment_readers_[i]->GetSegmentPosting(term, seg_posting, session_pool);
        if (ret) {
            seg_postings->push_back(seg_posting);
        }
    }
    if (seg_postings->empty())
        return nullptr;
    PostingIterator *iter = new PostingIterator(PostingFormatOption(flag_), session_pool);
    u32 state_pool_size = 0; // TODO
    iter->Init(seg_postings, state_pool_size);
    return iter;
}
} // namespace infinity

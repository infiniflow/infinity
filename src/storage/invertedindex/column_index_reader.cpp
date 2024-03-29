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
import segment_index_entry;
import infinity_exception;

namespace infinity {
ColumnIndexReader::ColumnIndexReader() {}

void ColumnIndexReader::Open(optionflag_t flag, String &&index_dir, Map<SegmentID, SharedPtr<SegmentIndexEntry>> &&index_by_segment) {
    flag_ = flag;
    index_dir_ = std::move(index_dir);
    index_by_segment_ = std::move(index_by_segment);
    // need to ensure that segment_id is in ascending order
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        auto [base_names, base_row_ids, memory_indexer] = segment_index_entry->GetFullTextIndexSnapshot();
        // segment_readers
        for (u32 i = 0; i < base_names.size(); ++i) {
            SharedPtr<DiskIndexSegmentReader> segment_reader = MakeShared<DiskIndexSegmentReader>(index_dir_, base_names[i], base_row_ids[i], flag);
            segment_readers_.push_back(std::move(segment_reader));
        }
        // for loading column length files
        base_names_.insert(base_names_.end(), std::move_iterator(base_names.begin()), std::move_iterator(base_names.end()));
        base_row_ids_.insert(base_row_ids_.end(), base_row_ids.begin(), base_row_ids.end());
        if (memory_indexer and memory_indexer->GetDocCount() != 0) {
            // segment_reader
            SharedPtr<InMemIndexSegmentReader> segment_reader = MakeShared<InMemIndexSegmentReader>(memory_indexer);
            segment_readers_.push_back(std::move(segment_reader));
            // for loading column length file
            base_names_.push_back(memory_indexer->GetBaseName());
            base_row_ids_.push_back(memory_indexer->GetBaseRowId());
        }
    }
    // put an INVALID_ROWID at the end of base_row_ids_
    base_row_ids_.emplace_back(INVALID_ROWID);
}

UniquePtr<PostingIterator> ColumnIndexReader::Lookup(const String &term, MemoryPool *session_pool) {
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
    auto iter = MakeUnique<PostingIterator>(flag_, session_pool);
    u32 state_pool_size = 0; // TODO
    iter->Init(seg_postings, state_pool_size);
    return iter;
}

float ColumnIndexReader::GetAvgColumnLength() const {
    u64 column_len_sum = 0;
    u32 column_len_cnt = 0;
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        auto [sum, cnt] = segment_index_entry->GetFulltextColumnLenInfo();
        column_len_sum += sum;
        column_len_cnt += cnt;
    }
    if (column_len_cnt == 0) {
        UnrecoverableError("column_len_cnt is 0");
    }
    return static_cast<float>(column_len_sum) / column_len_cnt;
}

} // namespace infinity

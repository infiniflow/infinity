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

#include <cassert>
#include <string>
#include <vector>
module column_length_io;

import stl;
import column_index_reader;
import chunk_index_entry;
import memory_indexer;
import buffer_obj;
import buffer_handle;

namespace infinity {

FullTextColumnLengthReader::FullTextColumnLengthReader(ColumnIndexReader *reader)
    : index_dir_(reader->index_dir_), chunk_index_entries_(reader->chunk_index_entries_),
      memory_indexer_(reader->memory_indexer_) {
    Pair<u64, float> df_and_avg_column_len = reader->GetTotalDfAndAvgColumnLength();
    total_df_ = df_and_avg_column_len.first;
    avg_column_len_ = df_and_avg_column_len.second;
}

u32 FullTextColumnLengthReader::SeekFile(RowID row_id) {
    // determine the ChunkIndexEntry which contains row_id
    current_chunk_buffer_handle_.~BufferHandle();
    SizeT left = 0;
    SizeT right = chunk_index_entries_.size();
    SizeT current_chunk = std::numeric_limits<SizeT>::max();
    while (left < right) {
        SizeT mid = left + (right - left) / 2;
        if (chunk_index_entries_[mid]->base_rowid_ > row_id) {
            right = mid;
        } else if (chunk_index_entries_[mid]->base_rowid_ + chunk_index_entries_[mid]->row_count_ <= row_id) {
            left = mid + 1;
        } else {
            current_chunk = mid;
            break;
        }
    }
    if (current_chunk == std::numeric_limits<SizeT>::max()) {
        return 0;
    }

    // Load the column-length file of the ChunkIndexEntry
    current_chunk_buffer_handle_ = chunk_index_entries_[current_chunk]->GetBufferObj()->Load();
    column_lengths_ = (const u32 *)current_chunk_buffer_handle_.GetData();
    current_chunk_base_rowid_ = chunk_index_entries_[current_chunk]->base_rowid_;
    current_chunk_row_count_ = chunk_index_entries_[current_chunk]->row_count_;
    return column_lengths_[row_id - current_chunk_base_rowid_];
}
} // namespace infinity
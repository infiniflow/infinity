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
import index_defines;
import internal_types;
import infinity_exception;
import logger;
import column_index_reader;
import file_system_type;
import file_system;
import local_file_system;
import chunk_index_entry;
import memory_indexer;
import buffer_obj;
import buffer_handle;

namespace infinity {

FullTextColumnLengthReader::FullTextColumnLengthReader(UniquePtr<FileSystem> file_system,
                                                       const String &index_dir,
                                                       const Vector<SharedPtr<ChunkIndexEntry>> &chunk_index_entries,
                                                       SharedPtr<MemoryIndexer> memory_indexer)
    : file_system_(std::move(file_system)), index_dir_(index_dir), chunk_index_entries_(chunk_index_entries), memory_indexer_(memory_indexer) {}

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

void ColumnLengthReader::AppendColumnLength(IndexReader *index_reader, const Vector<u64> &column_ids, Vector<float> &avg_column_length) {
    u64 column_id = column_ids.back();
    ColumnIndexReader *reader = index_reader->GetColumnIndexReader(column_id);
    column_length_vector_.emplace_back(MakeUnique<FullTextColumnLengthReader>(MakeUnique<LocalFileSystem>(),
                                                                              reader->index_dir_,
                                                                              reader->chunk_index_entries_,
                                                                              reader->memory_indexer_));
    avg_column_length.emplace_back(reader->GetAvgColumnLength());
}

} // namespace infinity
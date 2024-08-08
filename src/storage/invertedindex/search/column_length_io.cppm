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

export module column_length_io;

import stl;
import index_defines;
import internal_types;
import chunk_index_entry;
import memory_indexer;
import buffer_obj;
import buffer_handle;
import column_index_reader;

namespace infinity {
class FileSystem;

export class FullTextColumnLengthReader {
public:
    explicit FullTextColumnLengthReader(ColumnIndexReader *reader);

    inline u32 GetColumnLength(RowID row_id) {
        if (row_id >= current_chunk_base_rowid_ && row_id < current_chunk_base_rowid_ + current_chunk_row_count_) [[likely]] {
            assert(column_lengths_ != nullptr);
            return column_lengths_[row_id - current_chunk_base_rowid_];
        }
        if (memory_indexer_.get() != nullptr) {
            RowID base_rowid = memory_indexer_->GetBaseRowId();
            u32 doc_count = memory_indexer_->GetDocCount();
            if (row_id >= base_rowid && row_id < base_rowid + doc_count) {
                return memory_indexer_->GetColumnLength(row_id - base_rowid);
            }
        }
        return SeekFile(row_id);
    }

    inline u64 GetTotalDF() const { return total_df_; }
    inline float GetAvgColumnLength() const { return avg_column_len_; }

private:
    u32 SeekFile(RowID row_id);
    UniquePtr<FileSystem> file_system_;
    const String &index_dir_;
    const Vector<SharedPtr<ChunkIndexEntry>> &chunk_index_entries_; // must in ascending order
    SharedPtr<MemoryIndexer> memory_indexer_;
    u64 total_df_;
    float avg_column_len_;
    const u32 *column_lengths_{nullptr};
    RowID current_chunk_base_rowid_{(u64)0};
    u32 current_chunk_row_count_{0};
    BufferHandle current_chunk_buffer_handle_{};
};

} // namespace infinity
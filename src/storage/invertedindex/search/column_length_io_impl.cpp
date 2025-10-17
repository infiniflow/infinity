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

module infinity_core:column_length_io.impl;

import :column_length_io;
import :column_index_reader;
import :memory_indexer;

import std;

import row_id;

namespace infinity {

FullTextColumnLengthReader::FullTextColumnLengthReader(ColumnIndexReader *reader)
    : index_dir_(reader->index_dir_), memory_indexer_(reader->memory_indexer_) {
    chunk_index_meta_infos_ = reader->chunk_index_meta_infos_;
    for (const auto &chunk_meta_info : chunk_index_meta_infos_) {
        chunk_doc_cnt_ += chunk_meta_info.row_cnt_;
        chunk_term_cnt_ += chunk_meta_info.term_cnt_;
    }
}

FullTextColumnLengthReader::~FullTextColumnLengthReader() = default;

u32 FullTextColumnLengthReader::SeekFile(RowID row_id) {
    // determine the chunk index which contains row_id
    // current_chunk_buffer_handle_.~BufferHandle();
    size_t left = 0;
    size_t right = chunk_index_meta_infos_.size();
    size_t current_chunk = std::numeric_limits<size_t>::max();
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (chunk_index_meta_infos_[mid].base_rowid_ > row_id) {
            right = mid;
        } else if (chunk_index_meta_infos_[mid].base_rowid_ + chunk_index_meta_infos_[mid].row_cnt_ <= row_id) {
            left = mid + 1;
        } else {
            current_chunk = mid;
            break;
        }
    }
    if (current_chunk == std::numeric_limits<size_t>::max()) {
        return 0;
    }

    // Load the column-length file of the chunk index
    current_chunk_buffer_obj_ = chunk_index_meta_infos_[current_chunk].index_buffer_;
    column_lengths_ = (const u32 *)current_chunk_buffer_obj_->GetData();
    current_chunk_base_rowid_ = chunk_index_meta_infos_[current_chunk].base_rowid_;
    current_chunk_row_count_ = chunk_index_meta_infos_[current_chunk].row_cnt_;
    return column_lengths_[row_id - current_chunk_base_rowid_];
}

std::pair<size_t, size_t> FullTextColumnLengthReader::GetDocTermCount() const {
    if (memory_indexer_ != nullptr) {
        std::pair<size_t, size_t> doc_cnt_len = memory_indexer_->GetDocTermCount();
        doc_cnt_len.first += chunk_doc_cnt_;
        doc_cnt_len.second += chunk_term_cnt_;
        return doc_cnt_len;
    } else {
        return {chunk_doc_cnt_, chunk_term_cnt_};
    }
}
} // namespace infinity
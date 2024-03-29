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

namespace infinity {

FullTextColumnLengthFileHandler::FullTextColumnLengthFileHandler(UniquePtr<FileSystem> file_system,
                                                                 const String &path,
                                                                 SegmentIndexEntry *segment_index_entry)
    : file_system_(std::move(file_system)), segment_index_entry_(segment_index_entry) {
    u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
    file_handler_ = file_system_->OpenFile(path, file_flags, FileLockType::kNoLock);
}

FullTextColumnLengthFileHandler::~FullTextColumnLengthFileHandler() {
    std::lock_guard<std::mutex> lock(mutex_);
    file_handler_->Sync();
    file_handler_->Close();
}

void FullTextColumnLengthFileHandler::WriteColumnLength(const u32 *column_length_array, u32 column_length_count, u32 start_from_offset) {
    // 1. update column length info in segment index entry
    u64 total_column_len_sum = 0;
    for (u32 i = 0; i < column_length_count; i++) {
        total_column_len_sum += column_length_array[i];
    }
    segment_index_entry_->UpdateFulltextColumnLenInfo(total_column_len_sum, column_length_count);
    // 2. write column length info to file
    std::lock_guard<std::mutex> lock(mutex_);
    file_system_->Seek(*file_handler_, start_from_offset * sizeof(u32));
    u32 expect_write_count = column_length_count * sizeof(u32);
    i64 write_count = file_system_->Write(*file_handler_, column_length_array, expect_write_count);
    if (write_count != expect_write_count) {
        UnrecoverableError("WriteColumnLength: write_count != expect_write_count");
    }
    file_handler_->Sync();
}

FullTextColumnLengthUpdateJob::FullTextColumnLengthUpdateJob(SharedPtr<FullTextColumnLengthFileHandler> file_handler,
                                                             u32 column_length_count,
                                                             u32 start_from_offset,
                                                             std::shared_mutex &memory_indexer_mutex,
                                                             Vector<u32> &memory_indexer_array)
    : file_handler_(std::move(file_handler)), column_length_count_(column_length_count), start_from_offset_(start_from_offset),
      memory_indexer_mutex_(memory_indexer_mutex), memory_indexer_array_(memory_indexer_array) {
    column_length_array_ = MakeUniqueForOverwrite<u32[]>(column_length_count);
}

void FullTextColumnLengthUpdateJob::DumpToFile() {
    // 1. update column length info in memory indexer
    {
        std::unique_lock lock(memory_indexer_mutex_);
        if (memory_indexer_array_.size() < start_from_offset_ + column_length_count_) {
            memory_indexer_array_.resize(start_from_offset_ + column_length_count_);
        }
        Copy(column_length_array_.get(), column_length_array_.get() + column_length_count_, memory_indexer_array_.data() + start_from_offset_);
    }
    // 2. write to file
    file_handler_->WriteColumnLength(column_length_array_.get(), column_length_count_, start_from_offset_);
    file_handler_.reset();
    column_length_array_.reset();
}

FullTextColumnLengthReader::FullTextColumnLengthReader(UniquePtr<FileSystem> file_system,
                                                       const String &index_dir,
                                                       const Vector<String> &base_names,
                                                       const Vector<RowID> &base_row_ids)
    : file_system_(std::move(file_system)), index_dir_(index_dir), base_names_(base_names), base_row_ids_(base_row_ids) {}

void FullTextColumnLengthReader::SeekFile(RowID row_id) {
    while (base_row_ids_[next_base_rowid_index_] <= row_id) {
        ++next_base_rowid_index_;
    }
    current_base_rowid_ = base_row_ids_[next_base_rowid_index_ - 1];
    next_base_rowid_ = base_row_ids_[next_base_rowid_index_];
    // load file
    Path column_length_file_base = Path(index_dir_) / base_names_[next_base_rowid_index_ - 1];
    String column_length_file_path_prefix = column_length_file_base.string();
    String column_length_file_path = column_length_file_path_prefix + LENGTH_SUFFIX;
    UniquePtr<FileHandler> file_handler = file_system_->OpenFile(column_length_file_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
    const u32 file_size = file_system_->GetFileSize(*file_handler);
    if (u32 array_len = file_size / sizeof(u32); array_len > column_length_array_capacity_) {
        column_length_array_capacity_ = std::max(array_len, std::min<u32>(2 * array_len, 1024 * 8192));
        column_length_array_ = MakeUniqueForOverwrite<u32[]>(column_length_array_capacity_);
    }
    const i64 read_count = file_system_->Read(*file_handler, column_length_array_.get(), file_size);
    file_handler->Close();
    if (read_count != file_size) {
        UnrecoverableError("SeekFile: read_count != file_size");
    }
}

void ColumnLengthReader::LoadColumnLength(RowID first_doc_id,
                                          IndexReader &index_reader,
                                          const Vector<u64> &column_ids,
                                          Vector<float> &avg_column_length) {
    column_length_vector_.clear();
    column_length_vector_.reserve(column_ids.size());
    avg_column_length.resize(column_ids.size());
    for (u32 i = 0; i < column_ids.size(); i++) {
        u64 column_id = column_ids[i];
        ColumnIndexReader *reader = index_reader.GetColumnIndexReader(column_id);
        column_length_vector_.emplace_back(MakeUnique<LocalFileSystem>(), reader->index_dir_, reader->base_names_, reader->base_row_ids_);
        column_length_vector_.back().SeekFile(first_doc_id);
        avg_column_length[i] = reader->GetAvgColumnLength();
    }
}

} // namespace infinity
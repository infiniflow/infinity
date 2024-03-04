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

#include <filesystem>
#include <string>

module column_indexer;

import stl;
import memory_pool;
import index_defines;
import index_config;

import column_vector;
import third_party;
import indexer;
import internal_types;
import local_file_system;
import file_writer;
import posting_writer;
import term_meta;
import fst;

namespace infinity {

ColumnIndexer::ColumnIndexer(u64 column_id,
                             const String directory,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool,
                             ThreadPool &thread_pool)
    : thread_pool_(thread_pool) {
    active_memory_indexer_ = MakeUnique<MemoryIndexer>(column_id, index_config, byte_slice_pool, buffer_pool, thread_pool);
    standby_memory_indexer_ = MakeUnique<MemoryIndexer>(column_id, index_config, byte_slice_pool, buffer_pool, thread_pool);
    directory_ = directory;
    std::error_code ec;
    bool path_exists = std::filesystem::exists(directory);
    if (!path_exists) {
        std::filesystem::create_directories(directory, ec);
    }
}

ColumnIndexer::~ColumnIndexer() {}

void ColumnIndexer::Insert(const ColumnVector &column_vector, u32 row_offset, u32 row_count, RowID row_id_begin) {
    active_memory_indexer_->Insert(column_vector, row_offset, row_count, row_id_begin);
}

void ColumnIndexer::Commit() { active_memory_indexer_->Commit(); }

void ColumnIndexer::Dump() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        std::swap(active_memory_indexer_, standby_memory_indexer_);
    }
    standby_memory_indexer_->WaitInflightTasks();

    Path path = Path(directory_) / std::to_string(current_segment_id_);
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + POSTING_SUFFIX;
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, posting_file, 128000);
    String dict_file = index_prefix + DICT_SUFFIX;
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, dict_file, 128000);
    MemoryIndexer::PostingTable *posting_table = standby_memory_indexer_->GetPostingTable();
    TermMetaDumper term_meta_dumpler(standby_memory_indexer_->index_config_.GetPostingFormatOption());

    String fst_file = index_prefix + DICT_SUFFIX + ".fst";
    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder builder(wtr);

    if (posting_table) {
        SizeT term_meta_offset = 0;
        for (auto it = posting_table->Begin(); it != posting_table->End(); ++it) {
            const MemoryIndexer::PostingPtr posting_writer = it.Value();
            TermMeta term_meta(posting_writer->GetDF(), posting_writer->GetTotalTF());
            posting_writer->Dump(posting_file_writer, term_meta);
            term_meta_dumpler.Dump(dict_file_writer, term_meta);
            const String &term = it.Key();
            builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
            term_meta_offset = dict_file_writer->TotalWrittenBytes();
        }
        dict_file_writer->Sync();
        builder.Finish();
        fs.AppendFile(dict_file, fst_file);
        fs.DeleteFile(fst_file);
    }
    standby_memory_indexer_->Reset();
}

} // namespace infinity

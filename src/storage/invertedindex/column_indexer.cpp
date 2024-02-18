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

ColumnIndexer::ColumnIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool) {
    active_memory_indexer_ = MakeUnique<MemoryIndexer>(indexer, this, column_id, index_config, byte_slice_pool, buffer_pool);
    index_name_ = indexer->GetDirectory();
    Path path = Path(index_name_) / std::to_string(column_id);
    index_name_ = path.string();
    std::error_code ec;
    bool path_exists = std::filesystem::exists(path);
    if (!path_exists) {
        std::filesystem::create_directories(path, ec);
    }
}

ColumnIndexer::~ColumnIndexer() {}

void ColumnIndexer::Insert(RowID row_id, String &data) { active_memory_indexer_->Insert(row_id, data); }

void ColumnIndexer::Insert(SharedPtr<ColumnVector> column_vector, RowID start_row_id) { active_memory_indexer_->Insert(column_vector, start_row_id); }

void ColumnIndexer::PreCommit() { active_memory_indexer_->PreCommit(); }

void ColumnIndexer::Commit() { active_memory_indexer_->Commit(); }

void ColumnIndexer::Dump() {
    active_memory_indexer_->DisableCommit();
    Path path = Path(index_name_) / std::to_string(current_segment_id_);
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + POSTING_SUFFIX;
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, posting_file, 128000);
    String dict_file = index_prefix + DICT_SUFFIX;
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, dict_file, 128000);
    MemoryIndexer::PostingTable *posting_table = active_memory_indexer_->GetPostingTable();
    TermMetaDumper term_meta_dumpler(active_memory_indexer_->index_config_.GetPostingFormatOption());

    String fst_file = index_prefix + DICT_SUFFIX + ".fst";
    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder builder(wtr);

    if (posting_table) {
        SizeT term_meta_offset = 0;
        for (auto it = posting_table->begin(); it.valid(); ++it) {
            const MemoryIndexer::PostingPtr posting_writer = it.getData();
            TermMeta term_meta(posting_writer->GetDF(), posting_writer->GetTotalTF());
            posting_writer->Dump(posting_file_writer, term_meta);
            term_meta_dumpler.Dump(dict_file_writer, term_meta);
            const String &term = it.getKey();
            builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
            term_meta_offset = dict_file_writer->TotalWrittenBytes();
        }
        dict_file_writer->Sync();
        builder.Finish();
        fs.AppendFile(dict_file, fst_file);
        fs.DeleteFile(fst_file);
    }
    active_memory_indexer_->Reset();
}

} // namespace infinity

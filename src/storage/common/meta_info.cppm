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

export module infinity_core:meta_info;

import :default_values;
import :infinity_type;

import std;
import std.compat;
import third_party;

import data_type;
import create_index_info;
import column_def;

namespace infinity {

export struct WalChunkIndexInfo;
export enum class SegmentStatus : u8;

export struct DatabaseInfo {
    std::shared_ptr<std::string> db_name_{};
    std::shared_ptr<std::string> db_entry_dir_{};
    std::shared_ptr<std::string> absolute_db_path_{};
    std::shared_ptr<std::string> db_comment_{};
    i64 table_count_{};
};

export struct TableInfo {
    std::shared_ptr<std::string> db_name_{};
    std::shared_ptr<std::string> table_name_{};
    std::shared_ptr<std::string> table_comment_{};
    std::shared_ptr<std::string> table_full_dir_{};
    i64 column_count_{};
    i64 segment_count_{};
    TxnTimeStamp max_commit_ts_{UNCOMMIT_TS};
    std::vector<std::shared_ptr<ColumnDef>> column_defs_{};
    std::string db_id_{};
    std::string table_id_{};
    std::string table_key_{};

public:
    u64 GetColumnIdByName(const std::string &column_name) const;
    const ColumnDef *GetColumnDefByID(ColumnID column_id) const;
    const ColumnDef *GetColumnDefByIdx(size_t idx) const;
    const ColumnDef *GetColumnDefByName(const std::string &column_name) const;
    size_t GetColumnIdxByID(ColumnID column_id) const;
};

export struct TableIndexInfo {
    std::shared_ptr<std::string> index_name_{};
    std::shared_ptr<std::string> index_comment_{};
    std::shared_ptr<std::string> index_entry_dir_{};
    i64 segment_index_count_{};
    std::shared_ptr<std::string> index_type_{};
    std::shared_ptr<std::string> index_other_params_{};
    std::shared_ptr<std::string> index_column_ids_{};
    std::shared_ptr<std::string> index_column_names_{};
};

export struct SegmentInfo {
    SegmentID segment_id_{};
    SegmentStatus status_{};
    std::shared_ptr<std::string> segment_dir_{};
    i64 block_count_{};
    i64 row_count_{};
    i64 actual_row_count_{};
    i64 row_capacity_{};
    i64 column_count_{};
    size_t storage_size_{};
    std::vector<std::string> files_{};
};

export struct BlockInfo {
    BlockID block_id_;
    std::shared_ptr<std::string> block_dir_{};
    i64 row_count_{};
    i64 row_capacity_{};
    i64 checkpoint_row_count_{};
    i64 column_count_{};
    TxnTimeStamp checkpoint_ts_{};
    size_t storage_size_{};
    std::vector<std::string> files_{};
};

export struct BlockColumnInfo {
    ColumnID column_id_;
    std::shared_ptr<DataType> data_type_{};
    std::shared_ptr<std::string> filename_{};
    i64 storage_size_{};
    size_t extra_file_count_{};
    std::vector<std::shared_ptr<std::string>> extra_file_names_{};
};

export struct TableDetail {
    std::shared_ptr<std::string> db_name_{};
    std::shared_ptr<std::string> table_name_{};
    std::shared_ptr<std::string> table_id_{};
    std::shared_ptr<std::string> table_comment_{};
    i64 column_count_{};
    i64 row_count_{};
    i64 block_count_{};
    i64 block_capacity_{};
    i64 segment_count_{};
    i64 segment_capacity_{};
};

export struct ViewDetail {
    std::shared_ptr<std::string> db_name_{};
    std::shared_ptr<std::string> view_name_{};
    i64 column_count_{};
};

export struct SegmentIndexInfo {
    SegmentID segment_id_{};
    IndexType index_type_{};
    std::shared_ptr<std::string> index_dir_{};
    size_t chunk_count_{};
    std::vector<std::string> files_{};
};

} // namespace infinity

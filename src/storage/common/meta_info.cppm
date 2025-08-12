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

export module infinity_core:meta_info;

import :stl;
import :default_values;

import third_party;

import data_type;
import create_index_info;
import column_def;

namespace infinity {

export struct WalChunkIndexInfo;
export enum class SegmentStatus : u8;

export struct DatabaseInfo {
    SharedPtr<String> db_name_{};
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> absolute_db_path_{};
    SharedPtr<String> db_comment_{};
    i64 table_count_{};
};

export struct TableInfo {
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> table_comment_{};
    SharedPtr<String> table_full_dir_{};
    i64 column_count_{};
    i64 segment_count_{};
    i64 row_count_{};
    TxnTimeStamp max_commit_ts_{UNCOMMIT_TS};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
    String db_id_{};
    String table_id_{};
    String table_key_{};

public:
    u64 GetColumnIdByName(const String &column_name) const;
    const ColumnDef *GetColumnDefByID(ColumnID column_id) const;
    const ColumnDef *GetColumnDefByIdx(SizeT idx) const;
    const ColumnDef *GetColumnDefByName(const String &column_name) const;
    SizeT GetColumnIdxByID(ColumnID column_id) const;
};

export struct TableIndexInfo {
    SharedPtr<String> index_name_{};
    SharedPtr<String> index_comment_{};
    SharedPtr<String> index_entry_dir_{};
    i64 segment_index_count_{};
    SharedPtr<String> index_type_{};
    SharedPtr<String> index_other_params_{};
    SharedPtr<String> index_column_ids_{};
    SharedPtr<String> index_column_names_{};
};

export struct SegmentInfo {
    SegmentID segment_id_{};
    SegmentStatus status_{};
    SharedPtr<String> segment_dir_{};
    i64 block_count_{};
    i64 row_count_{};
    i64 actual_row_count_{};
    i64 row_capacity_{};
    i64 column_count_{};
    SizeT storage_size_{};
    Vector<String> files_{};
};

export struct BlockInfo {
    BlockID block_id_;
    SharedPtr<String> block_dir_{};
    i64 row_count_{};
    i64 row_capacity_{};
    i64 checkpoint_row_count_{};
    i64 column_count_{};
    TxnTimeStamp checkpoint_ts_{};
    SizeT storage_size_{};
    Vector<String> files_{};
};

export struct BlockColumnInfo {
    ColumnID column_id_;
    SharedPtr<DataType> data_type_{};
    SharedPtr<String> filename_{};
    i64 storage_size_{};
    SizeT extra_file_count_{};
    Vector<SharedPtr<String>> extra_file_names_{};
};

export struct TableDetail {
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> table_comment_{};
    i64 column_count_{};
    i64 row_count_{};
    i64 block_count_{};
    i64 block_capacity_{};
    i64 segment_count_{};
    i64 segment_capacity_{};
};

export struct ViewDetail {
    SharedPtr<String> db_name_{};
    SharedPtr<String> view_name_{};
    i64 column_count_{};
};

export struct SegmentIndexInfo {
    SegmentID segment_id_{};
    IndexType index_type_{};
    SharedPtr<String> index_dir_{};
    SizeT chunk_count_{};
    Vector<String> files_{};
};

} // namespace infinity

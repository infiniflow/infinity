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

import stl;
import table_entry_type;
import column_def;
import default_values;
import data_type;
import create_index_info;
import internal_types;
import third_party;

export module meta_info;

namespace infinity {

struct WalChunkIndexInfo;

enum class SegmentStatus;

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
    TableEntryType table_entry_type_{TableEntryType::kTableEntry};
    TxnTimeStamp max_commit_ts_{UNCOMMIT_TS};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
    String db_id_{};
    String table_id_{};

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
    TableEntryType table_entry_type_;
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

export struct ChunkIndexInfo {
    String base_name_{};
    Vector<String> paths_{};
    RowID base_rowid_{};
    u32 row_count_{};

public:
    ChunkIndexInfo() = default;
    ChunkIndexInfo(const WalChunkIndexInfo &wal_chunk_info);

    nlohmann::json ToJson() const;

    static bool FromJson(const nlohmann::json &json, ChunkIndexInfo &chunk_index_info);
};

} // namespace infinity

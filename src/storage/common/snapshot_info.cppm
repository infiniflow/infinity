// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:snapshot_info;

import :status;
import :index_base;
import :meta_info;

import std.compat;
import third_party;

import row_id;
import column_def;
import command_statement;

namespace infinity {

export struct SnapshotInfo {
    // structure to represent the snapshot
    std::string snapshot_name_;
    SnapshotScope scope_;
    size_t version_{1}; // version 1, start from 0.6.0
    Status RestoreSnapshotFiles(const std::string &snapshot_dir,
                                const std::string &snapshot_name,
                                const std::vector<std::string> &files_to_restore,
                                const std::string &new_table_id_str,
                                const std::string &new_db_id_str,
                                std::vector<std::string> &restored_file_paths,
                                bool ignore_table_id = false);
};

export struct OutlineSnapshotInfo {
    std::string filepath_;
};

export struct BlockColumnSnapshotInfo {
    ColumnID column_id_;
    std::string filepath_;
    u64 last_chunk_offset_;
    std::vector<std::shared_ptr<OutlineSnapshotInfo>> outline_snapshots_;

    nlohmann::json Serialize();
    static std::shared_ptr<BlockColumnSnapshotInfo> Deserialize(const nlohmann::json &column_block_json);
};

export struct BlockSnapshotInfo {
    BlockID block_id_;
    size_t row_count_;
    size_t row_capacity_;
    std::string block_dir_;
    TxnTimeStamp create_ts_;
    std::vector<std::shared_ptr<BlockColumnSnapshotInfo>> column_block_snapshots_;
    std::string fast_rough_filter_;

    nlohmann::json Serialize();
    static std::shared_ptr<BlockSnapshotInfo> Deserialize(const nlohmann::json &block_json);
};

export struct SegmentSnapshotInfo {
    SegmentID segment_id_;
    std::string segment_dir_;
    SegmentStatus status_;
    TxnTimeStamp first_delete_ts_;
    TxnTimeStamp deprecate_ts_;
    TxnTimeStamp create_ts_;
    TxnTimeStamp row_count_;
    TxnTimeStamp actual_row_count_;
    std::vector<std::shared_ptr<BlockSnapshotInfo>> block_snapshots_;

    nlohmann::json Serialize();
    static std::shared_ptr<SegmentSnapshotInfo> Deserialize(const nlohmann::json &segment_json);
};

export struct ChunkIndexSnapshotInfo {
    ChunkID chunk_id_;
    std::string index_filename_;
    std::vector<std::string> full_text_files_; // full text files of the chunk
    std::string base_name_{};
    RowID base_row_id_{};
    size_t row_cnt_{};
    size_t index_size_{};
    nlohmann::json Serialize();
    static std::shared_ptr<ChunkIndexSnapshotInfo> Deserialize(const nlohmann::json &chunk_index_json);
};

export struct SegmentIndexSnapshotInfo {
    SegmentID segment_id_;
    std::vector<std::shared_ptr<ChunkIndexSnapshotInfo>> chunk_index_snapshots_{};
    nlohmann::json Serialize();
    static std::shared_ptr<SegmentIndexSnapshotInfo> Deserialize(const nlohmann::json &segment_index_json);
};

export struct TableIndexSnapshotInfo {
    std::shared_ptr<IndexBase> index_base_{};
    std::shared_ptr<std::string> index_dir_{};
    std::shared_ptr<std::string> index_id_str_;
    std::vector<std::shared_ptr<SegmentIndexSnapshotInfo>> segment_index_snapshots_{};
    nlohmann::json Serialize();
    static std::shared_ptr<TableIndexSnapshotInfo> Deserialize(const nlohmann::json &table_index_json);
};

export struct TableSnapshotInfo : public SnapshotInfo {
    std::string db_id_str_;
    std::string table_id_str_;
    std::string db_name_;
    std::string table_name_;
    std::string table_comment_{};

    TxnTimeStamp txn_id_{};
    // TODO: remove these two fields and figure out why they are here in the first place
    // TxnTimeStamp begin_ts_{};
    // TxnTimeStamp commit_ts_{};
    TxnTimeStamp max_commit_ts_{};
    TxnTimeStamp create_ts_{};
    std::string table_entry_dir_{}; // no longer in use
    ColumnID next_column_id_{};
    SegmentID unsealed_id_{};
    SegmentID next_segment_id_{};
    size_t row_count_{};
    std::vector<std::shared_ptr<ColumnDef>> columns_{};
    std::map<SegmentID, std::shared_ptr<SegmentSnapshotInfo>> segment_snapshots_{};
    std::map<std::string, std::shared_ptr<TableIndexSnapshotInfo>> table_index_snapshots_{};

    std::vector<std::string> GetFiles() const;
    Status Serialize(const std::string &save_path, TxnTimeStamp commit_ts);
    std::string ToString() const;
    nlohmann::json CreateSnapshotMetadataJSON() const;
    static std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> Deserialize(const std::string &snapshot_dir, const std::string &snapshot_name);
    static std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> Deserialize(const nlohmann::json &snapshot_meta_json);
};

export struct DatabaseSnapshotInfo : public SnapshotInfo {
    std::string db_id_str_;
    std::string db_name_;
    std::string db_comment_{};
    std::string db_next_table_id_str_{};
    std::vector<std::shared_ptr<TableSnapshotInfo>> table_snapshots_{};

    std::vector<std::string> GetFiles() const;
    Status Serialize(const std::string &save_path, TxnTimeStamp commit_ts);
    std::string ToString() const;
    nlohmann::json CreateSnapshotMetadataJSON() const;
    static std::tuple<std::shared_ptr<DatabaseSnapshotInfo>, Status> Deserialize(const std::string &snapshot_dir, const std::string &snapshot_name);
};

} // namespace infinity
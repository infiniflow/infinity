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

module;

export module infinity_core:snapshot_info;

import :stl;
import :status;
import command_statement;
import :index_base;
import third_party;
import column_def;
import :meta_info;
import row_id;



namespace infinity {

export struct SnapshotInfo {
    // structure to represent the snapshot
    String snapshot_name_;
    SnapshotScope scope_;
    SizeT version_{1}; // version 1, start from 0.6.0
    Status RestoreSnapshotFiles(const String& snapshot_dir, const String& snapshot_name, const Vector<String>& files_to_restore, const String& new_table_id_str, const String& new_db_id_str, Vector<String>& restored_file_paths, bool ignore_table_id = false);
};

export struct OutlineSnapshotInfo {
    String filepath_;
};

export struct BlockColumnSnapshotInfo {
    ColumnID column_id_;
    String filepath_;
    u64 last_chunk_offset_;
    Vector<SharedPtr<OutlineSnapshotInfo>> outline_snapshots_;

    nlohmann::json Serialize();
    static SharedPtr<BlockColumnSnapshotInfo> Deserialize(const nlohmann::json &column_block_json);
};

export struct BlockSnapshotInfo {
    BlockID block_id_;
    SizeT row_count_;
    SizeT row_capacity_;
    String block_dir_;
    TxnTimeStamp create_ts_;
    Vector<SharedPtr<BlockColumnSnapshotInfo>> column_block_snapshots_;
    String fast_rough_filter_;

    nlohmann::json Serialize();
    static SharedPtr<BlockSnapshotInfo> Deserialize(const nlohmann::json &block_json);
};

export struct SegmentSnapshotInfo {
    SegmentID segment_id_;
    String segment_dir_;
    SegmentStatus status_;
    TxnTimeStamp first_delete_ts_;
    TxnTimeStamp deprecate_ts_;
    TxnTimeStamp create_ts_;
    TxnTimeStamp row_count_;
    TxnTimeStamp actual_row_count_;
    Vector<SharedPtr<BlockSnapshotInfo>> block_snapshots_;

    nlohmann::json Serialize();
    static SharedPtr<SegmentSnapshotInfo> Deserialize(const nlohmann::json &segment_json);
};

export struct ChunkIndexSnapshotInfo {
    ChunkID chunk_id_;
    String index_filename_;
    Vector<String> full_text_files_; // full text files of the chunk
    String base_name_{};
    RowID base_row_id_{};
    SizeT row_cnt_{};
    SizeT index_size_{};
    nlohmann::json Serialize();
    static SharedPtr<ChunkIndexSnapshotInfo> Deserialize(const nlohmann::json &chunk_index_json);
};

export struct SegmentIndexSnapshotInfo {
    SegmentID segment_id_;
    Vector<SharedPtr<ChunkIndexSnapshotInfo>> chunk_index_snapshots_{};
    nlohmann::json Serialize();
    static SharedPtr<SegmentIndexSnapshotInfo> Deserialize(const nlohmann::json &segment_index_json);
};

export struct TableIndexSnapshotInfo {
    SharedPtr<IndexBase> index_base_{};
    SharedPtr<String> index_dir_{};
    SharedPtr<String> index_id_str_;
    Vector<SharedPtr<SegmentIndexSnapshotInfo>> segment_index_snapshots_{};
    nlohmann::json Serialize();
    static SharedPtr<TableIndexSnapshotInfo> Deserialize(const nlohmann::json &table_index_json);
};

export struct TableSnapshotInfo : public SnapshotInfo {
    String db_id_str_;
    String table_id_str_;
    String db_name_;
    String table_name_;
    String table_comment_{};

    TxnTimeStamp txn_id_{};
    // TODO: remove these two fields and figure out why they are here in the first place
    // TxnTimeStamp begin_ts_{};
    // TxnTimeStamp commit_ts_{};
    TxnTimeStamp max_commit_ts_{};
    TxnTimeStamp create_ts_{};
    String table_entry_dir_{}; // no longer in use
    ColumnID next_column_id_{};
    SegmentID unsealed_id_{};
    SegmentID next_segment_id_{};
    SizeT row_count_{};
    Vector<SharedPtr<ColumnDef>> columns_{};
    Map<SegmentID, SharedPtr<SegmentSnapshotInfo>> segment_snapshots_{};
    Map<String, SharedPtr<TableIndexSnapshotInfo>> table_index_snapshots_{};

    Vector<String> GetFiles() const;
    Status Serialize(const String &save_path, TxnTimeStamp commit_ts);
    String ToString() const;
    nlohmann::json CreateSnapshotMetadataJSON() const;
    static Tuple<SharedPtr<TableSnapshotInfo>, Status> Deserialize(const String &snapshot_dir, const String &snapshot_name);
    static Tuple<SharedPtr<TableSnapshotInfo>, Status> Deserialize(const nlohmann::json &snapshot_meta_json);
};

export struct DatabaseSnapshotInfo : public SnapshotInfo {
    String db_id_str_;
    String db_name_;
    String db_comment_{};
    String db_next_table_id_str_{};
    Vector<SharedPtr<TableSnapshotInfo>> table_snapshots_{};

    Vector<String> GetFiles() const;
    Status Serialize(const String &save_path, TxnTimeStamp commit_ts);
    String ToString() const;
    nlohmann::json CreateSnapshotMetadataJSON() const;
    static Tuple<SharedPtr<DatabaseSnapshotInfo>, Status> Deserialize(const String &snapshot_dir, const String &snapshot_name);
};

} // namespace infinity
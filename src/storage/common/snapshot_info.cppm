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

export module snapshot_info;

import stl;
import status;
import command_statement;
import index_base;
import third_party;
import column_def;

namespace infinity {

enum class SegmentStatus;
export struct SnapshotInfo {
    // structure to represent the snapshot
    String snapshot_name_;
    SnapshotScope scope_;
    SizeT version_{1}; // version 1, start from 0.6.0
};

export struct OutlineSnapshotInfo {
    String filename_;
};

export struct BlockColumnSnapshotInfo {
    ColumnID column_id_;
    String filename_;
    u64 last_chunk_offset_;
    Vector<SharedPtr<OutlineSnapshotInfo>> outline_snapshots_;

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<BlockColumnSnapshotInfo> Deserialize(std::string_view column_block_str);
};

export struct BlockSnapshotInfo {
    BlockID block_id_;
    SizeT row_count_;
    SizeT row_capacity_;
    String block_dir_;
    Vector<SharedPtr<BlockColumnSnapshotInfo>> column_block_snapshots_;
    String fast_rough_filter_;

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<BlockSnapshotInfo> Deserialize(std::string_view block_str);
};

export struct SegmentSnapshotInfo {
    SegmentID segment_id_;
    String segment_dir_;
    SegmentStatus status_;
    TxnTimeStamp first_delete_ts_;
    TxnTimeStamp deprecate_ts_;
    TxnTimeStamp row_count_;
    TxnTimeStamp actual_row_count_;
    Vector<SharedPtr<BlockSnapshotInfo>> block_snapshots_;

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<SegmentSnapshotInfo> Deserialize(std::string_view segment_str);
};

export struct ChunkIndexSnapshotInfo {
    ChunkID chunk_id_;
    String base_name_;
    Vector<String> files_;
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<ChunkIndexSnapshotInfo> Deserialize(std::string_view chunk_index_str);
};

export struct SegmentIndexSnapshotInfo {
    SegmentID segment_id_;
    Vector<SharedPtr<ChunkIndexSnapshotInfo>> chunk_index_snapshots_{};
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<SegmentIndexSnapshotInfo> Deserialize(std::string_view segment_index_str);
};

export struct TableIndexSnapshotInfo {
    SharedPtr<IndexBase> index_base_{};
    SharedPtr<String> index_dir_{};
    Map<SegmentID, SharedPtr<SegmentIndexSnapshotInfo>> index_by_segment_{};
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    static SharedPtr<TableIndexSnapshotInfo> Deserialize(std::string_view table_index_str);
};

export struct TableSnapshotInfo : public SnapshotInfo {
    String db_name_;
    String table_name_;
    String table_comment_{};

    TxnTimeStamp txn_id_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnTimeStamp max_commit_ts_{};
    String table_entry_dir_{};
    ColumnID next_column_id_{};
    SegmentID unsealed_id_{};
    SegmentID next_segment_id_{};
    SizeT row_count_{};
    Vector<SharedPtr<ColumnDef>> columns_{};
    Map<SegmentID, SharedPtr<SegmentSnapshotInfo>> segment_snapshots_{};
    Map<String, SharedPtr<TableIndexSnapshotInfo>> table_index_snapshots_{};

    Vector<String> GetFiles() const;
    void Serialize(const String &save_path);
    String ToString() const;
    static Tuple<SharedPtr<TableSnapshotInfo>, Status> Deserialize(const String &snapshot_dir, const String &snapshot_name);
};

} // namespace infinity

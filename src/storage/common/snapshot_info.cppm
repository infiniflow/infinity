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
    Vector<SharedPtr<OutlineSnapshotInfo>> outline_snapshots_;

    nlohmann::json Serialize();
};

export struct BlockSnapshotInfo {
    BlockID block_id_;
    String block_dir_;
    Vector<SharedPtr<BlockColumnSnapshotInfo>> column_block_snapshots_;

    nlohmann::json Serialize();
};

export struct SegmentSnapshotInfo {
    SegmentID segment_id_;
    String segment_dir_;
    Vector<SharedPtr<BlockSnapshotInfo>> block_snapshots_;

    nlohmann::json Serialize();
};

export struct ChunkIndexSnapshot {
    ChunkID chunk_id_;
    String filename_;
};

export struct SegmentIndexSnapshotInfo {
    Vector<ChunkIndexSnapshot> chunk_index_snapshots_{};
};

export struct TableIndexSnapshotInfo {
    String table_index_name_;
    const SharedPtr<IndexBase> index_base_{};
    const SharedPtr<String> index_dir_{};
    Map<SegmentID, SharedPtr<SegmentIndexSnapshotInfo>> index_by_segment_{};
};

export struct TableSnapshotInfo : public SnapshotInfo {
    String db_name_;
    String table_name_;
    String table_comment_{};

    TxnTimeStamp txn_id_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnTimeStamp max_commit_ts_{};
    ColumnID next_column_id_{};
    SegmentID next_segment_id_{};
    Vector<SharedPtr<ColumnDef>> columns_{};

    Map<SegmentID, SharedPtr<SegmentSnapshotInfo>> segment_snapshots_{};
    Vector<String> GetFiles() const;
    void Serialize(const String& save_path);
};

} // namespace infinity

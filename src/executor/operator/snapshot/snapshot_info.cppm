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

namespace infinity {

export struct SnapshotBrief {
    String snapshot_name_; // snapshot_name_
    SnapshotScope scope_; // system / db / table snapshot
    u64 create_time_; // when create the snapshot
    u64 commit_ts_;   // txn ts the snapshot created.
    u64 size_; // total snapshot size
};

export struct SnapshotInfo {
    // structure to represent the snapshot
    String snapshot_name_;
    String filename_;
    SnapshotScope scope_;
};

export struct ColumnBlockSnapshotInfo {
    ColumnID column_id_;
    String filename_;
};

export struct BlockSnapshotInfo {
    BlockID block_id_;
    Vector<ColumnBlockSnapshotInfo> column_block_snapshots_;
};

export struct SegmentSnapshotInfo {
    SegmentID segment_id_;
    Vector<BlockSnapshotInfo> block_snapshots_;
};

export struct ChunkIndexSnapshot {
    ChunkID chunk_id_;
    String filename_;
};

export struct SegmentIndexSnapshot {
    Vector<ChunkIndexSnapshot> chunk_index_snapshots_{};
};

export struct TableIndexSnapshot {
    String table_index_name_;
    const SharedPtr<IndexBase> index_base_{};
    const SharedPtr<String> index_dir_{};
    Map<SegmentID, SharedPtr<SegmentIndexSnapshot>> index_by_segment_{};
};

export struct TableSnapshotInfo : public SnapshotInfo {
    String db_name_;
    String table_name_;
};

} // namespace infinity

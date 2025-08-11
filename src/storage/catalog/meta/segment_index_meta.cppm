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

export module infinity_core:segment_index_meta;

import :stl;
import :status;
import third_party;
import :meta_info;
import :new_catalog;
import :snapshot_info;

namespace infinity {

class KVInstance;
class TableIndexMeeta;
struct MemIndex;
// struct SegmentIndexFtInfo;
class NewTxn;

export class SegmentIndexMeta {
public:
    SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta);

    ~SegmentIndexMeta();

    SegmentID segment_id() const { return segment_id_; }

    TableIndexMeeta &table_index_meta() const { return table_index_meta_; }

    KVInstance &kv_instance() const { return kv_instance_; }

    Status GetNextChunkID(ChunkID &chunk_id);

    Tuple<ChunkID, Status> GetNextChunkID1();

    Tuple<Vector<ChunkID> *, Status> GetChunkIDs1();

    Status GetFtInfo(SharedPtr<SegmentIndexFtInfo> &ft_info);

    Status SetChunkIDs(const Vector<ChunkID> &chunk_ids);

    Status RemoveChunkIDs(const Vector<ChunkID> &chunk_ids);

    Status AddChunkIndexID1(ChunkID chunk_id, NewTxn *new_txn);

    Status SetNextChunkID(ChunkID chunk_id);

    Status UpdateFtInfo(u64 column_len_sum, u32 column_len_cnt);

    Status SetNoMemIndex();

    Status InitSet();

    Status InitSet1();

    Status RestoreSet(const ChunkID &next_chunk_id);

    Status LoadSet();

    Status UninitSet(UsageFlag usage_flag);

    Status UninitSet1(UsageFlag usage_flag);

    SharedPtr<MemIndex> GetMemIndex();
    SharedPtr<MemIndex> PopMemIndex();
    bool HasMemIndex();

    SharedPtr<String> GetSegmentIndexDir() const;

    SharedPtr<SegmentIndexInfo> GetSegmentIndexInfo();

    Tuple<SharedPtr<SegmentIndexSnapshotInfo>, Status> MapMetaToSnapShotInfo();

private:
    Status LoadChunkIDs1();

    Status LoadNextChunkID();

    Status LoadFtInfo();

    String GetSegmentIndexTag(const String &tag);

private:
    mutable std::mutex mtx_;

    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    KVInstance &kv_instance_;
    TableIndexMeeta &table_index_meta_;
    SegmentID segment_id_{};

    Optional<Vector<ChunkID>> chunk_ids_{};
    Optional<ChunkID> next_chunk_id_{};

    SharedPtr<SegmentIndexFtInfo> ft_info_{};
};

} // namespace infinity
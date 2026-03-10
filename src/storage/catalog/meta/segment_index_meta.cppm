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

export module infinity_core:segment_index_meta;

import :status;
import :meta_info;
import :new_catalog;
import :snapshot_info;

import third_party;

namespace infinity {

class KVInstance;
class TableIndexMeta;
struct MemIndex;
class PlaidGlobalCentroids;
// struct SegmentIndexFtInfo;
class NewTxn;

export class SegmentIndexMeta {
public:
    SegmentIndexMeta(SegmentID segment_id, TableIndexMeta &table_index_meta);

    ~SegmentIndexMeta();

    SegmentID segment_id() const { return segment_id_; }

    TableIndexMeta &table_index_meta() const { return table_index_meta_; }

    KVInstance &kv_instance() const { return kv_instance_; }

    Status GetNextChunkID(ChunkID &chunk_id);
    Status SetNextChunkID(ChunkID chunk_id);
    std::tuple<ChunkID, Status> GetAndSetNextChunkID();

    std::tuple<std::vector<ChunkID> *, Status> GetChunkIDs1();

    Status SetChunkIDs(const std::vector<ChunkID> &chunk_ids);

    Status RemoveChunkIDs(const std::vector<ChunkID> &chunk_ids);

    Status AddChunkIndexID1(ChunkID chunk_id, NewTxn *new_txn);

    Status InitSet1();

    Status RestoreSet(const ChunkID &next_chunk_id);

    Status LoadSet();

    Status UninitSet1(UsageFlag usage_flag);

    std::shared_ptr<MemIndex> GetMemIndex(bool for_update = false);
    std::shared_ptr<MemIndex> PopMemIndex();

    std::shared_ptr<std::string> GetSegmentIndexDir() const;

    std::shared_ptr<SegmentIndexInfo> GetSegmentIndexInfo();

    std::tuple<std::shared_ptr<SegmentIndexSnapshotInfo>, Status> MapMetaToSnapShotInfo();

    // PLAID global centroids management
    // The global centroids are shared across all chunks in this segment
    // Once trained, they are persisted and loaded for subsequent operations
    Status SetPlaidGlobalCentroids(const std::shared_ptr<PlaidGlobalCentroids> &centroids);
    std::shared_ptr<PlaidGlobalCentroids> GetPlaidGlobalCentroids();
    Status SavePlaidGlobalCentroids();
    Status LoadPlaidGlobalCentroids();
    bool HasPlaidGlobalCentroids();

private:
    Status LoadChunkIDs1();

    Status LoadNextChunkID();

    Status LoadFtInfo();

    std::string GetSegmentIndexTag(const std::string &tag);

private:
    mutable std::mutex mtx_;

    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    KVInstance &kv_instance_;
    TableIndexMeta &table_index_meta_;
    SegmentID segment_id_{};

    std::optional<std::vector<ChunkID>> chunk_ids_{};
    std::optional<ChunkID> next_chunk_id_{};

    std::shared_ptr<SegmentIndexFtInfo> ft_info_{};

    // PLAID-specific: global centroids shared by all chunks
    // Stored in KV store for persistence
    std::shared_ptr<PlaidGlobalCentroids> plaid_global_centroids_{};
};

} // namespace infinity
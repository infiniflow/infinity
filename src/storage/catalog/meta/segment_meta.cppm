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

export module infinity_core:segment_meta;

import :status;
import :default_values;
import :new_catalog;
import :snapshot_info;
import :wal_entry;

namespace infinity {

class KVInstance;
class TableMeta;
export struct SegmentInfo;
class FastRoughFilter;

export enum class SegmentStatus : u8 {
    kUnsealed,
    kSealed,
    kCompacting,
    kNoDelete,
    kDeprecated,
};

export std::string ToString(SegmentStatus segment_status) {
    switch (segment_status) {
        case SegmentStatus::kUnsealed:
            return "Unsealed";
        case SegmentStatus::kSealed:
            return "Sealed";
        case SegmentStatus::kCompacting:
            return "Compacting";
        case SegmentStatus::kNoDelete:
            return "NoDelete";
        case SegmentStatus::kDeprecated:
            return "Deprecated";
    }
}

export class SegmentMeta {
public:
    SegmentMeta(SegmentID segment_id, TableMeta &table_meta);

    TxnTimeStamp begin_ts() const { return begin_ts_; }
    TxnTimeStamp commit_ts() const { return commit_ts_; }

    TxnTimeStamp GetCreateTimestampFromKV() const;

    KVInstance &kv_instance() { return kv_instance_; }

    TableMeta &table_meta() { return table_meta_; }

    SegmentID segment_id() const { return segment_id_; }

    size_t segment_capacity() const { return DEFAULT_SEGMENT_CAPACITY; }

    Status SetFirstDeleteTS(TxnTimeStamp first_delete_ts);

    Status InitSet();

    // Status UninitSet(UsageFlag usage_flag);

    Status UninitSet(UsageFlag usage_flag, TxnTimeStamp begin_ts);

    // New
    Status Init();

    std::pair<BlockID, Status> AddBlockID1(TxnTimeStamp commit_ts);
    Status AddBlockWithID(TxnTimeStamp commit_ts, BlockID block_id);

    Status CommitBlock(BlockID block_id, TxnTimeStamp commit_ts);

    std::tuple<std::vector<BlockID> *, Status> GetBlockIDs1();
    std::tuple<std::vector<BlockID> *, Status> GetBlockIDs1(TxnTimeStamp commit_ts);

    std::tuple<size_t, Status> GetRowCnt1();

    Status GetFirstDeleteTS(TxnTimeStamp &first_delete_ts);

    std::tuple<std::shared_ptr<SegmentInfo>, Status> GetSegmentInfo();

    Status GetFastRoughFilter(std::shared_ptr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(std::shared_ptr<FastRoughFilter> fast_rough_filter);

    std::tuple<std::shared_ptr<SegmentSnapshotInfo>, Status> MapMetaToSnapShotInfo();

    Status RestoreSet();

    Status RestoreFromSnapshot(const WalSegmentInfoV2 &segment_info, bool is_link_files = false);

private:
    Status LoadFirstDeleteTS();

    std::string GetSegmentTag(const std::string &tag) const;

private:
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    KVInstance &kv_instance_;
    TableMeta &table_meta_;
    SegmentID segment_id_;
    std::optional<std::string> segment_dir_; // TODO: check if it is no longer in use

    std::optional<std::vector<BlockID>> block_ids1_;
    std::optional<size_t> row_cnt_;

    std::optional<TxnTimeStamp> first_delete_ts_;
    std::shared_ptr<FastRoughFilter> fast_rough_filter_;

    std::mutex mtx_;
};

} // namespace infinity

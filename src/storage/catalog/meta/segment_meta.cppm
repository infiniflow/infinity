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

export module infinity_core:segment_meta;

import :stl;
import :status;
import :default_values;
import :new_catalog;
import :base_meta;

namespace infinity {

class KVInstance;
// class ColumnDef;
class TableMeeta;
export struct SegmentInfo;
class FastRoughFilter;

export enum class SegmentStatus : u8 {
    kUnsealed,
    kSealed,
    kCompacting,
    kNoDelete,
    kDeprecated,
};

export String ToString(SegmentStatus segment_status) {
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

export class SegmentMeta : public BaseMeta {
public:
    SegmentMeta(SegmentID segment_id, TableMeeta &table_meta);

    TableMeeta &table_meta() { return table_meta_; }

    SegmentID segment_id() const { return segment_id_; }

    SizeT segment_capacity() const { return DEFAULT_SEGMENT_CAPACITY; }

    Status InitSet(KVInstance *kv_instance);

    Status UninitSet(UsageFlag usage_flag, KVInstance *kv_instance, TxnTimeStamp begin_ts);

    Pair<BlockID, Status> AddBlockID1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);
    Status AddBlockWithID(KVInstance *kv_instance, TxnTimeStamp commit_ts, BlockID block_id);

    Status SetFirstDeleteTS(KVInstance *kv_instance, TxnTimeStamp first_delete_ts);

    Status CommitBlock(KVInstance *kv_instance, BlockID block_id, TxnTimeStamp commit_ts);

    Tuple<SharedPtr<String>, Status> GetSegmentDir(KVInstance *kv_instance);

    Tuple<Vector<BlockID> *, Status> GetBlockIDs1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Tuple<SizeT, Status> GetRowCnt1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status GetFirstDeleteTS(KVInstance *kv_instance, TxnTimeStamp &first_delete_ts, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<SegmentInfo>, Status> GetSegmentInfo(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status GetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> fast_rough_filter);

private:
    Status LoadFirstDeleteTS(KVInstance *kv_instance);

    String GetSegmentTag(const String &tag) const;

private:
    TableMeeta &table_meta_;
    SegmentID segment_id_;
    Optional<String> segment_dir_;

    Optional<BlockID> next_block_id_;

    Optional<Vector<BlockID>> block_ids1_;
    Optional<SizeT> row_cnt_;

    Optional<TxnTimeStamp> first_delete_ts_;
    SharedPtr<FastRoughFilter> fast_rough_filter_;

    std::mutex mtx_;
};

} // namespace infinity

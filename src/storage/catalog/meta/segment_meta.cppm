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

export module segment_meta;

import stl;
import status;
import default_values;
import new_catalog;
import base_meta;

namespace infinity {

class KVInstance;
// class ColumnDef;
class TableMeeta;
class SegmentInfo;
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

    TxnTimeStamp begin_ts() const { return begin_ts_; }
    TxnTimeStamp commit_ts() const { return commit_ts_; }

    KVInstance &kv_instance() { return kv_instance_; }

    TableMeeta &table_meta() { return table_meta_; }

    SegmentID segment_id() const { return segment_id_; }

    SizeT segment_capacity() const { return DEFAULT_SEGMENT_CAPACITY; }

    Status SetFirstDeleteTS(TxnTimeStamp first_delete_ts);

    Status InitSet();

    Status UninitSet(UsageFlag usage_flag);

    Status UninitSet(UsageFlag usage_flag, TxnTimeStamp begin_ts);

    Pair<BlockID, Status> AddBlockID1(TxnTimeStamp commit_ts);
    Status AddBlockWithID(TxnTimeStamp commit_ts, BlockID block_id);

    Status CommitBlock(BlockID block_id, TxnTimeStamp commit_ts);

    Tuple<SharedPtr<String>, Status> GetSegmentDir();

    Tuple<Vector<BlockID> *, Status> GetBlockIDs1();
    Tuple<Vector<BlockID> *, Status> GetBlockIDs1(TxnTimeStamp commit_ts);

    Tuple<SizeT, Status> GetRowCnt1();

    Status GetFirstDeleteTS(TxnTimeStamp &first_delete_ts);

    Tuple<SharedPtr<SegmentInfo>, Status> GetSegmentInfo();

    Status GetFastRoughFilter(SharedPtr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(SharedPtr<FastRoughFilter> fast_rough_filter);

private:
    Status LoadFirstDeleteTS();

    String GetSegmentTag(const String &tag) const;

private:
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    KVInstance &kv_instance_;
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

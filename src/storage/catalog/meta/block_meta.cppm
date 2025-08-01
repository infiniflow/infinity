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

export module block_meta;

import stl;
import status;
import default_values;
import meta_info;
import new_catalog;
import base_meta;

namespace infinity {

class KVInstance;
class SegmentMeta;
// struct BlockLock;
class BufferObj;
class FastRoughFilter;

export class BlockMeta : public BaseMeta {
public:
    BlockMeta(BlockID block_id, SegmentMeta &segment_meta);

    SegmentMeta &segment_meta() const { return segment_meta_; }

    BlockID block_id() const { return block_id_; }

    SizeT block_capacity() const { return DEFAULT_BLOCK_CAPACITY; }

    SharedPtr<String> GetBlockDir();

    Status GetBlockLock(SharedPtr<BlockLock> &block_lock);

    // Status SetRowCnt(SizeT row_cnt);

    Status InitSet();

    Status LoadSet(TxnTimeStamp checkpoint_ts);

    Status RestoreSet();

    Status UninitSet(KVInstance *kv_instance, UsageFlag usage_flag);

    // Tuple<SizeT, Status> GetRowCnt();

    Tuple<SizeT, Status> GetRowCnt1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Tuple<BufferObj *, Status> GetVersionBuffer();

    Vector<String> FilePaths();

    Tuple<SharedPtr<BlockInfo>, Status> GetBlockInfo(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Tuple<SharedPtr<BlockColumnInfo>, Status>
    GetBlockColumnInfo(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, ColumnID column_id);

    String GetBlockTag(const String &tag) const;

    Status GetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> fast_rough_filter);

private:
    mutable std::mutex mtx_;

    //    TxnTimeStamp begin_ts_ = 0;
    //    TxnTimeStamp commit_ts_;
    //    KVInstance &kv_instance_;
    SegmentMeta &segment_meta_;
    BlockID block_id_;
    Optional<Vector<ColumnID>> column_ids1_;

    SharedPtr<String> block_dir_;
    Optional<SizeT> row_cnt_;

    BufferObj *version_buffer_ = nullptr;
    SharedPtr<FastRoughFilter> fast_rough_filter_;
};

} // namespace infinity

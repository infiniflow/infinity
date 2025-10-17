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

export module infinity_core:block_meta;

import :status;
import :default_values;
import :meta_info;
import :new_catalog;
import :snapshot_info;

namespace infinity {

class KVInstance;
class SegmentMeta;
// struct BlockLock;
class BufferObj;
class FastRoughFilter;

export class BlockMeta {
public:
    BlockMeta(BlockID block_id, SegmentMeta &segment_meta);

    KVInstance &kv_instance() const { return kv_instance_; }

    SegmentMeta &segment_meta() const { return segment_meta_; }

    BlockID block_id() const { return block_id_; }

    size_t block_capacity() const { return DEFAULT_BLOCK_CAPACITY; }

    std::shared_ptr<std::string> GetBlockDir();

    Status GetBlockLock(std::shared_ptr<BlockLock> &block_lock);

    Status InitSet();

    Status LoadSet(TxnTimeStamp checkpoint_ts);

    Status RestoreSet();

    Status RestoreSetFromSnapshot();

    Status UninitSet(UsageFlag usage_flag);

    // std::tuple<size_t, Status> GetRowCnt();
    TxnTimeStamp GetCreateTimestampFromKV() const;

    std::tuple<size_t, Status> GetRowCnt1();

    std::tuple<BufferObj *, Status> GetVersionBuffer();

    std::vector<std::string> FilePaths();

    std::tuple<std::shared_ptr<BlockInfo>, Status> GetBlockInfo();

    std::tuple<std::shared_ptr<BlockColumnInfo>, Status> GetBlockColumnInfo(ColumnID column_id);

    std::string GetBlockTag(const std::string &tag) const;

    Status GetFastRoughFilter(std::shared_ptr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(std::shared_ptr<FastRoughFilter> fast_rough_filter);

    Status RestoreFromSnapshot();

    std::tuple<std::shared_ptr<BlockSnapshotInfo>, Status> MapMetaToSnapShotInfo();

private:
    mutable std::mutex mtx_;

    TxnTimeStamp begin_ts_ = 0;
    TxnTimeStamp commit_ts_;
    KVInstance &kv_instance_;
    SegmentMeta &segment_meta_;
    BlockID block_id_;
    std::optional<std::vector<ColumnID>> column_ids1_; // stored in columndefs in kv

    std::shared_ptr<std::string> block_dir_;
    std::optional<size_t> row_cnt_; // stored in the block version file

    BufferObj *version_buffer_ = nullptr;
    std::shared_ptr<FastRoughFilter> fast_rough_filter_;
};

} // namespace infinity

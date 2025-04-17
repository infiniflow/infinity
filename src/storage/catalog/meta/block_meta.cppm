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

namespace infinity {

class KVInstance;
class SegmentMeta;
struct BlockLock;
class BufferObj;
class FastRoughFilter;

export class BlockMeta {
public:
    BlockMeta(BlockID block_id, SegmentMeta &segment_meta);

    KVInstance &kv_instance() const { return kv_instance_; }

    SegmentMeta &segment_meta() const { return segment_meta_; }

    BlockID block_id() const { return block_id_; }

    SizeT block_capacity() const { return DEFAULT_BLOCK_CAPACITY; }

    SharedPtr<String> GetBlockDir();

    Status GetBlockLock(SharedPtr<BlockLock> &block_lock);

    // Status SetRowCnt(SizeT row_cnt);

    Status InitSet();

    Status LoadSet(TxnTimeStamp checkpoint_ts);

    Status UninitSet();

    // Tuple<SizeT, Status> GetRowCnt();

    Tuple<SizeT, Status> GetRowCnt1();

    Tuple<BufferObj *, Status> GetVersionBuffer();

    Vector<String> FilePaths();

    Tuple<SharedPtr<BlockInfo>, Status> GetBlockInfo();

    Tuple<SharedPtr<BlockColumnInfo>, Status> GetBlockColumnInfo(ColumnID column_id);

    // Pair<ColumnID, Status> AddBlockColumnID1(TxnTimeStamp commit_ts);
    //
    // Tuple<Vector<BlockID> *, Status> GetBlockColumnIDs1();

    Status GetFastRoughFilter(SharedPtr<FastRoughFilter> &fast_rough_filter);

    Status SetFastRoughFilter(SharedPtr<FastRoughFilter> fast_rough_filter);

private:
    // Status LoadRowCnt();

    Status LoadVersionBuffer();

    String GetBlockTag(const String &tag) const;

    // Status LoadBlockColumnIDs1();

private:
    TxnTimeStamp begin_ts_ = 0;
    KVInstance &kv_instance_;
    SegmentMeta &segment_meta_;
    BlockID block_id_;
    // Optional<Vector<ColumnID>> column_ids1_;

    SharedPtr<String> block_dir_;
    Optional<SizeT> row_cnt_;

    BufferObj *version_buffer_ = nullptr;
    SharedPtr<FastRoughFilter> fast_rough_filter_;
};

} // namespace infinity

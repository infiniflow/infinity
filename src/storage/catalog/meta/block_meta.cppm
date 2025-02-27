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

namespace infinity {

class KVInstance;
class SegmentMeta;
struct BlockLock;

export class BlockMeta {
public:
    BlockMeta(BlockID block_id, SegmentMeta &segment_meta, KVInstance &kv_instance);

    KVInstance &kv_instance() const { return kv_instance_; }

    SegmentMeta &segment_meta() const { return segment_meta_; }

    BlockID block_id() const { return block_id_; }

    SizeT block_capacity() const { return DEFAULT_BLOCK_CAPACITY; }

    SharedPtr<String> GetBlockDir();

    Status GetRowCnt(SizeT &row_cnt) {
        if (!row_cnt_) {
            Status status = LoadRowCnt();
            if (!status.ok()) {
                return status;
            }
        }
        row_cnt = *row_cnt_;
        return Status::OK();
    }

    Status GetBlockLock(SharedPtr<BlockLock> &block_lock);

    Status SetRowCnt(SizeT row_cnt);

    Status InitSet();

private:
    Status LoadRowCnt();

    String GetBlockTag(const String &tag) const;

private:
    KVInstance &kv_instance_;
    SegmentMeta &segment_meta_;
    BlockID block_id_;

    SharedPtr<String> block_dir_;
    Optional<SizeT> row_cnt_;
};

} // namespace infinity

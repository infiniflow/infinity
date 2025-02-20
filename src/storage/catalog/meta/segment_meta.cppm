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

namespace infinity {

class KVInstance;
class ColumnDef;

export class SegmentMeta {
public:
    SegmentMeta(SegmentID segment_id, KVInstance &kv_instance);

    KVInstance &kv_instance() { return kv_instance_; }

    SegmentID segment_id() const { return segment_id_; }

    SizeT segment_capacity() const { return DEFAULT_SEGMENT_CAPACITY; }

    Status GetBlockIDs(Vector<BlockID> *&block_ids) {
        if (!block_ids_) {
            LoadBlockIDs();
        }
        block_ids = &block_ids_.value();
        return Status::OK();
    }

    Status GetNextBlockID(BlockID &next_block_id) {
        if (!next_block_id_) {
            LoadNextBlockID();
        }
        next_block_id = *next_block_id_;
        return Status::OK();
    }

    Status GetRowCnt(SizeT &row_cnt) {
        if (!row_cnt_) {
            LoadRowCnt();
        }
        row_cnt = *row_cnt_;
        return Status::OK();
    }

    Status SetBlockIDs(const Vector<BlockID> &block_ids);

    Status AddBlockID(BlockID block_id);

    Status SetNextBlockID(BlockID next_block_id);

    Status SetRowCnt(SizeT row_cnt);

    Status InitSet();

private:
    Status LoadBlockIDs();

    Status LoadNextBlockID();

    Status LoadRowCnt();

    String GetSegmentTag(const String &tag) const;

private:
    KVInstance &kv_instance_;
    SegmentID segment_id_;

    Optional<Vector<BlockID>> block_ids_;
    Optional<BlockID> next_block_id_;
    Optional<SizeT> row_cnt_;

public:
    String db_id_str_;
    String table_id_str_;
    String table_dir_;
    Vector<SharedPtr<ColumnDef>> column_defs_;
};

} // namespace infinity

// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

import stl;
import global_block_id;

export module block_index;

namespace infinity {

class DBMeeta;
class TableMeeta;
class SegmentMeta;
class BlockMeta;
class NewTxn;
class TableIndexMeeta;
class SegmentIndexMeta;

export struct NewSegmentSnapshot {
public:
    SegmentOffset segment_offset() const;
    const Vector<UniquePtr<BlockMeta>>& block_map() const;
    UniquePtr<SegmentMeta> segment_meta_;
private:
    mutable Vector<UniquePtr<BlockMeta>> block_map_;
};

export struct BlockIndex {
public:
    BlockIndex();

    ~BlockIndex();

    void NewInit(UniquePtr<TableMeeta> table_meta);

    SizeT BlockCount() const;

    SizeT SegmentCount() const;

    BlockMeta *GetBlockMeta(u32 segment_id, u16 block_id) const;

    SegmentOffset GetSegmentOffset(SegmentID segment_id) const;

    BlockOffset GetBlockOffset(SegmentID segment_id, BlockID block_id) const;

    bool IsEmpty() const;

public:
    Vector<SharedPtr<TableIndexMeeta>> table_index_meta_map_;
    UniquePtr<TableMeeta> table_meta_;
    Map<SegmentID, NewSegmentSnapshot> new_segment_block_index_;
};

export struct NewIndexSnapshot {
    SharedPtr<TableIndexMeeta> table_index_meta_;

    Map<SegmentID, UniquePtr<SegmentIndexMeta>> segment_index_metas_;
};

export struct IndexIndex {
public:
    SharedPtr<NewIndexSnapshot> Insert(const String &index_name, SharedPtr<TableIndexMeeta> table_index_meta);

    void Insert(String index_name, SharedPtr<NewIndexSnapshot> new_index_snapshot);

    bool IsEmpty() const { return new_index_snapshots_vec_.empty(); }

public:
    HashMap<String, SharedPtr<NewIndexSnapshot>> new_index_snapshots_;
    Vector<NewIndexSnapshot *> new_index_snapshots_vec_;
};

} // namespace infinity

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

struct SegmentEntry;
struct BlockEntry;
struct TableIndexEntry;
struct SegmentIndexEntry;
class Txn;

class TableMeeta;
class SegmentMeta;
class BlockMeta;
class NewTxn;
class TableIndexMeeta;
class SegmentIndexMeta;

export struct SegmentSnapshot {
    SegmentEntry *segment_entry_{};

    Vector<BlockEntry *> block_map_;

    SegmentOffset segment_offset_ = 0;
};

export struct NewSegmentSnapshot {
public:
    SegmentOffset segment_offset() const;
    Vector<UniquePtr<BlockMeta>> block_map() const;
    UniquePtr<SegmentMeta> segment_meta_;
};

export struct BlockIndex {
public:
    BlockIndex();

    ~BlockIndex();

    void NewInit(NewTxn *new_txn, const String &db_name, const String &table_name);

    void Insert(SegmentEntry *segment_entry, Txn *txn);

    SizeT BlockCount() const;

    SizeT SegmentCount() const;

    BlockEntry *GetBlockEntry(u32 segment_id, u16 block_id) const;

    BlockMeta *GetBlockMeta(u32 segment_id, u16 block_id) const;

    SegmentOffset GetSegmentOffset(SegmentID segment_id) const;

    BlockOffset GetBlockOffset(SegmentID segment_id, BlockID block_id) const;

    bool IsEmpty() const;

public:
    Map<SegmentID, SegmentSnapshot> segment_block_index_;

    UniquePtr<TableMeeta> table_meta_;
    Map<SegmentID, NewSegmentSnapshot> new_segment_block_index_;
};

export struct IndexSnapshot {
    TableIndexEntry *table_index_entry_;

    Map<SegmentID, SegmentIndexEntry *> segment_index_entries_;
};

export struct NewIndexSnapshot {
    SharedPtr<TableIndexMeeta> table_index_meta_;

    Map<SegmentID, UniquePtr<SegmentIndexMeta>> segment_index_metas_;
};

export struct IndexIndex {
public:
    SharedPtr<IndexSnapshot> Insert(TableIndexEntry *table_index_entry, Txn *txn);

    SharedPtr<NewIndexSnapshot> Insert(const String &index_name, SharedPtr<TableIndexMeeta> table_index_meta);

    void Insert(String index_name, SharedPtr<IndexSnapshot> index_snapshot);

    void Insert(String index_name, SharedPtr<NewIndexSnapshot> new_index_snapshot);

    void Insert(TableIndexEntry *table_index_entry, SegmentIndexEntry *segment_index_entry);

    bool IsEmpty() const { return index_snapshots_.empty(); }

public:
    HashMap<String, SharedPtr<IndexSnapshot>> index_snapshots_;
    Vector<IndexSnapshot *> index_snapshots_vec_;

    HashMap<String, SharedPtr<NewIndexSnapshot>> new_index_snapshots_;
    Vector<NewIndexSnapshot *> new_index_snapshots_vec_;
};

} // namespace infinity

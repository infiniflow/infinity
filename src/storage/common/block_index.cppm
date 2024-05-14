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

export struct SegmentSnapshot {
    SegmentEntry *segment_entry_{};

    Vector<BlockEntry *> block_map_;

    SegmentOffset segment_offset_ = 0;
};

export struct BlockIndex {
public:
    void Insert(SegmentEntry *segment_entry, Txn *txn);

    inline SizeT BlockCount() const {
        SizeT count = 0;
        for (const auto &[_, segment_info] : segment_block_index_) {
            count += segment_info.block_map_.size();
        }
        return count;
    }

    inline SizeT SegmentCount() const { return segment_block_index_.size(); }

    BlockEntry *GetBlockEntry(u32 segment_id, u16 block_id) const;

    SegmentOffset GetSegmentOffset(SegmentID segment_id) const;

public:
    Map<SegmentID, SegmentSnapshot> segment_block_index_;
};

export struct IndexSnapshot {
    TableIndexEntry *table_index_entry_;

    Map<SegmentID, SegmentIndexEntry *> segment_index_entries_;
};

export struct IndexIndex {
public:
    void Insert(TableIndexEntry *table_index_entry, Txn *txn);

    void Insert(String index_name, SharedPtr<IndexSnapshot> index_snapshot);

    void Insert(TableIndexEntry *table_index_entry, SegmentIndexEntry *segment_index_entry);

    bool IsEmpty() const { return index_snapshots_.empty(); }

public:
    HashMap<String, SharedPtr<IndexSnapshot>> index_snapshots_;
    Vector<IndexSnapshot *> index_snapshots_vec_;
};

} // namespace infinity

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

struct BlockEntry;
struct SegmentEntry;
class Txn;

export struct BlockIndex {
private:
    struct BlocksInfo {
        HashMap<BlockID, BlockEntry *> block_map_;
        SegmentOffset segment_offset_ = 0;
    };

public:
    void Insert(SegmentEntry *segment_entry, Txn *txn);

    void Reserve(SizeT n);

    inline SizeT BlockCount() const { return global_blocks_.size(); }

    inline SizeT SegmentCount() const { return segments_.size(); }

    BlockEntry *GetBlockEntry(u32 segment_id, u16 block_id) const;

    SegmentOffset GetSegmentOffset(SegmentID segment_id) const;

    Vector<SegmentEntry *> segments_;
    HashMap<SegmentID, SegmentEntry *> segment_index_;
    Vector<GlobalBlockID> global_blocks_;

private:
    HashMap<SegmentID, BlocksInfo> segment_block_index_;
};

} // namespace infinity

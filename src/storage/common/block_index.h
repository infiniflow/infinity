//
// Created by jinhai on 23-10-5.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/containers.h"
#include "storage/common/global_block_id.h"

namespace infinity {

class BlockEntry;
class SegmentEntry;

struct BlockIndex {
    void
    Insert(SegmentEntry* segment_entry, TxnTimeStamp timestamp);

    void
    Reserve(SizeT n);

    inline SizeT
    BlockCount() const {
        return global_blocks_.size();
    }

    inline SizeT
    SegmentCount() const {
        return segments_.size();
    }

    BlockEntry*
    GetBlockEntry(i32 segment_id, i16 block_id) const;

    Vector<SegmentEntry*> segments_;
    HashMap<i32, SegmentEntry*> segment_index_;
    HashMap<i32, HashMap<i16, BlockEntry*>> segment_block_index_;
    Vector<GlobalBlockID> global_blocks_;
};

}

//
// Created by jinhai on 23-10-5.
//

module;

import stl;
import segment_entry;
import block_entry;
import global_block_id;

module block_index;

namespace infinity {

void BlockIndex::Insert(SegmentEntry *segment_entry, TxnTimeStamp timestamp) {
    if (timestamp >= segment_entry->min_row_ts_) {
        segments_.emplace_back(segment_entry);
        segment_index_.emplace(segment_entry->segment_id_, segment_entry);
        if (segment_entry->block_entries_.empty()) {
            return;
        }
        segment_block_index_[segment_entry->segment_id_].reserve(segment_entry->block_entries_.size());
        SizeT block_count = segment_entry->block_entries_.size();
        for(SizeT idx = 0; idx < block_count; ++ idx) {
            const auto &block_entry = segment_entry->block_entries_[idx];
            if (timestamp >= block_entry->min_row_ts_) {
                segment_block_index_[segment_entry->segment_id_].emplace(block_entry->block_id_, block_entry.get());
                global_blocks_.emplace_back(GlobalBlockID{segment_entry->segment_id_, block_entry->block_id_});
            }
        }
    }
}

void BlockIndex::Reserve(SizeT n) {
    segments_.reserve(n);
    segment_index_.reserve(n);
    segment_block_index_.reserve(n);
}

BlockEntry *BlockIndex::GetBlockEntry(u32 segment_id, u16 block_id) const {
    auto seg_it = segment_block_index_.find(segment_id);
    if (seg_it != segment_block_index_.end()) {
        auto block_it = seg_it->second.find(block_id);
        if (block_it != seg_it->second.end()) {
            return block_it->second;
        }
    }
    return nullptr;
}

} // namespace infinity

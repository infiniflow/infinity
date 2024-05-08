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

module block_index;

import stl;
import segment_entry;
import global_block_id;
import block_iter;
import segment_iter;

namespace infinity {

void BlockIndex::Insert(SegmentEntry *segment_entry, TxnTimeStamp timestamp, bool check_ts) {
    if (!check_ts || segment_entry->CheckVisible(timestamp)) {
        u32 segment_id = segment_entry->segment_id();
        segments_.emplace_back(segment_entry);
        segment_index_.emplace(segment_id, segment_entry);
        BlocksInfo blocks_info;

        auto block_entry_iter = BlockEntryIter(segment_entry);
        for (auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
            if (timestamp >= block_entry->min_row_ts()) {
                blocks_info.block_map_.emplace(block_entry->block_id(), block_entry);
                global_blocks_.emplace_back(GlobalBlockID{segment_id, block_entry->block_id()});
            }
        }
        blocks_info.segment_offset_ = segment_entry->row_count(timestamp);
        // blocks_info.segment_offset_ = segment_entry->row_count(); // use false row count to pass benchmark

        segment_block_index_.emplace(segment_id, std::move(blocks_info));
    }
}

void BlockIndex::Reserve(SizeT n) {
    segments_.reserve(n);
    segment_index_.reserve(n);
    segment_block_index_.reserve(n);
}

SegmentOffset BlockIndex::GetSegmentOffset(SegmentID segment_id) const {
    auto seg_it = segment_block_index_.find(segment_id);
    if (seg_it != segment_block_index_.end()) {
        const auto &blocks_info = seg_it->second;
        return blocks_info.segment_offset_;
    }
    return 0;
}

BlockEntry *BlockIndex::GetBlockEntry(u32 segment_id, u16 block_id) const {
    auto seg_it = segment_block_index_.find(segment_id);
    if (seg_it != segment_block_index_.end()) {
        const auto &blocks_info = seg_it->second;
        auto block_it = blocks_info.block_map_.find(block_id);
        if (block_it != blocks_info.block_map_.end()) {
            return block_it->second;
        }
    }
    return nullptr;
}

} // namespace infinity

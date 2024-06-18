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

#include <vector>

module block_index;

import stl;
import segment_entry;
import global_block_id;
import txn;
import table_index_entry;
import segment_index_entry;
import default_values;

namespace infinity {

void BlockIndex::Insert(SegmentEntry *segment_entry, Txn *txn) {
    if (segment_entry->CheckVisible(txn)) {
        SegmentSnapshot segment_info;
        segment_info.segment_entry_ = segment_entry;
        {
            auto block_guard = segment_entry->GetBlocksGuard();
            for (const auto &block_entry : block_guard.block_entries_) {
                if (block_entry->CheckVisible(txn)) {
                    segment_info.block_map_.emplace_back(block_entry.get());
                }
            }
        }
        TxnTimeStamp begin_ts = txn->BeginTS();
        segment_info.segment_offset_ = segment_entry->row_count(begin_ts);

        SegmentID segment_id = segment_entry->segment_id();
        segment_block_index_.emplace(segment_id, std::move(segment_info));
    }
}

SegmentOffset BlockIndex::GetSegmentOffset(SegmentID segment_id) const {
    auto seg_it = segment_block_index_.find(segment_id);
    if (seg_it != segment_block_index_.end()) {
        const auto &blocks_info = seg_it->second;
        return blocks_info.segment_offset_;
    }
    return 0;
}

BlockOffset BlockIndex::GetBlockOffset(SegmentID segment_id, BlockID block_id) const {
    SegmentOffset segment_offset = this->GetSegmentOffset(segment_id);
    if ((SegmentOffset(block_id) + 1) * DEFAULT_BLOCK_CAPACITY <= segment_offset) {
        return DEFAULT_BLOCK_CAPACITY;
    }
    return segment_offset % DEFAULT_BLOCK_CAPACITY;
}

BlockEntry *BlockIndex::GetBlockEntry(u32 segment_id, u16 block_id) const {
    auto seg_it = segment_block_index_.find(segment_id);
    if (seg_it == segment_block_index_.end()) {
        return nullptr;
    }
    const auto &blocks_info = seg_it->second;
    if (blocks_info.block_map_.size() <= block_id) {
        return nullptr;
    }
    return blocks_info.block_map_[block_id];
}

void IndexIndex::Insert(String index_name, SharedPtr<IndexSnapshot> index_snapshot) {
    index_snapshots_vec_.push_back(index_snapshot.get());
    index_snapshots_.emplace(std::move(index_name), index_snapshot);
}

void IndexIndex::Insert(TableIndexEntry *table_index_entry, Txn *txn) {
    if (table_index_entry->CheckVisible(txn)) {
        auto index_snapshot = MakeUnique<IndexSnapshot>();
        index_snapshot->table_index_entry_ = table_index_entry;

        SegmentIndexesGuard segment_index_guard = table_index_entry->GetSegmentIndexesGuard();
        for (const auto &[segment_id, segment_index_entry] : segment_index_guard.index_by_segment_) {
            if (segment_index_entry->CheckVisible(txn)) {
                index_snapshot->segment_index_entries_.emplace(segment_id, segment_index_entry.get());
            }
        }

        String index_name = *table_index_entry->GetIndexName();
        Insert(std::move(index_name), std::move(index_snapshot));
    }
}

void IndexIndex::Insert(TableIndexEntry *table_index_entry, SegmentIndexEntry *segment_index_entry) {
    auto index_snapshot_it = index_snapshots_.find(*table_index_entry->GetIndexName());
    IndexSnapshot *index_snapshot_ptr = nullptr;
    if (index_snapshot_it == index_snapshots_.end()) {
        auto index_snapshot = MakeUnique<IndexSnapshot>();
        index_snapshot_ptr = index_snapshot.get();
        index_snapshot->table_index_entry_ = table_index_entry;
        String index_name = *table_index_entry->GetIndexName();
        Insert(std::move(index_name), std::move(index_snapshot));
    } else {
        index_snapshot_ptr = index_snapshot_it->second.get();
    }
    index_snapshot_ptr->segment_index_entries_.emplace(segment_index_entry->segment_id(), segment_index_entry);
}

} // namespace infinity

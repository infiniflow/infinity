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
import table_index_entry;
import segment_index_entry;
import default_values;

import status;
import infinity_exception;
import new_txn;
import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import kv_store;

import table_index_meeta;
import segment_index_meta;

namespace infinity {

SegmentOffset NewSegmentSnapshot::segment_offset() const {
    auto [segment_offset_, status] = segment_meta_->GetRowCnt1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    return segment_offset_;
}

const Vector<UniquePtr<BlockMeta>> &NewSegmentSnapshot::block_map() const {
    if (block_map_.size()) {
        return block_map_;
    }
    auto [block_ids_ptr, status] = segment_meta_->GetBlockIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (BlockID block_id : *block_ids_ptr) {
        auto block_meta = MakeUnique<BlockMeta>(block_id, *segment_meta_);
        block_map_.emplace_back(std::move(block_meta));
    }
    return block_map_;
}

BlockIndex::BlockIndex() = default;

BlockIndex::~BlockIndex() = default;

void BlockIndex::NewInit(UniquePtr<TableMeeta> table_meta) {
    table_meta_ = std::move(table_meta);
    Status status = Status::OK();
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta_->GetSegmentIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        NewSegmentSnapshot &segment_snapshot = new_segment_block_index_.emplace(segment_id, NewSegmentSnapshot()).first->second;
        segment_snapshot.segment_meta_ = MakeUnique<SegmentMeta>(segment_id, *table_meta_);
    }
}

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

SizeT BlockIndex::BlockCount() const {
    SizeT count = 0;
    if (!table_meta_)
        for (const auto &[_, segment_info] : segment_block_index_) {
            count += segment_info.block_map_.size();
        }
    else
        for (const auto &[_, segment_info] : new_segment_block_index_) {
            count += segment_info.block_map().size();
        }
    return count;
}

SizeT BlockIndex::SegmentCount() const {
    if (!table_meta_)
        return segment_block_index_.size();
    return new_segment_block_index_.size();
}

SegmentOffset BlockIndex::GetSegmentOffset(SegmentID segment_id) const {
    if (!table_meta_) {
        auto seg_it = segment_block_index_.find(segment_id);
        if (seg_it != segment_block_index_.end()) {
            const auto &blocks_info = seg_it->second;
            return blocks_info.segment_offset_;
        }
    } else {
        auto seg_it = new_segment_block_index_.find(segment_id);
        if (seg_it != new_segment_block_index_.end()) {
            const auto &blocks_info = seg_it->second;
            return blocks_info.segment_offset();
        }
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

bool BlockIndex::IsEmpty() const {
    if (!table_meta_)
        return segment_block_index_.empty();
    return new_segment_block_index_.empty();
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

BlockMeta *BlockIndex::GetBlockMeta(u32 segment_id, u16 block_id) const {
    auto seg_it = new_segment_block_index_.find(segment_id);
    if (seg_it == new_segment_block_index_.end()) {
        return nullptr;
    }
    const auto &blocks_info = seg_it->second;
    if (blocks_info.block_map().size() <= block_id) {
        return nullptr;
    }
    return blocks_info.block_map()[block_id].get();
}

void IndexIndex::Insert(String index_name, SharedPtr<IndexSnapshot> index_snapshot) {
    index_snapshots_vec_.push_back(index_snapshot.get());
    index_snapshots_.emplace(std::move(index_name), index_snapshot);
}

void IndexIndex::Insert(String index_name, SharedPtr<NewIndexSnapshot> new_index_snapshot) {
    new_index_snapshots_vec_.push_back(new_index_snapshot.get());
    new_index_snapshots_.emplace(std::move(index_name), new_index_snapshot);
}

SharedPtr<IndexSnapshot> IndexIndex::Insert(TableIndexEntry *table_index_entry, Txn *txn) {
    if (table_index_entry->CheckVisible(txn)) {
        auto index_snapshot = MakeShared<IndexSnapshot>();
        index_snapshot->table_index_entry_ = table_index_entry;

        SegmentIndexesGuard segment_index_guard = table_index_entry->GetSegmentIndexesGuard();
        for (const auto &[segment_id, segment_index_entry] : segment_index_guard.index_by_segment_) {
            if (segment_index_entry->CheckVisible(txn)) {
                index_snapshot->segment_index_entries_.emplace(segment_id, segment_index_entry.get());
            }
        }

        String index_name = *table_index_entry->GetIndexName();
        Insert(std::move(index_name), index_snapshot);
        return index_snapshot;
    }
    return nullptr;
}

SharedPtr<NewIndexSnapshot> IndexIndex::Insert(const String &index_name, SharedPtr<TableIndexMeeta> table_index_meta) {
    auto index_snapshot = MakeShared<NewIndexSnapshot>();
    index_snapshot->table_index_meta_ = table_index_meta;

    auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        index_snapshot->segment_index_metas_.emplace(segment_id, MakeUnique<SegmentIndexMeta>(segment_id, *table_index_meta));
    }

    Insert(index_name, index_snapshot);
    return index_snapshot;
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

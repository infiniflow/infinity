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

module infinity_core:block_index.impl;

import :block_index;
import :global_block_id;
import :default_values;
import :status;
import :infinity_exception;
import :new_txn;
import :db_meta;
import :table_meta;
import :segment_meta;
import :block_meta;
import :kv_store;
import :table_index_meta;
import :segment_index_meta;

import std;

namespace infinity {

SegmentOffset NewSegmentSnapshot::segment_offset() const {
    auto [segment_offset_, status] = segment_meta_->GetRowCnt1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    return segment_offset_;
}

const std::vector<std::unique_ptr<BlockMeta>> &NewSegmentSnapshot::block_map() const {
    if (block_map_.size()) {
        return block_map_;
    }
    auto [block_ids_ptr, status] = segment_meta_->GetBlockIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (BlockID block_id : *block_ids_ptr) {
        auto block_meta = std::make_unique<BlockMeta>(block_id, *segment_meta_);
        block_map_.emplace_back(std::move(block_meta));
    }
    return block_map_;
}

BlockIndex::BlockIndex() = default;

BlockIndex::~BlockIndex() = default;

void BlockIndex::NewInit(const std::shared_ptr<TableMeta> &table_meta) {
    table_meta_ = table_meta;
    Status status = Status::OK();
    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta_->GetSegmentIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        NewSegmentSnapshot &segment_snapshot = new_segment_block_index_.emplace(segment_id, NewSegmentSnapshot()).first->second;
        segment_snapshot.segment_meta_ = std::make_unique<SegmentMeta>(segment_id, *table_meta_);
    }
}

size_t BlockIndex::BlockCount() const {
    size_t count = 0;
    for (const auto &[_, segment_info] : new_segment_block_index_) {
        count += segment_info.block_map().size();
    }
    return count;
}

size_t BlockIndex::SegmentCount() const { return new_segment_block_index_.size(); }

SegmentOffset BlockIndex::GetSegmentOffset(SegmentID segment_id) const {
    auto seg_it = new_segment_block_index_.find(segment_id);
    if (seg_it != new_segment_block_index_.end()) {
        const auto &blocks_info = seg_it->second;
        return blocks_info.segment_offset();
    }
    return 0;
}

BlockOffset BlockIndex::GetBlockOffset(SegmentID segment_id, BlockID block_id) const {
    SegmentOffset segment_offset = GetSegmentOffset(segment_id);
    if ((SegmentOffset(block_id) + 1) * DEFAULT_BLOCK_CAPACITY <= segment_offset) {
        return DEFAULT_BLOCK_CAPACITY;
    }
    return segment_offset % DEFAULT_BLOCK_CAPACITY;
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
void IndexIndex::Insert(std::string index_name, std::shared_ptr<NewIndexSnapshot> new_index_snapshot) {
    new_index_snapshots_vec_.push_back(new_index_snapshot.get());
    new_index_snapshots_.emplace(std::move(index_name), new_index_snapshot);
}

std::shared_ptr<NewIndexSnapshot> IndexIndex::Insert(const std::string &index_name, std::shared_ptr<TableIndexMeta> table_index_meta) {
    auto index_snapshot = std::make_shared<NewIndexSnapshot>();
    index_snapshot->table_index_meta_ = table_index_meta;

    auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
    if (!status.ok()) {
        RecoverableError(status);
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        index_snapshot->segment_index_metas_.emplace(segment_id, std::make_unique<SegmentIndexMeta>(segment_id, *table_index_meta));
    }

    Insert(index_name, index_snapshot);
    return index_snapshot;
}

} // namespace infinity

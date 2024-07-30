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

#include <cassert>
#include <vector>

module compact_state_data;

import third_party;
import logger;

namespace infinity {

void RowIDRemap::AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id) {
    std::lock_guard lock(mutex_);
    auto &block_vec = row_id_map_[GlobalBlockID(segment_id, block_id)];
    // LOG_INFO(fmt::format("AddMap, segment_id: {}, block_id: {}, block_offset: {}, new_segment_id: {}, new_block_id: {}, new_block_offset: {}",
    //                      segment_id,
    //                      block_id,
    //                      block_offset,
    //                      new_row_id.segment_id_,
    //                      new_row_id.segment_offset_ / block_capacity_,
    //                      new_row_id.segment_offset_ % block_capacity_));
    bool insert_ok = block_vec.emplace(block_offset, new_row_id).second;
    if (!insert_ok) {
        UnrecoverableError(fmt::format("RowID already exists, segment_id: {}, block_id: {}, block_offset: {}", segment_id, block_id, block_offset));
    }
}

RowID RowIDRemap::GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const {
    auto &block_vec = row_id_map_.at(GlobalBlockID(segment_id, block_id));
    auto iter = block_vec.upper_bound(block_offset);
    if (iter == block_vec.begin()) {
        auto error_msg = fmt::format("RowID not found, segment_id: {}, block_id: {}, block_offset: {}", segment_id, block_id, block_offset);
        LOG_ERROR(error_msg);
        UnrecoverableError(error_msg);
    }
    --iter;
    RowID rtn = iter->second; // iter->first is the first that <= block_offset
    rtn.segment_offset_ += block_offset - iter->first;
    return rtn;
}

void RowIDRemap::AddMap(RowID old_row_id, RowID new_row_id) {
    std::lock_guard lock(mutex_);
    AddMap(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_, new_row_id);
}

RowID RowIDRemap::GetNewRowID(RowID old_row_id) const {
    return GetNewRowID(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_);
}

void CompactStateData::AddToDelete(TxnTimeStamp commit_ts, SegmentID segment_id, Vector<SegmentOffset> delete_offsets) {
    std::lock_guard lock(mutex_);
    to_delete_.emplace_back(commit_ts, segment_id, std::move(delete_offsets));
}

Vector<Pair<SegmentID, Vector<SegmentOffset>>> CompactStateData::GetToDelete() const {
    // return all to_delete entry that commits ts is larger than the current scan ts
    auto iter = std::upper_bound(to_delete_.begin(), to_delete_.end(), scan_ts_, [](const auto &ts, const auto &tp) { return ts < std::get<0>(tp); });
    Vector<Pair<SegmentID, Vector<SegmentOffset>>> res;
    for (auto it = iter; it != to_delete_.end(); ++it) {
        res.emplace_back(std::get<1>(*it), std::get<2>(*it));
    }
    return res;
}

void CompactStateData::AddNewSegment(SharedPtr<SegmentEntry> new_segment, Vector<SegmentEntry *> compacted_segments, Txn *txn) {
    std::lock_guard lock(mutex2_);
    auto *block_index = new_table_ref_->block_index_.get();
    block_index->Insert(new_segment.get(), txn);

    CompactSegmentData data{new_segment, std::move(compacted_segments)};
    segment_data_list_.push_back(std::move(data));
}

void CompactStateData::AddNewIndex(TableIndexEntry *table_index_entry, Txn *txn) {
    std::lock_guard lock(mutex2_);
    auto *index_index = new_table_ref_->index_index_.get();
    index_index->Insert(table_index_entry, txn);
}

void CompactStateData::AddNewIndexSegment(TableIndexEntry *table_index_entry, SegmentIndexEntry *index_segment_entry) {
    std::lock_guard lock(mutex2_);
    if (new_table_ref_->index_index_.get() == nullptr) {
        new_table_ref_->index_index_ = MakeShared<IndexIndex>();
    }
    new_table_ref_->index_index_->Insert(table_index_entry, index_segment_entry);
}

} // namespace infinity
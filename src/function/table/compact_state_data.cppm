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

export module compact_state_data;

import stl;
import infinity_exception;
import global_block_id;
import internal_types;
import default_values;
import table_entry;
import table_index_entry;
import segment_entry;
import segment_index_entry;
import base_table_ref;
import block_index;
import txn;
import logger;

namespace infinity {

export class RowIDRemap {
    using RowIDMap = HashMap<GlobalBlockID, Vector<Pair<BlockOffset, RowID>>, GlobalBlockIDHash>;

public:
    RowIDRemap(SizeT block_capacity = DEFAULT_BLOCK_CAPACITY) : block_capacity_(block_capacity) {}

    void AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id) {
        std::lock_guard lock(mutex_);
        auto &block_vec = row_id_map_[GlobalBlockID(segment_id, block_id)];
        block_vec.emplace_back(block_offset, new_row_id);
    }

    RowID GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const {
        auto &block_vec = row_id_map_.at(GlobalBlockID(segment_id, block_id));
        auto iter =
            std::upper_bound(block_vec.begin(),
                             block_vec.end(),
                             block_offset,
                             [](BlockOffset block_offset, const Pair<BlockOffset, RowID> &pair) { return block_offset < pair.first; } // NOLINT
            );
        if (iter == block_vec.begin()) {
            String error_message = "RowID not found";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        --iter;
        RowID rtn = iter->second;
        rtn.segment_offset_ += iter->first - block_offset;
        return rtn;
    }

    void AddMap(RowID old_row_id, RowID new_row_id) {
        std::lock_guard lock(mutex_);
        AddMap(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_, new_row_id);
    }

    RowID GetNewRowID(RowID old_row_id) const {
        return GetNewRowID(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_);
    }

private:
    std::mutex mutex_;
    const SizeT block_capacity_;

    RowIDMap row_id_map_;
};

export class CompactSegmentData {
public:
    SharedPtr<SegmentEntry> new_segment_{};
    Vector<SegmentEntry *> old_segments_{};
};

export class CompactStateData {
public:
    CompactStateData(TableEntry *table_entry) : new_table_ref_(MakeShared<BaseTableRef>(table_entry, MakeShared<BlockIndex>())){};

    void AddToDelete(SegmentID segment_id, const Vector<SegmentOffset> &delete_offsets) {
        std::lock_guard lock(mutex_);
        to_delete_[segment_id].insert(to_delete_[segment_id].end(), delete_offsets.begin(), delete_offsets.end());
    }

    const HashMap<SegmentID, Vector<SegmentOffset>> &GetToDelete() const { return to_delete_; }

    void AddNewSegment(SharedPtr<SegmentEntry> new_segment, Vector<SegmentEntry *> compacted_segments, Txn *txn) {
        std::lock_guard lock(mutex2_);
        auto *block_index = new_table_ref_->block_index_.get();
        block_index->Insert(new_segment.get(), txn);

        CompactSegmentData data{new_segment, std::move(compacted_segments)};
        segment_data_list_.push_back(std::move(data));
    }

    void AddNewIndex(TableIndexEntry *table_index_entry, Txn *txn) {
        std::lock_guard lock(mutex2_);
        auto *index_index = new_table_ref_->index_index_.get();
        index_index->Insert(table_index_entry, txn);
    }

    void AddNewIndexSegment(TableIndexEntry *table_index_entry, SegmentIndexEntry *index_segment_entry) {
        std::lock_guard lock(mutex2_);
        auto *index_index = new_table_ref_->index_index_.get();
        index_index->Insert(table_index_entry, index_segment_entry);
    }

    BaseTableRef *GetNewTableRef() const { return new_table_ref_.get(); }

public:
    Vector<CompactSegmentData> segment_data_list_;
    RowIDRemap remapper_{};

private:
    std::mutex mutex_;
    HashMap<SegmentID, Vector<SegmentOffset>> to_delete_;

    std::mutex mutex2_;
    SharedPtr<BaseTableRef> new_table_ref_{}; // table ref after compact
};

} // namespace infinity
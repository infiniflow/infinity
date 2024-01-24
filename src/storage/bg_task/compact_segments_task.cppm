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

import stl;
import bg_task;
import catalog;
import parser;
import default_values;
import infinity_exception;

export module compact_segments_task;

namespace infinity {

class RowIDRemapper {
public:
    void AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id) {
        auto &segment_map = row_id_map_[segment_id];
        auto &block_vec = segment_map[block_id];
        block_vec.emplace_back(block_offset, new_row_id);
    }

    RowID GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) {
        auto &segment_map = row_id_map_.at(segment_id);
        auto &block_vec = segment_map.at(block_id);
        auto iter =
            std::lower_bound(block_vec.begin(), block_vec.end(), block_offset, [](const auto &lhs, const auto &rhs) { return lhs.first < rhs; });
        if (iter == block_vec.end()) {
            UnrecoverableError("RowID not found");
        }
        RowID rtn = iter->second;
        rtn.segment_offset_ += iter->first - block_offset;
        return rtn;
    }

    void AddMap(RowID old_row_id, RowID new_row_id) {
        AddMap(old_row_id.segment_id_,
               old_row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY,
               old_row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY,
               new_row_id);
    }

    RowID GetNewRowID(RowID old_row_id) {
        return GetNewRowID(old_row_id.segment_id_,
                           old_row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY,
                           old_row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY);
    }

private:
    HashMap<SegmentID, HashMap<BlockID, Vector<Pair<BlockOffset, RowID>>>> row_id_map_;
};

export class CompactSegmentsTask final : public BGTask {
public:
    explicit CompactSegmentsTask(TableEntry *table_entry);

    String ToString() const override { return "Compact segments task"; }

    void Execute();

    void AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> block_row_hashmap);

private:
    SharedPtr<SegmentEntry> CompactSegments(Vector<SegmentID> segment_ids);

    void ApplyAllToDelete();

    void ApplyOneToDelete(SegmentID segment_id, const HashMap<BlockID, Vector<BlockOffset>> &to_delete);

private:
    TableEntry *const table_entry_{};
    const SegmentID max_segment_id_{};

    // A timestamp, txn

    Vector<SegmentEntry *> segment_entries_;

    List<Pair<SegmentID, HashMap<BlockID, Vector<BlockOffset>>>> to_deletes_;

    std::mutex mtx_{};

    RowIDRemapper remapper_;
};
} // namespace infinity
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
import bg_task;
import catalog;
import parser;
import default_values;
import infinity_exception;
import txn;

export module compact_segments_task;

namespace infinity {

class RowIDRemapper {
public:
    RowIDRemapper(SizeT block_capacity = DEFAULT_BLOCK_CAPACITY) : block_capacity_(block_capacity) {}

    void AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id) {
        auto &segment_map = row_id_map_[segment_id];
        auto &block_vec = segment_map[block_id];
        block_vec.emplace_back(block_offset, new_row_id);
    }

    RowID GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const;

    void AddMap(RowID old_row_id, RowID new_row_id) {
        AddMap(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_, new_row_id);
    }

    RowID GetNewRowID(RowID old_row_id) const {
        return GetNewRowID(old_row_id.segment_id_, old_row_id.segment_offset_ / block_capacity_, old_row_id.segment_offset_ % block_capacity_);
    }

private:
    const SizeT block_capacity_;

    HashMap<SegmentID, HashMap<BlockID, Vector<Pair<BlockOffset, RowID>>>> row_id_map_;
};

struct ToDeleteInfo {
    const SegmentID segment_id_;

    const HashMap<BlockID, Vector<BlockOffset>> block_row_hashmap_;

    const TxnTimeStamp commit_ts_;
};

export struct CompactSegmentsTaskState {
    RowIDRemapper remapper_;

    Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentID>>> segment_data_;
};

export class CompactSegmentsTask final : public BGTask {
public:
    explicit CompactSegmentsTask(TableEntry *table_entry, Txn *txn);

    String ToString() const override { return "Compact segments task"; }

    void Execute();

    // Called by `SegmentEntry::DeleteData` which is called by wal thread in
    // So to_deletes_ is thread-safe.
    void AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> &&block_row_hashmap, TxnTimeStamp commit_ts);

public:
    // these two are called by unit test. Do not use them directly.
    CompactSegmentsTaskState CompactSegs();

    void CommitCompacts(CompactSegmentsTaskState &&state);

private:
    SharedPtr<SegmentEntry> CompactSegsIntoOne(RowIDRemapper &remapper, const Vector<SegmentID> &segment_ids);

    void SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentID>>> &&segment_data);

    void ApplyToDelete(const RowIDRemapper &remapper);

private:
    TableEntry *const table_entry_;

    Txn *const txn_;

    Vector<ToDeleteInfo> to_deletes_;
};
} // namespace infinity
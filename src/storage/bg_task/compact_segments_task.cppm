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

export module compact_segments_task;

import stl;
import bg_task;
import default_values;
import infinity_exception;
import txn;
import global_block_id;
import base_table_ref;
import internal_types;

namespace infinity {

class TableEntry;
class SegmentEntry;

class RowIDRemapper {
private:
    using RowIDMap = HashMap<GlobalBlockID, Vector<Pair<BlockOffset, RowID>>, GlobalBlockIDHash>;

public:
    RowIDRemapper(SizeT block_capacity = DEFAULT_BLOCK_CAPACITY) : block_capacity_(block_capacity) {}

    void AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id) {
        auto &block_vec = row_id_map_[GlobalBlockID(segment_id, block_id)];
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

    RowIDMap row_id_map_;
};

struct ToDeleteInfo {
    const SegmentID segment_id_;

    const Vector<SegmentOffset> delete_offsets_;
};

export struct CompactSegmentsTaskState {
    // default copy construct of table ref
    CompactSegmentsTaskState() {}

    RowIDRemapper remapper_;

    Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> segment_data_;
    Vector<SegmentEntry *> old_segments_;

    UniquePtr<BaseTableRef> new_table_ref_ = nullptr;
};

export enum class CompactSegmentsTaskType : i8 {
    kCompactTable,
    kCompactPickedSegments,
    kInvalid,
};

export class CompactSegmentsTask final : public BGTask {
public:
    static SharedPtr<CompactSegmentsTask> MakeTaskWithPickedSegments(TableEntry *table_entry, Vector<SegmentEntry *> &&segments, Txn *txn);

    static SharedPtr<CompactSegmentsTask> MakeTaskWithWholeTable(TableEntry *table_entry, Txn *txn);

    explicit CompactSegmentsTask(TableEntry *table_entry, Vector<SegmentEntry *> &&segments, Txn *txn, CompactSegmentsTaskType type);

public:
    ~CompactSegmentsTask() override = default;

    String ToString() const override { return "Compact segments task"; }

    void CommitTxn() { txn_->txn_mgr()->CommitTxn(txn_); };

    void Execute();

    // Called by `SegmentEntry::DeleteData` which is called by wal thread in
    // So to_deletes_ is thread-safe.
    // TODO: remove lock
    void AddToDelete(SegmentID segment_id, Vector<SegmentOffset> &&delete_offsets);

    // these functions are called by unit test. Do not use them directly.
public:
    CompactSegmentsTaskState CompactSegments();

    void CreateNewIndex(BaseTableRef *table_ref);

    // Save new segment, set no_delete_ts, add compact wal cmd
    void SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data);

    // Apply the delete op commit in process of compacting
    void ApplyDeletes(const RowIDRemapper &remapper, const Vector<SegmentEntry *> &old_segments);

private:
    SharedPtr<SegmentEntry> CompactSegmentsToOne(RowIDRemapper &remapper, const Vector<SegmentEntry *> &segments);

public: // TMP: test
    const CompactSegmentsTaskType task_type_;
    TableEntry *table_entry_;
    Vector<SegmentEntry *> segments_;

    Txn *const txn_;

    std::mutex mutex_;
    Vector<ToDeleteInfo> to_deletes_;
};
} // namespace infinity
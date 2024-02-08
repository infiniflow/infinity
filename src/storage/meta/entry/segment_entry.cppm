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

export module segment_entry;

import stl;
import default_values;
import third_party;
import buffer_manager;
import data_access_state;
import block_entry;
import base_entry;
import infinity_exception;
import txn;

namespace infinity {

class TxnTableStore;
struct TableEntry;
class CompactSegmentsTask;
class BlockEntryIter;

export struct SegmentEntry : public BaseEntry {
public:
    friend class BlockEntryIter;
    friend struct TableEntry;

public:
    explicit SegmentEntry(const TableEntry *table_entry,
                          SharedPtr<String> segment_dir,
                          SegmentID segment_id,
                          SizeT row_capacity,
                          SizeT column_count,
                          bool sealed);

    static SharedPtr<SegmentEntry> NewSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn, bool sealed);

    static SharedPtr<SegmentEntry>
    NewReplaySegmentEntry(const TableEntry *table_entry, SegmentID segment_id, const SharedPtr<String> &segment_dir, TxnTimeStamp commit_ts);

    static SharedPtr<SegmentEntry> NewReplayCatalogSegmentEntry(const TableEntry *table_entry,
                                                                SegmentID segment_id,
                                                                const SharedPtr<String> &segment_dir,
                                                                u64 column_count,
                                                                SizeT row_count,
                                                                SizeT actual_row_count,
                                                                SizeT row_capacity,
                                                                TxnTimeStamp min_row_ts,
                                                                TxnTimeStamp max_row_ts,
                                                                TxnTimeStamp commit_ts,
                                                                TxnTimeStamp begin_ts,
                                                                TransactionID txn_id);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static SharedPtr<SegmentEntry> Deserialize(const nlohmann::json &table_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr);

    void MergeFrom(infinity::BaseEntry &other) override;

public:
    TxnTimeStamp min_row_ts() const { return min_row_ts_; }

    TxnTimeStamp deprecate_ts() {
        std::shared_lock lock(rw_locker_);
        return deprecate_ts_;
    }

    bool sealed() const { return false; } // FIXME: implement sealed

    void SetSealed() {
        if (sealed_.test_and_set()) {
            UnrecoverableError("SetSealed failed");
        }
    }

    BlockEntry *GetBlockEntryByID(BlockID block_id) const;

    int Room() const;

    void FlushNewData();

    void FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    bool TrySetCompacting(CompactSegmentsTask *compact_task, TxnTimeStamp compacting_ts);

    void SetNoDelete(TxnTimeStamp no_delete_ts);

    void SetDeprecated(TxnTimeStamp deprecate_ts);

    void RollbackCompact();

    static bool CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, Txn *delete_txn);

    bool CheckVisible(SegmentOffset segment_offset, TxnTimeStamp check_ts) const;

    // Check if the segment has any delete before check_ts
    bool CheckAnyDelete(TxnTimeStamp check_ts) const;

    // `this` is visible in one thread
    BlockID GetNextBlockID() const;

    // `this` is visible in one thread
    Pair<SizeT, BlockOffset> GetWalInfo() const;

    // `this` called in wal thread, and `block_entry_` is also accessed in flush, so lock is needed
    void AppendBlockEntry(UniquePtr<BlockEntry> block_entry);

public:
    // Const getter
    inline const TableEntry *GetTableEntry() const { return table_entry_; }
    inline const SharedPtr<String> &segment_dir() const { return segment_dir_; }
    inline SegmentID segment_id() const { return segment_id_; }
    inline SizeT row_capacity() const { return row_capacity_; }
    inline SizeT column_count() const { return column_count_; }

    inline SizeT row_count() const {
        if (sealed()) {
            return row_count_;
        }
        std::shared_lock lock(rw_locker_);
        return row_count_;
    }

    SizeT actual_row_count() const {
        std::shared_lock lock(rw_locker_);
        return actual_row_count_;
    }

    inline TxnTimeStamp max_row_ts() const { return deprecate_ts_; }

public:
    // called by wal thread
    u64 AppendData(TransactionID txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn);

    void DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap);

    void CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, BlockID block_id, u16 start_pos, u16 row_count);

    void CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap);

private:
    static SharedPtr<String> DetermineSegmentDir(const String &parent_dir, SegmentID seg_id);

protected: // protected for unit test
    // called when lock held
    inline void IncreaseRowCount(SizeT increased_row_count) {
        row_count_ += increased_row_count;
        actual_row_count_ += increased_row_count;
    }

    // called when lock held
    inline void DecreaseRemainRow(SizeT decrease_row_count) {
        if (decrease_row_count > actual_row_count_) {
            UnrecoverableError("Decrease row count exceed actual row count");
        }
        actual_row_count_ -= decrease_row_count;
    }

private:
    const TableEntry *table_entry_{};
    const SharedPtr<String> segment_dir_{};
    const SegmentID segment_id_{};
    const SizeT row_capacity_{};
    const u64 column_count_{};
    atomic_flag sealed_;

    mutable std::shared_mutex rw_locker_{}; // protect following

    SizeT row_count_{};
    SizeT actual_row_count_{}; // not deleted row count

    TxnTimeStamp min_row_ts_{UNCOMMIT_TS};      // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp first_delete_ts_{UNCOMMIT_TS}; // Indicate the first delete commit ts. If not delete, it is UNCOMMIT_TS
    TxnTimeStamp compacting_ts_{UNCOMMIT_TS};   // Indicate the commit_ts which start compacting this SegmentEntry
    TxnTimeStamp no_delete_ts_{UNCOMMIT_TS};    // Indicate the commit_ts which prehibit delete in this SegmentEntry
    TxnTimeStamp deprecate_ts_{UNCOMMIT_TS};    // Indicate the commit_ts which deprecate this SegmentEntry

    Vector<SharedPtr<BlockEntry>> block_entries_{};

    CompactSegmentsTask *compact_task_{};
};

} // namespace infinity

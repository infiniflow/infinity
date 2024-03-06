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
import txn_manager;
import fast_rough_filter;
import value;
import meta_entry_interface;
import cleanup_scanner;

namespace infinity {

class TxnTableStore;
struct TableEntry;
class CompactSegmentsTask;
class BlockEntryIter;
struct WalSegmentInfo;

export enum class SegmentStatus : u8 {
    kUnsealed,
    kSealing,
    kSealed,
    kCompacting,
    kNoDelete,
    kForbidCleanup,
    kDeprecated,
};

export struct SegmentEntry : public BaseEntry, public EntryInterface {
public:
    friend class BlockEntryIter;
    friend struct TableEntry;

public:
    explicit SegmentEntry(const TableEntry *table_entry,
                          SharedPtr<String> segment_dir,
                          SegmentID segment_id,
                          SizeT row_capacity,
                          SizeT column_count,
                          SegmentStatus status);

private:
    static SharedPtr<SegmentEntry> InnerNewSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn, SegmentStatus status);

public:
    // create unsealed entry, write delta catalog
    static SharedPtr<SegmentEntry> NewAppendSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn);

    // create sealed entry, write delta catalog
    static SharedPtr<SegmentEntry> NewImportSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn);

    // create sealed entry, write delta catalog
    static SharedPtr<SegmentEntry> NewCompactSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn);

    static SharedPtr<SegmentEntry>
    NewReplaySegmentEntry(const TableEntry *table_entry, SegmentID segment_id, const SharedPtr<String> &segment_dir, TxnTimeStamp commit_ts);

    static SharedPtr<SegmentEntry> NewReplayCatalogSegmentEntry(const TableEntry *table_entry,
                                                                SegmentID segment_id,
                                                                const SharedPtr<String> &segment_dir,
                                                                SegmentStatus status,
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
    // will only be called in TableEntry::Append
    // txn_mgr: nullptr if in wal replay, need to skip and recover sealing tasks after replay
    // task: step 1. wait for status change from unsealed to sealing in CommitAppend in TxnTableStore::Commit() in Txn::CommitBottom()
    //       step 2. create txn (sealing task), build bloomfilter and minmax filter, set sealed status
    void CreateTaskSetSegmentStatusSealed(TableEntry *table_entry, TxnManager *txn_mgr);

    void FinishTaskSetSegmentStatusSealed(SegmentStatus prev_status);

    bool TrySetCompacting(CompactSegmentsTask *compact_task);

    void SetNoDelete();

    void SetForbidCleanup(TxnTimeStamp deprecate_ts);

    void TrySetDeprecated();

    void RollbackCompact();

    void FlushNewData(TxnTimeStamp flush_ts);

    void FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static bool CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, TransactionID txn_id);

    bool CheckRowVisible(SegmentOffset segment_offset, TxnTimeStamp check_ts) const;

    bool CheckVisible(TxnTimeStamp check_ts) const;

    bool CheckDeprecate(TxnTimeStamp check_ts) const;

    // Check if the segment has any delete before check_ts
    bool CheckAnyDelete(TxnTimeStamp check_ts) const;

    // `this` is visible in one thread
    BlockID GetNextBlockID() const;

    // `this` is visible in one thread
    Pair<SizeT, BlockOffset> GetWalInfo() const;

    // `this` called in wal thread, and `block_entry_` is also accessed in flush, so lock is needed
    void AppendBlockEntry(UniquePtr<BlockEntry> block_entry);

    // used in Catalog::LoadFromEntry when restart
    void SetBlockEntryAt(SizeT index, UniquePtr<BlockEntry> block_entry);

    FastRoughFilter *GetFastRoughFilter() { return &fast_rough_filter_; }

    const FastRoughFilter *GetFastRoughFilter() const { return &fast_rough_filter_; }

    Vector<Pair<BlockID, String>> GetBlockFilterBinaryDataVector() const;

    void WalLoadFilterBinaryData(const String &segment_filter_data, const Vector<Pair<BlockID, String>> &block_filter_data);

    // called in lock, in serialize, deserialize job
    bool FinishedSealingTask() const { return FinishedSealingTask(status_); }

    static bool FinishedSealingTask(SegmentStatus segment_status) {
        switch (segment_status) {
            case SegmentStatus::kUnsealed:
            case SegmentStatus::kSealing: {
                return false;
            }
            case SegmentStatus::kSealed:
            case SegmentStatus::kCompacting:
            case SegmentStatus::kNoDelete:
            case SegmentStatus::kDeprecated: {
                return true;
            }
            default: {
                UnrecoverableError("Unexpected segment status");
            }
        }
    }


public:
    // Const getter
    const TableEntry *GetTableEntry() const { return table_entry_; }
    const SharedPtr<String> &segment_dir() const { return segment_dir_; }
    SegmentID segment_id() const { return segment_id_; }
    SizeT row_capacity() const { return row_capacity_; }
    SizeT column_count() const { return column_count_; }

    SegmentStatus status() const {
        std::shared_lock lock(rw_locker_);
        return status_;
    }

    SizeT row_count() const {
        std::shared_lock lock(rw_locker_);
        return row_count_;
    }

    SizeT actual_row_count() const {
        std::shared_lock lock(rw_locker_);
        return actual_row_count_;
    }

    int Room() const { return this->row_capacity_ - this->row_count(); }

    TxnTimeStamp min_row_ts() const { return min_row_ts_; }

    TxnTimeStamp max_row_ts() const {
        std::shared_lock lock(rw_locker_);
        return max_row_ts_;
    }

    BlockEntry *GetBlockEntryByID(BlockID block_id) const;

public:
    // called by wal thread
    u64 AppendData(TransactionID txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn);

    void DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap, Txn *txn);

    void CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, BlockID block_id, u16 start_pos, u16 row_count);

    void CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap);

private:
    // called by table entry
    void SetSealing();

    static SharedPtr<String> DetermineSegmentDir(const String &parent_dir, SegmentID seg_id);

protected: // protected for unit test
    // called when lock held
    void IncreaseRowCount(SizeT increased_row_count) {
        row_count_ += increased_row_count;
        actual_row_count_ += increased_row_count;
    }

    // called when lock held
    void DecreaseRemainRow(SizeT decrease_row_count) {
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

    mutable std::shared_mutex rw_locker_{}; // protect following

    SizeT row_count_{};
    SizeT actual_row_count_{}; // not deleted row count

    TxnTimeStamp min_row_ts_{UNCOMMIT_TS}; // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp max_row_ts_{UNCOMMIT_TS};
    TxnTimeStamp first_delete_ts_{UNCOMMIT_TS}; // Indicate the first delete commit ts. If not delete, it is UNCOMMIT_TS
    TxnTimeStamp deprecate_ts_{UNCOMMIT_TS};    // FIXME: need persist to disk

    Vector<SharedPtr<BlockEntry>> block_entries_{};

    // check if a value must not exist in the segment
    FastRoughFilter fast_rough_filter_;

    CompactSegmentsTask *compact_task_{};
    SegmentStatus status_;

    std::condition_variable_any no_delete_complete_cv_{};
    HashSet<TransactionID> delete_txns_; // current number of delete txn that write this segment

public:
    void Cleanup() override;

    void PickCleanup(CleanupScanner *scanner) override;
};

} // namespace infinity

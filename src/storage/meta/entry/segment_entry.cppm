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
struct TxnSegmentStore;
struct TableEntry;
class CompactSegmentsTask;
class BlockEntryIter;

export struct BlocksGuard {
    const Vector<SharedPtr<BlockEntry>> &block_entries_;
    std::shared_lock<std::shared_mutex> lock_;
};

export enum class SegmentStatus : u8 {
    kUnsealed,
    kSealed,
    kCompacting,
    kNoDelete,
    kDeprecated,
};

export struct SegmentEntry : public BaseEntry, public EntryInterface {
public:
    friend class BlockEntryIter;
    friend struct TableEntry;
    friend struct WalSegmentInfo;

    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const SegmentID segment_id, const TableEntry *table_entry);

public:
    explicit SegmentEntry(TableEntry *table_entry,
                          SharedPtr<String> segment_dir,
                          SegmentID segment_id,
                          SizeT row_capacity,
                          SizeT column_count,
                          SegmentStatus status);

    static SharedPtr<SegmentEntry> NewSegmentEntry(TableEntry *table_entry, SegmentID segment_id, Txn *txn);

    static SharedPtr<SegmentEntry> NewReplaySegmentEntry(TableEntry *table_entry,
                                                         SegmentID segment_id,
                                                         SegmentStatus status,
                                                         u64 column_count,
                                                         SizeT row_count,
                                                         SizeT actual_row_count,
                                                         SizeT row_capacity,
                                                         TxnTimeStamp min_row_ts,
                                                         TxnTimeStamp max_row_ts,
                                                         TxnTimeStamp commit_ts,
                                                         TxnTimeStamp deprecate_ts,
                                                         TxnTimeStamp begin_ts,
                                                         TransactionID txn_id);

    void UpdateSegmentReplay(SharedPtr<SegmentEntry> segment_entry, String segment_filter_binary_data);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static SharedPtr<SegmentEntry> Deserialize(const nlohmann::json &table_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr);

public:
    void AddBlockReplay(SharedPtr<BlockEntry> block_entry);

    void UpdateBlockReplay(SharedPtr<BlockEntry> block_entry, String block_filter_binary_data);

    bool SetSealed();

    bool TrySetCompacting(CompactSegmentsTask *compact_task);

    void SetNoDelete();

    void SetDeprecated(TxnTimeStamp deprecate_ts);

    void RollbackCompact();

    void FlushNewData();

    static bool CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, TransactionID txn_id);

    bool CheckRowVisible(SegmentOffset segment_offset, TxnTimeStamp check_ts, bool check_append) const;

    bool CheckDeleteVisible(HashMap<BlockID, Vector<BlockOffset>> &block_offsets_map, Txn *txn) const;

    virtual bool CheckVisible(Txn *txn) const override;

    bool CheckDeprecate(TxnTimeStamp check_ts) const;

    // Check if the segment has any delete before check_ts
    bool CheckAnyDelete(TxnTimeStamp check_ts) const;

    // `this` is visible in one thread
    BlockID GetNextBlockID() const;

    // `this` called in wal thread, and `block_entry_` is also accessed in flush, so lock is needed
    void AppendBlockEntry(UniquePtr<BlockEntry> block_entry);

    FastRoughFilter *GetFastRoughFilter() { return &fast_rough_filter_; }

    const FastRoughFilter *GetFastRoughFilter() const { return &fast_rough_filter_; }

    void LoadFilterBinaryData(const String &segment_filter_data);
    static String SegmentStatusToString(const SegmentStatus &type);

public:
    // Const getter
    const TableEntry *GetTableEntry() const { return table_entry_; }
    const SharedPtr<String> &segment_dir() const { return segment_dir_; }
    SegmentID segment_id() const { return segment_id_; }
    SizeT row_capacity() const { return row_capacity_; }
    SizeT column_count() const { return column_count_; }
    Vector<SharedPtr<BlockEntry>> &block_entries() { return block_entries_; }

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

    // only used in Serialize(), FullCheckpoint, and no concurrency
    SizeT checkpoint_row_count() const { return checkpoint_row_count_; }

    int Room() const { return this->row_capacity_ - this->row_count(); }

    TxnTimeStamp min_row_ts() const { return min_row_ts_; }

    TxnTimeStamp max_row_ts() const {
        std::shared_lock lock(rw_locker_);
        return max_row_ts_;
    }

    TxnTimeStamp deprecate_ts() const {
        std::shared_lock lock(rw_locker_);
        return deprecate_ts_;
    }

    SharedPtr<BlockEntry> GetBlockEntryByID(BlockID block_id) const;

    BlocksGuard GetBlocksGuard() const { return BlocksGuard{block_entries_, std::shared_lock(rw_locker_)}; }

public:
    SizeT row_count(TxnTimeStamp check_ts) const;

    u64 AppendData(TransactionID txn_id, TxnTimeStamp commit_ts, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn);

    SizeT DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap, Txn *txn);

    void CommitFlushed(TxnTimeStamp commit_ts);

    void CommitSegment(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnSegmentStore &segment_store);

    void RollbackBlocks(TxnTimeStamp commit_ts, const HashMap<BlockID, BlockEntry *> &block_entries);

private:
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
    TableEntry *table_entry_{};
    const SharedPtr<String> segment_dir_{};
    const SegmentID segment_id_{};
    const SizeT row_capacity_{};
    const u64 column_count_{};

    mutable std::shared_mutex rw_locker_{}; // protect following

    SizeT row_count_{};
    SizeT actual_row_count_{}; // not deleted row count
    SizeT checkpoint_row_count_{};

    TxnTimeStamp min_row_ts_{UNCOMMIT_TS}; // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp first_delete_ts_{UNCOMMIT_TS}; // Indicate the first delete commit ts. If not delete, it is UNCOMMIT_TS
    TxnTimeStamp deprecate_ts_{UNCOMMIT_TS};

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

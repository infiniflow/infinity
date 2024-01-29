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

export module catalog:segment_entry;

import :block_entry;
import :column_index_entry;
import :segment_column_index_entry;
import :base_entry;

import stl;
import default_values;
import third_party;
import buffer_manager;
import index_base;
import data_access_state;
import parser;
import index_file_worker;
import infinity_exception;

namespace infinity {

class TxnTableStore;
struct TableEntry;
class CompactSegmentsTask;

struct SegmentEntry : public BaseEntry {
    friend struct TableEntry;

public:
    explicit SegmentEntry(const TableEntry *table_entry);

    explicit SegmentEntry(const TableEntry *table_entry, SharedPtr<String> segment_dir, SegmentID segment_id, SizeT row_capacity, SizeT column_count);

    static SharedPtr<SegmentEntry> NewSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn);

    static SharedPtr<SegmentEntry>
    NewReplaySegmentEntry(const TableEntry *table_entry, SegmentID segment_id, const SharedPtr<String> &segment_dir, TxnTimeStamp commit_ts);

    static SharedPtr<SegmentEntry> NewReplayCatalogSegmentEntry(const TableEntry *table_entry,
                                                         SegmentID segment_id,
                                                         const SharedPtr<String> &segment_dir,
                                                         u64 column_count,
                                                         SizeT row_count,
                                                         SizeT row_capacity,
                                                         TxnTimeStamp min_row_ts,
                                                         TxnTimeStamp max_row_ts,
                                                         TxnTimeStamp commit_ts,
                                                         TxnTimeStamp begin_ts,
                                                         TransactionID txn_id);
    static UniquePtr<CreateIndexParam> GetCreateIndexParam(SizeT segment_row_count, const IndexBase *index_base, const ColumnDef *column_def);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static SharedPtr<SegmentEntry> Deserialize(const nlohmann::json &table_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr);

    void MergeFrom(infinity::BaseEntry &other) override;

    void WriteIndexToMemory(SharedPtr<ColumnDef> column_def,
                            bool prepare,
                            ColumnID column_id,
                            const IndexBase *index_base,
                            SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry);

    void FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

public:
    inline const Vector<SharedPtr<BlockEntry>> &block_entries() const { return block_entries_; }

    TxnTimeStamp min_row_ts() {
        std::shared_lock lock(rw_locker_);
        return min_row_ts_;
    }

    TxnTimeStamp deprecate_ts() {
        std::shared_lock lock(rw_locker_);
        return deprecate_ts_;
    }

    inline SegmentID segment_id() const { return segment_id_; }

    inline const TableEntry *GetTableEntry() const { return table_entry_; }

    BlockEntry *GetBlockEntryByID(BlockID block_id) const;

    inline const SharedPtr<String> &segment_dir() const { return segment_dir_; }

    inline SizeT row_count() const { return row_count_; }

    SizeT actual_row_count() const { return actual_row_count_; }

    int Room();

    void FlushData();

    void SetCompacting(CompactSegmentsTask *compact_task, TxnTimeStamp compacting_ts);

    void SetNoDelete(TxnTimeStamp no_delete_ts);

    void SetDeprecated(TxnTimeStamp deprecate_ts);

    void RollbackCompact();

    static bool CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, Txn *delete_txn);

    Vector<SharedPtr<BlockEntry>> &block_entries() { return block_entries_; }

    inline SizeT column_count() const { return column_count_; }

    inline TxnTimeStamp max_row_ts() const { return deprecate_ts_; }

    inline SizeT row_capacity() const { return row_capacity_; }

    const String &segment_dir() { return *segment_dir_; }

public:
    // Used in WAL replay & Physical Import & SegmentCompaction
    inline void AppendBlockEntry(UniquePtr<BlockEntry> block_entry) { block_entries_.emplace_back(std::move(block_entry)); }

    inline void IncreaseRowCount(SizeT increased_row_count) {
        row_count_ += increased_row_count;
        actual_row_count_ += increased_row_count;
    }

private:
    inline void DecreaseRemainRow(SizeT decrease_row_count) {
        if (decrease_row_count > actual_row_count_) {
            UnrecoverableError("Decrease row count exceed remain row count");
        }
        actual_row_count_ -= decrease_row_count;
    }

public:
    // Used in PhysicalImport
    inline BlockEntry *GetLastEntry() const {
        if (block_entries_.empty()) {
            UnrecoverableError("No any block entries");
        }
        return block_entries_.back().get();
    }

protected:
    u64 AppendData(TransactionID txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn);

public:
    void DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap);

protected:
    SharedPtr<SegmentColumnIndexEntry> CreateIndexFile(ColumnIndexEntry *column_index_entry,
                                                       SharedPtr<ColumnDef> column_def,
                                                       TxnTimeStamp create_ts,
                                                       BufferManager *buffer_mgr,
                                                       TxnTableStore *txn_store,
                                                       bool prepare,
                                                       bool is_replay);

    void CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, BlockID block_id, u16 start_pos, u16 row_count);

    void CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap);

private:
    static SharedPtr<String> DetermineSegmentDir(const String &parent_dir, SegmentID seg_id);

protected:
    std::shared_mutex rw_locker_{};

    const TableEntry *table_entry_{};
    const SharedPtr<String> segment_dir_{};
    const SegmentID segment_id_{};
    const SizeT row_capacity_{};
    const u64 column_count_{};

    SizeT row_count_{};
    SizeT actual_row_count_{}; // not deleted row count

    TxnTimeStamp min_row_ts_{0};              // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp compacting_ts_{UNCOMMIT_TS}; // Indicate the commit_ts which start compacting this SegmentEntry
    TxnTimeStamp no_delete_ts_{UNCOMMIT_TS};  // Indicate the commit_ts which prehibit delete in this SegmentEntry
    TxnTimeStamp deprecate_ts_{UNCOMMIT_TS};  // Indicate the commit_ts which deprecate this SegmentEntry

    Vector<SharedPtr<BlockEntry>> block_entries_{};

private:
    CompactSegmentsTask *compact_task_{};
};

} // namespace infinity

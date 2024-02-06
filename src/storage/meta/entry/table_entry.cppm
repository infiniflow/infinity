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

export module table_entry;

import stl;

import txn_store;
import buffer_manager;
import third_party;
import table_entry_type;
import block_index;
import data_access_state;
import status;
import extra_ddl_info;
import column_def;
import internal_types;
import base_entry;
import txn_manager;
import segment_entry;
import block_entry;
import table_index_meta;

namespace infinity {

class IndexDef;
struct TableIndexEntry;
class IrsIndexEntry;
class TableMeta;
class Txn;
struct NewCatalog;

export struct TableEntry : public BaseEntry {
    friend struct NewCatalog;

public:
    // for iterator unit test.
    explicit TableEntry() : BaseEntry(EntryType::kTable) {}

    explicit TableEntry(const SharedPtr<String> &db_entry_dir,
                        SharedPtr<String> table_collection_name,
                        const Vector<SharedPtr<ColumnDef>> &columns,
                        TableEntryType table_entry_type,
                        TableMeta *table_meta,
                        TransactionID txn_id,
                        TxnTimeStamp begin_ts);

    static SharedPtr<TableEntry> NewTableEntry(const SharedPtr<String> &db_entry_dir,
                                               SharedPtr<String> table_collection_name,
                                               const Vector<SharedPtr<ColumnDef>> &columns,
                                               TableEntryType table_entry_type,
                                               TableMeta *table_meta,
                                               TransactionID txn_id,
                                               TxnTimeStamp begin_ts);

    static SharedPtr<TableEntry> NewReplayTableEntry(TableMeta *table_meta,
                                                     SharedPtr<String> db_entry_dir,
                                                     SharedPtr<String> table_name,
                                                     Vector<SharedPtr<ColumnDef>> &column_defs,
                                                     TableEntryType table_entry_type,
                                                     TransactionID txn_id,
                                                     TxnTimeStamp begin_ts,
                                                     TxnTimeStamp commit_ts,
                                                     bool is_delete,
                                                     SizeT row_count);

private:
    Tuple<TableIndexEntry *, Status> CreateIndex(const SharedPtr<IndexDef> &index_def,
                                                 ConflictType conflict_type,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr,
                                                 bool is_replay = false,
                                                 String replay_table_index_dir = "");

    Tuple<TableIndexEntry *, Status>
    DropIndex(const String &index_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status> GetIndex(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    void RemoveIndexEntry(const String &index_name, TransactionID txn_id, TxnManager *txn_mgr);

    static void CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_, bool is_replay);

    TableMeta *GetTableMeta() const { return table_meta_; }

    void Append(TransactionID txn_id, void *txn_store, BufferManager *buffer_mgr);

    void CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, const AppendState *append_state_ptr);

    void RollbackAppend(TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store);

    Status Delete(TransactionID txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state);

    void CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const DeleteState &append_state);

    Status RollbackDelete(TransactionID txn_id, DeleteState &append_state, BufferManager *buffer_mgr);

    Status CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_state);

    Status RollbackCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_state);

    // the `call_with_lock` is set true if the caller has already hold the lock.
    Status CommitImport(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment, bool call_with_lock = false);

    // This is private, **DO NOT** use by catalog
    Status ImportSegment(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment, bool call_with_lock);

    SegmentID GetNextSegmentID() { return next_segment_id_++; }

public:
    // Getter

    const SharedPtr<String> &GetDBName() const;

    inline const SharedPtr<String> &GetTableName() const { return table_name_; }

    SegmentEntry *GetSegmentByID(SegmentID seg_id, TxnTimeStamp ts) const;

    const BlockEntry *GetBlockEntryByID(SegmentID seg_id, BlockID block_id, TxnTimeStamp ts) const;

    inline const ColumnDef *GetColumnDefByID(ColumnID column_id) const { return columns_[column_id].get(); }

    inline SizeT ColumnCount() const { return columns_.size(); }

    const SharedPtr<String> &TableEntryDir() const { return table_entry_dir_; }

    inline SizeT row_count() const { return row_count_; }

    inline TableEntryType EntryType() const { return table_entry_type_; }

    Tuple<SizeT, SizeT, Status> GetSegmentRowCountBySegmentID(u32 seg_id);

    SharedPtr<BlockIndex> GetBlockIndex(TxnTimeStamp begin_ts);

    void GetFullTextAnalyzers(TransactionID txn_id,
                              TxnTimeStamp begin_ts,
                              SharedPtr<IrsIndexEntry> &irs_index_entry,
                              Map<String, String> &column2analyzer);

public:
    nlohmann::json Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<TableEntry> Deserialize(const nlohmann::json &table_entry_json, TableMeta *table_meta, BufferManager *buffer_mgr);

    virtual void MergeFrom(BaseEntry &other);

    bool CheckDeleteConflict(const Vector<RowID> &delete_row_ids, Txn *delete_txn);

public:
    u64 GetColumnIdByName(const String &column_name) const;

    Map<SegmentID, SharedPtr<SegmentEntry>> &segment_map() { return segment_map_; }

    HashMap<String, UniquePtr<TableIndexMeta>> &index_meta_map() { return index_meta_map_; }

    Vector<SharedPtr<ColumnDef>> &column_defs() { return columns_; }

protected:
    TableMeta *table_meta_{};

    HashMap<String, ColumnID> column_name2column_id_;

    std::shared_mutex rw_locker_{};

    SharedPtr<String> table_entry_dir_{};

    SharedPtr<String> table_name_{};

    Vector<SharedPtr<ColumnDef>> columns_{};

    TableEntryType table_entry_type_{TableEntryType::kTableEntry};

    // From data table
    Atomic<SizeT> row_count_{}; // this is actual row count
    Map<SegmentID, SharedPtr<SegmentEntry>> segment_map_{};
    SegmentEntry *unsealed_segment_{};
    atomic_u32 next_segment_id_{};

    // Index meta
    HashMap<String, UniquePtr<TableIndexMeta>> index_meta_map_{};
};

} // namespace infinity

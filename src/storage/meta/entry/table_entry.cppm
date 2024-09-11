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
import compaction_alg;
import meta_map;

import meta_entry_interface;
import cleanup_scanner;
import random;

import meta_info;
import block_entry;
import column_index_reader;
import value;

namespace infinity {

class IndexBase;
struct DBEntry;
struct TableIndexEntry;
class TableMeta;
class Txn;
struct Catalog;
class AddTableEntryOp;

export struct TableEntry final : public BaseEntry, public EntryInterface {
    friend struct Catalog;

public:
    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const String &table_name, TableMeta *table_meta);

public:
    using EntryOp = AddTableEntryOp;

public:
    explicit TableEntry(bool is_delete,
                        const SharedPtr<String> &table_entry_dir,
                        SharedPtr<String> table_collection_name,
                        const Vector<SharedPtr<ColumnDef>> &columns,
                        TableEntryType table_entry_type,
                        TableMeta *table_meta,
                        TransactionID txn_id,
                        TxnTimeStamp begin_ts,
                        SegmentID unsealed_id,
                        SegmentID next_segment_id,
                        ColumnID next_column_id);

private:
    TableEntry(const TableEntry &other);

public:
    UniquePtr<TableEntry> Clone(TableMeta *meta = nullptr) const;

    static SharedPtr<TableEntry> NewTableEntry(bool is_delete,
                                               const SharedPtr<String> &db_entry_dir,
                                               SharedPtr<String> table_collection_name,
                                               const Vector<SharedPtr<ColumnDef>> &columns,
                                               TableEntryType table_entry_type,
                                               TableMeta *table_meta,
                                               TransactionID txn_id,
                                               TxnTimeStamp begin_ts);

    static SharedPtr<TableEntry> ReplayTableEntry(bool is_delete,
                                                  TableMeta *table_meta,
                                                  SharedPtr<String> table_entry_dir,
                                                  SharedPtr<String> table_name,
                                                  const Vector<SharedPtr<ColumnDef>> &column_defs,
                                                  TableEntryType table_entry_type,
                                                  TransactionID txn_id,
                                                  TxnTimeStamp begin_ts,
                                                  TxnTimeStamp commit_ts,
                                                  SizeT row_count,
                                                  SegmentID unsealed_id,
                                                  SegmentID next_segment_id,
                                                  ColumnID next_column_id) noexcept;

public:
    Tuple<TableIndexEntry *, Status>
    CreateIndex(const SharedPtr<IndexBase> &index_base, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<SharedPtr<TableIndexEntry>, Status>
    DropIndex(const String &index_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status> GetIndex(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<TableIndexInfo>, Status> GetTableIndexInfo(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    void RemoveIndexEntry(const String &index_name, TransactionID txn_id);

    MetaMap<TableIndexMeta>::MapGuard IndexMetaMap() { return index_meta_map_.GetMetaMap(); }

    void AddIndexMetaNoLock(const String& table_meta_name, UniquePtr<TableIndexMeta> table_index_meta);

    // replay
    void UpdateEntryReplay(const SharedPtr<TableEntry> &table_entry);

    TableIndexEntry *CreateIndexReplay(const SharedPtr<String> &index_name,
                                       std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts);

    void UpdateIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    void DropIndexReplay(const String &index_name,
                         std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                         TransactionID txn_id,
                         TxnTimeStamp begin_ts);

    TableIndexEntry *GetIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Vector<TableIndexEntry *> TableIndexes(TransactionID txn_id, TxnTimeStamp begin_ts);

    void AddSegmentReplayWalImport(SharedPtr<SegmentEntry> segment_entry);

    void AddSegmentReplayWalCompact(SharedPtr<SegmentEntry> segment_entry);

private:
    void AddSegmentReplayWal(SharedPtr<SegmentEntry> segment_entry);

public:
    void AddSegmentReplay(SharedPtr<SegmentEntry> segment_entry);

    void UpdateSegmentReplay(SharedPtr<SegmentEntry> segment_entry, String segment_filter_binary_data);

public:
    TableMeta *GetTableMeta() const { return table_meta_; }

    void Import(SharedPtr<SegmentEntry> segment_entry, Txn *txn);

    void AddCompactNew(SharedPtr<SegmentEntry> segment_entry);

    void AppendData(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr, bool is_replay = false);

    void RollbackAppend(TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store);

    Status Delete(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, DeleteState &delete_state);

    Status RollbackDelete(TransactionID txn_id, DeleteState &append_state, BufferManager *buffer_mgr);

    Status CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_state);

    Status RollbackCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_state);

    Status CommitWrite(TransactionID txn_id,
                       TxnTimeStamp commit_ts,
                       const HashMap<SegmentID, TxnSegmentStore> &segment_stores,
                       const DeleteState *delete_state);

    Status RollbackWrite(TxnTimeStamp commit_ts, const Vector<TxnSegmentStore> &segment_stores);

    SegmentID GetNextSegmentID() { return next_segment_id_++; }

    SegmentID next_segment_id() const { return next_segment_id_; }

    ColumnID next_column_id() const { return next_column_id_; }

    static SharedPtr<String> DetermineTableDir(const String &parent_dir, const String &table_name);

    // MemIndexInsert is non-blocking. Caller must ensure there's no RowID gap between each call.
    void MemIndexInsert(Txn *txn, Vector<AppendRange> &append_ranges);

    // Dump or spill the memory indexer
    // void MemIndexDump(Txn *txn, bool spill = false);

    // User shall invoke this reguarly to populate recently inserted rows into the fulltext index. Noop for other types of index.
    void MemIndexCommit();

    // Invoked once at init stage to recovery memory index.
    void MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts);

    void OptimizeIndex(Txn *txn);

public:
    // Getter

    const SharedPtr<String> &GetDBName() const;

    inline const SharedPtr<String> &GetTableName() const { return table_name_; }

    SharedPtr<SegmentEntry> GetSegmentByID(SegmentID seg_id, TxnTimeStamp ts) const;

    SharedPtr<SegmentEntry> GetSegmentByID(SegmentID seg_id, Txn *txn) const;

    inline const ColumnDef *GetColumnDefByID(ColumnID column_id) const { return columns_[column_id].get(); }

    inline SharedPtr<ColumnDef> GetColumnDefByName(const String &column_name) const { return columns_[GetColumnIdByName(column_name)]; }

    inline SizeT ColumnCount() const { return columns_.size(); }

    const SharedPtr<String> &TableEntryDir() const { return table_entry_dir_; }

    String GetPathNameTail() const;

    inline SizeT row_count() const { return row_count_; }

    inline TableEntryType EntryType() const { return table_entry_type_; }

    SegmentID unsealed_id() const { return unsealed_id_; }

    Pair<SizeT, Status> GetSegmentRowCountBySegmentID(u32 seg_id);

    SharedPtr<BlockIndex> GetBlockIndex(Txn *txn);

    SharedPtr<IndexIndex> GetIndexIndex(Txn *txn);

    void GetFulltextAnalyzers(TransactionID txn_id, TxnTimeStamp begin_ts, Map<String, String> &column2analyzer);

    Tuple<Vector<String>, Vector<TableIndexMeta*>, std::shared_lock<std::shared_mutex>> GetAllIndexMapGuard() const;

    TableIndexMeta* GetIndexMetaPtrByName(const String& name) const;

public:
    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<TableEntry> Deserialize(const nlohmann::json &table_entry_json, TableMeta *table_meta, BufferManager *buffer_mgr);

    bool CheckDeleteConflict(const Vector<RowID> &delete_row_ids, TransactionID txn_id);

public:
    u64 GetColumnIdByName(const String &column_name) const;

    i64 GetColumnID(const String &column_name) const;

    Map<SegmentID, SharedPtr<SegmentEntry>> &segment_map() { return segment_map_; }

    SegmentEntry *GetSegmentEntry(SegmentID seg_id) const {
        std::shared_lock lock(rw_locker_);
        auto iter = segment_map_.find(seg_id);
        if (iter == segment_map_.end()) {
            return nullptr;
        }
        return iter->second.get();
    }

    const Vector<SharedPtr<ColumnDef>> &column_defs() const { return columns_; }

    IndexReader GetFullTextIndexReader(Txn *txn);

    void UpdateFullTextSegmentTs(TxnTimeStamp ts, std::shared_mutex &segment_update_ts_mutex, TxnTimeStamp &segment_update_ts) {
        return fulltext_column_index_cache_->UpdateKnownUpdateTs(ts, segment_update_ts_mutex, segment_update_ts);
    }

private:
    TableMeta *table_meta_{};

    MetaMap<TableIndexMeta> index_meta_map_{};

    HashMap<String, ColumnID> column_name2column_id_;

    const SharedPtr<String> table_entry_dir_{};

    SharedPtr<String> table_name_{};

    Vector<SharedPtr<ColumnDef>> columns_{};
    ColumnID next_column_id_{};

    const TableEntryType table_entry_type_{TableEntryType::kTableEntry};

    mutable std::shared_mutex rw_locker_{};

    // From data table
    Atomic<SizeT> row_count_{}; // this is actual row count
    Map<SegmentID, SharedPtr<SegmentEntry>> segment_map_{};
    SharedPtr<SegmentEntry> unsealed_segment_{};
    SegmentID unsealed_id_{};
    Atomic<SegmentID> next_segment_id_{};

    // for full text search cache
    SharedPtr<TableIndexReaderCache> fulltext_column_index_cache_;

public:
    // set nullptr to close auto compaction
    void SetCompactionAlg(UniquePtr<CompactionAlg> compaction_alg) { compaction_alg_ = std::move(compaction_alg); }

    void AddSegmentToCompactionAlg(SegmentEntry *segment_entry);

    void AddDeleteToCompactionAlg(SegmentID segment_id);

    Vector<SegmentEntry *> CheckCompaction(TransactionID txn_id);

    bool CompactPrepare() const;

private:
    // the compaction algorithm, mutable because all its interface are protected by lock
    mutable UniquePtr<CompactionAlg> compaction_alg_{};

private:
    void MemIndexInsertInner(TableIndexEntry *table_index_entry, Txn *txn, SegmentID seg_id, Vector<AppendRange> &append_ranges);

public:
    bool CheckIfIndexColumn(ColumnID column_id, TransactionID txn_id, TxnTimeStamp begin_ts);

    bool CheckAnyDelete(TxnTimeStamp check_ts) const;

public:
    void PickCleanup(CleanupScanner *scanner) override;

    void Cleanup() override;

public:
    Status AddWriteTxnNum(Txn *txn);

    void DecWriteTxnNum();

    void SetLocked();

    void SetUnlock();

private:
    std::mutex mtx_; // when table is locked, write is not allowed.
    std::condition_variable cv_;
    bool locked_ = false;
    bool wait_lock_ = false;
    SizeT write_txn_num_ = 0;

public:
    void AddColumns(const Vector<SharedPtr<ColumnDef>> &columns, const Vector<Value> &default_values, TxnTableStore *txn_store);

    void DropColumns(const Vector<String> &column_names, TxnTableStore *txn_store);
};

} // namespace infinity

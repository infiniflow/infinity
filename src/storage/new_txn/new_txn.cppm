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

export module new_txn;

import stl;
import statement_common;
import meta_info;
import table_def;
import index_base;
import data_block;
import meta_state;
import data_access_state;
import buffer_manager;
import txn_state;
import new_txn_store;
import database_detail;
import status;
import extra_ddl_info;
import internal_types;
import column_def;
import value;
import snapshot_info;
import txn_context;
import kv_store;
import new_catalog;

namespace infinity {

class NewTxnManager;
struct Catalog;
struct TableEntry;
struct DBEntry;
struct BaseEntry;
struct TableIndexEntry;
struct SegmentEntry;
struct WalEntry;
struct WalCmd;
struct WalCmdCreateDatabase;
struct WalCmdDropDatabase;
struct WalCmdCreateTable;
struct WalCmdDropTable;
struct WalCmdAddColumns;
struct WalCmdDropColumns;
struct WalCmdCreateIndex;
struct WalCmdDropIndex;
struct WalCmdAppend;
struct WalCmdDumpIndex;
struct WalChunkIndexInfo;
class CatalogDeltaEntry;
class CatalogDeltaOperation;
class BaseTableRef;
enum class CompactStatementType;
struct SegmentIndexEntry;
struct AddDeltaEntryTask;

export class NewTxn : public EnableSharedFromThis<NewTxn> {
public:
    // For new txn
    explicit NewTxn(NewTxnManager *txn_manager,
                    BufferManager *buffer_manager,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    UniquePtr<KVInstance> kv_instance,
                    SharedPtr<String> txn_text,
                    TransactionType txn_type);

    // For replay txn
    explicit NewTxn(BufferManager *buffer_mgr,
                    NewTxnManager *txn_mgr,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    UniquePtr<KVInstance> kv_instance,
                    TransactionType txn_type);

    virtual ~NewTxn();

    static UniquePtr<NewTxn> NewReplayTxn(BufferManager *buffer_mgr,
                                          NewTxnManager *txn_mgr,
                                          TransactionID txn_id,
                                          TxnTimeStamp begin_ts,
                                          UniquePtr<KVInstance> kv_instance,
                                          TransactionType txn_type);

    // NewTxn steps:
    // 1. CreateTxn
    // 2. Begin
    // 3. Commit() / Rollback
    // 3.1 PrepareCommit - multiple thread
    // 3.2 WriteWAL - single threads
    // 3.3 PrepareWriteData - single thread
    // 3.4 Commit - multiple threads

    //    void Begin();

    //    void SetBeginTS(TxnTimeStamp begin_ts);

    Status Commit();

    bool CheckConflict();

    Optional<String> CheckConflict(NewTxn *txn);

    Status PrepareCommit();

    void CommitBottom();

    void PostCommit();

    void CancelCommitBottom();

    Status Rollback();

    SharedPtr<AddDeltaEntryTask> MakeAddDeltaEntryTask();

    // Database OPs
    Status CreateDatabase(const String &db_name, ConflictType conflict_type, const SharedPtr<String> &comment);

    Status DropDatabase(const String &db_name, ConflictType conflict_type);

    Tuple<DBEntry *, Status> GetDatabase(const String &db_name);

    bool CheckDatabaseExists(const String &db_name);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name);

    Vector<String> ListDatabase();

    Vector<DatabaseDetail> ListDatabases();

    // Table and Collection OPs
    Status GetTables(const String &db_name, Vector<TableDetail> &output_table_array);

    bool CheckTableExists(const String &db_name, const String &table_name);

    Status CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type);

    Status RenameTable(TableEntry *old_table_entry, const String &new_table_name);

    Status AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names);

    Status AddColumns(TableEntry *table_entry, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status DropColumns(TableEntry *table_entry, const Vector<String> &column_names);

    Status CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type, BaseEntry *&collection_entry);

    Status DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type);

    Status ListTable(const String &db_name, Vector<String> &table_list);

    Tuple<TableEntry *, Status> GetTableByName(const String &db_name, const String &table_name);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name);

    Status GetCollectionByName(const String &db_name, const String &table_name, BaseEntry *&collection_entry);

    Tuple<SharedPtr<TableSnapshotInfo>, Status> GetTableSnapshot(const String &db_name, const String &table_name);

    Status ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info);

    // Index OPs
    bool CheckIndexExists(const String &db_name, const String &table_name, const String &index_name);

    Status CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    Status DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    Status ListIndex(const String &db_name, const String &table_name, Vector<String> &index_list);

    Status GetIndexDefByName(const String &db_name, const String &table_name, const String &index_name, SharedPtr<IndexBase> &index_base);

    Status AddIndexSegment(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);

    Status
    AddIndexChunk(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, const ChunkIndexInfo &chunk_info);

    Status DeprecateIndexChunk(const String &db_name,
                               const String &table_name,
                               const String &index_name,
                               SegmentID segment_id,
                               const Vector<ChunkID> &deprecate_ids);

    Status GetIndexChunks(const String &db_name,
                          const String &table_name,
                          const String &index_name,
                          SegmentID segment_id,
                          Vector<ChunkIndexInfo> &chunk_infos);

    Status GetIndexChunksByID(const String &db_id_str,
                              const String &table_id_str,
                              const String &index_id_str,
                              SegmentID segment_id,
                              Vector<ChunkIndexInfo> &chunk_infos);

    // If `prepare` is false, the index will be created in single thread. (called by `FsPhysicalCreateIndex`)
    // Else, only data is stored in index (Called by `PhysicalCreateIndexPrepare`). And the index will be created by multiple threads in next
    // operator. (called by `PhysicalCreateIndexDo`)
    Tuple<TableIndexEntry *, Status> CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    Tuple<TableIndexEntry *, Status> GetIndexByName(const String &db_name, const String &table_name, const String &index_name);

    Tuple<SharedPtr<TableIndexInfo>, Status> GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name);

    Pair<Vector<SegmentIndexEntry *>, Status>
    CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts = true);

    Status CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes);

    Status CreateIndexDo(TableEntry *table_entry,
                         const Map<SegmentID, SegmentIndexEntry *> &segment_index_entries,
                         const String &index_name,
                         HashMap<SegmentID, atomic_u64> &create_index_idxes);

    Status CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &indef);

    Status CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry);

    // View Ops
    // Fixme: view definition should be given
    Status CreateView(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status GetViewByName(const String &db_name, const String &view_name, BaseEntry *&view_entry);

    Status GetViews(const String &db_name, Vector<ViewDetail> &output_view_array);

    // DML
    Status Import(TableEntry *table_entry, SharedPtr<SegmentEntry> segment_entry);

    Status Append(TableEntry *table_entry, const SharedPtr<DataBlock> &input_block);

    Status Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block);

    Status Delete(TableEntry *table_entry, const Vector<RowID> &row_ids, bool check_conflict = true);

    Status Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type);

    Status OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params);

    // Getter
    BufferManager *buffer_mgr() const { return buffer_mgr_; }

    inline TransactionID TxnID() const { return txn_context_ptr_->txn_id_; }

    TxnTimeStamp CommitTS() const;

    TxnTimeStamp BeginTS() const;

    TxnState GetTxnState() const;

    TransactionType GetTxnType() const;

    bool IsWriteTransaction() const;

    void SetTxnCommitted();

    void SetTxnCommitting(TxnTimeStamp commit_ts);

    void SetTxnRollbacking(TxnTimeStamp rollback_ts);

    void SetTxnRollbacked();

    void SetTxnRead();

    void SetTxnWrite();

    // WAL and replay OPS
    void AddWalCmd(const SharedPtr<WalCmd> &cmd);

    void FullCheckpoint(const TxnTimeStamp max_commit_ts);

    bool DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts);

    NewTxnManager *txn_mgr() const { return txn_mgr_; }

    // Create txn store if not exists
    NewTxnTableStore *GetNewTxnTableStore(const String &table_name);

    NewTxnTableStore *GetExistNewTxnTableStore(TableEntry *table_entry) const;

    WalEntry *GetWALEntry() const;

    const SharedPtr<String> GetTxnText() const { return txn_text_; }

    const String &db_name() const { return db_name_; }

    void SetDBName(const String &db_name) { db_name_ = db_name; }

    void AddWriteTxnNum(TableEntry *table_entry);

    // Some transaction need to pass the txn access right check in txn commit phase;
    void SetReaderAllowed(bool allowed) { allowed_in_reader_ = allowed; }

    bool IsReaderAllowed() const { return allowed_in_reader_; }

    NewTxnStore *txn_store() { return &txn_store_; }

    SharedPtr<TxnContext> txn_context() const { return txn_context_ptr_; }
    void AddOperation(const SharedPtr<String> &operation_text) { txn_context_ptr_->AddOperation(operation_text); }
    Vector<SharedPtr<String>> GetOperations() const { return txn_context_ptr_->GetOperations(); }

    KVInstance *kv_instance() const { return kv_instance_.get(); }

private:
    void CheckTxnStatus();

    void CheckTxn(const String &db_name);

    Status GetDbID(const String &db_name, String &db_key, String &db_id);
    Status GetTableID(const String &db_name, const String &table_name, String &table_key, String &table_id, String &db_id);
    Status GetIndexID(const String &db_name,
                      const String &table_name,
                      const String &index_name,
                      String &index_key,
                      String &index_id,
                      String &table_id,
                      String &db_id);
    Status CommitAddIdxChunkByCmd(const String &db_id,
                                  const String &table_id,
                                  const String &index_id,
                                  SegmentID segment_id,
                                  const WalChunkIndexInfo &chunk_info);
    Status CommitDropIdxChunkByID(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id, ChunkID chunk_id);

    Status CommitCreateDB(const WalCmdCreateDatabase *create_db_cmd);
    Status CommitDropDB(const WalCmdDropDatabase *drop_db_cmd);
    Status CommitCreateTable(const WalCmdCreateTable *create_table_cmd);
    Status CommitCreateTableDef(const TableDef *table_def, const String &db_id, const String &table_id);
    Status CommitDropTable(const String &db_name, const String &table_name);
    Status CommitDropTableDef(const String &db_id, const String &table_id);
    Status CommitAddColumns(const WalCmdAddColumns *add_columns_cmd);
    Status CommitDropColumns(const WalCmdDropColumns *drop_columns_cmd);
    Status CommitCreateIndex(const WalCmdCreateIndex *create_index_cmd);
    Status CommitDropIndex(const WalCmdDropIndex *drop_index_cmd);
    Status CommitAppend(const WalCmdAppend *drop_index_cmd);
    Status CommitDumpIndex(WalCmdDumpIndex *dump_index_cmd);

private:
    // Reference to external class
    NewTxnManager *txn_mgr_{};
    BufferManager *buffer_mgr_{}; // This BufferManager ptr Only for replaying wal
    Catalog *catalog_{};
    NewCatalog *new_catalog_{};

    NewTxnStore txn_store_; // this has this ptr, so txn cannot be moved.

    // Use as txn context;
    mutable std::shared_mutex rw_locker_{};

    // Handled database
    String db_name_{};

    /// LOG
    // WalEntry
    SharedPtr<WalEntry> wal_entry_{};
    // TODO: remove this
    UniquePtr<CatalogDeltaEntry> txn_delta_ops_entry_{};

    // WalManager notify the commit bottom half is done
    std::mutex commit_lock_{};
    std::condition_variable commit_cv_{};
    bool commit_bottom_done_{false};

    // KV txn instance
    UniquePtr<KVInstance> kv_instance_{};

    // String
    SharedPtr<String> txn_text_{nullptr};

    // ADMIN command which allowed in follower and learner
    bool allowed_in_reader_{false};

private:
    SharedPtr<TxnContext> txn_context_ptr_{};
};

} // namespace infinity

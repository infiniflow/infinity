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

export module txn;

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
import txn_context;
import txn_store;
import database_detail;
import status;
import extra_ddl_info;
import internal_types;
import column_def;
import value;

namespace infinity {

struct GetParam {
    const String &db_name_{};
    const String &table_name_{};
    const Vector<ColumnID> &column_ids_{};
};

struct ScanParam {
    const String &db_name_{};
    const String &table_name_{};
    const Vector<ColumnID> &column_ids_{};
};

class TxnManager;
struct Catalog;
struct TableEntry;
struct DBEntry;
struct BaseEntry;
struct TableIndexEntry;
struct SegmentEntry;
struct WalEntry;
struct WalCmd;
class CatalogDeltaEntry;
class CatalogDeltaOperation;
class BaseTableRef;
enum class CompactStatementType;
struct SegmentIndexEntry;

export class Txn : public EnableSharedFromThis<Txn> {
public:
    // For new txn
    explicit Txn(TxnManager *txn_manager, BufferManager *buffer_manager, TransactionID txn_id, TxnTimeStamp begin_ts, SharedPtr<String> txn_text);

    // For replay txn
    explicit Txn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts);

    virtual ~Txn();

    static UniquePtr<Txn> NewReplayTxn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts);

    // Txn steps:
    // 1. CreateTxn
    // 2. Begin
    // 3. Commit() / Rollback
    // 3.1 PrepareCommit - multiple thread
    // 3.2 WriteWAL - single threads
    // 3.3 PrepareWriteData - single thread
    // 3.4 Commit - multiple threads

    //    void Begin();

    //    void SetBeginTS(TxnTimeStamp begin_ts);

    TxnTimeStamp Commit();

    bool CheckConflict();

    bool CheckConflict(Txn *txn);

    void CommitBottom();

    void CancelCommitBottom();

    void Rollback();

    // Database OPs
    Status CreateDatabase(const SharedPtr<String> &db_name, ConflictType conflict_type, const SharedPtr<String> &comment);

    Status DropDatabase(const String &db_name, ConflictType conflict_type);

    Tuple<DBEntry *, Status> GetDatabase(const String &db_name);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name);

    Vector<DatabaseDetail> ListDatabases();

    // Table and Collection OPs
    Status GetTables(const String &db_name, Vector<TableDetail> &output_table_array);

    Status CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type);

    Status RenameTable(TableEntry *old_table_entry, const String &new_table_name);

    Status AddColumns(TableEntry *table_entry, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status DropColumns(TableEntry *table_entry, const Vector<String> &column_names);

    Status CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type, BaseEntry *&collection_entry);

    Status DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type);

    Tuple<TableEntry *, Status> GetTableByName(const String &db_name, const String &table_name);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name);

    Status GetCollectionByName(const String &db_name, const String &table_name, BaseEntry *&collection_entry);

    // Index OPs
    // If `prepare` is false, the index will be created in single thread. (called by `FsPhysicalCreateIndex`)
    // Else, only data is stored in index (Called by `PhysicalCreateIndexPrepare`). And the index will be created by multiple threads in next
    // operator. (called by `PhysicalCreateIndexDo`)
    Tuple<TableIndexEntry *, Status> CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    Tuple<TableIndexEntry *, Status> GetIndexByName(const String &db_name, const String &table_name, const String &index_name);

    Tuple<SharedPtr<TableIndexInfo>, Status> GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name);

    Pair<Vector<SegmentIndexEntry *>, Status>
    CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts = true);

    Status CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes);

    Status CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &indef);

    Status CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry);

    Status DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    // View Ops
    // Fixme: view definition should be given
    Status CreateView(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status GetViewByName(const String &db_name, const String &view_name, BaseEntry *&view_entry);

    Status GetViews(const String &db_name, Vector<ViewDetail> &output_view_array);

    // DML
    Status Import(TableEntry *table_entry, SharedPtr<SegmentEntry> segment_entry);

    Status Append(TableEntry *table_entry, const SharedPtr<DataBlock> &input_block);

    Status Delete(TableEntry *table_entry, const Vector<RowID> &row_ids, bool check_conflict = true);

    Status Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type);

    Status OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params);

    // Getter
    BufferManager *buffer_mgr() const { return buffer_mgr_; }

    Catalog *GetCatalog() { return catalog_; }

    inline TransactionID TxnID() const { return txn_id_; }

    TxnTimeStamp CommitTS();

    TxnTimeStamp BeginTS();

    TxnState GetTxnState();

    TxnType GetTxnType() const;

    void SetTxnCommitted();

    void SetTxnCommitting(TxnTimeStamp commit_ts);

    void SetTxnRollbacked();

    void SetTxnRead();

    void SetTxnWrite();

    // WAL and replay OPS
    void AddWalCmd(const SharedPtr<WalCmd> &cmd);

    void FullCheckpoint(const TxnTimeStamp max_commit_ts);

    bool DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts);

    TxnManager *txn_mgr() const { return txn_mgr_; }

    // Create txn store if not exists
    TxnTableStore *GetTxnTableStore(TableEntry *table_entry);

    TxnTableStore *GetExistTxnTableStore(TableEntry *table_entry) const;

    WalEntry *GetWALEntry() const;

    const SharedPtr<String> GetTxnText() const { return txn_text_; }

    const String &db_name() const { return db_name_; }

    void SetDBName(const String &db_name) { db_name_ = db_name; }

    void AddWriteTxnNum(TableEntry *table_entry);

    // Some transaction need to pass the txn access right check in txn commit phase;
    void SetReaderAllowed(bool allowed) { allowed_in_reader_ = allowed; }

    bool IsReaderAllowed() const { return allowed_in_reader_; }

    TxnStore *txn_store() { return &txn_store_; }

private:
    void CheckTxnStatus();

    void CheckTxn(const String &db_name);

private:
    // Reference to external class
    TxnManager *txn_mgr_{};
    BufferManager *buffer_mgr_{}; // This BufferManager ptr Only for replaying wal
    Catalog *catalog_{};

    TxnStore txn_store_; // this has this ptr, so txn cannot be moved.
    TransactionID txn_id_{};

    TxnContext txn_context_;

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

    // String
    SharedPtr<String> txn_text_{nullptr};

    // ADMIN command which allowed in follower and learner
    bool allowed_in_reader_{false};
};

} // namespace infinity

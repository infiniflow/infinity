//
// Created by jinhai on 23-10-16.
//
module;

import stl;
import base_entry;
import parser;
import table_collection_detail;
//import table_collection_entry;
import table_def;
import index_def;
import data_block;
import meta_state;
import data_access_state;
import buffer_manager;
import txn_state;
import txn_context;
import wal_entry;
import txn_store;
import index_def_entry;

export module txn;


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
class NewCatalog;
class TableCollectionEntry;
class DBEntry;

export class Txn {
public:
    explicit Txn(TxnManager *txn_mgr, NewCatalog *catalog, u32 txn_id);

    void BeginTxn();

    void BeginTxn(TxnTimeStamp begin_ts);

    void CommitTxn();

    void CommitTxn(TxnTimeStamp commit_ts);

    void CommitTxnBottom();

    void CancelCommitTxnBottom();

    void RollbackTxn();

    void RollbackTxn(TxnTimeStamp abort_ts);

    EntryResult CreateDatabase(const String &db_name, ConflictType conflict_type);

    EntryResult DropDatabase(const String &db_name, ConflictType conflict_type);

    EntryResult GetDatabase(const String &db_name);

    Vector<TableCollectionDetail> GetTableCollections(const String &db_name);

    EntryResult CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type);

    EntryResult CreateIndex(const String &db_name, const String &table_name, SharedPtr<IndexDef> index_def, ConflictType conflict_type);

    EntryResult GetTableByName(const String &db_name, const String &table_name);

    EntryResult CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type);

    EntryResult DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type);

    EntryResult DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    EntryResult GetCollectionByName(const String &db_name, const String &table_name);

    // Fixme: view definition should be given
    EntryResult CreateView(const String &db_name, const String &view_name, ConflictType conflict_type);

    EntryResult DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type);

    EntryResult GetViewByName(const String &db_name, const String &view_name);

    Vector<BaseEntry *> GetViews(const String &db_name);

    UniquePtr<String> Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block);

    UniquePtr<String> Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids);

    void GetMetaTableState(MetaTableState *meta_table_state, const String &db_name, const String &table_name, const Vector<ColumnID> &columns);

    void GetMetaTableState(MetaTableState *meta_table_state, const TableCollectionEntry *table_collection_entry, const Vector<ColumnID> &columns);

    SharedPtr<GetState> InitializeGet(GetParam);

    void TableGet();

    void IndexGet();

    SharedPtr<ScanState> InitializeScan(const String &db_name, const String &table_name, const Vector<ColumnID> &column_ids);

    void Scan(ScanState *scan_state, SharedPtr<DataBlock> &output_block);

    void TableScan(ScanState *scan_state, SharedPtr<DataBlock> &output_block);

    void IndexScan(ScanState *scan_state, SharedPtr<DataBlock> &output_block);

    void AnnScan(ScanState *scan_state, SharedPtr<DataBlock> &output_block);

    UniquePtr<String> CompleteScan(const String &db_name, const String &table_name);

    BufferManager *GetBufferMgr() const;

    inline u64 TxnID() const { return txn_id_; }

    inline TxnTimeStamp CommitTS() { return txn_context_.GetCommitTS(); }

    inline TxnTimeStamp BeginTS() { return txn_context_.GetBeginTS(); }

    inline TxnState GetTxnState() { return txn_context_.GetTxnState(); }

    void SetTxnCommitted() { txn_context_.SetTxnCommitted(); }

    void SetTxnCommitting(TxnTimeStamp commit_ts) { txn_context_.SetTxnCommitting(commit_ts); }

    void AddTxnTableStore(const String &table_name, UniquePtr<TxnTableStore> txn_table_store);

    TxnTableStore *GetTxnTableStore(const String &table_name);

    void AddWalCmd(const SharedPtr<WalCmd> &cmd);

    void Checkpoint(const TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    UniquePtr<String> GetTableEntry(const String &db_name, const String &table_name, TableCollectionEntry *&table_entry);

private:
    NewCatalog *catalog_{};
    u64 txn_id_{};
    TxnContext txn_context_{};

    // Related database
    Set<String> db_names_{};

    // Txn store
    Set<DBEntry *> txn_dbs_{};
    Set<TableCollectionEntry *> txn_tables_{};
    Set<IndexDefEntry *> txn_indexes_{};

    // Only one db can be handled in one transaction.
    HashMap<String, BaseEntry *> txn_table_entries_{};
    // Key: table name Value: TxnTableStore
    HashMap<String, SharedPtr<TxnTableStore>> txn_tables_store_{};

    // Handled database
    String db_name_;

    // WalEntry
    SharedPtr<WalEntry> wal_entry_;
    // WalManager notify the  commit bottom half is done
    Mutex m;
    CondVar cv;
    bool done_bottom_{false};

    // Txn Manager
    TxnManager *txn_mgr_{};
};

} // namespace infinity

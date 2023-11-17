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

import stl;
import base_entry;
import parser;
import table_collection_detail;
// import table_collection_entry;
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
import database_detail;
import status;

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

    void Begin();

    void Commit();

    void CommitBottom();

    void CancelCommitBottom();

    void Rollback();

    Status CreateDatabase(const String &db_name, ConflictType conflict_type, BaseEntry*& db_entry);

    Status DropDatabase(const String &db_name, ConflictType conflict_type, BaseEntry*& db_entry);

    Status GetDatabase(const String &db_name, BaseEntry *&new_db_entry);

    Vector<DatabaseDetail> ListDatabases();

    Status GetTableCollections(const String &db_name, Vector<TableCollectionDetail> &output_table_array);

    Status CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type, BaseEntry *&new_table_entry);

    Status CreateIndex(const String &db_name, const String &table_name, SharedPtr<IndexDef> index_def, ConflictType conflict_type);

    Status GetTableByName(const String &db_name, const String &table_name, BaseEntry *&new_table_entry);

    Status CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type, BaseEntry *&collection_entry);

    Status DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type, BaseEntry *&drop_table_entry);

    Status DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    Status GetCollectionByName(const String &db_name, const String &table_name, BaseEntry *&collection_entry);

    // Fixme: view definition should be given
    Status CreateView(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status GetViewByName(const String &db_name, const String &view_name, BaseEntry *&view_entry);

    Vector<BaseEntry *> GetViews(const String &db_name);

    Status Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block);

    Status Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids);

    void GetMetaTableState(MetaTableState *meta_table_state, const String &db_name, const String &table_name, const Vector<ColumnID> &columns);

    void GetMetaTableState(MetaTableState *meta_table_state, const TableCollectionEntry *table_collection_entry, const Vector<ColumnID> &columns);

    BufferManager *GetBufferMgr() const;

    NewCatalog *GetCatalog() { return catalog_; }

    inline u64 TxnID() const { return txn_id_; }

    inline TxnTimeStamp CommitTS() { return txn_context_.GetCommitTS(); }

    inline TxnTimeStamp BeginTS() { return txn_context_.GetBeginTS(); }

    inline TxnState GetTxnState() { return txn_context_.GetTxnState(); }

    void SetTxnCommitted() { txn_context_.SetTxnCommitted(); }

    // Dangerous! only used during replaying wal.
    void FakeCommit(TxnTimeStamp commit_ts);

    TxnTableStore *GetTxnTableStore(TableCollectionEntry *table_entry);

    void AddWalCmd(const SharedPtr<WalCmd> &cmd);

    void Checkpoint(const TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    Status GetTableEntry(const String &db_name, const String &table_name, TableCollectionEntry *&table_entry);

private:
    // Txn Manager
    TxnManager *txn_mgr_{};
    NewCatalog *catalog_{};
    u64 txn_id_{};

    TxnContext txn_context_{};

    // Related database
    Set<String> db_names_{};

    // Txn store
    Set<DBEntry *> txn_dbs_{};
    Set<TableCollectionEntry *> txn_tables_{};
    HashMap<String, IndexDefEntry *> txn_indexes_{};

    // Only one db can be handled in one transaction.
    HashMap<String, BaseEntry *> txn_table_entries_{};
    // Key: table name Value: TxnTableStore
    HashMap<String, SharedPtr<TxnTableStore>> txn_tables_store_{};

    // Handled database
    String db_name_{};

    // WalEntry
    SharedPtr<WalEntry> wal_entry_;

    // WalManager notify the  commit bottom half is done
    Mutex lock_{};
    CondVar cond_var_{};
    bool done_bottom_{false};
};

} // namespace infinity

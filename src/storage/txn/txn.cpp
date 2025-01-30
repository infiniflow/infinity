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

#include <string>
#include <tuple>
#include <vector>

module txn;

import stl;

import infinity_exception;
import txn_manager;
import buffer_manager;
import wal_entry;
import third_party;
import logger;
import data_block;
import txn_store;
import txn_state;

import meta_state;
import data_access_state;
import status;
import meta_info;
import table_entry_type;

import database_detail;
import status;
import table_def;
import index_base;
import catalog_delta_entry;
import bg_task;
import background_process;
import base_table_ref;
import compact_statement;
import default_values;
import chunk_index_entry;
import memory_indexer;
import persistence_manager;
import infinity_context;
import admin_statement;
import global_resource_usage;
import wal_manager;
import defer_op;
import snapshot_info;

namespace infinity {

Txn::Txn(TxnManager *txn_manager,
         BufferManager *buffer_manager,
         TransactionID txn_id,
         TxnTimeStamp begin_ts,
         SharedPtr<String> txn_text,
         TransactionType txn_type)
    : txn_mgr_(txn_manager), buffer_mgr_(buffer_manager), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()), txn_text_(std::move(txn_text)) {
    catalog_ = InfinityContext::instance().storage()->catalog();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Txn");
#endif
    txn_context_ptr_ = TxnContext::Make();
    txn_context_ptr_->txn_id_ = txn_id;
    txn_context_ptr_->begin_ts_ = begin_ts;
    txn_context_ptr_->text_ = txn_text_;
    txn_context_ptr_->txn_type_ = txn_type;
}

Txn::Txn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TransactionType txn_type)
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {
    catalog_ = InfinityContext::instance().storage()->catalog();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Txn");
#endif
    txn_context_ptr_ = TxnContext::Make();
    txn_context_ptr_->txn_id_ = txn_id;
    txn_context_ptr_->begin_ts_ = begin_ts;
    txn_context_ptr_->txn_type_ = txn_type;
}

UniquePtr<Txn>
Txn::NewReplayTxn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TransactionType txn_type) {
    auto txn = MakeUnique<Txn>(buffer_mgr, txn_mgr, txn_id, begin_ts, txn_type);
    txn->txn_context_ptr_->commit_ts_ = begin_ts;
    txn->txn_context_ptr_->state_ = TxnState::kCommitted;
    return txn;
}

Txn::~Txn() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Txn");
#endif
}

// DML
Status Txn::Import(const String &db_name, const String &table_name, SharedPtr<SegmentEntry> segment_entry) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    this->CheckTxn(db_name);

    // build WalCmd
    WalSegmentInfo segment_info(segment_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdImport>(db_name, table_name, std::move(segment_info));
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);
    table_store->Import(std::move(segment_entry), this);

    return Status::OK();
}

Status Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    this->CheckTxn(db_name);
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    auto [err_msg, append_status] = table_store->Append(input_block);
    return append_status;
}

Status Txn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids, bool check_conflict) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    this->CheckTxn(db_name);

    if (check_conflict && table_entry->CheckDeleteConflict(row_ids, txn_context_ptr_->txn_id_)) {
        String log_msg = fmt::format("Rollback delete in table {} due to conflict.", table_name);
        RecoverableError(Status::TxnRollback(TxnID(), log_msg));
    }

    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDelete>(db_name, table_name, row_ids);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    auto [err_msg, delete_status] = table_store->Delete(row_ids);
    return delete_status;
}

Status
Txn::Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type) {
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);

    auto [err_mgs, compact_status] = table_store->Compact(std::move(segment_data), type);
    return compact_status;
}

void Txn::OptimizeIndexes() {
    TransactionID txn_id = this->TxnID();
    TxnTimeStamp begin_ts = this->BeginTS();

    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            table_entry->OptimizeIndex(this);
        }
    }
}

Status Txn::OptimizeTableIndexes(const String &db_name, const String &table_name) {
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name);
    if (!table_status.ok()) {
        return table_status;
    }
    table_entry->OptimizeIndex(this);
    return Status::OK();
}

Status
Txn::OptimizeIndexByName(const String &db_name, const String &table_name, const String &index_name, Vector<UniquePtr<InitParameter>> init_params) {
    auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, index_name);
    if (!status.ok()) {
        return status;
    }
    this->OptIndex(table_index_entry, std::move(init_params));
    return Status::OK();
}

Status Txn::OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params) {
    TableEntry *table_entry = table_index_entry->table_index_meta()->table_entry();
    TxnTableStore *txn_table_store = this->GetTxnTableStore(table_entry);

    const String &index_name = *table_index_entry->GetIndexName();
    const String &table_name = *table_entry->GetTableName();
    table_index_entry->OptIndex(txn_table_store, init_params, false /*replay*/);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdOptimize>(db_name_, table_name, index_name, std::move(init_params));
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

TxnTableStore *Txn::GetTxnTableStore(TableEntry *table_entry) { return txn_store_.GetTxnTableStore(table_entry); }

TxnTableStore *Txn::GetExistTxnTableStore(TableEntry *table_entry) const { return txn_store_.GetExistTxnTableStore(table_entry); }

void Txn::CheckTxnStatus() {
    TxnState txn_state = this->GetTxnState();
    if (txn_state != TxnState::kStarted) {
        String error_message = "Transaction isn't started.";
        UnrecoverableError(error_message);
    }
}

void Txn::CheckTxn(const String &db_name) {
    this->CheckTxnStatus();
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else if (!IsEqual(db_name_, db_name)) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Attempt to get table from another database {}", db_name));
        RecoverableError(Status::InvalidIdentifierName(db_name));
    }
}

// Database OPs
Status Txn::CreateDatabase(const SharedPtr<String> &db_name, ConflictType conflict_type, const SharedPtr<String> &comment) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [db_entry, status] = catalog_->CreateDatabase(db_name, comment, txn_context_ptr_->txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (db_entry == nullptr) { // nullptr means some exception happened
        return status;
    }
    txn_store_.AddDBStore(db_entry);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateDatabase>(*db_name, db_entry->GetPathNameTail(), *comment);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    return Status::OK();
}

Status Txn::DropDatabase(const String &db_name, ConflictType conflict_type) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [dropped_db_entry, status] = catalog_->DropDatabase(db_name, txn_context_ptr_->txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (dropped_db_entry.get() == nullptr) {
        return status;
    }
    txn_store_.DropDBStore(dropped_db_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropDatabase>(db_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    return Status::OK();
}

Status Txn::GetDatabase(const String &db_name) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = this->BeginTS();
    auto [db_entry, status] = catalog_->GetDatabase(db_name, txn_context_ptr_->txn_id_, begin_ts);
    return status;
}

Tuple<SharedPtr<DatabaseInfo>, Status> Txn::GetDatabaseInfo(const String &db_name) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = this->BeginTS();

    return catalog_->GetDatabaseInfo(db_name, txn_context_ptr_->txn_id_, begin_ts);
}

Vector<DatabaseDetail> Txn::ListDatabases() {
    Vector<DatabaseDetail> res;

    Vector<DBEntry *> db_entries = catalog_->Databases(txn_context_ptr_->txn_id_, this->BeginTS());
    SizeT db_count = db_entries.size();
    for (SizeT idx = 0; idx < db_count; ++idx) {
        DBEntry *db_entry = db_entries[idx];
        res.emplace_back(DatabaseDetail{db_entry->db_name_ptr(), db_entry->db_entry_dir(), db_entry->db_comment_ptr()});
    }

    return res;
}

// Table and Collection OPs
Status Txn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) {
    this->CheckTxn(db_name);

    return catalog_->GetTables(db_name, output_table_array, this);
}

Status Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    LOG_TRACE("Txn::CreateTable try to insert a created table placeholder on catalog");

    auto [table_entry, table_status] = catalog_->CreateTable(db_name, txn_context_ptr_->txn_id_, begin_ts, table_def, conflict_type, txn_mgr_);

    if (table_entry == nullptr) {
        return table_status;
    }

    txn_store_.AddTableStore(table_entry);
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTable>(std::move(db_name), table_entry->GetPathNameTail(), table_def);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("Txn::CreateTable created table entry is inserted.");
    return Status::OK();
}

Status Txn::RenameTable(const String &old_table_name, const String &new_table_name) {
    UnrecoverableError("Not implemented yet");
    return Status::OK();
}

Status Txn::AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs) {
    auto [table_entry, table_status] = GetTableByName(db_name, table_name);
    if (!table_status.ok()) {
        return table_status;
    }

    TxnTimeStamp begin_ts = this->BeginTS();
    auto [db_entry, db_status] = catalog_->GetDatabase(db_name, txn_context_ptr_->txn_id_, begin_ts);
    if (!db_status.ok()) {
        return db_status;
    }

    UniquePtr<TableEntry> new_table_entry = table_entry->Clone();
    new_table_entry->InitCompactionAlg(begin_ts);
    TxnTableStore *txn_table_store = txn_store_.GetTxnTableStore(new_table_entry.get());
    new_table_entry->AddColumns(column_defs, txn_table_store);
    auto add_status = db_entry->AddTable(std::move(new_table_entry), txn_context_ptr_->txn_id_, begin_ts, txn_mgr_, true /*add_if_found*/);
    if (!add_status.ok()) {
        return add_status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAddColumns>(*table_entry->GetDBName(), *table_entry->GetTableName(), column_defs);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status Txn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) {
    auto [table_entry, table_status] = GetTableByName(db_name, table_name);
    if (!table_status.ok()) {
        return table_status;
    }

    TxnTimeStamp begin_ts = this->BeginTS();
    auto [db_entry, db_status] = catalog_->GetDatabase(db_name, txn_context_ptr_->txn_id_, begin_ts);
    if (!db_status.ok()) {
        return db_status;
    }

    UniquePtr<TableEntry> new_table_entry = table_entry->Clone();
    new_table_entry->InitCompactionAlg(begin_ts);
    TxnTableStore *txn_table_store = txn_store_.GetTxnTableStore(new_table_entry.get());
    new_table_entry->DropColumns(column_names, txn_table_store);
    auto drop_status = db_entry->AddTable(std::move(new_table_entry), txn_context_ptr_->txn_id_, begin_ts, txn_mgr_, true /*add_if_found*/);
    if (!drop_status.ok()) {
        return drop_status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropColumns>(*table_entry->GetDBName(), *table_entry->GetTableName(), column_names);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    LOG_TRACE("Txn::DropTableCollectionByName try to insert a dropped table placeholder on catalog");
    auto [table_entry, table_status] = catalog_->DropTableByName(db_name, table_name, conflict_type, txn_context_ptr_->txn_id_, begin_ts, txn_mgr_);

    if (table_entry.get() == nullptr) {
        return table_status;
    }

    txn_store_.DropTableStore(table_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropTable>(db_name, table_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("Txn::DropTableCollectionByName dropped table entry is inserted.");
    return Status::OK();
}

// Index OPs
Tuple<TableIndexEntry *, Status> Txn::CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [table_index_entry, index_status] =
        catalog_->CreateIndex(table_entry, index_base, conflict_type, txn_context_ptr_->txn_id_, begin_ts, txn_mgr_);
    if (table_index_entry == nullptr) { // nullptr means some exception happened
        return {nullptr, index_status};
    }
    auto *txn_table_store = txn_store_.GetTxnTableStore(table_entry);
    txn_table_store->AddIndexStore(table_index_entry);

    String index_dir_tail = table_index_entry->GetPathNameTail();

    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdCreateIndex>(*table_entry->GetDBName(), *table_entry->GetTableName(), std::move(index_dir_tail), index_base);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return {table_index_entry, index_status};
}

Tuple<TableIndexEntry *, Status> Txn::GetIndexByName(const String &db_name, const String &table_name, const String &index_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();
    return catalog_->GetIndexByName(db_name, table_name, index_name, txn_context_ptr_->txn_id_, begin_ts);
}

Tuple<SharedPtr<TableIndexInfo>, Status> Txn::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name) {
    return catalog_->GetTableIndexInfo(db_name, table_name, index_name, this);
}

Pair<Vector<SegmentIndexEntry *>, Status>
Txn::CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts) {
    auto [segment_index_entries, status] = table_index_entry->CreateIndexPrepare(table_ref, this, prepare, false, check_ts);
    if (!status.ok()) {
        return {segment_index_entries, status};
    }

    return {segment_index_entries, Status::OK()};
}

// TODO: use table ref instead of table entry
Status Txn::CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    auto [table_entry, table_status] = this->GetTableByName(*table_ref->table_info_->db_name_, *table_ref->table_info_->table_name_);
    if (!table_status.ok()) {
        return table_status;
    }

    const auto &db_name = *table_entry->GetDBName();
    const auto &table_name = *table_entry->GetTableName();

    auto [table_index_entry, index_status] = this->GetIndexByName(db_name, table_name, index_name);
    if (!index_status.ok()) {
        return index_status;
    }

    return table_index_entry->CreateIndexDo(table_ref, create_index_idxes, this);
}

Status Txn::CreateIndexDo(TableEntry *table_entry,
                          const Map<SegmentID, SegmentIndexEntry *> &segment_index_entries,
                          const String &index_name,
                          HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    // auto *table_entry = table_ref->table_entry_ptr_;
    const auto &db_name = *table_entry->GetDBName();
    const auto &table_name = *table_entry->GetTableName();

    auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, index_name);
    if (!status.ok()) {
        return status;
    }

    return table_index_entry->CreateIndexDo(segment_index_entries, create_index_idxes, this);
}

Status Txn::CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry) {
    // String index_dir_tail = table_index_entry->GetPathNameTail();
    // auto index_base = table_index_entry->table_index_def();
    // wal_entry_->cmds_.push_back(
    //     MakeShared<WalCmdCreateIndex>(*table_entry->GetDBName(), *table_entry->GetTableName(), std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status Txn::CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base) {
    // this->CheckTxn(db_name);

    // auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, *index_base->index_name_);
    // if (!status.ok()) {
    //     return status;
    // }
    // String index_dir_tail = table_index_entry->GetPathNameTail();
    // this->AddWalCmd(MakeShared<WalCmdCreateIndex>(db_name, table_name, std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status Txn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    auto [table_index_entry, index_status] =
        catalog_->DropIndex(db_name, table_name, index_name, conflict_type, txn_context_ptr_->txn_id_, begin_ts, txn_mgr_);
    if (table_index_entry.get() == nullptr) {
        return index_status;
    }
    auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
    auto *txn_table_store = this->GetTxnTableStore(table_entry);
    txn_table_store->DropIndexStore(table_index_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return index_status;
}

SharedPtr<IndexReader> Txn::GetFullTextIndexReader(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();
    auto [table_entry, status] = catalog_->GetTableByName(db_name, table_name, txn_context_ptr_->txn_id_, begin_ts);
    if (!status.ok()) {
        RecoverableError(status);
    }

    return table_entry->GetFullTextIndexReader(this);
}

Tuple<TableEntry *, Status> Txn::GetTableByName(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    return catalog_->GetTableByName(db_name, table_name, txn_context_ptr_->txn_id_, begin_ts);
}

SharedPtr<BlockIndex> Txn::GetBlockIndexFromTable(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);
    TxnTimeStamp begin_ts = this->BeginTS();
    auto [table_entry, status] = catalog_->GetTableByName(db_name, table_name, txn_context_ptr_->txn_id_, begin_ts);
    if (!status.ok()) {
        return nullptr;
    }
    return table_entry->GetBlockIndex(this);
}

Tuple<SharedPtr<TableInfo>, Status> Txn::GetTableInfo(const String &db_name, const String &table_name) {
    return catalog_->GetTableInfo(db_name, table_name, this);
}

Tuple<SharedPtr<SegmentInfo>, Status> Txn::GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    return catalog_->GetSegmentInfo(db_name, table_name, segment_id, this);
}

Tuple<Vector<SharedPtr<SegmentInfo>>, Status> Txn::GetSegmentsInfo(const String &db_name, const String &table_name) {
    return catalog_->GetSegmentsInfo(db_name, table_name, this);
}

Tuple<SharedPtr<BlockInfo>, Status> Txn::GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id) {
    return catalog_->GetBlockInfo(db_name, table_name, segment_id, block_id, this);
}

Tuple<Vector<SharedPtr<BlockInfo>>, Status> Txn::GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    return catalog_->GetBlocksInfo(db_name, table_name, segment_id, this);
}

Tuple<SharedPtr<BlockColumnInfo>, Status>
Txn::GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id) {
    return catalog_->GetBlockColumnInfo(db_name, table_name, segment_id, block_id, column_id, this);
}

Status Txn::CheckTableExist(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

bool Txn::CheckTableHasDelete(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        RecoverableError(status);
    }

    return table_entry->CheckAnyDelete(this->BeginTS());
}

Tuple<SharedPtr<TableSnapshotInfo>, Status> Txn::GetTableSnapshot(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);
    return catalog_->GetTableSnapshot(db_name, table_name, this);
}

Status Txn::ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info) {
    return catalog_->ApplyTableSnapshot(table_snapshot_info, this);
}

Status Txn::CreateCollection(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: CreateCollection"};
}

Status Txn::GetCollectionByName(const String &, const String &, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: GetCollectionByName"};
}

Status Txn::LockTable(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    status = table_entry->SetLocked();
    LOG_INFO(fmt::format("Table {}.{} is locked", db_name, table_name));
    return status;
}

Status Txn::UnLockTable(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    status = table_entry->SetUnlock();
    LOG_INFO(fmt::format("Table {}.{} is unlocked", db_name, table_name));
    return status;
}

Status Txn::AddWriteTxnNum(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    return table_entry->AddWriteTxnNum(this);
}

Tuple<SharedPtr<SegmentEntry>, Status> Txn::MakeNewSegment(const String &db_name, const String &table_name) {
    auto [table_entry, status] = this->GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return {nullptr, status};
    }
    u64 segment_id = Catalog::GetNextSegmentID(table_entry);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, this);
    return {segment_entry, Status::OK()};
}

Status Txn::CreateView(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: CreateView"};
}

Status Txn::DropViewByName(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: DropViewByName"};
}

Status Txn::GetViewByName(const String &, const String &, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: GetViewByName"};
}

Status Txn::GetViews(const String &, Vector<ViewDetail> &output_view_array) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: GetViews"};
}

TxnTimeStamp Txn::CommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->commit_ts_;
}

TxnTimeStamp Txn::BeginTS() const { return txn_context_ptr_->begin_ts_; }

TxnState Txn::GetTxnState() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->state_;
}

TransactionType Txn::GetTxnType() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->txn_type_;
}

bool Txn::IsWriteTransaction() const { return txn_context_ptr_->is_write_transaction_; }

void Txn::SetTxnRollbacking(TxnTimeStamp rollback_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    TxnState txn_state = txn_context_ptr_->state_;
    if (txn_state != TxnState::kCommitting && txn_state != TxnState::kStarted) {
        String error_message = fmt::format("Transaction is in {} status, which can't rollback.", TxnState2Str(txn_state));
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kRollbacking;
    txn_context_ptr_->commit_ts_ = rollback_ts; // update commit_ts ?
}

void Txn::SetTxnRollbacked() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    txn_context_ptr_->state_ = TxnState::kRollbacked;
}

void Txn::SetTxnRead() { txn_context_ptr_->is_write_transaction_ = false; }

void Txn::SetTxnWrite() { txn_context_ptr_->is_write_transaction_ = true; }

void Txn::SetTxnCommitted() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (txn_context_ptr_->state_ != TxnState::kCommitting) {
        String error_message = "Transaction isn't in COMMITTING status.";
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kCommitted;
}

void Txn::SetTxnCommitting(TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (txn_context_ptr_->state_ != TxnState::kStarted) {
        String error_message = "Transaction isn't in STARTED status.";
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kCommitting;
    txn_context_ptr_->commit_ts_ = commit_ts;
    wal_entry_->commit_ts_ = commit_ts;
}

WalEntry *Txn::GetWALEntry() const { return wal_entry_.get(); }

// void Txn::Begin() {
//     TxnTimeStamp ts = txn_mgr_->GetBeginTimestamp(txn_context_ptr_->txn_id_);
//     LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", txn_context_ptr_->txn_id_, ts));
//     this->SetTxnBegin(ts);
// }

// void Txn::SetBeginTS(TxnTimeStamp begin_ts) {
//     LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", txn_context_ptr_->txn_id_, begin_ts));
//     this->SetTxnBegin(begin_ts);
// }

TxnTimeStamp Txn::Commit() {
    DeferFn defer_op([&] { txn_store_.RevertTableStatus(); });
    if (wal_entry_->cmds_.empty() && txn_store_.ReadOnly()) {
        // Don't need to write empty WalEntry (read-only transactions).
        TxnTimeStamp commit_ts = txn_mgr_->GetReadCommitTS(this);
        this->SetTxnCommitting(commit_ts);
        this->SetTxnCommitted();
        return commit_ts;
    }

    StorageMode current_storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (current_storage_mode != StorageMode::kWritable) {
        if (!IsReaderAllowed()) {
            RecoverableError(
                Status::InvalidNodeRole(fmt::format("This node is: {}, only read-only transaction is allowed.", ToString(current_storage_mode))));
        }
    }

    // register commit ts in wal manager here, define the commit sequence
    TxnTimeStamp commit_ts = txn_mgr_->GetWriteCommitTS(this);
    LOG_TRACE(fmt::format("Txn: {} is committing, begin_ts:{} committing ts: {}", txn_context_ptr_->txn_id_, BeginTS(), commit_ts));

    this->SetTxnCommitting(commit_ts);

    txn_store_.PrepareCommit1(); // Only for import and compact, pre-commit segment
    // LOG_INFO(fmt::format("Txn {} commit ts: {}", txn_context_ptr_->txn_id_, commit_ts));

    if (const auto conflict_reason = txn_mgr_->CheckTxnConflict(this); conflict_reason) {
        LOG_ERROR(
            fmt::format("Txn: {} is rolled back. rollback ts: {}. Txn conflict reason: {}.", txn_context_ptr_->txn_id_, commit_ts, *conflict_reason));
        wal_entry_ = nullptr;
        txn_mgr_->SendToWAL(this);
        RecoverableError(Status::TxnConflict(txn_context_ptr_->txn_id_, fmt::format("Txn conflict reason: {}.", *conflict_reason)));
    }

    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id_ = txn_context_ptr_->txn_id_;
    txn_mgr_->SendToWAL(this);

    // Wait until CommitTxnBottom is done.
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_cv_.wait(lk, [this] { return commit_bottom_done_; });

    PostCommit();

    return commit_ts;
}

bool Txn::CheckConflict() { return txn_store_.CheckConflict(catalog_); }

Optional<String> Txn::CheckConflict(Txn *other_txn) {
    LOG_TRACE(fmt::format("Txn {} check conflict with {}.", txn_context_ptr_->txn_id_, other_txn->txn_context_ptr_->txn_id_));

    return txn_store_.CheckConflict(other_txn->txn_store_);
}

void Txn::CommitBottom() {
    LOG_TRACE(fmt::format("Txn bottom: {} is started.", txn_context_ptr_->txn_id_));
    // prepare to commit txn local data into table
    TxnTimeStamp commit_ts = this->CommitTS();

    txn_store_.PrepareCommit(txn_context_ptr_->txn_id_, commit_ts, buffer_mgr_);

    txn_store_.CommitBottom(txn_context_ptr_->txn_id_, commit_ts);

    txn_store_.AddDeltaOp(txn_delta_ops_entry_.get(), txn_mgr_);

    // Don't need to write empty CatalogDeltaEntry (read-only transactions).
    if (!txn_delta_ops_entry_->operations().empty()) {
        txn_delta_ops_entry_->SaveState(txn_context_ptr_->txn_id_, this->CommitTS(), txn_mgr_->NextSequence());
    }

    // Notify the top half
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
    LOG_TRACE(fmt::format("Txn bottom: {} is finished.", txn_context_ptr_->txn_id_));
}

void Txn::PostCommit() {
    txn_store_.MaintainCompactionAlg();

    for (auto &sema : txn_store_.semas()) {
        sema->acquire();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        if (wal_cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto *checkpoint_cmd = static_cast<WalCmdCheckpoint *>(wal_cmd.get());
            if (checkpoint_cmd->is_full_checkpoint_) {
                wal_manager->CommitFullCheckpoint(checkpoint_cmd->max_commit_ts_);
            } else {
                wal_manager->CommitDeltaCheckpoint(checkpoint_cmd->max_commit_ts_);
            }
        }
    }
}

void Txn::CancelCommitBottom() {
    this->SetTxnRollbacked();
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
}

void Txn::Rollback() {
    DeferFn defer_op([&] { txn_store_.RevertTableStatus(); });
    auto state = this->GetTxnState();
    TxnTimeStamp abort_ts = 0;
    if (state == TxnState::kStarted) {
        abort_ts = txn_mgr_->GetReadCommitTS(this);
    } else if (state == TxnState::kCommitting) {
        abort_ts = this->CommitTS();
    } else {
        String error_message = fmt::format("Transaction {} state is {}.", txn_context_ptr_->txn_id_, TxnState2Str(state));
        UnrecoverableError(error_message);
    }
    this->SetTxnRollbacking(abort_ts);

    txn_store_.Rollback(txn_context_ptr_->txn_id_, abort_ts);

    LOG_TRACE(fmt::format("Txn: {} is dropped.", txn_context_ptr_->txn_id_));
}

SharedPtr<AddDeltaEntryTask> Txn::MakeAddDeltaEntryTask() {
    if (!txn_delta_ops_entry_->operations().empty()) {
        LOG_TRACE(txn_delta_ops_entry_->ToStringSimple());
        return MakeShared<AddDeltaEntryTask>(std::move(txn_delta_ops_entry_));
    }
    return nullptr;
}

void Txn::AddWalCmd(const SharedPtr<WalCmd> &cmd) {
    std::lock_guard guard(txn_store_.mtx_);
    auto state = this->GetTxnState();
    if (state != TxnState::kStarted) {
        auto begin_ts = BeginTS();
        UnrecoverableError(fmt::format("Should add wal cmd in started state, begin_ts: {}", begin_ts));
    }
    wal_entry_->cmds_.push_back(cmd);
    txn_context_ptr_->AddOperation(MakeShared<String>(cmd->ToString()));
}

// the max_commit_ts is determined by the max commit ts of flushed delta entry
// Incremental checkpoint contains only the difference in status between the last checkpoint and this checkpoint (that is, "increment")
bool Txn::DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts) {
    String delta_path, delta_name;
    // only save the catalog delta entry
    if (!catalog_->SaveDeltaCatalog(last_ckp_ts, max_commit_ts, delta_path, delta_name)) {
        return false;
    }
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpoint>(max_commit_ts, false, delta_path, delta_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return true;
}

// those whose commit_ts is <= max_commit_ts will be checkpointed
void Txn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {
    String full_path, full_name;

    catalog_->SaveFullCatalog(max_commit_ts, full_path, full_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpoint>(max_commit_ts, true, full_path, full_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
}

void Txn::AddWriteTxnNum(TableEntry *table_entry) {
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);
    table_store->AddWriteTxnNum();
}

} // namespace infinity

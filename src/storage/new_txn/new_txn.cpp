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

module new_txn;

import stl;

import infinity_exception;
import new_txn_manager;
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
import kv_store;
import random;
import kv_code;

namespace infinity {

NewTxn::NewTxn(NewTxnManager *txn_manager,
               BufferManager *buffer_manager,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               UniquePtr<KVInstance> kv_instance,
               SharedPtr<String> txn_text,
               TransactionType txn_type)
    : txn_mgr_(txn_manager), buffer_mgr_(buffer_manager), txn_store_(nullptr), wal_entry_(MakeShared<WalEntry>()),
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()), kv_instance_(std::move(kv_instance)), txn_text_(std::move(txn_text)) {
    catalog_ = InfinityContext::instance().storage()->catalog();
    new_catalog_ = InfinityContext::instance().storage()->new_catalog();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("NewTxn");
#endif
    txn_context_ptr_ = TxnContext::Make();
    txn_context_ptr_->txn_id_ = txn_id;
    txn_context_ptr_->begin_ts_ = begin_ts;
    txn_context_ptr_->text_ = txn_text_;
    txn_context_ptr_->txn_type_ = txn_type;
}

NewTxn::NewTxn(BufferManager *buffer_mgr,
               NewTxnManager *txn_mgr,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               UniquePtr<KVInstance> kv_instance,
               TransactionType txn_type)
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), txn_store_(nullptr), wal_entry_(MakeShared<WalEntry>()),
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()), kv_instance_(std::move(kv_instance)) {
    catalog_ = InfinityContext::instance().storage()->catalog();
    new_catalog_ = InfinityContext::instance().storage()->new_catalog();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("NewTxn");
#endif
    txn_context_ptr_ = TxnContext::Make();
    txn_context_ptr_->txn_id_ = txn_id;
    txn_context_ptr_->begin_ts_ = begin_ts;
    txn_context_ptr_->txn_type_ = txn_type;
}

UniquePtr<NewTxn> NewTxn::NewReplayTxn(BufferManager *buffer_mgr,
                                       NewTxnManager *txn_mgr,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts,
                                       UniquePtr<KVInstance> kv_instance,
                                       TransactionType txn_type) {
    auto txn = MakeUnique<NewTxn>(buffer_mgr, txn_mgr, txn_id, begin_ts, std::move(kv_instance), txn_type);
    txn->txn_context_ptr_->commit_ts_ = begin_ts;
    txn->txn_context_ptr_->state_ = TxnState::kCommitted;
    return txn;
}

NewTxn::~NewTxn() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("NewTxn");
#endif
}

// DML
Status NewTxn::Import(TableEntry *table_entry, SharedPtr<SegmentEntry> segment_entry) {
    const String &db_name = *table_entry->GetDBName();
    const String &table_name = *table_entry->GetTableName();

    this->CheckTxn(db_name);

    // build WalCmd
    WalSegmentInfo segment_info(segment_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdImport>(db_name, table_name, std::move(segment_info));
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);
    table_store->Import(std::move(segment_entry), nullptr);

    return Status::OK();
}

Status NewTxn::Append(TableEntry *table_entry, const SharedPtr<DataBlock> &input_block) {
    const String &db_name = *table_entry->GetDBName();
    const String &table_name = *table_entry->GetTableName();

    this->CheckTxn(db_name);
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    auto [err_msg, append_status] = table_store->Append(input_block);
    return append_status;
}

Status NewTxn::Delete(TableEntry *table_entry, const Vector<RowID> &row_ids, bool check_conflict) {
    const String &db_name = *table_entry->GetDBName();
    const String &table_name = *table_entry->GetTableName();

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
NewTxn::Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type) {
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);

    auto [err_mgs, compact_status] = table_store->Compact(std::move(segment_data), type);
    return compact_status;
}

Status NewTxn::OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params) {
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

TxnTableStore *NewTxn::GetTxnTableStore(TableEntry *table_entry) { return txn_store_.GetTxnTableStore(table_entry); }

TxnTableStore *NewTxn::GetExistTxnTableStore(TableEntry *table_entry) const { return txn_store_.GetExistTxnTableStore(table_entry); }

void NewTxn::CheckTxnStatus() {
    TxnState txn_state = this->GetTxnState();
    if (txn_state != TxnState::kStarted) {
        String error_message = "Transaction isn't started.";
        UnrecoverableError(error_message);
    }
}

void NewTxn::CheckTxn(const String &db_name) {
    this->CheckTxnStatus();
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else if (!IsEqual(db_name_, db_name)) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Attempt to get table from another database {}", db_name));
        RecoverableError(Status::InvalidIdentifierName(db_name));
    }
}

// Database OPs
Status NewTxn::CreateDatabase(const String &db_name, ConflictType conflict_type, const SharedPtr<String> &comment) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    bool db_exist = this->CheckDatabaseExists(db_name);
    if (db_exist and conflict_type == ConflictType::kError) {
        return Status(ErrorCode::kDuplicateDatabaseName, MakeUnique<String>(fmt::format("Database: {} already exists", db_name)));
    }

    if (db_exist and conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    SharedPtr<String> db_dir = DetermineRandomString(InfinityContext::instance().config()->DataDir(), fmt::format("db_{}", db_name));
    String path_tail = NewCatalog::GetPathNameTail(*db_dir);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateDatabase>(db_name, path_tail, *comment);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    return Status::OK();
}

Status NewTxn::DropDatabase(const String &db_name, ConflictType conflict_type) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    bool db_exist = this->CheckDatabaseExists(db_name);
    if (!db_exist and conflict_type == ConflictType::kError) {
        return Status::DBNotExist(db_name);
    }

    if (!db_exist and conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropDatabase>(db_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    return Status::OK();
}

Tuple<DBEntry *, Status> NewTxn::GetDatabase(const String &db_name) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = this->BeginTS();

    return catalog_->GetDatabase(db_name, txn_context_ptr_->txn_id_, begin_ts);
}

bool NewTxn::CheckDatabaseExists(const String &db_name) {
    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        iter2->Next();
        ++found_count;
    }
    if (found_count == 0) {
        // Not found
        return false;
    }
    return true;
}

Tuple<SharedPtr<DatabaseInfo>, Status> NewTxn::GetDatabaseInfo(const String &db_name) {
    this->CheckTxnStatus();

    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    auto iter2 = kv_instance_->GetIterator();

    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;
    String db_id{};
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        //        std::cout << String("Key: ") << iter2->Key().ToString() << std::endl;
        //        std::cout << String("Value: ") << iter2->Value().ToString() << std::endl;
        db_id = iter2->Value().ToString();
        iter2->Next();
        ++found_count;
        if (found_count > 1) {
            UnrecoverableError("Found multiple database keys");
        }
    }

    if (found_count == 0) {
        return {nullptr, Status::DBNotExist(db_name)};
    }

    SharedPtr<DatabaseInfo> db_info = MakeShared<DatabaseInfo>();
    db_info->db_name_ = MakeShared<String>(db_name);

    String db_dir_prefix = KeyEncode::CatalogDbTagKey(db_id, "dir");
    db_info->db_entry_dir_ = MakeShared<String>();
    Status status = kv_instance_->Get(db_dir_prefix, *db_info->db_entry_dir_);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    String db_comment_prefix = KeyEncode::CatalogDbTagKey(db_id, "comment");
    String db_comment;
    status = kv_instance_->Get(db_comment_prefix, db_comment);
    if (status.ok()) {
        db_info->db_comment_ = MakeShared<String>(db_comment);
    }

    return {db_info, Status::OK()};
}

Vector<String> NewTxn::ListDatabase() {
    this->CheckTxnStatus();
    Vector<String> res;
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(KeyEncode::kCatalogDbHeader);
    while (iter2->Valid() && iter2->Key().starts_with(KeyEncode::kCatalogDbHeader)) {
        res.emplace_back(iter2->Value().ToString());
        iter2->Next();
    }
    return res;
}

Vector<DatabaseDetail> NewTxn::ListDatabases() {
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
Status NewTxn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) {
    this->CheckTxn(db_name);

    return catalog_->GetTables(db_name, output_table_array, nullptr);
}

bool NewTxn::CheckTableExists(const String &db_name, const String &table_name) {
    // DB exists
    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    String db_key = KeyEncode::CatalogDbKey(db_name, this->BeginTS());
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;
    String db_id{};
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        db_id = iter2->Value().ToString();
        iter2->Next();
        ++found_count;
    }
    if (found_count == 0) {
        // Not found
        return false;
    }

    // Table exists
    found_count = 0;
    String table_key_prefix = KeyEncode::CatalogTablePrefix(db_id, table_name);
    iter2->Seek(table_key_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_key_prefix)) {
        iter2->Next();
        ++found_count;
    }
    if (found_count == 0) {
        // Not found
        return false;
    }
    return true;
}

Status NewTxn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxn(db_name);

    bool table_exist = this->CheckTableExists(db_name, *table_def->table_name());
    if (table_exist and conflict_type == ConflictType::kError) {
        return Status(ErrorCode::kDuplicateTableName, MakeUnique<String>(fmt::format("Table: {} already exists", *table_def->table_name())));
    }

    if (table_exist and conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    SharedPtr<String> local_table_dir = DetermineRandomPath(*table_def->table_name());
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTable>(db_name, *local_table_dir, table_def);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("NewTxn::CreateTable created table entry is inserted.");
    return Status::OK();
}

Status NewTxn::RenameTable(TableEntry *old_table_entry, const String &new_table_name) {
    UnrecoverableError("Not implemented yet");
    return Status::OK();
}

Status NewTxn::AddColumns(TableEntry *table_entry, const Vector<SharedPtr<ColumnDef>> &column_defs) {
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [db_entry, db_status] = catalog_->GetDatabase(*table_entry->GetDBName(), txn_context_ptr_->txn_id_, begin_ts);
    if (!db_status.ok()) {
        return db_status;
    }
    UniquePtr<TableEntry> new_table_entry = table_entry->Clone();
    new_table_entry->InitCompactionAlg(begin_ts);
    TxnTableStore *txn_table_store = txn_store_.GetTxnTableStore(new_table_entry.get());
    new_table_entry->AddColumns(column_defs, txn_table_store);
    auto add_status = db_entry->AddTable(std::move(new_table_entry), txn_context_ptr_->txn_id_, begin_ts, nullptr, true /*add_if_found*/);
    if (!add_status.ok()) {
        return add_status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAddColumns>(*table_entry->GetDBName(), *table_entry->GetTableName(), column_defs);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status NewTxn::DropColumns(TableEntry *table_entry, const Vector<String> &column_names) {
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [db_entry, db_status] = catalog_->GetDatabase(*table_entry->GetDBName(), txn_context_ptr_->txn_id_, begin_ts);
    if (!db_status.ok()) {
        return db_status;
    }
    UniquePtr<TableEntry> new_table_entry = table_entry->Clone();
    new_table_entry->InitCompactionAlg(begin_ts);
    TxnTableStore *txn_table_store = txn_store_.GetTxnTableStore(new_table_entry.get());
    new_table_entry->DropColumns(column_names, txn_table_store);
    auto drop_status = db_entry->AddTable(std::move(new_table_entry), txn_context_ptr_->txn_id_, begin_ts, nullptr, true /*add_if_found*/);
    if (!drop_status.ok()) {
        return drop_status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropColumns>(*table_entry->GetDBName(), *table_entry->GetTableName(), column_names);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status NewTxn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    LOG_TRACE("NewTxn::DropTableCollectionByName try to insert a dropped table placeholder on catalog");
    auto [table_entry, table_status] = catalog_->DropTableByName(db_name, table_name, conflict_type, txn_context_ptr_->txn_id_, begin_ts, nullptr);

    if (table_entry.get() == nullptr) {
        return table_status;
    }

    txn_store_.DropTableStore(table_entry.get());

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropTable>(db_name, table_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("NewTxn::DropTableCollectionByName dropped table entry is inserted.");
    return Status::OK();
}

// Index OPs
Tuple<TableIndexEntry *, Status> NewTxn::CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [table_index_entry, index_status] =
        catalog_->CreateIndex(table_entry, index_base, conflict_type, txn_context_ptr_->txn_id_, begin_ts, nullptr);
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

Tuple<TableIndexEntry *, Status> NewTxn::GetIndexByName(const String &db_name, const String &table_name, const String &index_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();
    return catalog_->GetIndexByName(db_name, table_name, index_name, txn_context_ptr_->txn_id_, begin_ts);
}

Tuple<SharedPtr<TableIndexInfo>, Status> NewTxn::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name) {
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }
    return table_entry->GetTableIndexInfo(index_name, nullptr);
}

Pair<Vector<SegmentIndexEntry *>, Status>
NewTxn::CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts) {
    auto [segment_index_entries, status] = table_index_entry->CreateIndexPrepare(table_ref, nullptr, prepare, false, check_ts);
    if (!status.ok()) {
        return {segment_index_entries, status};
    }

    return {segment_index_entries, Status::OK()};
}

// TODO: use table ref instead of table entry
Status NewTxn::CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
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

    return table_index_entry->CreateIndexDo(table_ref, create_index_idxes, nullptr);
}

Status NewTxn::CreateIndexDo(TableEntry *table_entry,
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

    return table_index_entry->CreateIndexDo(segment_index_entries, create_index_idxes, nullptr);
}

Status NewTxn::CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry) {
    // String index_dir_tail = table_index_entry->GetPathNameTail();
    // auto index_base = table_index_entry->table_index_def();
    // wal_entry_->cmds_.push_back(
    //     MakeShared<WalCmdCreateIndex>(*table_entry->GetDBName(), *table_entry->GetTableName(), std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status NewTxn::CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base) {
    // this->CheckTxn(db_name);

    // auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, *index_base->index_name_);
    // if (!status.ok()) {
    //     return status;
    // }
    // String index_dir_tail = table_index_entry->GetPathNameTail();
    // this->AddWalCmd(MakeShared<WalCmdCreateIndex>(db_name, table_name, std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status NewTxn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    auto [table_index_entry, index_status] =
        catalog_->DropIndex(db_name, table_name, index_name, conflict_type, txn_context_ptr_->txn_id_, begin_ts, nullptr);
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

Tuple<TableEntry *, Status> NewTxn::GetTableByName(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    return catalog_->GetTableByName(db_name, table_name, txn_context_ptr_->txn_id_, begin_ts);
}

Tuple<SharedPtr<TableInfo>, Status> NewTxn::GetTableInfo(const String &db_name, const String &table_name) {
    return catalog_->GetTableInfo(db_name, table_name, nullptr);
}

Tuple<SharedPtr<TableSnapshotInfo>, Status> NewTxn::GetTableSnapshot(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);
    return catalog_->GetTableSnapshot(db_name, table_name, nullptr);
}

Status NewTxn::ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info) {
    return catalog_->ApplyTableSnapshot(table_snapshot_info, nullptr);
}

Status NewTxn::CreateCollection(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: CreateCollection"};
}

Status NewTxn::GetCollectionByName(const String &, const String &, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: GetCollectionByName"};
}

Status NewTxn::CreateView(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: CreateView"};
}

Status NewTxn::DropViewByName(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: DropViewByName"};
}

Status NewTxn::GetViewByName(const String &, const String &, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: GetViewByName"};
}

Status NewTxn::GetViews(const String &, Vector<ViewDetail> &output_view_array) {
    return {ErrorCode::kNotSupported, "Not Implemented NewTxn Operation: GetViews"};
}

TxnTimeStamp NewTxn::CommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->commit_ts_;
}

TxnTimeStamp NewTxn::BeginTS() const { return txn_context_ptr_->begin_ts_; }

TxnState NewTxn::GetTxnState() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->state_;
}

TransactionType NewTxn::GetTxnType() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->txn_type_;
}

bool NewTxn::IsWriteTransaction() const { return txn_context_ptr_->is_write_transaction_; }

void NewTxn::SetTxnRollbacking(TxnTimeStamp rollback_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    TxnState txn_state = txn_context_ptr_->state_;
    if (txn_state != TxnState::kCommitting && txn_state != TxnState::kStarted) {
        String error_message = fmt::format("Transaction is in {} status, which can't rollback.", TxnState2Str(txn_state));
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kRollbacking;
    txn_context_ptr_->commit_ts_ = rollback_ts; // update commit_ts ?
}

void NewTxn::SetTxnRollbacked() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    txn_context_ptr_->state_ = TxnState::kRollbacked;
}

void NewTxn::SetTxnRead() { txn_context_ptr_->is_write_transaction_ = false; }

void NewTxn::SetTxnWrite() { txn_context_ptr_->is_write_transaction_ = true; }

void NewTxn::SetTxnCommitted() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (txn_context_ptr_->state_ != TxnState::kCommitting) {
        String error_message = "Transaction isn't in COMMITTING status.";
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kCommitted;
}

void NewTxn::SetTxnCommitting(TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (txn_context_ptr_->state_ != TxnState::kStarted) {
        String error_message = "Transaction isn't in STARTED status.";
        UnrecoverableError(error_message);
    }
    txn_context_ptr_->state_ = TxnState::kCommitting;
    txn_context_ptr_->commit_ts_ = commit_ts;
    wal_entry_->commit_ts_ = commit_ts;
}

WalEntry *NewTxn::GetWALEntry() const { return wal_entry_.get(); }

// void NewTxn::Begin() {
//     TxnTimeStamp ts = txn_mgr_->GetBeginTimestamp(txn_context_ptr_->txn_id_);
//     LOG_TRACE(fmt::format("NewTxn: {} is Begin. begin ts: {}", txn_context_ptr_->txn_id_, ts));
//     this->SetTxnBegin(ts);
// }

// void NewTxn::SetBeginTS(TxnTimeStamp begin_ts) {
//     LOG_TRACE(fmt::format("NewTxn: {} is Begin. begin ts: {}", txn_context_ptr_->txn_id_, begin_ts));
//     this->SetTxnBegin(begin_ts);
// }

Status NewTxn::Commit() {
    DeferFn defer_op([&] { txn_store_.RevertTableStatus(); });
    if (wal_entry_->cmds_.empty() && txn_store_.ReadOnly()) {
        // Don't need to write empty WalEntry (read-only transactions).
        TxnTimeStamp commit_ts = txn_mgr_->GetReadCommitTS(this);
        this->SetTxnCommitting(commit_ts);
        this->SetTxnCommitted();
        return Status::OK();
    }

    StorageMode current_storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (current_storage_mode != StorageMode::kWritable) {
        if (!IsReaderAllowed()) {
            return Status::InvalidNodeRole(fmt::format("This node is: {}, only read-only transaction is allowed.", ToString(current_storage_mode)));
        }
    }

    // register commit ts in wal manager here, define the commit sequence
    TxnTimeStamp commit_ts = txn_mgr_->GetWriteCommitTS(this);
    LOG_TRACE(fmt::format("NewTxn: {} is committing, begin_ts:{} committing ts: {}", txn_context_ptr_->txn_id_, BeginTS(), commit_ts));

    this->SetTxnCommitting(commit_ts);

    Status status = this->PrepareCommit();
    if (!status.ok()) {
        wal_entry_ = nullptr;
        txn_mgr_->SendToWAL(this);
        return status;
    }

    txn_store_.PrepareCommit1(); // Only for import and compact, pre-commit segment
    // LOG_INFO(fmt::format("NewTxn {} commit ts: {}", txn_context_ptr_->txn_id_, commit_ts));

    if (const auto conflict_reason = txn_mgr_->CheckTxnConflict(this); conflict_reason) {
        LOG_ERROR(fmt::format("NewTxn: {} is rolled back. rollback ts: {}. NewTxn conflict reason: {}.",
                              txn_context_ptr_->txn_id_,
                              commit_ts,
                              *conflict_reason));
        wal_entry_ = nullptr;
        txn_mgr_->SendToWAL(this);
        return Status::TxnConflict(txn_context_ptr_->txn_id_, fmt::format("NewTxn conflict reason: {}.", *conflict_reason));
    }

    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id_ = txn_context_ptr_->txn_id_;
    txn_mgr_->SendToWAL(this);

    // Wait until CommitTxnBottom is done.
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_cv_.wait(lk, [this] { return commit_bottom_done_; });

    PostCommit();

    return Status::OK();
}

Status NewTxn::PrepareCommit() {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_DATABASE: {
                auto *create_db_cmd = static_cast<WalCmdCreateDatabase *>(command.get());
                // Get the latest database id of system
                String db_string_id;
                Status status = kv_instance_->GetForUpdate("latest_database_id", db_string_id);
                SizeT db_id = 0;
                if (status.ok()) {
                    db_id = std::stoull(db_string_id);
                } else {
                    LOG_DEBUG(fmt::format("Failed to get latest_database_id: {}", status.message()));
                }

                ++db_id;
                String db_key = KeyEncode::CatalogDbKey(create_db_cmd->db_name_, commit_ts);
                String db_id_str = fmt::format("{}", db_id);
                status = kv_instance_->Put(db_key, db_id_str);
                if (!status.ok()) {
                    return status;
                }
                status = kv_instance_->Put("latest_database_id", db_id_str);
                if (!status.ok()) {
                    return status;
                }

                String db_storage_dir = KeyEncode::CatalogDbTagKey(db_id_str, "dir");
                status = kv_instance_->Put(db_storage_dir, create_db_cmd->db_dir_tail_);
                if (!status.ok()) {
                    return status;
                }

                String db_latest_table_id = KeyEncode::CatalogDbTagKey(db_id_str, "latest_table_id");
                status = kv_instance_->Put(db_latest_table_id, "0");
                if (!status.ok()) {
                    return status;
                }

                //                String db_latest_index_id = KeyEncode::CatalogDbTagKey(db_id_str, "latest_index_id");
                //                status = kv_instance_->Put(db_latest_index_id, "0");
                //                if (!status.ok()) {
                //                    return status;
                //                }
                //
                //                String db_latest_segment_id = KeyEncode::CatalogDbTagKey(db_id_str, "latest_segment_id");
                //                status = kv_instance_->Put(db_latest_segment_id, "0");
                //                if (!status.ok()) {
                //                    return status;
                //                }

                if (!create_db_cmd->db_comment_.empty()) {
                    String db_comment_key = KeyEncode::CatalogDbTagKey(db_id_str, "comment");
                    status = kv_instance_->Put(db_comment_key, create_db_cmd->db_comment_);
                    if (!status.ok()) {
                        return status;
                    }
                }

                //                auto iter2 = kv_instance_->GetIterator();
                //                String prefix = "";
                //                iter2->Seek(prefix);
                //                while (iter2->Valid() && iter2->Key().starts_with(prefix)) {
                //                    std::cout << iter2->Key().ToString() << String(" : ") << iter2->Value().ToString() << std::endl;
                //                    iter2->Next();
                //                }
                break;
            }
            case WalCommandType::DROP_DATABASE: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabase *>(command.get());
                String db_key_prefix = KeyEncode::CatalogDbPrefix(drop_db_cmd->db_name_);

                String db_id_str;
                String db_key;
                Status status = GetDBId(drop_db_cmd->db_name_, db_key, db_id_str);
                if (!status.ok()) {
                    return status;
                }

                status = kv_instance_->Delete(db_key);
                if (!status.ok()) {
                    return status;
                }

                // Delete db_dir, db_comment and latest_table_id
                String db_storage_dir = KeyEncode::CatalogDbTagKey(db_id_str, "dir");
                status = kv_instance_->Delete(db_storage_dir);
                if (!status.ok()) {
                    return status;
                }

                String db_comment_key = KeyEncode::CatalogDbTagKey(db_id_str, "comment");
                status = kv_instance_->Delete(db_comment_key);
                if (!status.ok()) {
                    return status;
                }

                String db_latest_table_id = KeyEncode::CatalogDbTagKey(db_id_str, "latest_table_id");
                status = kv_instance_->Delete(db_latest_table_id);
                if (!status.ok()) {
                    return status;
                }

                //                String db_latest_index_id = KeyEncode::CatalogDbTagKey(db_id, "latest_index_id");
                //                status = kv_instance_->Delete(db_latest_index_id);
                //                if (!status.ok()) {
                //                    return status;
                //                }
                //
                //                String db_latest_segment_id = KeyEncode::CatalogDbTagKey(db_id, "latest_segment_id");
                //                status = kv_instance_->Delete(db_latest_segment_id);
                //                if (!status.ok()) {
                //                    return status;
                //                }
                break;
            }
            case WalCommandType::CREATE_TABLE: {
                auto *create_table_cmd = static_cast<WalCmdCreateTable *>(command.get());
                String &db_name = create_table_cmd->db_name_;
                String &table_name = *create_table_cmd->table_def_->table_name();

                // Get database ID
                String db_id_str;
                String db_key;
                Status status = GetDBId(db_name, db_key, db_id_str);
                if (!status.ok()) {
                    return status;
                }

                // Get latest table id and lock the id
                String db_latest_table_id = KeyEncode::CatalogDbTagKey(db_id_str, "latest_table_id");
                String table_id_str;
                status = kv_instance_->GetForUpdate(db_latest_table_id, table_id_str);
                SizeT table_id = 0;
                if (status.ok()) {
                    table_id = std::stoull(table_id_str);
                } else {
                    LOG_DEBUG(fmt::format("Failed to get latest_table_id: {}", status.message()));
                }

                // Create table key value pair
                ++table_id;
                String table_key = KeyEncode::CatalogTableKey(db_name, table_name, commit_ts);
                table_id_str = fmt::format("{}", table_id);
                status = kv_instance_->Put(table_key, table_id_str);
                if (!status.ok()) {
                    return status;
                }

                // Update latest table id
                status = kv_instance_->Put(db_latest_table_id, table_id_str);
                if (!status.ok()) {
                    return status;
                }

                // Create table comment;
                if (create_table_cmd->table_def_->table_comment() != nullptr and !create_table_cmd->table_def_->table_comment()->empty()) {
                    String &table_comment = *create_table_cmd->table_def_->table_comment();
                    String table_comment_key = KeyEncode::CatalogTableTagKey(table_id_str, "comment");
                    status = kv_instance_->Put(table_comment_key, table_comment);
                    if (!status.ok()) {
                        return status;
                    }
                }

                // Create table index id;
                String table_latest_index_id_key = KeyEncode::CatalogTableTagKey(table_id_str, "latest_index_id");
                status = kv_instance_->Put(table_latest_index_id_key, "0");
                if (!status.ok()) {
                    return status;
                }

                // Create table segment id;
                String table_latest_segment_id_key = KeyEncode::CatalogTableTagKey(table_id_str, "latest_segment_id");
                status = kv_instance_->Put(table_latest_segment_id_key, "0");
                if (!status.ok()) {
                    return status;
                }

                //                auto *create_table_cmd = static_cast<WalCmdCreateTable *>(command.get());

                //                String db_string_id;
                //                Status status = kv_instance_->GetForUpdate("latest_database_id", db_string_id);
                //                SizeT db_id = 0;
                //                if (status.ok()) {
                //                    db_id = std::stoull(db_string_id);
                //                } else {
                //                    LOG_DEBUG(fmt::format("Failed to get latest_database_id: {}", status.message()));
                //                }
                //
                //                ++db_id;
                //                String db_key = KeyEncode::CatalogDbKey(create_table_cmd->db_name_, commit_ts);
                //                String db_id_str = fmt::format("{}", db_id);
                //                status = kv_instance_->Put(db_key, db_id_str);
                //                if (!status.ok()) {
                //                    return status;
                //                }
                //                status = kv_instance_->Put("latest_database_id", db_id_str);
                //                if (!status.ok()) {
                //                    return status;
                //                }
                //
                //                String db_storage_dir = KeyEncode::CatalogDbTagKey(db_id_str, "dir", commit_ts);
                //                status = kv_instance_->Put(db_storage_dir, create_db_cmd->db_dir_tail_);
                //                if (!status.ok()) {
                //                    return status;
                //                }
                //
                //                if (!create_db_cmd->db_comment_.empty()) {
                //                    String db_comment_key = KeyEncode::CatalogDbTagKey(db_id_str, "comment", commit_ts);
                //                    status = kv_instance_->Put(db_comment_key, create_db_cmd->db_comment_);
                //                    if (!status.ok()) {
                //                        return status;
                //                    }
                //                }
                break;
            }
            default: {
                break;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::GetDBId(const String &db_name, String &db_key, String &db_id) {
    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_db_keys;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        db_key = iter2->Key().ToString();
        db_id = iter2->Value().ToString();
        if (found_count > 0) {
            // Error branch
            error_db_keys.push_back(iter2->Key().ToString());
        }
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No db, ignore it.
        return Status::DBNotExist(db_name);
    }

    if (!error_db_keys.empty()) {
        // join error_db_keys
        String error_db_keys_str =
            std::accumulate(std::next(error_db_keys.begin()), error_db_keys.end(), error_db_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple database keys: {}", error_db_keys_str));
    }

    return Status::OK();
}

bool NewTxn::CheckConflict() { return txn_store_.CheckConflict(catalog_); }

Optional<String> NewTxn::CheckConflict(NewTxn *other_txn) {
    LOG_TRACE(fmt::format("NewTxn {} check conflict with {}.", txn_context_ptr_->txn_id_, other_txn->txn_context_ptr_->txn_id_));

    return txn_store_.CheckConflict(other_txn->txn_store_);
}

void NewTxn::CommitBottom() {
    LOG_TRACE(fmt::format("NewTxn bottom: {} is started.", txn_context_ptr_->txn_id_));
    // prepare to commit txn local data into table
    //    TxnTimeStamp commit_ts = this->CommitTS();

    Status status = kv_instance_->Commit();
    if (!status.ok()) {
        UnrecoverableError("KV instance commit failed");
    }

    //    txn_store_.PrepareCommit(txn_context_ptr_->txn_id_, commit_ts, buffer_mgr_);
    //
    //    txn_store_.CommitBottom(txn_context_ptr_->txn_id_, commit_ts);
    //
    //    txn_store_.AddDeltaOp(txn_delta_ops_entry_.get(), nullptr);

    // Don't need to write empty CatalogDeltaEntry (read-only transactions).
    //    if (!txn_delta_ops_entry_->operations().empty()) {
    //        txn_delta_ops_entry_->SaveState(txn_context_ptr_->txn_id_, this->CommitTS(), txn_mgr_->NextSequence());
    //    }

    // Notify the top half
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
    LOG_TRACE(fmt::format("NewTxn bottom: {} is finished.", txn_context_ptr_->txn_id_));
}

void NewTxn::PostCommit() {
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

void NewTxn::CancelCommitBottom() {
    this->SetTxnRollbacked();
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
}

Status NewTxn::Rollback() {
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

    Status status = kv_instance_->Rollback();
    if (!status.ok()) {
        return status;
    }
    txn_store_.Rollback(txn_context_ptr_->txn_id_, abort_ts);

    LOG_TRACE(fmt::format("NewTxn: {} is dropped.", txn_context_ptr_->txn_id_));

    return Status::OK();
}

SharedPtr<AddDeltaEntryTask> NewTxn::MakeAddDeltaEntryTask() {
    if (!txn_delta_ops_entry_->operations().empty()) {
        LOG_TRACE(txn_delta_ops_entry_->ToStringSimple());
        return MakeShared<AddDeltaEntryTask>(std::move(txn_delta_ops_entry_));
    }
    return nullptr;
}

void NewTxn::AddWalCmd(const SharedPtr<WalCmd> &cmd) {
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
bool NewTxn::DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts) {
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
void NewTxn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {
    String full_path, full_name;

    catalog_->SaveFullCatalog(max_commit_ts, full_path, full_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpoint>(max_commit_ts, true, full_path, full_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
}

void NewTxn::AddWriteTxnNum(TableEntry *table_entry) {
    TxnTableStore *table_store = this->GetTxnTableStore(table_entry);
    table_store->AddWriteTxnNum();
}

} // namespace infinity

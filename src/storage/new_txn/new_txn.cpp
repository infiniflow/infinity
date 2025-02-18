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

#include "parser/type/serialize.h"
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
import new_txn_store;
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
import constant_expr;
import buffer_obj;
import data_file_worker;
import version_file_worker;
import block_version;
import extra_command;

namespace infinity {

NewTxn::NewTxn(NewTxnManager *txn_manager,
               BufferManager *buffer_manager,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               UniquePtr<KVInstance> kv_instance,
               SharedPtr<String> txn_text,
               TransactionType txn_type)
    : txn_mgr_(txn_manager), buffer_mgr_(buffer_manager), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
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
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
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

    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);
    table_store->Import(std::move(segment_entry), nullptr);

    return Status::OK();
}

Status NewTxn::Append(TableEntry *table_entry, const SharedPtr<DataBlock> &input_block) {
    const String &db_name = *table_entry->GetDBName();
    const String &table_name = *table_entry->GetTableName();

    this->CheckTxn(db_name);
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    auto [err_msg, append_status] = table_store->Append(input_block);
    return append_status;
}

Status NewTxn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    this->CheckTxn(db_name);
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    if (table_store->column_defs().empty()) {
        table_store->SetColumnDefs(db_name, table_name);
    }

    auto [err_msg, append_status] = table_store->Append(input_block);
    return Status::OK();
}

Status NewTxn::Delete(TableEntry *table_entry, const Vector<RowID> &row_ids, bool check_conflict) {
    const String &db_name = *table_entry->GetDBName();
    const String &table_name = *table_entry->GetTableName();

    this->CheckTxn(db_name);

    if (check_conflict && table_entry->CheckDeleteConflict(row_ids, txn_context_ptr_->txn_id_)) {
        String log_msg = fmt::format("Rollback delete in table {} due to conflict.", table_name);
        RecoverableError(Status::TxnRollback(TxnID(), log_msg));
    }

    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDelete>(db_name, table_name, row_ids);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    auto [err_msg, delete_status] = table_store->Delete(row_ids);
    return delete_status;
}

Status
NewTxn::Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type) {
    const String &table_name = *table_entry->GetTableName();
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);

    auto [err_mgs, compact_status] = table_store->Compact(std::move(segment_data), type);
    return compact_status;
}

Status NewTxn::OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params) {
    TableEntry *table_entry = table_index_entry->table_index_meta()->table_entry();
    const String &table_name = *table_entry->GetTableName();
    //    NewTxnTableStore *txn_table_store = this->GetNewTxnTableStore(table_name);

    const String &index_name = *table_index_entry->GetIndexName();
    // FIXME: adapt nullptr
    table_index_entry->OptIndex(nullptr, init_params, false /*replay*/);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdOptimize>(db_name_, table_name, index_name, std::move(init_params));
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

NewTxnTableStore *NewTxn::GetNewTxnTableStore(const String &table_name) { return txn_store_.GetNewTxnTableStore(table_name); }

NewTxnTableStore *NewTxn::GetExistNewTxnTableStore(TableEntry *table_entry) const { return txn_store_.GetExistNewTxnTableStore(table_entry); }

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
    String db_key;
    String db_id;
    Status status = GetDbID(db_name, db_key, db_id);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kDBNotExist) {
            UnrecoverableError(status.message());
        }
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
    String table_key;
    String table_id;
    String db_id;
    Status status = GetTableID(db_name, table_name, table_key, table_id, db_id);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kDBNotExist && status.code() != ErrorCode::kTableNotExist) {
            UnrecoverableError(status.message());
        }
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

Status NewTxn::AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs) {

    // Get table id
    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);

    // Construct added columns name map
    Map<String, String> column_map_to_added;
    for (const auto &column_def : column_defs) {
        String column_key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column_def->name());
        column_map_to_added.emplace(column_key, column_def->name());
    }

    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String table_column_key = iter2->Key().ToString();
        // Check table name conflict;
        if (column_map_to_added.find(table_column_key) != column_map_to_added.end()) {
            return Status::DuplicateColumnName(column_map_to_added[table_column_key]);
        }
        iter2->Next();
    }

    // Generate add column cmd
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAddColumns>(db_name, table_name, column_defs);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status NewTxn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) {

    // Get table id
    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);

    // Construct added columns name map
    Set<String> exist_columns;
    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String table_column_key = iter2->Key().ToString();
        exist_columns.insert(table_column_key);
        iter2->Next();
    }

    for (const auto &column_name : column_names) {
        String column_key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column_name);
        if (exist_columns.find(column_key) == exist_columns.end()) {
            return Status::ColumnNotExist(column_name);
        }
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropColumns>(db_name, table_name, column_names);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

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

    //    const String &table_name = *table_entry->GetTableName();
    //    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    // TODO: adapt nullptr
    new_table_entry->AddColumns(column_defs, nullptr);
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
    //    const String &table_name = *table_entry->GetTableName();
    //    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    // TODO: adapt nullptr
    new_table_entry->DropColumns(column_names, nullptr);
    auto drop_status = db_entry->AddTable(std::move(new_table_entry), txn_context_ptr_->txn_id_, begin_ts, nullptr, true /*add_if_found*/);
    if (!drop_status.ok()) {
        return drop_status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropColumns>(*table_entry->GetDBName(), *table_entry->GetTableName(), column_names);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status NewTxn::DropTable(const String &db_name, const String &table_name, ConflictType conflict_type) {

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    bool table_exist = this->CheckTableExists(db_name, table_name);
    if (!table_exist and conflict_type == ConflictType::kError) {
        return Status::TableNotExist(table_name);
    }

    if (!table_exist and conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->IncreaseTableWriteCount(table_key);
    if (!status.ok()) {
        return status;
    }

    if (status.ok()) {
        SharedPtr<DropTableCommand> extra_command = MakeShared<DropTableCommand>(db_id_str, table_id_str, table_key);
        extra_commands_.push_back(extra_command);
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropTable>(db_name, table_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE(fmt::format("NewTxn::DropTable dropped table: {}.{}", db_name, table_name));
    return Status::OK();
}

Status NewTxn::ListTable(const String &db_name, Vector<String> &table_list) {
    this->CheckTxnStatus();

    String db_key;
    String db_id;
    Status status = GetDbID(db_name, db_key, db_id);
    if (!status.ok()) {
        return status;
    }

    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_table_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(db_table_prefix)) {
        String key_str = iter2->Key().ToString();
        size_t start = db_table_prefix.size();
        size_t end = key_str.find('|', start);
        table_list.emplace_back(key_str.substr(start, end - start));
        iter2->Next();
    }
    return Status::OK();
}

Status NewTxn::LockTable(const String &db_name, const String &table_name) {
    this->CheckTxnStatus();

    String table_key;
    String table_id;
    String db_id;
    Status status = GetTableID(db_name, table_name, table_key, table_id, db_id);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->LockTable(table_key);
    if (status.ok()) {
        SharedPtr<LockTableCommand> extra_command = MakeShared<LockTableCommand>(table_key);
        extra_commands_.push_back(extra_command);
    }
    return status;
}

Status NewTxn::UnlockTable(const String &db_name, const String &table_name) {
    this->CheckTxnStatus();

    String table_key;
    String table_id;
    String db_id;
    Status status = GetTableID(db_name, table_name, table_key, table_id, db_id);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->UnlockTable(table_key);
    if (status.ok()) {
        SharedPtr<UnlockTableCommand> extra_command = MakeShared<UnlockTableCommand>(table_key);
        extra_commands_.push_back(extra_command);
    }

    return status;
}

// Index OPs

bool NewTxn::CheckIndexExists(const String &db_name, const String &table_name, const String &index_name) {
    String index_key;
    String index_id;
    String table_id;
    String db_id;
    Status status = GetIndexID(db_name, table_name, index_name, index_key, index_id, table_id, db_id);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kDBNotExist && status.code() != ErrorCode::kTableNotExist && status.code() != ErrorCode::kIndexNotExist) {
            UnrecoverableError(status.message());
        }
        return false;
    }
    return true;
}

Status NewTxn::CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxn(db_name);

    bool index_exist = this->CheckIndexExists(db_name, table_name, *index_base->index_name_);
    if (index_exist && conflict_type == ConflictType::kError) {
        return Status(ErrorCode::kDuplicateIndexName, MakeUnique<String>(fmt::format("Index: {} already exists", *index_base->index_name_)));
    }
    if (index_exist && conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    SharedPtr<String> local_index_dir = DetermineRandomPath(*index_base->index_name_);
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateIndex>(db_name, table_name, std::move(*local_index_dir), index_base);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("NewTxn::CreateIndex created index entry is inserted.");
    return Status::OK();
}

Status NewTxn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    bool index_exist = this->CheckIndexExists(db_name, table_name, index_name);
    if (!index_exist && conflict_type == ConflictType::kError) {
        return Status::IndexNotExist(index_name);
    }

    if (!index_exist && conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE(fmt::format("NewTxn::DropIndexByName dropped index: {}.{}.{}", db_name, table_name, index_name));
    return Status::OK();
}

Status NewTxn::ListIndex(const String &db_name, const String &table_name, Vector<String> &index_list) {
    this->CheckTxnStatus();

    String table_key;
    String table_id;
    String db_id;
    Status status = GetTableID(db_name, table_name, table_key, table_id, db_id);
    if (!status.ok()) {
        return status;
    }

    String table_index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id, table_id);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_index_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_index_prefix)) {
        String key_str = iter2->Key().ToString();
        size_t start = table_index_prefix.size();
        size_t end = key_str.find('|', start);
        index_list.emplace_back(key_str.substr(start, end - start));
        iter2->Next();
    }
    return Status::OK();
}

Status NewTxn::GetIndexDefByName(const String &db_name, const String &table_name, const String &index_name, SharedPtr<IndexBase> &index_base) {
    this->CheckTxnStatus();

    String index_key;
    String index_id;
    String table_id;
    String db_id;
    Status status = GetIndexID(db_name, table_name, index_name, index_key, index_id, table_id, db_id);
    if (!status.ok()) {
        return status;
    }

    String index_def_key = KeyEncode::CatalogIndexTagKey(db_id, table_id, index_id, "index_def");
    String index_def_str;
    status = kv_instance_->Get(index_def_key, index_def_str);
    if (!status.ok()) {
        return status;
    }
    nlohmann::json index_def_json = nlohmann::json::parse(index_def_str);
    index_base = IndexBase::Deserialize(index_def_json);

    return Status::OK();
}

Status NewTxn::AddIndexSegment(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    this->CheckTxn(db_name);

    bool index_exist = this->CheckIndexExists(db_name, table_name, index_name);
    if (!index_exist) {
        return Status::IndexNotExist(index_name);
    }

    auto wal_dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id);
    wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(wal_dump_cmd));
    String idx_segment_key = KeyEncode::CatalogIdxSegmentKey(db_name, table_name, index_name, segment_id);
    bool add_success = txn_store_.AddDumpIndexCmd(idx_segment_key, wal_dump_cmd.get());
    if (!add_success) {
        return Status::UnexpectedError(fmt::format("Add dump index command {} failed", idx_segment_key));
    }

    LOG_TRACE(fmt::format("NewTxn::AddIndexSegment added index segment: {}.{}.{}.{}", db_name, table_name, index_name, segment_id));
    return Status::OK();
}

Status NewTxn::AddIndexChunk(const String &db_name,
                             const String &table_name,
                             const String &index_name,
                             SegmentID segment_id,
                             const ChunkIndexInfo &chunk_info) {
    this->CheckTxn(db_name);

    bool index_exist = this->CheckIndexExists(db_name, table_name, index_name);
    if (!index_exist) {
        return Status::IndexNotExist(index_name);
    }

    String idx_segment_key = KeyEncode::CatalogIdxSegmentKey(db_name, table_name, index_name, segment_id);
    WalCmdDumpIndex *dump_cmd = txn_store_.GetDumpIndexCmd(idx_segment_key);
    if (dump_cmd == nullptr) {
        auto wal_dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id);
        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(wal_dump_cmd));
        bool add_success = txn_store_.AddDumpIndexCmd(idx_segment_key, wal_dump_cmd.get());
        if (!add_success) {
            return Status::UnexpectedError(fmt::format("Add dump index command {} failed", idx_segment_key));
        }
        dump_cmd = wal_dump_cmd.get();
    }
    WalChunkIndexInfo wal_chunk_info;
    wal_chunk_info.base_name_ = chunk_info.base_name_;
    wal_chunk_info.paths_ = chunk_info.paths_;
    wal_chunk_info.base_rowid_ = chunk_info.base_rowid_;
    wal_chunk_info.row_count_ = chunk_info.row_count_;
    {
        std::unique_lock lock(rw_locker_);
        dump_cmd->chunk_infos_.push_back(std::move(wal_chunk_info));
    }

    LOG_TRACE(fmt::format("NewTxn::AddIndexChunk added index chunk: {}.{}.{}.{}", db_name, table_name, index_name, segment_id));
    return Status::OK();
}

Status NewTxn::DeprecateIndexChunk(const String &db_name,
                                   const String &table_name,
                                   const String &index_name,
                                   SegmentID segment_id,
                                   const Vector<ChunkID> &deprecate_ids) {
    this->CheckTxn(db_name);

    bool index_exist = this->CheckIndexExists(db_name, table_name, index_name);
    if (!index_exist) {
        return Status::IndexNotExist(index_name);
    }

    String idx_segment_key = KeyEncode::CatalogIdxSegmentKey(db_name, table_name, index_name, segment_id);
    WalCmdDumpIndex *dump_cmd = txn_store_.GetDumpIndexCmd(idx_segment_key);
    if (dump_cmd == nullptr) {
        auto wal_dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id);
        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(wal_dump_cmd));
        bool add_success = txn_store_.AddDumpIndexCmd(idx_segment_key, wal_dump_cmd.get());
        if (!add_success) {
            return Status::UnexpectedError(fmt::format("Add dump index command {} failed", idx_segment_key));
        }
        dump_cmd = wal_dump_cmd.get();
    }
    dump_cmd->deprecate_ids_.insert(dump_cmd->deprecate_ids_.end(), deprecate_ids.begin(), deprecate_ids.end());

    LOG_TRACE(fmt::format("NewTxn::DeprecateIndexChunk deprecated index chunk: {}.{}.{}.{}", db_name, table_name, index_name, segment_id));
    return Status::OK();
}

Status NewTxn::GetIndexChunks(const String &db_name,
                              const String &table_name,
                              const String &index_name,
                              SegmentID segment_id,
                              Vector<ChunkIndexInfo> &chunk_infos) {
    this->CheckTxn(db_name);

    bool index_exist = this->CheckIndexExists(db_name, table_name, index_name);
    if (!index_exist) {
        return Status::IndexNotExist(index_name);
    }

    String db_id_str;
    String table_id_str;
    String index_id_str;
    String index_key;
    Status status = GetIndexID(db_name, table_name, index_name, index_key, index_id_str, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    return GetIndexChunksByID(db_id_str, table_id_str, index_id_str, segment_id, chunk_infos);
}

Status NewTxn::GetIndexChunksByID(const String &db_id_str,
                                  const String &table_id_str,
                                  const String &index_id_str,
                                  SegmentID segment_id,
                                  Vector<ChunkIndexInfo> &chunk_infos) {
    String chunk_key_prefix = KeyEncode::CatalogIdxChunkPrefix(db_id_str, table_id_str, index_id_str, segment_id);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(chunk_key_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(chunk_key_prefix)) {
        String key_str = iter2->Key().ToString();
        size_t start = chunk_key_prefix.size();
        size_t end = key_str.find('|', start);
        ChunkID chunk_id = std::stoull(key_str.substr(start, end - start));
        String chunk_info_key = KeyEncode::CatalogIdxChunkTagKey(db_id_str, table_id_str, index_id_str, segment_id, chunk_id, "chunk_info");
        String chunk_info_str;
        Status status = kv_instance_->Get(chunk_info_key, chunk_info_str);
        if (!status.ok()) {
            return status;
        }
        ChunkIndexInfo chunk_info;
        nlohmann::json chunk_info_json = nlohmann::json::parse(chunk_info_str);
        bool parse_success = ChunkIndexInfo::FromJson(chunk_info_json, chunk_info);
        if (!parse_success) {
            return Status::UnexpectedError(fmt::format("Parse chunk info failed: {}", chunk_info_str));
        }
        chunk_infos.push_back(std::move(chunk_info));
        iter2->Next();
    }

    return Status::OK();
}

Tuple<TableIndexEntry *, Status> NewTxn::CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    TxnTimeStamp begin_ts = this->BeginTS();

    auto [table_index_entry, index_status] =
        catalog_->CreateIndex(table_entry, index_base, conflict_type, txn_context_ptr_->txn_id_, begin_ts, nullptr);
    if (table_index_entry == nullptr) { // nullptr means some exception happened
        return {nullptr, index_status};
    }

    const String &table_name = *table_entry->GetTableName();
    auto *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
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

Tuple<TableEntry *, Status> NewTxn::GetTableByName(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = this->BeginTS();

    return catalog_->GetTableByName(db_name, table_name, txn_context_ptr_->txn_id_, begin_ts);
}

Tuple<SharedPtr<TableInfo>, Status> NewTxn::GetTableInfo(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);
    SharedPtr<TableInfo> table_info = MakeShared<TableInfo>();
    table_info->db_name_ = MakeShared<String>(db_name);
    table_info->table_name_ = MakeShared<String>(table_name);
    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return {nullptr, status};
    }

    // Create table comment;
    String table_comment_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "comment");
    String table_comment;
    status = kv_instance_->Get(table_comment_key, table_comment);
    if (!status.ok() and status.code() != ErrorCode::kNotFound) {
        return {nullptr, status};
    }

    String db_dir;
    String db_dir_prefix = KeyEncode::CatalogDbTagKey(db_id_str, "dir");
    status = kv_instance_->Get(db_dir_prefix, db_dir);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    String table_storage_dir = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "dir");
    String table_storage_tail;
    status = kv_instance_->Get(table_storage_dir, table_storage_tail);
    if (!status.ok()) {
        return {nullptr, status};
    }

    table_info->table_full_dir_ =
        MakeShared<String>(fmt::format("{}/{}/{}", InfinityContext::instance().config()->DataDir(), db_dir, table_storage_tail));

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String column_key = iter2->Key().ToString();
        String column_value = iter2->Value().ToString();
        auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value));
        table_info->column_defs_.emplace_back(column_def);
        iter2->Next();
    }

    // sort table_info->column_defs_ by column_id
    std::sort(table_info->column_defs_.begin(), table_info->column_defs_.end(), [](const SharedPtr<ColumnDef> &a, const SharedPtr<ColumnDef> &b) {
        return a->id_ < b->id_;
    });

    return {table_info, Status::OK()};
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

    txn_store_.PrepareCommit1(); // Only for import and compact, pre-commit segment
    // LOG_INFO(fmt::format("NewTxn {} commit ts: {}", txn_context_ptr_->txn_id_, commit_ts));

    // if (const auto conflict_reason = txn_mgr_->CheckTxnConflict(this); conflict_reason) {
    //     LOG_ERROR(fmt::format("NewTxn: {} is rolled back. rollback ts: {}. NewTxn conflict reason: {}.",
    //                           txn_context_ptr_->txn_id_,
    //                           commit_ts,
    //                           *conflict_reason));
    //     wal_entry_ = nullptr;
    //     txn_mgr_->SendToWAL(this);
    //     return Status::TxnConflict(txn_context_ptr_->txn_id_, fmt::format("NewTxn conflict reason: {}.", *conflict_reason));
    // }

    Status status = this->PrepareCommit();
    if (!status.ok()) {
        wal_entry_ = nullptr;
        txn_mgr_->SendToWAL(this);
        return status;
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
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_DATABASE: {
                auto *create_db_cmd = static_cast<WalCmdCreateDatabase *>(command.get());
                Status status = CommitCreateDB(create_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabase *>(command.get());
                Status status = CommitDropDB(drop_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_TABLE: {
                auto *create_table_cmd = static_cast<WalCmdCreateTable *>(command.get());
                Status status = CommitCreateTable(create_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_TABLE: {
                auto *drop_table_cmd = static_cast<WalCmdDropTable *>(command.get());
                Status status = CommitDropTable(drop_table_cmd->db_name_, drop_table_cmd->table_name_);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS: {
                auto *add_column_cmd = static_cast<WalCmdAddColumns *>(command.get());
                Status status = CommitAddColumns(add_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS: {
                auto *drop_column_cmd = static_cast<WalCmdDropColumns *>(command.get());
                Status status = CommitDropColumns(drop_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_INDEX: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndex *>(command.get());
                Status status = CommitCreateIndex(create_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_INDEX: {
                auto *drop_index_cmd = static_cast<WalCmdDropIndex *>(command.get());
                Status status = CommitDropIndex(drop_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndex *>(command.get());
                Status status = CommitDumpIndex(dump_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::APPEND: {
                break;
            }
            default: {
                LOG_WARN(fmt::format("NewTxn::PrepareCommit Wal type not implemented: {}", static_cast<u8>(command_type)));
                break;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::GetDbID(const String &db_name, String &db_key, String &db_id) {
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
            error_db_keys.push_back(db_key);
        }
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No table, ignore it.
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

Status NewTxn::GetTableID(const String &db_name, const String &table_name, String &table_key, String &table_id_str, String &db_id_str) {

    // Get db_id
    String db_key;
    Status status = GetDbID(db_name, db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    String table_key_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_table_keys;
    while (iter2->Valid() && iter2->Key().starts_with(table_key_prefix)) {
        table_key = iter2->Key().ToString();
        table_id_str = iter2->Value().ToString();
        if (found_count > 0) {
            // Error branch
            error_table_keys.push_back(table_key);
        }
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No table, ignore it.
        return Status::TableNotExist(table_name);
    }

    if (!error_table_keys.empty()) {
        // join error_table_keys
        String error_table_keys_str =
            std::accumulate(std::next(error_table_keys.begin()), error_table_keys.end(), error_table_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple table keys: {}", error_table_keys_str));
    }

    return Status::OK();
}

Status NewTxn::GetIndexID(const String &db_name,
                          const String &table_name,
                          const String &index_name,
                          String &index_key,
                          String &index_id,
                          String &table_id,
                          String &db_id) {
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id, db_id);
    if (!status.ok()) {
        return status;
    }

    String index_key_prefix = KeyEncode::CatalogIndexPrefix(db_id, table_id, index_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(index_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_index_keys;
    while (iter2->Valid() && iter2->Key().starts_with(index_key_prefix)) {
        index_key = iter2->Key().ToString();
        index_id = iter2->Value().ToString();
        if (found_count > 0) {
            // Error branch
            error_index_keys.push_back(index_key);
        }
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        return Status::IndexNotExist(index_name);
    }

    if (!error_index_keys.empty()) {
        // join error_index_keys
        String error_index_keys_str =
            std::accumulate(std::next(error_index_keys.begin()), error_index_keys.end(), error_index_keys.front(), [](String a, String b) {
                return std::move(a) + ", " + std::move(b);
            });
        UnrecoverableError(fmt::format("Found multiple index keys: {}", error_index_keys_str));
    }

    return Status::OK();
}

Status NewTxn::GetColumnDefs(const String &db_id, const String &table_id, Vector<SharedPtr<ColumnDef>> &column_defs) {
    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id, table_id);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String column_key = iter2->Key().ToString();
        String column_value = iter2->Value().ToString();
        [[maybe_unused]] String column_name = column_key.substr(column_key.find_last_of('|') + 1);
        auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value));
        column_defs.emplace_back(column_def);
        iter2->Next();
    }
    return Status::OK();
}

Status NewTxn::CommitAddIdxChunkByCmd(const String &db_id,
                                      const String &table_id,
                                      const String &index_id,
                                      SegmentID segment_id,
                                      const WalChunkIndexInfo &chunk_info) {
    String chunk_info_key = KeyEncode::CatalogIdxChunkTagKey(db_id, table_id, index_id, segment_id, chunk_info.chunk_id_, "chunk_info");
    String chunk_info_str = ChunkIndexInfo(chunk_info).ToJson().dump();
    Status status = kv_instance_->Put(chunk_info_key, std::move(chunk_info_str));
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitDropIdxChunkByID(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id, ChunkID chunk_id) {
    String chunk_info_key = KeyEncode::CatalogIdxChunkTagKey(db_id, table_id, index_id, segment_id, chunk_id, "chunk_info");
    Status status = kv_instance_->Delete(chunk_info_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitCreateDB(const WalCmdCreateDatabase *create_db_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    // Get the latest database id of system
    String db_id_str;
    Status status = IncrLatestID(db_id_str, LATEST_DATABASE_ID);
    if (!status.ok()) {
        return status;
    }

    String db_key = KeyEncode::CatalogDbKey(create_db_cmd->db_name_, commit_ts);
    status = kv_instance_->Put(db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    String db_storage_dir = KeyEncode::CatalogDbTagKey(db_id_str, "dir");
    status = kv_instance_->Put(db_storage_dir, create_db_cmd->db_dir_tail_);
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
    return Status::OK();
}
Status NewTxn::CommitDropDB(const WalCmdDropDatabase *drop_db_cmd) {
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    String db_key_prefix = KeyEncode::CatalogDbPrefix(drop_db_cmd->db_name_);

    String db_id_str;
    String db_key;
    Status status = GetDbID(drop_db_cmd->db_name_, db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    // All tables in dropped DB will be cleanup by background thread
    //    // Find all tables of this database to drop
    //    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str);
    //    auto iter2 = kv_instance_->GetIterator();
    //    iter2->Seek(db_table_prefix);
    //
    //    while (iter2->Valid() && iter2->Key().starts_with(db_table_prefix)) {
    //        String table_key = iter2->Key().ToString();
    //        // extract table name
    //        size_t start = db_table_prefix.size();
    //        size_t end = table_key.find('|', start);
    //        String table_name = table_key.substr(start, end - start);
    //        LOG_TRACE(fmt::format("Drop table: {}.{}", drop_db_cmd->db_name_, table_name));
    //        Status status = CommitDropTable(drop_db_cmd->db_name_, table_name);
    //        if (!status.ok()) {
    //            return status;
    //        }
    //        iter2->Next();
    //    }

    LOG_TRACE(fmt::format("Drop database: {}", drop_db_cmd->db_name_));
    status = kv_instance_->Delete(db_key);
    if (!status.ok()) {
        return status;
    }

    // Delete db_dir, db_comment
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

    return Status::OK();
}

Status NewTxn::CommitCreateTable(const WalCmdCreateTable *create_table_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_table_cmd->db_name_;
    const String &table_name = *create_table_cmd->table_def_->table_name();

    // Get database ID
    String db_id_str;
    String db_key;
    Status status = GetDbID(db_name, db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    // Get latest table id and increase the id
    String table_id_str;
    status = IncrLatestID(table_id_str, LATEST_TABLE_ID);
    if (!status.ok()) {
        return status;
    }

    // Create table key value pair
    String table_key = KeyEncode::CatalogTableKey(db_id_str, table_name, commit_ts);
    status = kv_instance_->Put(table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }

    // Create table comment;
    if (create_table_cmd->table_def_->table_comment() != nullptr and !create_table_cmd->table_def_->table_comment()->empty()) {
        String &table_comment = *create_table_cmd->table_def_->table_comment();
        String table_comment_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "comment");
        status = kv_instance_->Put(table_comment_key, table_comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create table column id;
    String table_latest_column_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_COLUMN_ID.data());
    status = kv_instance_->Put(table_latest_column_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // Create table segment id;
    String table_latest_segment_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_SEGMENT_ID.data());
    status = kv_instance_->Put(table_latest_segment_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // Create table dir
    String table_storage_dir = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "dir");
    status = kv_instance_->Put(table_storage_dir, create_table_cmd->table_dir_tail_);
    if (!status.ok()) {
        return status;
    }

    status = CommitCreateTableDef(create_table_cmd->table_def_.get(), db_id_str, table_id_str);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitCreateTableDef(const TableDef *table_def, const String &db_id, const String &table_id) {
    for (const auto &column : table_def->columns()) {
        String column_key = KeyEncode::TableColumnKey(db_id, table_id, column->name());
        Status status = kv_instance_->Put(column_key, column->ToJson().dump());
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitDropTable(const String &db_name, const String &table_name) {
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    // delete table key
    status = kv_instance_->Delete(table_key);
    if (!status.ok()) {
        return status;
    }

    // delete table segment id;
    String table_latest_segment_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_SEGMENT_ID.data());
    status = kv_instance_->Delete(table_latest_segment_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table column id;
    String table_latest_column_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_COLUMN_ID.data());
    status = kv_instance_->Delete(table_latest_column_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table dir
    String table_storage_dir = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "dir");
    status = kv_instance_->Delete(table_storage_dir);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String table_comment_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "comment");
    status = kv_instance_->Delete(table_comment_key);
    if (!status.ok()) {
        return status;
    }

    status = CommitDropTableDef(db_id_str, table_id_str);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->DecreaseTableWriteCount(table_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitDropTableDef(const String &db_id, const String &table_id) {
    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id, table_id);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);

    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String table_column_key = iter2->Key().ToString();
        Status status = kv_instance_->Delete(table_column_key);
        if (!status.ok()) {
            return status;
        }
        iter2->Next();
    }

    return Status::OK();
}

Status NewTxn::CommitAddColumns(const WalCmdAddColumns *add_columns_cmd) {
    const String &db_name = add_columns_cmd->db_name_;
    const String &table_name = add_columns_cmd->table_name_;

    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    // TODO: Check the column id conflict

    String column_name_value;
    for (const auto &column : add_columns_cmd->column_defs_) {
        String column_key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column->name());
        Status status = kv_instance_->Get(column_key, column_name_value);
        if (status.code() == ErrorCode::kNotFound) {
            Status status = kv_instance_->Put(column_key, column->ToJson().dump());
            if (!status.ok()) {
                return status;
            }
        } else {
            return Status::DuplicateColumnName(column->name());
        }
    }
    return Status::OK();
}

Status NewTxn::CommitDropColumns(const WalCmdDropColumns *drop_columns_cmd) {
    const String &db_name = drop_columns_cmd->db_name_;
    const String &table_name = drop_columns_cmd->table_name_;

    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    String column_name_value;
    for (const auto &column_name : drop_columns_cmd->column_names_) {
        String column_key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column_name);
        Status status = kv_instance_->Get(column_key, column_name_value);
        if (status.ok()) {
            status = kv_instance_->Delete(column_key);
            if (!status.ok()) {
                return status;
            }
        } else {
            return Status::ColumnNotExist(column_name);
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCreateIndex(const WalCmdCreateIndex *create_index_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_index_cmd->db_name_;
    const String &table_name = create_index_cmd->table_name_;
    const String &index_name = *create_index_cmd->index_base_->index_name_;

    // Get table ID
    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    // Get latest index id and lock the id
    String index_id_str;
    status = IncrLatestID(index_id_str, LATEST_INDEX_ID);
    if (!status.ok()) {
        return status;
    }

    // Create index key value pair
    String index_key = KeyEncode::CatalogIndexKey(db_id_str, table_id_str, index_name, commit_ts);
    status = kv_instance_->Put(index_key, index_id_str);
    if (!status.ok()) {
        return status;
    }

    // Create index def;
    String index_def_key = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "index_def");
    status = kv_instance_->Put(index_def_key, create_index_cmd->index_base_->Serialize().dump());
    if (!status.ok()) {
        return status;
    }

    // create tags
    // ...

    // Create index dir
    String index_storage_dir = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "dir");
    status = kv_instance_->Put(index_storage_dir, create_index_cmd->index_dir_tail_);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitDropIndex(const WalCmdDropIndex *drop_index_cmd) {
    String db_id_str;
    String table_id_str;
    String index_id_str;
    String index_key;
    Status status = GetIndexID(drop_index_cmd->db_name_,
                               drop_index_cmd->table_name_,
                               drop_index_cmd->index_name_,
                               index_key,
                               index_id_str,
                               table_id_str,
                               db_id_str);
    if (!status.ok()) {
        return status;
    }

    // delete index key
    status = kv_instance_->Delete(index_key);
    if (!status.ok()) {
        return status;
    }

    // delete index index def
    String index_comment_key = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "index_def");
    status = kv_instance_->Delete(index_comment_key);
    if (!status.ok()) {
        return status;
    }

    // delete index tags
    // ...

    // delete index dir
    String index_storage_dir = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "dir");
    status = kv_instance_->Delete(index_storage_dir);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitAppend(const WalCmdAppend *append_cmd) {
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = append_cmd->db_name_;
    const String &table_name = append_cmd->table_name_;

    // Get table ID
    String db_id_str;
    String table_id_str;
    String table_key;
    Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }

    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);

    String table_dir_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "dir");
    String table_dir;
    status = kv_instance_->Get(table_dir_key, table_dir);
    if (!status.ok()) {
        return status;
    }

    AppendState *append_state = txn_table_store->GetAppendState();
    if (append_state == nullptr) {
        return Status::OK();
    }
    append_state->Finalize();
    if (append_state->Finished()) {
        return Status::OK();
    }

    String table_def_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "table_def");

    String table_latest_segment_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_SEGMENT_ID.data());
    String latest_segment_id_str;
    SegmentID latest_segment_id = 0;
    auto update_kv_instance = txn_mgr_->kv_store()->GetInstance();

    status = update_kv_instance->GetForUpdate(table_latest_segment_id_key, latest_segment_id_str);
    if (!status.ok()) {
        return status;
    }
    latest_segment_id = std::stoull(latest_segment_id_str);

    SizeT block_capacity = DEFAULT_BLOCK_CAPACITY;
    SizeT segment_capacity = DEFAULT_SEGMENT_CAPACITY;
    SizeT cur_block_row_count = 0;
    SizeT cur_segment_row_count = 0;
    while (!append_state->Finished()) {
        if (latest_segment_id == 0 || cur_segment_row_count >= segment_capacity) {
            this->AddNewSegment(db_id_str, table_id_str, latest_segment_id, segment_capacity, table_dir, txn_table_store);
            ++latest_segment_id;
            cur_segment_row_count = 0;
        } else {
            String segment_row_cnt_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, "row_cnt");
            String segment_row_cnt_str;
            status = kv_instance_->Get(segment_row_cnt_key, segment_row_cnt_str);
            if (!status.ok()) {
                return status;
            }
            cur_segment_row_count = std::stoull(segment_row_cnt_str);
        }
        String last_block_id_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, LATEST_BLOCK_ID.data());
        BlockID last_block_id = 0;
        String last_block_id_str;
        status = kv_instance_->GetForUpdate(last_block_id_key, last_block_id_str);
        if (!status.ok()) {
            return status;
        }
        last_block_id = std::stoull(last_block_id_str);
        if (last_block_id == 0 || cur_block_row_count >= block_capacity) {
            this->AddNewBlock(db_id_str, table_id_str, latest_segment_id - 1, last_block_id, block_capacity, table_dir, txn_table_store);
            ++last_block_id;
            cur_block_row_count = 0;
        } else {
            String block_row_cnt_key =
                KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, latest_segment_id - 1, last_block_id - 1, "row_cnt");
            String block_row_cnt_str;
            status = kv_instance_->Get(block_row_cnt_key, block_row_cnt_str);
            if (!status.ok()) {
                return status;
            }
            cur_block_row_count = std::stoull(block_row_cnt_str);
        }

        SizeT actual_append = 0;
        status =
            this->PrepareAppendInBlock(latest_segment_id - 1, last_block_id - 1, append_state, block_capacity, cur_block_row_count, actual_append);
        if (!status.ok()) {
            return status;
        }
        cur_block_row_count += actual_append;
        cur_segment_row_count += actual_append;

        String lastest_block_id_str = fmt::format("{}", last_block_id);
        kv_instance_->Put(last_block_id_key, lastest_block_id_str);
    }

    latest_segment_id_str = fmt::format("{}", latest_segment_id);
    update_kv_instance->Put(table_latest_segment_id_key, latest_segment_id_str);
    status = update_kv_instance->Commit();
    if (!status.ok()) {
        UnrecoverableError("KV instance commit failed");
    }

    return Status::OK();
}

Status NewTxn::CommitAppend(const WalCmdAppend *append_cmd) {
    const String &db_name = append_cmd->db_name_;
    const String &table_name = append_cmd->table_name_;
    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    const AppendState *append_state = txn_table_store->GetAppendState();
    if (append_state == nullptr) {
        return Status::OK();
    }

    for (const AppendRange &range : append_state->append_ranges_) {
        LOG_INFO(fmt::format("CCC {}", sizeof(range)));
        const DataBlock *data_block = append_state->blocks_[range.data_block_idx_].get();
        AppendInBlock(db_name,
                      table_name,
                      range.segment_id_,
                      range.block_id_,
                      range.start_offset_,
                      range.row_count_,
                      data_block,
                      range.data_block_offset_);
    }
    return Status::OK();
}

Status NewTxn::CommitDumpIndex(WalCmdDumpIndex *dump_index_cmd) {
    String db_id_str;
    String table_id_str;
    String index_id_str;
    String index_key;
    Status status = GetIndexID(dump_index_cmd->db_name_,
                               dump_index_cmd->table_name_,
                               dump_index_cmd->index_name_,
                               index_key,
                               index_id_str,
                               table_id_str,
                               db_id_str);
    if (!status.ok()) {
        return status;
    }

    String idx_segment_last_chunk_key =
        KeyEncode::CatalogIdxSegmentTagKey(db_id_str, table_id_str, index_id_str, dump_index_cmd->segment_id_, LATEST_CHUNK_ID.data());
    String last_chunk_id_str;
    ChunkID last_chunk_id = 0;
    status = kv_instance_->GetForUpdate(idx_segment_last_chunk_key, last_chunk_id_str);
    if (status.code() == ErrorCode::kNotFound) {
        kv_instance_->Put(idx_segment_last_chunk_key, "0");
        last_chunk_id_str = "0";
        last_chunk_id = 0;
    } else {
        last_chunk_id = std::stoull(last_chunk_id_str);
    }
    for (WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
        chunk_info.chunk_id_ = last_chunk_id++;
        CommitAddIdxChunkByCmd(db_id_str, table_id_str, index_id_str, dump_index_cmd->segment_id_, chunk_info);
    }
    last_chunk_id_str = fmt::format("{}", last_chunk_id);
    kv_instance_->Put(idx_segment_last_chunk_key, last_chunk_id_str);

    for (ChunkID deprecate_id : dump_index_cmd->deprecate_ids_) {
        CommitDropIdxChunkByID(db_id_str, table_id_str, index_id_str, dump_index_cmd->segment_id_, deprecate_id);
    }

    return Status::OK();
}

Status NewTxn::IncrLatestID(String &id_str, std::string_view id_name) const {
    String string_id;
    Status status = kv_instance_->Get(id_name.data(), string_id);
    if (!status.ok()) {
        return status;
    }
    SizeT id_num = std::stoull(string_id);
    ++id_num;
    id_str = fmt::format("{}", id_num);
    return kv_instance_->Put(id_name.data(), id_str);
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

    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::APPEND: {
                auto *append_cmd = static_cast<WalCmdAppend *>(command.get());
                Status status = PrepareCommitAppend(append_cmd);
                if (!status.ok()) {
                    UnrecoverableError("PrepareCommitAppend failed");
                }
                break;
            }
            default: {
                break;
            }
        }
    }

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
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND: {
                auto *append_cmd = static_cast<WalCmdAppend *>(wal_cmd.get());
                Status status = CommitAppend(append_cmd);
                if (!status.ok()) {
                    UnrecoverableError("CommitAppend failed");
                }
                break;
            }
            case WalCommandType::CHECKPOINT: {
                auto *checkpoint_cmd = static_cast<WalCmdCheckpoint *>(wal_cmd.get());
                if (checkpoint_cmd->is_full_checkpoint_) {
                    wal_manager->CommitFullCheckpoint(checkpoint_cmd->max_commit_ts_);
                } else {
                    wal_manager->CommitDeltaCheckpoint(checkpoint_cmd->max_commit_ts_);
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    for (const auto &extra_command : extra_commands_) {
        switch (extra_command->GetType()) {
            case ExtraCommandType::kDropTable: {
                DropTableCommand *drop_table_command = static_cast<DropTableCommand *>(extra_command.get());
                Status status = new_catalog_->DecreaseTableWriteCount(drop_table_command->table_key());
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
                break;
            }
            default: {
                break;
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

    for (const auto &extra_command : extra_commands_) {
        switch (extra_command->GetType()) {
            case ExtraCommandType::kLockTable: {
                // Not check the unlock result
                LockTableCommand *lock_table_command = static_cast<LockTableCommand *>(extra_command.get());
                new_catalog_->UnlockTable(lock_table_command->table_key());
                break;
            }
            case ExtraCommandType::kUnlockTable: {
                // Not check the lock result
                UnlockTableCommand *unlock_table_command = static_cast<UnlockTableCommand *>(extra_command.get());
                new_catalog_->LockTable(unlock_table_command->table_key());
                break;
            }
            case ExtraCommandType::kDropTable: {
                DropTableCommand *drop_table_command = static_cast<DropTableCommand *>(extra_command.get());
                Status status = new_catalog_->DecreaseTableWriteCount(drop_table_command->table_key());
                if (!status.ok()) {
                    return status;
                }
                break;
            }
        }
    }

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
    const String &table_name = *table_entry->GetTableName();
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);
    table_store->AddWriteTxnNum();
}

} // namespace infinity

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

import catalog_meta;
import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import meta_key;

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

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return Status(ErrorCode::kDuplicateDatabaseName, MakeUnique<String>(fmt::format("Database: {} already exists", db_name)));
    } else if (status.code() != ErrorCode::kDBNotExist) {
        return status;
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

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kDBNotExist) {
            return status;
        }
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return status;
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

Tuple<SharedPtr<DatabaseInfo>, Status> NewTxn::GetDatabaseInfo(const String &db_name) {
    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return {nullptr, status};
    }

    auto db_info = MakeShared<DatabaseInfo>();
    db_info->db_name_ = MakeShared<String>(db_name);

    status = db_meta->GetDatabaseInfo(*db_info);
    if (!status.ok()) {
        return {nullptr, status};
    }
    return {std::move(db_info), Status::OK()};
}

Status NewTxn::ListDatabase(Vector<String> &db_names) {
    Vector<String> *db_id_strs_ptr;
    Vector<String> *db_names_ptr;

    CatalogMeta catalog_meta(*kv_instance_);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    if (!status.ok()) {
        return status;
    }
    db_names = *db_names_ptr;
    return status;
}

// Table and Collection OPs
Status NewTxn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) {
    this->CheckTxn(db_name);

    return catalog_->GetTables(db_name, output_table_array, nullptr);
}

Status NewTxn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    String table_id_str;
    String table_key;
    status = db_meta->GetTableID(*table_def->table_name(), table_key, table_id_str);

    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return Status(ErrorCode::kDuplicateTableName, MakeUnique<String>(fmt::format("Table: {} already exists", *table_def->table_name())));
    } else if (status.code() != ErrorCode::kTableNotExist) {
        return status;
    }

    SharedPtr<String> local_table_dir = DetermineRandomPath(*table_def->table_name());
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTable>(db_name, *local_table_dir, table_def);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE("NewTxn::CreateTable created table entry is inserted.");
    return Status::OK();
}

// Status NewTxn::RenameTable(TableEntry *old_table_entry, const String &new_table_name) {
//     UnrecoverableError("Not implemented yet");
//     return Status::OK();
// }

Status NewTxn::AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs) {

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Construct added columns name map
    Set<SizeT> column_idx_set;
    Set<String> column_name_set;
    for (const auto &column_def : column_defs) {
        column_idx_set.insert(column_def->id());
        column_name_set.insert(column_def->name());
    }

    status = new_catalog_->ImmutateTable(table_key, txn_context_ptr_->txn_id_);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> old_column_defs;
    std::tie(old_column_defs, status) = table_meta->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    for (auto &column_def : *old_column_defs) {
        if (column_name_set.contains(column_def->name())) {
            return Status::DuplicateColumnName(column_def->name());
        }
        if (column_idx_set.contains(column_def->id())) {
            return Status::DuplicateColumnIndex(fmt::format("Duplicate table column index: {}", column_def->id()));
        }
    }

    // Generate add column cmd
    SharedPtr<WalCmdAddColumns> wal_command = MakeShared<WalCmdAddColumns>(db_name, table_name, column_defs);
    wal_command->table_key_ = table_key;
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return Status::OK();
}

Status NewTxn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) {

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> old_column_defs;
    std::tie(old_column_defs, status) = table_meta->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    Set<String> column_name_set;
    for (const auto &column_def : *old_column_defs) {
        column_name_set.insert(column_def->name());
    }
    for (const auto &column_name : column_names) {
        if (!column_name_set.contains(column_name)) {
            return Status::ColumnNotExist(column_name);
        }
    }

    status = new_catalog_->ImmutateTable(table_key, txn_context_ptr_->txn_id_);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<WalCmdDropColumns> wal_command = MakeShared<WalCmdDropColumns>(db_name, table_name, column_names);
    wal_command->table_key_ = table_key;
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

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    String table_key;
    String table_id_str;
    status = db_meta->GetTableID(table_name, table_key, table_id_str);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kTableNotExist) {
            return status;
        }
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return status;
    }

    status = new_catalog_->IncreaseTableWriteCount(table_key);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<WalCmdDropTable> wal_command = MakeShared<WalCmdDropTable>(db_name, table_name);
    wal_command->db_id_ = db_meta->db_id_str();
    wal_command->table_id_ = table_id_str;
    wal_command->table_key_ = table_key;
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE(fmt::format("NewTxn::DropTable dropped table: {}.{}", db_name, table_name));
    return Status::OK();
}

Status NewTxn::RenameTable(const String &db_name, const String &old_table_name, const String &new_table_name) {

    this->CheckTxnStatus();
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }

    {
        String table_id;
        String table_key;
        Status status = db_meta->GetTableID(new_table_name, table_key, table_id);

        if (status.ok()) {
            return Status(ErrorCode::kDuplicateTableName, MakeUnique<String>(fmt::format("Table: {} already exists", new_table_name)));
        } else if (status.code() != ErrorCode::kTableNotExist) {
            return status;
        }
    }

    String table_id_str;
    String table_key;
    status = db_meta->GetTableID(old_table_name, table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->IncreaseTableWriteCount(table_key);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<WalCmdRenameTable> wal_command = MakeShared<WalCmdRenameTable>(db_name, old_table_name, new_table_name);
    wal_command->old_table_key_ = table_key;
    wal_command->old_table_id_ = table_id_str;
    wal_command->old_db_id_ = db_meta->db_id_str();
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE(fmt::format("NewTxn::Rename table from {}.{} to {}.{}.", db_name, old_table_name, db_name, new_table_name));
    return Status::OK();
}

Status NewTxn::LockTable(const String &db_name, const String &table_name) {
    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->LockTable(table_key, txn_context_ptr_->txn_id_);
    if (status.ok()) {
        SharedPtr<LockTableCommand> extra_command = MakeShared<LockTableCommand>(table_key);
        extra_commands_.push_back(extra_command);
    }
    return status;
}

Status NewTxn::UnlockTable(const String &db_name, const String &table_name) {
    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->UnlockTable(table_key, txn_context_ptr_->txn_id_);
    if (status.ok()) {
        SharedPtr<UnlockTableCommand> extra_command = MakeShared<UnlockTableCommand>(table_key);
        extra_commands_.push_back(extra_command);
    }

    return status;
}

Status NewTxn::ListTable(const String &db_name, Vector<String> &table_names) {
    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    Vector<String> *table_id_strs_ptr = nullptr;
    Vector<String> *table_names_ptr = nullptr;
    status = db_meta->GetTableIDs(table_id_strs_ptr, &table_names_ptr);
    if (!status.ok()) {
        return status;
    }
    table_names = *table_names_ptr;
    return Status::OK();
}

// Index OPs

Status NewTxn::CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    String index_key;
    String index_id;
    status = table_meta->GetIndexID(*index_base->index_name_, index_key, index_id);
    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return Status(ErrorCode::kDuplicateIndexName, MakeUnique<String>(fmt::format("Index: {} already exists", *index_base->index_name_)));
    } else if (status.code() != ErrorCode::kIndexNotExist) {
        return status;
    }

    SharedPtr<WalCmdCreateIndex> wal_command = MakeShared<WalCmdCreateIndex>(db_name, table_name, index_base);
    wal_command->db_id_ = db_meta->db_id_str();
    wal_command->table_id_ = table_meta->table_id_str();
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

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    String index_key;
    String index_id;
    status = table_meta->GetIndexID(index_name, index_key, index_id);
    if (!status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return status;
    }

    SharedPtr<WalCmdDropIndex> wal_command = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
    wal_command->db_id_ = std::move(db_meta->db_id_str());
    wal_command->table_id_ = std::move(table_meta->table_id_str());
    wal_command->index_id_ = std::move(index_id);
    wal_command->index_key_ = std::move(index_key);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    LOG_TRACE(fmt::format("NewTxn::DropIndexByName dropped index: {}.{}.{}", db_name, table_name, index_name));
    return Status::OK();
}

Tuple<SharedPtr<TableInfo>, Status> NewTxn::GetTableInfo(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);
    SharedPtr<TableInfo> table_info = MakeShared<TableInfo>();
    table_info->db_name_ = MakeShared<String>(db_name);
    table_info->table_name_ = MakeShared<String>(table_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {nullptr, status};
    }
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        return {nullptr, status};
    }
    return {std::move(table_info), Status::OK()};
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
        return PostReadTxnCommit();
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

Status NewTxn::PostReadTxnCommit() {
    for (const auto &extra_command : extra_commands_) {
        switch (extra_command->GetType()) {
            case ExtraCommandType::kLockTable: {
                // Not check the unlock result
                LockTableCommand *lock_table_command = static_cast<LockTableCommand *>(extra_command.get());
                Status status = new_catalog_->CommitLockTable(lock_table_command->table_key(), txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case ExtraCommandType::kUnlockTable: {
                // Not check the lock result
                UnlockTableCommand *unlock_table_command = static_cast<UnlockTableCommand *>(extra_command.get());
                Status status = new_catalog_->CommitUnlockTable(unlock_table_command->table_key(), txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
        }
    }

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
                Status status = CommitDropTable(drop_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RENAME_TABLE: {
                auto *rename_table_cmd = static_cast<WalCmdRenameTable *>(command.get());
                Status status = CommitRenameTable(rename_table_cmd);
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
            case WalCommandType::DUMP_INDEX:
            case WalCommandType::APPEND:
            case WalCommandType::DELETE: {
                break;
            }
            case WalCommandType::IMPORT: {
                auto *import_cmd = static_cast<WalCmdImport *>(command.get());
                Status status = CommitImport(import_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::COMPACT: {
                auto *compact_cmd = static_cast<WalCmdCompact *>(command.get());
                Status status = CommitCompact(compact_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            default: {
                UnrecoverableError(fmt::format("NewTxn::PrepareCommit Wal type not implemented: {}", static_cast<u8>(command_type)));
                break;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::GetDBMeta(const String &db_name, Optional<DBMeeta> &db_meta, String *db_key_ptr) {
    CatalogMeta catalog_meta(*kv_instance_);
    String db_key;
    String db_id_str;
    Status status = catalog_meta.GetDBID(db_name, db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }
    db_meta.emplace(db_id_str, *kv_instance_);
    if (db_key_ptr) {
        *db_key_ptr = db_key;
    }
    return Status::OK();
}

Status NewTxn::GetTableMeta(const String &db_name,
                            const String &table_name,
                            Optional<DBMeeta> &db_meta,
                            Optional<TableMeeta> &table_meta,
                            String *table_key_ptr) {
    Status status;
    status = this->GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    status = GetTableMeta(table_name, *db_meta, table_meta, table_key_ptr);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::GetTableMeta(const String &table_name, DBMeeta &db_meta, Optional<TableMeeta> &table_meta, String *table_key_ptr) {
    String table_key;
    String table_id_str;
    Status status = db_meta.GetTableID(table_name, table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }
    table_meta.emplace(db_meta.db_id_str(), table_id_str, *kv_instance_);
    if (table_key_ptr) {
        *table_key_ptr = table_key;
    }
    return Status::OK();
}

Status NewTxn::GetTableIndexMeta(const String &db_name,
                                 const String &table_name,
                                 const String &index_name,
                                 Optional<DBMeeta> &db_meta,
                                 Optional<TableMeeta> &table_meta,
                                 Optional<TableIndexMeeta> &table_index_meta,
                                 String *index_key_ptr) {
    Status status;
    status = this->GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    status = GetTableMeta(table_name, *db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    status = GetTableIndexMeta(index_name, *table_meta, table_index_meta, index_key_ptr);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status
NewTxn::GetTableIndexMeta(const String &index_name, TableMeeta &table_meta, Optional<TableIndexMeeta> &table_index_meta, String *index_key_ptr) {
    String index_key;
    String index_id_str;
    Status status = table_meta.GetIndexID(index_name, index_key, index_id_str);
    if (!status.ok()) {
        return status;
    }
    table_index_meta.emplace(index_id_str, table_meta, *kv_instance_);
    if (index_key_ptr) {
        *index_key_ptr = index_key;
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

    Optional<DBMeeta> db_meta;
    const String *db_comment = create_db_cmd->db_comment_.empty() ? nullptr : &create_db_cmd->db_comment_;
    status = NewCatalog::AddNewDB(kv_instance_.get(), db_id_str, commit_ts, create_db_cmd->db_name_, db_comment, db_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}
Status NewTxn::CommitDropDB(const WalCmdDropDatabase *drop_db_cmd) {
    String db_key;
    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(drop_db_cmd->db_name_, db_meta, &db_key);
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE(fmt::format("Drop database: {}", drop_db_cmd->db_name_));
    status = kv_instance_->Delete(db_key);
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    new_catalog_->AddCleanedMeta(commit_ts, MakeUnique<DBMetaKey>(db_meta->db_id_str()));

    return Status::OK();
}

Status NewTxn::CommitCreateTable(const WalCmdCreateTable *create_table_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_table_cmd->db_name_;

    // Get database ID
    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }

    // Get latest table id and increase the id
    String table_id_str;
    status = IncrLatestID(table_id_str, LATEST_TABLE_ID);
    if (!status.ok()) {
        return status;
    }

    Optional<TableMeeta> table_meta;
    status = NewCatalog::AddNewTable(*db_meta, table_id_str, commit_ts, create_table_cmd->table_def_, table_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitDropTable(const WalCmdDropTable *drop_table_cmd) {
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_id_str = drop_table_cmd->db_id_;
    const String &table_id_str = drop_table_cmd->table_id_;
    const String &table_key = drop_table_cmd->table_key_;

    // delete table key
    Status status = kv_instance_->Delete(table_key);
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    new_catalog_->AddCleanedMeta(commit_ts, MakeUnique<TableMetaKey>(db_id_str, table_id_str));

    return Status::OK();
}

Status NewTxn::CommitRenameTable(const WalCmdRenameTable *rename_table_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &old_table_key = rename_table_cmd->old_table_key_;
    const String &table_id = rename_table_cmd->old_table_id_;
    const String &db_id = rename_table_cmd->old_db_id_;
    // delete table key
    Status status = kv_instance_->Delete(old_table_key);
    if (!status.ok()) {
        return status;
    }
    // create new table key
    String new_table_key = KeyEncode::CatalogTableKey(db_id, rename_table_cmd->new_table_name_, commit_ts);
    status = kv_instance_->Put(new_table_key, table_id);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitAddColumns(const WalCmdAddColumns *add_columns_cmd) {
    const String &db_name = add_columns_cmd->db_name_;
    const String &table_name = add_columns_cmd->table_name_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }

    for (const auto &column : add_columns_cmd->column_defs_) {
        Status status = table_meta->AddColumn(*column);
        if (!status.ok()) {
            return status;
        }
    }

    status = this->AddColumnsData(*table_meta, add_columns_cmd->column_defs_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::CommitDropColumns(const WalCmdDropColumns *drop_columns_cmd) {
    const String &db_name = drop_columns_cmd->db_name_;
    const String &table_name = drop_columns_cmd->table_name_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }

    for (const auto &column_name : drop_columns_cmd->column_names_) {
        Status status = table_meta->DropColumn(column_name);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitDropIndex(const WalCmdDropIndex *drop_index_cmd) {
    const String &db_id_str = drop_index_cmd->db_id_;
    const String &table_id_str = drop_index_cmd->table_id_;
    const String &index_id_str = drop_index_cmd->index_id_;
    const String &index_key = drop_index_cmd->index_key_;

    // delete index key
    Status status = kv_instance_->Delete(index_key);
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    new_catalog_->AddCleanedMeta(commit_ts, MakeUnique<TableIndexMetaKey>(db_id_str, table_id_str, index_id_str));

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
                Status status = CommitAppend(append_cmd);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("CommitAppend failed: {}", status.message()));
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
    // txn_store_.MaintainCompactionAlg();

    for (auto &sema : txn_store_.semas()) {
        sema->acquire();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND: {
                auto *append_cmd = static_cast<WalCmdAppend *>(wal_cmd.get());
                Status status = PostCommitAppend(append_cmd);
                if (!status.ok()) {
                    UnrecoverableError("PostCommitAppend failed");
                }
                break;
            }
            case WalCommandType::DELETE: {
                auto *delete_cmd = static_cast<WalCmdDelete *>(wal_cmd.get());
                Status status = PostCommitDelete(delete_cmd);
                if (!status.ok()) {
                    UnrecoverableError("PostCommitDelete failed");
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
            case WalCommandType::DROP_TABLE: {
                auto *cmd = static_cast<WalCmdDropTable *>(wal_cmd.get());
                Status status = new_catalog_->DecreaseTableWriteCount(cmd->table_key_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::RENAME_TABLE: {
                auto *cmd = static_cast<WalCmdRenameTable *>(wal_cmd.get());
                Status status = new_catalog_->DecreaseTableWriteCount(cmd->old_table_key_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS: {
                auto *cmd = static_cast<WalCmdDropColumns *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS: {
                auto *cmd = static_cast<WalCmdAddColumns *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::DUMP_INDEX: {
                auto *cmd = static_cast<WalCmdDumpIndex *>(wal_cmd.get());
                Status status = PostCommitDumpIndex(cmd);
                if (!status.ok()) {
                    UnrecoverableError("Fail to dump index");
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

    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::ADD_COLUMNS: {
                auto *cmd = static_cast<WalCmdDropColumns *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS: {
                auto *cmd = static_cast<WalCmdAddColumns *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::DROP_TABLE: {
                auto *cmd = static_cast<WalCmdDropTable *>(wal_cmd.get());
                Status status = new_catalog_->DecreaseTableWriteCount(cmd->table_key_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
                }
                break;
            }
            case WalCommandType::RENAME_TABLE: {
                auto *cmd = static_cast<WalCmdRenameTable *>(wal_cmd.get());
                Status status = new_catalog_->DecreaseTableWriteCount(cmd->old_table_key_);
                if (!status.ok()) {
                    UnrecoverableError("Fail to unlock table");
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
            case ExtraCommandType::kLockTable: {
                // Not check the unlock result
                LockTableCommand *lock_table_command = static_cast<LockTableCommand *>(extra_command.get());
                new_catalog_->RollbackLockTable(lock_table_command->table_key(), txn_context_ptr_->txn_id_);
                break;
            }
            case ExtraCommandType::kUnlockTable: {
                // Not check the lock result
                UnlockTableCommand *unlock_table_command = static_cast<UnlockTableCommand *>(extra_command.get());
                new_catalog_->RollbackUnlockTable(unlock_table_command->table_key(), txn_context_ptr_->txn_id_);
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

Status NewTxn::Cleanup(TxnTimeStamp ts, KVInstance *kv_instance) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    Vector<UniquePtr<MetaKey>> metas;
    new_catalog->GetCleanedMeta(ts, metas);

    for (auto &meta : metas) {
        switch (meta->type_) {
            case MetaKey::Type::kDB: {
                auto *db_meta_key = static_cast<DBMetaKey *>(meta.get());
                DBMeeta db_meta(db_meta_key->db_id_str_, *kv_instance);
                Status status = NewCatalog::CleanDB(db_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kTable: {
                auto *table_meta_key = static_cast<TableMetaKey *>(meta.get());
                TableMeeta table_meta(table_meta_key->db_id_str_, table_meta_key->table_id_str_, *kv_instance);
                Status status = NewCatalog::CleanTable(table_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kSegment: {
                auto *segment_meta_key = static_cast<SegmentMetaKey *>(meta.get());
                TableMeeta table_meta(segment_meta_key->db_id_str_, segment_meta_key->table_id_str_, *kv_instance);
                SegmentMeta segment_meta(segment_meta_key->segment_id_, table_meta, *kv_instance);
                Status status = NewCatalog::CleanSegment(segment_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kBlock: {
                auto *block_meta_key = static_cast<BlockMetaKey *>(meta.get());
                TableMeeta table_meta(block_meta_key->db_id_str_, block_meta_key->table_id_str_, *kv_instance);
                SegmentMeta segment_meta(block_meta_key->segment_id_, table_meta, *kv_instance);
                BlockMeta block_meta(block_meta_key->block_id_, segment_meta, *kv_instance);
                Status status = NewCatalog::CleanBlock(block_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kColumn: {
                auto *column_meta_key = static_cast<ColumnMetaKey *>(meta.get());
                TableMeeta table_meta(column_meta_key->db_id_str_, column_meta_key->table_id_str_, *kv_instance);
                SegmentMeta segment_meta(column_meta_key->segment_id_, table_meta, *kv_instance);
                BlockMeta block_meta(column_meta_key->block_id_, segment_meta, *kv_instance);
                ColumnMeta column_meta(column_meta_key->column_idx_, block_meta, *kv_instance);
                Status status = NewCatalog::CleanBlockColumn(column_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kTableIndex: {
                auto *table_index_meta_key = static_cast<TableIndexMetaKey *>(meta.get());
                TableMeeta table_meta(table_index_meta_key->db_id_str_, table_index_meta_key->table_id_str_, *kv_instance);
                TableIndexMeeta table_index_meta(table_index_meta_key->index_id_str_, table_meta, *kv_instance);
                Status status = NewCatalog::CleanTableIndex(table_index_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kSegmentIndex: {
                auto *segment_index_meta_key = static_cast<SegmentIndexMetaKey *>(meta.get());
                TableMeeta table_meta(segment_index_meta_key->db_id_str_, segment_index_meta_key->table_id_str_, *kv_instance);
                TableIndexMeeta table_index_meta(segment_index_meta_key->index_id_str_, table_meta, *kv_instance);
                SegmentIndexMeta segment_index_meta(segment_index_meta_key->segment_id_, table_index_meta, *kv_instance);
                Status status = NewCatalog::CleanSegmentIndex(segment_index_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kChunkIndex: {
                auto *chunk_index_meta_key = static_cast<ChunkIndexMetaKey *>(meta.get());
                TableMeeta table_meta(chunk_index_meta_key->db_id_str_, chunk_index_meta_key->table_id_str_, *kv_instance);
                TableIndexMeeta table_index_meta(chunk_index_meta_key->index_id_str_, table_meta, *kv_instance);
                SegmentIndexMeta segment_index_meta(chunk_index_meta_key->segment_id_, table_index_meta, *kv_instance);
                ChunkIndexMeta chunk_index_meta(chunk_index_meta_key->chunk_id_, segment_index_meta, *kv_instance);
                Status status = NewCatalog::CleanChunkIndex(chunk_index_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
        }
    }

    return Status::OK();
}

} // namespace infinity

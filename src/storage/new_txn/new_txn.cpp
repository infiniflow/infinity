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
import new_catalog;
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
import persist_result_handler;
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
import segment_entry;

namespace infinity {

NewTxn::NewTxn(NewTxnManager *txn_manager,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               UniquePtr<KVInstance> kv_instance,
               SharedPtr<String> txn_text,
               TransactionType txn_type)
    : txn_mgr_(txn_manager), buffer_mgr_(txn_mgr_->GetBufferMgr()), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
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

UniquePtr<NewTxn>
NewTxn::NewReplayTxn(NewTxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, UniquePtr<KVInstance> kv_instance) {
    auto txn = MakeUnique<NewTxn>(txn_mgr, txn_id, begin_ts, std::move(kv_instance), nullptr, TransactionType::kReplay);
    txn->txn_context_ptr_->commit_ts_ = commit_ts;
    return txn;
}

UniquePtr<NewTxn> NewTxn::NewRecoveryTxn(NewTxnManager *txn_mgr, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    KVStore *kv_code = txn_mgr->kv_store();
    UniquePtr<NewTxn> txn = MakeUnique<NewTxn>(txn_mgr, 0, begin_ts, kv_code->GetInstance(), nullptr, TransactionType::kRecovery);
    txn->txn_context_ptr_->commit_ts_ = commit_ts;
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
    }
    if (status.code() != ErrorCode::kDBNotExist) {
        return status;
    }

    String db_id;
    status = IncrLatestID(db_id, LATEST_DATABASE_ID);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateDatabaseV2>(db_name, db_id, *comment);
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

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropDatabaseV2>(db_name, db_meta->db_id_str());
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

    Vector<String> table_names;
    Status status = this->ListTable(db_name, table_names);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_name : table_names) {
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
        if (!status.ok()) {
            return status;
        }
        output_table_array.push_back(TableDetail{});
        status = table_meta->GetTableDetail(output_table_array.back(), db_name, table_name);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
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

    String table_id;
    status = IncrLatestID(table_id, LATEST_TABLE_ID);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<String> local_table_dir = DetermineRandomPath(*table_def->table_name());
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTableV2>(db_name, db_meta->db_id_str(), table_id, table_def);
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
    auto wal_command = MakeShared<WalCmdAddColumnsV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), column_defs);
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
    Map<String, ColumnID> column_name_set;
    for (const auto &column_def : *old_column_defs) {
        column_name_set.emplace(column_def->name(), column_def->id());
    }
    Vector<ColumnID> column_ids;
    for (const auto &column_name : column_names) {
        if (auto iter = column_name_set.find(column_name); iter != column_name_set.end()) {
            column_ids.push_back(iter->second);
        } else {
            return Status::ColumnNotExist(column_name);
        }
    }

    status = new_catalog_->ImmutateTable(table_key, txn_context_ptr_->txn_id_);
    if (!status.ok()) {
        return status;
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    status = table_meta->GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    for (const String &index_id : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id, *table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        if (!index_status.ok()) {
            status = new_catalog_->MutateTable(table_key, txn_context_ptr_->txn_id_);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Can't mutable table: {}, cause: {}", table_name, status.message()));
            }
            return index_status;
        }
        for (const String &column_name : column_names) {
            if (IsEqual(index_base->column_name(), column_name)) {
                status = new_catalog_->MutateTable(table_key, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Can't mutable table: {}, cause: {}", table_name, status.message()));
                }
                return Status::IndexOnColumn(column_name);
            }
        }
    }

    auto wal_command =
        MakeShared<WalCmdDropColumnsV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), column_names, column_ids);
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

    status = this->IncreaseTableReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    auto wal_command = MakeShared<WalCmdDropTableV2>(db_name, db_meta->db_id_str(), table_name, table_id_str);
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

    status = this->IncreaseTableReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    auto wal_command = MakeShared<WalCmdRenameTableV2>(db_name, db_meta->db_id_str(), old_table_name, table_id_str, new_table_name);
    wal_command->old_table_key_ = table_key;
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
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    status = this->IncreaseTableReferenceCount(table_key);
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

    // Get latest index id and lock the id
    String index_id_str;
    status = IncrLatestID(index_id_str, LATEST_INDEX_ID);
    if (!status.ok()) {
        return status;
    }

    auto wal_command =
        MakeShared<WalCmdCreateIndexV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), index_id_str, index_base);
    wal_command->table_key_ = table_key;
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
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    status = this->IncreaseTableReferenceCount(table_key);
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

    TableIndexMeeta table_index_meta(index_id, *table_meta);
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = table_meta->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    // Check if the column existence
    for (const auto &column_name : index_base->column_names_) {
        bool exist = false;
        for (const auto &column_def : *column_defs) {
            if (IsEqual(column_def->name(), column_name)) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            return Status::ColumnNotExist(column_name);
        }
    }

    auto wal_command = MakeShared<WalCmdDropIndexV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), index_name, index_id);
    wal_command->index_key_ = index_key;
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

Tuple<SharedPtr<TableIndexInfo>, Status> NewTxn::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SharedPtr<TableIndexInfo> table_index_info = MakeShared<TableIndexInfo>();
    status = table_index_meta->GetTableIndexInfo(*table_index_info);
    return {std::move(table_index_info), Status::OK()};
}

Tuple<SharedPtr<SegmentIndexInfo>, Status>
NewTxn::GetSegmentIndexInfo(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
    SharedPtr<SegmentIndexInfo> segment_index_info = segment_index_meta.GetSegmentIndexInfo();
    return {std::move(segment_index_info), Status::OK()};
}

Tuple<SharedPtr<ChunkIndexMetaInfo>, Status>
NewTxn::GetChunkIndexInfo(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, ChunkID chunk_id) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
    ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
    ChunkIndexMetaInfo *chunk_index_info_ptr;
    status = chunk_index_meta.GetChunkInfo(chunk_index_info_ptr);
    if (!status.ok()) {
        return {nullptr, status};
    }
    SharedPtr<ChunkIndexMetaInfo> chunk_index_info = MakeShared<ChunkIndexMetaInfo>(*chunk_index_info_ptr);
    return {std::move(chunk_index_info), status};
}

Tuple<SharedPtr<SegmentInfo>, Status> NewTxn::GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    SharedPtr<SegmentInfo> segment_info = MakeShared<SegmentInfo>();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {nullptr, status};
    }
    SegmentMeta segment_meta(segment_id, table_meta.value());
    std::tie(segment_info, status) = segment_meta.GetSegmentInfo();
    if (!status.ok()) {
        return {nullptr, status};
    }
    return {std::move(segment_info), Status::OK()};
}

Tuple<Vector<SharedPtr<SegmentInfo>>, Status> NewTxn::GetSegmentsInfo(const String &db_name, const String &table_name) {
    Vector<SharedPtr<SegmentInfo>> segment_info_list;
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs = nullptr;
    SegmentID unsealed_segment_id = 0;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {segment_info_list, status};
    }
    std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
    if (!status.ok()) {
        return {segment_info_list, status};
    }
    std::tie(column_defs, status) = table_meta->GetColumnDefs();
    if (!status.ok()) {
        return {segment_info_list, status};
    }
    status = table_meta->GetUnsealedSegmentID(unsealed_segment_id);
    if (!status.ok()) {
        return {segment_info_list, status};
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta.value());
        auto segment_info = MakeShared<SegmentInfo>();
        std::tie(segment_info, status) = segment_meta.GetSegmentInfo();
        if (!status.ok()) {
            return {segment_info_list, status};
        }
        segment_info_list.push_back(std::move(segment_info));
    }
    return {std::move(segment_info_list), status};
}

Tuple<SharedPtr<BlockInfo>, Status> NewTxn::GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id) {
    SharedPtr<SegmentInfo> segment_info = MakeShared<SegmentInfo>();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {nullptr, status};
    }
    SegmentMeta segment_meta(segment_id, table_meta.value());
    BlockMeta block_meta(block_id, segment_meta);
    return block_meta.GetBlockInfo();
}

Tuple<Vector<SharedPtr<BlockInfo>>, Status> NewTxn::GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    Vector<SharedPtr<BlockInfo>> block_info_list;
    SharedPtr<SegmentInfo> segment_info = MakeShared<SegmentInfo>();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {block_info_list, status};
    }
    SegmentMeta segment_meta(segment_id, table_meta.value());
    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return {block_info_list, status};
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        auto block_info = MakeShared<BlockInfo>();
        std::tie(block_info, status) = block_meta.GetBlockInfo();
        if (!status.ok()) {
            return {block_info_list, status};
        }
        block_info_list.push_back(std::move(block_info));
    }
    return {block_info_list, status};
}

Tuple<SharedPtr<BlockColumnInfo>, Status>
NewTxn::GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id) {
    SharedPtr<SegmentInfo> segment_info = MakeShared<SegmentInfo>();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {nullptr, status};
    }
    SegmentMeta segment_meta(segment_id, table_meta.value());
    BlockMeta block_meta(block_id, segment_meta);
    return block_meta.GetBlockColumnInfo(column_id);
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

TxnTimeStamp NewTxn::GetCurrentCkpTS() const {
    TransactionType txn_type = GetTxnType();
    if (txn_type != TransactionType::kNewCheckpoint) {
        UnrecoverableError(fmt::format("Expected transaction type is checkpoint."));
    }

    TxnState txn_state = NewTxn::GetTxnState();
    if (txn_state != TxnState::kCommitted) {
        UnrecoverableError(fmt::format("Expected transaction state is 'committed'."));
    }

    return current_ckp_ts_;
}

Status NewTxn::Checkpoint(TxnTimeStamp last_ckp_ts) {
    TransactionType txn_type = GetTxnType();
    if (txn_type != TransactionType::kNewCheckpoint) {
        UnrecoverableError(fmt::format("Expected transaction type is checkpoint."));
    }

    Status status;
    TxnTimeStamp checkpoint_ts = txn_context_ptr_->begin_ts_;
    CheckpointOption option{checkpoint_ts};

    current_ckp_ts_ = checkpoint_ts;

    if (last_ckp_ts >= checkpoint_ts) {
        return Status::OK();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    if (!wal_manager->SetCheckpointing()) {
        // Checkpointing
        return Status::OK();
    }
    DeferFn defer([&] { wal_manager->UnsetCheckpoint(); });

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(*kv_instance_);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, *kv_instance_);
        Status status = this->CheckpointDB(db_meta, option);
        if (!status.ok()) {
            return status;
        }
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize(true);
        handler.HandleWriteResult(result);
    }

    status = txn_mgr_->kv_store()->Flush();
    if (!status.ok()) {
        return status;
    }

    auto checkpoint_cmd = MakeShared<WalCmdCheckpointV2>(option.checkpoint_ts_);
    wal_entry_->cmds_.push_back(checkpoint_cmd);
    txn_context_ptr_->AddOperation(MakeShared<String>(checkpoint_cmd->ToString()));

    return Status::OK();
}

Status NewTxn::CheckpointDB(DBMeeta &db_meta, const CheckpointOption &option) {
    TxnTimeStamp begin_ts = this->BeginTS();

    Vector<String> *table_id_strs_ptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, *kv_instance_, begin_ts);
        Status status = this->CheckpointTable(table_meta, option);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::CheckpointTable(TableMeeta &table_meta, const CheckpointOption &option) {
    Status status = CheckpointTableData(table_meta, option);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

void NewTxn::AddMetaKeyForCommit(const String &key) { keys_wait_for_commit_.push_back(key); }

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
    if (wal_entry_->cmds_.empty() && txn_store_.ReadOnly() && !this->IsReplay()) {
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
    TxnTimeStamp commit_ts;
    if (this->IsReplay()) {
        commit_ts = this->CommitTS(); // Replayed from WAL
    } else {
        commit_ts = txn_mgr_->GetWriteCommitTS(shared_from_this());
    }
    LOG_TRACE(fmt::format("NewTxn: {} is committing, begin_ts:{} committing ts: {}", txn_context_ptr_->txn_id_, BeginTS(), commit_ts));

    this->SetTxnCommitting(commit_ts);

    txn_store_.PrepareCommit1(); // Only for import and compact, pre-commit segment
    // LOG_INFO(fmt::format("NewTxn {} commit ts: {}", txn_context_ptr_->txn_id_, commit_ts));

    Status status = this->PrepareCommit(commit_ts);

    // For non-replay transaction
    if (status.ok()) {
        String conflict_reason;
        bool conflict = txn_mgr_->CheckConflict1(this, conflict_reason);
        if (conflict) {
            status = Status::TxnConflict(txn_context_ptr_->txn_id_, fmt::format("NewTxn conflict reason: {}.", conflict_reason));
        }
    }

    if (!status.ok()) {
        // If prepare commit or conflict check failed, rollback the transaction
        this->SetTxnRollbacking(commit_ts);

        txn_mgr_->SendToWAL(this);

        // Wait until CommitTxnBottom is done.
        std::unique_lock<std::mutex> lk(commit_lock_);
        commit_cv_.wait(lk, [this] { return commit_bottom_done_; });

        PostRollback(commit_ts);
        return status;
    }

    // Try to commit the transaction
    status = kv_instance_->Commit();
    if (!status.ok()) {
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

Status NewTxn::CommitReplay() {

    TxnTimeStamp commit_ts = this->CommitTS(); // Replayed from WAL
    LOG_TRACE(fmt::format("NewTxn: {} is committing, begin_ts:{} committing ts: {}", txn_context_ptr_->txn_id_, BeginTS(), commit_ts));

    this->SetTxnCommitting(commit_ts);

    txn_store_.PrepareCommit1(); // Only for import and compact, pre-commit segment
    Status status = this->PrepareCommit(commit_ts);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Replay transaction, prepare commit: {}", status.message()));
    }

    // Try to commit the transaction
    status = kv_instance_->Commit();
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Replay transaction, commit: {}", status.message()));
    }
    PostCommit();

    return Status::OK();
}

Status NewTxn::CommitRecovery() {
    // Try to commit the rocksdb transaction
    Status status = kv_instance_->Commit();
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Replay transaction, commit: {}", status.message()));
    }
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

    // Restore the table write reference count
    for (const auto &ref_cnt_pair : table_write_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        //        LOG_INFO(fmt::format("DecreaseTableReferenceCount (commit): txn_id: {}, table_key: {}", this->TxnID(), table_key));
        Status status = new_catalog_->DecreaseTableWriteCount(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
        }
    }

    // Restore the mem index reference count
    for (const auto &ref_cnt_pair : mem_index_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        Status status = new_catalog_->DecreaseTableReferenceCountForMemIndex(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease mem index reference count on post commit phase: {}", status.message()));
        }
    }

    return Status::OK();
}

Status NewTxn::PrepareCommit(TxnTimeStamp commit_ts) {
    // TODO: for replayed transaction, meta data need to check if there is duplicated operation.
    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::DUMMY: {
                break;
            }
            case WalCommandType::CREATE_DATABASE_V2: {
                auto *create_db_cmd = static_cast<WalCmdCreateDatabaseV2 *>(command.get());
                Status status = CommitCreateDB(create_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(command.get());
                Status status = CommitDropDB(drop_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_TABLE_V2: {
                auto *create_table_cmd = static_cast<WalCmdCreateTableV2 *>(command.get());
                Status status = CommitCreateTable(create_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(command.get());
                Status status = CommitDropTable(drop_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RENAME_TABLE_V2: {
                auto *rename_table_cmd = static_cast<WalCmdRenameTableV2 *>(command.get());
                Status status = CommitRenameTable(rename_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_column_cmd = static_cast<WalCmdAddColumnsV2 *>(command.get());
                Status status = CommitAddColumns(add_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_column_cmd = static_cast<WalCmdDropColumnsV2 *>(command.get());
                Status status = CommitDropColumns(drop_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(command.get());
                Status status = CommitCreateIndex(create_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_INDEX_V2: {
                auto *drop_index_cmd = static_cast<WalCmdDropIndexV2 *>(command.get());
                Status status = CommitDropIndex(drop_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                // TODO: move follow to post commit
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(command.get());
                Status status = PostCommitDumpIndex(dump_index_cmd, kv_instance_.get());
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(command.get());

                /* Not retry now */

                // auto commit_append_func = [&]() -> Status {
                //     KVStore *kv_store = txn_mgr_->kv_store();
                //     UniquePtr<KVInstance> kv_instance = kv_store->GetInstance();
                //     Status status = CommitAppend(append_cmd, kv_instance.get());
                //     if (!status.ok()) {
                //         return status;
                //     }
                //     status = PostCommitAppend(append_cmd, kv_instance.get());
                //     if (!status.ok()) {
                //         return status;
                //     }
                //     status = kv_instance->Commit();
                //     if (!status.ok()) {
                //         return status;
                //     }
                //     return Status::OK();
                // };

                // SizeT attempt_count = 0;
                // Status status = Status::OK();
                // while (true) {
                //     attempt_count++;
                //     status = commit_append_func();
                //     if (status.ok()) {
                //         if (attempt_count > 0) {
                //             LOG_DEBUG(fmt::format("Append transaction retry count: {}", attempt_count));
                //         }
                //         break;
                //     } else {
                //         if (status.code() == ErrorCode::kRocksDBError && status.rocksdb_status_->IsBusy()) {
                //             LOG_DEBUG(fmt::format("Retry the append transaction, retry count: {}", attempt_count));
                //         } else {
                //             LOG_ERROR(fmt::format("Other append transaction error: {}", status.message()));
                //             return status;
                //         }
                //     }
                // }

                Status status = CommitAppend(append_cmd, kv_instance_.get());
                if (!status.ok()) {
                    return status;
                }
                status = PostCommitAppend(append_cmd, kv_instance_.get());
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DELETE_V2: {
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(command.get());

                Status status = PrepareCommitDelete(delete_cmd, kv_instance_.get());
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(command.get());
                Status status = CommitImport(import_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(command.get());
                Status status = CommitCompact(compact_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CHECKPOINT_V2: {
                auto *checkpoint_cmd = static_cast<WalCmdCheckpointV2 *>(command.get());
                Status status = CommitCheckpoint(checkpoint_cmd);
                if (!status.ok()) {
                    UnrecoverableError("Fail to checkpoint");
                }
                break;
            }
            case WalCommandType::OPTIMIZE_V2: {
                [[maybe_unused]] auto *optimize_cmd = static_cast<WalCmdOptimizeV2 *>(command.get());
                break;
            }
            default: {
                UnrecoverableError(fmt::format("NewTxn::PrepareCommit Wal type not implemented: {}", static_cast<u8>(command_type)));
                break;
            }
        }
    }

    String commit_ts_str = std::to_string(commit_ts);
    for (const String &meta_key : keys_wait_for_commit_) {
        kv_instance_->Put(meta_key, commit_ts_str);
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
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    String table_key;
    String table_id_str;
    Status status = db_meta.GetTableID(table_name, table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }
    table_meta.emplace(db_meta.db_id_str(), table_id_str, *kv_instance_, begin_ts);
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
                                 String *table_key_ptr,
                                 String *index_key_ptr) {
    Status status;
    status = this->GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    status = GetTableMeta(table_name, *db_meta, table_meta, table_key_ptr);
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
    table_index_meta.emplace(index_id_str, table_meta);
    if (index_key_ptr) {
        *index_key_ptr = index_key;
    }
    return Status::OK();
}

Status NewTxn::CommitCreateDB(const WalCmdCreateDatabaseV2 *create_db_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    Optional<DBMeeta> db_meta;
    const String *db_comment = create_db_cmd->db_comment_.empty() ? nullptr : &create_db_cmd->db_comment_;
    Status status = NewCatalog::AddNewDB(kv_instance_.get(), create_db_cmd->db_id_, commit_ts, create_db_cmd->db_name_, db_comment, db_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}
Status NewTxn::CommitDropDB(const WalCmdDropDatabaseV2 *drop_db_cmd) {
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
    new_catalog_->AddCleanedMeta(commit_ts, MakeUnique<DBMetaKey>(db_meta->db_id_str(), drop_db_cmd->db_name_));

    return Status::OK();
}

Status NewTxn::CommitCreateTable(const WalCmdCreateTableV2 *create_table_cmd) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_table_cmd->db_name_;

    // Get database ID
    Optional<DBMeeta> db_meta;
    Status status = GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }

    Optional<TableMeeta> table_meta;
    status = NewCatalog::AddNewTable(*db_meta, create_table_cmd->table_id_, begin_ts, commit_ts, create_table_cmd->table_def_, table_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitDropTable(const WalCmdDropTableV2 *drop_table_cmd) {
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
    new_catalog_->AddCleanedMeta(commit_ts, MakeUnique<TableMetaKey>(db_id_str, table_id_str, drop_table_cmd->table_name_));

    return Status::OK();
}

Status NewTxn::CommitRenameTable(const WalCmdRenameTableV2 *rename_table_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &old_table_key = rename_table_cmd->old_table_key_;
    const String &table_id = rename_table_cmd->table_id_;
    const String &db_id = rename_table_cmd->db_id_;
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

Status NewTxn::CommitAddColumns(const WalCmdAddColumnsV2 *add_columns_cmd) {
    const String &db_name = add_columns_cmd->db_name_;
    const String &table_name = add_columns_cmd->table_name_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }

    ColumnID next_column_id = 0;
    status = table_meta->GetNextColumnID(next_column_id);
    if (!status.ok()) {
        return status;
    }
    for (const auto &column : add_columns_cmd->column_defs_) {
        column->id_ = next_column_id++;
        Status status = table_meta->AddColumn(*column);
        if (!status.ok()) {
            return status;
        }
    }
    status = table_meta->SetNextColumnID(next_column_id);
    if (!status.ok()) {
        return status;
    }

    status = this->AddColumnsData(*table_meta, add_columns_cmd->column_defs_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::CommitDropColumns(const WalCmdDropColumnsV2 *drop_columns_cmd) {
    const String &db_name = drop_columns_cmd->db_name_;
    const String &table_name = drop_columns_cmd->table_name_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }

    status = this->DropColumnsData(*table_meta, drop_columns_cmd->column_ids_);
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

Status NewTxn::CommitCheckpoint(const WalCmdCheckpointV2 *checkpoint_cmd) {
    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(*kv_instance_);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, *kv_instance_);
        Status status = this->CommitCheckpointDB(db_meta, checkpoint_cmd);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCheckpointDB(DBMeeta &db_meta, const WalCmdCheckpointV2 *checkpoint_cmd) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Vector<String> *table_id_strs_ptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, *kv_instance_, begin_ts);
        Status status = this->CommitCheckpointTable(table_meta, checkpoint_cmd);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCheckpointTable(TableMeeta &table_meta, const WalCmdCheckpointV2 *checkpoint_cmd) {
    TxnTimeStamp checkpoint_ts = checkpoint_cmd->max_commit_ts_;
    Status status = CommitCheckpointTableData(table_meta, checkpoint_ts);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::IncrLatestID(String &id_str, std::string_view id_name) const {
    //    String string_id;
    //    Status status = kv_instance_->Get(id_name.data(), string_id);
    //    if (!status.ok()) {
    //        return status;
    //    }
    //    SizeT id_num = std::stoull(string_id);
    //    ++id_num;
    //    id_str = fmt::format("{}", id_num);
    //    return kv_instance_->Put(id_name.data(), id_str);
    return NewCatalog::IncrLatestID(kv_instance_.get(), id_str, id_name);
}

bool NewTxn::CheckConflictWithAppend(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    cause =
                        fmt::format("Create index {} on table {} in database {}.", *create_index_cmd->index_base_->index_name_, table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithImport(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    cause =
                        fmt::format("Create index {} on table {} in database {}.", *create_index_cmd->index_base_->index_name_, table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithAddColumns(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    cause = fmt::format("Compact table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithDropColumns(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    cause = fmt::format("Compact table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithCompact(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    cause =
                        fmt::format("Create index {} on table {} in database {}.", *create_index_cmd->index_base_->index_name_, table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_columns_cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                if (add_columns_cmd->db_name_ == db_name && add_columns_cmd->table_name_ == table_name) {
                    cause = fmt::format("Add columns on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (drop_columns_cmd->db_name_ == db_name && drop_columns_cmd->table_name_ == table_name) {
                    cause = fmt::format("Drop columns on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::DELETE_V2: {
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(wal_cmd.get());
                if (delete_cmd->db_name_ == db_name && delete_cmd->table_name_ == table_name) {
                    cause = fmt::format("Delete on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                switch (dump_index_cmd->dump_cause_) {
                    case DumpIndexCause::kOptimizeIndex: {
                        // Compact table conflicts with optimize index
                        if (dump_index_cmd->db_name_ == db_name && dump_index_cmd->table_name_ == table_name) {
                            cause = fmt::format("Optimize index on table {} in database {}.", table_name, db_name);
                            return true;
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithCreateIndex(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                if (append_cmd->db_name_ == db_name && append_cmd->table_name_ == table_name) {
                    cause = fmt::format("Append on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(wal_cmd.get());
                if (import_cmd->db_name_ == db_name && import_cmd->table_name_ == table_name) {
                    cause = fmt::format("Import on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    cause = fmt::format("Compact on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithOptimizeIndex(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    cause = fmt::format("Compact on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflictWithDelete(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause) {
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    cause = fmt::format("Compact on table {} in database {}.", table_name, db_name);
                    return true;
                }
                break;
            }
            default: {
                //
            }
        }
    }
    return false;
}

bool NewTxn::CheckConflict1(SharedPtr<NewTxn> check_txn, String &conflict_reason) {
    // LOG_INFO(fmt::format("Txn {} check conflict with txn: {}.", *txn_text_, *check_txn->txn_text_));
    bool conflict{false};
    for (SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();

        switch (command_type) {
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                conflict = CheckConflictWithAppend(append_cmd->db_name_, append_cmd->table_name_, check_txn.get(), conflict_reason);
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(wal_cmd.get());
                conflict = CheckConflictWithImport(import_cmd->db_name_, import_cmd->table_name_, check_txn.get(), conflict_reason);
                break;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                conflict = CheckConflictWithCompact(compact_cmd->db_name_, compact_cmd->table_name_, check_txn.get(), conflict_reason);
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                conflict = CheckConflictWithCreateIndex(create_index_cmd->db_name_, create_index_cmd->table_name_, check_txn.get(), conflict_reason);
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_columns_cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                conflict = CheckConflictWithAddColumns(add_columns_cmd->db_name_, add_columns_cmd->table_name_, check_txn.get(), conflict_reason);
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                bool conflict =
                    CheckConflictWithDropColumns(drop_columns_cmd->db_name_, drop_columns_cmd->table_name_, check_txn.get(), conflict_reason);
                if (conflict) {
                    return true;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                switch (dump_index_cmd->dump_cause_) {
                    case DumpIndexCause::kOptimizeIndex: {
                        conflict =
                            CheckConflictWithOptimizeIndex(dump_index_cmd->db_name_, dump_index_cmd->table_name_, check_txn.get(), conflict_reason);
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            case WalCommandType::DELETE_V2: {
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(wal_cmd.get());
                conflict = CheckConflictWithDelete(delete_cmd->db_name_, delete_cmd->table_name_, check_txn.get(), conflict_reason);

                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            conflicted_txn_ = check_txn;
            return true;
        }
    }
    return false;
}

void NewTxn::CommitBottom() {
    // update txn manager ts to commit_ts
    // erase txn_id from not_committed_txns_

    TransactionID txn_id = this->TxnID();
    TxnTimeStamp commit_ts = this->CommitTS();
    txn_mgr_->CommitBottom(commit_ts, txn_id);

    LOG_TRACE(fmt::format("NewTxn bottom: {} is started.", txn_context_ptr_->txn_id_));

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
    //    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND_V2: {
                // auto *cmd = static_cast<WalCmdAppend *>(wal_cmd.get());
                break;
            }
            case WalCommandType::IMPORT_V2: {
                // auto *cmd = static_cast<WalCmdImport *>(wal_cmd.get());
                break;
            }
            case WalCommandType::DELETE_V2: {
                //                auto *cmd = static_cast<WalCmdDelete *>(wal_cmd.get());
                break;
            }
            case WalCommandType::CHECKPOINT_V2: {
                // auto *checkpoint_cmd = static_cast<WalCmdCheckpoint *>(wal_cmd.get());
                // if (checkpoint_cmd->is_full_checkpoint_) {
                //     wal_manager->CommitFullCheckpoint(checkpoint_cmd->max_commit_ts_);
                // } else {
                //     wal_manager->CommitDeltaCheckpoint(checkpoint_cmd->max_commit_ts_);
                // }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                //                auto *cmd = static_cast<WalCmdDropTable *>(wal_cmd.get());
                break;
            }
            case WalCommandType::RENAME_TABLE_V2: {
                //                auto *cmd = static_cast<WalCmdRenameTable *>(wal_cmd.get());
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                //                auto *cmd = static_cast<WalCmdCreateIndex *>(wal_cmd.get());
                break;
            }
            case WalCommandType::DROP_INDEX_V2: {
                //                auto *cmd = static_cast<WalCmdDropIndex *>(wal_cmd.get());
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                if (!IsReplay()) {
                    Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
                    }
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (!IsReplay()) {
                    Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
                    }
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                if (cmd->dump_cause_ == DumpIndexCause::kDumpMemIndex) {
                    Status mem_index_status = new_catalog_->UnsetMemIndexDump(cmd->table_key_);
                    if (!mem_index_status.ok()) {
                        UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", cmd->table_name_, mem_index_status.message()));
                    }
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                // auto *cmd = static_cast<WalCmdCompact *>(wal_cmd.get());
                break;
            }
            default: {
                break;
            }
        }
    }

    // Restore the table write reference count
    for (const auto &ref_cnt_pair : table_write_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        //        LOG_INFO(fmt::format("DecreaseTableReferenceCount (commit): txn_id: {}, table_key: {}", this->TxnID(), table_key));
        Status status = new_catalog_->DecreaseTableWriteCount(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
        }
    }

    // Restore the mem index reference count
    for (const auto &ref_cnt_pair : mem_index_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        Status status = new_catalog_->DecreaseTableReferenceCountForMemIndex(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease mem index reference count on post commit phase: {}", status.message()));
        }
    }

    TransactionType txn_type = GetTxnType();
    if (txn_type == TransactionType::kNewCheckpoint) {
        // TODO: Shouldn't set the ckp ts if checkpoint is skipped.
        wal_manager->SetLastCheckpointTS(current_ckp_ts_);
        wal_manager->SetLastCkpWalSize(wal_size_); // Update last checkpoint wal size
    }

    SetCompletion();
}

void NewTxn::CancelCommitBottom() {
    this->SetTxnRollbacked();
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
}

Status NewTxn::PostRollback(TxnTimeStamp abort_ts) {
    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                Status status = new_catalog_->MutateTable(cmd->table_key_, txn_context_ptr_->txn_id_);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Fail to decrease table write count on post commit phase: {}", status.message()));
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                //                auto *cmd = static_cast<WalCmdDropTable *>(wal_cmd.get());
                break;
            }
            case WalCommandType::RENAME_TABLE_V2: {
                //                auto *cmd = static_cast<WalCmdRenameTable *>(wal_cmd.get());
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                auto *cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                Optional<DBMeeta> db_meta;
                Optional<TableMeeta> table_meta;
                Optional<TableIndexMeeta> table_index_meta;
                String table_key;
                String index_key;
                Status status = GetTableIndexMeta(cmd->db_name_,
                                                  cmd->table_name_,
                                                  *cmd->index_base_->index_name_,
                                                  db_meta,
                                                  table_meta,
                                                  table_index_meta,
                                                  &table_key,
                                                  &index_key);
                if (!status.ok()) {
                    return status;
                }

                status = NewCatalog::CleanTableIndex(*table_index_meta, *cmd->index_base_->index_name_, chunk_infos_);
                if (!status.ok()) {
                    return status;
                }

                Vector<String> object_paths;
                object_paths.reserve(chunk_infos_.size());
                String data_dir = InfinityContext::instance().config()->DataDir();
                SharedPtr<String> file_dir = table_index_meta->GetTableIndexDir();
                for (auto &chunk_info : chunk_infos_) {
                    if (chunk_info.db_id_ == cmd->db_id_ && chunk_info.table_id_ == cmd->table_id_) {
                        String object_path =
                            fmt::format("{}/{}/seg{}_chunk{}.idx", data_dir, *file_dir, chunk_info.segment_id_, chunk_info.chunk_id_);
                        object_paths.push_back(object_path);
                    }
                }
                buffer_mgr_->RemoveBufferObjects(object_paths);

                break;
            }
            case WalCommandType::DROP_INDEX_V2: {
                //                auto *cmd = static_cast<WalCmdDropIndex *>(wal_cmd.get());
                break;
            }
            case WalCommandType::APPEND_V2: {
                //                auto *cmd = static_cast<WalCmdAppend *>(wal_cmd.get());
                break;
            }
            case WalCommandType::DELETE_V2: {
                auto *cmd = static_cast<WalCmdDeleteV2 *>(wal_cmd.get());
                Status status = RollbackDelete(cmd, kv_instance_.get());
                if (!status.ok()) {
                    UnrecoverableError("Fail to rollback delete operation");
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                //                auto *cmd = static_cast<WalCmdImport *>(wal_cmd.get());
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                if (cmd->dump_cause_ == DumpIndexCause::kDumpMemIndex) {
                    Status mem_index_status = new_catalog_->UnsetMemIndexDump(cmd->table_key_);
                    if (!mem_index_status.ok()) {
                        UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", cmd->table_name_, mem_index_status.message()));
                    }
                }
                // Restore memory index here
                // TODO: Not implemented.
                UnrecoverableError("Not implemented");
                break;
            }
            case WalCommandType::COMPACT_V2: {
                //                auto *cmd = static_cast<WalCmdCompact *>(wal_cmd.get());
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

    // Restore the table write reference count
    for (const auto &ref_cnt_pair : table_write_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        //        LOG_INFO(fmt::format("DecreaseTableReferenceCount (rollback): txn_id: {}, table_key: {}", this->TxnID(), table_key));
        status = new_catalog_->DecreaseTableWriteCount(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease table write count on post rollback phase: {}", status.message()));
        }
    }

    // Restore the mem index reference count
    for (const auto &ref_cnt_pair : mem_index_reference_count_) {
        const auto &table_key = ref_cnt_pair.first;
        const auto &ref_cnt = ref_cnt_pair.second;
        status = new_catalog_->DecreaseTableReferenceCountForMemIndex(table_key, ref_cnt);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Fail to decrease mem index reference count on post rollback phase: {}", status.message()));
        }
    }

    if (conflicted_txn_ != nullptr) {
        // Wait for dependent transaction finished
        conflicted_txn_->WaitForCompletion();
    }

    SetCompletion();

    return Status::OK();
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

    Status status = PostRollback(abort_ts);

    LOG_TRACE(fmt::format("NewTxn: {} is dropped.", txn_context_ptr_->txn_id_));

    return status;
}

// the max_commit_ts is determined by the max commit ts of flushed delta entry
// Incremental checkpoint contains only the difference in status between the last checkpoint and this checkpoint (that is, "increment")
bool NewTxn::DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts) {
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpointV2>(max_commit_ts);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    return true;
}

// those whose commit_ts is <= max_commit_ts will be checkpointed
void NewTxn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {
    String full_path, full_name;

    catalog_->SaveFullCatalog(max_commit_ts, full_path, full_name);

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpointV2>(max_commit_ts);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
}

void NewTxn::AddWriteTxnNum(TableEntry *table_entry) {
    const String &table_name = *table_entry->GetTableName();
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);
    table_store->AddWriteTxnNum();
}

Status NewTxn::Cleanup(TxnTimeStamp ts, KVInstance *kv_instance) {
    TxnTimeStamp begin_ts = ts;
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    Vector<UniquePtr<MetaKey>> metas;
    new_catalog->GetCleanedMeta(ts, metas);

    for (auto &meta : metas) {
        switch (meta->type_) {
            case MetaKey::Type::kDB: {
                auto *db_meta_key = static_cast<DBMetaKey *>(meta.get());
                DBMeeta db_meta(db_meta_key->db_id_str_, *kv_instance);
                Status status = NewCatalog::CleanDB(db_meta, db_meta_key->db_name_, ts);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kTable: {
                auto *table_meta_key = static_cast<TableMetaKey *>(meta.get());
                TableMeeta table_meta(table_meta_key->db_id_str_, table_meta_key->table_id_str_, *kv_instance, begin_ts);
                Status status = NewCatalog::CleanTable(table_meta, table_meta_key->table_name_, ts);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kSegment: {
                auto *segment_meta_key = static_cast<SegmentMetaKey *>(meta.get());
                TableMeeta table_meta(segment_meta_key->db_id_str_, segment_meta_key->table_id_str_, *kv_instance, begin_ts);
                SegmentMeta segment_meta(segment_meta_key->segment_id_, table_meta);
                Status status = NewCatalog::CleanSegment(segment_meta, ts);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kBlock: {
                auto *block_meta_key = static_cast<BlockMetaKey *>(meta.get());
                TableMeeta table_meta(block_meta_key->db_id_str_, block_meta_key->table_id_str_, *kv_instance, begin_ts);
                SegmentMeta segment_meta(block_meta_key->segment_id_, table_meta);
                BlockMeta block_meta(block_meta_key->block_id_, segment_meta);
                Status status = NewCatalog::CleanBlock(block_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kColumn: {
                auto *column_meta_key = static_cast<ColumnMetaKey *>(meta.get());
                TableMeeta table_meta(column_meta_key->db_id_str_, column_meta_key->table_id_str_, *kv_instance, begin_ts);
                SegmentMeta segment_meta(column_meta_key->segment_id_, table_meta);
                BlockMeta block_meta(column_meta_key->block_id_, segment_meta);
                ColumnMeta column_meta(column_meta_key->column_def_->id(), block_meta);
                Status status = NewCatalog::CleanBlockColumn(column_meta, column_meta_key->column_def_.get());
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kTableIndex: {
                auto *table_index_meta_key = static_cast<TableIndexMetaKey *>(meta.get());
                TableMeeta table_meta(table_index_meta_key->db_id_str_, table_index_meta_key->table_id_str_, *kv_instance, begin_ts);
                TableIndexMeeta table_index_meta(table_index_meta_key->index_id_str_, table_meta);
                Status status = NewCatalog::CleanTableIndex(table_index_meta, table_index_meta_key->index_name_);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kSegmentIndex: {
                auto *segment_index_meta_key = static_cast<SegmentIndexMetaKey *>(meta.get());
                TableMeeta table_meta(segment_index_meta_key->db_id_str_, segment_index_meta_key->table_id_str_, *kv_instance, begin_ts);
                TableIndexMeeta table_index_meta(segment_index_meta_key->index_id_str_, table_meta);
                SegmentIndexMeta segment_index_meta(segment_index_meta_key->segment_id_, table_index_meta);
                Status status = NewCatalog::CleanSegmentIndex(segment_index_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaKey::Type::kChunkIndex: {
                auto *chunk_index_meta_key = static_cast<ChunkIndexMetaKey *>(meta.get());
                TableMeeta table_meta(chunk_index_meta_key->db_id_str_, chunk_index_meta_key->table_id_str_, *kv_instance, begin_ts);
                TableIndexMeeta table_index_meta(chunk_index_meta_key->index_id_str_, table_meta);
                SegmentIndexMeta segment_index_meta(chunk_index_meta_key->segment_id_, table_index_meta);
                ChunkIndexMeta chunk_index_meta(chunk_index_meta_key->chunk_id_, segment_index_meta);
                Status status = NewCatalog::CleanChunkIndex(chunk_index_meta);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
        }
    }

    buffer_mgr->RemoveClean();

    return Status::OK();
}

bool NewTxn::IsReplay() const { return txn_context_ptr_->txn_type_ == TransactionType::kReplay; }

Status NewTxn::ReplayWalCmd(const SharedPtr<WalCmd> &command) {
    WalCommandType command_type = command->GetType();
    switch (command_type) {
        case WalCommandType::CREATE_INDEX_V2: {
            auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(command.get());

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;
            Status status = GetTableMeta(create_index_cmd->db_name_, create_index_cmd->table_name_, db_meta, table_meta, &table_key);
            if (!status.ok()) {
                return status;
            }
            create_index_cmd->db_id_ = db_meta->db_id_str();
            create_index_cmd->table_id_ = table_meta->table_id_str();
            create_index_cmd->table_key_ = std::move(table_key);
            break;
        }
        case WalCommandType::APPEND_V2: {
            auto *append_cmd = static_cast<WalCmdAppendV2 *>(command.get());

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;
            Status status = GetTableMeta(append_cmd->db_name_, append_cmd->table_name_, db_meta, table_meta, &table_key);
            if (!status.ok()) {
                return status;
            }
            append_cmd->db_id_ = db_meta->db_id_str();
            append_cmd->table_id_ = table_meta->table_id_str();
            break;
        }
        case WalCommandType::DELETE_V2: {
            auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(command.get());

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = GetTableMeta(delete_cmd->db_name_, delete_cmd->table_name_, db_meta, table_meta);
            if (!status.ok()) {
                return status;
            }
            delete_cmd->db_id_ = db_meta->db_id_str();
            delete_cmd->table_id_ = table_meta->table_id_str();
            break;
        }
        case WalCommandType::IMPORT_V2: {
            auto *import_cmd = static_cast<WalCmdImportV2 *>(command.get());

            Status status = ReplayImport(import_cmd);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::DUMP_INDEX_V2: {
            auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(command.get());

            Status status = ReplayDumpIndex(dump_index_cmd);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::COMPACT_V2: {
            auto *compact_cmd = static_cast<WalCmdCompactV2 *>(command.get());

            Status status = ReplayCompact(compact_cmd);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::OPTIMIZE_V2: {
            auto *optimize_cmd = static_cast<WalCmdOptimizeV2 *>(command.get());

            Status status = ReplayOptimizeIndeByParams(optimize_cmd);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        default: {
            break;
        }
    }
    wal_entry_->cmds_.push_back(command);
    return Status::OK();
}

Status NewTxn::GetDBFilePaths(const String &db_name, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Status status = this->GetDBMeta(db_name, db_meta);
    if (!status.ok()) {
        return status;
    }
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    return NewCatalog::GetDBFilePaths(begin_ts, *db_meta, file_paths);
}

Status NewTxn::GetTableFilePaths(const String &db_name, const String &table_name, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    return NewCatalog::GetTableFilePaths(begin_ts, *table_meta, file_paths);
}

Status NewTxn::GetSegmentFilePaths(const String &db_name, const String &table_name, SegmentID segment_id, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    SegmentMeta segment_meta(segment_id, *table_meta);
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    return NewCatalog::GetSegmentFilePaths(begin_ts, segment_meta, file_paths);
}

Status
NewTxn::GetBlockFilePaths(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    SegmentMeta segment_meta(segment_id, *table_meta);
    BlockMeta block_meta(block_id, segment_meta);
    return NewCatalog::GetBlockFilePaths(block_meta, file_paths);
}

Status NewTxn::GetBlockColumnFilePaths(const String &db_name,
                                       const String &table_name,
                                       SegmentID segment_id,
                                       BlockID block_id,
                                       ColumnID column_id,
                                       Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    SegmentMeta segment_meta(segment_id, *table_meta);
    BlockMeta block_meta(block_id, segment_meta);
    ColumnMeta column_meta(column_id, block_meta);
    return NewCatalog::GetBlockColumnFilePaths(column_meta, file_paths);
}

Status NewTxn::GetColumnFilePaths(const String &db_name, const String &table_name, const String &column_name, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    SharedPtr<ColumnDef> column_def;
    {
        auto [column_defs, status] = table_meta->GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        for (auto &column : *column_defs) {
            if (column->name() == column_name) {
                column_def = column;
                break;
            }
        }
        if (!column_def) {
            UnrecoverableError(fmt::format("Column {} not found in table {}", column_name, table_name));
        }
    }
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    return NewCatalog::GetColumnFilePaths(begin_ts, *table_meta, column_def, file_paths);
}

Status NewTxn::GetTableIndexFilePaths(const String &db_name, const String &table_name, const String &index_name, Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = this->GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return status;
    }
    return NewCatalog::GetTableIndexFilePaths(*table_index_meta, file_paths);
}

Status NewTxn::GetSegmentIndexFilepaths(const String &db_name,
                                        const String &table_name,
                                        const String &index_name,
                                        SegmentID segment_id,
                                        Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = this->GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return status;
    }
    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
    return NewCatalog::GetSegmentIndexFilepaths(segment_index_meta, file_paths);
}

Status NewTxn::GetChunkIndexFilePaths(const String &db_name,
                                      const String &table_name,
                                      const String &index_name,
                                      SegmentID segment_id,
                                      ChunkID chunk_id,
                                      Vector<String> &file_paths) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    Status status = this->GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return status;
    }
    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
    ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
    return NewCatalog::GetChunkIndexFilePaths(chunk_index_meta, file_paths);
}

Status NewTxn::IncreaseTableReferenceCount(const String &table_key) {
    //    LOG_INFO(fmt::format("IncreaseTableReferenceCount: txn_id: {}, table_key: {}", this->TxnID(), table_key));
    Status status = new_catalog_->IncreaseTableWriteCount(table_key);
    if (status.ok()) {
        ++table_write_reference_count_[table_key];
    }
    return status;
}

SizeT NewTxn::GetTableReferenceCount(const String &table_key) {
    if (table_write_reference_count_.find(table_key) == table_write_reference_count_.end()) {
        return 0;
    }
    return table_write_reference_count_[table_key];
}

Status NewTxn::IncreaseMemIndexReferenceCount(const String &table_key) {
    Status status = new_catalog_->IncreaseTableReferenceCountForMemIndex(table_key);
    if (status.ok()) {
        ++mem_index_reference_count_[table_key];
    }
    return status;
}

SizeT NewTxn::GetMemIndexReferenceCount(const String &table_key) {
    if (mem_index_reference_count_.find(table_key) == mem_index_reference_count_.end()) {
        return 0;
    }
    return mem_index_reference_count_[table_key];
}

Status NewTxn::Dummy() {
    wal_entry_->cmds_.push_back(MakeShared<WalCmdDummy>());
    return Status::OK();
}

void NewTxn::SetWalSize(i64 wal_size) {
    TransactionType txn_type = GetTxnType();
    if (txn_type != TransactionType::kNewCheckpoint) {
        UnrecoverableError(fmt::format("Expected transaction type is checkpoint."));
    }
    wal_size_ = wal_size;
}

void NewTxn::SetCompletion() {
    std::unique_lock<std::mutex> lock(finished_mutex_);
    finished_ = true;
    finished_cv_.notify_all();
}

void NewTxn::WaitForCompletion() {
    std::unique_lock<std::mutex> lock(finished_mutex_);
    finished_cv_.wait(lock, [this] { return finished_; });
}

} // namespace infinity

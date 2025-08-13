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

#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

module infinity_core:new_txn.impl;

import :new_txn;
import :stl;
import :new_catalog;
import :infinity_exception;
import :new_txn_manager;
import :buffer_manager;
import :wal_entry;
import :third_party;
import :logger;
import :data_block;
import :txn_state;

import :data_access_state;
import :status;
import :meta_info;

import :database_detail;
import :status;
import :table_def;
import :index_base;
import :bg_task;
import :background_process;
import :base_table_ref;
import compact_statement;
import :default_values;
import :memory_indexer;
import :persistence_manager;
import :persist_result_handler;
import :infinity_context;
import admin_statement;
import global_resource_usage;
import :wal_manager;
import :defer_op;
import :snapshot_info;
import :kv_store;
import :random;
import :kv_code;
import constant_expr;
import :buffer_obj;
import :data_file_worker;
import :version_file_worker;
import :block_version;
import :catalog_meta;
import :db_meeta;
import :table_meeta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :meta_key;
import :txn_allocator_task;
import :meta_type;
import :base_txn_store;
import :buffer_handle;
import :virtual_store;
import :txn_context;
import :kv_utility;
import extra_ddl_info;
import column_def;
import row_id;

namespace infinity {

NewTxn::NewTxn(NewTxnManager *txn_manager,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               TxnTimeStamp last_kv_commit_ts,
               TxnTimeStamp last_commit_ts,
               UniquePtr<KVInstance> kv_instance,
               SharedPtr<String> txn_text,
               TransactionType txn_type)
    : txn_mgr_(txn_manager), buffer_mgr_(txn_mgr_->GetBufferMgr()), wal_entry_(MakeShared<WalEntry>()), kv_instance_(std::move(kv_instance)),
      txn_text_(std::move(txn_text)) {
    new_catalog_ = InfinityContext::instance().storage()->new_catalog();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("NewTxn");
#endif
    txn_context_ptr_ = TxnContext::Make();
    txn_context_ptr_->txn_id_ = txn_id;
    txn_context_ptr_->begin_ts_ = begin_ts;
    txn_context_ptr_->last_kv_commit_ts_ = last_kv_commit_ts;
    txn_context_ptr_->last_commit_ts_ = last_commit_ts;
    txn_context_ptr_->text_ = txn_text_;
    txn_context_ptr_->txn_type_ = txn_type;
}

NewTxn::NewTxn(BufferManager *buffer_mgr,
               NewTxnManager *txn_mgr,
               TransactionID txn_id,
               TxnTimeStamp begin_ts,
               UniquePtr<KVInstance> kv_instance,
               TransactionType txn_type)
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), wal_entry_(MakeShared<WalEntry>()), kv_instance_(std::move(kv_instance)) {
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
    TxnTimeStamp last_kv_commit_ts = commit_ts + 1; // The last kv commit ts is commit_ts + 1
    auto txn = MakeUnique<NewTxn>(txn_mgr, txn_id, begin_ts, last_kv_commit_ts, commit_ts, std::move(kv_instance), nullptr, TransactionType::kReplay);
    txn->txn_context_ptr_->commit_ts_ = commit_ts;
    return txn;
}

UniquePtr<NewTxn> NewTxn::NewRecoveryTxn(NewTxnManager *txn_mgr, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    KVStore *kv_code = txn_mgr->kv_store();
    UniquePtr<NewTxn> txn =
        MakeUnique<NewTxn>(txn_mgr, 0, begin_ts, commit_ts + 1, commit_ts, kv_code->GetInstance(), nullptr, TransactionType::kRecovery);
    txn->txn_context_ptr_->commit_ts_ = commit_ts;
    return txn;
}

NewTxn::~NewTxn() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("NewTxn");
#endif
}

TransactionID NewTxn::TxnID() const { return txn_context_ptr_->txn_id_; }

void NewTxn::AddOperation(const SharedPtr<String> &operation_text) { txn_context_ptr_->AddOperation(operation_text); }
Vector<SharedPtr<String>> NewTxn::GetOperations() const { return txn_context_ptr_->GetOperations(); }

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
    this->SetTxnType(TransactionType::kCreateDB);

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    String db_id_str;
    Status status = IncrLatestID(db_id_str, NEXT_DATABASE_ID);
    LOG_TRACE(fmt::format("txn: {}, create db, apply db_id: {}", txn_context_ptr_->txn_id_, db_id_str));
    if (!status.ok()) {
        return Status(status.code(), MakeUnique<String>(fmt::format("Fail to fetch next database id, {}", status.message())));
    }

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return Status::DuplicateDatabase(db_name);
    }
    if (status.code() != ErrorCode::kDBNotExist) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<CreateDBTxnStore>();
    CreateDBTxnStore *txn_store = static_cast<CreateDBTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->comment_ptr_ = comment;
    txn_store->db_id_str_ = db_id_str;
    txn_store->db_id_ = std::stoull(db_id_str);
    return Status::OK();
}

Status NewTxn::ReplayCreateDb(WalCmdCreateDatabaseV2 *create_db_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // Check if the database already exists in kv store
    String db_key = KeyEncode::CatalogDbKey(create_db_cmd->db_name_, commit_ts);
    String db_id;
    Status status = kv_instance_->Get(db_key, db_id);
    if (status.ok()) {
        if (db_id == create_db_cmd->db_id_) {
            LOG_WARN(fmt::format("Skipping replay create db: Database {} with id {} already exists, commit ts: {}, txn: {}.",
                                 create_db_cmd->db_name_,
                                 create_db_cmd->db_id_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay create db: Database {} with id {} already exists with different id {}, commit ts: {}, txn: {}.",
                                  create_db_cmd->db_name_,
                                  create_db_cmd->db_id_,
                                  db_id,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Database ID mismatch during replay of database creation.");
        }
    }

    // If the database does not exist, create it
    status = PrepareCommitCreateDB(create_db_cmd);
    if (!status.ok()) {
        return status;
    }

    // Update next db id if necessary
    String current_next_db_id_str;
    status = kv_instance_->Get(NEXT_DATABASE_ID.data(), current_next_db_id_str);
    if (!status.ok()) {
        return status;
    }

    u64 current_next_db_id = std::stoull(current_next_db_id_str);
    u64 this_db_id = std::stoull(create_db_cmd->db_id_);
    if (this_db_id + 1 > current_next_db_id) {
        // Update the next db id
        String next_db_id_str = std::to_string(this_db_id + 1);
        status = kv_instance_->Put(NEXT_DATABASE_ID.data(), next_db_id_str);
        if (!status.ok()) {
            return status;
        }
        LOG_TRACE(fmt::format("Update next db id to {}.", next_db_id_str));
    }
    LOG_TRACE(fmt::format("Replay create db: {} with id {}.", create_db_cmd->db_name_, create_db_cmd->db_id_));
    return Status::OK();
}

Status NewTxn::DropDatabase(const String &db_name, ConflictType conflict_type) {
    this->SetTxnType(TransactionType::kDropDB);

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kDBNotExist) {
            return status;
        }
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<DropDBTxnStore>();
    DropDBTxnStore *txn_store = static_cast<DropDBTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    String db_id_str = db_meta->db_id_str();
    txn_store->db_id_str_ = db_id_str;
    txn_store->db_id_ = std::stoull(db_id_str);
    txn_store->create_ts_ = db_create_ts;
    return Status::OK();
}

Status NewTxn::ReplayDropDb(WalCmdDropDatabaseV2 *drop_db_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // Check if the database is already dropped in kv store
    String drop_db_key = KeyEncode::DropDBKey(drop_db_cmd->db_name_, drop_db_cmd->create_ts_, drop_db_cmd->db_id_);
    String drop_db_commit_ts_str;
    Status status = kv_instance_->Get(drop_db_key, drop_db_commit_ts_str);
    if (status.ok()) {
        TxnTimeStamp db_commit_ts = std::stoull(drop_db_commit_ts_str);
        if (db_commit_ts == commit_ts) {
            LOG_WARN(fmt::format("Skipping replay drop db: Database {} created at {} with id {} and ts: {} already dropped, commit ts: {}, txn: {}.",
                                 drop_db_cmd->db_name_,
                                 drop_db_cmd->create_ts_,
                                 drop_db_cmd->db_id_,
                                 drop_db_cmd->create_ts_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay drop db: Database {} created at {} with id {} and ts: {} already dropped with different commit ts {}, "
                                  "commit ts: {}, txn: {}.",
                                  drop_db_cmd->db_name_,
                                  drop_db_cmd->create_ts_,
                                  drop_db_cmd->db_id_,
                                  drop_db_cmd->create_ts_,
                                  db_commit_ts,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Database commit timestamp mismatch during replay of database drop.");
        }
    }

    status = PrepareCommitDropDB(drop_db_cmd);
    if (!status.ok()) {
        return status;
    }
    LOG_TRACE(fmt::format("Replay drop db: {} with id {}.", drop_db_cmd->db_name_, drop_db_cmd->db_id_));
    return Status::OK();
}

Tuple<SharedPtr<DatabaseInfo>, Status> NewTxn::GetDatabaseInfo(const String &db_name) {
    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }

    auto [db_info, info_status] = db_meta->GetDatabaseInfo();
    if (!info_status.ok()) {
        return {nullptr, info_status};
    }
    db_info->db_name_ = MakeShared<String>(db_name);

    return {std::move(db_info), Status::OK()};
}

Status NewTxn::ListDatabase(Vector<String> &db_names) {
    Vector<String> *db_id_strs_ptr;
    Vector<String> *db_names_ptr;

    CatalogMeta catalog_meta(this);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    if (!status.ok()) {
        return status;
    }
    db_names = *db_names_ptr;
    return status;
}

// Table and Collection OPs
Status NewTxn::GetTables(const String &db_name, Vector<SharedPtr<TableDetail>> &output_table_array) {
    this->CheckTxn(db_name);

    Vector<String> table_names;
    Status status = this->ListTable(db_name, table_names);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_name : table_names) {
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = GetTableMeta(db_name, table_name, db_meta, table_meta);
        if (!status.ok()) {
            return status;
        }
        output_table_array.push_back(MakeShared<TableDetail>());
        table_meta->SetDBTableName(db_name, table_name);
        status = table_meta->GetTableDetail(*output_table_array.back());
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {
    this->SetTxnType(TransactionType::kCreateTable);

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    String table_id_str;
    String table_key;
    TxnTimeStamp create_table_ts;
    status = db_meta->GetTableID(*table_def->table_name(), table_key, table_id_str, create_table_ts);

    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return Status::DuplicateTable(*table_def->table_name());
    } else if (status.code() != ErrorCode::kTableNotExist) {
        return status;
    }

    // Get the latest table id
    std::tie(table_id_str, status) = db_meta->GetNextTableID();
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<CreateTableTxnStore>();
    CreateTableTxnStore *txn_store = static_cast<CreateTableTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(db_meta->db_id_str());
    txn_store->table_name_ = *table_def->table_name();
    txn_store->table_id_str_ = table_id_str;
    txn_store->table_id_ = std::stoull(table_id_str);
    txn_store->table_def_ = table_def;

    // Add operation record with table_id for traceability
    String operation_msg =
        fmt::format("CREATE TABLE {}.{} (db_id: {}, table_id: {})", db_name, *table_def->table_name(), db_meta->db_id_str(), table_id_str);
    txn_context_ptr_->AddOperation(MakeShared<String>(operation_msg));

    LOG_TRACE("NewTxn::CreateTable created table entry is inserted.");
    return Status::OK();
}

Status NewTxn::ReplayCreateTable(WalCmdCreateTableV2 *create_table_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // Check if the table already exists in kv store
    String table_key = KeyEncode::CatalogTableKey(create_table_cmd->db_id_, *create_table_cmd->table_def_->table_name(), commit_ts);
    String table_id;
    // auto new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    // new_txn_mgr->PrintAllKeyValue();
    // auto new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    // new_txn_mgr->PrintAllKeyValue();
    Status status = kv_instance_->Get(table_key, table_id);
    if (status.ok()) {
        if (table_id == create_table_cmd->table_id_) {
            LOG_WARN(fmt::format("Skipping replay create table: Table {} with id {} already exists, commit ts: {}, txn: {}.",
                                 *create_table_cmd->table_def_->table_name(),
                                 create_table_cmd->table_id_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay create table: Table {} with id {} already exists with different id {}, commit ts: {}, txn: {}.",
                                  *create_table_cmd->table_def_->table_name(),
                                  create_table_cmd->table_id_,
                                  table_id,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Table ID mismatch during replay of table creation.");
        }
    }

    // If the table does not exist, create it
    status = PrepareCommitCreateTable(create_table_cmd);
    if (!status.ok()) {
        return status;
    }

    // Get db meta to set next table id
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    status = GetDBMeta(create_table_cmd->db_name_, db_meta, db_create_ts);

    // Get next table id of the db
    String next_table_id_key = KeyEncode::CatalogDbTagKey(create_table_cmd->db_id_, NEXT_TABLE_ID.data());
    String next_table_id_str;
    status = kv_instance_->Get(next_table_id_key, next_table_id_str);
    if (!status.ok()) {
        return status;
    }
    u64 next_table_id = std::stoull(next_table_id_str);
    u64 this_table_id = std::stoull(create_table_cmd->table_id_);
    if (this_table_id + 1 > next_table_id) {
        // Update the next table id
        String new_next_table_id_str = std::to_string(this_table_id + 1);
        status = kv_instance_->Put(next_table_id_key, new_next_table_id_str);
        if (!status.ok()) {
            return status;
        }
        LOG_TRACE(fmt::format("Update next table id to {} for database {}.", new_next_table_id_str, create_table_cmd->db_name_));
    }

    LOG_TRACE(fmt::format("Replay create table: {} with id {} in database {}.",
                          *create_table_cmd->table_def_->table_name(),
                          create_table_cmd->table_id_,
                          create_table_cmd->db_name_));
    return Status::OK();
}

Status NewTxn::DropTable(const String &db_name, const String &table_name, ConflictType conflict_type) {
    this->SetTxnType(TransactionType::kDropTable);

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    this->CheckTxnStatus();

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    String table_key;
    String table_id_str;
    TxnTimeStamp table_create_ts;
    status = db_meta->GetTableID(table_name, table_key, table_id_str, table_create_ts);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kTableNotExist) {
            return status;
        }
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<DropTableTxnStore>();
    DropTableTxnStore *txn_store = static_cast<DropTableTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(db_meta->db_id_str());
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_id_str;
    txn_store->table_id_ = std::stoull(table_id_str);
    txn_store->create_ts_ = table_create_ts;
    txn_store->table_key_ = table_key;

    // Add operation record with table_id for traceability
    String operation_msg = fmt::format("DROP TABLE {}.{} (db_id: {}, table_id: {})", db_name, table_name, db_meta->db_id_str(), table_id_str);
    txn_context_ptr_->AddOperation(MakeShared<String>(operation_msg));

    LOG_TRACE(fmt::format("NewTxn::DropTable dropped table: {}.{}", db_name, table_name));
    return Status::OK();
}

Status NewTxn::ReplayDropTable(WalCmdDropTableV2 *drop_table_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // Check if the table is already dropped in kv store
    String drop_table_key =
        KeyEncode::DropTableKey(drop_table_cmd->db_id_, drop_table_cmd->table_name_, drop_table_cmd->table_id_, drop_table_cmd->create_ts_);
    String drop_table_commit_ts_str;
    Status status = kv_instance_->Get(drop_table_key, drop_table_commit_ts_str);
    if (status.ok()) {
        TxnTimeStamp table_commit_ts = std::stoull(drop_table_commit_ts_str);
        if (table_commit_ts == commit_ts) {
            LOG_WARN(fmt::format("Skipping replay drop table: Table {} created at {} with id {} and ts {} already dropped, commit ts: {}, txn: {}.",
                                 drop_table_cmd->table_name_,
                                 drop_table_cmd->create_ts_,
                                 drop_table_cmd->table_id_,
                                 drop_table_cmd->create_ts_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format(
                "Replay drop table: Table {} created at {} with id {} and ts {} already dropped with different commit ts {}, commit ts: {}, txn: {}.",
                drop_table_cmd->table_name_,
                drop_table_cmd->create_ts_,
                drop_table_cmd->table_id_,
                drop_table_cmd->create_ts_,
                table_commit_ts,
                commit_ts,
                txn_id));
            return Status::UnexpectedError("Table commit timestamp mismatch during replay of table drop.");
        }
    }

    status = PrepareCommitDropTable(drop_table_cmd);
    if (!status.ok()) {
        return status;
    }
    LOG_TRACE(fmt::format("Replay drop table: {} with id {} in database {}.",
                          drop_table_cmd->table_name_,
                          drop_table_cmd->table_id_,
                          drop_table_cmd->db_name_));

    return Status::OK();
}

Status NewTxn::RenameTable(const String &db_name, const String &old_table_name, const String &new_table_name) {
    this->SetTxnType(TransactionType::kRenameTable);

    this->CheckTxnStatus();
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }

    {
        String table_id;
        String table_key;
        TxnTimeStamp create_table_ts;
        status = db_meta->GetTableID(new_table_name, table_key, table_id, create_table_ts);

        if (status.ok()) {
            return Status::DuplicateTable(new_table_name);
        } else if (status.code() != ErrorCode::kTableNotExist) {
            return status;
        }
    }

    String table_id_str;
    String table_key;
    TxnTimeStamp create_table_ts;
    status = db_meta->GetTableID(old_table_name, table_key, table_id_str, create_table_ts);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<RenameTableTxnStore>();
    RenameTableTxnStore *txn_store = static_cast<RenameTableTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->old_table_name_ = old_table_name;
    txn_store->table_id_str_ = table_id_str;
    txn_store->new_table_name_ = new_table_name;
    txn_store->old_table_key_ = table_key;
    LOG_TRACE(fmt::format("NewTxn::Rename table from {}.{} to {}.{}.", db_name, old_table_name, db_name, new_table_name));
    return Status::OK();
}

Status NewTxn::ReplayRenameTable(WalCmdRenameTableV2 *rename_table_cmd, TxnTimeStamp commit_ts, i64 txn_id) {

    const String &db_id = rename_table_cmd->db_id_;
    const String &old_table_name = rename_table_cmd->table_name_;
    const String &old_table_key = rename_table_cmd->old_table_key_;
    const String &table_id = rename_table_cmd->table_id_;
    const TxnTimeStamp create_ts = infinity::GetTimestampFromKey(old_table_key);

    String rename_table_key = KeyEncode::RenameTableKey(db_id, old_table_name, table_id, create_ts);
    String rename_table_commit_ts_str;
    // TODO: Check if the table is already renamed in kv store
    Status status = kv_instance_->Get(rename_table_key, rename_table_commit_ts_str);
    if (status.ok()) {
        TxnTimeStamp rename_table_commit_ts = std::stoull(rename_table_commit_ts_str);
        if (rename_table_commit_ts == commit_ts) {
            LOG_WARN(fmt::format("Skipping replay rename table: Table {} with id {} already renamed to {}, commit ts: {}, txn: {}.",
                                 old_table_name,
                                 table_id,
                                 rename_table_cmd->new_table_name_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay rename table: Table {} with id {} already renamed to {} with different commit ts {}, "
                                  "commit ts: {}, txn: {}.",
                                  old_table_name,
                                  table_id,
                                  rename_table_cmd->new_table_name_,
                                  rename_table_commit_ts,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Table commit timestamp mismatch during replay of table rename.");
        }
    }

    String ts_str = std::to_string(commit_ts);
    kv_instance_->Put(KeyEncode::RenameTableKey(db_id, old_table_name, table_id, create_ts), ts_str);

    // create new table key
    String new_table_key = KeyEncode::CatalogTableKey(db_id, rename_table_cmd->new_table_name_, commit_ts);
    status = kv_instance_->Put(new_table_key, table_id);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

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

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<AddColumnsTxnStore>();
    AddColumnsTxnStore *txn_store = static_cast<AddColumnsTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(db_meta->db_id_str());
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_meta->table_id_str();
    txn_store->table_id_ = std::stoull(table_meta->table_id_str());
    txn_store->column_defs_ = column_defs;
    txn_store->table_key_ = table_key;

    return Status::OK();
}

Status NewTxn::ReplayAddColumns(WalCmdAddColumnsV2 *add_columns_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // This implementation might have error, since not check the column id and create timestamp

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(add_columns_cmd->db_name_, add_columns_cmd->table_name_, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Construct added columns name map
    Set<SizeT> column_idx_set;
    Set<String> column_name_set;
    for (const auto &column_def : add_columns_cmd->column_defs_) {
        column_idx_set.insert(column_def->id());
        column_name_set.insert(column_def->name());
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> old_column_defs;
    std::tie(old_column_defs, status) = table_meta->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    // TODO: TS also needed, check the column name / id / ts, Skip if they are same.
    for (auto &column_def : *old_column_defs) {
        if (column_name_set.contains(column_def->name())) {
            LOG_WARN(fmt::format("Skipping replay add columns: Duplicate column name: {} in table: {}.{}.",
                                 column_def->name(),
                                 add_columns_cmd->db_name_,
                                 add_columns_cmd->table_name_));
            return Status::OK();
        }
        if (column_idx_set.contains(column_def->id())) {
            return Status::DuplicateColumnIndex(fmt::format("Duplicate table column index: {}", column_def->id()));
        }
    }

    // If columns don't exist, create them
    status = PrepareCommitAddColumns(add_columns_cmd);
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE(fmt::format("Replay add columns to table: {}.{}.", add_columns_cmd->db_name_, add_columns_cmd->table_name_));
    return Status::OK();
}

Status NewTxn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) {

    if (column_names.empty()) {
        return Status::NotSupport("DropColumns: column_names is empty");
    }

    {
        Set<String> name_set;
        for (const auto &name : column_names) {
            if (!name_set.insert(name).second) {
                return Status::DuplicateColumnName(name);
            }
        }
    }

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

    if (column_names.size() == old_column_defs->size()) {
        return Status::NotSupport("Cannot delete all the columns of a table");
    }

    Vector<String> column_keys;
    for (const auto &column_name : column_names) {
        String column_key;
        std::tie(column_key, status) = table_meta->GetColumnKeyByColumnName(column_name);
        if (!status.ok()) {
            return status;
        }
        column_keys.emplace_back(column_key);
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
            return index_status;
        }
        for (const String &column_name : column_names) {
            if (IsEqual(index_base->column_name(), column_name)) {
                return Status::IndexOnColumn(column_name);
            }
        }
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<DropColumnsTxnStore>();
    auto *txn_store = static_cast<DropColumnsTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(db_meta->db_id_str());
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_meta->table_id_str();
    txn_store->table_id_ = std::stoull(table_meta->table_id_str());
    txn_store->column_names_ = column_names;
    txn_store->column_ids_ = column_ids;
    txn_store->table_key_ = table_key;
    txn_store->column_keys_ = column_keys;
    return Status::OK();
}

Status NewTxn::ReplayDropColumns(WalCmdDropColumnsV2 *drop_columns_cmd, TxnTimeStamp commit_ts, i64 txn_id) {

    const String &db_name = drop_columns_cmd->db_name_;
    const String &table_name = drop_columns_cmd->table_name_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }

    for (SizeT i = 0; i < drop_columns_cmd->column_names_.size(); ++i) {
        const String &column_key = drop_columns_cmd->column_keys_[i];
        TxnTimeStamp create_ts = infinity::GetTimestampFromKey(column_key);
        String drop_column_key =
            KeyEncode::DropTableColumnKey(drop_columns_cmd->db_id_, drop_columns_cmd->table_id_, drop_columns_cmd->column_names_[i], create_ts);
        String drop_column_commit_ts_str;
        status = kv_instance_->Get(drop_column_key, drop_column_commit_ts_str);
        if (status.ok()) {
            TxnTimeStamp drop_column_commit_ts = std::stoull(drop_column_commit_ts_str);
            if (drop_column_commit_ts == commit_ts) {
                LOG_WARN(fmt::format("Skipping replay drop column: Column {} created at {} in table {}.{} already dropped, commit ts: {}, txn: {}.",
                                     drop_columns_cmd->column_names_[i],
                                     create_ts,
                                     db_name,
                                     table_name,
                                     commit_ts,
                                     txn_id));
                continue;
            } else {
                LOG_ERROR(fmt::format(
                    "Replay drop column: Column {} created at {} in table {}.{} already dropped with different commit ts {}, commit ts: {}, txn: {}.",
                    drop_columns_cmd->column_names_[i],
                    create_ts,
                    db_name,
                    table_name,
                    drop_column_commit_ts,
                    commit_ts,
                    txn_id));
                return Status::UnexpectedError("Column commit timestamp mismatch during replay of column drop.");
            }
        }
    }

    status = this->DropColumnsData(*table_meta, drop_columns_cmd->column_ids_);
    if (!status.ok()) {
        return status;
    }

    auto ts_str = std::to_string(commit_ts);
    for (SizeT i = 0; i < drop_columns_cmd->column_names_.size(); ++i) {
        const String &column_key = drop_columns_cmd->column_keys_[i];
        TxnTimeStamp create_ts = infinity::GetTimestampFromKey(column_key);
        kv_instance_->Put(
            KeyEncode::DropTableColumnKey(drop_columns_cmd->db_id_, drop_columns_cmd->table_id_, drop_columns_cmd->column_names_[i], create_ts),
            ts_str);
    }
    return Status::OK();
}

Status NewTxn::ListTable(const String &db_name, Vector<String> &table_names) {
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
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
    this->SetTxnType(TransactionType::kCreateIndex);

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

    String index_key;
    String index_id;
    TxnTimeStamp create_index_ts;
    status = table_meta->GetIndexID(*index_base->index_name_, index_key, index_id, create_index_ts);
    if (status.ok()) {
        if (conflict_type == ConflictType::kIgnore) {
            return Status::OK();
        }
        LOG_ERROR(fmt::format("CreateIndex: index {} already exists, index_key: {}, index_id: {}", *index_base->index_name_, index_key, index_id));
        return Status(ErrorCode::kDuplicateIndexName, MakeUnique<String>(fmt::format("Index: {} already exists", *index_base->index_name_)));
    } else if (status.code() != ErrorCode::kIndexNotExist) {
        return status;
    }

    // Get the latest index id and lock the id
    String index_id_str;
    std::tie(index_id_str, status) = table_meta->GetNextIndexID();
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<CreateIndexTxnStore>();
    CreateIndexTxnStore *txn_store = static_cast<CreateIndexTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(txn_store->db_id_str_);
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_meta->table_id_str();
    txn_store->table_id_ = std::stoull(txn_store->table_id_str_);
    txn_store->index_id_str_ = index_id_str;
    txn_store->index_base_ = index_base;
    txn_store->table_key_ = table_key;

    auto wal_command =
        MakeShared<WalCmdCreateIndexV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), index_id_str, index_base, table_key);
    wal_command->table_key_ = table_key;
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
    LOG_TRACE("NewTxn::CreateIndex created index entry is inserted.");
    return Status::OK();
}

Status NewTxn::ReplayCreateIndex(WalCmdCreateIndexV2 *create_index_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    // Check if the index already exists in kv store
    String index_key =
        KeyEncode::CatalogIndexKey(create_index_cmd->db_id_, create_index_cmd->table_id_, *create_index_cmd->index_base_->index_name_, commit_ts);
    String index_id;
    Status status = kv_instance_->Get(index_key, index_id);
    if (status.ok()) {
        if (index_id == create_index_cmd->index_id_) {
            LOG_WARN(fmt::format("Skipping replay create index: Index {} already exists in table {} of database {}, commit ts: {}, txn: {}.",
                                 *create_index_cmd->index_base_->index_name_,
                                 create_index_cmd->table_name_,
                                 create_index_cmd->db_name_,
                                 commit_ts,
                                 txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(
                fmt::format("Replay create index: Index {} already exists in table {} of database {} with different id {}, commit ts: {}, txn: {}.",
                            *create_index_cmd->index_base_->index_name_,
                            create_index_cmd->table_name_,
                            create_index_cmd->db_name_,
                            index_id,
                            commit_ts,
                            txn_id));
            return Status::UnexpectedError("Index ID mismatch during replay of index creation.");
        }
    }

    status = PrepareCommitCreateIndex(create_index_cmd);
    if (!status.ok()) {
        return status;
    }

    String next_index_id_key = KeyEncode::CatalogTableTagKey(create_index_cmd->db_id_, create_index_cmd->table_id_, NEXT_INDEX_ID.data());
    String next_index_id_str;
    status = kv_instance_->Get(next_index_id_key, next_index_id_str);

    u64 next_index_id = std::stoull(next_index_id_str);
    u64 this_index_id = std::stoull(create_index_cmd->index_id_);
    if (this_index_id + 1 > next_index_id) {
        // Update the next index id
        String new_next_index_id_str = std::to_string(this_index_id + 1);
        status = kv_instance_->Put(next_index_id_key, new_next_index_id_str);
        if (!status.ok()) {
            return status;
        }
        LOG_TRACE(fmt::format("Update next index id to {} for table {} of database {}.",
                              new_next_index_id_str,
                              create_index_cmd->table_name_,
                              create_index_cmd->db_name_));
    }
    LOG_TRACE(fmt::format("Replay create index: {} with id {} in table {} of database {}.",
                          *create_index_cmd->index_base_->index_name_,
                          create_index_cmd->index_id_,
                          create_index_cmd->table_name_,
                          create_index_cmd->db_name_));
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

    String index_key;
    String index_id;
    TxnTimeStamp create_index_ts;
    status = table_meta->GetIndexID(index_name, index_key, index_id, create_index_ts);
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

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<DropIndexTxnStore>();
    DropIndexTxnStore *txn_store = static_cast<DropIndexTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_meta->db_id_str();
    txn_store->db_id_ = std::stoull(txn_store->db_id_str_);
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_meta->table_id_str();
    txn_store->table_id_ = std::stoull(txn_store->table_id_str_);
    txn_store->index_name_ = index_name;
    txn_store->index_id_str_ = index_id;
    txn_store->index_id_ = std::stoull(txn_store->index_id_str_);
    txn_store->create_ts_ = create_index_ts;
    txn_store->index_key_ = index_key;

    LOG_TRACE(fmt::format("NewTxn::DropIndexByName dropped index: {}.{}.{}", db_name, table_name, index_name));
    return Status::OK();
}

Status NewTxn::ReplayDropIndex(WalCmdDropIndexV2 *drop_index_cmd, TxnTimeStamp commit_ts, i64 txn_id) {

    // Check if the index is already dropped in kv store
    String drop_index_key = KeyEncode::DropTableIndexKey(drop_index_cmd->db_id_,
                                                         drop_index_cmd->table_id_,
                                                         drop_index_cmd->index_name_,
                                                         drop_index_cmd->create_ts_,
                                                         drop_index_cmd->index_id_);
    String drop_index_commit_ts_str;
    Status status = kv_instance_->Get(drop_index_key, drop_index_commit_ts_str);
    if (status.ok()) {
        TxnTimeStamp index_commit_ts = std::stoull(drop_index_commit_ts_str);
        if (index_commit_ts == commit_ts) {
            LOG_WARN(
                fmt::format("Skipping replay drop index: Index {} created at {} already dropped in table {} of database {}, commit ts: {}, txn: {}.",
                            drop_index_cmd->index_name_,
                            drop_index_cmd->create_ts_,
                            drop_index_cmd->table_name_,
                            drop_index_cmd->db_name_,
                            commit_ts,
                            txn_id));
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay drop index: Index {} created at {} already dropped in table {} of database {} with different commit ts {}, "
                                  "commit ts: {}, txn: {}.",
                                  drop_index_cmd->index_name_,
                                  drop_index_cmd->create_ts_,
                                  drop_index_cmd->table_name_,
                                  drop_index_cmd->db_name_,
                                  index_commit_ts,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Index commit timestamp mismatch during replay of index drop.");
        }
    }

    status = PrepareCommitDropIndex(drop_index_cmd);
    if (!status.ok()) {
        return status;
    }
    LOG_TRACE(fmt::format("Replay drop index: {} in table {} of database {}.",
                          drop_index_cmd->index_name_,
                          drop_index_cmd->table_name_,
                          drop_index_cmd->db_name_));

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
    status = table_meta->GetComment(*table_info);
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

    String chunk_id_key = KeyEncode::CatalogIdxChunkKey(table_meta->db_id_str(),
                                                        table_meta->table_id_str(),
                                                        table_index_meta->index_id_str(),
                                                        segment_index_meta.segment_id(),
                                                        chunk_id);
    String commit_ts_str;
    status = kv_instance_->Get(chunk_id_key, commit_ts_str);
    if (!status.ok()) {
        return {nullptr, status};
    }

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
    status = table_meta->CheckSegments({segment_id});
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
    Status status = this->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {segment_info_list, status};
    }
    std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
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

Status NewTxn::CreateSnapshot(const String &db_name, const String &table_name, const String &snapshot_name, SnapshotScope snapshot_type) {
    this->SetTxnType(TransactionType::kCreateSnapshot);
    if (snapshot_type == SnapshotScope::kTable) {
        // Check if the DB is valid
        this->CheckTxn(db_name);

        // check if the table exists
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta_opt;
        String table_key;
        Status status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, &table_key);
        if (!status.ok()) {
            return status;
        }

    } else if (snapshot_type == SnapshotScope::kDatabase) {
        // check if the db exists
        Optional<DBMeeta> db_meta;
        TxnTimeStamp db_create_ts;
        Status status = GetDBMeta(db_name, db_meta, db_create_ts);
        if (!status.ok()) {
            return status;
        }
    } else if (snapshot_type == SnapshotScope::kSystem) {
        // the whole system
    } else {
        return Status::UnexpectedError("Invalid snapshot type");
    }

    base_txn_store_ = MakeShared<CreateSnapshotTxnStore>();
    CreateSnapshotTxnStore *txn_store = static_cast<CreateSnapshotTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->table_name_ = table_name;
    txn_store->snapshot_name_ = snapshot_name;
    txn_store->max_commit_ts_ = txn_context_ptr_->begin_ts_;
    txn_store->snapshot_type_ = snapshot_type;

    return Status::OK(); // Success
}

Tuple<SharedPtr<DatabaseSnapshotInfo>, Status> NewTxn::GetDatabaseSnapshotInfo(const String &db_name) {
    this->CheckTxn(db_name);
    SharedPtr<DatabaseSnapshotInfo> database_snapshot_info;
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }
    database_snapshot_info = MakeShared<DatabaseSnapshotInfo>();
    database_snapshot_info->db_name_ = db_name;
    database_snapshot_info->db_id_str_ = db_meta->db_id_str();

    Vector<String> *table_ids_ptr = nullptr;
    Vector<String> *table_names_ptr = nullptr;
    status = db_meta->GetTableIDs(table_ids_ptr, &table_names_ptr);
    if (!status.ok()) {
        return {nullptr, status};
    }

    // String *db_comment = nullptr;
    // status = db_meta->GetComment(db_comment);
    // database_snapshot_info->db_comment_ = *db_comment;
    database_snapshot_info->db_next_table_id_str_ = std::to_string(std::stoull(table_ids_ptr->back()) + 1);
    for (size_t i = 0; i < table_ids_ptr->size(); i++) {
        Optional<TableMeeta> table_meta;
        status = GetTableMeta(table_names_ptr->at(i), *db_meta, table_meta);
        if (!status.ok()) {
            return {nullptr, status};
        }
        SharedPtr<TableSnapshotInfo> table_snapshot_info;
        std::tie(table_snapshot_info, status) = table_meta->MapMetaToSnapShotInfo(db_name, table_names_ptr->at(i));
        if (!status.ok()) {
            return {nullptr, status};
        }
        database_snapshot_info->table_snapshots_.push_back(table_snapshot_info);
    }
    return {std::move(database_snapshot_info), Status::OK()};
}

Status NewTxn::RestoreTableSnapshot(const String &db_name, const SharedPtr<TableSnapshotInfo> &table_snapshot_info) {
    // Cleanup();
    this->SetTxnType(TransactionType::kRestoreTable);
    const String &table_name = table_snapshot_info->table_name_;
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    String table_id_str;
    String table_key;
    TxnTimeStamp table_create_ts;
    status = db_meta->GetTableID(table_name, table_key, table_id_str, table_create_ts);

    if (status.ok()) {
        // if (conflict_type == ConflictType::kIgnore) {
        //     return Status::OK();
        // }
        return Status(ErrorCode::kDuplicateTableName, MakeUnique<String>(fmt::format("Table: {} already exists", table_name)));
    } else if (status.code() != ErrorCode::kTableNotExist) {
        return status;
    }

    // Get the latest table id
    std::tie(table_id_str, status) = db_meta->GetNextTableID();
    if (!status.ok()) {
        return status;
    }

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>(db_name),
                                                   MakeShared<String>(table_name),
                                                   MakeShared<String>(table_snapshot_info->table_comment_),
                                                   table_snapshot_info->columns_);
    // copy files from snapshot to data dir
    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    String snapshot_name = table_snapshot_info->snapshot_name_;
    Vector<String> restored_file_paths;

    status = table_snapshot_info->RestoreSnapshotFiles(snapshot_dir,
                                                       snapshot_name,
                                                       table_snapshot_info->GetFiles(),
                                                       table_id_str,
                                                       db_meta->db_id_str(),
                                                       restored_file_paths,
                                                       false);

    if (!status.ok()) {
        return status;
    }

    base_txn_store_ = MakeShared<RestoreTableTxnStore>();
    RestoreTableTxnStore *txn_store = static_cast<RestoreTableTxnStore *>(base_txn_store_.get());
    txn_store->files_ = restored_file_paths;

    // Use the helper function to process snapshot restoration data
    status = ProcessSnapshotRestorationData(db_name,
                                            db_meta->db_id_str(),
                                            table_name,
                                            table_id_str,
                                            table_def,
                                            table_snapshot_info,
                                            snapshot_name,
                                            txn_store);
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE("NewTxn::RestoreTable created table entry is inserted.");
    // figure out why this is needed
    // status = db_meta.value().kv_instance().Commit();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::RestoreDatabaseSnapshot(const SharedPtr<DatabaseSnapshotInfo> &database_snapshot_info) {
    // Cleanup();
    this->SetTxnType(TransactionType::kRestoreDatabase);
    const String &db_name = database_snapshot_info->db_name_;

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (status.ok()) {
        return Status::DuplicateDatabase(db_name);
    }
    if (status.code() != ErrorCode::kDBNotExist) {
        return status;
    }

    String db_id_str;
    status = IncrLatestID(db_id_str, NEXT_DATABASE_ID);
    if (!status.ok()) {
        return status;
    }

    base_txn_store_ = MakeShared<RestoreDatabaseTxnStore>();
    RestoreDatabaseTxnStore *txn_store = static_cast<RestoreDatabaseTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_id_str;
    txn_store->db_comment_ = database_snapshot_info->db_comment_;

    // Process each table snapshot within the database
    for (const auto &table_snapshot_info : database_snapshot_info->table_snapshots_) {
        const String &table_name = table_snapshot_info->table_name_;

        // Create table definition
        SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>(db_name),
                                                       MakeShared<String>(table_name),
                                                       MakeShared<String>(table_snapshot_info->table_comment_),
                                                       table_snapshot_info->columns_);

        String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
        String snapshot_name = database_snapshot_info->snapshot_name_;
        Vector<String> restored_file_paths;
        status = table_snapshot_info->RestoreSnapshotFiles(snapshot_dir,
                                                           snapshot_name,
                                                           table_snapshot_info->GetFiles(),
                                                           table_snapshot_info->table_id_str_,
                                                           db_id_str,
                                                           restored_file_paths,
                                                           false);

        if (!status.ok()) {
            return status;
        }

        SharedPtr<RestoreTableTxnStore> tmp_txn_store_ = MakeShared<RestoreTableTxnStore>();

        // Use the helper function to process snapshot restoration data
        status = ProcessSnapshotRestorationData(db_name,
                                                db_id_str,
                                                table_name,
                                                table_snapshot_info->table_id_str_,
                                                table_def,
                                                table_snapshot_info,
                                                snapshot_name,
                                                tmp_txn_store_.get());

        if (!status.ok()) {
            return status;
        }
        txn_store->restore_table_txn_stores_.push_back(std::move(tmp_txn_store_));
    }

    LOG_TRACE("NewTxn::RestoreDatabaseSnapshot created database entry is inserted.");
    return Status::OK();
}

Status NewTxn::RestoreSystemSnapshot(const SharedPtr<SystemSnapshotInfo> &system_snapshot_info) {
    // Cleanup();
    this->SetTxnType(TransactionType::kRestoreSystem);
    base_txn_store_ = MakeShared<RestoreSystemTxnStore>();
    RestoreSystemTxnStore *txn_store = static_cast<RestoreSystemTxnStore *>(base_txn_store_.get());
    txn_store->snapshot_name_ = system_snapshot_info->snapshot_name_;

    Status status;

    // get new database ids for new databases
    for (const auto &database_snapshot_info : system_snapshot_info->database_snapshots_) {
        String db_id_str;
        status = IncrLatestID(db_id_str, NEXT_DATABASE_ID);
        if (!status.ok()) {
            return status;
        }

        SharedPtr<RestoreDatabaseTxnStore> db_txn_store_ = MakeShared<RestoreDatabaseTxnStore>();
        db_txn_store_->db_name_ = database_snapshot_info->db_name_;
        db_txn_store_->db_id_str_ = db_id_str;
        db_txn_store_->db_comment_ = database_snapshot_info->db_comment_;

        // Process each table snapshot within the database
        for (const auto &table_snapshot_info : database_snapshot_info->table_snapshots_) {
            const String &table_name = table_snapshot_info->table_name_;

            // Create table definition
            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>(db_txn_store_->db_name_),
                                                           MakeShared<String>(table_name),
                                                           MakeShared<String>(table_snapshot_info->table_comment_),
                                                           table_snapshot_info->columns_);

            String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
            // String snapshot_name = database_snapshot_info->snapshot_name_;
            Vector<String> restored_file_paths;
            status = table_snapshot_info->RestoreSnapshotFiles(snapshot_dir,
                                                               system_snapshot_info->snapshot_name_,
                                                               table_snapshot_info->GetFiles(),
                                                               table_snapshot_info->table_id_str_,
                                                               db_id_str,
                                                               restored_file_paths,
                                                               false);

            if (!status.ok()) {
                return status;
            }

            SharedPtr<RestoreTableTxnStore> tmp_txn_store_ = MakeShared<RestoreTableTxnStore>();

            // Use the helper function to process snapshot restoration data
            status = ProcessSnapshotRestorationData(db_txn_store_->db_name_,
                                                    db_id_str,
                                                    table_name,
                                                    table_snapshot_info->table_id_str_,
                                                    table_def,
                                                    table_snapshot_info,
                                                    system_snapshot_info->snapshot_name_,
                                                    tmp_txn_store_.get());

            if (!status.ok()) {
                return status;
            }
            tmp_txn_store_->files_ = restored_file_paths;
            db_txn_store_->restore_table_txn_stores_.push_back(std::move(tmp_txn_store_));
        }
        txn_store->restore_db_txn_stores_.push_back(std::move(db_txn_store_));
    }

    // drop all existing databases
    Vector<String> *db_id_strs_ptr;
    Vector<String> *db_names_ptr;
    CatalogMeta catalog_meta(this);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    if (!status.ok()) {
        return status;
    }

    for (size_t i = 0; i < db_id_strs_ptr->size(); i++) {
        Optional<DBMeeta> db_meta;
        TxnTimeStamp db_create_ts;
        status = GetDBMeta(db_names_ptr->at(i), db_meta, db_create_ts);
        if (!status.ok()) {
            return status;
        }
        SharedPtr<DropDBTxnStore> drop_db_txn_store_ = MakeShared<DropDBTxnStore>();
        drop_db_txn_store_->db_name_ = db_names_ptr->at(i);
        drop_db_txn_store_->db_id_str_ = db_id_strs_ptr->at(i);
        drop_db_txn_store_->create_ts_ = db_create_ts;
        txn_store->drop_db_txn_stores_.push_back(std::move(drop_db_txn_store_));
    }

    return Status::OK();
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
    if (txn_type != TransactionType::kNewCheckpoint && txn_type != TransactionType::kCreateSnapshot) {
        UnrecoverableError(fmt::format("Expected transaction type is checkpoint or create table snapshot."));
    }

    if (last_ckp_ts % 2 == 0 and last_ckp_ts > 0) {
        UnrecoverableError(fmt::format("last checkpoint ts isn't correct: {}", last_ckp_ts));
    }

    Status status;
    TxnTimeStamp checkpoint_ts = txn_context_ptr_->begin_ts_;
    CheckpointOption option{checkpoint_ts};

    current_ckp_ts_ = checkpoint_ts;
    LOG_INFO(fmt::format("checkpoint ts: {}", current_ckp_ts_));

    if (last_ckp_ts > 0 and last_ckp_ts + 2 >= checkpoint_ts) {
        // last checkpoint ts: last checkpoint txn begin ts. checkpoint is the begin_ts of current txn
        txn_type_ = TxnType::kReadOnly;
        LOG_INFO(fmt::format("Last checkpoint ts {}, this checkpoint begin ts: {}, SKIP CHECKPOINT", last_ckp_ts, checkpoint_ts));
        return Status::OK();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    if (!wal_manager->SetCheckpointing()) {
        // Checkpointing
        LOG_INFO(fmt::format("checkpoint ts: {} skipped due to the system is checkpointing.", checkpoint_ts));
        return Status::OK();
    }
    DeferFn defer([&] { wal_manager->UnsetCheckpoint(); });

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(this);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    LOG_DEBUG(fmt::format("checkpoint ts {}, got {} DBs.", checkpoint_ts, db_id_strs_ptr->size()));
    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }
    base_txn_store_ = MakeShared<CheckpointTxnStore>();
    CheckpointTxnStore *txn_store = static_cast<CheckpointTxnStore *>(base_txn_store_.get());
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, this);
        status = this->CheckpointDB(db_meta, option, txn_store);
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

    txn_store->max_commit_ts_ = option.checkpoint_ts_;

    return Status::OK();
}

Status NewTxn::ReplayCheckpoint(WalCmdCheckpointV2 *optimize_cmd, TxnTimeStamp commit_ts, i64 txn_id) { return Status::OK(); }

Status NewTxn::CheckpointDB(DBMeeta &db_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store) {
    Vector<String> *table_id_strs_ptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    LOG_DEBUG(fmt::format("checkpoint ts {}, db id {}, got {} tables.", option.checkpoint_ts_, db_meta.db_id_str(), table_id_strs_ptr->size()));
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, this);
        status = this->CheckpointTable(table_meta, option, ckp_txn_store);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

void NewTxn::AddMetaKeyForCommit(const String &key) { keys_wait_for_commit_.push_back(key); }

TxnTimeStamp NewTxn::CommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->commit_ts_;
}

TxnTimeStamp NewTxn::KVCommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->kv_commit_ts_;
}

TxnTimeStamp NewTxn::LastSystemKVCommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->last_kv_commit_ts_;
}

TxnTimeStamp NewTxn::LastSystemCommitTS() const {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return txn_context_ptr_->last_commit_ts_;
}

[[maybe_unused]] void NewTxn::SetTxnKVCommitTS(TxnTimeStamp kv_commit_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    txn_context_ptr_->kv_commit_ts_ = kv_commit_ts;
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

void NewTxn::SetTxnBottomDone() {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    bottom_done_ = true;
}

bool NewTxn::GetTxnBottomDone() {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    return bottom_done_;
};

bool NewTxn::NeedToAllocate() const {
    TransactionType txn_type = TransactionType::kInvalid;
    if (base_txn_store_ != nullptr) {
        txn_type = base_txn_store_->type_;
        if (txn_type != GetTxnType()) {
            LOG_DEBUG(fmt::format("Transaction type mismatch: {} vs {}", TransactionType2Str(txn_type), TransactionType2Str(GetTxnType())));
        }
    } else {
        txn_type = GetTxnType();
    }

    switch (txn_type) {
        case TransactionType::kAppend:   // for data range to append
        case TransactionType::kUpdate: { // for data range to append
            return true;
        }
        default:
            break;
    }
    return false;
}

void NewTxn::SetTxnType(TransactionType type) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (txn_context_ptr_->txn_type_ == type) {
        return;
    }

    switch (txn_context_ptr_->txn_type_) {
        case TransactionType::kNormal: {
            txn_context_ptr_->txn_type_ = type;
            break;
        }
        case TransactionType::kUpdate: {
            if (type == TransactionType::kAppend or type == TransactionType::kDelete) {
                break;
            } else {
                UnrecoverableError(fmt::format("Attempt to change transaction type from {} to {}",
                                               TransactionType2Str(txn_context_ptr_->txn_type_),
                                               TransactionType2Str(type)));
            }
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Attempt to change transaction type from {} to {}",
                                           TransactionType2Str(txn_context_ptr_->txn_type_),
                                           TransactionType2Str(type)));
        }
    }
}

bool NewTxn::IsWriteTransaction() const { return txn_context_ptr_->is_write_transaction_; }

void NewTxn::SetTxnRollbacking(TxnTimeStamp rollback_ts) {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    TxnState txn_state = txn_context_ptr_->state_;
    if (txn_state == TxnState::kRollbacking) {
        return;
    }
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
    if ((base_txn_store_ == nullptr && !this->IsReplay()) or txn_type_ == TxnType::kReadOnly) {
        // Don't need to write empty WalEntry (read-only transactions).
        TxnTimeStamp commit_ts = txn_mgr_->GetReadCommitTS(this);
        this->SetTxnCommitting(commit_ts);
        this->SetTxnCommitted();
        LOG_TRACE(fmt::format("Commit READ txn: {}. begin ts: {}, Command: {}", txn_context_ptr_->txn_id_, BeginTS(), *GetTxnText()));
        return Status::OK();
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
    LOG_TRACE(fmt::format("Committing WRITE txn: {}, begin_ts:{} committing ts: {}, Command: {}",
                          txn_context_ptr_->txn_id_,
                          BeginTS(),
                          commit_ts,
                          *GetTxnText()));

    this->SetTxnCommitting(commit_ts);

    Status status;
    String conflict_reason;
    bool retry_query = true;
    bool conflict = txn_mgr_->CheckConflict1(this, conflict_reason, retry_query);
    if (conflict) {
        if (retry_query) {
            status = Status::TxnConflict(txn_context_ptr_->txn_id_, fmt::format("NewTxn conflict reason: {}.", conflict_reason));
        } else {
            status = Status::TxnConflictNoRetry(txn_context_ptr_->txn_id_, fmt::format("NewTxn conflict reason: {}.", conflict_reason));
        }

        this->SetTxnRollbacking(commit_ts);
    }

    if (NeedToAllocate()) {
        // If the txn is 'append' / 'import' / 'dump index' / 'create index' go to id generator;
        TxnState txn_state = this->GetTxnState();
        switch (txn_state) {
            case TxnState::kCommitting: {
                // LOG_INFO(fmt::format("To allocation task: {}, transaction: {}", *this->GetTxnText(), txn_context_ptr_->txn_id_));
                SharedPtr<TxnAllocatorTask> txn_allocator_task = MakeShared<TxnAllocatorTask>(this);
                txn_mgr_->SubmitForAllocation(txn_allocator_task);
                txn_allocator_task->Wait();
                status = txn_allocator_task->status_;
                // LOG_INFO(fmt::format("Finish allocation task: {}, transaction: {}", *this->GetTxnText(), txn_context_ptr_->txn_id_));
                break;
            }
            case TxnState::kRollbacking: {
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Unexpected transaction state: {}", TxnState2Str(txn_state)));
            }
        }
    }

    if (status.ok()) {
        status = this->PrepareCommit();
    }
    // LOG_INFO(fmt::format("Prepare commit status: {}", status.message()));

    if (!status.ok()) {
        LOG_INFO("Prepare commit failed, rollback the transaction");
        // If prepare commit or conflict check failed, rollback the transaction
        this->SetTxnRollbacking(commit_ts);
        txn_mgr_->SendToWAL(this);
        PostRollback(commit_ts);
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

    Status status = this->PrepareCommit();
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Replay transaction, prepare commit: {}", status.message()));
    }

    CommitBottom();

    // Try to commit the transaction
    txn_mgr_->CommitKVInstance(this);

    PostCommit();

    return Status::OK();
}

Status NewTxn::CommitRecovery() {
    // Try to commit the rocksdb transaction
    txn_mgr_->CommitKVInstance(this);

    return Status::OK();
}

Status NewTxn::PrepareCommit() {
    // TODO: for replayed transaction, meta data need to check if there is duplicated operation.
    // TODO: CreateIndex has populated wal_entry_ via PopulateIndex(). Need to unify the way.
    if (base_txn_store_.get() != nullptr && GetTxnType() != TransactionType::kCreateIndex) {
        wal_entry_ = base_txn_store_->ToWalEntry(this->CommitTS());
    }
    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::DUMMY: {
                break;
            }
            case WalCommandType::CREATE_DATABASE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of CREATE_DATABASE_V2 command.
                    break;
                }
                auto *create_db_cmd = static_cast<WalCmdCreateDatabaseV2 *>(command.get());
                Status status = PrepareCommitCreateDB(create_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_DATABASE_V2 command.
                    break;
                }
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(command.get());
                Status status = PrepareCommitDropDB(drop_db_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_TABLE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_TABLE_V2 command.
                    break;
                }
                auto *create_table_cmd = static_cast<WalCmdCreateTableV2 *>(command.get());
                auto status = PrepareCommitCreateTable(create_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_TABLE_V2 command.
                    break;
                }
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(command.get());
                Status status = PrepareCommitDropTable(drop_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RENAME_TABLE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of CREATE_DATABASE_V2 command.
                    break;
                }
                auto *rename_table_cmd = static_cast<WalCmdRenameTableV2 *>(command.get());
                Status status = PrepareCommitRenameTable(rename_table_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                if (this->IsReplay()) {
                    // Skip replay of ADD_COLUMNS_V2 command.
                    break;
                }
                auto *add_column_cmd = static_cast<WalCmdAddColumnsV2 *>(command.get());
                Status status = PrepareCommitAddColumns(add_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_COLUMNS_V2 command.
                    break;
                }
                auto *drop_column_cmd = static_cast<WalCmdDropColumnsV2 *>(command.get());
                Status status = PrepareCommitDropColumns(drop_column_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_TABLE_V2 command.
                    break;
                }
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(command.get());
                Status status = PrepareCommitCreateIndex(create_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DROP_INDEX_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_INDEX_V2 command.
                    break;
                }
                auto *drop_index_cmd = static_cast<WalCmdDropIndexV2 *>(command.get());
                Status status = PrepareCommitDropIndex(drop_index_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                // Commit of dump mem index command is handled in CommitBottom().
                // Process dump mem index operation caused by other commands (import, compact, optimizeIndex) here.
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(command.get());
                if (dump_index_cmd->dump_cause_ != DumpIndexCause::kDumpMemIndex) {
                    Status status = PrepareCommitDumpIndex(dump_index_cmd, kv_instance_.get());
                    if (!status.ok()) {
                        return status;
                    }
                }
                break;
            }
            case WalCommandType::APPEND_V2: {
                // LOG_INFO(fmt::format("txn: {}  prepare commit append", txn_context_ptr_->txn_id_));
                break;
            }
            case WalCommandType::DELETE_V2: {
                if (this->IsReplay()) {
                    // Skip replay of DROP_INDEX_V2 command.
                    break;
                }
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(command.get());

                Status status = PrepareCommitDelete(delete_cmd);
                if (!status.ok()) {
                    LOG_INFO(fmt::format("Prepare commit delete failed: {}", status.message()));
                    return status;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                if (this->IsReplay()) {
                    // Skip replay of IMPORT_V2 command.
                    break;
                }
                auto *import_cmd = static_cast<WalCmdImportV2 *>(command.get());
                Status status = PrepareCommitImport(import_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                if (this->IsReplay()) {
                    // Skip replay of COMPACT_V2 command.
                    break;
                }
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(command.get());
                Status status = PrepareCommitCompact(compact_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::CHECKPOINT_V2: {
                if (this->IsReplay()) {
                    // Skip replay of CHECKPOINT_V2 command.
                    break;
                }
                auto *checkpoint_cmd = static_cast<WalCmdCheckpointV2 *>(command.get());
                Status status = PrepareCommitCheckpoint(checkpoint_cmd);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Fail to checkpoint: {}", status.message()));
                }
                break;
            }
            case WalCommandType::OPTIMIZE_V2: {
                [[maybe_unused]] auto *optimize_cmd = static_cast<WalCmdOptimizeV2 *>(command.get());
                break;
            }
            case WalCommandType::CLEANUP: {
                break;
            }
            case WalCommandType::CREATE_SNAPSHOT: {
                if (this->IsReplay()) {
                    // Skip replay of CREATE_SNAPSHOT command.
                    LOG_TRACE("Skip replay of CREATE_SNAPSHOT command.");
                    break;
                }
                auto *create_snapshot_cmd = static_cast<WalCmdCreateSnapshot *>(command.get());
                Status status = PrepareCommitCreateSnapshot(create_snapshot_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RESTORE_TABLE_SNAPSHOT: {
                if (this->IsReplay()) {
                    // Skip replay of RESTORE_TABLE_SNAPSHOT command.
                    LOG_TRACE("Skip replay of RESTORE_TABLE_SNAPSHOT command.");
                    break;
                }
                auto *restore_table_snapshot_cmd = static_cast<WalCmdRestoreTableSnapshot *>(command.get());
                Status status = PrepareCommitRestoreTableSnapshot(restore_table_snapshot_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RESTORE_DATABASE_SNAPSHOT: {
                if (this->IsReplay()) {
                    // Skip replay of RESTORE_TABLE_SNAPSHOT command.
                    LOG_TRACE("Skip replay of RESTORE_DATABASE_SNAPSHOT command.");
                    break;
                }
                auto *restore_database_snapshot_cmd = static_cast<WalCmdRestoreDatabaseSnapshot *>(command.get());
                Status status = PrepareCommitRestoreDatabaseSnapshot(restore_database_snapshot_cmd);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case WalCommandType::RESTORE_SYSTEM_SNAPSHOT: {
                if (this->IsReplay()) {
                    // Skip replay of RESTORE_SYSTEM_SNAPSHOT command.
                    LOG_TRACE("Skip replay of RESTORE_SYSTEM_SNAPSHOT command.");
                    break;
                }
                auto *restore_system_snapshot_cmd = static_cast<WalCmdRestoreSystemSnapshot *>(command.get());
                Status status = PrepareCommitRestoreSystemSnapshot(restore_system_snapshot_cmd);
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

Status NewTxn::GetDBMeta(const String &db_name, Optional<DBMeeta> &db_meta, TxnTimeStamp &db_create_ts, String *db_key_ptr) {
    CatalogMeta catalog_meta(this);
    String db_key;
    String db_id_str;
    Status status = catalog_meta.GetDBID(db_name, db_key, db_id_str, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    db_meta.emplace(db_id_str, this);
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
    TxnTimeStamp db_create_ts;
    status = this->GetDBMeta(db_name, db_meta, db_create_ts);
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
    TxnTimeStamp create_table_ts;
    Status status = db_meta.GetTableID(table_name, table_key, table_id_str, create_table_ts);
    if (!status.ok()) {
        return status;
    }
    LOG_DEBUG(fmt::format("GetTableMeta: txn_id: {} table_id: {}", TxnID(), table_id_str));
    table_meta.emplace(db_meta.db_id_str(), table_id_str, this);
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
    TxnTimeStamp db_create_ts;
    status = this->GetDBMeta(db_name, db_meta, db_create_ts);
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
    TxnTimeStamp create_index_ts;
    Status status = table_meta.GetIndexID(index_name, index_key, index_id_str, create_index_ts);
    if (!status.ok()) {
        return status;
    }
    table_index_meta.emplace(index_id_str, table_meta);
    if (index_key_ptr) {
        *index_key_ptr = index_key;
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitCreateSnapshot(const WalCmdCreateSnapshot *create_snapshot_cmd) {
    // check if duplicate snapshot name
    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    String snapshot_name = create_snapshot_cmd->snapshot_name_;
    String snapshot_path = snapshot_dir + "/" + snapshot_name;
    if (std::filesystem::exists(snapshot_path)) {
        return Status::SnapshotAlreadyExists(snapshot_name);
    }

    // // dump indexes for snapshot
    // Vector<SharedPtr<MemIndexDetail>> table_mem_indexes = GetTableMemIndexes(db_name, table_name);

    // // Submit all dump tasks in parallel
    // Vector<SharedPtr<DumpMemIndexTask>> dump_tasks;
    // auto *dump_index_processor = InfinityContext::instance().storage()->dump_index_processor();

    // for (const auto &mem_index_detail : table_mem_indexes) {
    //     auto dump_index_task = MakeShared<DumpMemIndexTask>(mem_index_detail->db_name_, mem_index_detail->table_name_,
    //     mem_index_detail->index_name_, mem_index_detail->segment_id_, mem_index_detail->begin_row_id_); dump_tasks.push_back(dump_index_task);
    //     dump_index_processor->Submit(std::move(dump_index_task));
    // }

    // // Wait for all dumps to complete
    // Status status = Status::OK();
    // for (auto &dump_task : dump_tasks) {
    //     dump_task->Wait();
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    // After calling Checkpoint()
    TxnTimeStamp last_checkpoint_ts = InfinityContext::instance().storage()->wal_manager()->LastCheckpointTS();
    SharedPtr<CheckpointTxnStore> ckp_txn_store = MakeShared<CheckpointTxnStore>();
    Status status = this->CheckpointforSnapshot(last_checkpoint_ts, ckp_txn_store.get());
    if (!status.ok()) {
        return status;
    }

    // Check if checkpoint actually happened
    if (ckp_txn_store != nullptr) {
        if (!ckp_txn_store->entries_.empty()) {
            LOG_INFO(fmt::format("Checkpoint successful! {} blocks flushed", ckp_txn_store->entries_.size()));

            // Use the transaction store to generate WAL command
            SharedPtr<WalEntry> wal_entry = ckp_txn_store->ToWalEntry(txn_context_ptr_->commit_ts_);

            // There's only ONE command - the checkpoint command
            if (!wal_entry->cmds_.empty()) {
                auto checkpoint_cmd = static_cast<WalCmdCheckpointV2 *>(wal_entry->cmds_[0].get());
                PrepareCommitCheckpoint(checkpoint_cmd);
            }
        } else {
            LOG_INFO("Checkpoint transaction exists but no blocks were flushed (skip checkpoint)");
        }
    } else {
        LOG_INFO("No checkpoint transaction found");
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitRestoreSystemSnapshot(const WalCmdRestoreSystemSnapshot *restore_system_snapshot_cmd) {
    for (auto &drop_db_cmd : restore_system_snapshot_cmd->drop_database_wal_cmds_) {
        Status status = PrepareCommitDropDB(&drop_db_cmd);
        if (!status.ok()) {
            return status;
        }
    }
    for (auto &restore_db_cmd : restore_system_snapshot_cmd->restore_database_wal_cmds_) {
        Status status = PrepareCommitRestoreDatabaseSnapshot(&restore_db_cmd);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitCreateDB(const WalCmdCreateDatabaseV2 *create_db_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    Optional<DBMeeta> db_meta;
    const String *db_comment = create_db_cmd->db_comment_.empty() ? nullptr : &create_db_cmd->db_comment_;
    Status status = NewCatalog::AddNewDB(this, create_db_cmd->db_id_, commit_ts, create_db_cmd->db_name_, db_comment, db_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}
Status NewTxn::PrepareCommitDropDB(const WalCmdDropDatabaseV2 *drop_db_cmd) {

    String db_key;
    Optional<DBMeeta> db_meta;
    TxnTimeStamp create_db_ts;
    Status status = GetDBMeta(drop_db_cmd->db_name_, db_meta, create_db_ts, &db_key);
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE(fmt::format("Drop database: {}", drop_db_cmd->db_name_));

    String create_db_commit_ts = GetLastPartOfKey(db_key, '|');
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto ts_str = std::to_string(commit_ts);
    kv_instance_->Put(KeyEncode::DropDBKey(drop_db_cmd->db_name_, std::stoull(create_db_commit_ts), db_meta->db_id_str()), ts_str);

    return Status::OK();
}

Status NewTxn::PrepareCommitCreateTable(const WalCmdCreateTableV2 *create_table_cmd) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_table_cmd->db_name_;

    // Get database ID
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
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

Status NewTxn::PrepareCommitDropTable(const WalCmdDropTableV2 *drop_table_cmd) {
    //    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_id_str = drop_table_cmd->db_id_;
    const String &table_id_str = drop_table_cmd->table_id_;
    const String &table_key = drop_table_cmd->table_key_;
    TxnTimeStamp create_ts = infinity::GetTimestampFromKey(table_key);

    auto ts_str = std::to_string(txn_context_ptr_->commit_ts_);
    kv_instance_->Put(KeyEncode::DropTableKey(db_id_str, drop_table_cmd->table_name_, table_id_str, create_ts), ts_str);

    return Status::OK();
}

Status NewTxn::PrepareCommitRenameTable(const WalCmdRenameTableV2 *rename_table_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &db_id = rename_table_cmd->db_id_;
    const String &old_table_name = rename_table_cmd->table_name_;
    const String &old_table_key = rename_table_cmd->old_table_key_;
    const String &table_id = rename_table_cmd->table_id_;
    const TxnTimeStamp create_ts = infinity::GetTimestampFromKey(old_table_key);

    String ts_str = std::to_string(commit_ts);
    kv_instance_->Put(KeyEncode::RenameTableKey(db_id, old_table_name, table_id, create_ts), ts_str);

    // create new table key
    String new_table_key = KeyEncode::CatalogTableKey(db_id, rename_table_cmd->new_table_name_, commit_ts);
    Status status = kv_instance_->Put(new_table_key, table_id);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitAddColumns(const WalCmdAddColumnsV2 *add_columns_cmd) {
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
        status = table_meta->AddColumn(*column);
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

Status NewTxn::PrepareCommitDropColumns(const WalCmdDropColumnsV2 *drop_columns_cmd) {
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

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto ts_str = std::to_string(commit_ts);
    for (SizeT i = 0; i < drop_columns_cmd->column_names_.size(); ++i) {
        const String &column_key = drop_columns_cmd->column_keys_[i];
        TxnTimeStamp create_ts = infinity::GetTimestampFromKey(column_key);
        kv_instance_->Put(
            KeyEncode::DropTableColumnKey(drop_columns_cmd->db_id_, drop_columns_cmd->table_id_, drop_columns_cmd->column_names_[i], create_ts),
            ts_str);
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitCheckpoint(const WalCmdCheckpointV2 *checkpoint_cmd) {
    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(this);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, this);
        status = this->CommitCheckpointDB(db_meta, checkpoint_cmd);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCheckpointDB(DBMeeta &db_meta, const WalCmdCheckpointV2 *checkpoint_cmd) {
    Vector<String> *table_id_strs_ptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, this);
        status = this->CommitCheckpointTable(table_meta, checkpoint_cmd);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitRestoreTableSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, bool is_link_files) {

    const String &db_name = restore_table_snapshot_cmd->db_name_;

    // Get database ID
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }

    status = RestoreTableFromSnapshot(restore_table_snapshot_cmd, *db_meta, is_link_files);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::RestoreTableFromSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, DBMeeta &db_meta, bool is_link_files) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    Status status;
    Optional<TableMeeta> table_meta;
    if (!is_link_files) {
        status = NewCatalog::RestoreTable(db_meta,
                                          restore_table_snapshot_cmd->table_id_,
                                          begin_ts,
                                          commit_ts,
                                          restore_table_snapshot_cmd->table_def_,
                                          table_meta);
        // table with the same name already exists
        if (!status.ok()) {
            return status;
        }
    } else {
        table_meta.emplace(db_meta.db_id_str(), restore_table_snapshot_cmd->table_id_, this);
    }

    // restore meta data of the table
    status = table_meta->RestoreFromSnapshot(const_cast<WalCmdRestoreTableSnapshot *>(restore_table_snapshot_cmd), is_link_files);
    if (!status.ok()) {
        return status;
    }

    status = RestoreTableIndexesFromSnapshot(*table_meta, restore_table_snapshot_cmd->index_cmds_, is_link_files);
    if (!status.ok()) {
        return status;
    }

    // Initialize full-text index cache after table and indexes are restored
    table_meta->SetBeginTS(commit_ts);
    status = table_meta->LoadSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitRestoreDatabaseSnapshot(const WalCmdRestoreDatabaseSnapshot *restore_database_snapshot_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    Optional<DBMeeta> db_meta;
    const String *db_comment = restore_database_snapshot_cmd->db_comment_.empty() ? nullptr : &restore_database_snapshot_cmd->db_comment_;
    Status status = NewCatalog::AddNewDB(this,
                                         restore_database_snapshot_cmd->db_id_str_,
                                         commit_ts,
                                         restore_database_snapshot_cmd->db_name_,
                                         db_comment,
                                         db_meta);
    if (!status.ok()) {
        return status;
    }
    for (const auto &restore_table_snapshot_cmd : restore_database_snapshot_cmd->restore_table_wal_cmds_) {
        status = RestoreTableFromSnapshot(&restore_table_snapshot_cmd, *db_meta, false);
        if (!status.ok()) {
            return status;
        }
        // Get next table id of the db
        String next_table_id_key = KeyEncode::CatalogDbTagKey(restore_database_snapshot_cmd->db_id_str_, NEXT_TABLE_ID.data());
        String next_table_id_str;
        status = kv_instance_->Get(next_table_id_key, next_table_id_str);
        if (!status.ok()) {
            return status;
        }
        u64 next_table_id = std::stoull(next_table_id_str);
        u64 this_table_id = std::stoull(restore_table_snapshot_cmd.table_id_);
        if (this_table_id + 1 > next_table_id) {
            // Update the next table id
            String new_next_table_id_str = std::to_string(this_table_id + 1);
            status = kv_instance_->Put(next_table_id_key, new_next_table_id_str);
            if (!status.ok()) {
                return status;
            }
            LOG_TRACE(fmt::format("Update next table id to {} for database {}.", new_next_table_id_str, restore_database_snapshot_cmd->db_name_));
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
    String string_id;
    Status status = kv_instance_->Get(id_name.data(), string_id);
    if (!status.ok()) {
        return status;
    }
    SizeT id_num = std::stoull(string_id);
    ++id_num;
    id_str = fmt::format("{}", id_num);
    return kv_instance_->Put(id_name.data(), id_str);
    // return new_catalog_->IncrLatestID(id_str, id_name);
}

bool NewTxn::CheckConflictTxnStore(NewTxn *previous_txn, String &cause, bool &retry_query) {
    // FIXME: We will store information for more operations in the base_txn_store_ in the future.
    if (base_txn_store_ == nullptr || previous_txn->base_txn_store_ == nullptr) {
        return false;
    }

    TransactionType txn_type = base_txn_store_->type_;
    switch (txn_type) {
        case TransactionType::kCreateDB: {
            return CheckConflictTxnStore(static_cast<const CreateDBTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDropDB: {
            return CheckConflictTxnStore(static_cast<const DropDBTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kCreateTable: {
            return CheckConflictTxnStore(static_cast<const CreateTableTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kAppend: {
            return CheckConflictTxnStore(static_cast<const AppendTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kImport: {
            return CheckConflictTxnStore(static_cast<const ImportTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kCompact: {
            return CheckConflictTxnStore(static_cast<const CompactTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kCreateIndex: {
            return CheckConflictTxnStore(static_cast<const CreateIndexTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDropIndex: {
            return CheckConflictTxnStore(static_cast<const DropIndexTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDumpMemIndex: {
            return CheckConflictTxnStore(static_cast<const DumpMemIndexTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kOptimizeIndex: {
            return CheckConflictTxnStore(static_cast<const OptimizeIndexTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDelete: {
            return CheckConflictTxnStore(static_cast<const DeleteTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kAddColumn: {
            return CheckConflictTxnStore(static_cast<const AddColumnsTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDropColumn: {
            return CheckConflictTxnStore(static_cast<const DropColumnsTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kDropTable: {
            return CheckConflictTxnStore(static_cast<const DropTableTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kRenameTable: {
            return CheckConflictTxnStore(static_cast<const RenameTableTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kUpdate: {
            return CheckConflictTxnStore(static_cast<const UpdateTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kRestoreTable: {
            return CheckConflictTxnStore(static_cast<const RestoreTableTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kRestoreDatabase: {
            return CheckConflictTxnStore(static_cast<const RestoreDatabaseTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kCreateSnapshot: {
            return CheckConflictTxnStore(static_cast<const CreateSnapshotTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kRestoreSystem: {
            return CheckConflictTxnStore(static_cast<const RestoreSystemTxnStore &>(*base_txn_store_), previous_txn, cause, retry_query);
        }
        case TransactionType::kNewCheckpoint:
        default: {
            return false;
        }
    }
}

bool NewTxn::CheckConflictCmd(const WalCmd &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    switch (cmd.GetType()) {
        case WalCommandType::CREATE_DATABASE_V2: {
            return CheckConflictCmd(static_cast<const WalCmdCreateDatabaseV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::DROP_DATABASE_V2: {
            return CheckConflictCmd(static_cast<const WalCmdDropDatabaseV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::CREATE_TABLE_V2: {
            return CheckConflictCmd(static_cast<const WalCmdCreateTableV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::APPEND_V2: {
            return CheckConflictCmd(static_cast<const WalCmdAppendV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::IMPORT_V2: {
            return CheckConflictCmd(static_cast<const WalCmdImportV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::ADD_COLUMNS_V2: {
            return CheckConflictCmd(static_cast<const WalCmdAddColumnsV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::DROP_COLUMNS_V2: {
            return CheckConflictCmd(static_cast<const WalCmdDropColumnsV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::COMPACT_V2: {
            return CheckConflictCmd(static_cast<const WalCmdCompactV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::CREATE_INDEX_V2: {
            return CheckConflictCmd(static_cast<const WalCmdCreateIndexV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::DUMP_INDEX_V2: {
            return CheckConflictCmd(static_cast<const WalCmdDumpIndexV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::DELETE_V2: {
            return CheckConflictCmd(static_cast<const WalCmdDeleteV2 &>(cmd), previous_txn, cause, retry_query);
        }
        case WalCommandType::DROP_TABLE_V2: {
            return CheckConflictCmd(static_cast<const WalCmdDropTableV2 &>(cmd), previous_txn, cause, retry_query);
        }
        default: {
            return false;
        }
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdCreateDatabaseV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_DATABASE_V2: {
                auto *prev_cmd = static_cast<WalCmdCreateDatabaseV2 *>(wal_cmd.get());
                if (prev_cmd->db_name_ == db_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdDropDatabaseV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const CreateDBTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateDB: {
            CreateDBTxnStore *create_db_txn_store = static_cast<CreateDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreDatabase: {
            RestoreDatabaseTxnStore *restore_database_txn_store = static_cast<RestoreDatabaseTxnStore *>(previous_txn->base_txn_store_.get());
            if (restore_database_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->db_name_ == db_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const RestoreDatabaseTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateDB: {
            CreateDBTxnStore *create_db_txn_store = static_cast<CreateDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        case TransactionType::kRestoreDatabase: {
            RestoreDatabaseTxnStore *restore_database_txn_store = static_cast<RestoreDatabaseTxnStore *>(previous_txn->base_txn_store_.get());
            if (restore_database_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->db_name_ == db_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const RestoreSystemTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
    retry_query = false;
    return true;
}

bool NewTxn::CheckConflictTxnStore(const DropDBTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    bool conflict = false;
    //    LOG_TRACE(fmt::format("Txn: {}, current cmd: {}, previous txn: {}, previous cmd: {}",
    //                         this->txn_context_ptr_->txn_id_,
    //                         txn_store.ToString(),
    //                         previous_txn->txn_context_ptr_->txn_id_,
    //                         previous_txn->base_txn_store_->ToString()));
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->db_name_ == db_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdCreateTableV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const SharedPtr<String> &table_name = cmd.table_def_->table_name();
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_DATABASE_V2: {
                auto *prev_cmd = static_cast<WalCmdCreateDatabaseV2 *>(wal_cmd.get());
                if (prev_cmd->db_name_ == db_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::CREATE_TABLE_V2: {
                auto *prev_cmd = static_cast<WalCmdCreateTableV2 *>(wal_cmd.get());
                const SharedPtr<String> &prev_table_name = prev_cmd->table_def_->table_name();
                if (prev_cmd->db_name_ == db_name && *prev_table_name == *table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == *table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const CreateTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateTable: {
            CreateTableTxnStore *create_table_txn_store = static_cast<CreateTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_table_txn_store->db_name_ == db_name && create_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRenameTable: {
            RenameTableTxnStore *rename_table_txn_store = static_cast<RenameTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (rename_table_txn_store->db_name_ == db_name && rename_table_txn_store->new_table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreTable: {
            RestoreTableTxnStore *restore_table_txn_store = static_cast<RestoreTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (restore_table_txn_store->db_name_ == db_name && restore_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const RestoreTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateTable: {
            CreateTableTxnStore *create_table_txn_store = static_cast<CreateTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_table_txn_store->db_name_ == db_name && create_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRenameTable: {
            RenameTableTxnStore *rename_table_txn_store = static_cast<RenameTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (rename_table_txn_store->db_name_ == db_name && rename_table_txn_store->new_table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreTable: {
            RestoreTableTxnStore *restore_table_txn_store = static_cast<RestoreTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (restore_table_txn_store->db_name_ == db_name && restore_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdAppendV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    Set<SegmentID> segment_ids;
    for (const auto &row_range : cmd.row_ranges_) {
        RowID row_id = row_range.first;
        segment_ids.insert(row_id.segment_id_);
    }
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                if (dump_index_cmd->db_name_ == db_name && dump_index_cmd->table_name_ == table_name &&
                    segment_ids.contains(dump_index_cmd->segment_id_) && dump_index_cmd->dump_cause_ != DumpIndexCause::kOptimizeIndex) {
                    conflict = true;
                }
                break;
            }
                //            case WalCommandType::APPEND_V2: {
                //                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                //                if (append_cmd->db_name_ == db_name && append_cmd->table_name_ == table_name) {
                //                    conflict = true;
                //                }
                //                break;
                //            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_columns_cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                if (add_columns_cmd->db_name_ == db_name && add_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (drop_columns_cmd->db_name_ == db_name && drop_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                // No conflict
                break;
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const AppendTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kAddColumn: {
            AddColumnsTxnStore *add_columns_txn_store = static_cast<AddColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (add_columns_txn_store->db_name_ == db_name && add_columns_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRenameTable: {
            RenameTableTxnStore *rename_table_txn_store = static_cast<RenameTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (rename_table_txn_store->db_name_ == db_name && rename_table_txn_store->old_table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdImportV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_columns_cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                if (add_columns_cmd->db_name_ == db_name && add_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (drop_columns_cmd->db_name_ == db_name && drop_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const ImportTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kAddColumn: {
            AddColumnsTxnStore *add_columns_txn_store = static_cast<AddColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (add_columns_txn_store->db_name_ == db_name && add_columns_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdAddColumnsV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                if (append_cmd->db_name_ == db_name && append_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(wal_cmd.get());
                if (import_cmd->db_name_ == db_name && import_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const AddColumnsTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kAppend: {
            AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(previous_txn->base_txn_store_.get());
            if (append_txn_store->db_name_ == db_name && append_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kImport: {
            ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(previous_txn->base_txn_store_.get());
            if (import_txn_store->db_name_ == db_name && import_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kAddColumn: {
            AddColumnsTxnStore *add_columns_txn_store = static_cast<AddColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (add_columns_txn_store->db_name_ == db_name && add_columns_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdDropColumnsV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                if (append_cmd->db_name_ == db_name && append_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(wal_cmd.get());
                if (import_cmd->db_name_ == db_name && import_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    for (const auto &column_name : cmd.column_names_) {
                        if (create_index_cmd->index_base_->ContainsColumn(column_name)) {
                            retry_query = false;
                            conflict = true;
                        }
                    }
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const DropColumnsTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kAppend: {
            AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(previous_txn->base_txn_store_.get());
            if (append_txn_store->db_name_ == db_name && append_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kImport: {
            ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(previous_txn->base_txn_store_.get());
            if (import_txn_store->db_name_ == db_name && import_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name) {
                for (const auto &column_name : txn_store.column_names_) {
                    if (create_index_txn_store->index_base_->ContainsColumn(column_name)) {
                        retry_query = false;
                        conflict = true;
                    }
                }
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name &&
                std::find_first_of(txn_store.column_names_.begin(),
                                   txn_store.column_names_.end(),
                                   drop_columns_txn_store->column_names_.begin(),
                                   drop_columns_txn_store->column_names_.end()) != txn_store.column_names_.end()) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdCompactV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    Set<SegmentID> segment_ids;
    for (const auto segment_id : cmd.deprecated_segment_ids_) {
        segment_ids.insert(segment_id);
    }
    for (const auto &new_segment_info : cmd.new_segment_infos_) {
        segment_ids.insert(new_segment_info.segment_id_);
    }
    const Vector<SharedPtr<WalCmd>> &wal_cmds = previous_txn->wal_entry_->cmds_;
    for (const SharedPtr<WalCmd> &wal_cmd : wal_cmds) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (create_index_cmd->db_name_ == db_name && create_index_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_columns_cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get());
                if (add_columns_cmd->db_name_ == db_name && add_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (drop_columns_cmd->db_name_ == db_name && drop_columns_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DELETE_V2: {
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(wal_cmd.get());
                if (delete_cmd->db_name_ == db_name && delete_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(wal_cmd.get());
                switch (dump_index_cmd->dump_cause_) {
                    case DumpIndexCause::kOptimizeIndex: {
                        // Compact table conflicts with optimize index
                        if (segment_ids.contains(dump_index_cmd->segment_id_)) {
                            conflict = true;
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const CompactTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    const Vector<SegmentID> &segment_ids = txn_store.deprecated_segment_ids_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kAddColumn: {
            AddColumnsTxnStore *add_columns_txn_store = static_cast<AddColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (add_columns_txn_store->db_name_ == db_name && add_columns_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDelete: {
            DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(previous_txn->base_txn_store_.get());
            if (delete_txn_store->db_name_ == db_name && delete_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kOptimizeIndex: {
            OptimizeIndexTxnStore *optimize_index_txn_store = static_cast<OptimizeIndexTxnStore *>(previous_txn->base_txn_store_.get());
            for (const auto &store_entry : optimize_index_txn_store->entries_) {
                if (store_entry.db_name_ == db_name && store_entry.table_name_ == table_name &&
                    std::find(segment_ids.begin(), segment_ids.end(), store_entry.segment_id_) != segment_ids.end()) {
                    retry_query = false;
                    conflict = true;
                    break;
                }
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdCreateIndexV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    const String &index_name = *cmd.index_base_->index_name_;
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_INDEX_V2: {
                auto *prev_cmd = static_cast<WalCmdCreateIndexV2 *>(wal_cmd.get());
                if (prev_cmd->db_name_ == db_name && prev_cmd->table_name_ == table_name && *prev_cmd->index_base_->index_name_ == index_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(wal_cmd.get());
                if (append_cmd->db_name_ == db_name && append_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(wal_cmd.get());
                if (import_cmd->db_name_ == db_name && import_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_columns_cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                if (drop_columns_cmd->db_name_ == db_name && drop_columns_cmd->table_name_ == table_name) {
                    for (const auto &column_name : drop_columns_cmd->column_names_) {
                        if (cmd.index_base_->ContainsColumn(column_name)) {
                            retry_query = false;
                            conflict = true;
                        }
                    }
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const CreateIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    const String &index_name = *txn_store.index_base_->index_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name &&
                *create_index_txn_store->index_base_->index_name_ == index_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kAppend: {
            AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(previous_txn->base_txn_store_.get());
            if (append_txn_store->db_name_ == db_name && append_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kImport: {
            ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(previous_txn->base_txn_store_.get());
            if (import_txn_store->db_name_ == db_name && import_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name) {
                for (const auto &column_name : drop_columns_txn_store->column_names_) {
                    if (txn_store.index_base_->ContainsColumn(column_name)) {
                        retry_query = false;
                        conflict = true;
                    }
                }
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const DropIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    const String &index_name = txn_store.index_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name &&
                drop_index_txn_store->index_name_ == index_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if ((create_snapshot_txn_store->db_name_ == db_name &&
                 (create_snapshot_txn_store->table_name_ == table_name || create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase)) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdDumpIndexV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    if (cmd.dump_cause_ != DumpIndexCause::kOptimizeIndex)
        return false;
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    Set<SegmentID> segment_ids;
                    for (const auto segment_id : compact_cmd->deprecated_segment_ids_) {
                        segment_ids.insert(segment_id);
                    }
                    for (const auto &new_segment_info : compact_cmd->new_segment_infos_) {
                        segment_ids.insert(new_segment_info.segment_id_);
                    }
                    if (segment_ids.contains(cmd.segment_id_)) {
                        conflict = true;
                    }
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                ;
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdDeleteV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    const String &table_name = cmd.table_name_;
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(wal_cmd.get());
                if (compact_cmd->db_name_ == db_name && compact_cmd->table_name_ == table_name) {
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(wal_cmd.get());
                if (drop_table_cmd->db_name_ == db_name && drop_table_cmd->table_name_ == table_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_db_cmd->db_name_ == db_name) {
                    retry_query = false;
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictCmd(const WalCmdDropTableV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = cmd.db_name_;
    //    const String &table_name = cmd.table_name_;
    for (SharedPtr<WalCmd> &wal_cmd : previous_txn->wal_entry_->cmds_) {
        bool conflict = false;
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::DROP_DATABASE_V2: {
                auto *drop_database_cmd = static_cast<WalCmdDropDatabaseV2 *>(wal_cmd.get());
                if (drop_database_cmd->db_name_ == db_name) {
                    conflict = true;
                }
                break;
            }
            default: {
                //
            }
        }
        if (conflict) {
            cause = fmt::format("{} vs. {}", wal_cmd->CompactInfo(), cmd.CompactInfo());
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const DropTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const OptimizeIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const Vector<String> &db_names = txn_store.db_names_;
    const Map<String, Vector<String>> &table_names_in_db = txn_store.table_names_in_db_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kOptimizeIndex: {
            OptimizeIndexTxnStore *optimize_index_txn_store = static_cast<OptimizeIndexTxnStore *>(previous_txn->base_txn_store_.get());
            const Vector<String> &prev_db_names = optimize_index_txn_store->db_names_;
            const Map<String, Vector<String>> &prev_table_names_in_db = optimize_index_txn_store->table_names_in_db_;

            // If there are multiple databases or multiple tables involved in index optimization, the optimization is processed for all indexes.
            if (db_names.size() > 1 || prev_db_names.size() > 1 || table_names_in_db.at(db_names[0]).size() > 1 ||
                prev_table_names_in_db.at(prev_db_names[0]).size() > 1) {
                conflict = true;
            } else {
                for (const auto &prev_store_entry : optimize_index_txn_store->entries_) {
                    for (const auto &current_store_entry : txn_store.entries_) {
                        if (prev_store_entry.db_name_ == current_store_entry.db_name_ &&
                            prev_store_entry.table_name_ == current_store_entry.table_name_ &&
                            prev_store_entry.index_name_ == current_store_entry.index_name_) {
                            retry_query = false;
                            conflict = true;
                            break;
                        }
                    }
                    if (conflict) {
                        break;
                    }
                }
            }
            break;
        }
        case TransactionType::kAppend: {
            AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = append_txn_store->db_name_;
            const String &prev_table_name = append_txn_store->table_name_;
            if (std::find(db_names.begin(), db_names.end(), prev_db_name) != db_names.end() &&
                std::find(table_names_in_db.at(prev_db_name).begin(), table_names_in_db.at(prev_db_name).end(), prev_table_name) !=
                    table_names_in_db.at(prev_db_name).end()) {
                conflict = true;
            }
            break;
        }
        // There shold be no conflict between optimize index and import
        // case TransactionType::kImport: {
        //     LOG_INFO("OptimizeIndexTxnStore vs. ImportTxnStore conflict");
        //     ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(previous_txn->base_txn_store_.get());
        //     const String &prev_db_name = import_txn_store->db_name_;
        //     const String &prev_table_name = import_txn_store->table_name_;
        //     if (std::find(db_names.begin(), db_names.end(), prev_db_name) != db_names.end() &&
        //         std::find(table_names_in_db.at(prev_db_name).begin(), table_names_in_db.at(prev_db_name).end(), prev_table_name) !=
        //             table_names_in_db.at(prev_db_name).end()) {
        //         conflict = true;
        //     }
        //     break;
        // }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = compact_txn_store->db_name_;
            const String &prev_table_name = compact_txn_store->table_name_;
            for (const auto &store_entry : txn_store.entries_) {
                if (store_entry.db_name_ == prev_db_name && store_entry.table_name_ == prev_table_name &&
                    std::find(compact_txn_store->deprecated_segment_ids_.begin(),
                              compact_txn_store->deprecated_segment_ids_.end(),
                              store_entry.segment_id_) != compact_txn_store->deprecated_segment_ids_.end()) {
                    conflict = true;
                    break;
                }
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = update_txn_store->db_name_;
            const String &prev_table_name = update_txn_store->table_name_;
            if (std::find(db_names.begin(), db_names.end(), prev_db_name) != db_names.end() &&
                std::find(table_names_in_db.at(prev_db_name).begin(), table_names_in_db.at(prev_db_name).end(), prev_table_name) !=
                    table_names_in_db.at(prev_db_name).end()) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = drop_index_txn_store->db_name_;
            const String &prev_table_name = drop_index_txn_store->table_name_;
            const String &prev_index_name = drop_index_txn_store->index_name_;

            // If there are multiple databases or multiple tables involved in index optimization, the optimization is processed for all indexes.
            if (db_names.size() > 1 || table_names_in_db.at(db_names[0]).size() > 1) {
                conflict = true;
            } else {
                for (const auto &current_store_entry : txn_store.entries_) {
                    if (prev_db_name == current_store_entry.db_name_ && prev_table_name == current_store_entry.table_name_ &&
                        prev_index_name == current_store_entry.index_name_) {
                        retry_query = false;
                        conflict = true;
                        break;
                    }
                }
            }
            break;
        }
        case TransactionType::kDumpMemIndex: {
            DumpMemIndexTxnStore *dump_index_txn_store = static_cast<DumpMemIndexTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = dump_index_txn_store->db_name_;
            const String &prev_table_name = dump_index_txn_store->table_name_;
            const String &prev_index_name = dump_index_txn_store->index_name_;
            for (const auto &current_store_entry : txn_store.entries_) {
                if (prev_db_name == current_store_entry.db_name_ && prev_table_name == current_store_entry.table_name_ &&
                    prev_index_name == current_store_entry.index_name_) {
                    conflict = true;
                    break;
                }
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (std::find(db_names.begin(), db_names.end(), drop_db_txn_store->db_name_) != db_names.end()) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = drop_table_txn_store->db_name_;
            const String &prev_table_name = drop_table_txn_store->table_name_;
            if (std::find(db_names.begin(), db_names.end(), prev_db_name) != db_names.end() &&
                std::find(table_names_in_db.at(prev_db_name).begin(), table_names_in_db.at(prev_db_name).end(), prev_table_name) !=
                    table_names_in_db.at(prev_db_name).end()) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            const String &prev_db_name = create_snapshot_txn_store->db_name_;
            const String &prev_table_name = create_snapshot_txn_store->table_name_;
            if (std::find(db_names.begin(), db_names.end(), prev_db_name) != db_names.end() &&
                std::find(table_names_in_db.at(prev_db_name).begin(), table_names_in_db.at(prev_db_name).end(), prev_table_name) !=
                    table_names_in_db.at(prev_db_name).end()) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const DumpMemIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    const String &index_name = txn_store.index_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kDumpMemIndex: {
            DumpMemIndexTxnStore *dump_mem_index_txn_store = static_cast<DumpMemIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (dump_mem_index_txn_store->db_name_ == db_name && dump_mem_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name &&
                drop_index_txn_store->index_name_ == index_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const DeleteTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDelete: {
            DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(previous_txn->base_txn_store_.get());
            if (delete_txn_store->db_name_ == db_name && delete_txn_store->table_name_ == table_name &&
                std::find_first_of(txn_store.row_ids_.begin(),
                                   txn_store.row_ids_.end(),
                                   delete_txn_store->row_ids_.begin(),
                                   delete_txn_store->row_ids_.end()) != txn_store.row_ids_.end()) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name &&
                std::find_first_of(txn_store.row_ids_.begin(),
                                   txn_store.row_ids_.end(),
                                   update_txn_store->row_ids_.begin(),
                                   update_txn_store->row_ids_.end()) != txn_store.row_ids_.end()) {

                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }

    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const RenameTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.old_table_name_;
    const String &new_table_name = txn_store.new_table_name_;
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCreateTable: {
            CreateTableTxnStore *create_table_txn_store = static_cast<CreateTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_table_txn_store->db_name_ == db_name && create_table_txn_store->table_name_ == new_table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRenameTable: {
            RenameTableTxnStore *rename_table_txn_store = static_cast<RenameTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (rename_table_txn_store->db_name_ == db_name && rename_table_txn_store->old_table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreTable: {
            RestoreTableTxnStore *restore_table_txn_store = static_cast<RestoreTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (restore_table_txn_store->db_name_ == db_name && restore_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }
    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const UpdateTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    const String &db_name = txn_store.db_name_;
    const String &table_name = txn_store.table_name_;
    Set<SegmentID> segment_ids;
    for (const auto &row_id : txn_store.row_ids_) {
        segment_ids.insert(row_id.segment_id_);
    }
    bool conflict = false;
    switch (previous_txn->base_txn_store_->type_) {
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(previous_txn->base_txn_store_.get());
            if (compact_txn_store->db_name_ == db_name && compact_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_index_txn_store->db_name_ == db_name && create_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropIndex: {
            DropIndexTxnStore *drop_index_txn_store = static_cast<DropIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_index_txn_store->db_name_ == db_name && drop_index_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kAddColumn: {
            AddColumnsTxnStore *add_columns_txn_store = static_cast<AddColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (add_columns_txn_store->db_name_ == db_name && add_columns_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropColumn: {
            DropColumnsTxnStore *drop_columns_txn_store = static_cast<DropColumnsTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_columns_txn_store->db_name_ == db_name && drop_columns_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDumpMemIndex: {
            LOG_INFO("UpdateTxnStore vs. DumpMemIndexTxnStore conflict");
            LOG_INFO(fmt::format("segment_ids_size: {}", segment_ids.size()));
            for (SegmentID segment_id : segment_ids) {
                LOG_INFO(fmt::format("segment_id: {}", segment_id));
            }
            DumpMemIndexTxnStore *dump_index_txn_store = static_cast<DumpMemIndexTxnStore *>(previous_txn->base_txn_store_.get());
            if (dump_index_txn_store->db_name_ == db_name && dump_index_txn_store->table_name_ == table_name) {
                for (SegmentID segment_id : dump_index_txn_store->segment_ids_) {
                    if (segment_ids.contains(segment_id)) {
                        LOG_INFO("same segment id");
                        conflict = true;
                        break;
                    }
                }
            }
            break;
        }
        case TransactionType::kDelete: {
            DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(previous_txn->base_txn_store_.get());
            if (delete_txn_store->db_name_ == db_name && delete_txn_store->table_name_ == table_name &&
                std::find_first_of(txn_store.row_ids_.begin(),
                                   txn_store.row_ids_.end(),
                                   delete_txn_store->row_ids_.begin(),
                                   delete_txn_store->row_ids_.end()) != txn_store.row_ids_.end()) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kUpdate: {
            UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(previous_txn->base_txn_store_.get());
            if (update_txn_store->db_name_ == db_name && update_txn_store->table_name_ == table_name) {
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropDB: {
            DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_db_txn_store->db_name_ == db_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kDropTable: {
            DropTableTxnStore *drop_table_txn_store = static_cast<DropTableTxnStore *>(previous_txn->base_txn_store_.get());
            if (drop_table_txn_store->db_name_ == db_name && drop_table_txn_store->table_name_ == table_name) {
                retry_query = false;
                conflict = true;
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            CreateSnapshotTxnStore *create_snapshot_txn_store = static_cast<CreateSnapshotTxnStore *>(previous_txn->base_txn_store_.get());
            if (create_snapshot_txn_store->table_name_ == table_name ||
                (create_snapshot_txn_store->db_name_ == db_name && create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kDatabase) ||
                create_snapshot_txn_store->snapshot_type_ == SnapshotScope::kSystem) {
                retry_query = true;
                conflict = true;
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            retry_query = false;
            conflict = true;
            break;
        }
        default: {
        }
    }
    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return false;
}

bool NewTxn::CheckConflictTxnStore(const CreateSnapshotTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query) {
    retry_query = true;
    bool conflict = true;
    if (conflict) {
        cause = fmt::format("{} vs. {}", previous_txn->base_txn_store_->ToString(), txn_store.ToString());
        return true;
    }
    return true;
}

bool NewTxn::CheckConflict1(SharedPtr<NewTxn> check_txn, String &conflict_reason, bool &retry_query) {
    // LOG_INFO(fmt::format("Txn {} check conflict with txn: {}.", *txn_text_, *check_txn->txn_text_));
    for (SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        bool conflict = this->CheckConflictCmd(*wal_cmd, check_txn.get(), conflict_reason, retry_query);
        if (conflict) {
            // conflicted_txn_ = check_txn;
            return true;
        }
    }
    return false;
}

bool NewTxn::CheckConflictTxnStores(SharedPtr<NewTxn> check_txn, String &conflict_reason, bool &retry_query) {
    LOG_TRACE(fmt::format("CheckConflictTxnStores::Txn {} check conflict with txn: {}.", *txn_text_, *check_txn->txn_text_));
    bool conflict = this->CheckConflictTxnStore(check_txn.get(), conflict_reason, retry_query);
    if (conflict) {
        // conflicted_txn_ = check_txn;
        return true;
    }
    return false;
}

void NewTxn::CommitBottom() {
    TransactionID txn_id = this->TxnID();
    LOG_TRACE(fmt::format("Transaction commit bottom: {} start.", txn_id));
    TxnState txn_state = this->GetTxnState();
    if (txn_state != TxnState::kCommitting) {
        UnrecoverableError(fmt::format("Unexpected transaction state: {}", TxnState2Str(txn_state)));
    }

    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(command.get());
                auto status = CommitBottomAppend(append_cmd);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("CommitBottomAppend failed: {}", status.message()));
                }
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(command.get());
                if (dump_index_cmd->dump_cause_ == DumpIndexCause::kDumpMemIndex && !IsReplay()) {
                    auto status = CommitBottomDumpMemIndex(dump_index_cmd);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("CommitBottomDumpMemIndex failed: {}", status.message()));
                    }
                }
                break;
            }
            case WalCommandType::CREATE_SNAPSHOT: {
                if (this->IsReplay()) {
                    break;
                }
                auto *create_snapshot_cmd = static_cast<WalCmdCreateSnapshot *>(command.get());
                if (create_snapshot_cmd->snapshot_type_ == SnapshotScope::kTable) {
                    Status status = CommitBottomCreateTableSnapshot(create_snapshot_cmd->db_name_,
                                                                    create_snapshot_cmd->table_name_,
                                                                    create_snapshot_cmd->snapshot_name_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("CommitBottomCreateTableSnapshot failed: {}", status.message()));
                    }
                } else if (create_snapshot_cmd->snapshot_type_ == SnapshotScope::kDatabase) {
                    Status status = CommitBottomCreateDatabaseSnapshot(create_snapshot_cmd->db_name_, create_snapshot_cmd->snapshot_name_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("CommitBottomCreateDatabaseSnapshot failed: {}", status.message()));
                    }
                } else if (create_snapshot_cmd->snapshot_type_ == SnapshotScope::kSystem) {
                    Status status = CommitBottomCreateSystemSnapshot(create_snapshot_cmd->snapshot_name_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("CommitBottomCreateSystemSnapshot failed: {}", status.message()));
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    TxnTimeStamp commit_ts = this->CommitTS();
    String commit_ts_str = std::to_string(commit_ts);
    for (const String &meta_key : keys_wait_for_commit_) {
        kv_instance_->Put(meta_key, commit_ts_str);
    }
    txn_mgr_->CommitBottom(this);
}

void NewTxn::NotifyTopHalf() {
    TxnState txn_state = this->GetTxnState();
    if (txn_state == TxnState::kCommitting) {
        // Try to commit rocksdb transaction
        txn_mgr_->CommitKVInstance(this);
    }
    // Notify the top half
    std::unique_lock<std::mutex> lk(commit_lock_);
    commit_bottom_done_ = true;
    commit_cv_.notify_one();
    LOG_TRACE(fmt::format("Transaction {} notify top half, commit ts {}.", TxnID(), CommitTS()));
}

void NewTxn::PostCommit() {

    for (auto &sema : this->semas()) {
        sema->acquire();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    //    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    for (const SharedPtr<WalCmd> &wal_cmd : wal_entry_->cmds_) {
        WalCommandType command_type = wal_cmd->GetType();
        switch (command_type) {
            case WalCommandType::APPEND_V2: {
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
                //                auto *cmd = static_cast<WalCmdAddColumnsV2 *>(wal_cmd.get())
                break;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                //                auto *cmd = static_cast<WalCmdDropColumnsV2 *>(wal_cmd.get());
                break;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                break;
            }
            case WalCommandType::COMPACT_V2: {
                // auto *cmd = static_cast<WalCmdCompact *>(wal_cmd.get());
                break;
            }
            case WalCommandType::CLEANUP: {
                auto *cmd = static_cast<WalCmdCleanup *>(wal_cmd.get());
                new_catalog_->SetLastCleanupTS(cmd->timestamp_);
                break;
            }
            default: {
                break;
            }
        }
    }

    TransactionType txn_type = GetTxnType();
    if (txn_type == TransactionType::kNewCheckpoint) {
        if (!wal_entry_->cmds_.empty()) {
            // Shouldn't set the ckp ts if checkpoint is skipped.
            wal_manager->SetLastCheckpointTS(current_ckp_ts_);
            wal_manager->SetLastCkpWalSize(wal_size_); // Update last checkpoint wal size
        }
    }

    if (!this->IsReplay()) {
        // To avoid the txn is hold by other object and the data in base_txn_store can't be released.
        base_txn_store_->ClearData();
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
    TransactionType txn_type = TransactionType::kInvalid;
    if (base_txn_store_ != nullptr) {
        txn_type = base_txn_store_->type_;
    }
    switch (txn_type) {
        case TransactionType::kCreateDB: {
            break;
        }
        case TransactionType::kDropDB: {
            break;
        }
        case TransactionType::kCreateTable: {
            break;
        }
        case TransactionType::kAppend: {
            break;
        }
        case TransactionType::kImport: {
            ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store_.get());
            for (auto &[_, blocks] : import_txn_store->input_blocks_in_imports_) {
                std::for_each(blocks.begin(), blocks.end(), [](auto &block) { block->UnInit(); });
            }

            const Vector<SegmentID> &segment_ids = import_txn_store->segment_ids_;
            Vector<UniquePtr<MetaKey>> metas;
            auto &db_id_str = import_txn_store->db_id_str_;
            auto &table_id_str = import_txn_store->table_id_str_;
            auto index_names_size = import_txn_store->index_names_.size();
            for (SizeT i = 0; i < index_names_size; ++i) {
                // Restore memory index here
                auto index_id_str = import_txn_store->index_ids_str_[i];
                for (SegmentID segment_id : segment_ids) {
                    metas.emplace_back(MakeUnique<SegmentMetaKey>(db_id_str, table_id_str, segment_id));
                    metas.emplace_back(MakeUnique<SegmentIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id));
                }
            }

            Status status = CleanupInner(std::move(metas));
            // if (status.code_ == ErrorCode::kIOError) {
            //     // TODO: move metas to kv_store
            // } else
            if (!status.ok()) {
                UnrecoverableError("During PostRollback, cleanup failed.");
            }
            break;
        }
        case TransactionType::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store_.get());
            Vector<UniquePtr<MetaKey>> metas;
            auto &db_id_str = compact_txn_store->db_id_str_;
            auto &table_id_str = compact_txn_store->table_id_str_;
            auto index_names_size = compact_txn_store->index_names_.size();
            for (SizeT i = 0; i < index_names_size; ++i) {
                // Restore memory index here
                auto index_id_str = compact_txn_store->index_ids_str_[i];
                const Vector<SegmentID> &segment_ids_ = compact_txn_store->segment_ids_;
                for (SegmentID segment_id : segment_ids_) {
                    metas.emplace_back(MakeUnique<SegmentMetaKey>(db_id_str, table_id_str, segment_id));
                    metas.emplace_back(MakeUnique<SegmentIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id));
                }
            }

            Status status = CleanupInner(std::move(metas));
            // if (status.code_ == ErrorCode::kIOError) {
            //     // TODO: move metas to kv_store
            // } else
            if (!status.ok()) {
                UnrecoverableError("During PostRollback, cleanup failed.");
            }
            break;
        }
        case TransactionType::kCreateIndex: {
            CreateIndexTxnStore *create_index_txn_store = static_cast<CreateIndexTxnStore *>(base_txn_store_.get());
            TableMeeta table_meta(create_index_txn_store->db_id_str_,
                                  create_index_txn_store->table_id_str_,
                                  kv_instance_.get(),
                                  abort_ts,
                                  MAX_TIMESTAMP);
            Vector<String> *index_id_strs_ptr = nullptr;
            Status status = table_meta.GetIndexIDs(index_id_strs_ptr);
            if (!status.ok()) {
                RecoverableError(status);
            }

            if (std::find(index_id_strs_ptr->begin(), index_id_strs_ptr->end(), create_index_txn_store->index_id_str_) != index_id_strs_ptr->end()) {
                Vector<UniquePtr<MetaKey>> metas;
                metas.emplace_back(MakeUnique<TableIndexMetaKey>(create_index_txn_store->db_id_str_,
                                                                 create_index_txn_store->table_id_str_,
                                                                 create_index_txn_store->index_id_str_,
                                                                 *create_index_txn_store->index_base_->index_name_));

                status = CleanupInner(std::move(metas));
                if (!status.ok()) {
                    UnrecoverableError("During PostRollback, cleanup failed.");
                }
            }
            break;
        }
        case TransactionType::kDropIndex: {
            break;
        }
        case TransactionType::kDumpMemIndex: {
            break;
        }
        case TransactionType::kOptimizeIndex: {
            OptimizeIndexTxnStore *optimize_index_txn_store = static_cast<OptimizeIndexTxnStore *>(base_txn_store_.get());
            Vector<UniquePtr<MetaKey>> metas;
            auto &entries = optimize_index_txn_store->entries_;

            for (const auto &entry : entries) {
                auto &db_id_str = entry.db_id_str_;
                auto &table_id_str = entry.table_id_str_;
                auto &index_id_str = entry.index_id_str_;
                auto segment_id = entry.segment_id_;
                auto &new_chunk_infos = entry.new_chunk_infos_;
                for (const auto &new_chunk_info : new_chunk_infos) {
                    auto chunk_id = new_chunk_info.chunk_id_;
                    metas.emplace_back(MakeUnique<ChunkIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id, chunk_id));
                }
            }

            Status status = CleanupInner(std::move(metas));
            // if (status.code_ == ErrorCode::kIOError) {
            //     // TODO: move metas to kv_store
            // } else
            if (!status.ok()) {
                UnrecoverableError("During PostRollback, cleanup failed.");
            }
            break;
        }
        case TransactionType::kDelete: {
            DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(base_txn_store_.get());
            Status status = RollbackDelete(delete_txn_store);
            if (!status.ok()) {
                UnrecoverableError("Fail to rollback delete operation");
            }
            break;
        }
        case TransactionType::kAddColumn: {
            break;
        }
        case TransactionType::kDropColumn: {
            break;
        }
        case TransactionType::kDropTable: {
            break;
        }
        case TransactionType::kRenameTable: {
            break;
        }
        case TransactionType::kUpdate: {
            break;
        }
        case TransactionType::kNewCheckpoint: {
            UnrecoverableError("Unexpected case: rollback checkpoint");
            break;
        }
        case TransactionType::kRestoreTable: {
            RestoreTableTxnStore *restore_table_txn_store = static_cast<RestoreTableTxnStore *>(base_txn_store_.get());

            // Clean up the copied files from snapshot
            Config *config = InfinityContext::instance().config();
            String data_dir = config->DataDir();

            // Remove all the files that were copied during restore
            for (const auto &segment_info : restore_table_txn_store->segment_infos_) {
                // Remove segment files
                String segment_dir = fmt::format("{}/db_{}/tbl_{}/seg_{}",
                                                 data_dir,
                                                 restore_table_txn_store->db_id_str_,
                                                 restore_table_txn_store->table_id_str_,
                                                 segment_info.segment_id_);

                if (VirtualStore::Exists(segment_dir)) {
                    Status remove_status = VirtualStore::RemoveDirectory(segment_dir);
                    if (!remove_status.ok()) {
                        LOG_WARN(fmt::format("Failed to remove segment directory during rollback: {}", segment_dir));
                    }
                }
            }

            // Remove index files
            for (const auto &index_cmd : restore_table_txn_store->index_cmds_) {
                String index_dir = fmt::format("{}/db_{}/tbl_{}/idx_{}",
                                               data_dir,
                                               restore_table_txn_store->db_id_str_,
                                               restore_table_txn_store->table_id_str_,
                                               index_cmd.index_id_);

                if (VirtualStore::Exists(index_dir)) {
                    Status remove_status = VirtualStore::RemoveDirectory(index_dir);
                    if (!remove_status.ok()) {
                        LOG_WARN(fmt::format("Failed to remove index directory during rollback: {}", index_dir));
                    }
                }
            }

            // Remove the table directory if it's empty
            String table_dir = fmt::format("{}/db_{}/tbl_{}", data_dir, restore_table_txn_store->db_id_str_, restore_table_txn_store->table_id_str_);

            if (VirtualStore::Exists(table_dir)) {
                Status remove_status = VirtualStore::RemoveDirectory(table_dir);
                if (!remove_status.ok()) {
                    LOG_WARN(fmt::format("Failed to removetable directory during rollback: {}", table_dir));
                }
            }

            // // Clean up metadata entries that were created
            // Vector<UniquePtr<MetaKey>> metas;
            // for (const auto &segment_info : restore_table_txn_store->segment_infos_) {
            //     metas.emplace_back(MakeUnique<SegmentMetaKey>(restore_table_txn_store->db_id_str_,
            //                                                   restore_table_txn_store->table_id_str_,
            //                                                   segment_info.segment_id_));
            //     for (const auto &block_id : segment_info.block_ids_) {
            //     metas.emplace_back(MakeUnique<BlockMetaKey>(restore_table_txn_store->db_id_str_,
            //                                                 restore_table_txn_store->table_id_str_,
            //                                                 segment_info.segment_id_,
            //                                                 block_id));
            //         for (const auto &column_def : restore_table_txn_store->table_def_->columns()) {
            //             metas.emplace_back(MakeUnique<ColumnMetaKey>(restore_table_txn_store->db_id_str_,
            //                                                         restore_table_txn_store->table_id_str_,
            //                                                         segment_info.segment_id_,
            //                                                         block_id,
            //                                                         column_def));
            //             }
            //     }

            // }

            // for (const auto &index_cmd : restore_table_txn_store->index_cmds_) {

            //     for (const auto &segment_index_info : index_cmd.segment_index_infos_) {
            //         metas.emplace_back(MakeUnique<SegmentIndexMetaKey>(restore_table_txn_store->db_id_str_,
            //                                                            restore_table_txn_store->table_id_str_,
            //                                                            index_cmd.index_id_,
            //                                                            segment_index_info.segment_id_));
            //         for (const auto &chunk_info : segment_index_info.chunk_infos_) {
            //         metas.emplace_back(MakeUnique<ChunkIndexMetaKey>(restore_table_txn_store->db_id_str_,
            //                                                             restore_table_txn_store->table_id_str_,
            //                                                             index_cmd.index_id_,
            //                                                             segment_index_info.segment_id_,
            //                                                             chunk_info.chunk_id_));
            //         }
            //     }
            // }

            // Status cleanup_status = CleanupInner(std::move(metas));
            // if (!cleanup_status.ok()) {
            //     LOG_WARN(fmt::format("Failed to cleanup metadata during restore table rollback: {}", cleanup_status.message()));
            // }

            break;
        }
        case TransactionType::kRestoreDatabase: {
            Config *config = InfinityContext::instance().config();
            String data_dir = config->DataDir();
            RestoreDatabaseTxnStore *restore_database_txn_store = static_cast<RestoreDatabaseTxnStore *>(base_txn_store_.get());
            // Remove the database directory
            String db_dir = fmt::format("{}/db_{}", data_dir, restore_database_txn_store->db_id_str_);

            if (VirtualStore::Exists(db_dir)) {
                Status remove_status = VirtualStore::RemoveDirectory(db_dir);
                if (!remove_status.ok()) {
                    LOG_WARN(fmt::format("Failed to remove database directory during rollback: {}", db_dir));
                }
            }
            break;
        }
        case TransactionType::kRestoreSystem: {
            Config *config = InfinityContext::instance().config();
            String data_dir = config->DataDir();
            RestoreSystemTxnStore *restore_system_txn_store = static_cast<RestoreSystemTxnStore *>(base_txn_store_.get());
            for (const auto &restore_db_txn_store : restore_system_txn_store->restore_db_txn_stores_) {
                String db_dir = fmt::format("{}/db_{}", data_dir, restore_db_txn_store->db_id_str_);

                if (VirtualStore::Exists(db_dir)) {
                    Status remove_status = VirtualStore::RemoveDirectory(db_dir);
                    if (!remove_status.ok()) {
                        LOG_WARN(fmt::format("Failed to remove database directory during rollback: {}", db_dir));
                    }
                }
            }
            break;
        }
        case TransactionType::kCreateSnapshot: {
            break;
        }
        default: {
        }
    }

    Status status = kv_instance_->Rollback();
    if (!status.ok()) {
        return status;
    }

    // TODO: due to dead lock, ignore the conflict txn.
    //    if (conflicted_txn_ != nullptr) {
    //        // Wait for dependent transaction finished
    //        conflicted_txn_->WaitForCompletion();
    //    }

    // To avoid the txn is hold by other object and the data in base_txn_store can't be released.
    if (base_txn_store_ != nullptr) {
        base_txn_store_->ClearData();
    }

    SetCompletion();

    return Status::OK();
}

Status NewTxn::Rollback() {
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

    LOG_TRACE(fmt::format("NewTxn: {} is rolled back.", txn_context_ptr_->txn_id_));

    return status;
}

Status NewTxn::Cleanup() {
    TxnTimeStamp last_cleanup_ts = new_catalog_->GetLastCleanupTS();
    TxnTimeStamp oldest_txn_begin_ts = txn_mgr_->GetOldestAliveTS();
    TxnTimeStamp last_checkpoint_ts = InfinityContext::instance().storage()->wal_manager()->LastCheckpointTS();

    if (last_cleanup_ts >= oldest_txn_begin_ts) {
        LOG_TRACE("SKIP cleanup");
        return Status::OK();
    }

    KVInstance *kv_instance = kv_instance_.get();
    TxnTimeStamp begin_ts = BeginTS();
    TxnTimeStamp visible_ts = std::min(begin_ts, last_checkpoint_ts);

    LOG_INFO(fmt::format("Cleaning ts < {} dropped entities...", visible_ts));

    Vector<String> dropped_keys;
    Vector<UniquePtr<MetaKey>> metas;
    Status status = new_catalog_->GetCleanedMeta(visible_ts, kv_instance, metas, dropped_keys);
    if (!status.ok()) {
        return status;
    }

    for (auto &key : dropped_keys) {
        status = kv_instance->Delete(key);
        if (!status.ok()) {
            return status;
        }
    }

    if (metas.empty()) {
        LOG_TRACE("Cleanup: No data need to clean. Try to remove all empty directories...");
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        auto data_dir_str = buffer_mgr->GetFullDataDir();
        auto data_dir = static_cast<Path>(*data_dir_str);
        // Delete empty dir
        VirtualStore::RecursiveCleanupAllEmptyDir(data_dir);
        return Status::OK();
    }

    status = CleanupInner(std::move(metas));
    if (!status.ok()) {
        return status;
    }

    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = MakeShared<CleanupTxnStore>();
    CleanupTxnStore *txn_store = static_cast<CleanupTxnStore *>(base_txn_store_.get());
    txn_store->timestamp_ = begin_ts;

    return Status::OK();
}

Status NewTxn::ReplayCleanup(WalCmdCleanup *cleanup_cmd, TxnTimeStamp commit_ts, i64 txn_id) { return Status::OK(); }

Status NewTxn::CleanupInner(const Vector<UniquePtr<MetaKey>> &metas) {
    KVInstance *kv_instance = kv_instance_.get();
    TxnTimeStamp begin_ts = BeginTS();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    for (auto &meta : metas) {
        switch (meta->type_) {
            case MetaType::kDB: {
                auto *db_meta_key = static_cast<DBMetaKey *>(meta.get());
                String db_key = KeyEncode::CatalogDbKey(db_meta_key->db_name_, db_meta_key->commit_ts_);
                Status status = kv_instance->Delete(db_key);
                if (!status.ok()) {
                    return status;
                }

                DBMeeta db_meta(db_meta_key->db_id_str_, kv_instance);
                status = NewCatalog::CleanDB(db_meta, begin_ts, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kTable: {
                auto *table_meta_key = static_cast<TableMetaKey *>(meta.get());
                String table_key = KeyEncode::CatalogTableKey(table_meta_key->db_id_str_, table_meta_key->table_name_, table_meta_key->commit_ts_);
                Status status = kv_instance->Delete(table_key);
                if (!status.ok()) {
                    return status;
                }
                TableMeeta table_meta(table_meta_key->db_id_str_, table_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                status = NewCatalog::CleanTable(table_meta, begin_ts, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kTableName: {
                auto *table_meta_key = static_cast<TableMetaKey *>(meta.get());
                String table_key = KeyEncode::CatalogTableKey(table_meta_key->db_id_str_, table_meta_key->table_name_, table_meta_key->commit_ts_);
                Status status = kv_instance->Delete(table_key);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kSegment: {
                auto *segment_meta_key = static_cast<SegmentMetaKey *>(meta.get());
                TableMeeta table_meta(segment_meta_key->db_id_str_, segment_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                Status status = table_meta.RemoveSegmentIDs1({segment_meta_key->segment_id_});
                if (!status.ok()) {
                    return status;
                }

                SegmentMeta segment_meta(segment_meta_key->segment_id_, table_meta);
                status = NewCatalog::CleanSegment(segment_meta, begin_ts, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kBlock: {
                auto *block_meta_key = static_cast<BlockMetaKey *>(meta.get());
                TableMeeta table_meta(block_meta_key->db_id_str_, block_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                SegmentMeta segment_meta(block_meta_key->segment_id_, table_meta);
                BlockMeta block_meta(block_meta_key->block_id_, segment_meta);
                Status status = NewCatalog::CleanBlock(block_meta, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kTableColumn: {
                auto *table_column_meta_key = static_cast<TableColumnMetaKey *>(meta.get());
                String column_key = KeyEncode::TableColumnKey(table_column_meta_key->db_id_str_,
                                                              table_column_meta_key->table_id_str_,
                                                              table_column_meta_key->column_name_,
                                                              table_column_meta_key->commit_ts_);
                Status status = kv_instance->Delete(column_key);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kBlockColumn: {
                auto *column_meta_key = static_cast<ColumnMetaKey *>(meta.get());
                TableMeeta table_meta(column_meta_key->db_id_str_, column_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                SegmentMeta segment_meta(column_meta_key->segment_id_, table_meta);
                BlockMeta block_meta(column_meta_key->block_id_, segment_meta);
                ColumnMeta column_meta(column_meta_key->column_def_->id(), block_meta);
                Status status = NewCatalog::CleanBlockColumn(column_meta, column_meta_key->column_def_.get(), UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kTableIndex: {
                auto *table_index_meta_key = static_cast<TableIndexMetaKey *>(meta.get());
                String index_key = KeyEncode::CatalogIndexKey(table_index_meta_key->db_id_str_,
                                                              table_index_meta_key->table_id_str_,
                                                              table_index_meta_key->index_name_,
                                                              table_index_meta_key->commit_ts_);
                Status status = kv_instance->Delete(index_key);
                if (!status.ok()) {
                    return status;
                }

                TableMeeta table_meta(table_index_meta_key->db_id_str_, table_index_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                TableIndexMeeta table_index_meta(table_index_meta_key->index_id_str_, table_meta);
                status = NewCatalog::CleanTableIndex(table_index_meta, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kSegmentIndex: {
                auto *segment_index_meta_key = static_cast<SegmentIndexMetaKey *>(meta.get());
                TableMeeta table_meta(segment_index_meta_key->db_id_str_,
                                      segment_index_meta_key->table_id_str_,
                                      kv_instance,
                                      begin_ts,
                                      MAX_TIMESTAMP);
                TableIndexMeeta table_index_meta(segment_index_meta_key->index_id_str_, table_meta);
                SegmentIndexMeta segment_index_meta(segment_index_meta_key->segment_id_, table_index_meta);
                Status status = NewCatalog::CleanSegmentIndex(segment_index_meta, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case MetaType::kChunkIndex: {
                auto *chunk_index_meta_key = static_cast<ChunkIndexMetaKey *>(meta.get());
                TableMeeta table_meta(chunk_index_meta_key->db_id_str_, chunk_index_meta_key->table_id_str_, kv_instance, begin_ts, MAX_TIMESTAMP);
                TableIndexMeeta table_index_meta(chunk_index_meta_key->index_id_str_, table_meta);
                SegmentIndexMeta segment_index_meta(chunk_index_meta_key->segment_id_, table_index_meta);
                ChunkIndexMeta chunk_index_meta(chunk_index_meta_key->chunk_id_, segment_index_meta);
                Status status = NewCatalog::CleanChunkIndex(chunk_index_meta, UsageFlag::kOther);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            default: {
                UnrecoverableError("Unexpected");
            }
        }
    }

    Status status = buffer_mgr->RemoveClean(kv_instance_.get());

    auto data_dir_str = buffer_mgr->GetFullDataDir();
    auto data_dir = static_cast<Path>(*data_dir_str);
    // Delete empty dir
    VirtualStore::RecursiveCleanupAllEmptyDir(data_dir);

    return status;
}

bool NewTxn::IsReplay() const { return txn_context_ptr_->txn_type_ == TransactionType::kReplay; }

Status NewTxn::ReplayWalCmd(const SharedPtr<WalCmd> &command, TxnTimeStamp commit_ts, i64 txn_id) {
    WalCommandType command_type = command->GetType();
    switch (command_type) {
        case WalCommandType::DUMMY: {
            return Status::UnexpectedError("Dummy WAL command");
        }
        case WalCommandType::CREATE_DATABASE_V2: {
            auto *create_db_cmd = static_cast<WalCmdCreateDatabaseV2 *>(command.get());
            Status status = ReplayCreateDb(create_db_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::DROP_DATABASE_V2: {
            auto *drop_db_cmd = static_cast<WalCmdDropDatabaseV2 *>(command.get());
            Status status = ReplayDropDb(drop_db_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::CREATE_TABLE_V2: {
            auto *create_table_cmd = static_cast<WalCmdCreateTableV2 *>(command.get());

            Status status = ReplayCreateTable(create_table_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::DROP_TABLE_V2: {
            auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(command.get());

            Status status = ReplayDropTable(drop_table_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::RENAME_TABLE_V2: {
            auto *rename_table_cmd = static_cast<WalCmdRenameTableV2 *>(command.get());
            Status status = ReplayRenameTable(rename_table_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::ADD_COLUMNS_V2: {
            auto *add_column_cmd = static_cast<WalCmdAddColumnsV2 *>(command.get());
            Status status = PrepareCommitAddColumns(add_column_cmd);
            // Status status = ReplayAddColumns(add_column_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::DROP_COLUMNS_V2: {
            auto *drop_column_cmd = static_cast<WalCmdDropColumnsV2 *>(command.get());
            Status status = PrepareCommitDropColumns(drop_column_cmd);
            // Status status = ReplayDropColumns(drop_column_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::CREATE_INDEX_V2: {
            auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(command.get());
            Status status = ReplayCreateIndex(create_index_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::DROP_INDEX_V2: {
            auto *drop_index_cmd = static_cast<WalCmdDropIndexV2 *>(command.get());
            Status status = ReplayDropIndex(drop_index_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
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
            if (append_cmd->db_id_ != db_meta->db_id_str() || append_cmd->table_id_ != table_meta->table_id_str()) {
                return Status::CatalogError(fmt::format("WalCmdAppendV2 db_id or table_id ({}, {}) mismatch with the expected value ({}, {})",
                                                        append_cmd->db_id_,
                                                        append_cmd->table_id_,
                                                        db_meta->db_id_str(),
                                                        table_meta->table_id_str()));
            }
            break;
        }
        case WalCommandType::DELETE_V2: {
            auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(command.get());

            Status status = ReplayDelete(delete_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::IMPORT_V2: {
            auto *import_cmd = static_cast<WalCmdImportV2 *>(command.get());

            Status status = ReplayImport(import_cmd, commit_ts, txn_id);
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

            Status status = ReplayCompact(compact_cmd, commit_ts, txn_id);
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
        case WalCommandType::CHECKPOINT_V2: {
            auto *checkpoint_cmd = static_cast<WalCmdCheckpointV2 *>(command.get());
            Status status = PrepareCommitCheckpoint(checkpoint_cmd);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::RESTORE_TABLE_SNAPSHOT: {
            auto *restore_table_cmd = static_cast<WalCmdRestoreTableSnapshot *>(command.get());

            Status status = ReplayRestoreTableSnapshot(restore_table_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::RESTORE_DATABASE_SNAPSHOT: {
            auto *restore_database_cmd = static_cast<WalCmdRestoreDatabaseSnapshot *>(command.get());
            Status status = ReplayRestoreDatabaseSnapshot(restore_database_cmd, commit_ts, txn_id);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case WalCommandType::RESTORE_SYSTEM_SNAPSHOT: {
            auto *restore_system_cmd = static_cast<WalCmdRestoreSystemSnapshot *>(command.get());
            Status status = this->ReplayRestoreSystemSnapshot(restore_system_cmd, commit_ts, txn_id);
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
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    return NewCatalog::GetDBFilePaths(begin_ts, commit_ts, *db_meta, file_paths);
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
        auto [column_defs, col_status] = table_meta->GetColumnDefs();
        if (!col_status.ok()) {
            return col_status;
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

Status NewTxn::Dummy() {
    base_txn_store_ = MakeShared<DummyTxnStore>();
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

String NewTxn::GetTableIdStr() {
    for (auto &command : wal_entry_->cmds_) {
        WalCommandType command_type = command->GetType();
        switch (command_type) {
            case WalCommandType::CREATE_TABLE_V2: {
                auto *create_table_cmd = static_cast<WalCmdCreateTableV2 *>(command.get());
                return create_table_cmd->table_id_;
            }
            case WalCommandType::DROP_TABLE_V2: {
                auto *drop_table_cmd = static_cast<WalCmdDropTableV2 *>(command.get());
                return drop_table_cmd->table_id_;
            }
            case WalCommandType::RENAME_TABLE_V2: {
                auto *rename_table_cmd = static_cast<WalCmdRenameTableV2 *>(command.get());
                return rename_table_cmd->table_id_;
            }
            case WalCommandType::ADD_COLUMNS_V2: {
                auto *add_column_cmd = static_cast<WalCmdAddColumnsV2 *>(command.get());
                return add_column_cmd->table_id_;
            }
            case WalCommandType::DROP_COLUMNS_V2: {
                auto *drop_column_cmd = static_cast<WalCmdDropColumnsV2 *>(command.get());
                return drop_column_cmd->table_id_;
            }
            case WalCommandType::CREATE_INDEX_V2: {
                auto *create_index_cmd = static_cast<WalCmdCreateIndexV2 *>(command.get());
                return create_index_cmd->table_id_;
            }
            case WalCommandType::DROP_INDEX_V2: {
                auto *drop_index_cmd = static_cast<WalCmdDropIndexV2 *>(command.get());
                return drop_index_cmd->table_id_;
            }
            case WalCommandType::DUMP_INDEX_V2: {
                auto *dump_index_cmd = static_cast<WalCmdDumpIndexV2 *>(command.get());
                return dump_index_cmd->table_id_;
            }
            case WalCommandType::APPEND_V2: {
                auto *append_cmd = static_cast<WalCmdAppendV2 *>(command.get());
                return append_cmd->table_id_;
            }
            case WalCommandType::DELETE_V2: {
                auto *delete_cmd = static_cast<WalCmdDeleteV2 *>(command.get());
                return delete_cmd->table_id_;
            }
            case WalCommandType::IMPORT_V2: {
                auto *import_cmd = static_cast<WalCmdImportV2 *>(command.get());
                return import_cmd->table_id_;
            }
            case WalCommandType::COMPACT_V2: {
                auto *compact_cmd = static_cast<WalCmdCompactV2 *>(command.get());
                return compact_cmd->table_id_;
            }
            case WalCommandType::OPTIMIZE_V2: {
                auto *optimize_cmd = static_cast<WalCmdOptimizeV2 *>(command.get());
                return optimize_cmd->table_id_;
            }
            default: {
                break;
            }
        }
    }
    return "";
}

void NewTxn::AddSemaphore(UniquePtr<std::binary_semaphore> sema) { semas_.push_back(std::move(sema)); }

const Vector<UniquePtr<std::binary_semaphore>> &NewTxn::semas() const { return semas_; }

Status NewTxn::ProcessSnapshotRestorationData(const String &db_name,
                                              const String &db_id_str,
                                              const String &table_name,
                                              const String &table_id_str,
                                              const SharedPtr<TableDef> &table_def,
                                              const SharedPtr<TableSnapshotInfo> &table_snapshot_info,
                                              const String &snapshot_name,
                                              RestoreTableTxnStore *txn_store) {
    // Set basic transaction store fields
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = db_id_str;
    txn_store->db_id_ = std::stoull(db_id_str);
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_id_str;
    txn_store->table_id_ = std::stoull(table_id_str);
    txn_store->table_def_ = table_def;
    txn_store->snapshot_name_ = snapshot_name;

    // Process segment information
    for (const auto &segment_info : table_snapshot_info->segment_snapshots_) {
        WalSegmentInfoV2 wal_segment_info{};
        wal_segment_info.segment_id_ = segment_info.second->segment_id_;
        for (const auto &block_info : segment_info.second->block_snapshots_) {
            wal_segment_info.block_ids_.push_back(block_info->block_id_);
        }
        txn_store->segment_infos_.push_back(wal_segment_info);
    }

    // Process index information
    for (const auto &index_snapshot : table_snapshot_info->table_index_snapshots_) {
        String table_key = KeyEncode::CatalogTableKey(txn_store->db_id_str_, txn_store->table_name_, this->CommitTS());
        WalCmdCreateIndexV2 wal_index_cmd{txn_store->db_name_,
                                          txn_store->db_id_str_,
                                          txn_store->table_name_,
                                          txn_store->table_id_str_,
                                          *index_snapshot.second->index_id_str_,
                                          index_snapshot.second->index_base_,
                                          table_key};
        for (const auto &segment_index : index_snapshot.second->segment_index_snapshots_) {
            WalSegmentIndexInfo wal_segment_index_info{};
            wal_segment_index_info.segment_id_ = segment_index->segment_id_;
            for (const auto &chunk_index : segment_index->chunk_index_snapshots_) {
                WalChunkIndexInfo wal_chunk_index_info{};
                wal_chunk_index_info.chunk_id_ = chunk_index->chunk_id_;
                wal_chunk_index_info.base_name_ = chunk_index->base_name_;
                wal_chunk_index_info.base_rowid_ = chunk_index->base_row_id_;
                wal_chunk_index_info.row_count_ = chunk_index->row_cnt_;
                wal_chunk_index_info.index_size_ = chunk_index->index_size_;
                wal_segment_index_info.chunk_infos_.push_back(wal_chunk_index_info);
            }
            wal_index_cmd.segment_index_infos_.push_back(wal_segment_index_info);
        }
        txn_store->index_cmds_.push_back(wal_index_cmd);
    }
    return Status::OK();
}

Status NewTxn::ReplayRestoreTableSnapshot(WalCmdRestoreTableSnapshot *restore_table_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    const String &db_name = restore_table_cmd->db_name_;

    // Check if the table already exists
    String table_key = KeyEncode::CatalogTableKey(restore_table_cmd->db_id_, *restore_table_cmd->table_def_->table_name(), commit_ts);
    String table_id;
    Status status = kv_instance_->Get(table_key, table_id);
    bool is_link_files = false;
    if (status.ok()) {
        if (table_id == restore_table_cmd->table_id_) {

            LOG_WARN(fmt::format("Skipping replay restore table: Table {} with id {} already exists, commit ts: {}, txn: {}.",
                                 *restore_table_cmd->table_def_->table_name(),
                                 restore_table_cmd->table_id_,
                                 commit_ts,
                                 txn_id));
            is_link_files = true;
        } else {
            LOG_ERROR(fmt::format("Replay restore table: Table {} with id {} already exists with different id {}, commit ts: {}, txn: {}.",
                                  *restore_table_cmd->table_def_->table_name(),
                                  restore_table_cmd->table_id_,
                                  table_id,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Table ID mismatch during replay of table restore.");
        }
    }

    // Check persistence manager state during restore replay
    PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();
    if (persistence_manager != nullptr) {
        restore_table_cmd->addr_serializer_.AddToPersistenceManager(persistence_manager);
        HashMap<String, ObjAddr> all_files = persistence_manager->GetAllFiles();
        LOG_DEBUG(fmt::format("Persistence manager has {} registered files during restore replay, commit ts: {}, txn: {}",
                              all_files.size(),
                              commit_ts,
                              txn_id));

        // Check if any files from this table are registered
        String table_prefix = "db_" + restore_table_cmd->db_id_ + "/tbl_" + restore_table_cmd->table_id_;
        SizeT table_file_count = 0;
        for (const auto &[file_path, obj_addr] : all_files) {
            if (file_path.find(table_prefix) != String::npos) {
                table_file_count++;
                LOG_DEBUG(fmt::format("Found registered table file: {} -> obj_addr: ({}, {}, {})",
                                      file_path,
                                      obj_addr.obj_key_,
                                      obj_addr.part_offset_,
                                      obj_addr.part_size_));
            }
        }
        LOG_DEBUG(fmt::format("Table {} has {} files registered in persistence manager", restore_table_cmd->table_id_, table_file_count));
    } else { // check if the data still exist in the system
        String data_dir = InfinityContext::instance().config()->DataDir();
        String table_data_dir = VirtualStore::ConcatenatePath(VirtualStore::ConcatenatePath(data_dir, "db_" + restore_table_cmd->db_id_),
                                                              "tbl_" + restore_table_cmd->table_id_);
        if (!VirtualStore::Exists(table_data_dir)) {
            LOG_ERROR(fmt::format("Table data directory {} does not exist, commit ts: {}, txn: {}.", table_data_dir, commit_ts, txn_id));
            // return Status::OK();
        }
    }

    // if exist proceed
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }

    // Get next table id of the db
    String next_table_id_key = KeyEncode::CatalogDbTagKey(restore_table_cmd->db_id_, NEXT_TABLE_ID.data());
    String next_table_id_str;
    status = kv_instance_->Get(next_table_id_key, next_table_id_str);
    if (!status.ok()) {
        return status;
    }
    u64 next_table_id = std::stoull(next_table_id_str);
    u64 this_table_id = std::stoull(restore_table_cmd->table_id_);
    if (this_table_id + 1 > next_table_id) {
        // Update the next table id
        String new_next_table_id_str = std::to_string(this_table_id + 1);
        status = kv_instance_->Put(next_table_id_key, new_next_table_id_str);
        if (!status.ok()) {
            return status;
        }
        LOG_TRACE(fmt::format("Update next table id to {} for database {}.", new_next_table_id_str, restore_table_cmd->db_name_));
    }

    status = PrepareCommitRestoreTableSnapshot(restore_table_cmd, is_link_files);

    if (!status.ok()) {
        return status;
    }

    // String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    // String snapshot_name = restore_table_cmd->snapshot_name_;
    // SharedPtr<TableSnapshotInfo> table_snapshot;
    // std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, snapshot_name);
    // if (!status.ok()) {
    //     return status;
    // }

    // Vector<String> files_to_restore = table_snapshot->GetFiles();
    // status = table_snapshot->RestoreSnapshotFiles(snapshot_dir, snapshot_name, files_to_restore, restore_table_cmd->table_id_);
    // if (!status.ok()) {
    //     return status;
    // }

    return Status::OK();
}

Status NewTxn::ReplayRestoreDatabaseSnapshot(WalCmdRestoreDatabaseSnapshot *restore_database_cmd, TxnTimeStamp commit_ts, i64 txn_id) {

    // Check if the database already exists
    String db_key = KeyEncode::CatalogDbKey(restore_database_cmd->db_id_str_, commit_ts);
    String db_id;
    Status status = kv_instance_->Get(db_key, db_id);
    if (status.ok()) {
        if (db_id == restore_database_cmd->db_id_str_) {
            LOG_WARN(fmt::format("Skipping replay restore database: Database {} with id {} already exists, commit ts: {}, txn: {}.",
                                 restore_database_cmd->db_name_,
                                 restore_database_cmd->db_id_str_,
                                 commit_ts,
                                 txn_id));
        } else {
            LOG_ERROR(fmt::format("Replay restore database: Database {} with id {} already exists with different id {}, commit ts: {}, txn: {}.",
                                  restore_database_cmd->db_name_,
                                  restore_database_cmd->db_id_str_,
                                  db_id,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Database ID mismatch during replay of database restore.");
        }
    }

    for (const auto &restore_table_cmd : restore_database_cmd->restore_table_wal_cmds_) {
        // Check persistence manager state during restore replay
        PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();
        if (persistence_manager != nullptr) {
            restore_table_cmd.addr_serializer_.AddToPersistenceManager(persistence_manager);
            HashMap<String, ObjAddr> all_files = persistence_manager->GetAllFiles();
            LOG_DEBUG(fmt::format("Persistence manager has {} registered files during restore replay, commit ts: {}, txn: {}",
                                  all_files.size(),
                                  commit_ts,
                                  txn_id));

            // Check if any files from this table are registered
            String table_prefix = "db_" + restore_table_cmd.db_id_ + "/tbl_" + restore_table_cmd.table_id_;
            SizeT table_file_count = 0;
            for (const auto &[file_path, obj_addr] : all_files) {
                if (file_path.find(table_prefix) != String::npos) {
                    table_file_count++;
                    LOG_DEBUG(fmt::format("Found registered table file: {} -> obj_addr: ({}, {}, {})",
                                          file_path,
                                          obj_addr.obj_key_,
                                          obj_addr.part_offset_,
                                          obj_addr.part_size_));
                }
            }
            LOG_DEBUG(fmt::format("Table {} has {} files registered in persistence manager", restore_table_cmd.table_id_, table_file_count));
        } else { // check if the data still exist in the system
            String data_dir = InfinityContext::instance().config()->DataDir();
            String table_data_dir = VirtualStore::ConcatenatePath(VirtualStore::ConcatenatePath(data_dir, "db_" + restore_table_cmd.db_id_),
                                                                  "tbl_" + restore_table_cmd.table_id_);
            if (!VirtualStore::Exists(table_data_dir)) {
                LOG_ERROR(fmt::format("Table data directory {} does not exist, commit ts: {}, txn: {}.", table_data_dir, commit_ts, txn_id));
                // return Status::OK();
            }
        }
    }

    status = PrepareCommitRestoreDatabaseSnapshot(restore_database_cmd);
    if (!status.ok()) {
        return status;
    }

    // Update next db id if necessary
    String current_next_db_id_str;
    status = kv_instance_->Get(NEXT_DATABASE_ID.data(), current_next_db_id_str);
    if (!status.ok()) {
        return status;
    }

    u64 current_next_db_id = std::stoull(current_next_db_id_str);
    u64 this_db_id = std::stoull(restore_database_cmd->db_id_str_);
    if (this_db_id + 1 > current_next_db_id) {
        // Update the next db id
        String next_db_id_str = std::to_string(this_db_id + 1);
        status = kv_instance_->Put(NEXT_DATABASE_ID.data(), next_db_id_str);
        if (!status.ok()) {
            return status;
        }
        LOG_TRACE(fmt::format("Update next db id to {}.", next_db_id_str));
    }

    return Status::OK();
}

Status NewTxn::ReplayRestoreSystemSnapshot(WalCmdRestoreSystemSnapshot *restore_system_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    for (const auto &drop_database_cmd : restore_system_cmd->drop_database_wal_cmds_) {
        Status status = this->ReplayDropDb(const_cast<WalCmdDropDatabaseV2 *>(&drop_database_cmd), commit_ts, txn_id);
        if (!status.ok()) {
            return status;
        }
    }

    for (const auto &restore_database_cmd : restore_system_cmd->restore_database_wal_cmds_) {
        Status status = this->ReplayRestoreDatabaseSnapshot(const_cast<WalCmdRestoreDatabaseSnapshot *>(&restore_database_cmd), commit_ts, txn_id);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::CommitBottomCreateTableSnapshot(const String &db_name, const String &table_name, const String &snapshot_name) {

    ManualDumpIndex(db_name, table_name);

    // create a new snapshot
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    table_meta->SetBeginTS(txn_context_ptr_->commit_ts_);

    SharedPtr<TableSnapshotInfo> table_snapshot_info;
    std::tie(table_snapshot_info, status) = table_meta->MapMetaToSnapShotInfo(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    table_snapshot_info->snapshot_name_ = snapshot_name;

    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    status = table_snapshot_info->Serialize(snapshot_dir, this->TxnID());
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitBottomCreateDatabaseSnapshot(const String &db_name, const String &snapshot_name) {

    // create a new snapshot
    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }
    // db_meta->SetBeginTS(txn_context_ptr_->commit_ts_);

    SharedPtr<DatabaseSnapshotInfo> database_snapshot_info;
    database_snapshot_info = MakeShared<DatabaseSnapshotInfo>();
    database_snapshot_info->db_name_ = db_name;
    database_snapshot_info->db_id_str_ = db_meta->db_id_str();
    database_snapshot_info->snapshot_name_ = snapshot_name;

    Vector<String> *table_ids_ptr = nullptr;
    Vector<String> *table_names_ptr = nullptr;
    status = db_meta->GetTableIDs(table_ids_ptr, &table_names_ptr);
    if (!status.ok()) {
        return status;
    }

    if (table_ids_ptr->size() > 0) {
        database_snapshot_info->db_next_table_id_str_ = std::to_string(std::stoull(table_ids_ptr->back()) + 1);
    } else {
        database_snapshot_info->db_next_table_id_str_ = std::to_string(0);
    }

    for (size_t i = 0; i < table_ids_ptr->size(); i++) {
        ManualDumpIndex(db_name, table_names_ptr->at(i));
        Optional<TableMeeta> table_meta;
        status = GetTableMeta(table_names_ptr->at(i), *db_meta, table_meta);
        if (!status.ok()) {
            return status;
        }
        SharedPtr<TableSnapshotInfo> table_snapshot_info;
        std::tie(table_snapshot_info, status) = table_meta->MapMetaToSnapShotInfo(db_name, table_names_ptr->at(i));
        if (!status.ok()) {
            return status;
        }
        database_snapshot_info->table_snapshots_.push_back(table_snapshot_info);
    }

    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    status = database_snapshot_info->Serialize(snapshot_dir, this->TxnID());
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitBottomCreateSystemSnapshot(const String &snapshot_name) {
    Vector<String> *db_id_strs_ptr;
    Vector<String> *db_names_ptr;
    CatalogMeta catalog_meta(this);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    SharedPtr<SystemSnapshotInfo> system_snapshot_info;
    system_snapshot_info = MakeShared<SystemSnapshotInfo>();
    system_snapshot_info->snapshot_name_ = snapshot_name;
    if (!status.ok()) {
        return status;
    }
    for (const String &db_name : *db_names_ptr) {
        // create a new snapshot
        Optional<DBMeeta> db_meta;
        TxnTimeStamp db_create_ts;
        Status status = GetDBMeta(db_name, db_meta, db_create_ts);
        if (!status.ok()) {
            return status;
        }
        // db_meta->SetBeginTS(txn_context_ptr_->commit_ts_);

        SharedPtr<DatabaseSnapshotInfo> database_snapshot_info;
        database_snapshot_info = MakeShared<DatabaseSnapshotInfo>();
        database_snapshot_info->db_name_ = db_name;
        database_snapshot_info->db_id_str_ = db_meta->db_id_str();

        Vector<String> *table_ids_ptr = nullptr;
        Vector<String> *table_names_ptr = nullptr;
        status = db_meta->GetTableIDs(table_ids_ptr, &table_names_ptr);
        if (!status.ok()) {
            return status;
        }

        if (table_ids_ptr->size() > 0) {
            database_snapshot_info->db_next_table_id_str_ = std::to_string(std::stoull(table_ids_ptr->back()) + 1);
        } else {
            database_snapshot_info->db_next_table_id_str_ = std::to_string(0);
        }

        for (size_t i = 0; i < table_ids_ptr->size(); i++) {
            ManualDumpIndex(db_name, table_names_ptr->at(i));
            Optional<TableMeeta> table_meta;
            status = GetTableMeta(table_names_ptr->at(i), *db_meta, table_meta);
            if (!status.ok()) {
                return status;
            }
            SharedPtr<TableSnapshotInfo> table_snapshot_info;
            std::tie(table_snapshot_info, status) = table_meta->MapMetaToSnapShotInfo(db_name, table_names_ptr->at(i));
            if (!status.ok()) {
                return status;
            }
            database_snapshot_info->table_snapshots_.push_back(table_snapshot_info);
        }
        system_snapshot_info->database_snapshots_.push_back(database_snapshot_info);
    }

    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    status = system_snapshot_info->Serialize(snapshot_dir, this->TxnID());
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CheckpointforSnapshot(TxnTimeStamp last_ckp_ts, CheckpointTxnStore *txn_store) {
    TransactionType txn_type = GetTxnType();
    if (txn_type != TransactionType::kNewCheckpoint && txn_type != TransactionType::kCreateSnapshot) {
        UnrecoverableError(fmt::format("Expected transaction type is checkpoint or create table snapshot."));
    }

    if (last_ckp_ts % 2 == 0 and last_ckp_ts > 0) {
        UnrecoverableError(fmt::format("last checkpoint ts isn't correct: {}", last_ckp_ts));
    }

    Status status;
    TxnTimeStamp checkpoint_ts = txn_context_ptr_->begin_ts_;
    CheckpointOption option{checkpoint_ts};

    current_ckp_ts_ = checkpoint_ts;
    LOG_INFO(fmt::format("checkpoint ts: {}", current_ckp_ts_));

    if (last_ckp_ts > 0 and last_ckp_ts + 2 >= checkpoint_ts) {
        // last checkpoint ts: last checkpoint txn begin ts. checkpoint is the begin_ts of current txn
        txn_type_ = TxnType::kReadOnly;
        LOG_INFO(fmt::format("Last checkpoint ts {}, this checkpoint begin ts: {}, SKIP CHECKPOINT", last_ckp_ts, checkpoint_ts));
        return Status::OK();
    }

    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    if (!wal_manager->SetCheckpointing()) {
        return Status::Checkpointing();
    }
    DeferFn defer([&] { wal_manager->UnsetCheckpoint(); });

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(this);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, this);
        status = this->CheckpointDB(db_meta, option, txn_store);
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

    return Status::OK();
}

} // namespace infinity
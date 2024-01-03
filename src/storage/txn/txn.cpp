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
#include <vector>

import stl;

import infinity_exception;

import txn_manager;
import wal_entry;
import third_party;
import logger;
import data_block;
import txn_store;
import txn_state;
import parser;
import meta_state;
import buffer_manager;
import data_access_state;

import table_detail;
import table_entry_type;
import catalog;
import database_detail;
import status;
import table_def;
import index_def;

module txn;

namespace infinity {

Txn::Txn(TxnManager *txn_mgr, NewCatalog *catalog, u32 txn_id)
    : txn_mgr_(txn_mgr), catalog_(catalog), txn_id_(txn_id), wal_entry_(MakeShared<WalEntry>()) {}

Tuple<TableEntry*, Status> Txn::GetTableEntry(const String &db_name, const String &table_name) {
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else {
        if (!IsEqual(db_name_, db_name)) {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Attempt to get table {} from another database {}", db_name, table_name));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
        }
    }

    auto table_iter = txn_table_entries_.find(table_name);
    if (table_iter == txn_table_entries_.end()) {
        // not found the table in cached entries
        auto [table_entry, status] = this->GetTableByName(db_name, table_name);
        if (!status.ok()) {
            return {nullptr, status};
        }

        txn_table_entries_[table_name] = reinterpret_cast<BaseEntry *>(table_entry);
        return {table_entry, status};
    } else {
        return {(TableEntry *)table_iter->second, Status::OK()};
    }
}

Status Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    auto [table_entry, status] = GetTableEntry(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    wal_entry_->cmds.push_back(MakeShared<WalCmdAppend>(db_name, table_name, input_block));
    UniquePtr<String> err_msg = table_store->Append(input_block);
    if (err_msg.get() != nullptr) {
        return Status(ErrorCode::kError, Move(err_msg));
    }
    return Status::OK();
}

Status Txn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids) {
    auto [table_entry, status] = GetTableEntry(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    wal_entry_->cmds.push_back(MakeShared<WalCmdDelete>(db_name, table_name, row_ids));
    UniquePtr<String> err_msg = table_store->Delete(row_ids);
    if (err_msg.get() != nullptr) {
        return Status(ErrorCode::kError, Move(err_msg));
    }
    return Status::OK();
}

TxnTableStore *Txn::GetTxnTableStore(TableEntry *table_entry) {
    UniqueLock<Mutex> lk(lock_);
    auto txn_table_iter = txn_tables_store_.find(*table_entry->GetTableName());
    if (txn_table_iter != txn_tables_store_.end()) {
        return txn_table_iter->second.get();
    }
    txn_tables_store_[*table_entry->GetTableName()] = MakeUnique<TxnTableStore>(table_entry, this);
    TxnTableStore *txn_table_store = txn_tables_store_[*table_entry->GetTableName()].get();
    return txn_table_store;
}

BufferManager *Txn::GetBufferMgr() const { return this->txn_mgr_->GetBufferMgr(); }

Status Txn::CreateDatabase(const String &db_name, ConflictType conflict_type) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [db_entry, status] = catalog_->CreateDatabase(db_name, this->txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (!status.ok()) {
        return status;
    }

    txn_dbs_.insert(db_entry);
    db_names_.insert(db_name);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateDatabase>(db_name));
    return Status::OK();
}

Status Txn::DropDatabase(const String &db_name, ConflictType) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [dropped_db_entry, status] = catalog_->DropDatabase(db_name, txn_id_, begin_ts, txn_mgr_);
    if (!status.ok()) {
        return status;
    }

    if (txn_dbs_.contains(dropped_db_entry)) {
        txn_dbs_.erase(dropped_db_entry);
    } else {
        txn_dbs_.insert(dropped_db_entry);
    }

    if (db_names_.contains(db_name)) {
        db_names_.erase(db_name);
    } else {
        db_names_.insert(db_name);
    }
    wal_entry_->cmds.push_back(MakeShared<WalCmdDropDatabase>(db_name));
    return Status::OK();
}

Tuple<DBEntry *, Status> Txn::GetDatabase(const String &db_name) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetDatabase(db_name, this->txn_id_, begin_ts);
}

Vector<DatabaseDetail> Txn::ListDatabases() {
    Vector<DatabaseDetail> res;

    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id_, txn_context_.GetBeginTS());
    SizeT db_count = db_entries.size();
    for (SizeT idx = 0; idx < db_count; ++idx) {
        DBEntry *db_entry = db_entries[idx];
        res.emplace_back(DatabaseDetail{db_entry->db_name_ptr()});
    }

    return res;
}

Status Txn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetTables(db_name, output_table_array, txn_id_, begin_ts);
}

Status Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_status] = catalog_->CreateTable(db_name, txn_id_, begin_ts, table_def, conflict_type, txn_mgr_);

    if (table_entry == nullptr) {
        if (table_status.ok()) {
            UniquePtr<String> err_msg = MakeUnique<String>("TODO: CreateTableCollectionFailed");
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kError, Move(err_msg));
        } else {
            return table_status;
        }
    }

    txn_tables_.insert(table_entry);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateTable>(db_name, table_def));
    return Status::OK();
}

Status Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_status] = catalog_->DropTableByName(db_name, table_name, conflict_type, txn_id_, begin_ts, txn_mgr_);

    if (table_entry == nullptr) {
        return table_status;
    }

    if (txn_tables_.contains(table_entry)) {
        txn_tables_.erase(table_entry);
    } else {
        txn_tables_.insert(table_entry);
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropTable>(db_name, table_name));
    return Status::OK();
}

Status Txn::CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexDef> &index_def, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction is not started");
    }
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_index_entry, index_status] = catalog_->CreateIndex(db_name, table_name, index_def, conflict_type, txn_id_, begin_ts, txn_mgr_);
    if (!index_status.ok()) {
        return index_status;
    }

    if (index_status.ok() && table_index_entry == nullptr && conflict_type == ConflictType::kIgnore) {
        return index_status;
    }

    txn_indexes_.emplace(*index_def->index_name_, table_index_entry);

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    // Create Index Synchronously
    NewCatalog::CreateIndexFile(table_entry, table_store, table_index_entry, begin_ts, GetBufferMgr());

    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateIndex>(db_name, table_name, index_def));
    return index_status;
}

Status Txn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();
    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction is not started");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_index_entry, index_status] = catalog_->DropIndex(db_name, table_name, index_name, conflict_type, txn_id_, begin_ts, txn_mgr_);
    if (!index_status.ok()) {
        return index_status;
    }

    if (index_status.ok() && table_index_entry == nullptr && conflict_type == ConflictType::kIgnore) {
        return index_status;
    }

    if (auto iter = txn_indexes_.find(index_name); iter != txn_indexes_.end()) {
        txn_indexes_.erase(iter);
    } else {
        txn_indexes_.emplace(index_name, table_index_entry);
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropIndex>(db_name, table_name, index_name));
    return index_status;
}

Tuple<TableEntry *, Status> Txn::GetTableByName(const String &db_name, const String &table_name) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetTableByName(db_name, table_name, txn_id_, begin_ts);
}

Status Txn::CreateCollection(const String &, const String &, ConflictType, BaseEntry *&) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

Status Txn::GetCollectionByName(const String &, const String &, BaseEntry *&) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

Status Txn::CreateView(const String &, const String &, ConflictType, BaseEntry *&) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

Status Txn::DropViewByName(const String &, const String &, ConflictType, BaseEntry *&) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

Status Txn::GetViewByName(const String &, const String &, BaseEntry *&) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

Status Txn::GetViews(const String &, Vector<ViewDetail> &output_view_array) {
    Error<TransactionException>("Not Implemented");
    return {ErrorCode::kNotImplemented, "Not Implemented"};
}

void Txn::Begin() { txn_context_.BeginCommit(txn_mgr_->GetTimestamp()); }

TxnTimeStamp Txn::Commit() {
    TxnTimeStamp commit_ts = txn_mgr_->GetTimestamp(true);
    txn_context_.SetTxnCommitting(commit_ts);
    // TODO: serializability validation. ASSUMES always valid for now.
    bool valid = true;
    if (!valid) {
        txn_mgr_->Invalidate(commit_ts);
        txn_context_.SetTxnRollbacked();
        return commit_ts;
    }

    if (wal_entry_->cmds.empty()) {
        // Don't need to write empty WalEntry (read-only transactions).
        txn_mgr_->Invalidate(commit_ts);
        txn_context_.SetTxnCommitted();
        return commit_ts;
    }
    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id = txn_id_;
    wal_entry_->commit_ts = commit_ts;
    txn_mgr_->PutWalEntry(wal_entry_);

    // Wait until CommitTxnBottom is done.
    UniqueLock<Mutex> lk(lock_);
    cond_var_.wait(lk, [this] { return done_bottom_; });
    return commit_ts;
}

void Txn::CommitBottom() {

    // prepare to commit txn local data into table
    for (const auto &name_table_pair : txn_tables_store_) {
        TxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->PrepareCommit();
    }

    txn_context_.SetTxnCommitted();

    // Commit the prepared data
    for (const auto &name_table_pair : txn_tables_store_) {
        TxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->Commit();
    }

    TxnTimeStamp commit_ts = txn_context_.GetCommitTS();

    // Commit databases to memory catalog
    for (auto *db_entry : txn_dbs_) {
        db_entry->Commit(commit_ts);
    }

    // Commit tables to memory catalog
    for (auto *table_entry : txn_tables_) {
        table_entry->Commit(commit_ts);
    }

    //  Commit indexes in catalog
    for (const auto &[index_name, table_index_entry] : txn_indexes_) {
        table_index_entry->Commit(commit_ts);
    }
    LOG_TRACE(Format("Txn: {} is committed.", txn_id_));

    // Notify the top half
    UniqueLock<Mutex> lk(lock_);
    done_bottom_ = true;
    cond_var_.notify_one();
}

void Txn::CancelCommitBottom() {
    txn_context_.SetTxnRollbacked();
    UniqueLock<Mutex> lk(lock_);
    done_bottom_ = true;
    cond_var_.notify_one();
}

// Dangerous! only used during replaying wal.
void Txn::FakeCommit(TxnTimeStamp commit_ts) {
    txn_context_.begin_ts_ = commit_ts;
    txn_context_.commit_ts_ = commit_ts;
    txn_context_.state_ = TxnState::kCommitted;
}

void Txn::Rollback() {
    TxnTimeStamp abort_ts = txn_mgr_->GetTimestamp();
    txn_context_.SetTxnRollbacking(abort_ts);

    for (const auto &base_entry : txn_tables_) {
        auto *table_entry = (TableEntry *)(base_entry);
        NewCatalog::RemoveTableEntry(table_entry, txn_id_, txn_mgr_);
    }

    for (const auto &[index_name, table_index_entry] : txn_indexes_) {
        NewCatalog::RemoveIndexEntry(index_name, table_index_entry, txn_id_, txn_mgr_);
    }

    for (const auto &db_name : db_names_) {
        catalog_->RemoveDBEntry(db_name, this->txn_id_, txn_mgr_);
    }

    // Rollback the prepared data
    for (const auto &name_table_pair : txn_tables_store_) {
        TxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->Rollback();
    }

    txn_context_.SetTxnRollbacked();

    LOG_TRACE(Format("Txn: {} is dropped.", txn_id_));
}

void Txn::AddWalCmd(const SharedPtr<WalCmd> &cmd) { wal_entry_->cmds.push_back(cmd); }

void Txn::Checkpoint(const TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    String dir_name = *txn_mgr_->GetBufferMgr()->BaseDir().get() + "/catalog";
    String catalog_path = NewCatalog::SaveAsFile(catalog_, dir_name, max_commit_ts, is_full_checkpoint);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCheckpoint>(max_commit_ts, is_full_checkpoint, catalog_path));
}
} // namespace infinity

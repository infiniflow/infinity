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
import compact_segments_task;
import default_values;
import chunk_index_entry;

namespace infinity {

Txn::Txn(TxnManager *txn_manager,
         BufferManager *buffer_manager,
         Catalog *catalog,
         BGTaskProcessor *bg_task_processor,
         TransactionID txn_id,
         TxnTimeStamp begin_ts)
    : txn_store_(this, catalog), txn_mgr_(txn_manager), buffer_mgr_(buffer_manager), bg_task_processor_(bg_task_processor), catalog_(catalog),
      txn_id_(txn_id), txn_context_(begin_ts), wal_entry_(MakeShared<WalEntry>()), local_catalog_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {}

Txn::Txn(BufferManager *buffer_mgr, TxnManager *txn_mgr, Catalog *catalog, TransactionID txn_id, TxnTimeStamp begin_ts)
    : txn_store_(this, catalog), txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), catalog_(catalog), txn_id_(txn_id), txn_context_(begin_ts),
      wal_entry_(MakeShared<WalEntry>()), local_catalog_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {}

UniquePtr<Txn> Txn::NewReplayTxn(BufferManager *buffer_mgr, TxnManager *txn_mgr, Catalog *catalog, TransactionID txn_id) {
    auto txn = MakeUnique<Txn>(buffer_mgr, txn_mgr, catalog, txn_id, MAX_TIMESTAMP);
    return txn;
}

// DML
Status Txn::Import(const String &db_name, const String &table_name, SharedPtr<SegmentEntry> segment_entry) {
    this->CheckTxn(db_name);

    // build WalCmd
    WalSegmentInfo segment_info(segment_entry.get());
    wal_entry_->cmds_.push_back(MakeShared<WalCmdImport>(db_name, table_name, std::move(segment_info)));

    TxnTableStore *table_store = this->GetTxnTableStore(table_name);
    table_store->Import(std::move(segment_entry), this);

    return Status::OK();
}

Status Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    this->CheckTxn(db_name);

    TxnTableStore *table_store = this->GetTxnTableStore(table_name);

    wal_entry_->cmds_.push_back(MakeShared<WalCmdAppend>(db_name, table_name, input_block));
    auto [err_msg, append_status] = table_store->Append(input_block);
    return append_status;
}

Status Txn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids, bool check_conflict) {
    this->CheckTxn(db_name);

    auto [table_entry, status] = GetTableByName(db_name, table_name);
    if (!status.ok()) {
        return status;
    }
    if (check_conflict && table_entry->CheckDeleteConflict(row_ids, txn_id_)) {
        LOG_WARN(fmt::format("Rollback delete in table {} due to conflict.", table_name));
        RecoverableError(Status::TxnRollback(TxnID()));
    }

    TxnTableStore *table_store = this->GetTxnTableStore(table_name);

    wal_entry_->cmds_.push_back(MakeShared<WalCmdDelete>(db_name, table_name, row_ids));
    auto [err_msg, delete_status] = table_store->Delete(row_ids);
    return delete_status;
}

Status
Txn::Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactSegmentsTaskType type) {
    const String &table_name = *table_entry->GetTableName();
    TxnTableStore *table_store = this->GetTxnTableStore(table_name);

    auto [err_mgs, compact_status] = table_store->Compact(std::move(segment_data), type);
    return compact_status;
}

TxnTableStore *Txn::GetTxnTableStore(TableEntry *table_entry) { return txn_store_.GetTxnTableStore(table_entry); }

TxnTableStore *Txn::GetTxnTableStore(const String &table_name) {
    auto *store = txn_store_.GetTxnTableStore(table_name);
    if (store == nullptr) {
        auto [table_entry, status] = this->GetTableByName(db_name_, table_name);
        if (table_entry == nullptr) {
            UnrecoverableError(status.message());
        }
        store = txn_store_.GetTxnTableStore(table_entry);
    }
    return store;
}

void Txn::CheckTxnStatus() {
    TxnState txn_state = txn_context_.GetTxnState();
    if (txn_state != TxnState::kStarted) {
        UnrecoverableError("Transaction isn't started.");
    }
}

void Txn::CheckTxn(const String &db_name) {
    this->CheckTxnStatus();
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else if (!IsEqual(db_name_, db_name)) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Attempt to get table from another database {}", db_name));
        LOG_ERROR(*err_msg);
        RecoverableError(Status::InvalidDBName(db_name));
    }
}

// Database OPs
Status Txn::CreateDatabase(const String &db_name, ConflictType conflict_type) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [db_entry, status] = catalog_->CreateDatabase(db_name, this->txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (db_entry == nullptr) { // nullptr means some exception happened
        return status;
    }
    txn_store_.AddDBStore(db_entry);

    wal_entry_->cmds_.push_back(MakeShared<WalCmdCreateDatabase>(std::move(db_name), db_entry->GetPathNameTail()));
    return Status::OK();
}

Status Txn::DropDatabase(const String &db_name, ConflictType conflict_type) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [dropped_db_entry, status] = catalog_->DropDatabase(db_name, txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (dropped_db_entry.get() == nullptr) {
        return status;
    }
    txn_store_.DropDBStore(dropped_db_entry.get());

    wal_entry_->cmds_.push_back(MakeShared<WalCmdDropDatabase>(db_name));
    return Status::OK();
}

Tuple<DBEntry *, Status> Txn::GetDatabase(const String &db_name) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetDatabase(db_name, this->txn_id_, begin_ts);
}

Tuple<SharedPtr<DatabaseInfo>, Status> Txn::GetDatabaseInfo(const String &db_name) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetDatabaseInfo(db_name, this->txn_id_, begin_ts);
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

// Table and Collection OPs
Status Txn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetTables(db_name, output_table_array, txn_id_, begin_ts);
}

Status Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_status] = catalog_->CreateTable(db_name, txn_id_, begin_ts, table_def, conflict_type, txn_mgr_);

    if (table_entry == nullptr) {
        return table_status;
    }

    txn_store_.AddTableStore(table_entry);
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCreateTable>(std::move(db_name), table_entry->GetPathNameTail(), table_def));

    return Status::OK();
}

Status Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_status] = catalog_->DropTableByName(db_name, table_name, conflict_type, txn_id_, begin_ts, txn_mgr_);

    if (table_entry.get() == nullptr) {
        return table_status;
    }

    txn_store_.DropTableStore(table_entry.get());
    wal_entry_->cmds_.push_back(MakeShared<WalCmdDropTable>(db_name, table_name));
    return Status::OK();
}

// Index OPs
Tuple<TableIndexEntry *, Status> Txn::CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_index_entry, index_status] = catalog_->CreateIndex(table_entry, index_base, conflict_type, txn_id_, begin_ts, txn_mgr_);
    if (table_index_entry == nullptr) { // nullptr means some exception happened
        return {nullptr, index_status};
    }
    auto *txn_table_store = txn_store_.GetTxnTableStore(table_entry);
    txn_table_store->AddIndexStore(table_index_entry);
    return {table_index_entry, index_status};
}

Tuple<TableIndexEntry *, Status> Txn::GetIndexByName(const String &db_name, const String &table_name, const String &index_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();
    return catalog_->GetIndexByName(db_name, table_name, index_name, txn_id_, begin_ts);
}

Tuple<SharedPtr<TableIndexInfo>, Status> Txn::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name) {
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();
    return catalog_->GetTableIndexInfo(db_name, table_name, index_name, txn_id_, begin_ts);
}

Status Txn::CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts) {
    auto *table_entry = table_ref->table_entry_ptr_;
    auto [segment_index_entries, status] =
        table_index_entry->CreateIndexPrepare(table_entry, table_ref->block_index_.get(), this, prepare, false, check_ts);
    if (!status.ok()) {
        return Status::OK();
    }

    auto *txn_table_store = txn_store_.GetTxnTableStore(table_entry);
    txn_table_store->AddSegmentIndexesStore(table_index_entry, segment_index_entries);
    for (auto &segment_index_entry : segment_index_entries) {
        Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
        segment_index_entry->GetChunkIndexEntries(chunk_index_entries);
        for (auto &chunk_index_intry : chunk_index_entries) {
            txn_table_store->AddChunkIndexStore(table_index_entry, chunk_index_intry.get());
        }
    }
    return Status::OK();
}

// TODO: use table ref instead of table entry
Status Txn::CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    auto *table_entry = table_ref->table_entry_ptr_;

    auto [table_index_entry, status] = this->GetIndexByName(db_name_, *table_entry->GetTableName(), index_name);
    if (!status.ok()) {
        return status;
    }
    if (table_index_entry->txn_id_ != txn_id_) {
        UnrecoverableError("Index is not created by this txn. Something error happened.");
    }

    return table_index_entry->CreateIndexDo(table_entry, create_index_idxes);
}

Status Txn::CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry) {
    String index_dir_tail = table_index_entry->GetPathNameTail();
    auto index_base = table_index_entry->table_index_def();
    wal_entry_->cmds_.push_back(
        MakeShared<WalCmdCreateIndex>(*table_entry->GetDBName(), *table_entry->GetTableName(), std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status Txn::CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base) {
    this->CheckTxn(db_name);

    auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, *index_base->index_name_);
    if (!status.ok()) {
        return status;
    }
    String index_dir_tail = table_index_entry->GetPathNameTail();
    this->AddWalCmd(MakeShared<WalCmdCreateIndex>(db_name, table_name, std::move(index_dir_tail), index_base));
    return Status::OK();
}

Status Txn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_index_entry, index_status] = catalog_->DropIndex(db_name, table_name, index_name, conflict_type, txn_id_, begin_ts, txn_mgr_);
    if (table_index_entry.get() == nullptr) {
        return index_status;
    }

    auto *txn_table_store = this->GetTxnTableStore(table_name);
    txn_table_store->DropIndexStore(table_index_entry.get());

    wal_entry_->cmds_.push_back(MakeShared<WalCmdDropIndex>(db_name, table_name, index_name));
    return index_status;
}

Tuple<TableEntry *, Status> Txn::GetTableByName(const String &db_name, const String &table_name) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return catalog_->GetTableByName(db_name, table_name, txn_id_, begin_ts);
}

Tuple<SharedPtr<TableInfo>, Status> Txn::GetTableInfo(const String &db_name, const String &table_name) {
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();
    return catalog_->GetTableInfo(db_name, table_name, txn_id_, begin_ts);
}

Status Txn::CreateCollection(const String &, const String &, ConflictType, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: CreateCollection"};
}

Status Txn::GetCollectionByName(const String &, const String &, BaseEntry *&) {
    return {ErrorCode::kNotSupported, "Not Implemented Txn Operation: GetCollectionByName"};
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

void Txn::SetTxnCommitting(TxnTimeStamp commit_ts) {
    txn_context_.SetTxnCommitting(commit_ts);
    wal_entry_->commit_ts_ = commit_ts;
}

WalEntry *Txn::GetWALEntry() const { return wal_entry_.get(); }

// void Txn::Begin() {
//     TxnTimeStamp ts = txn_mgr_->GetBeginTimestamp(txn_id_);
//     LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", txn_id_, ts));
//     txn_context_.SetTxnBegin(ts);
// }

// void Txn::SetBeginTS(TxnTimeStamp begin_ts) {
//     LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", txn_id_, begin_ts));
//     txn_context_.SetTxnBegin(begin_ts);
// }

TxnTimeStamp Txn::Commit() {
    //    TxnTimeStamp commit_ts = txn_mgr_->GetTimestamp(true);
    //    txn_context_.SetTxnCommitting(commit_ts);

    if (wal_entry_->cmds_.empty()) {
        // Don't need to write empty WalEntry (read-only transactions).
        TxnTimeStamp commit_ts = txn_mgr_->GetTimestamp();
        this->SetTxnCommitting(commit_ts);
        this->SetTxnCommitted();
        LOG_TRACE(fmt::format("Txn: {} is committed. commit ts: {}", txn_id_, commit_ts));
        return commit_ts;
    }
    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id_ = txn_id_;
    txn_mgr_->SendToWAL(this);

    // Wait until CommitTxnBottom is done.
    std::unique_lock<std::mutex> lk(lock_);
    cond_var_.wait(lk, [this] { return done_bottom_; });
    LOG_TRACE(fmt::format("Txn: {} is committed. commit ts: {}", txn_id_, this->CommitTS()));

    if (txn_context_.GetTxnState() == TxnState::kToRollback) {
        // abort because of conflict
        RecoverableError(Status::TxnRollback(txn_id_));
    }
    if (txn_context_.GetTxnState() != TxnState::kCommitted) {
        UnrecoverableError("Transaction isn't in COMMITTED status.");
    }

    if (txn_mgr_->enable_compaction()) {
        txn_store_.MaintainCompactionAlg();
    }
    if (!local_catalog_delta_ops_entry_->operations().empty()) {
        txn_mgr_->AddDeltaEntry(std::move(local_catalog_delta_ops_entry_));
    }

    return this->CommitTS();
}

bool Txn::CheckConflict() {
    LOG_TRACE(fmt::format("Txn check conflict: {} is started.", txn_id_));

    return txn_store_.CheckConflict();
}

void Txn::CommitBottom() {
    LOG_TRACE(fmt::format("Txn bottom: {} is started.", txn_id_));
    if (txn_context_.GetTxnState() != TxnState::kToRollback) {
        // prepare to commit txn local data into table
        TxnTimeStamp commit_ts = txn_context_.GetCommitTS();

        txn_store_.PrepareCommit(txn_id_, commit_ts, buffer_mgr_);

        txn_context_.SetTxnCommitted();

        txn_store_.CommitBottom(txn_id_, commit_ts);

        txn_store_.AddDeltaOp(local_catalog_delta_ops_entry_.get(), txn_mgr_);

        // Don't need to write empty CatalogDeltaEntry (read-only transactions).
        if (!local_catalog_delta_ops_entry_->operations().empty()) {
            local_catalog_delta_ops_entry_->SaveState(txn_id_, txn_context_.GetCommitTS(), txn_mgr_->NextSequence());
        }
    }

    // Notify the top half
    std::unique_lock<std::mutex> lk(lock_);
    done_bottom_ = true;
    cond_var_.notify_one();
    LOG_TRACE(fmt::format("Txn bottom: {} is finished.", txn_id_));
}

void Txn::CancelCommitBottom() {
    txn_context_.SetTxnRollbacked();
    std::unique_lock<std::mutex> lk(lock_);
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

    txn_store_.Rollback(txn_id_, abort_ts);

    txn_context_.SetTxnRollbacked();

    LOG_TRACE(fmt::format("Txn: {} is dropped.", txn_id_));
}

void Txn::AddWalCmd(const SharedPtr<WalCmd> &cmd) { wal_entry_->cmds_.push_back(cmd); }

bool Txn::Checkpoint(const TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    if (is_full_checkpoint) {
        FullCheckpoint(max_commit_ts);
        return true;
    } else {
        return DeltaCheckpoint(max_commit_ts);
    }
}

// Incremental checkpoint contains only the difference in status between the last checkpoint and this checkpoint (that is, "increment")
bool Txn::DeltaCheckpoint(const TxnTimeStamp max_commit_ts) {
    String delta_path;
    // only save the catalog delta entry
    bool skip = catalog_->SaveDeltaCatalog(max_commit_ts, delta_path);
    if (skip) {
        LOG_INFO("No delta catalog file is written");
        return false;
    }
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCheckpoint>(max_commit_ts, false, delta_path));
    return true;
}

void Txn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {
    String full_path;

    catalog_->SaveFullCatalog(max_commit_ts, full_path);
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCheckpoint>(max_commit_ts, true, full_path));
}

} // namespace infinity

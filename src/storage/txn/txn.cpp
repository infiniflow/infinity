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

namespace infinity {

Txn::Txn(TxnManager *txn_manager, BufferManager *buffer_manager, Catalog *catalog, BGTaskProcessor *bg_task_processor, TransactionID txn_id)
    : txn_mgr_(txn_manager), buffer_mgr_(buffer_manager), bg_task_processor_(bg_task_processor), catalog_(catalog), txn_id_(txn_id),
      wal_entry_(MakeShared<WalEntry>()), local_catalog_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {}

Txn::Txn(BufferManager *buffer_mgr, TxnManager *txn_mgr, Catalog *catalog, TransactionID txn_id)
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), catalog_(catalog), txn_id_(txn_id), wal_entry_(MakeShared<WalEntry>()),
      local_catalog_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {}

UniquePtr<Txn> Txn::NewReplayTxn(BufferManager *buffer_mgr, TxnManager *txn_mgr, Catalog *catalog, TransactionID txn_id) {
    auto txn = MakeUnique<Txn>(buffer_mgr, txn_mgr, catalog, txn_id);
    return txn;
}

// DML
Status Txn::Import(const String &db_name, const String &table_name, SharedPtr<SegmentEntry> segment_entry) {
    this->CheckTxn(db_name);

    String segment_filter_binary_data = segment_entry->GetFastRoughFilter()->SerializeToString();
    Vector<Pair<BlockID, String>> block_filter_binary_data = segment_entry->GetBlockFilterBinaryDataVector();
    const auto [block_cnt, last_block_row_count] = segment_entry->GetWalInfo();
    // build WalCmd
    wal_entry_->cmds_.push_back(MakeShared<WalCmdImport>(db_name,
                                                         table_name,
                                                         WalSegmentInfo{*segment_entry->segment_dir(),
                                                                        segment_entry->segment_id(),
                                                                        static_cast<u16>(block_cnt),
                                                                        DEFAULT_BLOCK_CAPACITY, // TODO: store block capacity in segment_entry
                                                                        last_block_row_count,
                                                                        u8(1), // have_rough_filter_: true
                                                                        segment_filter_binary_data,
                                                                        block_filter_binary_data}));

    // build delta catalog operation
    auto catalog_delta_op =
        MakeUnique<UpdateSegmentBloomFilterDataOp>(segment_entry.get(), std::move(segment_filter_binary_data), std::move(block_filter_binary_data));
    this->AddCatalogDeltaOperation(std::move(catalog_delta_op));

    TxnTableStore *table_store = this->GetTxnTableStore(table_name);
    table_store->Import(std::move(segment_entry));

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

TxnTableStore *Txn::GetTxnTableStore(const String &table_name) {
    if (auto iter = txn_tables_store_.find(table_name); iter != txn_tables_store_.end()) {
        return iter->second.get();
    }
    auto [table_entry, status] = this->GetTableByName(db_name_, table_name);
    if (table_entry == nullptr) {
        UnrecoverableError(status.message());
    }
    auto [iter, insert_ok] = txn_tables_store_.emplace(table_name, MakeUnique<TxnTableStore>(table_entry, this));
    return iter->second.get();
}

TxnTableStore *Txn::GetTxnTableStore(TableEntry *table_entry) {
    const String &table_name = *table_entry->GetTableName();
    if (auto iter = txn_tables_store_.find(table_name); iter != txn_tables_store_.end()) {
        return iter->second.get();
    }
    auto [iter, insert_ok] = txn_tables_store_.emplace(table_name, MakeUnique<TxnTableStore>(table_entry, this));
    return iter->second.get();
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
    this->AddDBStore(db_entry);

    wal_entry_->cmds_.push_back(MakeShared<WalCmdCreateDatabase>(db_name));
    return Status::OK();
}

Status Txn::DropDatabase(const String &db_name, ConflictType conflict_type) {
    this->CheckTxnStatus();
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [dropped_db_entry, status] = catalog_->DropDatabase(db_name, txn_id_, begin_ts, txn_mgr_, conflict_type);
    if (dropped_db_entry.get() == nullptr) {
        return status;
    }
    this->DropDBStore(dropped_db_entry.get());

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

    this->AddTableStore(table_entry);
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCreateTable>(db_name, table_def));

    return Status::OK();
}

Status Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    this->CheckTxn(db_name);

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    auto [table_entry, table_status] = catalog_->DropTableByName(db_name, table_name, conflict_type, txn_id_, begin_ts, txn_mgr_);

    if (table_entry.get() == nullptr) {
        return table_status;
    }

    this->DropTableStore(table_entry.get());
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
    auto *txn_table_store = this->GetTxnTableStore(table_entry);
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
    auto [fulltext_index_entry, segment_index_entries, status] =
        table_index_entry->CreateIndexPrepare(table_entry, table_ref->block_index_.get(), this, prepare, false, check_ts);
    if (!status.ok()) {
        return status;
    }

    auto *txn_table_store = this->GetTxnTableStore(table_entry);
    if (fulltext_index_entry == nullptr) {
        txn_table_store->AddSegmentIndexesStore(table_index_entry, segment_index_entries);
    } else {
        txn_table_store->AddFulltextIndexStore(table_index_entry, fulltext_index_entry);
    }

    if (!prepare) {
        String index_dir = *table_index_entry->index_dir();
        auto index_base = table_index_entry->table_index_def();
        wal_entry_->cmds_.push_back(MakeShared<WalCmdCreateIndex>(*table_entry->GetDBName(), *table_entry->GetTableName(), index_dir, index_base));
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

Status Txn::CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base) {
    this->CheckTxn(db_name);

    auto [table_index_entry, status] = this->GetIndexByName(db_name, table_name, *index_base->index_name_);
    if (!status.ok()) {
        return status;
    }

    this->AddWalCmd(MakeShared<WalCmdCreateIndex>(db_name, table_name, *table_index_entry->index_dir(), index_base));
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
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

Status Txn::GetCollectionByName(const String &, const String &, BaseEntry *&) {
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

Status Txn::CreateView(const String &, const String &, ConflictType, BaseEntry *&) {
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

Status Txn::DropViewByName(const String &, const String &, ConflictType, BaseEntry *&) {
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

Status Txn::GetViewByName(const String &, const String &, BaseEntry *&) {
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

Status Txn::GetViews(const String &, Vector<ViewDetail> &output_view_array) {
    UnrecoverableError("Not Implemented");
    return {ErrorCode::kNotSupported, "Not Implemented"};
}

void Txn::Begin() {
    TxnTimeStamp ts = txn_mgr_->GetBeginTimestamp(txn_id_);
    LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", txn_id_, ts));
    txn_context_.BeginCommit(ts);
}

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

    if (wal_entry_->cmds_.empty()) {
        // Don't need to write empty WalEntry (read-only transactions).
        txn_mgr_->Invalidate(commit_ts);
        txn_context_.SetTxnCommitted();
        return commit_ts;
    }
    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id_ = txn_id_;
    wal_entry_->commit_ts_ = commit_ts;
    txn_mgr_->PutWalEntry(wal_entry_);

    // Wait until CommitTxnBottom is done.
    std::unique_lock<std::mutex> lk(lock_);
    cond_var_.wait(lk, [this] { return done_bottom_; });
    return commit_ts;
}

void Txn::CommitBottom() noexcept {

    // prepare to commit txn local data into table
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->PrepareCommit();
    }

    txn_context_.SetTxnCommitted();

    // Commit the prepared data
    for (const auto &name_table_pair : txn_tables_store_) {
        TxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->Commit();
        table_local_store->TryTriggerCompaction(bg_task_processor_, txn_mgr_);
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

    // Don't need to write empty CatalogDeltaEntry (read-only transactions).
    if (!local_catalog_delta_ops_entry_->operations().empty()) {
        // Snapshot the physical operations in one txn
        local_catalog_delta_ops_entry_->SaveState(txn_id_, commit_ts);
        auto catalog_delta_ops_merge_task = MakeShared<CatalogDeltaOpsMergeTask>(std::move(local_catalog_delta_ops_entry_), catalog_);
        bg_task_processor_->Submit(catalog_delta_ops_merge_task);
        txn_mgr_->AddWaitFlushTxn(txn_id_);
    }

    LOG_TRACE(fmt::format("Txn: {} is committed. commit ts: {}", txn_id_, commit_ts));

    // Notify the top half
    std::unique_lock<std::mutex> lk(lock_);
    done_bottom_ = true;
    cond_var_.notify_one();
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

    for (auto *table_entry : txn_tables_) {
        table_entry->Cleanup();
        Catalog::RemoveTableEntry(table_entry, txn_id_);
    }

    for (auto *db_entry : txn_dbs_) {
        db_entry->Cleanup();
        catalog_->RemoveDBEntry(db_entry, txn_id_);
    }

    // Rollback the prepared data
    for (const auto &name_table_pair : txn_tables_store_) {
        TxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->Rollback();
    }

    txn_context_.SetTxnRollbacked();

    LOG_TRACE(fmt::format("Txn: {} is dropped.", txn_id_));
}

void Txn::AddWalCmd(const SharedPtr<WalCmd> &cmd) { wal_entry_->cmds_.push_back(cmd); }

// called by worker thread when create new entry
// Add lock because multiple threads may add catalog
void Txn::AddCatalogDeltaOperation(UniquePtr<CatalogDeltaOperation> operation) { local_catalog_delta_ops_entry_->AddOperation(std::move(operation)); }

void Txn::Checkpoint(const TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    if (is_full_checkpoint) {
        FullCheckpoint(max_commit_ts);
    } else {
        DeltaCheckpoint(max_commit_ts);
    }
}

// Incremental checkpoint contains only the difference in status between the last checkpoint and this checkpoint (that is, "increment")
void Txn::DeltaCheckpoint(const TxnTimeStamp max_commit_ts) {
    String dir_name = *txn_mgr_->GetBufferMgr()->BaseDir().get() + "/catalog";
    String delta_path = String(fmt::format("{}/CATALOG_DELTA_ENTRY.DELTA.{}", dir_name, max_commit_ts));
    // only save the catalog delta entry
    bool skip = catalog_->SaveDeltaCatalog(delta_path, max_commit_ts);
    if (skip) {
        return;
    }
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCheckpoint>(max_commit_ts, false, delta_path));
}

void Txn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {
    String dir_name = *txn_mgr_->GetBufferMgr()->BaseDir().get() + "/catalog";

    //    String delta_path = String(fmt::format("{}/META_CATALOG.{}.delta", dir_name, max_commit_ts));
    //    String delta_tmp_path = String(fmt::format("{}/_META_CATALOG.{}.delta", dir_name, max_commit_ts));
    //
    //    String catalog_path = String(fmt::format("{}/META_CATALOG.{}.json", dir_name, max_commit_ts));
    //    String catalog_tmp_path = String(fmt::format("{}/_META_CATALOG.{}.json", dir_name, max_commit_ts));

    // Full Check point don't need increment checkpoint
    // catalog_->SaveDeltaCatalog(delta_path, max_commit_ts, true);
    UniquePtr<String> catalog_path_ptr = catalog_->SaveFullCatalog(dir_name, max_commit_ts);
    wal_entry_->cmds_.push_back(MakeShared<WalCmdCheckpoint>(max_commit_ts, true, *catalog_path_ptr));
}

void Txn::AddDBStore(DBEntry *db_entry) { txn_dbs_.insert(db_entry); }

void Txn::DropDBStore(DBEntry *dropped_db_entry) {
    if (txn_dbs_.contains(dropped_db_entry)) {
        txn_dbs_.erase(dropped_db_entry);
        dropped_db_entry->Cleanup();
    } else {
        txn_dbs_.insert(dropped_db_entry);
    }
}

void Txn::AddTableStore(TableEntry *table_entry) { txn_tables_.insert(table_entry); }

void Txn::DropTableStore(TableEntry *dropped_table_entry) {
    if (txn_tables_.contains(dropped_table_entry)) {
        txn_tables_.erase(dropped_table_entry);
        if (catalog_->CheckAllowCleanup(dropped_table_entry)) {
            dropped_table_entry->Cleanup();
        }
        txn_tables_store_.erase(*dropped_table_entry->GetTableName());
    } else {
        txn_tables_.insert(dropped_table_entry);
    }
}

} // namespace infinity

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
import txn_state;

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
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()), txn_text_(std::move(txn_text)) {}

Txn::Txn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TransactionType txn_type)
    : txn_mgr_(txn_mgr), buffer_mgr_(buffer_mgr), txn_store_(this), wal_entry_(MakeShared<WalEntry>()),
      txn_delta_ops_entry_(MakeUnique<CatalogDeltaEntry>()) {}

UniquePtr<Txn>
Txn::NewReplayTxn(BufferManager *buffer_mgr, TxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TransactionType txn_type) {
    return nullptr;
}

Txn::~Txn() {}

// DML
Status Txn::Import(const String &db_name, const String &table_name, SharedPtr<SegmentEntry> segment_entry) { return Status::OK(); }

Status Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) { return Status::OK(); }

Status Txn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids, bool check_conflict) { return Status::OK(); }

Status
Txn::Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type) {
    return Status::OK();
}

void Txn::OptimizeIndexes() {}

Status Txn::OptimizeTableIndexes(const String &db_name, const String &table_name) { return Status::OK(); }

Status
Txn::OptimizeIndexByName(const String &db_name, const String &table_name, const String &index_name, Vector<UniquePtr<InitParameter>> init_params) {
    return Status::OK();
}

Status Txn::OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params) { return Status::OK(); }

TxnTableStore *Txn::GetTxnTableStore(TableEntry *table_entry) { return txn_store_.GetTxnTableStore(table_entry); }

TxnTableStore *Txn::GetExistTxnTableStore(TableEntry *table_entry) const { return txn_store_.GetExistTxnTableStore(table_entry); }

void Txn::CheckTxnStatus() {}

void Txn::CheckTxn(const String &db_name) {}

// Database OPs
Status Txn::CreateDatabase(const SharedPtr<String> &db_name, ConflictType conflict_type, const SharedPtr<String> &comment) { return Status::OK(); }

Status Txn::DropDatabase(const String &db_name, ConflictType conflict_type) { return Status::OK(); }

Status Txn::GetDatabase(const String &db_name) { return Status::OK(); }

Tuple<SharedPtr<DatabaseInfo>, Status> Txn::GetDatabaseInfo(const String &db_name) { return {nullptr, Status::OK()}; }

Vector<DatabaseDetail> Txn::ListDatabases() {
    Vector<DatabaseDetail> res;

    return res;
}

// Table and Collection OPs
Status Txn::GetTables(const String &db_name, Vector<TableDetail> &output_table_array) { return Status::OK(); }

Status Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) { return Status::OK(); }

Status Txn::RenameTable(const String &old_table_name, const String &new_table_name) { return Status::OK(); }

Status Txn::AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs) { return Status::OK(); }

Status Txn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) { return Status::OK(); }

Status Txn::DropTable(const String &db_name, const String &table_name, ConflictType conflict_type) { return Status::OK(); }

// Index OPs
Tuple<TableIndexEntry *, Status> Txn::CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type) {
    return {nullptr, Status::OK()};
}

Tuple<TableIndexEntry *, Status> Txn::GetIndexByName(const String &db_name, const String &table_name, const String &index_name) {
    return {nullptr, Status::OK()};
}

Tuple<SharedPtr<TableIndexInfo>, Status> Txn::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name) {
    return {nullptr, Status::OK()};
}

Pair<Vector<SegmentIndexEntry *>, Status>
Txn::CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts) {
    Vector<SegmentIndexEntry *> res;
    return {res, Status::OK()};
}

// TODO: use table ref instead of table entry
Status Txn::CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    return Status::OK();
}

Status Txn::CreateIndexDo(TableEntry *table_entry,
                          const Map<SegmentID, SegmentIndexEntry *> &segment_index_entries,
                          const String &index_name,
                          HashMap<SegmentID, atomic_u64> &create_index_idxes) {

    return Status::OK();
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
    return Status::OK();
}

SharedPtr<IndexReader> Txn::GetFullTextIndexReader(const String &db_name, const String &table_name) { return nullptr; }

Tuple<SharedPtr<SegmentIndexInfo>, Status>
Txn::GetSegmentIndexInfo(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {

    return {nullptr, Status::OK()};
}

Tuple<TableEntry *, Status> Txn::GetTableByName(const String &db_name, const String &table_name) { return {nullptr, Status::OK()}; }

SharedPtr<BlockIndex> Txn::GetBlockIndexFromTable(const String &db_name, const String &table_name) { return nullptr; }

Tuple<SharedPtr<TableInfo>, Status> Txn::GetTableInfo(const String &db_name, const String &table_name) {
    return catalog_->GetTableInfo(db_name, table_name, this);
}

Tuple<SharedPtr<SegmentInfo>, Status> Txn::GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    return {nullptr, Status::OK()};
}

Tuple<Vector<SharedPtr<SegmentInfo>>, Status> Txn::GetSegmentsInfo(const String &db_name, const String &table_name) {
    Vector<SharedPtr<SegmentInfo>> res;
    return {res, Status::OK()}; }

Tuple<SharedPtr<BlockInfo>, Status> Txn::GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id) {
    return {nullptr, Status::OK()};
}

Tuple<Vector<SharedPtr<BlockInfo>>, Status> Txn::GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id) {
    Vector<SharedPtr<BlockInfo>> res;
    return {res, Status::OK()};
}

Tuple<SharedPtr<BlockColumnInfo>, Status>
Txn::GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id) {
    return {nullptr, Status::OK()};
}

Status Txn::CheckTableExist(const String &db_name, const String &table_name) { return Status::OK(); }

bool Txn::CheckTableHasDelete(const String &db_name, const String &table_name) { return false; }

Tuple<SharedPtr<TableSnapshotInfo>, Status> Txn::GetTableSnapshot(const String &db_name, const String &table_name) { return {nullptr, Status::OK()}; }

Status Txn::ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info) { return Status::OK(); }

Status Txn::CreateCollection(const String &, const String &, ConflictType, BaseEntry *&) { return Status::OK(); }

Status Txn::GetCollectionByName(const String &, const String &, BaseEntry *&) { return Status::OK(); }

Status Txn::AddWriteTxnNum(const String &db_name, const String &table_name) { return Status::OK(); }

Tuple<SharedPtr<SegmentEntry>, Status> Txn::MakeNewSegment(const String &db_name, const String &table_name) { return {nullptr, Status::OK()}; }

Status Txn::CreateView(const String &, const String &, ConflictType, BaseEntry *&) { return Status::OK(); }

Status Txn::DropViewByName(const String &, const String &, ConflictType, BaseEntry *&) { return Status::OK(); }

Status Txn::GetViewByName(const String &, const String &, BaseEntry *&) { return Status::OK(); }

Status Txn::GetViews(const String &, Vector<ViewDetail> &output_view_array) { return Status::OK(); }

TxnTimeStamp Txn::CommitTS() const { return 0; }

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

void Txn::SetTxnRollbacking(TxnTimeStamp rollback_ts) {}

void Txn::SetTxnRollbacked() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    txn_context_ptr_->state_ = TxnState::kRollbacked;
}

void Txn::SetTxnRead() { txn_context_ptr_->is_write_transaction_ = false; }

void Txn::SetTxnWrite() { txn_context_ptr_->is_write_transaction_ = true; }

void Txn::SetTxnCommitted() {}

void Txn::SetTxnCommitting(TxnTimeStamp commit_ts) {}

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

TxnTimeStamp Txn::Commit() { return 0; }

bool Txn::CheckConflict() { return false; }

Optional<String> Txn::CheckConflict(Txn *other_txn) { return ""; }

void Txn::CommitBottom() {}

void Txn::PostCommit() {}

void Txn::CancelCommitBottom() {}

void Txn::Rollback() {}

SharedPtr<AddDeltaEntryTask> Txn::MakeAddDeltaEntryTask() { return nullptr; }

void Txn::AddWalCmd(const SharedPtr<WalCmd> &cmd) {}

// the max_commit_ts is determined by the max commit ts of flushed delta entry
// Incremental checkpoint contains only the difference in status between the last checkpoint and this checkpoint (that is, "increment")
bool Txn::DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts) { return true; }

// those whose commit_ts is <= max_commit_ts will be checkpointed
void Txn::FullCheckpoint(const TxnTimeStamp max_commit_ts) {}

void Txn::AddWriteTxnNum(TableEntry *table_entry) {}

} // namespace infinity

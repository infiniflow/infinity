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

import base_entry;
import segment_entry;
import block_entry;
import table_collection_meta;
import table_collection_entry;
import table_collection_detail;
import table_collection_type;
import new_catalog;
import database_detail;
import status;
import table_def;
import index_def;
import index_def_meta;
import index_def_entry;

module txn;

namespace infinity {

Txn::Txn(TxnManager *txn_mgr, NewCatalog *catalog, u32 txn_id)
    : txn_mgr_(txn_mgr), catalog_(catalog), txn_id_(txn_id), wal_entry_(MakeShared<WalEntry>()) {}

Status Txn::GetTableEntry(const String &db_name, const String &table_name, TableCollectionEntry *&table_entry) {
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else {
        if (!IsEqual(db_name_, db_name)) {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Attempt to get table {} from another database {}", db_name, table_name));
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kNotFound, Move(err_msg));
        }
    }

    auto table_iter = txn_table_entries_.find(table_name);
    if (table_iter == txn_table_entries_.end()) {
        // not found the table in cached entries
        BaseEntry *base_table_entry{};
        Status status = this->GetTableByName(db_name, table_name, base_table_entry);
        if (!status.ok()) {
            return status;
        }

        table_entry = (TableCollectionEntry *)base_table_entry;
        txn_table_entries_[table_name] = reinterpret_cast<BaseEntry *>(table_entry);
    } else {
        table_entry = (TableCollectionEntry *)table_iter->second;
    }

    return Status::OK();
}

Status Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    TableCollectionEntry *table_entry{nullptr};
    Status status = GetTableEntry(db_name, table_name, table_entry);
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
    TableCollectionEntry *table_entry{nullptr};
    Status status = GetTableEntry(db_name, table_name, table_entry);
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

void Txn::GetMetaTableState(MetaTableState *meta_table_state, const String &db_name, const String &table_name, const Vector<ColumnID> &columns) {
    auto txn_table_iter = txn_tables_store_.find(table_name);
    if (txn_table_iter != txn_tables_store_.end()) {
        Vector<SharedPtr<DataBlock>> &local_blocks = txn_table_iter->second->blocks_;
        meta_table_state->local_blocks_.reserve(local_blocks.size());
        SizeT block_count = local_blocks.size();
        for (SizeT idx = 0; idx < block_count; ++idx) {
            const auto &data_block = local_blocks[idx];
            MetaLocalDataState data_block_state;
            data_block_state.data_block_ = data_block.get();
            SizeT column_count = columns.size();
            for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
                const auto &column_id = columns[col_idx];
                MetaColumnVectorState column_vector_state;
                column_vector_state.column_vector_ = data_block->column_vectors[column_id].get();
                data_block_state.column_vector_map_[column_id] = column_vector_state;
            }
            meta_table_state->local_blocks_.emplace_back(data_block_state);
        }
    }

    TableCollectionEntry *table_entry{nullptr};
    Status status = GetTableEntry(db_name, table_name, table_entry);
    if (!status.ok()) {
        Error<TransactionException>(status.message());
    }

    return GetMetaTableState(meta_table_state, table_entry, columns);
}

void Txn::GetMetaTableState(MetaTableState *meta_table_state, const TableCollectionEntry *table_collection_entry, const Vector<ColumnID> &columns) {
    u64 max_segment_id = TableCollectionEntry::GetMaxSegmentID(table_collection_entry);
    for (u64 segment_id = 0; segment_id < max_segment_id; ++segment_id) {
        SegmentEntry *segment_entry_ptr = TableCollectionEntry::GetSegmentByID(table_collection_entry, segment_id);

        MetaSegmentState segment_state;
        segment_state.segment_entry_ = segment_entry_ptr;

        u16 max_block_id = SegmentEntry::GetMaxBlockID(segment_entry_ptr);
        for (u16 block_id = 0; block_id < max_block_id; ++block_id) {
            BlockEntry *block_entry_ptr = SegmentEntry::GetBlockEntryByID(segment_entry_ptr, block_id);

            MetaBlockState block_state;
            block_state.block_entry_ = block_entry_ptr;

            SizeT column_count = columns.size();
            for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
                const auto &column_id = columns[col_idx];
                MetaBlockColumnState column_data_state;
                column_data_state.block_column_ = BlockEntry::GetColumnDataByID(block_entry_ptr, column_id);
                block_state.column_data_map_[column_id] = column_data_state;
            }

            segment_state.block_map_[block_id] = block_state;
        }

        meta_table_state->segment_map_[segment_id] = segment_state;
    }
}

TxnTableStore *Txn::GetTxnTableStore(TableCollectionEntry *table_entry) {
    UniqueLock<Mutex> lk(lock_);
    auto txn_table_iter = txn_tables_store_.find(*table_entry->table_collection_name_);
    if (txn_table_iter != txn_tables_store_.end()) {
        return txn_table_iter->second.get();
    }
    txn_tables_store_[*table_entry->table_collection_name_] = MakeUnique<TxnTableStore>(table_entry, this);
    TxnTableStore *txn_table_store = txn_tables_store_[*table_entry->table_collection_name_].get();
    return txn_table_store;
}

BufferManager *Txn::GetBufferMgr() const { return this->txn_mgr_->GetBufferMgr(); }

Status Txn::CreateDatabase(const String &db_name, ConflictType conflict_type, BaseEntry *&base_entry) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    Status status = NewCatalog::CreateDatabase(catalog_, db_name, this->txn_id_, begin_ts, txn_mgr_, base_entry);
    if (!status.ok()) {
        return status;
    }

    if (base_entry->entry_type_ != EntryType::kDatabase) {
        Error<TransactionException>("Entry type should be table entry.");
    }

    auto *db_entry = static_cast<DBEntry *>(base_entry);
    txn_dbs_.insert(db_entry);
    db_names_.insert(db_name);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateDatabase>(db_name));
    return Status::OK();
}

Status Txn::DropDatabase(const String &db_name, ConflictType conflict_type, BaseEntry *&base_entry) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    Status status = NewCatalog::DropDatabase(catalog_, db_name, txn_id_, begin_ts, txn_mgr_, base_entry);
    if (!status.ok()) {
        return status;
    }

    DBEntry *dropped_db_entry = static_cast<DBEntry *>(base_entry);

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

Status Txn::GetDatabase(const String &db_name, BaseEntry *&new_db_entry) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts, new_db_entry);
}

Vector<DatabaseDetail> Txn::ListDatabases() {
    Vector<DatabaseDetail> res;

    Vector<DBEntry *> db_entries = NewCatalog::Databases(catalog_, txn_id_, txn_context_.GetBeginTS());
    SizeT db_count = db_entries.size();
    for (SizeT idx = 0; idx < db_count; ++idx) {
        DBEntry *db_entry = db_entries[idx];
        res.emplace_back(DatabaseDetail{db_entry->db_name_});
    }

    return res;
}

Status Txn::GetTableCollections(const String &db_name, Vector<TableCollectionDetail> &output_table_array) {
    BaseEntry *base_entry{nullptr};
    Status status = GetDatabase(db_name, base_entry);
    if (!status.ok()) {
        return status;
    }

    DBEntry *db_entry = (DBEntry *)base_entry;
    return DBEntry::GetTableCollectionsDetail(db_entry, txn_id_, txn_context_.GetBeginTS(), output_table_array);
}

Status Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type, BaseEntry *&new_table_entry) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    BaseEntry *base_db_entry{nullptr};
    Status status = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts, base_db_entry);
    if (!status.ok()) {
        // Error
        return status;
    }

    DBEntry *db_entry = (DBEntry *)base_db_entry;

    status = DBEntry::CreateTableCollection(db_entry,
                                            TableCollectionType::kTableEntry,
                                            table_def->table_name(),
                                            table_def->columns(),
                                            txn_id_,
                                            begin_ts,
                                            txn_mgr_,
                                            new_table_entry);

    if (new_table_entry == nullptr) {
        if(status.ok()) {
            UniquePtr<String> err_msg = MakeUnique<String>("TODO: CreateTableCollectionFailed");
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kError, Move(err_msg));
        } else {
            return status;
        }
    }

    if (new_table_entry->entry_type_ != EntryType::kTable) {
        Error<TransactionException>("Entry type should be table entry.");
    }

    auto *table_entry = static_cast<TableCollectionEntry *>(new_table_entry);
    txn_tables_.insert(table_entry);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateTable>(db_name, table_def));
    return Status::OK();
}

Status Txn::CreateIndex(const String &db_name, const String &table_name, SharedPtr<IndexDef> index_def, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction is not started");
    }
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    TableCollectionEntry *table_entry{nullptr};
    Status table_status = GetTableEntry(db_name, table_name, table_entry);
    if (!table_status.ok()) {
        return table_status;
    }

    BaseEntry *base_entry{nullptr};
    Status index_status = TableCollectionEntry::CreateIndex(table_entry, index_def, conflict_type, txn_id_, begin_ts, txn_mgr_, base_entry);

    if (!index_status.ok()) {
        return index_status;
    }

    if (index_status.ok() && base_entry == nullptr && conflict_type == ConflictType::kIgnore) {
        return index_status;
    }

    if (base_entry->entry_type_ != EntryType::kIndexDef) {
        Error<TransactionException>("Invalid index type");
    }
    auto index_def_entry = static_cast<IndexDefEntry *>(base_entry);
    txn_indexes_.emplace(*index_def->index_name_, index_def_entry);

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    TableCollectionEntry::CreateIndexFile(table_entry, table_store, index_def_entry, begin_ts, GetBufferMgr());

    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateIndex>(db_name, table_name, index_def));
    return index_status;
}

Status Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type, BaseEntry *&drop_table_entry) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    BaseEntry *base_db_entry{nullptr};
    Status status = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts, base_db_entry);
    if (!status.ok()) {
        // Error
        return status;
    }

    DBEntry *db_entry = (DBEntry *)base_db_entry;

    status = DBEntry::DropTableCollection(db_entry, table_name, conflict_type, txn_id_, begin_ts, txn_mgr_, drop_table_entry);

    if (drop_table_entry == nullptr) {
        return status;
    }

    TableCollectionEntry *dropped_table_entry = static_cast<TableCollectionEntry *>(drop_table_entry);

    if (txn_tables_.contains(dropped_table_entry)) {
        txn_tables_.erase(dropped_table_entry);
    } else {
        txn_tables_.insert(dropped_table_entry);
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropTable>(db_name, table_name));
    return Status::OK();
}

Status Txn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();
    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction is not started");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();
    TableCollectionEntry *table_entry{nullptr};
    Status table_status = GetTableEntry(db_name, table_name, table_entry);
    if (!table_status.ok()) {
        return table_status;
    }

    BaseEntry *base_entry{nullptr};
    Status index_status = TableCollectionEntry::DropIndex(table_entry, index_name, conflict_type, txn_id_, begin_ts, txn_mgr_, base_entry);
    if (!index_status.ok()) {
        return index_status;
    }

    if (index_status.ok() && base_entry == nullptr && conflict_type == ConflictType::kIgnore) {
        return index_status;
    }

    if (auto iter = txn_indexes_.find(index_name); iter != txn_indexes_.end()) {
        txn_indexes_.erase(iter);
    } else {
        txn_indexes_.emplace(index_name, static_cast<IndexDefEntry *>(base_entry));
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropIndex>(db_name, table_name, index_name));
    return index_status;
}

Status Txn::GetTableByName(const String &db_name, const String &table_name, BaseEntry *&new_table_entry) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        Error<TransactionException>("Transaction isn't started.");
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    // Check the db entries
    BaseEntry *base_db_entry{nullptr};
    Status status = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts, base_db_entry);
    if (!status.ok()) {
        // Error
        return status;
    }

    // Check the table entries
    auto db_entry = (DBEntry *)base_db_entry;

    return DBEntry::GetTableCollection(db_entry, table_name, txn_id_, begin_ts, new_table_entry);
}

Status Txn::CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type, BaseEntry *&collection_entry) {
    Error<TransactionException>("Not Implemented");
    return Status(ErrorCode::kNotImplemented, "Not Implemented");
}

Status Txn::GetCollectionByName(const String &db_name, const String &table_name, BaseEntry *&collection_entry) {
    Error<TransactionException>("Not Implemented");
    return Status(ErrorCode::kNotImplemented, "Not Implemented");
}

Status Txn::CreateView(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry) {
    Error<TransactionException>("Not Implemented");
    return Status(ErrorCode::kNotImplemented, "Not Implemented");
}

Status Txn::DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry) {
    Error<TransactionException>("Not Implemented");
    return Status(ErrorCode::kNotImplemented, "Not Implemented");
}

Status Txn::GetViewByName(const String &db_name, const String &view_name, BaseEntry *&view_entry) {
    Error<TransactionException>("Not Implemented");
    return Status(ErrorCode::kNotImplemented, "Not Implemented");
}

Vector<BaseEntry *> Txn::GetViews(const String &db_name) {
    Error<TransactionException>("Not Implemented");
    return Vector<BaseEntry *>();
}

void Txn::Begin() { txn_context_.BeginCommit(txn_mgr_->GetTimestamp()); }

void Txn::Commit() {
    TxnTimeStamp commit_ts = txn_mgr_->GetTimestamp(true);
    txn_context_.SetTxnCommitting(commit_ts);
    // TODO: serializability validation. ASSUMES always valid for now.
    bool valid = true;
    if (!valid) {
        txn_mgr_->Invalidate(commit_ts);
        txn_context_.SetTxnRollbacked();
        return;
    }

    if (wal_entry_->cmds.empty()) {
        // Don't need to write empty WalEntry (read-only transactions).
        txn_mgr_->Invalidate(commit_ts);
        txn_context_.SetTxnCommitted();
        return;
    }
    // Put wal entry to the manager in the same order as commit_ts.
    wal_entry_->txn_id = txn_id_;
    wal_entry_->commit_ts = commit_ts;
    txn_mgr_->PutWalEntry(wal_entry_);

    // Wait until CommitTxnBottom is done.
    UniqueLock<Mutex> lk(lock_);
    cond_var_.wait(lk, [this] { return done_bottom_; });
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
    for (const auto &[index_name, index_def_entry] : txn_indexes_) {
        index_def_entry->Commit(commit_ts);
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
        auto *table_entry = (TableCollectionEntry *)(base_entry);
        TableCollectionMeta *table_meta = TableCollectionEntry::GetTableMeta(table_entry);
        DBEntry *db_entry = TableCollectionEntry::GetDBEntry(table_entry);
        DBEntry::RemoveTableCollectionEntry(db_entry, *table_meta->table_collection_name_, txn_id_, txn_mgr_);
    }

    for (const auto &[index_name, index_def_entry] : txn_indexes_) {
        IndexDefMeta *index_def_meta = index_def_entry->index_def_meta_;
        TableCollectionEntry *table_entry = index_def_meta->table_collection_entry_;
        TableCollectionEntry::RemoveIndexEntry(table_entry, index_name, txn_id_, txn_mgr_);
    }

    for (const auto &db_name : db_names_) {
        NewCatalog::RemoveDBEntry(catalog_, db_name, this->txn_id_, txn_mgr_);
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

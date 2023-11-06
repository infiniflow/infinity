//
// Created by jinhai on 23-6-4.
//

module;

#include <vector>
#include <string>

import stl;
import infinity_assert;
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

import table_def;
import index_def;
import index_def_meta;
import index_def_entry;

module txn;

namespace infinity {

Txn::Txn(TxnManager *txn_mgr, NewCatalog *catalog, u32 txn_id)
    : txn_mgr_(txn_mgr), catalog_(catalog), txn_id_(txn_id), wal_entry_(MakeShared<WalEntry>()) {}

UniquePtr<String> Txn::GetTableEntry(const String &db_name, const String &table_name, TableCollectionEntry *&table_entry) {
    if (db_name_.empty()) {
        db_name_ = db_name;
    } else {
        if (!IsEqual(db_name_, db_name)) {
            String err_msg = Format("Attempt to get table {} from another database {}", db_name, table_name);
            LOG_ERROR(err_msg);
            return MakeUnique<String>(err_msg);
        }
    }

    auto table_iter = txn_table_entries_.find(table_name);
    if (table_iter == txn_table_entries_.end()) {
        // not found the table in cached entries
        EntryResult entry_result = this->GetTableByName(db_name, table_name);
        if (entry_result.entry_ == nullptr) {
            return Move(entry_result.err_);
        }

        table_entry = (TableCollectionEntry *)entry_result.entry_;
        txn_table_entries_[table_name] = reinterpret_cast<BaseEntry *>(table_entry);
    } else {
        table_entry = (TableCollectionEntry *)table_iter->second;
    }

    return nullptr;
}

UniquePtr<String> Txn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    TableCollectionEntry *table_entry{nullptr};
    UniquePtr<String> err_msg = GetTableEntry(db_name, table_name, table_entry);
    if (err_msg.get() != nullptr) {
        return err_msg;
    }

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    wal_entry_->cmds.push_back(MakeShared<WalCmdAppend>(db_name, table_name, input_block));
    return table_store->Append(input_block);
}

UniquePtr<String> Txn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids) {
    TableCollectionEntry *table_entry{nullptr};
    UniquePtr<String> err_msg = GetTableEntry(db_name, table_name, table_entry);
    if (err_msg.get() != nullptr) {
        return err_msg;
    }

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    wal_entry_->cmds.push_back(MakeShared<WalCmdDelete>(db_name, table_name, row_ids));
    return table_store->Delete(row_ids);
}

void Txn::GetMetaTableState(MetaTableState *meta_table_state, const String &db_name, const String &table_name, const Vector<ColumnID> &columns) {
    auto txn_table_iter = txn_tables_store_.find(table_name);
    if (txn_table_iter != txn_tables_store_.end()) {
        Vector<SharedPtr<DataBlock>> &local_blocks = txn_table_iter->second->blocks_;
        meta_table_state->local_blocks_.reserve(local_blocks.size());
        SizeT block_count = local_blocks.size();
        for(SizeT idx = 0; idx < block_count; ++ idx) {
            const auto &data_block = local_blocks[idx];
            MetaLocalDataState data_block_state;
            data_block_state.data_block_ = data_block.get();
            SizeT column_count = columns.size();
            for(SizeT col_idx = 0; col_idx < column_count; ++ col_idx) {
                const auto &column_id = columns[col_idx];
                MetaColumnVectorState column_vector_state;
                column_vector_state.column_vector_ = data_block->column_vectors[column_id].get();
                data_block_state.column_vector_map_[column_id] = column_vector_state;
            }
            meta_table_state->local_blocks_.emplace_back(data_block_state);
        }
    }

    TableCollectionEntry *table_entry{nullptr};
    UniquePtr<String> err_msg = GetTableEntry(db_name, table_name, table_entry);
    if (err_msg.get() != nullptr) {
        Error<TransactionException>(*err_msg, __FILE_NAME__, __LINE__);
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
            for(SizeT col_idx = 0; col_idx < column_count; ++ col_idx) {
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
    UniqueLock<Mutex> lk(m);
    auto txn_table_iter = txn_tables_store_.find(*table_entry->table_collection_name_);
    if (txn_table_iter != txn_tables_store_.end()) {
        return txn_table_iter->second.get();
    }
    txn_tables_store_[*table_entry->table_collection_name_] = MakeUnique<TxnTableStore>(table_entry, this);
    TxnTableStore *txn_table_store = txn_tables_store_[*table_entry->table_collection_name_].get();
    return txn_table_store;
}

BufferManager *Txn::GetBufferMgr() const { return this->txn_mgr_->GetBufferMgr(); }

EntryResult Txn::CreateDatabase(const String &db_name, ConflictType conflict_type) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    EntryResult res = NewCatalog::CreateDatabase(catalog_, db_name, this->txn_id_, begin_ts, txn_mgr_);
    if (res.entry_ == nullptr) {
        return res;
    }

    if (res.entry_->entry_type_ != EntryType::kDatabase) {
        return {nullptr, MakeUnique<String>("Invalid database type")};
    }

    auto *db_entry = static_cast<DBEntry *>(res.entry_);
    txn_dbs_.insert(db_entry);
    db_names_.insert(db_name);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateDatabase>(db_name));
    return res;
}

EntryResult Txn::DropDatabase(const String &db_name, ConflictType conflict_type) {

    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    EntryResult res = NewCatalog::DropDatabase(catalog_, db_name, txn_id_, begin_ts, txn_mgr_);

    if (res.entry_ == nullptr) {
        return res;
    }

    DBEntry *dropped_db_entry = static_cast<DBEntry *>(res.entry_);

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
    return res;
}

EntryResult Txn::GetDatabase(const String &db_name) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    return NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts);
}

Vector<DatabaseDetail> Txn::ListDatabases() {
    Vector<DatabaseDetail> res;

    Vector<DBEntry *> db_entries = NewCatalog::Databases(catalog_, txn_id_, txn_context_.GetBeginTS());
    SizeT db_count = db_entries.size();
    for(SizeT idx = 0; idx < db_count; ++ idx) {
        DBEntry* db_entry = db_entries[idx];
        res.emplace_back(DatabaseDetail{db_entry->db_name_});
    }

    return res;
}

Vector<TableCollectionDetail> Txn::GetTableCollections(const String &db_name) {
    EntryResult entry_result = GetDatabase(db_name);
    if (entry_result.err_.get() != nullptr) {
        Error<TransactionException>(*entry_result.err_, __FILE_NAME__, __LINE__);
    }

    DBEntry *db_entry = (DBEntry *)entry_result.entry_;
    return DBEntry::GetTableCollectionsDetail(db_entry, txn_id_, txn_context_.GetBeginTS());
}

EntryResult Txn::CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    EntryResult db_entry_result = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts);
    if (db_entry_result.entry_ == nullptr) {
        // Error
        return db_entry_result;
    }

    DBEntry *db_entry = (DBEntry *)db_entry_result.entry_;

    EntryResult res = DBEntry::CreateTableCollection(db_entry,
                                                     TableCollectionType::kTableEntry,
                                                     table_def->table_name(),
                                                     table_def->columns(),
                                                     txn_id_,
                                                     begin_ts,
                                                     txn_mgr_);

    if (res.entry_ == nullptr) {
        return res;
    }

    if (res.entry_->entry_type_ != EntryType::kTable) {
        return {nullptr, MakeUnique<String>("Invalid database type")};
    }

    auto *table_entry = static_cast<TableCollectionEntry *>(res.entry_);
    txn_tables_.insert(table_entry);
    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateTable>(db_name, table_def));
    return res;
}

EntryResult Txn::CreateIndex(const String &db_name, const String &table_name, SharedPtr<IndexDef> index_def, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction is not started");
        return {.entry_ = nullptr, .err_ = MakeUnique<String>("Transaction is not started")};
    }
    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    TableCollectionEntry *table_entry{nullptr};
    UniquePtr<String> err_msg = GetTableEntry(db_name, table_name, table_entry);
    if (err_msg.get() != nullptr) {
        return {nullptr, Move(err_msg)};
    }

    EntryResult res = TableCollectionEntry::CreateIndex(table_entry, index_def, conflict_type, txn_id_, begin_ts, txn_mgr_);

    if (res.entry_ == nullptr) {
        return res;
    }
    if (res.entry_->entry_type_ != EntryType::kIndexDef) {
        return {nullptr, MakeUnique<String>("Invalid index type")};
    }
    auto index_def_entry = static_cast<IndexDefEntry *>(res.entry_);
    txn_indexes_.insert(index_def_entry);

    TxnTableStore *table_store{nullptr};
    if (txn_tables_store_.find(table_name) == txn_tables_store_.end()) {
        txn_tables_store_[table_name] = MakeUnique<TxnTableStore>(table_entry, this);
    }
    table_store = txn_tables_store_[table_name].get();

    TableCollectionEntry::CreateIndexFile(table_entry, table_store, *index_def, begin_ts, GetBufferMgr());

    wal_entry_->cmds.push_back(MakeShared<WalCmdCreateIndex>(db_name, table_name, index_def));
    return res;
}

EntryResult Txn::DropTableCollectionByName(const String &db_name, const String &table_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    EntryResult db_entry_result = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts);
    if (db_entry_result.entry_ == nullptr) {
        // Error
        return db_entry_result;
    }

    DBEntry *db_entry = (DBEntry *)db_entry_result.entry_;

    EntryResult res = DBEntry::DropTableCollection(db_entry, table_name, conflict_type, txn_id_, begin_ts, txn_mgr_);

    if (res.entry_ == nullptr) {
        return res;
    }

    TableCollectionEntry *dropped_table_entry = static_cast<TableCollectionEntry *>(res.entry_);

    if (txn_tables_.contains(dropped_table_entry)) {
        txn_tables_.erase(dropped_table_entry);
    } else {
        txn_tables_.insert(dropped_table_entry);
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropTable>(db_name, table_name));
    return res;
}

EntryResult Txn::DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();
    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();
    TableCollectionEntry *table_entry{nullptr};
    UniquePtr<String> err_msg = GetTableEntry(db_name, table_name, table_entry);
    if (err_msg.get() != nullptr) {
        return {nullptr, Move(err_msg)};
    }

    EntryResult res = TableCollectionEntry::DropIndex(table_entry, index_name, conflict_type, txn_id_, begin_ts, txn_mgr_);
    if (res.entry_ == nullptr) {
        return res;
    }

    auto dropped_index_entry = static_cast<IndexDefEntry *>(res.entry_);
    if (txn_indexes_.contains(dropped_index_entry)) {
        txn_indexes_.erase(dropped_index_entry);
    } else {
        txn_indexes_.insert(dropped_index_entry);
    }

    wal_entry_->cmds.push_back(MakeShared<WalCmdDropIndex>(db_name, table_name, index_name));
    return res;
}

EntryResult Txn::GetTableByName(const String &db_name, const String &table_name) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    // Check the db entries
    EntryResult db_entry_result = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts);
    if (db_entry_result.entry_ == nullptr) {
        // Error
        return db_entry_result;
    }

    // Check the table entries
    auto db_entry = (DBEntry *)db_entry_result.entry_;

    return DBEntry::GetTableCollection(db_entry, table_name, txn_id_, begin_ts);
}

EntryResult Txn::CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type) {
    TxnState txn_state = txn_context_.GetTxnState();

    if (txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.");
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    TxnTimeStamp begin_ts = txn_context_.GetBeginTS();

    EntryResult db_entry_result = NewCatalog::GetDatabase(catalog_, db_name, this->txn_id_, begin_ts);
    if (db_entry_result.entry_ == nullptr) {
        // Error
        return db_entry_result;
    }

    DBEntry *db_entry = (DBEntry *)db_entry_result.entry_;

    Vector<SharedPtr<ColumnDef>> empty_columns;

    EntryResult res = DBEntry::CreateTableCollection(db_entry,
                                                     TableCollectionType::kCollectionEntry,
                                                     MakeShared<String>(collection_name),
                                                     empty_columns,
                                                     txn_id_,
                                                     begin_ts,
                                                     txn_mgr_);
    if (res.entry_ == nullptr) {
        return res;
    }

    if (res.entry_->entry_type_ != EntryType::kTable) {
        return {nullptr, MakeUnique<String>("Invalid database type")};
    }

    auto *table_entry = static_cast<TableCollectionEntry *>(res.entry_);
    txn_tables_.insert(table_entry);
    return res;
}

EntryResult Txn::GetCollectionByName(const String &db_name, const String &table_name) {
    Error<TransactionException>("Not Implemented", __FILE_NAME__, __LINE__);
}

EntryResult Txn::CreateView(const String &db_name, const String &view_name, ConflictType conflict_type) {
    Error<TransactionException>("Not Implemented", __FILE_NAME__, __LINE__);
}

EntryResult Txn::DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type) {
    Error<TransactionException>("Not Implemented", __FILE_NAME__, __LINE__);
}

EntryResult Txn::GetViewByName(const String &db_name, const String &view_name) {
    Error<TransactionException>("Not Implemented", __FILE_NAME__, __LINE__);
}

Vector<BaseEntry *> Txn::GetViews(const String &db_name) {
    Error<TransactionException>("Not Implemented", __FILE_NAME__, __LINE__);
}

void Txn::BeginTxn() { txn_context_.BeginCommit(txn_mgr_->GetTimestamp()); }

void Txn::CommitTxn() {
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
    UniqueLock<Mutex> lk(m);
    cv.wait(lk, [this] { return done_bottom_; });
}

void Txn::CommitTxnBottom() {

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
    for (auto *index_def_entry : txn_indexes_) {
        index_def_entry->Commit(commit_ts);
    }
    LOG_TRACE(Format("Txn: {} is committed.", txn_id_));

    // Notify the top half
    UniqueLock<Mutex> lk(m);
    done_bottom_ = true;
    cv.notify_one();
}

void Txn::CancelCommitTxnBottom() {
    txn_context_.SetTxnRollbacked();
    UniqueLock<Mutex> lk(m);
    done_bottom_ = true;
    cv.notify_one();
}

// Dangerous! only used during replaying wal.
void Txn::FakeCommit(TxnTimeStamp commit_ts) {
    txn_context_.begin_ts_ = commit_ts;
    txn_context_.commit_ts_ = commit_ts;
    txn_context_.state_ = TxnState::kCommitted;
}

void Txn::RollbackTxn() {
    TxnTimeStamp abort_ts = txn_mgr_->GetTimestamp();
    txn_context_.SetTxnRollbacking(abort_ts);

    for (const auto &base_entry : txn_tables_) {
        auto *table_entry = (TableCollectionEntry *)(base_entry);
        TableCollectionMeta *table_meta = TableCollectionEntry::GetTableMeta(table_entry);
        DBEntry *db_entry = TableCollectionEntry::GetDBEntry(table_entry);
        DBEntry::RemoveTableCollectionEntry(db_entry, *table_meta->table_collection_name_, txn_id_, txn_mgr_);
    }

    for (const auto &index_def_entry : txn_indexes_) {
        IndexDefMeta *index_def_meta = index_def_entry->index_def_meta_;
        TableCollectionEntry *table_entry = index_def_meta->table_collection_entry_;
        TableCollectionEntry::RemoveIndexEntry(table_entry, index_def_meta->index_name_, txn_id_, txn_mgr_);
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

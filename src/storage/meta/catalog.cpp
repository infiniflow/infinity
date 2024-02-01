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

#include <fstream>
#include <vector>
module catalog;

import stl;
import parser;
import txn_manager;
import logger;
import third_party;
import status;
import infinity_exception;
import function_set;
import table_function;
import special_function;
import buffer_manager;

import local_file_system;
import file_system_type;
import file_system;
import table_def;
import table_entry_type;
import table_detail;
import index_def;
import txn_store;
import data_access_state;
import catalog_delta_entry;
import file_writer;

namespace infinity {

// TODO Consider letting it commit as a transaction.
NewCatalog::NewCatalog(SharedPtr<String> dir, bool create_default_db) : current_dir_(std::move(dir)) {
    if (create_default_db) {
        // db current dir is same level as catalog
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto data_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir(), db_meta->db_name(), 0, 0);
        // TODO commit ts == 0 is true??
        db_entry->commit_ts_ = 0;
        DBMeta::AddEntry(db_meta.get(), std::move(db_entry));

        this->rw_locker_.lock();
        this->databases_["default"] = std::move(db_meta);
        this->rw_locker_.unlock();
    }
}

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
Tuple<DBEntry *, Status>
NewCatalog::CreateDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {

    // Check if there is db_meta with the db_name
    DBMeta *db_meta{nullptr};

    this->rw_locker_.lock_shared();
    auto db_iter = this->databases_.find(db_name);
    if (db_iter != this->databases_.end()) {
        // Find the db
        db_meta = db_iter->second.get();
        this->rw_locker_.unlock_shared();
    } else {
        this->rw_locker_.unlock_shared();

        LOG_TRACE(fmt::format("Create new database: {}", db_name));
        // Not find the db and create new db meta
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto db_dir = MakeShared<String>(parent_path.string());
        // Physical wal log
        UniquePtr<DBMeta> new_db_meta = DBMeta::NewDBMeta(db_dir, MakeShared<String>(db_name));

        // When replay database creation,if the txn_manger is nullptr, represent the txn manager not running, it is reasonable.
        // In replay phase, not need to recording the catalog delta operation.
        if (txn_mgr != nullptr) {
            auto operation = MakeUnique<AddDBMetaOp>(new_db_meta.get(), begin_ts);
            txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
        }

        db_meta = new_db_meta.get();
        this->rw_locker_.lock();
        auto db_iter2 = this->databases_.find(db_name);
        if (db_iter2 == this->databases_.end()) {
            this->databases_[db_name] = std::move(new_db_meta);
        } else {
            db_meta = db_iter2->second.get();
        }
        this->rw_locker_.unlock();
    }

    LOG_TRACE(fmt::format("Adding new database entry: {}", db_name));
    return db_meta->CreateNewEntry(txn_id, begin_ts, txn_mgr, conflict_type);
}

// do not only use this method to drop database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::DropDatabase instead
Tuple<DBEntry *, Status> NewCatalog::DropDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    this->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (this->databases_.find(db_name) != this->databases_.end()) {
        db_meta = this->databases_[db_name].get();
    }
    this->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Attempt to drop not existed database entry {}", db_name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kDBNotExist, std::move(err_msg))};
    }

    LOG_TRACE(fmt::format("Drop a database entry {}", db_name));
    return db_meta->DropNewEntry(txn_id, begin_ts, txn_mgr);
}

Tuple<DBEntry *, Status> NewCatalog::GetDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {

    DBMeta *db_meta{nullptr};
    this->rw_locker_.lock_shared();
    auto iter = this->databases_.find(db_name);
    if (iter != this->databases_.end()) {
        db_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Attempt to get not existed database {}", db_name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kDBNotExist, std::move(err_msg))};
    }
    return db_meta->GetEntry(txn_id, begin_ts);
}

void NewCatalog::RemoveDBEntry(const String &db_name, TransactionID txn_id, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (this->databases_.find(db_name) != this->databases_.end()) {
        db_meta = this->databases_[db_name].get();
    }
    this->rw_locker_.unlock_shared();

    LOG_TRACE(fmt::format("Remove a database entry {}", db_name));
    db_meta->DeleteNewEntry(txn_id, txn_mgr);
}

Vector<DBEntry *> NewCatalog::Databases(TransactionID txn_id, TxnTimeStamp begin_ts) {
    this->rw_locker_.lock_shared();

    Vector<DBEntry *> res;
    res.reserve(this->databases_.size());
    for (const auto &db_meta_pair : this->databases_) {
        DBMeta *db_meta = db_meta_pair.second.get();
        auto [db_entry, status] = db_meta->GetEntry(txn_id, begin_ts);
        if (status.ok()) {
            res.emplace_back(db_entry);
        }
    }
    this->rw_locker_.unlock_shared();
    return res;
}

Tuple<TableEntry *, Status> NewCatalog::CreateTable(const String &db_name,
                                                    TransactionID txn_id,
                                                    TxnTimeStamp begin_ts,
                                                    const SharedPtr<TableDef> &table_def,
                                                    ConflictType conflict_type,
                                                    TxnManager *txn_mgr) {
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry->CreateTable(TableEntryType::kTableEntry, table_def->table_name(), table_def->columns(), txn_id, begin_ts, txn_mgr);
}

Tuple<TableEntry *, Status> NewCatalog::DropTableByName(const String &db_name,
                                                        const String &table_name,
                                                        ConflictType conflict_type,
                                                        TransactionID txn_id,
                                                        TxnTimeStamp begin_ts,
                                                        TxnManager *txn_mgr) {
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }
    return db_entry->DropTable(table_name, conflict_type, txn_id, begin_ts, txn_mgr);
}

Status NewCatalog::GetTables(const String &db_name, Vector<TableDetail> &output_table_array, TransactionID txn_id, TxnTimeStamp begin_ts) {
    // Check the db entries
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return status;
    }
    return db_entry->GetTablesDetail(txn_id, begin_ts, output_table_array);
}

Tuple<TableEntry *, Status> NewCatalog::GetTableByName(const String &db_name, const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    // Check the db entries
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry->GetTableCollection(table_name, txn_id, begin_ts);
}

Status NewCatalog::RemoveTableEntry(TableEntry *table_entry, TransactionID txn_id, TxnManager *txn_mgr) {
    TableMeta *table_meta = table_entry->GetTableMeta();
    LOG_TRACE(fmt::format("Remove a table/collection entry: {}", *table_entry->GetTableName()));
    table_meta->DeleteNewEntry(txn_id, txn_mgr);

    return Status::OK();
}

Tuple<TableIndexEntry *, Status> NewCatalog::CreateIndex(TableEntry *table_entry,
                                                         const SharedPtr<IndexDef> &index_def,
                                                         ConflictType conflict_type,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         TxnManager *txn_mgr,
                                                         bool is_replay,
                                                         String replay_table_index_dir) {

    return table_entry->CreateIndex(index_def, conflict_type, txn_id, begin_ts, txn_mgr, is_replay, replay_table_index_dir);
}

Tuple<TableIndexEntry *, Status> NewCatalog::DropIndex(const String &db_name,
                                                       const String &table_name,
                                                       const String &index_name,
                                                       ConflictType conflict_type,
                                                       TransactionID txn_id,
                                                       TxnTimeStamp begin_ts,
                                                       TxnManager *txn_mgr) {
    auto [table_entry, table_status] = GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        LOG_ERROR(fmt::format("Database: {}, Table: {} is invalid", db_name, table_name));
        return {nullptr, table_status};
    }

    return table_entry->DropIndex(index_name, conflict_type, txn_id, begin_ts, txn_mgr);
}

Status NewCatalog::RemoveIndexEntry(const String &index_name, TableIndexEntry *table_index_entry, TransactionID txn_id, TxnManager *txn_mgr) {
    const TableIndexMeta *table_index_meta = table_index_entry->table_index_meta();
    TableEntry *table_entry = table_index_meta->GetTableEntry();
    table_entry->RemoveIndexEntry(index_name, txn_id, txn_mgr);
    return Status::OK();
}

void NewCatalog::CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_, bool is_replay) {
    return TableEntry::CommitCreateIndex(txn_indexes_store_, is_replay);
}

void NewCatalog::Append(TableEntry *table_entry, TransactionID txn_id, void *txn_store, BufferManager *buffer_mgr) {
    return table_entry->Append(txn_id, txn_store, buffer_mgr);
}

void NewCatalog::CommitAppend(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const AppendState *append_state_ptr) {
    return table_entry->CommitAppend(txn_id, commit_ts, append_state_ptr);
}

void NewCatalog::RollbackAppend(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    return table_entry->RollbackAppend(txn_id, commit_ts, txn_store);
}

Status NewCatalog::Delete(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    return table_entry->Delete(txn_id, commit_ts, delete_state);
}

void NewCatalog::CommitDelete(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const DeleteState &append_state) {
    return table_entry->CommitDelete(txn_id, commit_ts, append_state);
}

Status NewCatalog::RollbackDelete(TableEntry *table_entry, TransactionID txn_id, DeleteState &append_state, BufferManager *buffer_mgr) {
    return table_entry->RollbackDelete(txn_id, append_state, buffer_mgr);
}

Status NewCatalog::CommitCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    return table_entry->CommitCompact(txn_id, commit_ts, compact_store);
}

Status NewCatalog::RollbackCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    return table_entry->RollbackCompact(txn_id, commit_ts, compact_store);
}

Status NewCatalog::CommitImport(TableEntry *table_entry, TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment) {
    return table_entry->CommitImport(commit_ts, segment);
}

SegmentID NewCatalog::GetNextSegmentID(TableEntry *table_entry) { return table_entry->GetNextSegmentID(); }

void NewCatalog::AddSegment(TableEntry *table_entry, SharedPtr<SegmentEntry> &segment_entry) {
    table_entry->segment_map_.emplace(segment_entry->segment_id(), std::move(segment_entry));
    // ATTENTION: focusing on the segment id
    table_entry->next_segment_id_++;
}

SharedPtr<FunctionSet> NewCatalog::GetFunctionSetByName(NewCatalog *catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if (!catalog->function_sets_.contains(function_name)) {
        RecoverableError(Status::FunctionNotFound(function_name));
    }
    return catalog->function_sets_[function_name];
}

void NewCatalog::AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        UnrecoverableError(fmt::format("Trying to add duplicated function table_name into catalog: {}", name));
    }
    catalog->function_sets_.emplace(name, function_set);
}

// Table Function related methods
SharedPtr<TableFunction> NewCatalog::GetTableFunctionByName(NewCatalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->table_functions_.contains(function_name)) {
        RecoverableError(Status::FunctionNotFound(function_name));
    }
    return catalog->table_functions_[function_name];
}

void NewCatalog::AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function) {
    String name = table_function->name();
    StringToLower(name);
    if (catalog->table_functions_.contains(name)) {
        UnrecoverableError(fmt::format("Trying to add duplicated table function into catalog: {}", name));
    }
    catalog->table_functions_.emplace(name, table_function);
}

void NewCatalog::AddSpecialFunction(NewCatalog *catalog, const SharedPtr<SpecialFunction> &special_function) {
    String name = special_function->name();
    StringToLower(name);
    if (catalog->table_functions_.contains(name)) {
        UnrecoverableError(fmt::format("Trying to add duplicated special function into catalog: {}", name));
    }
    catalog->special_functions_.emplace(name, special_function);
}

Tuple<SpecialFunction *, Status> NewCatalog::GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->special_functions_.contains(function_name)) {
        return {nullptr, Status::SpecialFunctionNotFound()};
    }
    return {catalog->special_functions_[function_name].get(), Status::OK()};
}

nlohmann::json NewCatalog::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<DBMeta *> databases;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["current_dir"] = *this->current_dir_;
        json_res["next_txn_id"] = this->next_txn_id_;
        json_res["catalog_version"] = this->catalog_version_;
        databases.reserve(this->databases_.size());
        for (auto &db_meta : this->databases_) {
            databases.push_back(db_meta.second.get());
        }
    }

    for (auto &db_meta : databases) {
        json_res["databases"].emplace_back(db_meta->DBMeta::Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<NewCatalog> NewCatalog::LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr) {
    auto catalog = MakeUnique<NewCatalog>(nullptr);
    if (catalog_paths.empty()) {
        UnrecoverableError(fmt::format("Catalog path is empty"));
    }
    // 1. load json
    // 2. load entries
    LOG_INFO(fmt::format("Load base FULL catalog json from: {}", catalog_paths[0]));
    catalog = NewCatalog::LoadFromFile(catalog_paths[0], buffer_mgr);

    // Load catalogs delta checkpoints and merge.
    for (SizeT i = 1; i < catalog_paths.size(); i++) {
        LOG_INFO(fmt::format("Load catalog DELTA entry binary from: {}", catalog_paths[i]));
        NewCatalog::LoadFromEntry(catalog.get(), catalog_paths[i], buffer_mgr);
    }

    LOG_TRACE(fmt::format("Catalog Delta Op is done"));

    return catalog;
}

void NewCatalog::LoadFromEntry(NewCatalog *catalog, const String &catalog_path, BufferManager *buffer_mgr) {
    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    i32 file_size = fs.GetFileSize(*catalog_file_handler);
    Vector<char> buf(file_size);
    fs.Read(*catalog_file_handler, buf.data(), file_size);
    fs.Close(*catalog_file_handler);
    char *ptr = buf.data();
    auto catalog_delta_entry = CatalogDeltaEntry::ReadAdv(ptr, file_size);
    i32 n_bytes = catalog_delta_entry->GetSizeInBytes();
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }

    auto global_commit_ts = catalog_delta_entry->commit_ts();
    auto global_txn_id = catalog_delta_entry->txn_id();
    auto &operations = catalog_delta_entry->operations();
    for (auto &op : operations) {
        auto type = op->GetType();
        LOG_TRACE(fmt::format("Catalog Delta Op is {}", op->ToString()));
        auto commit_ts = op->commit_ts();
        auto txn_id = op->txn_id();
        auto begin_ts = op->begin_ts();
        auto is_delete = op->is_delete();
        if (txn_id < catalog->next_txn_id_) {
            // Ignore the old txn
            continue;
        }
        switch (type) {
            case CatalogDeltaOpType::ADD_DATABASE_META: {
                auto add_db_meta_op = static_cast<AddDBMetaOp *>(op.get());
                auto db_dir = add_db_meta_op->data_dir();
                auto db_name = add_db_meta_op->db_name();
                UniquePtr<DBMeta> new_db_meta = DBMeta::NewDBMeta(MakeShared<String>(db_dir), MakeShared<String>(db_name));

                catalog->databases_.insert({db_name, std::move(new_db_meta)});
                break;
            }
            case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
                auto add_db_entry_op = static_cast<AddDBEntryOp *>(op.get());
                auto db_name = add_db_entry_op->db_name();

                auto db_meta = catalog->databases_.at(db_name).get();
                auto db_entry = DBEntry::NewReplayDBEntry(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts, commit_ts, is_delete);
                if (db_meta->entry_list_.empty()) {
                    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
                    db_meta->entry_list_.emplace_front(std::move(dummy_entry));
                }
                db_meta->entry_list_.emplace_front(std::move(db_entry));
                break;
            }
            case CatalogDeltaOpType::ADD_TABLE_META: {
                auto add_table_meta_op = static_cast<AddTableMetaOp *>(op.get());
                auto db_name = add_table_meta_op->db_name();
                auto table_name = add_table_meta_op->table_name();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = TableMeta::NewTableMeta(db_entry->db_entry_dir_, MakeShared<String>(table_name), db_entry);
                db_entry->tables_.insert({table_name, std::move(table_meta)});
                break;
            }
            case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
                auto add_table_entry_op = static_cast<AddTableEntryOp *>(op.get());
                auto db_name = add_table_entry_op->db_name();
                auto table_name = add_table_entry_op->table_name();
                auto table_entry_dir = add_table_entry_op->table_entry_dir();
                auto column_defs = add_table_entry_op->column_defs();
                auto entry_type = add_table_entry_op->table_entry_type();
                auto row_count = add_table_entry_op->row_count();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto table_entry = TableEntry::NewReplayTableEntry(table_meta,
                                                                   MakeUnique<String>(table_entry_dir),
                                                                   MakeUnique<String>(table_name),
                                                                   column_defs,
                                                                   entry_type,
                                                                   txn_id,
                                                                   begin_ts,
                                                                   commit_ts,
                                                                   is_delete,
                                                                   row_count);
                if (table_meta->entry_list_.empty()) {
                    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
                    table_meta->entry_list_.emplace_front(std::move(dummy_entry));
                }
                table_meta->entry_list_.emplace_front(std::move(table_entry));
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
                auto add_segment_entry_op = static_cast<AddSegmentEntryOp *>(op.get());
                auto db_name = add_segment_entry_op->db_name();
                auto table_name = add_segment_entry_op->table_name();
                auto segment_id = add_segment_entry_op->segment_id();
                auto segment_dir = add_segment_entry_op->segment_dir();
                auto column_count = add_segment_entry_op->column_count();
                auto row_count = add_segment_entry_op->row_count();
                auto actual_row_count = add_segment_entry_op->actual_row_count();
                auto row_capacity = add_segment_entry_op->row_capacity();
                auto min_row_ts = add_segment_entry_op->min_row_ts();
                auto max_row_ts = add_segment_entry_op->max_row_ts();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto segment_entry = SegmentEntry::NewReplayCatalogSegmentEntry(table_entry,
                                                                                segment_id,
                                                                                MakeUnique<String>(segment_dir),
                                                                                column_count,
                                                                                row_count,
                                                                                actual_row_count,
                                                                                row_capacity,
                                                                                min_row_ts,
                                                                                max_row_ts,
                                                                                commit_ts,
                                                                                begin_ts,
                                                                                txn_id);

                table_entry->segment_map_.insert({segment_id, std::move(segment_entry)});
                break;
            }
            case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                auto add_block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                auto db_name = add_block_entry_op->db_name();
                auto table_name = add_block_entry_op->table_name();
                auto segment_id = add_block_entry_op->segment_id();
                auto block_id = add_block_entry_op->block_id();
                auto block_dir = add_block_entry_op->block_dir();
                auto row_count = add_block_entry_op->row_count();
                auto row_capacity = add_block_entry_op->row_capacity();
                auto min_row_ts = add_block_entry_op->min_row_ts();
                auto max_row_ts = add_block_entry_op->max_row_ts();
                auto check_point_ts = add_block_entry_op->checkpoint_ts();
                auto check_point_row_count = add_block_entry_op->checkpoint_row_count();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto segment_entry = table_entry->segment_map_.at(segment_id).get();
                auto block_entry = BlockEntry::NewReplayCatalogBlockEntry(segment_entry,
                                                                          block_id,
                                                                          row_count,
                                                                          row_capacity,
                                                                          min_row_ts,
                                                                          max_row_ts,
                                                                          check_point_ts,
                                                                          check_point_row_count,
                                                                          buffer_mgr);
                segment_entry->block_entries().push_back(std::move(block_entry));
                break;
            }
            case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
                auto add_column_entry_op = static_cast<AddColumnEntryOp *>(op.get());
                auto db_name = add_column_entry_op->db_name();
                auto table_name = add_column_entry_op->table_name();
                auto segment_id = add_column_entry_op->segment_id();
                auto block_id = add_column_entry_op->block_id();
                auto column_id = add_column_entry_op->column_id();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto segment_entry = table_entry->segment_map_.at(segment_id).get();
                auto block_entry = segment_entry->GetBlockEntryByID(block_id);
                auto column_entry = BlockColumnEntry::NewReplayBlockColumnEntry(block_entry, column_id, buffer_mgr);
                block_entry->columns().push_back(std::move(column_entry));
                break;
            }

            case CatalogDeltaOpType::ADD_INDEX_META: {
                auto add_index_meta_op = static_cast<AddIndexMetaOp *>(op.get());
                String db_name = add_index_meta_op->db_name();
                auto table_name = add_index_meta_op->table_name();
                auto index_name = add_index_meta_op->index_name();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto index_meta = TableIndexMeta::NewTableIndexMeta(table_entry, MakeShared<String>(index_name));
                table_entry->index_meta_map_.insert({index_name, std::move(index_meta)});
                break;
            }
            case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
                auto add_table_index_entry_op = static_cast<AddTableIndexEntryOp *>(op.get());
                auto db_name = add_table_index_entry_op->db_name();
                auto table_name = add_table_index_entry_op->table_name();
                auto index_name = add_table_index_entry_op->index_name();
                auto index_dir = add_table_index_entry_op->index_dir();
                auto index_def = add_table_index_entry_op->index_def();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto index_meta = table_entry->index_meta_map_.at(index_name).get();
                auto table_index_entry = TableIndexEntry::NewReplayTableIndexEntry(index_meta,
                                                                                   index_def,
                                                                                   MakeUnique<String>(index_dir),
                                                                                   txn_id,
                                                                                   begin_ts,
                                                                                   commit_ts,
                                                                                   is_delete);
                if (index_meta->entry_list().empty()) {
                    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
                    index_meta->entry_list().emplace_front(std::move(dummy_entry));
                }
                index_meta->entry_list().emplace_front(std::move(table_index_entry));
                break;
            }
            case CatalogDeltaOpType::ADD_IRS_INDEX_ENTRY: {
                auto add_irs_index_entry_op = static_cast<AddIrsIndexEntryOp *>(op.get());
                auto db_name = add_irs_index_entry_op->db_name();
                auto table_name = add_irs_index_entry_op->table_name();
                auto index_name = add_irs_index_entry_op->index_name();
                auto index_dir = add_irs_index_entry_op->index_dir();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto *index_meta = table_entry->index_meta_map_.at(index_name).get();
                auto [table_index_entry, index_status] = index_meta->GetEntryReplay(txn_id, begin_ts);
                if (!index_status.ok()) {
                    UnrecoverableError(index_status.message());
                }
                auto irs_index_entry =
                    IrsIndexEntry::NewReplayIrsIndexEntry(table_index_entry, MakeUnique<String>(index_dir), txn_id, begin_ts, commit_ts, is_delete);
                table_index_entry->irs_index_entry() = std::move(irs_index_entry);
                break;
            }
            case CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY: {
                auto add_column_index_entry_op = static_cast<AddColumnIndexEntryOp *>(op.get());
                auto db_name = add_column_index_entry_op->db_name();
                auto table_name = add_column_index_entry_op->table_name();
                auto index_name = add_column_index_entry_op->index_name();
                auto column_id = add_column_index_entry_op->column_id();
                auto index_base = add_column_index_entry_op->index_base();
                auto column_index_dir = add_column_index_entry_op->col_index_dir();

                auto *db_meta = catalog->databases_.at(db_name).get();
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto *index_meta = table_entry->index_meta_map_.at(index_name).get();
                auto [table_index_entry, index_status] = index_meta->GetEntryReplay(txn_id, begin_ts);
                if (!index_status.ok()) {
                    UnrecoverableError(index_status.message());
                }
                auto column_index_entry = ColumnIndexEntry::NewReplayColumnIndexEntry(table_index_entry,
                                                                                      index_base,
                                                                                      column_id,
                                                                                      MakeUnique<String>(column_index_dir),
                                                                                      txn_id,
                                                                                      begin_ts,
                                                                                      commit_ts,
                                                                                      is_delete);
                table_index_entry->column_index_map().insert({column_id, std::move(column_index_entry)});
                break;
            }

            case CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY: {
                auto add_segment_column_index_entry_op = static_cast<AddSegmentColumnIndexEntryOp *>(op.get());
                auto db_name = add_segment_column_index_entry_op->db_name();
                auto table_name = add_segment_column_index_entry_op->table_name();
                auto index_name = add_segment_column_index_entry_op->index_name();
                auto segment_id = add_segment_column_index_entry_op->segment_id();
                auto min_ts = add_segment_column_index_entry_op->min_ts();
                auto max_ts = add_segment_column_index_entry_op->max_ts();
                auto column_id = add_segment_column_index_entry_op->column_id();

                auto *db_meta = catalog->databases_.at(db_name).get();
                LOG_TRACE(fmt::format("at db {}", db_name));
                auto [db_entry, db_status] = db_meta->GetEntryReplay(txn_id, begin_ts);
                if (!db_status.ok()) {
                    UnrecoverableError(db_status.message());
                }
                auto table_meta = db_entry->tables_.at(table_name).get();
                auto [table_entry, tb_status] = table_meta->GetEntryReplay(txn_id, begin_ts);
                if (!tb_status.ok()) {
                    UnrecoverableError(tb_status.message());
                }
                auto *index_meta = table_entry->index_meta_map_.at(index_name).get();
                auto [table_index_entry, index_status] = index_meta->GetEntryReplay(txn_id, begin_ts);
                if (!index_status.ok()) {
                    UnrecoverableError(index_status.message());
                }
                auto *column_index_entry = table_index_entry->column_index_map().at(column_id).get();
                auto segment_column_index_entry = SegmentColumnIndexEntry::NewReplaySegmentIndexEntry(column_index_entry,
                                                                                                      table_entry,
                                                                                                      segment_id,
                                                                                                      buffer_mgr,
                                                                                                      min_ts,
                                                                                                      max_ts,
                                                                                                      txn_id,
                                                                                                      begin_ts,
                                                                                                      commit_ts,
                                                                                                      is_delete);
                column_index_entry->index_by_segment().insert({segment_id, std::move(segment_column_index_entry)});
                break;
            }
            default:
                UnrecoverableError(fmt::format("Unknown catalog delta op type: {}", op->GetTypeStr()));
        }
    }
}

void NewCatalog::MergeFrom(NewCatalog &other) {
    // Merge databases.
    for (auto &[db_name, db_meta2] : other.databases_) {
        auto it = this->databases_.find(db_name);
        if (it == this->databases_.end()) {
            this->databases_.emplace(db_name, std::move(db_meta2));
        } else {
            it->second->MergeFrom(*db_meta2.get());
        }
    }
}

UniquePtr<NewCatalog> NewCatalog::LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr) {
    UniquePtr<NewCatalog> catalog = nullptr;
    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    SizeT file_size = fs.GetFileSize(*catalog_file_handler);
    String json_str(file_size, 0);
    SizeT n_bytes = catalog_file_handler->Read(json_str.data(), file_size);
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }

    nlohmann::json catalog_json = nlohmann::json::parse(json_str);
    Deserialize(catalog_json, buffer_mgr, catalog);
    return catalog;
}

void NewCatalog::Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog) {
    SharedPtr<String> current_dir = MakeShared<String>(catalog_json["current_dir"]);

    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    catalog = MakeUnique<NewCatalog>(current_dir);
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->catalog_version_ = catalog_json["catalog_version"];
    if (catalog_json.contains("databases")) {
        for (const auto &db_json : catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->databases_.emplace(*db_meta->db_name(), std::move(db_meta));
        }
    }
}

void NewCatalog::SaveAsFile(const String &catalog_path, TxnTimeStamp max_commit_ts) {
    nlohmann::json catalog_json = Serialize(max_commit_ts, true);
    String catalog_str = catalog_json.dump();

    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;

    u8 fileflags = FileFlags::WRITE_FLAG;
    if (!fs.Exists(catalog_path)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }

    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT n_bytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if (n_bytes != catalog_str.size()) {
        LOG_ERROR(fmt::format("Saving catalog file failed: {}", catalog_path));
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();

    LOG_INFO(fmt::format("Saved catalog to: {}", catalog_path));
}

void NewCatalog::FlushGlobalCatalogDeltaEntry(const String &delta_catalog_path, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    LOG_INFO("FLUSH GLOBAL DELTA CATALOG ENTRY");

    auto const global_catalog_delta_entry = std::move(this->global_catalog_delta_entry_);
    this->global_catalog_delta_entry_ = MakeUnique<GlobalCatalogDeltaEntry>();

    // Assert catalog entry commit ts <= max_commit_ts
    auto global_entry_commit_ts = global_catalog_delta_entry->commit_ts();
    LOG_INFO(fmt::format("Global catalog delta entry commit ts:{}, checkpoint max commit ts:{}.", global_entry_commit_ts, max_commit_ts));
    if (global_entry_commit_ts > max_commit_ts) {
        UnrecoverableError(
            fmt::format("Global catalog delta entry commit ts {} not match checkpoint max commit ts {}", global_entry_commit_ts, max_commit_ts));
    }

    // Check the SegmentEntry's for flush the data to disk.
    auto &ops = global_catalog_delta_entry->operations();
    for (auto &op : ops) {
        switch (op->GetType()) {
            case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
                auto add_table_entry_op = static_cast<AddTableEntryOp *>(op.get());
                add_table_entry_op->SaveSate();
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
                auto add_segment_entry_op = static_cast<AddSegmentEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush segment entry: {}", add_segment_entry_op->ToString()));
                add_segment_entry_op->FlushDataToDisk(max_commit_ts, is_full_checkpoint);
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY: {
                auto add_segment_column_index_entry_op = static_cast<AddSegmentColumnIndexEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush segment index entry: {}", add_segment_column_index_entry_op->ToString()));
                add_segment_column_index_entry_op->Flush(max_commit_ts);
                break;
            }
            default:
                break;
        }
    }

    // Save the global catalog delta entry to disk.
    auto exp_size = global_catalog_delta_entry->GetSizeInBytes();
    Vector<char> buf(exp_size);
    char *ptr = buf.data();
    global_catalog_delta_entry->WriteAdv(ptr);
    i32 act_size = ptr - buf.data();
    if (exp_size != act_size) {
        UnrecoverableError(fmt::format("Flush global catalog delta entry failed, exp_size: {}, act_size: {}", exp_size, act_size));
    }

    std::ofstream outfile;
    outfile.open(delta_catalog_path, std::ios::binary);
    outfile.write((reinterpret_cast<const char *>(buf.data())), act_size);
    outfile.close();
}

} // namespace infinity

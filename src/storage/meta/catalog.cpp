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
import wal;

namespace infinity {

NewCatalog::NewCatalog(SharedPtr<String> dir, bool create_default_db) : current_dir_(std::move(dir)) {
    if (create_default_db) {
        // db current dir is same level as catalog
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto data_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir(), db_meta->db_name(), 0, 0);
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
            auto operation = MakeUnique<AddDBMetaOperation>(new_db_meta.get());
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

void NewCatalog::CreateIndexFile(TableEntry *table_entry,
                                 void *txn_store,
                                 TableIndexEntry *table_index_entry,
                                 TxnTimeStamp begin_ts,
                                 BufferManager *buffer_mgr,
                                 bool prepare,
                                 bool is_replay) {
    return table_entry->CreateIndexFile(txn_store, table_index_entry, begin_ts, buffer_mgr, prepare, is_replay);
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

Status NewCatalog::ImportSegment(TableEntry *table_entry, TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment) {
    return table_entry->ImportSegment(commit_ts, segment);
}

u32 NewCatalog::GetNextSegmentID(TableEntry *table_entry) { return TableEntry::GetNextSegmentID(table_entry); }

u32 NewCatalog::GetMaxSegmentID(const TableEntry *table_entry) { return TableEntry::GetMaxSegmentID(table_entry); }

void NewCatalog::ImportSegment(TableEntry *table_entry, u32 segment_id, SharedPtr<SegmentEntry> &segment_entry) {
    table_entry->segment_map_.emplace(segment_id, std::move(segment_entry));
    // ATTENTION: focusing on the segment id
    table_entry->next_segment_id_++;
}

void NewCatalog::IncreaseTableRowCount(TableEntry *table_entry, u64 increased_row_count) { table_entry->row_count_ += increased_row_count; }

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

// void NewCatalog::CheckCatalog() {
//     for (auto &[db_name, db_meta] : this->databases_) {
//         for (auto &base_entry : db_meta->entry_list()) {
//             if (base_entry->entry_type_ == EntryType::kDummy) {
//                 continue;
//             }
//             auto db_entry = static_cast<DBEntry *>(base_entry.get());
//             for (auto &[table_name, table_meta] : db_entry->tables_) {
//                 for (auto &base_entry : table_meta->entry_list_) {
//                     if (base_entry->entry_type_ == EntryType::kDummy) {
//                         continue;
//                     }
//                     auto table_entry = static_cast<TableEntry *>(base_entry.get());
//                     if (table_entry->table_entry_->db_entry_ != db_entry) {
//                         //                        int a = 1;
//                     }
//                 }
//             }
//         }
//     }
// }

UniquePtr<NewCatalog> NewCatalog::LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr) {
    auto catalog1 = MakeUnique<NewCatalog>(nullptr);
    if (catalog_paths.empty()) {
        UnrecoverableError(fmt::format("Catalog path is empty"));
    }
    // Load the latest full checkpoint.
    LOG_INFO(fmt::format("Load base catalog1 from: {}", catalog_paths[0]));
    catalog1 = NewCatalog::LoadFromFile(catalog_paths[0], buffer_mgr);

    // Load catalogs delta checkpoints and merge.
    for (SizeT i = 1; i < catalog_paths.size(); i++) {
        LOG_INFO(fmt::format("Load delta catalog1 from: {}", catalog_paths[i]));
        UniquePtr<NewCatalog> catalog2 = NewCatalog::LoadFromFile(catalog_paths[i], buffer_mgr);
        catalog1->MergeFrom(*catalog2);
    }

    return catalog1;
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
    SizeT nbytes = catalog_file_handler->Read(json_str.data(), file_size);
    if (file_size != nbytes) {
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

String NewCatalog::SaveAsFile(const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json catalog_json = Serialize(max_commit_ts, is_full_checkpoint);
    String catalog_str = catalog_json.dump();

    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;

    if (!fs.Exists(dir)) {
        fs.CreateDirectory(dir);
    }

    String file_name = fmt::format("META_{}", max_commit_ts);
    if (is_full_checkpoint)
        file_name += ".full.json";
    else
        file_name += ".delta.json";
    String file_path = fmt::format("{}/{}", dir, file_name);

    u8 fileflags = FileFlags::WRITE_FLAG;
    if (!fs.Exists(file_path)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }

    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(file_path, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT nbytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if (nbytes != catalog_str.size()) {
        LOG_ERROR(fmt::format("Saving catalog file failed: {}", file_path));
        RecoverableError(Status::CatalogCorrupted(file_path));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();

    LOG_INFO(fmt::format("Saved catalog to: {}", file_path));
    return file_path;
}

} // namespace infinity

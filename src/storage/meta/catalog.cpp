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

import stl;
import base_entry;
import txn_manager;
import db_meta;
import db_entry;
import logger;
import third_party;
import status;
import infinity_exception;
import function_set;
import table_function;
import buffer_manager;

import local_file_system;
import file_system_type;
import file_system;

import table_collection_entry;

module new_catalog;

namespace infinity {

NewCatalog::NewCatalog(SharedPtr<String> dir, bool create_default_db) : current_dir_(Move(dir)) {
    if (create_default_db) {
        // db current dir is same level as catalog
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto data_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir(), db_meta->db_name(), 0, 0);
        db_entry->commit_ts_ = 0;
        DBMeta::AddEntry(db_meta.get(), Move(db_entry));

        this->rw_locker_.lock();
        this->databases_["default"] = Move(db_meta);
        this->rw_locker_.unlock();
    }
}

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
Status
NewCatalog::CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry) {

    // Check if there is db_meta with the db_name
    DBMeta *db_meta{nullptr};

    catalog->rw_locker_.lock_shared();
    auto db_iter = catalog->databases_.find(db_name);
    if (db_iter != catalog->databases_.end()) {
        // Find the db
        db_meta = db_iter->second.get();
        catalog->rw_locker_.unlock_shared();
    } else {
        catalog->rw_locker_.unlock_shared();

        LOG_TRACE(Format("Create new database: {}", db_name));
        // Not find the db and create new db meta
        Path catalog_path(*catalog->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto db_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(db_dir, MakeShared<String>(db_name));
        db_meta = new_db_meta.get();

        catalog->rw_locker_.lock();
        auto db_iter2 = catalog->databases_.find(db_name);
        if(db_iter2 == catalog->databases_.end()) {
            catalog->databases_[db_name] = Move(new_db_meta);
        } else {
            db_meta = db_iter2->second.get();
        }
        catalog->rw_locker_.unlock();
    }

    LOG_TRACE(Format("Add new database entry: {}", db_name));
    return DBMeta::CreateNewEntry(db_meta, txn_id, begin_ts, txn_mgr, db_entry);
}

// do not only use this method to drop database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::DropDatabase instead
Status
NewCatalog::DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry) {

    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>(Format("Attempt to drop not existed database entry {}", db_name));
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    LOG_TRACE(Format("Drop a database entry {}", db_name));
    return DBMeta::DropNewEntry(db_meta, txn_id, begin_ts, txn_mgr, db_entry);
}

Status NewCatalog::GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&new_db_entry) {

    DBMeta *db_meta{nullptr};
    catalog->rw_locker_.lock_shared();
    auto iter = catalog->databases_.find(db_name);
    if (iter != catalog->databases_.end()) {
        db_meta = iter->second.get();
    }
    catalog->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>(Format("Attempt to get not existed database {}", db_name));
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }
    return DBMeta::GetEntry(db_meta, txn_id, begin_ts, new_db_entry);
}

void NewCatalog::RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr) {
    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    LOG_TRACE(Format("Remove a database entry {}", db_name));
    DBMeta::DeleteNewEntry(db_meta, txn_id, txn_mgr);
}

Vector<DBEntry *> NewCatalog::Databases(NewCatalog *catalog, u64 txn_id, TxnTimeStamp begin_ts) {
    catalog->rw_locker_.lock_shared();

    Vector<DBEntry *> res;
    res.reserve(catalog->databases_.size());
    for (const auto &db_meta : catalog->databases_) {
        BaseEntry *base_db_entry{nullptr};
        Status status = DBMeta::GetEntry(db_meta.second.get(), txn_id, begin_ts, base_db_entry);
        if (status.ok()) {
            res.emplace_back(static_cast<DBEntry *>(base_db_entry));
        }
    }
    catalog->rw_locker_.unlock_shared();
    return res;
}

SharedPtr<FunctionSet> NewCatalog::GetFunctionSetByName(NewCatalog *catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if (!catalog->function_sets_.contains(function_name)) {
        Error<CatalogException>(Format("No function name: {}", function_name));
    }
    return catalog->function_sets_[function_name];
}

void NewCatalog::AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        Error<CatalogException>(Format("Trying to add duplicated function table_name into catalog: {}", name));
    }
    catalog->function_sets_.emplace(name, function_set);
}

void NewCatalog::DeleteFunctionSet(NewCatalog *catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if (!catalog->function_sets_.contains(function_name)) {
        Error<CatalogException>(Format("Delete not exist function: {}", function_name));
    }
    catalog->function_sets_.erase(function_name);
}

// Table Function related methods
SharedPtr<TableFunction> NewCatalog::GetTableFunctionByName(NewCatalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->table_functions_.contains(function_name)) {
        Error<CatalogException>(Format("No table function table_name: {}", function_name));
    }
    return catalog->table_functions_[function_name];
}

void NewCatalog::AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function) {
    String name = table_function->name();
    StringToLower(name);
    if (catalog->table_functions_.contains(name)) {
        Error<CatalogException>(Format("Trying to add duplicated table function into catalog: {}", name));
    }
    catalog->table_functions_.emplace(name, table_function);
}

void NewCatalog::DeleteTableFunction(NewCatalog *catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if (!catalog->table_functions_.contains(function_name)) {
        Error<CatalogException>(Format("Delete not exist table function: {}", function_name));
    }
    catalog->table_functions_.erase(function_name);
}

Json NewCatalog::Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<DBMeta *> databases;
    {
        SharedLock<RWMutex> lck(catalog->rw_locker_);
        json_res["current_dir"] = *catalog->current_dir_;
        json_res["next_txn_id"] = catalog->next_txn_id_;
        json_res["catalog_version"] = catalog->catalog_version_;
        databases.reserve(catalog->databases_.size());
        for (auto &db_meta : catalog->databases_) {
            databases.push_back(db_meta.second.get());
        }
    }

    for (auto &db_meta : databases) {
        json_res["databases"].emplace_back(DBMeta::Serialize(db_meta, max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

void NewCatalog::CheckCatalog() {
    for (auto &[db_name, db_meta] : this->databases_) {
        for (auto &base_entry : db_meta->entry_list()) {
            if (base_entry->entry_type_ == EntryType::kDummy) {
                continue;
            }
            auto db_entry = static_cast<DBEntry *>(base_entry.get());
            for (auto &[table_name, table_meta] : db_entry->tables_) {
                for (auto &base_entry : table_meta->entry_list_) {
                    if (base_entry->entry_type_ == EntryType::kDummy) {
                        continue;
                    }
                    auto table_entry = static_cast<TableCollectionEntry *>(base_entry.get());
                    if (table_entry->table_collection_meta_->db_entry_ != db_entry) {
                        int a = 1;
                    }
                }
            }
        }
    }
}

UniquePtr<NewCatalog> NewCatalog::LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr) {
    auto catalog1 = MakeUnique<NewCatalog>(nullptr);
    Assert<CatalogException>(!catalog_paths.empty(), "Catalog paths is empty");
    // Load the latest full checkpoint.
    LOG_INFO(Format("Load base catalog1 from: {}", catalog_paths[0]));
    catalog1 = NewCatalog::LoadFromFile(catalog_paths[0], buffer_mgr);

    // Load catalogs delta checkpoints and merge.
    for (int i = 1; i < catalog_paths.size(); i++) {
        LOG_INFO(Format("Load delta catalog1 from: {}", catalog_paths[i]));
        UniquePtr<NewCatalog> catalog2 = NewCatalog::LoadFromFile(catalog_paths[i], buffer_mgr);
        catalog1->MergeFrom(*catalog2);
    }
    // catalog1->CheckCatalog();
    return catalog1;
}

void NewCatalog::MergeFrom(NewCatalog &other) {
    // Merge databases.
    for (auto &[db_name, db_meta2] : other.databases_) {
        auto it = this->databases_.find(db_name);
        if (it == this->databases_.end()) {
            this->databases_.emplace(db_name, Move(db_meta2));
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
        Error<StorageException>(Format("Catalog file {}, read error.", catalog_path));
    }

    Json catalog_json = Json::parse(json_str);
    Deserialize(catalog_json, buffer_mgr, catalog);
    return catalog;
}

void NewCatalog::Deserialize(const Json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog) {
    SharedPtr<String> current_dir = MakeShared<String>(catalog_json["current_dir"]);

    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    catalog = MakeUnique<NewCatalog>(current_dir);
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->catalog_version_ = catalog_json["catalog_version"];
    if (catalog_json.contains("databases")) {
        for (const auto &db_json : catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->databases_.emplace(*db_meta->db_name(), Move(db_meta));
        }
    }
}

String NewCatalog::SaveAsFile(NewCatalog *catalog_ptr, const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json catalog_json = Serialize(catalog_ptr, max_commit_ts, is_full_checkpoint);
    String catalog_str = catalog_json.dump();

    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;

    if (!fs.Exists(dir)) {
        fs.CreateDirectory(dir);
    }

    String file_name = Format("META_{}", max_commit_ts);
    if (is_full_checkpoint)
        file_name += ".full.json";
    else
        file_name += ".delta.json";
    String file_path = Format("{}/{}", dir, file_name);

    u8 fileflags = FileFlags::WRITE_FLAG;
    if (!fs.Exists(file_path)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }

    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(file_path, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT nbytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if (nbytes != catalog_str.size()) {
        Error<StorageException>(Format("Catalog file {}, saving error.", file_path));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();

    LOG_INFO(Format("Saved catalog to: {}", file_path));
    return file_path;
}

} // namespace infinity

//
// Created by jinhai on 23-6-4.
//

#include "storage/meta/catalog.h"
#include "function/function_set.h"
#include "function/table_function.h"
#include "main/logger.h"
#include "storage/io/local_file_system.h"

namespace infinity {

NewCatalog::NewCatalog(SharedPtr<String> dir, bool create_default_db) : current_dir_(std::move(dir)) {
    if (create_default_db) {
        // db current dir is same level as catalog
        std::filesystem::path catalog_path(*this->current_dir_);
        std::filesystem::path parent_path = catalog_path.parent_path();
        auto data_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, 0, 0);
        db_entry->commit_ts_ = 0;
        db_meta->entry_list_.emplace_front(std::move(db_entry));
        this->databases_["default"] = std::move(db_meta);
    }
}

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
EntryResult NewCatalog::CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    // Check if there is db_meta with the db_name
    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    // no db_meta
    if (db_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new database: {}", db_name);
        // db current dir is same level as catalog
        std::filesystem::path catalog_path(*catalog->current_dir_);
        std::filesystem::path parent_path = catalog_path.parent_path();
        auto db_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(db_dir, MakeShared<String>(db_name));
        db_meta = new_db_meta.get();

        catalog->rw_locker_.lock();
        catalog->databases_[db_name] = std::move(new_db_meta);
        catalog->rw_locker_.unlock();
    }

    LOG_TRACE("Add new database entry: {}", db_name);
    EntryResult res = DBMeta::CreateNewEntry(db_meta, txn_id, begin_ts, txn_mgr);

    return res;
}

// do not only use this method to drop database
// it will not record database in transaction, so when you commit transaction
// it will lost operation
// use Txn::DropDatabase instead
EntryResult NewCatalog::DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed database entry {}", db_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed database entry")};
    }

    LOG_TRACE("Drop a database entry {}", db_name);
    EntryResult res = DBMeta::DropNewEntry(db_meta, txn_id, begin_ts, txn_mgr);

    return res;
}

EntryResult NewCatalog::GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts) {
    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();
    if (db_meta == nullptr) {
        LOG_ERROR("Attempt to get not existed database {}", db_name);
        return {nullptr, MakeUnique<String>("Attempt to get not existed database")};
    }
    return DBMeta::GetEntry(db_meta, txn_id, begin_ts);
}

void NewCatalog::RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr) {
    catalog->rw_locker_.lock_shared();

    DBMeta *db_meta{nullptr};
    if (catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    LOG_TRACE("Remove a database entry {}", db_name);
    DBMeta::DeleteNewEntry(db_meta, txn_id, txn_mgr);
}

SharedPtr<FunctionSet> NewCatalog::GetFunctionSetByName(NewCatalog *catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if (!catalog->function_sets_.contains(function_name)) {
        CatalogError("No function name: " + function_name);
    }
    return catalog->function_sets_[function_name];
}

void NewCatalog::AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        CatalogError("Trying to add duplicated function table_name into catalog: " + name);
    }
    catalog->function_sets_.emplace(name, function_set);
}

void NewCatalog::DeleteFunctionSet(NewCatalog *catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if (!catalog->function_sets_.contains(function_name)) {
        CatalogError("Delete not exist function: " + function_name);
    }
    catalog->function_sets_.erase(function_name);
}

// Table Function related methods
SharedPtr<TableFunction> NewCatalog::GetTableFunctionByName(NewCatalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->table_functions_.contains(function_name)) {
        CatalogError("No table function table_name: " + function_name);
    }
    return catalog->table_functions_[function_name];
}

void NewCatalog::AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function) {
    String name = table_function->name();
    StringToLower(name);
    if (catalog->table_functions_.contains(name)) {
        CatalogError("Trying to add duplicated table function into catalog: " + name);
    }
    catalog->table_functions_.emplace(name, table_function);
}

void NewCatalog::DeleteTableFunction(NewCatalog *catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if (!catalog->table_functions_.contains(function_name)) {
        CatalogError("Delete not exist table function: " + function_name);
    }
    catalog->table_functions_.erase(function_name);
}

nlohmann::json NewCatalog::Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<DBMeta *> databases;
    {
        std::shared_lock<std::shared_mutex> lck(catalog->rw_locker_);
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

UniquePtr<NewCatalog> NewCatalog::LoadFromFile(const SharedPtr<DirEntry> &dir_entry, BufferManager *buffer_mgr) {
    UniquePtr<NewCatalog> catalog = nullptr;
    String filename = dir_entry->path();

    LOG_INFO("Load catalog from: {}", filename);

    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(filename, FileFlags::READ_FLAG, FileLockType::kReadLock);

    SizeT file_size = dir_entry->file_size();
    String json_str(file_size, 0);
    SizeT nbytes = catalog_file_handler->Read(json_str.data(), file_size);
    if (file_size != nbytes) {
        StorageError(fmt::format("Catalog file {}, read error.", filename));
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
            catalog->databases_.emplace(*db_meta->db_name_, std::move(db_meta));
        }
    }
}

void NewCatalog::SaveAsFile(NewCatalog *catalog_ptr,
                            TxnTimeStamp max_commit_ts,
                            const String &dir,
                            const String &file_name,
                            bool is_full_checkpoint) {
    nlohmann::json catalog_json = Serialize(catalog_ptr, max_commit_ts, is_full_checkpoint);
    String catalog_str = catalog_json.dump();

    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;

    if (!fs.Exists(dir)) {
        fs.CreateDirectory(dir);
    }

    String file_path = dir + '/' + file_name;

    u8 fileflags = FileFlags::WRITE_FLAG;
    if (!fs.Exists(file_path)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }

    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(file_path, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT nbytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if (nbytes != catalog_str.size()) {
        StorageError(fmt::format("Catalog file {}, saving error.", file_path));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();

    LOG_INFO("Saved catalog to: {}", file_path);
}

} // namespace infinity

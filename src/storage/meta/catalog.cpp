//
// Created by jinhai on 23-6-4.
//

#include "catalog.h"
#include "main/logger.h"
#include "storage/io/local_file_system.h"
#include "function/function_set.h"
#include "function/table_function.h"

namespace infinity {

NewCatalog::NewCatalog(SharedPtr<String> dir)
    : current_dir_(std::move(dir)) {
}

EntryResult
NewCatalog::CreateDatabase(NewCatalog* catalog,
                           const String& db_name,
                           u64 txn_id,
                           TxnTimeStamp begin_ts,
                           TxnManager* txn_mgr) {

    // Check if there is db_meta with the db_name
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    // no db_meta
    if(db_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new database: {}", db_name);
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(catalog->current_dir_,
                                                           MakeShared<String>(db_name));
        db_meta = new_db_meta.get();

        catalog->rw_locker_.lock();
        catalog->databases_[db_name] = std::move(new_db_meta);
        catalog->rw_locker_.unlock();

    }

    LOG_TRACE("Add new database entry: {}", db_name);
    EntryResult res = DBMeta::CreateNewEntry(db_meta, txn_id, begin_ts, txn_mgr);

    return res;
}

EntryResult
NewCatalog::DropDatabase(NewCatalog* catalog,
                         const String& db_name,
                         u64 txn_id,
                         TxnTimeStamp begin_ts,
                         TxnManager* txn_mgr) {

    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();
    if(db_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed database entry {}", db_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed database entry")};
    }

    LOG_TRACE("Drop a database entry {}", db_name);
    EntryResult res = DBMeta::DropNewEntry(db_meta, txn_id, begin_ts, txn_mgr);

    return res;

}

EntryResult
NewCatalog::GetDatabase(NewCatalog* catalog,
                        const String& db_name,
                        u64 txn_id,
                        TxnTimeStamp begin_ts) {
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

//    LOG_TRACE("Get a database entry {}", db_name);
    return DBMeta::GetEntry(db_meta, txn_id, begin_ts);
}

void
NewCatalog::RemoveDBEntry(NewCatalog* catalog,
                          const String& db_name,
                          u64 txn_id,
                          TxnManager* txn_mgr) {
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    LOG_TRACE("Remove a database entry {}", db_name);
    DBMeta::DeleteNewEntry(db_meta, txn_id, txn_mgr);
}

Vector<DBEntry*>
NewCatalog::Databases(NewCatalog* catalog, u64 txn_id, TxnTimeStamp begin_ts) {

    Vector<DBEntry*> results;
    catalog->rw_locker_.lock_shared();

    results.reserve(catalog->databases_.size());
    for(const auto& db: catalog->databases_) {
        EntryResult result = DBMeta::GetEntry(db.second.get(), txn_id, begin_ts);
        if(result.err_ != nullptr) {
            CatalogError(*result.err_);
        }
        results.emplace_back((DBEntry*)result.entry_);
    }

    return results;
}

SharedPtr<FunctionSet>
NewCatalog::GetFunctionSetByName(NewCatalog* catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if(!catalog->function_sets_.contains(function_name)) {
        CatalogError("No function name: " + function_name);
    }
    return catalog->function_sets_[function_name];
}

void
NewCatalog::AddFunctionSet(NewCatalog* catalog, const SharedPtr<FunctionSet>& function_set) {
    String name = function_set->name();
    StringToLower(name);
    if(catalog->function_sets_.contains(name)) {
        CatalogError("Trying to add duplicated function table_name into catalog: " + name);
    }
    catalog->function_sets_.emplace(name, function_set);
}

void
NewCatalog::DeleteFunctionSet(NewCatalog* catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if(!catalog->function_sets_.contains(function_name)) {
        CatalogError("Delete not exist function: " + function_name);
    }
    catalog->function_sets_.erase(function_name);
}

// Table Function related methods
SharedPtr<TableFunction>
NewCatalog::GetTableFunctionByName(NewCatalog* catalog, String function_name) {
    StringToLower(function_name);
    if(!catalog->table_functions_.contains(function_name)) {
        CatalogError("No table function table_name: " + function_name);
    }
    return catalog->table_functions_[function_name];
}

void
NewCatalog::AddTableFunction(NewCatalog* catalog, const SharedPtr<TableFunction>& table_function) {
    String name = table_function->name();
    StringToLower(name);
    if(catalog->table_functions_.contains(name)) {
        CatalogError("Trying to add duplicated table function table_name into catalog: " + name);
    }
    catalog->table_functions_.emplace(name, table_function);
}

void
NewCatalog::DeleteTableFunction(NewCatalog* catalog, String function_name) {
    // Unused now.
    StringToLower(function_name);
    if(!catalog->table_functions_.contains(function_name)) {
        CatalogError("Delete not exist table function: " + function_name);
    }
    catalog->table_functions_.erase(function_name);
}

nlohmann::json
NewCatalog::Serialize(const NewCatalog* catalog) {
    nlohmann::json json_res;

    json_res["current_dir"] = *catalog->current_dir_;
    json_res["next_txn_id"] = catalog->next_txn_id_;
    json_res["catalog_version"] = catalog->catalog_version_;
    for(const auto& db_meta: catalog->databases_) {
        json_res["databases"].emplace_back(DBMeta::Serialize(db_meta.second.get()));
    }
    return json_res;
}

UniquePtr<NewCatalog>
NewCatalog::LoadFromFile(const SharedPtr<DirEntry>& dir_entry,
                         BufferManager* buffer_mgr) {
    UniquePtr<NewCatalog> catalog = nullptr;
    String filename = dir_entry->path();

    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(filename,
                                                              FileFlags::READ_FLAG,
                                                              FileLockType::kReadLock);

    SizeT file_size = dir_entry->file_size();
    String json_str(file_size, 0);
    SizeT nbytes = catalog_file_handler->Read(json_str.data(), file_size);
    if(file_size != nbytes) {
        StorageError(fmt::format("Catalog file {}, read error.", filename));
    }

    nlohmann::json catalog_json = nlohmann::json::parse(json_str);
    Deserialize(catalog_json, buffer_mgr, catalog);
    return catalog;
}

void
NewCatalog::Deserialize(const nlohmann::json& catalog_json,
                        BufferManager* buffer_mgr,
                        UniquePtr<NewCatalog>& catalog) {
    SharedPtr<String> current_dir = MakeShared<String>(catalog_json["current_dir"]);

    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    catalog = MakeUnique<NewCatalog>(current_dir);
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->catalog_version_ = catalog_json["catalog_version"];
    if(catalog_json.contains("databases")) {
        for(const auto& db_json: catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->databases_.emplace(*db_meta->db_name_, std::move(db_meta));
        }
    }
}

void
NewCatalog::SaveAsFile(const NewCatalog* catalog_ptr, const String& file_name) {
    nlohmann::json catalog_json = Serialize(catalog_ptr);
    String catalog_str = catalog_json.dump();

    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;
    u8 fileflags = FileFlags::WRITE_FLAG;
    if(!fs.Exists(file_name)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(file_name, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT nbytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if(nbytes != catalog_str.size()) {
        StorageError(fmt::format("Catalog file {}, saving error.", file_name));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();
}

}

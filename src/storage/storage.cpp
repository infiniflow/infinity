//
// Created by JinHai on 2022/9/14.
//

#include <regex>
#include "storage.h"
#include "schema_definition.h"
#include "catalog.h"
#include "function/builtin_functions.h"
#include "storage/io/local_file_system.h"

namespace infinity {

Storage::Storage(const Config* config_ptr)
    : config_ptr_(config_ptr)
    {}

void
Storage::Init() {
    // Construct buffer manager
    buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->buffer_pool_size(),
                                            config_ptr_->data_dir(),
                                            config_ptr_->temp_dir());

    // Check the data dir to get latest catalog file.
    String catalog_dir = *config_ptr_->data_dir() + "/catalog";
    SharedPtr<DirEntry> catalog_file_entry = GetLatestCatalog(catalog_dir);
    if(catalog_file_entry == nullptr) {
        // No catalog file at all
        new_catalog_ = MakeUnique<NewCatalog>(MakeShared<String>(catalog_dir));
        txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(), buffer_mgr_.get());

        Storage::InitCatalog(new_catalog_.get(), txn_mgr_.get());
    } else {
        // load catalog file.
        new_catalog_ = NewCatalog::LoadFromFile(catalog_file_entry, buffer_mgr_.get());
        txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(), buffer_mgr_.get());
    }

    // Check current schema/catalog to default database.

#if 0
    catalog_ = MakeUnique<Catalog>();
    // Update schema need to begin transaction
    SharedPtr<String> schema_name = MakeShared<String>("default");
    SharedPtr<SchemaDefinition> schema_def_ptr = MakeShared<SchemaDefinition>(schema_name, ConflictType::kError);
    catalog_->CreateSchema(schema_def_ptr);
    // Commit transaction
#endif

    BuiltinFunctions builtin_functions(new_catalog_);
    builtin_functions.Init();
}

void
Storage::Uninit() {
    txn_mgr_.reset();
    new_catalog_.reset();
    buffer_mgr_.reset();
    config_ptr_ = nullptr;
}

SharedPtr<DirEntry>
Storage::GetLatestCatalog(const String& dir) {
    LocalFileSystem fs;
    if(!fs.Exists(dir)) {
        fs.CreateDirectory(dir);
        return nullptr;
    }

    Vector<SharedPtr<DirEntry>> dir_array = fs.ListDirectory(dir);
    SharedPtr<DirEntry> latest;
    const std::regex catalog_file_regex("META_[0-9]+\\.json");
    for(const auto& dir_entry_ptr: dir_array) {
        if(dir_entry_ptr->is_regular_file()) {
            if(latest == nullptr) {
                latest = dir_entry_ptr;
            } else {

                String current_file_name = dir_entry_ptr->path();
                if(!std::regex_match(current_file_name, catalog_file_regex)) {
                    continue;
                }

                if(dir_entry_ptr->path() > latest->path()) {
                    latest = dir_entry_ptr;
                }
            }
        }
    }

    return latest;
}

void
Storage::InitCatalog(NewCatalog* catalog, TxnManager* txn_mgr) {
    EntryResult create_res;
    Txn* new_txn = txn_mgr->CreateTxn();
    new_txn->BeginTxn();
    create_res = new_txn->CreateDatabase("default", ConflictType::kError);
    new_txn->CommitTxn();
    if(create_res.err_ != nullptr) {
        StorageError(*create_res.err_);
    }
}

}

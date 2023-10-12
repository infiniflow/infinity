//
// Created by JinHai on 2022/9/14.
//

#include "storage/storage.h"
#include "function/builtin_functions.h"
#include "schema_definition.h"
#include "storage/io/local_file_system.h"
#include "storage/wal/wal_manager.h"
#include <regex>

namespace infinity {

Storage::Storage(const Config *config_ptr) : config_ptr_(config_ptr) {}

void Storage::Init() {
    // Construct buffer manager
    buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->buffer_pool_size(), config_ptr_->data_dir(), config_ptr_->temp_dir());

    // Check the data dir to get latest catalog file.
    String catalog_dir = *config_ptr_->data_dir() + "/catalog";

    // Construct wal manager
    wal_mgr_ = MakeShared<WalManager>(this,
                                      std::filesystem::path(*config_ptr_->wal_dir()) / WAL_FILE_TEMP_FILE,
                                      config_ptr_->wal_size_threshold(),
                                      config_ptr_->full_checkpoint_time_interval(),
                                      config_ptr_->full_checkpoint_txn_interval(),
                                      config_ptr_->delta_checkpoint_time_interval(),
                                      config_ptr_->delta_checkpoint_txn_interval());
    auto start_time_stamp = wal_mgr_->ReplayWalFile();
    wal_mgr_->Start();

    // Construct txn manager
    // TODO:xuanwei if replay wal file, should not create new catalog. must load from wal file. if no wal file, should create new catalog.
    SharedPtr<DirEntry> catalog_file_entry = GetLatestCatalog(catalog_dir);
    if (catalog_file_entry == nullptr) {
        // No catalog file at all
        new_catalog_ = MakeUnique<NewCatalog>(MakeShared<String>(catalog_dir));
        txn_mgr_ =
            MakeUnique<TxnManager>(new_catalog_.get(), buffer_mgr_.get(), std::bind(&WalManager::PutEntry, wal_mgr_.get(), std::placeholders::_1));
        txn_mgr_->Start();

        Storage::InitCatalog(new_catalog_.get(), txn_mgr_.get());
    } else {
        // load catalog file.
        new_catalog_ = NewCatalog::LoadFromFile(catalog_file_entry, buffer_mgr_.get());
        txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(),
                                          buffer_mgr_.get(),
                                          std::bind(&WalManager::PutEntry, wal_mgr_.get(), std::placeholders::_1),
                                          0,
                                          start_time_stamp + 1);

        txn_mgr_->Start();
    }

    BuiltinFunctions builtin_functions(new_catalog_);
    builtin_functions.Init();
}

void Storage::UnInit() {
    txn_mgr_->Stop();
    wal_mgr_->Stop();

    txn_mgr_.reset();
    wal_mgr_.reset();

    new_catalog_.reset();
    buffer_mgr_.reset();
    config_ptr_ = nullptr;
}

SharedPtr<DirEntry> Storage::GetLatestCatalog(const String &dir) {
    LocalFileSystem fs;
    if (!fs.Exists(dir)) {
        fs.CreateDirectory(dir);
        return nullptr;
    }

    Vector<SharedPtr<DirEntry>> dir_array = fs.ListDirectory(dir);
    SharedPtr<DirEntry> latest;
    int64_t latest_version_number = -1;
    const std::regex catalog_file_regex("META_[0-9]+\\.json");
    for (const auto &dir_entry_ptr : dir_array) {
        LOG_TRACE("Candidate file name: {}", dir_entry_ptr->path().c_str());
        if (dir_entry_ptr->is_regular_file()) {
            String current_file_name = dir_entry_ptr->path().filename();
            if (std::regex_match(current_file_name, catalog_file_regex)) {
                int64_t version_number = std::stoll(current_file_name.substr(5, current_file_name.size() - 10));
                if (version_number > latest_version_number) {
                    latest_version_number = version_number;
                    latest = dir_entry_ptr;
                }
            }
        }
    }

    return latest;
}

void Storage::InitCatalog(NewCatalog *catalog, TxnManager *txn_mgr) {
    EntryResult create_res;
    Txn *new_txn = txn_mgr->CreateTxn();
    new_txn->BeginTxn();
    create_res = new_txn->CreateDatabase("default", ConflictType::kError);
    new_txn->CommitTxn();
    if (create_res.err_ != nullptr) {
        StorageError(*create_res.err_);
    }
}

} // namespace infinity

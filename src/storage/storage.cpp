//
// Created by JinHai on 2022/9/14.
//

#include "storage/storage.h"
#include "function/builtin_functions.h"
#include "storage/io/local_file_system.h"
#include <regex>

namespace infinity {

Storage::Storage(const Config *config_ptr) : config_ptr_(config_ptr) {}

void Storage::Init() {
    // Construct buffer manager
    buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->buffer_pool_size(), config_ptr_->data_dir(), config_ptr_->temp_dir());

    // Check the data dir to get latest catalog file.
    String catalog_dir = std::string(*config_ptr_->data_dir()) + "/" + std::string(CATALOG_FILE_DIR);

    // Construct wal manager
    wal_mgr_ = MakeUnique<WalManager>(this,
                                      std::filesystem::path(*config_ptr_->wal_dir()) / WAL_FILE_TEMP_FILE,
                                      config_ptr_->wal_size_threshold(),
                                      config_ptr_->full_checkpoint_interval_sec(),
                                      config_ptr_->delta_checkpoint_interval_sec(),
                                      config_ptr_->delta_checkpoint_interval_wal_bytes());

    // Must init catalog before txn manager.
    // Replay wal file wrap init catalog
    auto start_time_stamp = wal_mgr_->ReplayWalFile();

    // Construct txn
    if (!exist_catalog_) {
        // No catalog file at all
        txn_mgr_ =
            MakeUnique<TxnManager>(new_catalog_.get(), buffer_mgr_.get(), std::bind(&WalManager::PutEntry, wal_mgr_.get(), std::placeholders::_1));
    } else {
        txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(),
                                          buffer_mgr_.get(),
                                          std::bind(&WalManager::PutEntry, wal_mgr_.get(), std::placeholders::_1),
                                          0,
                                          start_time_stamp + 1);
    }
    txn_mgr_->Start();
    // start WalManager after TxnManager since it depends on TxnManager.
    wal_mgr_->Start();

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
    const std::regex catalog_file_regex("META_[0-9]+\\.full.json");
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

void Storage::AttachCatalog(const String &catalog_path) {
    LocalFileSystem fs;

    String catalog_dir = *config_ptr_->data_dir() + "/" + String(CATALOG_FILE_DIR);
    Vector<SharedPtr<DirEntry>> dir_array = fs.ListDirectory(catalog_dir);
    SharedPtr<DirEntry> match;

    const std::regex catalog_file_regex("META_[0-9]+\\.full.json");
    for (const auto &dir_entry_ptr : dir_array) {

        if (dir_entry_ptr->path().c_str() == catalog_path) {

            String current_file_name = dir_entry_ptr->path().filename();

            if (dir_entry_ptr->is_regular_file() && std::regex_match(current_file_name, catalog_file_regex)) {
                LOG_TRACE("Attach file name: {}", dir_entry_ptr->path().c_str());
                // match the catalog path
                match = dir_entry_ptr;
                break;
            }
        }
    }
    if (match == nullptr) {
        StorageError("Catalog file not match");
    }
    LOG_INFO("Attach catalog file: {}", catalog_path.c_str());
    new_catalog_ = NewCatalog::LoadFromFile(catalog_path, buffer_mgr_.get());
    exist_catalog_ = true;
}

void Storage::InitNewCatalog() {
    LOG_INFO("Init new catalog");
    String catalog_dir = std::string(*config_ptr_->data_dir()) + "/" + std::string(CATALOG_FILE_DIR);
    LocalFileSystem fs;
    if (!fs.Exists(catalog_dir)) {
        fs.CreateDirectory(catalog_dir);
    }
    new_catalog_ = MakeUnique<NewCatalog>(MakeShared<String>(catalog_dir), true);
    exist_catalog_ = false;
}

} // namespace infinity

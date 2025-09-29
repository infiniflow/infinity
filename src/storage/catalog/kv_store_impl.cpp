// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:kv_store.impl;

import :kv_store;
import :status;
import :rocksdb_merge_operator;
import :logger;
import :virtual_store;
import :storage;
import :infinity_context;
import :default_values;
import :utility;

import third_party;

namespace infinity {

KVIterator::KVIterator(rocksdb::Iterator *iterator_) : iterator_(iterator_) {}

KVIterator::~KVIterator() { delete iterator_; }

void KVIterator::Seek(const std::string &key) {
    if (key.empty()) {
        iterator_->SeekToFirst();
    } else {
        iterator_->Seek(key);
    }
}

bool KVIterator::Valid() { return iterator_->Valid(); }
void KVIterator::Next() { iterator_->Next(); }
rocksdb::Slice KVIterator::Key() { return iterator_->key(); }
rocksdb::Slice KVIterator::Value() { return iterator_->value(); }

KVInstance::~KVInstance() {
    if (read_options_.iterate_lower_bound != nullptr) {
        delete read_options_.iterate_lower_bound;
        read_options_.iterate_lower_bound = nullptr;
    }
    if (read_options_.iterate_upper_bound != nullptr) {
        delete read_options_.iterate_upper_bound;
        read_options_.iterate_upper_bound = nullptr;
    }
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
}

Status KVInstance::Put(const std::string &key, const std::string &value) {
    //    LOG_TRACE(fmt::format("To put key: {}, value: {}", key, value));
    rocksdb::Status s = transaction_->Put(key, value);
    if (!s.ok()) {
        std::string msg = fmt::format("rocksdb::Transaction::Put key: {}, value: {}", key, value);
        LOG_DEBUG(msg);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVInstance::Delete(const std::string &key) {
    //    LOG_TRACE(fmt::format("To delete key: {}", key));
    rocksdb::Status s = transaction_->Delete(key);
    if (!s.ok()) {
        std::string msg = fmt::format("rocksdb::Transaction::Delete key: {}", key);
        LOG_DEBUG(msg);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVInstance::Get(const std::string &key, std::string &value) {
    rocksdb::Status s = transaction_->Get(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                std::string msg = fmt::format("rocksdb::Transaction::Get key: {}", key);
                LOG_DEBUG(msg);
                return Status::RocksDBError(std::move(s), msg);
            }
        }
    }
    return Status::OK();
}

Status KVInstance::GetForUpdate(const std::string &key, std::string &value) {
    rocksdb::Status s = transaction_->GetForUpdate(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                std::string msg = fmt::format("rocksdb::Transaction::GetForUpdate key: {}", key);
                LOG_DEBUG(msg);
                return Status::RocksDBError(std::move(s), key);
            }
        }
    }
    return Status::OK();
}

std::unique_ptr<KVIterator> KVInstance::GetIterator() { return std::make_unique<KVIterator>(transaction_->GetIterator(read_options_)); }

std::unique_ptr<KVIterator> KVInstance::GetIterator(const char *lower_bound_key, const char *upper_bound_key) {
    if (lower_bound_key != nullptr) {
        read_options_.iterate_lower_bound = new rocksdb::Slice(lower_bound_key);
    }
    if (upper_bound_key != nullptr) {
        read_options_.iterate_upper_bound = new rocksdb::Slice(upper_bound_key);
    }
    return std::make_unique<KVIterator>(transaction_->GetIterator(read_options_));
}

std::vector<std::pair<std::string, std::string>> KVInstance::GetAllKeyValue() {
    std::vector<std::pair<std::string, std::string>> result;
    rocksdb::ReadOptions read_option;
    std::unique_ptr<rocksdb::Iterator> iter{transaction_->GetIterator(read_options_)};
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        result.push_back({iter->key().ToString(), iter->value().ToString()});
    }
    return result;
}

std::string KVInstance::ToString() const {
    std::stringstream ss;
    rocksdb::ReadOptions read_option;
    std::unique_ptr<rocksdb::Iterator> iter{transaction_->GetIterator(read_options_)};
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        auto key = iter->key().ToString();
        auto value = iter->value().ToString();
        if (key.find("fast_rough_filter") == std::string::npos) {
            ss << key << " : " << value << '\n';
        } else {
            ss << key << '\n';
        }
    }
    return ss.str();
}

Status KVInstance::Commit() {
    rocksdb::Status s = transaction_->Commit();
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), "rocksdb::Transaction::Commit");
    }
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
    return Status::OK();
}
Status KVInstance::Rollback() {
    rocksdb::Status s = transaction_->Rollback();
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), "rocksdb::Transaction::Rollback");
    }
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
    return Status::OK();
}

bool IsSstFile(const std::string &file_name) { return file_name.find(".sst") != std::string::npos; }

class FlushListener : public rocksdb::EventListener {
public:
    ~FlushListener() override = default;

    void OnFlushCompleted(rocksdb::DB *db, const rocksdb::FlushJobInfo &info) final {
        const auto &absolute_file_path = info.file_path;
        auto v = infinity::Partition(absolute_file_path, '/');
        auto &file = v.back();
        auto *config = infinity::InfinityContext::instance().config();
        const auto &catalog_path = config->CatalogDir();

        // sst
        auto remote_file_path = fmt::format("{}/{}", S3_META_SST_PREFIX, file);
        auto local_file_path = fmt::format("{}/{}", catalog_path, file);
        // if (!std::filesystem::exists(local_file_path)) {
        //     return;
        // }
        VirtualStore::UploadObject(local_file_path, remote_file_path);
        std::vector<std::string> local_live_files;
        uint64_t manifest_file_size{};
        db->GetLiveFiles(local_live_files, &manifest_file_size);
        std::flat_set<std::string> local_live_files_set{local_live_files};

        for (auto &file : local_live_files | std::views::filter(std::not_fn(IsSstFile))) {
            // upload to s3
            auto v = infinity::Partition(file, '/');
            auto &file1 = v.back();
            remote_file_path = fmt::format("{}/{}", S3_META_PREFIX, file1);
            local_file_path = fmt::format("{}/{}", catalog_path, file1);
            // if (!std::filesystem::exists(local_file_path)) {
            //     return;
            // }
            VirtualStore::UploadObject(local_file_path, remote_file_path);
        }
    }
};

class CompactListener : public rocksdb::EventListener {
public:
    ~CompactListener() override = default;

    void OnCompactionCompleted(rocksdb::DB *db, const rocksdb::CompactionJobInfo &info) final {
        // sst
        const auto &output_files = info.output_files;
        const auto &input_files = info.input_files;
        auto *config = infinity::InfinityContext::instance().config();
        const auto &catalog_path = config->CatalogDir();

        for (const auto &absolute_file_path : output_files) {
            auto v = infinity::Partition(absolute_file_path, '/');
            auto &file = v.back();
            auto remote_file_path = fmt::format("{}/{}", S3_META_SST_PREFIX, file);
            auto local_file_path = fmt::format("{}/{}", catalog_path, file);
            VirtualStore::UploadObject(local_file_path, remote_file_path);
        }
        for (const auto &absolute_file_path : input_files) {
            auto v = infinity::Partition(absolute_file_path, '/');
            auto &file = v.back();
            auto remote_file_path = fmt::format("{}/{}", S3_META_SST_PREFIX, file);
            VirtualStore::RemoveObject(remote_file_path);
        }

        std::vector<std::string> local_live_files;
        uint64_t manifest_file_size{};
        db->GetLiveFiles(local_live_files, &manifest_file_size, false);
        std::flat_set<std::string> local_live_files_set{local_live_files};

        std::vector<std::string> remote_live_files;
        VirtualStore::ListObjects(S3_DEFAULT_BUCKET, S3_META_PREFIX, remote_live_files);
        std::flat_set<std::string> remote_live_files_set{remote_live_files};

        for (auto &file : local_live_files | std::views::filter(std::not_fn(IsSstFile))) {
            // upload to s3
            auto v = infinity::Partition(file, '/');
            auto &file1 = v.back();
            auto remote_file_path = fmt::format("{}/{}", S3_META_PREFIX, file1);
            auto local_file_path = fmt::format("{}/{}", catalog_path, file1);
            // if (!std::filesystem::exists(local_file_path)) {
            //     return;
            // }
            VirtualStore::UploadObject(local_file_path, remote_file_path);
        }
    }
};

Status KVStore::Init(const std::string &db_path) {
    db_path_ = db_path;
    options_.create_if_missing = true;
    options_.merge_operator = String2UInt64AddOperator::Create();
    options_.avoid_flush_during_shutdown = true;
    options_.manual_wal_flush = true;
    write_options_.disableWAL = true;

    txn_options_.set_snapshot = true;

    auto *config = infinity::InfinityContext::instance().config();
    if (config != nullptr && config->StorageType() == StorageType::kMinio) {
        options_.listeners.emplace_back(std::make_shared<FlushListener>());
        options_.listeners.emplace_back(std::make_shared<CompactListener>());
    }

    rocksdb::Status s = rocksdb::TransactionDB::Open(options_, txn_db_options_, db_path_, &transaction_db_);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::TransactionDB::Open db path: {}", db_path));
    }
    return Status::OK();
}

Status KVStore::Uninit() {
    if (transaction_db_) {
        delete transaction_db_;
        transaction_db_ = nullptr;
    }
    LOG_INFO("KV store is stopped.");
    return Status::OK();
}

KVStore::~KVStore() {
    if (transaction_db_) {
        delete transaction_db_;
        transaction_db_ = nullptr;
    }
    LOG_INFO("KV store is stopped.");
}

Status KVStore::Flush() {
    rocksdb::FlushOptions flush_opts;
    rocksdb::Status s = transaction_db_->Flush(flush_opts);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), "rocksdb::TransactionDB::Flush");
    }
    return Status::OK();
}

Status KVStore::CreateBackup(const std::string &backup_path, std::vector<rocksdb::BackupInfo> &backup_info_list) {
    rocksdb::BackupEngine *backup_engine;
    rocksdb::Status s = rocksdb::BackupEngine::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::BackupEngine::Open: {}", backup_path));
    }

    rocksdb::IOStatus io_status = backup_engine->CreateNewBackup(transaction_db_);
    if (!io_status.ok()) {
        return Status::RocksDBError(std::move(io_status), fmt::format("rocksdb::BackupEngine::CreateNewBackup: {}", backup_path));
    }

    backup_engine->GetBackupInfo(&backup_info_list, true);

    return Status::OK();
}

Status KVStore::RestoreFromBackup(const std::string &backup_path, const std::string &db_path) {
    rocksdb::BackupEngineReadOnly *backup_engine_ro{};
    rocksdb::IOStatus s = rocksdb::BackupEngineReadOnly::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine_ro);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::BackupEngineReadOnly::Open: {}", backup_path));
    }
    s = backup_engine_ro->RestoreDBFromLatestBackup(db_path, db_path);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::BackupEngineReadOnly::RestoreDBFromLatestBackup: {}", backup_path));
    }
    return Status::OK();
}

std::unique_ptr<KVInstance> KVStore::GetInstance() {
    std::unique_ptr<KVInstance> kv_instance = std::make_unique<KVInstance>();
    kv_instance->transaction_ = transaction_db_->BeginTransaction(write_options_, txn_options_);
    kv_instance->read_options_.snapshot = kv_instance->transaction_->GetSnapshot();
    return kv_instance;
}

Status KVStore::Put(const std::string &key, const std::string &value, bool disable_wal) {
    rocksdb::WriteOptions write_options;
    write_options.disableWAL = disable_wal;
    rocksdb::Status s = transaction_db_->Put(write_options, key, value);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::TransactionDB::Put key: {}, value: {}", key, value));
    }

    if (!disable_wal) {
        transaction_db_->FlushWAL(false);
    }
    return Status::OK();
}

Status KVStore::Delete(const std::string &key, bool disable_wal) {
    rocksdb::WriteOptions write_options;
    write_options.disableWAL = disable_wal;
    rocksdb::Status s = transaction_db_->Delete(write_options, key);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::TransactionDB::Delete key: {}", key));
    }

    if (!disable_wal) {
        transaction_db_->FlushWAL(false);
    }
    return Status::OK();
}

Status KVStore::Get(const std::string &key, std::string &value) {
    rocksdb::Status s = transaction_db_->Get(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                return Status::RocksDBError(std::move(s), fmt::format("rocksdb::TransactionDB::Get key: {}", key));
            }
        }
    }
    return Status::OK();
}

Status KVStore::Merge(const std::string &key, const std::string &value, bool disable_wal) {
    rocksdb::WriteOptions write_options;
    write_options.disableWAL = disable_wal;
    rocksdb::Status s = transaction_db_->Merge(write_options, nullptr, key, value);
    if (!s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::TransactionDB::Merge key: {}, value: {}", key, value));
    }

    if (!disable_wal) {
        transaction_db_->FlushWAL(false);
    }
    return Status::OK();
}

std::string KVStore::ToString() const {
    std::stringstream ss;
    rocksdb::ReadOptions read_option;
    std::unique_ptr<rocksdb::Iterator> iter{transaction_db_->NewIterator(read_option)};
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        auto key = iter->key().ToString();
        auto value = iter->value().ToString();
        if (key.find("fast_rough_filter") == std::string::npos) {
            ss << key << " : " << value << '\n';
        } else {
            ss << key << '\n';
        }
    }
    return ss.str();
}

size_t KVStore::KeyValueNum() const {
    size_t cnt = 0;
    rocksdb::ReadOptions read_option;
    std::unique_ptr<rocksdb::Iterator> iter{transaction_db_->NewIterator(read_option)};
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        ++cnt;
    }
    return cnt;
}

std::vector<std::pair<std::string, std::string>> KVStore::GetAllKeyValue() {
    std::vector<std::pair<std::string, std::string>> result;
    rocksdb::ReadOptions read_option;
    std::unique_ptr<rocksdb::Iterator> iter{transaction_db_->NewIterator(read_option)};
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        result.push_back({iter->key().ToString(), iter->value().ToString()});
    }
    return result;
}

Status KVStore::Destroy(const std::string &db_path) {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.manual_wal_flush = true;
    options.avoid_flush_during_shutdown = true;

    if (auto s = rocksdb::DestroyDB(db_path, options); !s.ok()) {
        return Status::RocksDBError(std::move(s), fmt::format("rocksdb::DestroyDB: {}", db_path));
    }
    return Status::OK();
}

} // namespace infinity
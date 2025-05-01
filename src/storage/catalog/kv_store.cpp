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

module;

// #include "rocksdb/db.h"
// #include "rocksdb/options.h"
// #include "rocksdb/slice.h"
// #include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"

module kv_store;

import stl;
import status;
import third_party;
import rocksdb_merge_operator;
import logger;

namespace infinity {

KVIterator::KVIterator(rocksdb::Iterator *iterator_) : iterator_(iterator_) {}

KVIterator::KVIterator(rocksdb::Transaction *transaction, rocksdb::ReadOptions &read_options, const String &upper_bound) {
    if (!upper_bound.empty()) {
        upper_bound_ = MakeUnique<rocksdb::Slice>(upper_bound);
        read_options.iterate_upper_bound = upper_bound_.get();
    }

    iterator_ = transaction->GetIterator(read_options);
}

KVIterator::~KVIterator() { delete iterator_; }

void KVIterator::Seek(const String &key) {
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

Status KVInstance::Put(const String &key, const String &value) {
    //    LOG_INFO(fmt::format("Put key: {}, value: {}", key, value));
    rocksdb::Status s = transaction_->Put(key, value);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::Transaction::Put key: {}, value: {}", key, value);
        LOG_DEBUG(msg);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVInstance::Delete(const String &key) {
    rocksdb::Status s = transaction_->Delete(key);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::Transaction::Delete key: {}", key);
        LOG_DEBUG(msg);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVInstance::Get(const String &key, String &value) {
    rocksdb::Status s = transaction_->Get(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                String msg = fmt::format("rocksdb::Transaction::Get key: {}", key);
                LOG_DEBUG(msg);
                return Status::RocksDBError(std::move(s), msg);
            }
        }
    }
    return Status::OK();
}

Status KVInstance::GetForUpdate(const String &key, String &value) {
    rocksdb::Status s = transaction_->GetForUpdate(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                String msg = fmt::format("rocksdb::Transaction::GetForUpdate key: {}", key);
                LOG_DEBUG(msg);
                return Status::RocksDBError(std::move(s), key);
            }
        }
    }
    return Status::OK();
}

UniquePtr<KVIterator> KVInstance::GetIterator() { return MakeUnique<KVIterator>(transaction_->GetIterator(read_options_)); }

UniquePtr<KVIterator> KVInstance::GetIterator(const char *lower_bound_key, const char *upper_bound_key) {
    if (lower_bound_key != nullptr) {
        read_options_.iterate_lower_bound = new rocksdb::Slice(lower_bound_key);
    }
    if (upper_bound_key != nullptr) {
        read_options_.iterate_upper_bound = new rocksdb::Slice(upper_bound_key);
    }
    return MakeUnique<KVIterator>(transaction_->GetIterator(read_options_));
}

Vector<Pair<String, String>> KVInstance::GetAllKeyValue() {
    Vector<Pair<String, String>> result;
    rocksdb::ReadOptions read_option;
    auto iter = transaction_->GetIterator(read_options_);
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        result.push_back({iter->key().ToString(), iter->value().ToString()});
    }
    delete iter;
    return result;
}

Status KVInstance::Commit() {
    rocksdb::Status s = transaction_->Commit();
    if (!s.ok()) {
        String msg("rocksdb::Transaction::Commit");
        return Status::RocksDBError(std::move(s), msg);
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
        String msg("rocksdb::Transaction::Rollback");
        return Status::RocksDBError(std::move(s), msg);
    }
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
    return Status::OK();
}

Status KVStore::Init(const String &db_path) {
    db_path_ = db_path;
    options_.create_if_missing = true;
    options_.manual_wal_flush = true;
    options_.avoid_flush_during_shutdown = true;
    options_.merge_operator = String2UInt64AddOperator::Create();

    txn_options_.set_snapshot = true;

    write_options_.disableWAL = true;

    rocksdb::Status s = rocksdb::TransactionDB::Open(options_, txn_db_options_, db_path_, &transaction_db_);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::TransactionDB::Open db path: {}", db_path);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVStore::Uninit() {
    delete transaction_db_;
    transaction_db_ = nullptr;

    return Status::OK();
}

Status KVStore::Flush() {
    rocksdb::FlushOptions flush_opts;
    rocksdb::Status s = transaction_db_->Flush(flush_opts);
    if (!s.ok()) {
        String msg("rocksdb::TransactionDB::Flush");
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVStore::CreateBackup(const String &backup_path, Vector<rocksdb::BackupInfo> &backup_info_list) {
    rocksdb::BackupEngine *backup_engine;
    rocksdb::Status s = rocksdb::BackupEngine::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::BackupEngine::Open: {}", backup_path);
        return Status::RocksDBError(std::move(s), msg);
    }

    rocksdb::IOStatus io_status = backup_engine->CreateNewBackup(transaction_db_);
    if (!io_status.ok()) {
        String msg = fmt::format("rocksdb::BackupEngine::CreateNewBackup: {}", backup_path);
        return Status::RocksDBError(std::move(io_status), msg);
    }

    backup_engine->GetBackupInfo(&backup_info_list, true);

    return Status::OK();
}

Status KVStore::RestoreFromBackup(const String &backup_path, const String &db_path) {
    rocksdb::BackupEngineReadOnly *backup_engine_ro{};
    rocksdb::IOStatus s = rocksdb::BackupEngineReadOnly::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine_ro);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::BackupEngineReadOnly::Open: {}", backup_path);
        return Status::RocksDBError(std::move(s), msg);
    }
    s = backup_engine_ro->RestoreDBFromLatestBackup(db_path, db_path);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::BackupEngineReadOnly::RestoreDBFromLatestBackup: {}", backup_path);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

UniquePtr<KVInstance> KVStore::GetInstance() {
    UniquePtr<KVInstance> kv_instance = MakeUnique<KVInstance>();
    kv_instance->transaction_ = transaction_db_->BeginTransaction(write_options_, txn_options_);
    kv_instance->read_options_.snapshot = kv_instance->transaction_->GetSnapshot();
    return kv_instance;
}

Status KVStore::Put(const String &key, const String &value) {
    rocksdb::Status s = transaction_db_->Put(write_options_, key, value);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::TransactionDB::Put key: {}, value: {}", key, value);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVStore::Delete(const String &key) {
    rocksdb::Status s = transaction_db_->Delete(write_options_, key);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::TransactionDB::Delete key: {}", key);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

Status KVStore::Get(const String &key, String &value) {
    rocksdb::Status s = transaction_db_->Get(read_options_, key, &value);
    if (!s.ok()) {
        switch (s.code()) {
            case rocksdb::Status::Code::kNotFound: {
                return Status::NotFound(fmt::format("Key not found: {}", key));
            }
            default: {
                String msg = fmt::format("rocksdb::TransactionDB::Get key: {}", key);
                return Status::RocksDBError(std::move(s), msg);
            }
        }
    }
    return Status::OK();
}

Status KVStore::Merge(const String &key, const String &value) {
    rocksdb::Status s = transaction_db_->Merge(write_options_, nullptr, key, value);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::TransactionDB::Merge key: {}, value: {}", key, value);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

String KVStore::ToString() const {
    std::stringstream ss;
    rocksdb::ReadOptions read_option;
    auto iter = transaction_db_->NewIterator(read_option);
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        ss << iter->key().ToString() << " : " << iter->value().ToString() << std::endl;
    }
    delete iter;
    return ss.str();
}

SizeT KVStore::KeyValueNum() const {
    SizeT cnt = 0;
    rocksdb::ReadOptions read_option;
    auto iter = transaction_db_->NewIterator(read_option);
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        ++cnt;
    }
    delete iter;
    return cnt;
}

Vector<Pair<String, String>> KVStore::GetAllKeyValue() {
    Vector<Pair<String, String>> result;
    rocksdb::ReadOptions read_option;
    auto iter = transaction_db_->NewIterator(read_option);
    iter->SeekToFirst();
    for (; iter->Valid(); iter->Next()) {
        result.push_back({iter->key().ToString(), iter->value().ToString()});
    }
    return result;
}

Status KVStore::Destroy(const String &db_path) {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.manual_wal_flush = true;
    options.avoid_flush_during_shutdown = true;

    rocksdb::Status s = ::rocksdb::DestroyDB(db_path, options);
    if (!s.ok()) {
        String msg = fmt::format("rocksdb::DestroyDB: {}", db_path);
        return Status::RocksDBError(std::move(s), msg);
    }
    return Status::OK();
}

} // namespace infinity
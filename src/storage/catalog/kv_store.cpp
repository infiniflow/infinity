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

namespace infinity {

Status KVInstance::Put(const String &key, const String &value) {
    rocksdb::Status s = transaction_->Put(key, value);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

Status KVInstance::Delete(const String &key) {
    rocksdb::Status s = transaction_->Delete(key);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

Status KVInstance::Get(const String &key, String &value) {
    rocksdb::Status s = transaction_->Get(read_options_, key, &value);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

Status KVInstance::GetForUpdate(const String &key, String &value) {
    rocksdb::Status s = transaction_->GetForUpdate(read_options_, key, &value);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

rocksdb::Iterator *KVInstance::GetIterator() { return transaction_->GetIterator(read_options_); }

Status KVInstance::Commit() {
    rocksdb::Status s = transaction_->Commit();
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}
Status KVInstance::Rollback() {
    rocksdb::Status s = transaction_->Rollback();
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

Status KVStore::Init(const String &db_path) {
    db_path_ = db_path;
    options_.create_if_missing = true;
    options_.manual_wal_flush = true;
    options_.avoid_flush_during_shutdown = true;
    txn_options_.set_snapshot = true;

    write_options_.disableWAL = true;

    rocksdb::Status s = rocksdb::TransactionDB::Open(options_, txn_db_options_, db_path_, &transaction_db_);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
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
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

Status KVStore::CreateBackup(const String &backup_path, Vector<rocksdb::BackupInfo> &backup_info_list) {
    rocksdb::BackupEngine *backup_engine;
    rocksdb::Status s = rocksdb::BackupEngine::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }

    rocksdb::IOStatus io_status = backup_engine->CreateNewBackup(transaction_db_);
    if (!io_status.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", io_status.ToString()));
    }

    backup_engine->GetBackupInfo(&backup_info_list, true);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }

    return Status::OK();
}

Status KVStore::RestoreFromBackup(const String &backup_path, const String &db_path) {
    rocksdb::BackupEngineReadOnly *backup_engine_ro{};
    rocksdb::IOStatus s = rocksdb::BackupEngineReadOnly::Open(rocksdb::Env::Default(), rocksdb::BackupEngineOptions(backup_path), &backup_engine_ro);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    s = backup_engine_ro->RestoreDBFromLatestBackup(db_path, db_path);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

UniquePtr<KVInstance> KVStore::GetInstance() {
    UniquePtr<KVInstance> kv_instance = MakeUnique<KVInstance>();
    kv_instance->transaction_ = transaction_db_->BeginTransaction(write_options_, txn_options_);
    kv_instance->read_options_.snapshot = kv_instance->transaction_->GetSnapshot();
    return kv_instance;
}

Status KVStore::Destroy(const String &db_path) {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.manual_wal_flush = true;
    options.avoid_flush_during_shutdown = true;

    rocksdb::Status s = ::rocksdb::DestroyDB(db_path, options);
    if (!s.ok()) {
        return Status::UnexpectedError(fmt::format("Unexpected error: {}", s.ToString()));
    }
    return Status::OK();
}

} // namespace infinity
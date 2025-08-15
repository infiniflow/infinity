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

export module infinity_core:kv_store;

import :stl;
import :status;

import third_party;

namespace infinity {

export class KVStore;
export class KVInstance;

export class KVIterator {
public:
    explicit KVIterator(rocksdb::Iterator *iterator_);
    ~KVIterator();

    void Seek(const std::string &key);
    bool Valid();
    void Next();
    rocksdb::Slice Key();
    rocksdb::Slice Value();

private:
    rocksdb::Iterator *iterator_{};
};

class KVInstance {

    friend class KVStore;

public:
    KVInstance() = default;
    ~KVInstance();

    Status Put(const std::string &key, const std::string &value);
    Status Delete(const std::string &key);
    Status Get(const std::string &key, std::string &value);
    Status GetForUpdate(const std::string &key, std::string &value);
    std::unique_ptr<KVIterator> GetIterator();
    std::unique_ptr<KVIterator> GetIterator(const char *lower_bound_key, const char *upper_bound_key);
    std::vector<std::pair<std::string, std::string>> GetAllKeyValue();
    std::string ToString() const;

    Status Commit();
    Status Rollback();

private:
    rocksdb::Transaction *transaction_{};
    rocksdb::ReadOptions read_options_;
};

class KVStore {
public:
    KVStore() = default;
    ~KVStore();

    Status Init(const std::string &db_path);
    Status Uninit();
    Status Flush();
    Status CreateBackup(const std::string &backup_path, std::vector<rocksdb::BackupInfo> &backup_info_list);
    static Status RestoreFromBackup(const std::string &backup_path, const std::string &db_path);

    std::unique_ptr<KVInstance> GetInstance();

    Status Put(const std::string &key, const std::string &value);
    Status Delete(const std::string &key);
    Status Get(const std::string &key, std::string &value);
    Status Merge(const std::string &key, const std::string &value);

    std::string ToString() const;
    size_t KeyValueNum() const;
    std::vector<std::pair<std::string, std::string>> GetAllKeyValue();

    // For UT
    static Status Destroy(const std::string &db_path);

private:
    std::string db_path_{};
    rocksdb::TransactionDB *transaction_db_{}; // RocksDB transaction db
    rocksdb::Options options_;
    rocksdb::TransactionDBOptions txn_db_options_;
    rocksdb::TransactionOptions txn_options_;
    rocksdb::WriteOptions write_options_;
    rocksdb::ReadOptions read_options_;
};

} // namespace infinity
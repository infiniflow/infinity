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
import :third_party;
import :status;

namespace infinity {

export class KVStore;
export class KVInstance;

export class KVIterator {
public:
    explicit KVIterator(rocksdb::Iterator *iterator_);
    ~KVIterator();

    void Seek(const String &key);
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

    Status Put(const String &key, const String &value);
    Status Delete(const String &key);
    Status Get(const String &key, String &value);
    Status GetForUpdate(const String &key, String &value);
    UniquePtr<KVIterator> GetIterator();
    UniquePtr<KVIterator> GetIterator(const char *lower_bound_key, const char *upper_bound_key);
    Vector<Pair<String, String>> GetAllKeyValue();
    String ToString() const;

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

    Status Init(const String &db_path);
    Status Uninit();
    Status Flush();
    Status CreateBackup(const String &backup_path, Vector<rocksdb::BackupInfo> &backup_info_list);
    static Status RestoreFromBackup(const String &backup_path, const String &db_path);

    UniquePtr<KVInstance> GetInstance();

    Status Put(const String &key, const String &value);
    Status Delete(const String &key);
    Status Get(const String &key, String &value);
    Status Merge(const String &key, const String &value);

    String ToString() const;
    SizeT KeyValueNum() const;
    Vector<Pair<String, String>> GetAllKeyValue();

    // For UT
    static Status Destroy(const String &db_path);

private:
    String db_path_{};
    rocksdb::TransactionDB *transaction_db_{}; // RocksDB transaction db
    rocksdb::Options options_;
    rocksdb::TransactionDBOptions txn_db_options_;
    rocksdb::TransactionOptions txn_options_;
    rocksdb::WriteOptions write_options_;
    rocksdb::ReadOptions read_options_;
};

} // namespace infinity
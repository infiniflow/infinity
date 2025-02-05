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

export module kv_store;

import stl;
import third_party;
import status;

namespace infinity {

export class KVStore;

export class KVInstance {

    friend class KVStore;

public:
    KVInstance() = default;
    ~KVInstance() = default;

    Status Put(const String &key, const String &value);
    Status Delete(const String &key);
    Status Get(const String &key, String &value);
    rocksdb::Iterator* GetIterator();

    Status Commit();
    Status Rollback();

private:
    rocksdb::Transaction *transaction_{};
    rocksdb::ReadOptions read_options_;
};

class KVStore {
public:
    KVStore() = default;
    ~KVStore() = default;

    Status Init(const String &db_path);
    Status Uninit();
    Status Flush();

    UniquePtr<KVInstance> GetInstance();

    // For UT
    Status Destroy();

private:
    String db_path_{};
    rocksdb::TransactionDB *transaction_db_{}; // RocksDB transaction db
    rocksdb::Options options_;
    rocksdb::TransactionDBOptions txn_db_options_;
    rocksdb::TransactionOptions txn_options_;
    rocksdb::WriteOptions write_options_;
};

} // namespace infinity
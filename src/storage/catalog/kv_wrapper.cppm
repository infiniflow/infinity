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

export class KVStore {
public:
    KVStore();
    ~KVStore();

    Status Init();
    Status Uninit();

public:
private:
    TransactionDB *transaction_db_{}; // RocksDB transaction db
};

} // namespace infinity
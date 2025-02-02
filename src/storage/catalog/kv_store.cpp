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

//#include "rocksdb/db.h"
//#include "rocksdb/options.h"
//#include "rocksdb/slice.h"
//#include "rocksdb/utilities/transaction.h"
//#include "rocksdb/utilities/transaction_db.h"

module kv_store;

import stl;
import status;
import third_party;

namespace infinity {

Status KVStore::Init(const String &db_path) {
//    Options options;
//    TransactionDBOptions txn_db_options;
//    options.create_if_missing = true;

    return Status::OK(); }

Status KVStore::Uninit() { return Status::OK(); }

} // namespace infinity
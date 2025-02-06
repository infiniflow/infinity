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

module new_catalog;

import stl;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import kv_code;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

Status NewCatalog::CreateDatabase(const SharedPtr<String> &db_name, const SharedPtr<String> &comment, NewTxn *new_txn, ConflictType conflict_type) {
    KVInstance *kv_instance = new_txn->kv_instance();
    switch (conflict_type) {
        case ConflictType::kError:
            //            if (CheckDatabaseExists(db_name, new_txn)) {
            //                return {ErrorCode::kDatabaseAlreadyExists, "Database already exists"};
            //            }
            kv_instance->Put("abc", "abc");
            break;
        case ConflictType::kIgnore:
            //            if (CheckDatabaseExists(db_name, new_txn)) {
            //                return Status::OK();
            //            }
            break;
        case ConflictType::kReplace:
            break;
        default:
            return {ErrorCode::kInvalidConflictType, "Invalid conflict type"};
    }
    return Status::OK();
}

Status NewCatalog::DropDatabase(const SharedPtr<String> &db_name, NewTxn *new_txn, ConflictType conflict_type) { return Status::OK(); }

bool NewCatalog::CheckDatabaseExists(const SharedPtr<String> &db_name, NewTxn *new_txn) {
    KVInstance *kv_instance = new_txn->kv_instance();
    String db_key_prefix = KeyEncode::CatalogDbKeyPrefix(*db_name);
    String db_key = KeyEncode::CatalogDbKey(*db_name, new_txn->BeginTS(), new_txn->TxnID());
    auto iter2 = kv_instance->GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        iter2->Next();
        ++found_count;
    }
    if (found_count == 0) {
        // Not found
        return false;
    }
    return true;
}

Tuple<SharedPtr<DatabaseInfo>, Status> NewCatalog::GetDatabaseInfo(const String &db_name, NewTxn *new_txn) { return {nullptr, Status::OK()}; }

} // namespace infinity
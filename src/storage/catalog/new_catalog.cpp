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

#include <string>

module new_catalog;

import stl;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import kv_code;
import third_party;
import logger;
import infinity_exception;
import default_values;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

Status NewCatalog::CreateDatabase(const SharedPtr<String> &db_name, const SharedPtr<String> &comment, NewTxn *new_txn, ConflictType conflict_type) {
    KVInstance *kv_instance = new_txn->kv_instance();

    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }

    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    bool db_exist = CheckDatabaseExists(db_name, new_txn);
    if (db_exist and conflict_type == ConflictType::kError) {
        return Status(ErrorCode::kDuplicateDatabaseName, MakeUnique<String>(fmt::format("Database: {} already exists", *db_name)));
    }

    if (db_exist and conflict_type == ConflictType::kIgnore) {
        return Status::OK();
    }

    // Get the latest database id of system
    String db_string_id;
    Status status = kv_instance->GetForUpdate(LATEST_DATABASE_ID.data(), db_string_id);
    SizeT db_id = 0;
    if (status.ok()) {
        db_id = std::stoull(db_string_id);
    } else {
        LOG_DEBUG(fmt::format("Failed to get latest_database_id: {}", status.message()));
    }

    ++db_id;
    String db_key = KeyEncode::CatalogDbKey(*db_name, new_txn->BeginTS());
    String db_value = fmt::format("{}", db_id);
    status = kv_instance->Put(db_key, db_value);
    if (!status.ok()) {
        return status;
    }
    status = kv_instance->Put(LATEST_DATABASE_ID.data(), db_value);
    if (!status.ok()) {
        return status;
    }
    if (comment != nullptr) {
        String db_comment_key = KeyEncode::CatalogDbTagKey(db_value, "comment");
        status = kv_instance->Put(db_comment_key, *comment);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::DropDatabase(const SharedPtr<String> &db_name, NewTxn *new_txn, ConflictType conflict_type) {
    if (conflict_type == ConflictType::kReplace) {
        return Status::NotSupport("ConflictType::kReplace");
    }
    if (conflict_type == ConflictType::kInvalid) {
        return Status::UnexpectedError("Unknown ConflictType");
    }

    KVInstance *kv_instance = new_txn->kv_instance();
    String db_key_prefix = KeyEncode::CatalogDbPrefix(*db_name);
    String db_key = KeyEncode::CatalogDbKey(*db_name, new_txn->BeginTS());
    auto iter2 = kv_instance->GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_db_keys;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        if (found_count == 0) {
            Status status = kv_instance->Delete(iter2->Key().ToString());
            if (!status.ok()) {
                return status;
            }
        } else {
            // Error branch
            error_db_keys.push_back(iter2->Key().ToString());
        }
        iter2->Next();
        ++found_count;
    }

    if (!error_db_keys.empty()) {
        // join error_db_keys
        String error_db_keys_str =
            std::accumulate(std::next(error_db_keys.begin()), error_db_keys.end(), error_db_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple database keys: {}", error_db_keys_str));
    }

    if (found_count == 0 && conflict_type == ConflictType::kError) {
        return Status::DBNotExist(*db_name);
    }

    return Status::OK();
}

bool NewCatalog::CheckDatabaseExists(const SharedPtr<String> &db_name, NewTxn *new_txn) {
    KVInstance *kv_instance = new_txn->kv_instance();
    String db_key_prefix = KeyEncode::CatalogDbPrefix(*db_name);
    String db_key = KeyEncode::CatalogDbKey(*db_name, new_txn->BeginTS());
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

String NewCatalog::GetPathNameTail(const String &path) {
    SizeT delimiter_i = path.rfind('/');
    if (delimiter_i == String::npos) {
        return path;
    }
    return path.substr(delimiter_i + 1);
}

Status NewCatalog::LockTable(const String &table_key) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (table_memory_context->locked_) {
        return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
    }
    table_memory_context->locked_ = true;
    return Status::OK();
}
Status NewCatalog::UnlockTable(const String &table_key) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    SharedPtr<TableMemoryContext> &table_memory_context = table_memory_context_map_[table_key];
    if (table_memory_context->locked_) {
        table_memory_context->locked_ = false;
        return Status::OK();
    }

    return Status::NotLocked(fmt::format("Table key: {} wasn't locked before", table_key));
}

bool NewCatalog::IsTableLocked(const String &table_key) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return false;
    }

    SharedPtr<TableMemoryContext> &table_memory_context = table_memory_context_map_[table_key];
    if (table_memory_context->locked_) {
        return true;
    }

    return false;
}

Status NewCatalog::IncreaseTableWriteCount(const String &table_key) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locked_) {
        return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
    }

    ++table_memory_context->write_txn_num_;
    return Status::OK();
}

Status NewCatalog::DecreaseTableWriteCount(const String &table_key) {
    std::lock_guard lock(mtx_);

    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locked_) {
        UnrecoverableError(fmt::format("Table key: {} is already locked", table_key));
    }

    SizeT& write_txn_num = iter->second->write_txn_num_;
    if (write_txn_num > 0) {
        --write_txn_num;
    }

    if (write_txn_num == 0) {
        table_memory_context_map_.erase(table_key);
    }
    return Status::OK();
}

} // namespace infinity
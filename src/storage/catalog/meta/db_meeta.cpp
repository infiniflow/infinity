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

module db_meeta;

import stl;
import kv_code;
import kv_store;
import third_party;
import infinity_exception;
import meta_info;
import default_values;

namespace infinity {

DBMeeta::DBMeeta(String db_id_str, KVInstance &kv_instance) : db_id_str_(std::move(db_id_str)), kv_instance_(kv_instance) {}

Status DBMeeta::InitSet(const String *comment) {
    if (comment) {
        String db_comment_key = GetDBTag("comment");
        Status status = kv_instance_.Put(db_comment_key, *comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create next table id;
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    Status status = kv_instance_.Put(next_table_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status DBMeeta::UninitSet(UsageFlag usage_flag) {
    Status status;

    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str_);

    auto iter = kv_instance_.GetIterator();
    iter->Seek(db_table_prefix);
    while (iter->Valid() && iter->Key().starts_with(db_table_prefix)) {
        String table_key = iter->Key().ToString();
        status = kv_instance_.Delete(table_key);
        if (!status.ok()) {
            return status;
        }
        iter->Next();
    }

    String db_comment_key = GetDBTag("comment");
    status = kv_instance_.Delete(db_comment_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String db_next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    status = kv_instance_.Delete(db_next_table_id_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status DBMeeta::GetTableID(const String &table_name, String &table_key, String &table_id_str) {
    String table_key_prefix = KeyEncode::CatalogTablePrefix(db_id_str_, table_name);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(table_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_table_keys;
    while (iter2->Valid() && iter2->Key().starts_with(table_key_prefix)) {
        if (found_count > 0) {
            // Error branch
            error_table_keys.push_back(table_key);
        }
        table_key = iter2->Key().ToString();
        table_id_str = iter2->Value().ToString();
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No table, ignore it.
        return Status::TableNotExist(table_name);
    }

    if (!error_table_keys.empty()) {
        error_table_keys.push_back(table_key);
        // join error_table_keys
        String error_table_keys_str =
            std::accumulate(std::next(error_table_keys.begin()), error_table_keys.end(), error_table_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple table keys: {}", error_table_keys_str));
    }

    return Status::OK();
}

Status DBMeeta::GetTableName(const String &table_id_str, String &table_key, String &table_name) const {
    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str_);

    size_t start, end;
    auto iter = kv_instance_.GetIterator();
    iter->Seek(db_table_prefix);
    while (iter->Valid() && iter->Key().starts_with(db_table_prefix)) {
        table_key = iter->Key().ToString();
        start = db_table_prefix.size();
        end = table_key.find('|', start);
        if (table_id_str == iter->Value().ToString()) {
            table_name = table_key.substr(start, end - start);
            return Status::OK();
        }
        iter->Next();
    }
    table_key = "";
    return Status::TableNotExist(table_id_str);
}

Status DBMeeta::GetDBName(const String &db_id_str, String &db_name) const {
    auto iter = kv_instance_.GetIterator();
    iter->Seek(KeyEncode::kCatalogDbHeader);
    while (iter->Valid() && iter->Key().starts_with(KeyEncode::kCatalogDbHeader)) {
        String key_str = iter->Key().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        if (db_id_str == iter->Value().ToString()) {
            db_name = key_str.substr(start, end - start);
            return Status::OK();
        }
        iter->Next();
    }
    return Status::DBNotExist(db_id_str);
}

Status DBMeeta::GetDatabaseInfo(DatabaseInfo &db_info) {
    Status status;

    String *db_comment = nullptr;
    status = this->GetComment(db_comment);
    if (!status.ok()) {
        return status;
    }
    db_info.db_comment_ = MakeShared<String>(*db_comment);

    db_info.db_entry_dir_ = MakeShared<String>(fmt::format("db_{}", db_id_str_));

    return Status::OK();
}

Tuple<String, Status> DBMeeta::GetNextTableID() {
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    String next_table_id_str;
    Status status = kv_instance_.Get(next_table_id_key, next_table_id_str);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Fail to get next table id from kv store, key: {}, cause: {}", next_table_id_key, status.message()));
    }
    u64 next_table_id = std::stoull(next_table_id_str);
    ++next_table_id;
    String new_next_table_id_str = std::to_string(next_table_id);
    status = kv_instance_.Put(next_table_id_key, new_next_table_id_str);
    if (!status.ok()) {
        return {"", status};
    }
    return {next_table_id_str, Status::OK()};
}

Status DBMeeta::LoadComment() {
    String comment;
    String db_comment_key = GetDBTag("comment");
    Status status = kv_instance_.Get(db_comment_key, comment);
    if (!status.ok() && status.code() != ErrorCode::kNotFound) { // "comment" not found is ok
        return status;
    }
    comment_ = std::move(comment);
    return Status::OK();
}

Status DBMeeta::LoadTableIDs() {
    table_id_strs_ = Vector<String>();
    table_names_ = Vector<String>();

    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str_);

    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(db_table_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(db_table_prefix)) {
        String key_str = iter2->Key().ToString();
        size_t start = db_table_prefix.size();
        size_t end = key_str.find('|', start);
        String table_id_str = iter2->Value().ToString();
        table_names_->emplace_back(key_str.substr(start, end - start));
        table_id_strs_->emplace_back(table_id_str);
        iter2->Next();
    }

    return Status::OK();
}

String DBMeeta::GetDBTag(const String &tag) const { return KeyEncode::CatalogDbTagKey(db_id_str_, tag); }

Status DBMeeta::SetNextTableID(const String &table_id_str) {
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    Status status = kv_instance_.Put(next_table_id_key, table_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

} // namespace infinity
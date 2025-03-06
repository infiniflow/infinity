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

namespace infinity {

DBMeeta::DBMeeta(String db_id_str, KVInstance &kv_instance) : db_id_str_(std::move(db_id_str)), kv_instance_(kv_instance) {}

Status DBMeeta::InitSet() {
    //
    return Status::OK();
}

Status DBMeeta::UninitSet() {
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

    return Status::OK();
}

Status DBMeeta::GetTableID(const String &table_name, String &table_key, String &table_id_str) {
    String table_key_prefix = KeyEncode::CatalogTablePrefix(db_id_str_, table_name);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(table_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_table_keys;
    while (iter2->Valid() && iter2->Key().starts_with(table_key_prefix)) {
        table_key = iter2->Key().ToString();
        table_id_str = iter2->Value().ToString();
        if (found_count > 0) {
            // Error branch
            error_table_keys.push_back(table_key);
        }
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No table, ignore it.
        return Status::TableNotExist(table_name);
    }

    if (!error_table_keys.empty()) {
        // join error_table_keys
        String error_table_keys_str =
            std::accumulate(std::next(error_table_keys.begin()), error_table_keys.end(), error_table_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple table keys: {}", error_table_keys_str));
    }

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

} // namespace infinity
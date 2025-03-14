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

#include <vector>

module catalog_meta;

import kv_code;
import kv_store;
import infinity_exception;
import third_party;

namespace infinity {

CatalogMeta::CatalogMeta(KVInstance &kv_instance) : kv_instance_(kv_instance) {}

Status CatalogMeta::GetDBID(const String &db_name, String &db_key, String &db_id) {
    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(db_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_db_keys;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        if (found_count > 0) {
            // Error branch
            error_db_keys.push_back(db_key);
        }
        db_key = iter2->Key().ToString();
        db_id = iter2->Value().ToString();
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        // No table, ignore it.
        return Status::DBNotExist(db_name);
    }

    if (!error_db_keys.empty()) {
        error_db_keys.push_back(db_key);
        // join error_db_keys
        String error_db_keys_str =
            std::accumulate(std::next(error_db_keys.begin()), error_db_keys.end(), error_db_keys.front(), [](String a, String b) {
                return a + ", " + b;
            });
        UnrecoverableError(fmt::format("Found multiple database keys: {}", error_db_keys_str));
    }

    return Status::OK();
}

Status CatalogMeta::LoadDBIDs() {
    Vector<String> db_id_strs;
    Vector<String> db_names;

    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(KeyEncode::kCatalogDbHeader);

    while (iter2->Valid() && iter2->Key().starts_with(KeyEncode::kCatalogDbHeader)) {
        String key_str = iter2->Key().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        db_id_strs.emplace_back(iter2->Value().ToString());
        db_names.emplace_back(key_str.substr(start, end - start));
        iter2->Next();
    }
    db_id_strs_ = std::move(db_id_strs);
    db_names_ = std::move(db_names);

    return Status::OK();
}

} // namespace infinity
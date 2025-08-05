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

module infinity_core:meta_cache.impl;

import :stl;
import :meta_cache;
import :lru;

namespace infinity {

MetaCache::MetaCache(SizeT capacity) : lru_cache_(capacity) {}

void MetaCache::Put(const String &key, SharedPtr<BaseMeta> meta) { lru_cache_.set(key, std::move(meta)); }

void MetaCache::Erase(const String &key) { lru_cache_.unset(key); }

SharedPtr<BaseMeta> MetaCache::Get(const String &key) { return lru_cache_.maybe_get(key).value_or(nullptr); }

Vector<String> MetaCache::GetAllKeys() const { return lru_cache_.get_all_keys(); }

String MetaCache::DbKey(const String &db_name) { return fmt::format("db|{}", db_name); }

String MetaCache::TableKey(const String &db_name, const String &table_name) { return fmt::format("table|{}|{}", db_name, table_name); }

String MetaCache::TableIndexKey(const String &db_name, const String &table_name, const String &index_name) {
    return fmt::format("table_index|{}|{}|{}", db_name, table_name, index_name);
}

} // namespace infinity
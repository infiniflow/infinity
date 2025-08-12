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

#include <iterator>

module infinity_core:meta_cache.impl;

import :meta_cache;
import :stl;
import :infinity_exception;
import :kv_code;
import :logger;
import :kv_store;

namespace infinity {

String MetaDbCache::name() { return db_name_; }
u64 MetaDbCache::commit_ts() { return commit_ts_; }
String MetaDbCache::type() { return "Database"; }
String MetaDbCache::detail() {
    if (is_dropped_) {
        return "dropped";
    } else {
        return "created";
    }
}

String MetaTableCache::name() { return table_name_; }
u64 MetaTableCache::commit_ts() { return commit_ts_; }
String MetaTableCache::type() { return "Table"; }
String MetaTableCache::detail() {
    String extend = is_dropped_ ? "dropped" : "created";
    return fmt::format("db_id: {}, table_name: {}, table_id: {}, {}", db_id_, table_name_, table_id_, extend);
}

String MetaIndexCache::name() { return index_name_; }
u64 MetaIndexCache::commit_ts() { return commit_ts_; }
String MetaIndexCache::type() { return "Index"; }
String MetaIndexCache::detail() {
    String extend = is_dropped_ ? "dropped" : "created";
    return fmt::format("db_id: {}, table_id: {}, index_name: {}, index_id: {}, {}", db_id_, table_id_, table_id_, index_name_, index_id_, extend);
}

Status MetaCache::PutOrErase(const Vector<SharedPtr<MetaBaseCache>> &cache_items, KVInstance *kv_instance) {
    std::unique_lock lock(cache_mtx_);
    for (const auto &cache_item : cache_items) {
        PutOrEraseNolock(cache_item);
    }
    if (kv_instance != nullptr) {
        return kv_instance->Commit();
    }
    return Status::OK();
}

void MetaCache::PutOrEraseNolock(const SharedPtr<MetaBaseCache> &meta_base_cache) {
    switch (meta_base_cache->type_) {
        case MetaCacheType::kCreateDB: {
            SharedPtr<MetaDbCache> db_cache = std::static_pointer_cast<MetaDbCache>(meta_base_cache);
            PutDbNolock(db_cache);
            break;
        }
        case MetaCacheType::kCreateTable: {
            SharedPtr<MetaTableCache> table_cache = std::static_pointer_cast<MetaTableCache>(meta_base_cache);
            PutTableNolock(table_cache);
            break;
        }
        case MetaCacheType::kCreateIndex: {
            SharedPtr<MetaIndexCache> index_cache = std::static_pointer_cast<MetaIndexCache>(meta_base_cache);
            PutIndexNolock(index_cache);
            break;
        }
        case MetaCacheType::kEraseDB: {
            SharedPtr<MetaEraseDbCache> drop_cache = std::static_pointer_cast<MetaEraseDbCache>(meta_base_cache);
            EraseDbNolock(drop_cache->db_name_);
            break;
        }
        case MetaCacheType::kEraseTable: {
            SharedPtr<MetaEraseTableCache> drop_cache = std::static_pointer_cast<MetaEraseTableCache>(meta_base_cache);
            EraseTableNolock(drop_cache->db_id_, drop_cache->table_name_);
            break;
        }
        case MetaCacheType::kEraseIndex: {
            SharedPtr<MetaEraseIndexCache> drop_cache = std::static_pointer_cast<MetaEraseIndexCache>(meta_base_cache);
            EraseIndexNolock(drop_cache->db_id_, drop_cache->table_id_, drop_cache->index_name_);
            break;
        }
        default: {
            UnrecoverableError("Invalid meta type");
        }
    }
}

void MetaCache::PutDbNolock(const SharedPtr<MetaDbCache> &db_cache) {
    String name = KeyEncode::CatalogDbPrefix(db_cache->db_name_);
    TxnTimeStamp commit_ts = db_cache->commit_ts_;

    auto iter = dbs_.find(name);
    if (iter != dbs_.end()) {
        Map<u64, List<CacheItem>::iterator> &db_map_ref = iter->second;
        auto cache_iter = db_map_ref.find(commit_ts);
        if (cache_iter != db_map_ref.end()) {
            TouchNolock(cache_iter->second);
            cache_iter->second->meta_cache_ = db_cache;
            return;
        }
    }

    // db is not in map
    lru_.push_front({name, db_cache});
    dbs_[name][commit_ts] = lru_.begin();
    TrimCacheNolock();
}

SharedPtr<MetaDbCache> MetaCache::GetDb(const String &db_name, TxnTimeStamp begin_ts) {
    String name = KeyEncode::CatalogDbPrefix(db_name);
    std::unique_lock lock(cache_mtx_);
    auto iter = dbs_.find(name);
    if (iter != dbs_.end()) {
        Map<u64, List<CacheItem>::iterator> &db_map_ref = iter->second;
        for (auto r_cache_iter = db_map_ref.rbegin(); r_cache_iter != db_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaDbCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

void MetaCache::EraseDbNolock(const String &db_name) {
    String name = KeyEncode::CatalogDbPrefix(db_name);
    dbs_.erase(name);
}

void MetaCache::PutTableNolock(const SharedPtr<MetaTableCache> &table_cache) {
    const String &table_name = table_cache->table_name_;
    u64 db_id = table_cache->db_id_;
    String name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);
    TxnTimeStamp commit_ts = table_cache->commit_ts_;

    auto iter = tables_.find(name);
    if (iter != tables_.end()) {
        Map<u64, List<CacheItem>::iterator> &db_map_ref = iter->second;
        auto cache_iter = db_map_ref.find(commit_ts);
        if (cache_iter != db_map_ref.end()) {
            TouchNolock(cache_iter->second);
            cache_iter->second->meta_cache_ = table_cache;
            return;
        }
    }

    // table is not in map
    lru_.push_front({name, table_cache});
    tables_[name][commit_ts] = lru_.begin();
    TrimCacheNolock();
}

SharedPtr<MetaTableCache> MetaCache::GetTable(u64 db_id, const String &table_name, TxnTimeStamp begin_ts) {
    String name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);

    std::unique_lock lock(cache_mtx_);
    auto iter = tables_.find(name);
    if (iter != tables_.end()) {
        Map<u64, List<CacheItem>::iterator> &table_map_ref = iter->second;
        for (auto r_cache_iter = table_map_ref.rbegin(); r_cache_iter != table_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaTableCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

void MetaCache::EraseTableNolock(u64 db_id, const String &table_name) {
    String name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);
    tables_.erase(name);
}

void MetaCache::PutIndexNolock(const SharedPtr<MetaIndexCache> &index_cache) {
    const String &index_name = index_cache->index_name_;
    u64 db_id = index_cache->db_id_;
    u64 table_id = index_cache->table_id_;
    String name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);
    TxnTimeStamp commit_ts = index_cache->commit_ts_;

    auto iter = indexes_.find(name);
    if (iter != indexes_.end()) {
        Map<u64, List<CacheItem>::iterator> &db_map_ref = iter->second;
        auto cache_iter = db_map_ref.find(commit_ts);
        if (cache_iter != db_map_ref.end()) {
            TouchNolock(cache_iter->second);
            cache_iter->second->meta_cache_ = index_cache;
            return;
        }
    }

    // table is not in map
    lru_.push_front({name, index_cache});
    indexes_[name][commit_ts] = lru_.begin();
    TrimCacheNolock();
}

void MetaCache::EraseIndexNolock(u64 db_id, u64 table_id, const String &index_name) {
    String name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);
    indexes_.erase(name);
}

SharedPtr<MetaIndexCache> MetaCache::GetIndex(u64 db_id, u64 table_id, const String &index_name, TxnTimeStamp begin_ts) {
    String name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);

    std::unique_lock lock(cache_mtx_);
    auto iter = indexes_.find(name);
    if (iter != indexes_.end()) {
        Map<u64, List<CacheItem>::iterator> &index_map_ref = iter->second;
        for (auto r_cache_iter = index_map_ref.rbegin(); r_cache_iter != index_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaIndexCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

void MetaCache::PrintLRU() const {
    std::unique_lock lock(cache_mtx_);
    for (const auto &item : lru_) {
        TxnTimeStamp commit_ts = 0;
        switch (item.meta_cache_->type_) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                commit_ts = db_cache->commit_ts_;
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                commit_ts = table_cache->commit_ts_;
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                commit_ts = index_cache->commit_ts_;
                break;
            }
            default: {
                UnrecoverableError("Invalid meta type");
            }
        }
        LOG_INFO(fmt::format("name: {}, commit: {}", item.name_, commit_ts));
    }
}

void MetaCache::TrimCacheNolock() {
    while (lru_.size() > capacity_) {
        CacheItem &item = lru_.back();
        switch (item.meta_cache_->type_) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                auto iter = dbs_.find(item.name_);
                if (iter != dbs_.end()) {
                    iter->second.erase(db_cache->commit_ts_);
                }
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                auto iter = tables_.find(item.name_);
                if (iter != tables_.end()) {
                    iter->second.erase(table_cache->commit_ts_);
                }
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                auto iter = indexes_.find(item.name_);
                if (iter != indexes_.end()) {
                    iter->second.erase(index_cache->commit_ts_);
                }
                break;
            }
            default: {
                UnrecoverableError("Invalid meta type");
            }
        }
        lru_.pop_back();
    }
}

SizeT MetaCache::Size() const {
    std::unique_lock lock(cache_mtx_);
    return lru_.size();
}

Vector<SharedPtr<MetaBaseCache>> MetaCache::GetAllCacheItems() const {
    Vector<SharedPtr<MetaBaseCache>> result;
    std::unique_lock lock(cache_mtx_);
    result.reserve(lru_.size());
    for (const auto &item : lru_) {
        switch (item.meta_cache_->type_) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                auto iter = dbs_.find(item.name_);
                if (iter != dbs_.end()) {
                    if (iter->second.contains(db_cache->commit_ts_)) {
                        result.push_back(item.meta_cache_);
                    }
                }
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                auto iter = tables_.find(item.name_);
                if (iter != tables_.end()) {
                    if (iter->second.contains(table_cache->commit_ts_)) {
                        result.push_back(item.meta_cache_);
                    }
                }
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                auto iter = indexes_.find(item.name_);
                if (iter != tables_.end()) {
                    if (iter->second.contains(index_cache->commit_ts_)) {
                        result.push_back(item.meta_cache_);
                    }
                }
                break;
            }
            default: {
                UnrecoverableError("Invalid meta type");
            }
        }
    }
    return result;
}

void MetaCache::TouchNolock(List<CacheItem>::iterator iter) { lru_.splice(lru_.begin(), lru_, iter); }

} // namespace infinity

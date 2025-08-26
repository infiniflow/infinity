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

import :meta_cache;
import std;
import :infinity_exception;
import :kv_code;
import :logger;
import :kv_store;

namespace infinity {

std::string ToString(MetaCacheType type) {
    switch (type) {
        case MetaCacheType::kCreateDB: {
            return "database";
        }
        case MetaCacheType::kCreateTable: {
            return "table";
        }
        case MetaCacheType::kCreateIndex: {
            return "index";
        }
        default: {
            UnrecoverableError("Invalid meta type");
        }
    }
    return std::string();
}

MetaCacheType MetaBaseCache::type() const {
    std::unique_lock lock(mtx_);
    return type_;
}

MetaDbCache::~MetaDbCache() = default;

std::string MetaDbCache::name() const {
    std::unique_lock lock(mtx_);
    return db_name_;
}
u64 MetaDbCache::db_id() const {
    std::unique_lock lock(mtx_);
    return db_id_;
}
const std::string &MetaDbCache::db_key() const {
    std::unique_lock lock(mtx_);
    return db_key_;
}
u64 MetaDbCache::commit_ts() const {
    std::unique_lock lock(mtx_);
    return commit_ts_;
}
std::string MetaDbCache::detail() const {
    std::unique_lock lock(mtx_);
    std::string extend = is_dropped_ ? "dropped" : "created";
    return fmt::format("db_name: {}, db_id: {}, commit_ts: {},  {}", db_name_, db_id_, commit_ts_, extend);
}
bool MetaDbCache::is_dropped() const {
    std::unique_lock lock(mtx_);
    return is_dropped_;
}
void MetaDbCache::set_comment(const std::shared_ptr<std::string> &comment) {
    std::unique_lock lock(mtx_);
    get_comment_ = true;
    comment_ = comment;
}
bool MetaDbCache::get_comment() const {
    std::unique_lock lock(mtx_);
    return get_comment_;
}
std::shared_ptr<std::string> MetaDbCache::comment() const {
    std::unique_lock lock(mtx_);
    return comment_;
}

std::string MetaTableCache::name() const {
    std::unique_lock lock(mtx_);
    return table_name_;
}
u64 MetaTableCache::db_id() const {
    std::unique_lock lock(mtx_);
    return db_id_;
}
u64 MetaTableCache::table_id() const {
    std::unique_lock lock(mtx_);
    return table_id_;
}
const std::string &MetaTableCache::table_key() const {
    std::unique_lock lock(mtx_);
    return table_key_;
}
u64 MetaTableCache::commit_ts() const {
    std::unique_lock lock(mtx_);
    return commit_ts_;
}
std::string MetaTableCache::detail() const {
    std::unique_lock lock(mtx_);
    std::string extend = is_dropped_ ? "dropped" : "created";
    return fmt::format("db_id: {}, table_name: {}, table_id: {}, commit_ts: {}, {}", db_id_, table_name_, table_id_, commit_ts_, extend);
}
bool MetaTableCache::is_dropped() const {
    std::unique_lock lock(mtx_);
    return is_dropped_;
}

std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> MetaTableCache::get_columns() const {
    std::unique_lock lock(mtx_);
    return columns_;
}

void MetaTableCache::set_columns(const std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> &columns) {
    std::unique_lock lock(mtx_);
    columns_ = columns;
}

std::shared_ptr<std::vector<SegmentID>> MetaTableCache::get_segments() const {
    std::unique_lock lock(mtx_);
    return segment_ids_;
}

void MetaTableCache::set_segments(const std::shared_ptr<std::vector<SegmentID>> &segments) {
    std::unique_lock lock(mtx_);
    segment_ids_ = segments;
}

std::tuple<std::shared_ptr<std::vector<std::string>>, std::shared_ptr<std::vector<std::string>>> MetaTableCache::get_index_ids() const {
    std::unique_lock lock(mtx_);
    return {index_ids_ptr_, index_names_ptr_};
}

void MetaTableCache::set_index_ids(const std::shared_ptr<std::vector<std::string>> &index_ids,
                                   const std::shared_ptr<std::vector<std::string>> &index_names) {
    std::unique_lock lock(mtx_);

    index_ids_ptr_ = index_ids;
    index_names_ptr_ = index_names;
}

void MetaTableCache::set_segment_tag(SegmentID segment_id, const std::string &tag, u64 value) {
    std::unique_lock lock(mtx_);
    segment2tag2value_[segment_id][tag] = value;
}

std::optional<u64> MetaTableCache::get_segment_tag(SegmentID segment_id, const std::string &tag) {
    std::unique_lock lock(mtx_);
    auto segment_iter = segment2tag2value_.find(segment_id);
    if (segment_iter == segment2tag2value_.end()) {
        return std::nullopt;
    }
    auto tag_iter = segment_iter->second.find(tag);
    if (tag_iter == segment_iter->second.end()) {
        return std::nullopt;
    }
    return tag_iter->second;
}

std::string MetaIndexCache::name() const {
    std::unique_lock lock(mtx_);
    return index_name_;
}

u64 MetaIndexCache::db_id() const {
    std::unique_lock lock(mtx_);
    return db_id_;
}

u64 MetaIndexCache::table_id() const {
    std::unique_lock lock(mtx_);
    return table_id_;
}

u64 MetaIndexCache::index_id() const {
    std::unique_lock lock(mtx_);
    return index_id_;
}

u64 MetaIndexCache::commit_ts() const {
    std::unique_lock lock(mtx_);
    return commit_ts_;
}

const std::string &MetaIndexCache::index_key() const {
    std::unique_lock lock(mtx_);
    return index_key_;
}

std::shared_ptr<IndexBase> MetaIndexCache::get_index_def() const {
    std::unique_lock lock(mtx_);
    return index_def_;
}

void MetaIndexCache::set_index_def(const std::shared_ptr<IndexBase> &index_def_ptr) {
    std::unique_lock lock(mtx_);
    index_def_ = index_def_ptr;
}

std::string MetaIndexCache::detail() const {
    std::unique_lock lock(mtx_);
    std::string extend = is_dropped_ ? "dropped" : "created";
    return fmt::format("db_id: {}, table_id: {}, index_name: {}, index_id: {}, commit_ts: {}, {}",
                       db_id_,
                       table_id_,
                       table_id_,
                       index_name_,
                       index_id_,
                       commit_ts_,
                       extend);
}
bool MetaIndexCache::is_dropped() const {
    std::unique_lock lock(mtx_);
    return is_dropped_;
}

void MetaCache::Put(const std::vector<std::shared_ptr<MetaBaseCache>> &cache_items, TxnTimeStamp begin_ts) {
    std::unique_lock lock(cache_mtx_);
    if (begin_ts > latest_erased_ts_) {
        // When put txn's begin ts should be larger than the erased ts by erased txn.
        for (const auto &cache_item : cache_items) {
            switch (cache_item->type()) {
                case MetaCacheType::kCreateDB: {
                    std::shared_ptr<MetaDbCache> db_cache = std::static_pointer_cast<MetaDbCache>(cache_item);
                    PutDbNolock(db_cache);
                    break;
                }
                case MetaCacheType::kCreateTable: {
                    std::shared_ptr<MetaTableCache> table_cache = std::static_pointer_cast<MetaTableCache>(cache_item);
                    PutTableNolock(table_cache);
                    break;
                }
                case MetaCacheType::kCreateIndex: {
                    std::shared_ptr<MetaIndexCache> index_cache = std::static_pointer_cast<MetaIndexCache>(cache_item);
                    PutIndexNolock(index_cache);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid meta type");
                }
            }
        }
        LOG_TRACE(fmt::format("Put cache items: {}", cache_items.size()));
    }
}

Status MetaCache::Erase(const std::vector<std::shared_ptr<EraseBaseCache>> &cache_items, KVInstance *kv_instance, TxnTimeStamp commit_ts) {
    std::unique_lock lock(cache_mtx_);
    for (const auto &cache_item : cache_items) {
        switch (cache_item->type_) {
            case EraseCacheType::kEraseDB: {
                MetaEraseDbCache *db_cache_ptr = static_cast<MetaEraseDbCache *>(cache_item.get());
                EraseDbNolock(db_cache_ptr->db_name_);
                break;
            }
            case EraseCacheType::kEraseTable: {
                MetaEraseTableCache *table_cache_ptr = static_cast<MetaEraseTableCache *>(cache_item.get());
                EraseTableNolock(table_cache_ptr->db_id_, table_cache_ptr->table_name_);
                break;
            }
            case EraseCacheType::kEraseIndex: {
                MetaEraseIndexCache *index_cache_ptr = static_cast<MetaEraseIndexCache *>(cache_item.get());
                EraseIndexNolock(index_cache_ptr->db_id_, index_cache_ptr->table_id_, index_cache_ptr->index_name_);
                break;
            }
            default: {
                UnrecoverableError("Invalid meta type");
            }
        }
    }

    if (!cache_items.empty()) {
        latest_erased_ts_ = commit_ts;
    }

    if (kv_instance != nullptr) {
        return kv_instance->Commit();
    }

    return Status::OK();
}

Status MetaCache::PutOrErase(const std::vector<std::shared_ptr<MetaBaseCache>> &cache_items, KVInstance *kv_instance) {
    std::unique_lock lock(cache_mtx_);
    for (const auto &cache_item : cache_items) {
        PutOrEraseNolock(cache_item);
    }
    if (kv_instance != nullptr) {
        return kv_instance->Commit();
    }
    return Status::OK();
}

void MetaCache::PutOrEraseNolock(const std::shared_ptr<MetaBaseCache> &meta_base_cache) {
    switch (meta_base_cache->type()) {
        case MetaCacheType::kCreateDB: {
            std::shared_ptr<MetaDbCache> db_cache = std::static_pointer_cast<MetaDbCache>(meta_base_cache);
            PutDbNolock(db_cache);
            break;
        }
        case MetaCacheType::kCreateTable: {
            std::shared_ptr<MetaTableCache> table_cache = std::static_pointer_cast<MetaTableCache>(meta_base_cache);
            PutTableNolock(table_cache);
            break;
        }
        case MetaCacheType::kCreateIndex: {
            std::shared_ptr<MetaIndexCache> index_cache = std::static_pointer_cast<MetaIndexCache>(meta_base_cache);
            PutIndexNolock(index_cache);
            break;
        }
        default: {
            UnrecoverableError("Invalid meta type");
        }
    }
}

void MetaCache::PutDbNolock(const std::shared_ptr<MetaDbCache> &db_cache) {
    std::string name = KeyEncode::CatalogDbPrefix(db_cache->name());
    TxnTimeStamp commit_ts = db_cache->commit_ts();

    auto iter = dbs_.find(name);
    if (iter != dbs_.end()) {
        std::map<u64, std::list<CacheItem>::iterator> &db_map_ref = iter->second;
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

std::shared_ptr<MetaDbCache> MetaCache::GetDb(const std::string &db_name, TxnTimeStamp begin_ts) {
    std::string name = KeyEncode::CatalogDbPrefix(db_name);
    std::unique_lock lock(cache_mtx_);
    ++db_request_count_;
    auto iter = dbs_.find(name);
    if (iter != dbs_.end()) {
        std::map<u64, std::list<CacheItem>::iterator> &db_map_ref = iter->second;
        for (auto r_cache_iter = db_map_ref.rbegin(); r_cache_iter != db_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                ++db_hit_count_;
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaDbCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

void MetaCache::EraseDbNolock(const std::string &db_name) {
    std::string name = KeyEncode::CatalogDbPrefix(db_name);
    dbs_.erase(name);
    LOG_TRACE(fmt::format("Erase db name: {}", name));
    for (const auto &db : dbs_) {
        LOG_TRACE(fmt::format("cached db: {}", db.first));
    }
}

void MetaCache::PutTableNolock(const std::shared_ptr<MetaTableCache> &table_cache) {
    const std::string &table_name = table_cache->name();
    u64 db_id = table_cache->db_id();
    std::string name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);
    TxnTimeStamp commit_ts = table_cache->commit_ts();

    auto iter = tables_.find(name);
    if (iter != tables_.end()) {
        std::map<u64, std::list<CacheItem>::iterator> &db_map_ref = iter->second;
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

std::shared_ptr<MetaTableCache> MetaCache::GetTable(u64 db_id, const std::string &table_name, TxnTimeStamp begin_ts) {
    std::string name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);

    std::unique_lock lock(cache_mtx_);
    ++table_request_count_;
    auto iter = tables_.find(name);
    if (iter != tables_.end()) {
        LOG_TRACE(fmt::format("Find table cache name: {}", name));
        std::map<u64, std::list<CacheItem>::iterator> &table_map_ref = iter->second;
        for (auto r_cache_iter = table_map_ref.rbegin(); r_cache_iter != table_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                ++table_hit_count_;
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaTableCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

void MetaCache::EraseTableNolock(u64 db_id, const std::string &table_name) {
    std::string name = KeyEncode::CatalogTablePrefix(std::to_string(db_id), table_name);
    tables_.erase(name);
    LOG_TRACE(fmt::format("Erase table name: {}", name));
}

void MetaCache::PutIndexNolock(const std::shared_ptr<MetaIndexCache> &index_cache) {
    const std::string &index_name = index_cache->name();
    u64 db_id = index_cache->db_id();
    u64 table_id = index_cache->table_id();
    std::string name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);
    TxnTimeStamp commit_ts = index_cache->commit_ts();

    auto iter = indexes_.find(name);
    if (iter != indexes_.end()) {
        std::map<u64, std::list<CacheItem>::iterator> &db_map_ref = iter->second;
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

void MetaCache::EraseIndexNolock(u64 db_id, u64 table_id, const std::string &index_name) {
    std::string name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);
    indexes_.erase(name);
    LOG_TRACE(fmt::format("Erase index name: {}", name));
}

std::shared_ptr<MetaIndexCache> MetaCache::GetIndex(u64 db_id, u64 table_id, const std::string &index_name, TxnTimeStamp begin_ts) {
    std::string name = KeyEncode::CatalogIndexPrefix(std::to_string(db_id), std::to_string(table_id), index_name);

    std::unique_lock lock(cache_mtx_);
    ++index_request_count_;
    auto iter = indexes_.find(name);
    if (iter != indexes_.end()) {
        std::map<u64, std::list<CacheItem>::iterator> &index_map_ref = iter->second;
        for (auto r_cache_iter = index_map_ref.rbegin(); r_cache_iter != index_map_ref.rend(); ++r_cache_iter) {
            TxnTimeStamp commit_ts = r_cache_iter->first;
            if (begin_ts > commit_ts) {
                ++index_hit_count_;
                TouchNolock(r_cache_iter->second);
                return std::static_pointer_cast<MetaIndexCache>(r_cache_iter->second->meta_cache_);
            }
        }
    }
    return nullptr;
}

CacheStatus MetaCache::GetCacheStatus(MetaCacheType cache_type) const {
    std::unique_lock lock(cache_mtx_);
    switch (cache_type) {
        case MetaCacheType::kCreateDB: {
            return {dbs_.size(), db_request_count_, db_hit_count_};
        }
        case MetaCacheType::kCreateTable: {
            return {tables_.size(), table_request_count_, table_hit_count_};
        }
        case MetaCacheType::kCreateIndex: {
            return {indexes_.size(), index_request_count_, index_hit_count_};
        }
        default: {
            UnrecoverableError("Unexpected error");
        }
    }
    return {};
}

void MetaCache::PrintLRU() const {
    std::unique_lock lock(cache_mtx_);
    for (const auto &item : lru_) {
        TxnTimeStamp commit_ts = 0;
        switch (item.meta_cache_->type()) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                commit_ts = db_cache->commit_ts();
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                commit_ts = table_cache->commit_ts();
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                commit_ts = index_cache->commit_ts();
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
        switch (item.meta_cache_->type()) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                auto iter = dbs_.find(item.name_);
                if (iter != dbs_.end()) {
                    iter->second.erase(db_cache->commit_ts());
                }
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                auto iter = tables_.find(item.name_);
                if (iter != tables_.end()) {
                    iter->second.erase(table_cache->commit_ts());
                }
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                auto iter = indexes_.find(item.name_);
                if (iter != indexes_.end()) {
                    iter->second.erase(index_cache->commit_ts());
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

size_t MetaCache::Size() const {
    std::unique_lock lock(cache_mtx_);
    return lru_.size();
}

std::vector<std::shared_ptr<MetaBaseCache>> MetaCache::GetAllCacheItems() const {
    std::vector<std::shared_ptr<MetaBaseCache>> result;
    std::unique_lock lock(cache_mtx_);
    result.reserve(lru_.size());
    for (const auto &item : lru_) {
        switch (item.meta_cache_->type()) {
            case MetaCacheType::kCreateDB: {
                MetaDbCache *db_cache = static_cast<MetaDbCache *>(item.meta_cache_.get());
                auto iter = dbs_.find(item.name_);
                if (iter != dbs_.end()) {
                    if (iter->second.contains(db_cache->commit_ts())) {
                        result.push_back(item.meta_cache_);
                    }
                }
                break;
            }
            case MetaCacheType::kCreateTable: {
                MetaTableCache *table_cache = static_cast<MetaTableCache *>(item.meta_cache_.get());
                auto iter = tables_.find(item.name_);
                if (iter != tables_.end()) {
                    if (iter->second.contains(table_cache->commit_ts())) {
                        result.push_back(item.meta_cache_);
                    }
                }
                break;
            }
            case MetaCacheType::kCreateIndex: {
                MetaIndexCache *index_cache = static_cast<MetaIndexCache *>(item.meta_cache_.get());
                auto iter = indexes_.find(item.name_);
                if (iter != tables_.end()) {
                    if (iter->second.contains(index_cache->commit_ts())) {
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

void MetaCache::TouchNolock(std::list<CacheItem>::iterator iter) { lru_.splice(lru_.begin(), lru_, iter); }

} // namespace infinity

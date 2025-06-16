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

#include <filesystem>
#include <string>
module new_catalog;

import stl;
import third_party;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import third_party;
import logger;
import infinity_exception;
import default_values;
import mem_index;
import column_index_reader;
import data_type;
import parsed_expr;
import constant_expr;
import meta_key;
import db_meeta;
import table_def;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_entry;
import table_index_meeta;
import index_secondary;
import index_ivf;
import index_hnsw;
import index_full_text;
import index_bmp;
import index_emvb;
import kv_code;
import config;
import create_index_info;
import statement_common;
import virtual_store;
import logger;
import utility;
import db_entry;
import table_entry;
import segment_entry;
import block_entry;
import block_column_entry;
import buffer_manager;
import infinity_context;
import segment_index_meta;
import chunk_index_meta;
import fast_rough_filter;
import persistence_manager;
import meta_key;
import catalog_cache;
import segment_index_entry;
import chunk_index_entry;
import table_index_meta;
import meta_type;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

NewCatalog::~NewCatalog() = default;

Status NewCatalog::Init(KVStore *kv_store) {
    auto kv_instance = kv_store->GetInstance();
    String db_string_id;
    Status status = kv_instance->Get(NEXT_DATABASE_ID.data(), db_string_id);
    if (!status.ok()) {
        kv_instance->Put(NEXT_DATABASE_ID.data(), "0");
    }
    status = kv_instance->Commit();
    if (!status.ok()) {
        UnrecoverableError("Can't initialize latest ID");
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>());
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key, TxnTimeStamp checkpoint_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>(checkpoint_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock) {
    block_lock = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(block_lock_mtx_);
        if (auto iter = block_lock_map_.find(block_key); iter != block_lock_map_.end()) {
            block_lock = iter->second;
        }
    }
    if (block_lock == nullptr) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::DropBlockLockByBlockKey(const String &block_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(block_lock_mtx_);
        delete_success = block_lock_map_.erase(block_key) > 0;
    }
    if (!delete_success) {
        LOG_WARN(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

SharedPtr<MemIndex> NewCatalog::GetMemIndex(const String &mem_index_key) {
    std::shared_lock<std::shared_mutex> lck(mem_index_mtx_);
    if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
        return iter->second;
    }
    return nullptr;
}

bool NewCatalog::GetOrSetMemIndex(const String &mem_index_key, SharedPtr<MemIndex> &mem_index) {
    std::unique_lock<std::shared_mutex> lck(mem_index_mtx_);
    if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
        mem_index = iter->second;
        return false;
    }
    mem_index_map_.emplace(mem_index_key, mem_index);
    return true;
}

Status NewCatalog::DropMemIndexByMemIndexKey(const String &mem_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(mem_index_mtx_);
        delete_success = mem_index_map_.erase(mem_index_key) > 0;
    }
    if (!delete_success) {
        LOG_WARN(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Vector<Pair<String, String>> NewCatalog::GetAllMemIndexInfo() {
    Vector<Pair<String, String>> result;
    {
        std::unique_lock lock(mem_index_mtx_);
        for (const auto &mem_index_pair : mem_index_map_) {
            if (mem_index_pair.second->memory_hnsw_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "hnsw"});
                continue;
            }
            if (mem_index_pair.second->memory_ivf_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "ivf"});
                continue;
            }
            if (mem_index_pair.second->memory_indexer_ != nullptr) {
                result.push_back({mem_index_pair.first, "full-text"});
                continue;
            }
            if (mem_index_pair.second->memory_secondary_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "secondary"});
                continue;
            }
            if (mem_index_pair.second->memory_emvb_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "emvb"});
                continue;
            }
            if (mem_index_pair.second->memory_bmp_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "bmp"});
                continue;
            }
            result.push_back({mem_index_pair.first, "empty"});
        }
    }

    return result;
}

Status NewCatalog::AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache) {
    bool insert_success = false;
    HashMap<String, SharedPtr<TableIndexReaderCache>>::iterator iter;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        std::tie(iter, insert_success) = ft_index_cache_map_.emplace(std::move(ft_index_cache_key), std::move(ft_index_cache));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache) {
    ft_index_cache = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(ft_index_cache_mtx_);
        if (auto iter = ft_index_cache_map_.find(ft_index_cache_key); iter != ft_index_cache_map_.end()) {
            ft_index_cache = iter->second;
        }
    }
    if (ft_index_cache == nullptr) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        delete_success = ft_index_cache_map_.erase(ft_index_cache_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::AddSegmentUpdateTS(String segment_update_ts_key, SharedPtr<SegmentUpdateTS> segment_update_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<SegmentUpdateTS>>::iterator iter;
    {
        std::unique_lock lock(segment_update_ts_mtx_);
        std::tie(iter, insert_success) = segment_update_ts_map_.emplace(std::move(segment_update_ts_key), std::move(segment_update_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentUpdateTS(const String &segment_update_ts_key, SharedPtr<SegmentUpdateTS> &segment_update_ts) {
    segment_update_ts = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(segment_update_ts_mtx_);
        if (auto iter = segment_update_ts_map_.find(segment_update_ts_key); iter != segment_update_ts_map_.end()) {
            segment_update_ts = iter->second;
        }
    }
    if (segment_update_ts == nullptr) {
        return Status::CatalogError(fmt::format("Get SegmentUpdateTS key: {} not found", segment_update_ts_key));
    }
    return Status::OK();
}

void NewCatalog::DropSegmentUpdateTSByKey(const String &segment_update_ts_key) {
    std::unique_lock lock(segment_update_ts_mtx_);
    segment_update_ts_map_.erase(segment_update_ts_key);
}

void NewCatalog::GetCleanedMeta(TxnTimeStamp ts, Vector<UniquePtr<MetaKey>> &metas, KVInstance *kv_instance) {
    auto GetCleanedMetaImpl = [&](const Vector<String> &keys) {
        const String &type_str = keys[1];
        const String &meta_str = keys[2];
        auto meta_infos = infinity::Partition(meta_str, '/');
        if (type_str == "db") {
            metas.emplace_back(MakeUnique<DBMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1])));
        } else if (type_str == "tbl") {
            metas.emplace_back(MakeUnique<TableMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1]), std::move(meta_infos[2])));
        } else if (type_str == "seg") {
            metas.emplace_back(MakeUnique<SegmentMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1]), std::stoull(meta_infos[2])));
        } else if (type_str == "blk") {
            metas.emplace_back(
                MakeUnique<BlockMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1]), std::stoull(meta_infos[2]), std::stoull(meta_infos[3])));
        } else if (type_str == "blk_col") {
            metas.emplace_back(MakeUnique<ColumnMetaKey>(std::move(meta_infos[0]),
                                                         std::move(meta_infos[1]),
                                                         std::stoull(meta_infos[2]),
                                                         std::stoull(meta_infos[3]),
                                                         ColumnDef::FromJson(nlohmann::json::parse(std::move(meta_infos[4])))));
        } else if (type_str == "idx") {
            metas.emplace_back(MakeUnique<TableIndexMetaKey>(std::move(meta_infos[0]),
                                                             std::move(meta_infos[1]),
                                                             std::move(meta_infos[2]),
                                                             std::move(meta_infos[3])));
        } else if (type_str == "idx_seg") {
            metas.emplace_back(MakeUnique<SegmentIndexMetaKey>(std::move(meta_infos[0]),
                                                               std::move(meta_infos[1]),
                                                               std::move(meta_infos[2]),
                                                               std::stoull(meta_infos[3])));
        } else if (type_str == "idx_chunk") {
            metas.emplace_back(MakeUnique<ChunkIndexMetaKey>(std::move(meta_infos[0]),
                                                             std::move(meta_infos[1]),
                                                             std::move(meta_infos[2]),
                                                             std::stoull(meta_infos[3]),
                                                             std::stoull(meta_infos[4])));
        } else {
            UnrecoverableError("Unknown meta key type.");
        }
    };

    constexpr std::string drop_prefix = "drop";
    auto iter = kv_instance->GetIterator();
    iter->Seek(drop_prefix);
    String drop_key, drop_ts_str;
    TxnTimeStamp drop_ts;
    Vector<String> drop_keys;

    while (iter->Valid() && iter->Key().starts_with(drop_prefix)) {
        drop_key = iter->Key().ToString();
        drop_ts_str = iter->Value().ToString();
        drop_ts = std::stoull(drop_ts_str); // It might not be an integer
        if (drop_ts <= ts) {
            drop_keys.emplace_back(drop_key);
        }
        iter->Next();
    }

    for (const auto &drop_key : drop_keys) {
        auto keys = infinity::Partition(drop_key, '|');
        GetCleanedMetaImpl(keys);

        // delete from kv_instance
        Status status = kv_instance->Delete(drop_key);
        if (!status.ok()) {
            UnrecoverableError(fmt::format("Remove clean meta failed. {}", *status.msg_));
        }
    }
    std::sort(metas.begin(), metas.end(), [&](const UniquePtr<MetaKey> &lhs, const UniquePtr<MetaKey> &rhs) {
        return static_cast<SizeT>(lhs->type_) > static_cast<SizeT>(rhs->type_);
    });
}

Status NewCatalog::IncrLatestID(String &id_str, std::string_view id_name) {
    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();
    Status s = kv_instance->Get(id_name.data(), id_str);
    if (!s.ok()) {
        kv_instance->Rollback();
        return s;
    }
    SizeT id_num = std::stoull(id_str);
    ++id_num;
    s = kv_instance->Put(id_name.data(), fmt::format("{}", id_num));
    if (!s.ok()) {
        kv_instance->Rollback();
        return s;
    }
    s = kv_instance->Commit();
    return s;
}

void NewCatalog::SetLastCleanupTS(TxnTimeStamp cleanup_ts) { last_cleanup_ts_ = cleanup_ts; }

TxnTimeStamp NewCatalog::GetLastCleanupTS() const { return last_cleanup_ts_; }

SharedPtr<MetaTree> NewCatalog::MakeMetaTree() const {
    auto entries = this->MakeMetaKeys();
    auto meta_tree_ptr = MetaTree::MakeMetaTree(entries);
    return meta_tree_ptr;
}

Vector<SharedPtr<MetaKey>> NewCatalog::MakeMetaKeys() const {
    auto kv_instance_ptr = kv_store_->GetInstance();
    auto all_key_values = kv_instance_ptr->GetAllKeyValue();
    auto meta_count = all_key_values.size();

    Vector<SharedPtr<MetaKey>> meta_keys;
    meta_keys.reserve(meta_count);

    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const auto &pair = all_key_values[idx];
        SharedPtr<MetaKey> meta_key = MetaParse(pair.first, pair.second);
        if (meta_key == nullptr) {
            LOG_ERROR(fmt::format("Can't parse {}: {}: {}", idx, pair.first, pair.second));
        } else {
            LOG_INFO(fmt::format("META[{}] KEY: {}", idx, meta_key->ToString()));
        }
        meta_keys.emplace_back(meta_key);
    }

    auto new_end = std::remove_if(meta_keys.begin(), meta_keys.end(), [&](const auto &meta_key) {
        if (meta_key->type_ == MetaType::kPmPath) {
            auto pm_path_key = static_cast<PmPathMetaKey *>(meta_key.get());
            nlohmann::json pm_path_json = nlohmann::json::parse(pm_path_key->value_);
            String object_key = pm_path_json["obj_key"];
            if (object_key == "KEY_EMPTY") {
                kv_instance_ptr->Delete(KeyEncode::PMObjectKey(pm_path_key->path_key_));
                return true;
            }
        }
        return false;
    });
    meta_keys.erase(new_end, meta_keys.end());

    kv_instance_ptr->Commit();
    return meta_keys;
}

Status NewCatalog::RestoreCatalogCache(Storage *storage_ptr) {
    LOG_INFO("Restore catalog cache");

    auto meta_tree = this->MakeMetaTree();
    // Vector<> = meta_tree->Check();
    // String meta_tree_str = meta_tree->ToJson().dump(4);
    // LOG_INFO(meta_tree_str);

    system_cache_ = meta_tree->RestoreSystemCache(storage_ptr);
    // Vector<MetaTableObject *> table_ptrs = meta_tree->ListTables();
    // for (const auto &table_ptr : table_ptrs) {
    //     SegmentID unsealed_segment_id = table_ptr->GetUnsealedSegmentID();
    //     SegmentID next_segment_id = table_ptr->GetNextSegmentID();
    //     SizeT current_segment_row_count = table_ptr->GetCurrentSegmentRowCount(storage_ptr);
    //     LOG_INFO(fmt::format("Table: {}, unsealed_segment_id: {}, next_segment_id: {}, current_segment_row_count: {}",
    //                          table_ptr->GetTableName(),
    //                          unsealed_segment_id,
    //                          next_segment_id,
    //                          current_segment_row_count));
    // }

    return Status::OK();
}

SharedPtr<SystemCache> NewCatalog::GetSystemCache() const { return system_cache_; }

SystemCache *NewCatalog::GetSystemCachePtr() const { return system_cache_.get(); }

KVStore *NewCatalog::kv_store() const { return kv_store_; }

} // namespace infinity

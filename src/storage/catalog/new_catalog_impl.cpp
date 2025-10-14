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

module infinity_core:new_catalog.impl;

import :new_catalog;
import :new_txn;
import :status;
import :kv_store;
import :logger;
import :infinity_exception;
import :default_values;
import :mem_index;
import :column_index_reader;
import :meta_key;
import :db_meta;
import :table_def;
import :table_meta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_index_meta;
import :index_secondary;
import :index_ivf;
import :index_hnsw;
import :index_full_text;
import :index_bmp;
import :index_emvb;
import :kv_code;
import :config;
import :virtual_store;
import :logger;
import :utility;
import :buffer_manager;
import :infinity_context;
import :fast_rough_filter;
import :persistence_manager;
import :meta_key;
import :catalog_cache;
import :meta_type;

import std;
import third_party;

import column_def;
import create_index_info;
import statement_common;
import data_type;
import parsed_expr;
import constant_expr;
import extra_ddl_info;
import third_party;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

NewCatalog::~NewCatalog() = default;

Status NewCatalog::Init(KVStore *kv_store) {
    auto kv_instance = kv_store->GetInstance();
    std::string db_string_id;
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

Status NewCatalog::AddBlockLock(std::string block_key) {
    bool insert_success = false;
    std::unordered_map<std::string, std::shared_ptr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), std::make_shared<BlockLock>());
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(std::string block_key, TxnTimeStamp checkpoint_ts) {
    bool insert_success = false;
    std::unordered_map<std::string, std::shared_ptr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), std::make_shared<BlockLock>(checkpoint_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetBlockLock(const std::string &block_key, std::shared_ptr<BlockLock> &block_lock) {
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

Status NewCatalog::DropBlockLockByBlockKey(const std::string &block_key) {
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

std::shared_ptr<MemIndex> NewCatalog::GetMemIndex(const std::string &mem_index_key, bool for_update) {
    std::shared_ptr<MemIndex> mem_index = nullptr;
    std::unique_lock<std::shared_mutex> lck(mem_index_mtx_);
    if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
        mem_index = iter->second;
    } else {
        mem_index = std::make_shared<MemIndex>();
        mem_index_map_.emplace(mem_index_key, mem_index);
    }
    if (for_update) {
        mem_index->UpdateBegin();
    }
    return mem_index;
}

std::shared_ptr<MemIndex> NewCatalog::PopMemIndex(const std::string &mem_index_key) {
    std::unique_lock<std::shared_mutex> lck(mem_index_mtx_);
    if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
        std::shared_ptr<MemIndex> mem_index = iter->second;
        mem_index_map_.erase(iter);
        return mem_index;
    }
    return nullptr;
}

Status NewCatalog::DropMemIndexByMemIndexKey(const std::string &mem_index_key) {
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

std::vector<std::pair<std::string, std::string>> NewCatalog::GetAllMemIndexInfo() {
    std::vector<std::pair<std::string, std::string>> result;
    {
        std::unique_lock lock(mem_index_mtx_);
        for (const auto &mem_index_pair : mem_index_map_) {
            if (mem_index_pair.second->GetHnswIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "hnsw"});
                continue;
            }
            if (mem_index_pair.second->GetIVFIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "ivf"});
                continue;
            }
            if (mem_index_pair.second->GetFulltextIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "full-text"});
                continue;
            }
            if (mem_index_pair.second->GetSecondaryIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "secondary"});
                continue;
            }
            if (mem_index_pair.second->GetEMVBIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "emvb"});
                continue;
            }
            if (mem_index_pair.second->GetBMPIndex() != nullptr) {
                result.push_back({mem_index_pair.first, "bmp"});
                continue;
            }
            result.push_back({mem_index_pair.first, "empty"});
        }
    }

    return result;
}

Status NewCatalog::AddFtIndexCache(std::string ft_index_cache_key, std::shared_ptr<TableIndexReaderCache> ft_index_cache) {
    bool insert_success = false;
    std::unordered_map<std::string, std::shared_ptr<TableIndexReaderCache>>::iterator iter;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        std::tie(iter, insert_success) = ft_index_cache_map_.emplace(std::move(ft_index_cache_key), std::move(ft_index_cache));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetFtIndexCache(const std::string &ft_index_cache_key, std::shared_ptr<TableIndexReaderCache> &ft_index_cache) {
    std::unique_lock<std::shared_mutex> lck(ft_index_cache_mtx_);
    if (auto iter = ft_index_cache_map_.find(ft_index_cache_key); iter != ft_index_cache_map_.end()) {
        ft_index_cache = iter->second;
    } else {
        ft_index_cache_map_.emplace(ft_index_cache_key, ft_index_cache);
    }
    return Status::OK();
}

Status NewCatalog::DropFtIndexCacheByFtIndexCacheKey(const std::string &ft_index_cache_key) {
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

Status NewCatalog::GetCleanedMeta(TxnTimeStamp ts,
                                  KVInstance *kv_instance,
                                  std::vector<std::shared_ptr<MetaKey>> &metas,
                                  std::vector<std::string> &drop_keys) {
    auto GetCleanedMetaImpl = [&](const std::vector<std::string> &keys) {
        const std::string &type_str = keys[1];
        const std::string &meta_str = keys[2];
        auto meta_infos = infinity::Partition(meta_str, '/');
        if (type_str == "db") {
            metas.emplace_back(std::make_shared<DBMetaKey>(std::move(meta_infos[2]), std::move(meta_infos[0]), std::stoull(meta_infos[1])));
        } else if (type_str == "tbl") {
            std::shared_ptr<TableMetaKey> table_meta_key =
                std::make_shared<TableMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[3]), std::move(meta_infos[1]));
            table_meta_key->commit_ts_ = std::stoull(meta_infos[2]);
            metas.emplace_back(std::move(table_meta_key));
        } else if (type_str == "tbl_name") {
            std::shared_ptr<TableNameMetaKey> table_name_meta_key =
                std::make_shared<TableNameMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[3]), std::move(meta_infos[1]));
            table_name_meta_key->commit_ts_ = std::stoull(meta_infos[2]);
            metas.emplace_back(std::move(table_name_meta_key));
        } else if (type_str == "seg") {
            metas.emplace_back(std::make_shared<SegmentMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1]), std::stoull(meta_infos[2])));
        } else if (type_str == "blk") {
            metas.emplace_back(std::make_shared<BlockMetaKey>(std::move(meta_infos[0]),
                                                              std::move(meta_infos[1]),
                                                              std::stoull(meta_infos[2]),
                                                              std::stoull(meta_infos[3])));
        } else if (type_str == "tbl_col") {
            std::shared_ptr<TableColumnMetaKey> table_column_meta_key =
                std::make_shared<TableColumnMetaKey>(std::move(meta_infos[0]), std::move(meta_infos[1]), std::move(meta_infos[2]));
            table_column_meta_key->commit_ts_ = std::stoull(meta_infos[3]);
            metas.emplace_back(std::move(table_column_meta_key));
        } else if (type_str == "blk_col") {
            metas.emplace_back(std::make_shared<ColumnMetaKey>(std::move(meta_infos[0]),
                                                               std::move(meta_infos[1]),
                                                               std::stoull(meta_infos[2]),
                                                               std::stoull(meta_infos[3]),
                                                               ColumnDef::FromJson(meta_infos[4])));
        } else if (type_str == "idx") {
            std::shared_ptr<TableIndexMetaKey> table_index_meta_key = std::make_shared<TableIndexMetaKey>(std::move(meta_infos[0]),
                                                                                                          std::move(meta_infos[1]),
                                                                                                          std::move(meta_infos[4]),
                                                                                                          std::move(meta_infos[2]));
            table_index_meta_key->commit_ts_ = std::stoull(meta_infos[3]);
            metas.emplace_back(std::move(table_index_meta_key));
        } else if (type_str == "idx_seg") {
            metas.emplace_back(std::make_shared<SegmentIndexMetaKey>(std::move(meta_infos[0]),
                                                                     std::move(meta_infos[1]),
                                                                     std::move(meta_infos[2]),
                                                                     std::stoull(meta_infos[3])));
        } else if (type_str == "idx_chunk") {
            metas.emplace_back(std::make_shared<ChunkIndexMetaKey>(std::move(meta_infos[0]),
                                                                   std::move(meta_infos[1]),
                                                                   std::move(meta_infos[2]),
                                                                   std::stoull(meta_infos[3]),
                                                                   std::stoull(meta_infos[4])));
        } else {
            UnrecoverableError("Unknown meta key type.");
        }
    };

    static constexpr std::string drop_prefix = "drop";
    auto iter = kv_instance->GetIterator();
    iter->Seek(drop_prefix);
    TxnTimeStamp drop_ts;

    while (iter->Valid() && iter->Key().starts_with(drop_prefix)) {
        std::string drop_key = iter->Key().ToString();
        std::string commit_ts_str = iter->Value().ToString();

        auto keys = infinity::Partition(drop_key, '|');
        const std::string &type_str = keys[1];
        const std::string &meta_str = keys[2];
        auto meta_infos = infinity::Partition(meta_str, '/');
        if (type_str == "db") {
            drop_ts = std::stoull(meta_infos[1]);
        } else if (type_str == "tbl") {
            drop_ts = std::stoull(meta_infos[2]);
        } else if (type_str == "tbl_name") {
            drop_ts = std::stoull(meta_infos[2]);
        } else if (type_str == "tbl_col") {
            drop_ts = std::stoull(meta_infos[3]);
        } else if (type_str == "idx") {
            drop_ts = std::stoull(meta_infos[3]);
        } else {
            drop_ts = std::stoull(commit_ts_str); // It might not be an integer
        }
        if (drop_ts <= ts) {
            drop_keys.emplace_back(drop_key);
        }
        iter->Next();
    }

    for (const auto &drop_key : drop_keys) {
        auto keys = infinity::Partition(drop_key, '|');
        GetCleanedMetaImpl(keys);
    }
    // Delete entities at lower hierarchy level first to avoid missing them when removing higher-level entities.
    std::sort(metas.begin(), metas.end(), [&](const std::shared_ptr<MetaKey> &lhs, const std::shared_ptr<MetaKey> &rhs) {
        return static_cast<size_t>(lhs->type_) > static_cast<size_t>(rhs->type_);
    });
    return Status::OK();
}

void NewCatalog::SetLastCleanupTS(TxnTimeStamp cleanup_ts) { last_cleanup_ts_ = cleanup_ts; }

TxnTimeStamp NewCatalog::GetLastCleanupTS() const { return last_cleanup_ts_; }

std::shared_ptr<MetaTree> NewCatalog::MakeMetaTree() const {
    auto entries = this->MakeMetaKeys();
    auto meta_tree_ptr = MetaTree::MakeMetaTree(entries);
    return meta_tree_ptr;
}

std::vector<std::shared_ptr<MetaKey>> NewCatalog::MakeMetaKeys() const {
    auto kv_instance_ptr = kv_store_->GetInstance();
    auto all_key_values = kv_instance_ptr->GetAllKeyValue();
    auto meta_count = all_key_values.size();

    std::vector<std::shared_ptr<MetaKey>> meta_keys;
    meta_keys.reserve(meta_count);

    std::vector<std::string> dropped_keys;
    std::vector<std::shared_ptr<MetaKey>> metas;
    if (auto status = GetCleanedMeta(MAX_TIMESTAMP, kv_instance_ptr.get(), metas, dropped_keys); !status.ok()) {
        LOG_ERROR((fmt::format("GetCleanedMeta failed: {}", status.message())));
        return meta_keys;
    }

    // Get encode keys for dropped metas.
    std::vector<std::string> keys_encode = GetEncodeKeys(metas);

    for (size_t idx = 0; idx < meta_count; ++idx) {
        const auto &pair = all_key_values[idx];
        // Skip dropped metas.
        if (!keys_encode.empty() && std::find(keys_encode.begin(), keys_encode.end(), pair.first) != keys_encode.end()) {
            continue;
        }

        std::shared_ptr<MetaKey> meta_key = MetaParse(pair.first, pair.second);
        if (meta_key == nullptr) {
            LOG_ERROR(fmt::format("Can't parse {}: {}: {}", idx, pair.first, pair.second));
        } else {
            LOG_INFO(fmt::format("META[{}] KEY: {}", idx, meta_key->ToString()));
        }
        meta_keys.emplace_back(meta_key);
    }
    return meta_keys;
}

std::unique_ptr<SystemCache> NewCatalog::RestoreCatalogCache(Storage *storage_ptr) {
    LOG_INFO("Restore catalog cache");

    auto meta_tree = this->MakeMetaTree();
    // std::vector<> = meta_tree->Check();
    // std::string meta_tree_str = meta_tree->ToJson().dump(4);
    // LOG_INFO(meta_tree_str);
    std::unique_ptr<SystemCache> system_cache = meta_tree->RestoreSystemCache(storage_ptr);
    // std::vector<MetaTableObject *> table_ptrs = meta_tree->ListTables();
    // for (const auto &table_ptr : table_ptrs) {
    //     SegmentID unsealed_segment_id = table_ptr->GetUnsealedSegmentID();
    //     SegmentID next_segment_id = table_ptr->GetNextSegmentID();
    //     size_t current_segment_row_count = table_ptr->GetCurrentSegmentRowCount(storage_ptr);
    //     LOG_INFO(fmt::format("Table: {}, unsealed_segment_id: {}, next_segment_id: {}, current_segment_row_count: {}",
    //                          table_ptr->GetTableName(),
    //                          unsealed_segment_id,
    //                          next_segment_id,
    //                          current_segment_row_count));
    // }

    return system_cache;
}

KVStore *NewCatalog::kv_store() const { return kv_store_; }

std::vector<std::string> NewCatalog::GetEncodeKeys(std::vector<std::shared_ptr<MetaKey>> &metas) const {
    std::vector<std::string> keys_encode;
    keys_encode.reserve(metas.size());
    for (auto &meta : metas) {
        switch (meta->type_) {
            case MetaType::kDB: {
                auto *db_meta_key = static_cast<DBMetaKey *>(meta.get());
                keys_encode.emplace_back(KeyEncode::CatalogDbKey(db_meta_key->db_name_, db_meta_key->commit_ts_));
                break;
            }
            case MetaType::kTable:
            case MetaType::kTableName: {
                auto *table_meta_key = static_cast<TableMetaKey *>(meta.get());
                keys_encode.emplace_back(
                    KeyEncode::CatalogTableKey(table_meta_key->db_id_str_, table_meta_key->table_name_, table_meta_key->commit_ts_));
                break;
            }
            case MetaType::kTableColumn: {
                auto *table_column_meta_key = static_cast<TableColumnMetaKey *>(meta.get());
                keys_encode.emplace_back(KeyEncode::TableColumnKey(table_column_meta_key->db_id_str_,
                                                                   table_column_meta_key->table_id_str_,
                                                                   table_column_meta_key->column_name_,
                                                                   table_column_meta_key->commit_ts_));
                break;
            }
            case MetaType::kSegment: {
                auto *segment_meta_key = static_cast<SegmentMetaKey *>(meta.get());
                keys_encode.emplace_back(
                    KeyEncode::CatalogTableSegmentKey(segment_meta_key->db_id_str_, segment_meta_key->table_id_str_, segment_meta_key->segment_id_));
                break;
            }
            case MetaType::kTableIndex: {
                auto *table_index_meta_key = static_cast<TableIndexMetaKey *>(meta.get());
                keys_encode.emplace_back(KeyEncode::CatalogIndexKey(table_index_meta_key->db_id_str_,
                                                                    table_index_meta_key->table_id_str_,
                                                                    table_index_meta_key->index_name_,
                                                                    table_index_meta_key->commit_ts_));
                break;
            }
            default: {
                break;
            }
        }
    }
    return keys_encode;
}

} // namespace infinity

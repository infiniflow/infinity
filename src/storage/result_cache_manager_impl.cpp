// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:result_cache_manager.impl;

import :result_cache_manager;
import :cached_scan_base;
import :infinity_exception;
import :logical_match;
import :physical_match;
import :logical_node_type;
import :logger;

import std;

namespace infinity {

std::unique_ptr<CacheContent> CacheContent::AppendColumns(const CacheContent &other, const std::vector<size_t> &column_idxes) const {
    if (data_blocks_.size() != other.data_blocks_.size()) {
        return nullptr;
    }
    std::unique_ptr<CacheContent> result = Clone();
    for (size_t i = 0; i < result->data_blocks_.size(); ++i) {
        DataBlock &old_block = *result->data_blocks_[i];
        const DataBlock &new_block = *other.data_blocks_[i];
        bool success = old_block.AppendColumns(new_block, column_idxes);
        if (!success) {
            return nullptr;
        }
    }
    for (size_t idx : column_idxes) {
        result->column_names_->push_back((*(other.column_names_))[idx]);
    }
    return result;
}

std::unique_ptr<CacheContent> CacheContent::Clone() const {
    std::vector<std::unique_ptr<DataBlock>> data_blocks;
    for (const auto &block : data_blocks_) {
        data_blocks.push_back(block->Clone());
    }
    return std::make_unique<CacheContent>(std::move(data_blocks), column_names_);
}

bool CacheResultMap::AddCache(
    std::unique_ptr<CachedNodeBase> cached_node,
    std::vector<std::unique_ptr<DataBlock>> data_blocks,
    const std::function<void(std::unique_ptr<CachedNodeBase>, CacheContent &, std::vector<std::unique_ptr<DataBlock>>)> &update_content_func) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto mp_iter = lru_map_.find(cached_node.get());
    if (mp_iter != lru_map_.end()) {
        CacheContent &old_content = *mp_iter->second->cache_content_;
        update_content_func(std::move(cached_node), old_content, std::move(data_blocks));
        return false;
    }
    if (lru_list_.size() >= cache_num_capacity_) {
        LRUEntry &envict_entry = lru_list_.back();
        size_t remove_n = lru_map_.erase(envict_entry.cached_node_.get());
        if (remove_n != 1) {
            UnrecoverableError("Failed to remove cache entry from lru_map_");
        }
        lru_list_.pop_back();
    }
    auto *cached_node_ptr = cached_node.get();
    auto cache_content = std::make_shared<CacheContent>(std::move(data_blocks), cached_node->output_names());
    lru_list_.emplace_front(std::move(cached_node), cache_content);
    lru_map_.emplace_hint(mp_iter, cached_node_ptr, lru_list_.begin());
    return true;
}

std::shared_ptr<CacheContent> CacheResultMap::GetCache(const CachedNodeBase &cached_node) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto mp_iter = lru_map_.find(&cached_node);
    if (mp_iter == lru_map_.end()) {
        return nullptr;
    }
    auto iter = mp_iter->second;
    lru_list_.splice(lru_list_.begin(), lru_list_, iter);
    return iter->cache_content_;
}

size_t CacheResultMap::DropIF(std::function<bool(const CachedNodeBase &)> pred) {
    std::lock_guard<std::mutex> lock(mtx_);
    size_t removed = 0;
    for (auto mp_iter = lru_map_.begin(); mp_iter != lru_map_.end();) {
        const auto &[cached_node, iter] = *mp_iter;
        if (pred(*cached_node)) {
            lru_list_.erase(iter);
            mp_iter = lru_map_.erase(mp_iter);
            ++removed;
        } else {
            ++mp_iter;
        }
    }
    return removed;
}

void CacheResultMap::ResetCacheNumCapacity(size_t cache_num_capacity) {
    std::lock_guard<std::mutex> lock(mtx_);
    cache_num_capacity_ = cache_num_capacity;
    while (lru_list_.size() > cache_num_capacity_) {
        LRUEntry &envict_entry = lru_list_.back();
        size_t remove_n = lru_map_.erase(envict_entry.cached_node_.get());
        if (remove_n != 1) {
            UnrecoverableError("Failed to remove cache entry from lru_map_");
        }
        lru_list_.pop_back();
    }
}

void CacheResultMap::ClearCache() {
    std::lock_guard<std::mutex> lock(mtx_);
    lru_list_.clear();
    lru_map_.clear();
}

bool ResultCacheManager::AddCache(std::unique_ptr<CachedNodeBase> cached_node, std::vector<std::unique_ptr<DataBlock>> data_blocks) {
    if (cached_node == nullptr) {
        return false;
    }
    auto update_content_func =
        [](std::unique_ptr<CachedNodeBase> cached_node, CacheContent &old_content, std::vector<std::unique_ptr<DataBlock>> data_blocks) {
            auto new_content = std::make_shared<CacheContent>(std::move(data_blocks), cached_node->output_names());
            std::shared_ptr<std::vector<std::string>> new_output_names = new_content->column_names_;
            std::shared_ptr<std::vector<std::string>> old_output_names = old_content.column_names_;

            std::vector<size_t> add_columns;
            for (size_t i = 0; i < new_output_names->size(); ++i) {
                const std::string &output_name = (*new_output_names)[i];
                auto iter = std::find(old_output_names->begin(), old_output_names->end(), output_name);
                if (iter == old_output_names->end()) {
                    add_columns.push_back(i);
                }
            }
            if (add_columns.empty()) {
                return;
            }
            std::unique_ptr<CacheContent> updated_content = old_content.AppendColumns(*new_content, add_columns);
            if (!updated_content) {
                LOG_WARN("Failed to append columns to cache content");
                old_content = std::move(*new_content);
            } else {
                LOG_INFO("Success update cache content");
                old_content = std::move(*updated_content);
            }
        };
    return cache_map_.AddCache(std::move(cached_node), std::move(data_blocks), update_content_func);
}

std::optional<CacheOutput> ResultCacheManager::GetCache(const CachedNodeBase &cached_node) {
    std::shared_ptr<CacheContent> cache_content = cache_map_.GetCache(cached_node);
    if (!cache_content) {
        return std::nullopt;
    }
    std::shared_ptr<std::vector<std::string>> output_names = cached_node.output_names();

    std::vector<size_t> column_map;
    for (const std::string &output_name : *output_names) {
        auto column_iter = std::find(cache_content->column_names_->begin(), cache_content->column_names_->end(), output_name);
        if (column_iter == cache_content->column_names_->end()) {
            return std::nullopt;
        }
        column_map.push_back(column_iter - cache_content->column_names_->begin());
    }
    return CacheOutput{std::move(cache_content), std::move(column_map)};
}

size_t ResultCacheManager::DropTable(const std::string &schema_name, const std::string &table_name) {
    auto pred = [&](const CachedNodeBase &cached_node_base) {
        switch (cached_node_base.type()) {
            case LogicalNodeType::kMatch: {
                const auto &cached_scan_base = static_cast<const CachedScanBase &>(cached_node_base);
                return cached_scan_base.schema_name() == schema_name && cached_scan_base.table_name() == table_name;
            }
            default: {
                return false;
            }
        }
    };
    return cache_map_.DropIF(pred);
}

} // namespace infinity
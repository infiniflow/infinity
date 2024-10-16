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

#include <vector>

module result_cache_manager;

import cached_match_base;
import infinity_exception;
import logical_match;
import physical_match;
import logical_node_type;

namespace infinity {

bool CacheContent::AppendColumns(const CacheContent &other, const Vector<SizeT> &column_idxes) {
    if (data_blocks_.size() != other.data_blocks_.size()) {
        return false;
    }
    for (SizeT i = 0; i < data_blocks_.size(); ++i) {
        DataBlock &old_block = *data_blocks_[i];
        const DataBlock &new_block = *other.data_blocks_[i];
        bool success = old_block.AppendColumns(new_block, column_idxes);
        if (!success) {
            return false;
        }
    }
    for (SizeT idx : column_idxes) {
        column_names_->push_back((*(other.column_names_))[idx]);
    }
    return true;
}

bool ResultCacheManager::AddCache(UniquePtr<CachedNodeBase> cached_node, Vector<UniquePtr<DataBlock>> data_blocks) {
    if (cached_node == nullptr) {
        return false;
    }
    auto cache_content = MakeShared<CacheContent>(std::move(data_blocks), cached_node->output_names());

    std::lock_guard<std::mutex> lock(mtx_);
    auto iter = cache_map_.find(static_cast<const CachedNodeBase *>(cached_node.get()));
    if (iter == cache_map_.end()) {
        cache_map_.emplace(std::move(cached_node), std::move(cache_content));
        return true;
    }

    SharedPtr<Vector<String>> new_output_names = cached_node->output_names();
    SharedPtr<Vector<String>> old_output_names = iter->second->column_names_;

    Vector<SizeT> add_columns;
    for (SizeT i = 0; i < new_output_names->size(); ++i) {
        const String &output_name = (*new_output_names)[i];
        auto iter = std::find(old_output_names->begin(), old_output_names->end(), output_name);
        if (iter == old_output_names->end()) {
            add_columns.push_back(i);
        }
    }
    if (add_columns.empty()) {
        return true;
    }
    bool success = iter->second->AppendColumns(*cache_content, add_columns);
    if (!success) {
        iter->second = std::move(cache_content);
        return false;
    }
    return true;
}

Optional<CacheOutput> ResultCacheManager::GetCache(const CachedNodeBase &cached_node) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto iter = cache_map_.find(&cached_node);
    if (iter == cache_map_.end()) {
        return None;
    }
    SharedPtr<CacheContent> cache_content = iter->second;
    SharedPtr<Vector<String>> output_names = cached_node.output_names();
    Vector<SizeT> column_map;
    for (const String &output_name : *output_names) {
        auto column_iter = std::find(cache_content->column_names_->begin(), cache_content->column_names_->end(), output_name);
        if (column_iter == cache_content->column_names_->end()) {
            return None;
        }
        column_map.push_back(column_iter - cache_content->column_names_->begin());
    }
    return CacheOutput{std::move(cache_content), std::move(column_map)};
}

SizeT ResultCacheManager::DropTable(const String &schema_name, const String &table_name) {
    std::lock_guard<std::mutex> lock(mtx_);

    SizeT removed = 0;
    for (auto iter = cache_map_.begin(); iter != cache_map_.end();) {
        const CachedNodeBase &cached_node_base = *iter->first;
        switch (cached_node_base.type()) {
            case LogicalNodeType::kMatch: {
                const auto &cached_match_base = static_cast<const CachedMatchBase &>(cached_node_base);
                if (cached_match_base.schema_name() == schema_name && cached_match_base.table_name() == table_name) {
                    iter = cache_map_.erase(iter);
                    ++removed;
                } else {
                    ++iter;
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    return removed;
}

} // namespace infinity
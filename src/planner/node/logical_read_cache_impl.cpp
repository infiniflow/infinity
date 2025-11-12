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

module infinity_core:logical_read_cache.impl;

import :logical_read_cache;
import :default_values;
import :result_cache_manager;
import :infinity_exception;

import std;

import logical_type;
import data_type;

namespace infinity {

LogicalReadCache::LogicalReadCache(u64 node_id,
                                   LogicalNodeType origin_type,
                                   std::shared_ptr<BaseTableRef> base_table_ref,
                                   std::shared_ptr<CacheContent> cache_content,
                                   std::vector<size_t> column_map,
                                   bool is_min_heap)
    : LogicalNode(node_id, LogicalNodeType::kReadCache), origin_type_(origin_type), base_table_ref_(std::move(base_table_ref)),
      cache_content_(std::move(cache_content)), column_map_(std::move(column_map)), is_min_heap_(is_min_heap) {}

std::vector<ColumnBinding> LogicalReadCache::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (size_t col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalReadCache::GetOutputNames() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("CacheContent data blocks is empty");
    }
    auto result_strings = std::make_shared<std::vector<std::string>>();
    for (size_t col_id : column_map_) {
        std::string column_name = cache_content_->column_names_->at(col_id);
        result_strings->push_back(std::move(column_name));
    }
    return result_strings;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalReadCache::GetOutputTypes() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("CacheContent data blocks is empty");
    }
    auto result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    for (size_t column_id : column_map_) {
        std::shared_ptr<DataType> data_type = cache_content_->data_blocks_[0]->column_vectors_[column_id]->data_type();
        result_types->push_back(data_type);
    }
    return result_types;
}

std::string LogicalReadCache::ToString(i64 &space) const { return ""; }

} // namespace infinity
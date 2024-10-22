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

module logical_read_cache;

import default_values;
import logical_type;
import result_cache_manager;
import infinity_exception;

namespace infinity {

LogicalReadCache::LogicalReadCache(u64 node_id,
                                   LogicalNodeType origin_type,
                                   SharedPtr<BaseTableRef> base_table_ref,
                                   SharedPtr<CacheContent> cache_content,
                                   Vector<SizeT> column_map,
                                   bool is_min_heap)
    : LogicalNode(node_id, LogicalNodeType::kReadCache), origin_type_(origin_type), base_table_ref_(std::move(base_table_ref)),
      cache_content_(std::move(cache_content)), column_map_(std::move(column_map)), is_min_heap_(is_min_heap) {}

Vector<ColumnBinding> LogicalReadCache::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (SizeT col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalReadCache::GetOutputNames() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("CacheContent data blocks is empty");
    }
    auto result_strings = MakeShared<Vector<String>>();
    for (SizeT col_id : column_map_) {
        String column_name = cache_content_->column_names_->at(col_id);
        result_strings->push_back(std::move(column_name));
    }
    return result_strings;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalReadCache::GetOutputTypes() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("CacheContent data blocks is empty");
    }
    auto result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    for (SizeT column_id : column_map_) {
        SharedPtr<DataType> data_type = cache_content_->data_blocks_[0]->column_vectors[column_id]->data_type();
        result_types->push_back(data_type);
    }
    return result_types;
}

String LogicalReadCache::ToString(i64 &space) const { return ""; }

} // namespace infinity
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

export module infinity_core:logical_read_cache;

import :logical_node;
import :data_block;
import :column_binding;
import :base_table_ref;
import :logical_node_type;

import data_type;

namespace infinity {

export struct CacheContent;

export class LogicalReadCache : public LogicalNode {
public:
    LogicalReadCache(u64 node_id,
                     LogicalNodeType origin_type,
                     std::shared_ptr<BaseTableRef> base_table_ref,
                     std::shared_ptr<CacheContent> cache_content,
                     std::vector<size_t> column_map,
                     bool is_min_heap);

public:
    virtual std::vector<ColumnBinding> GetColumnBindings() const;

    virtual std::shared_ptr<std::vector<std::string>> GetOutputNames() const;

    virtual std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const;

    virtual std::string ToString(i64 &space) const;

    virtual std::string name() { return "LogicalReadCache"; }

public:
    LogicalNodeType origin_type_;
    std::shared_ptr<BaseTableRef> base_table_ref_;
    std::shared_ptr<CacheContent> cache_content_;
    std::vector<size_t> column_map_; // result column id -> cache column id
    bool is_min_heap_;
};

} // namespace infinity
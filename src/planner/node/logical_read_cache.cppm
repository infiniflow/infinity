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

export module logical_read_cache;

import stl;
import logical_node;
import data_block;
import column_binding;
import data_type;
import base_table_ref;
import logical_node_type;

namespace infinity {

struct CacheContent;

export class LogicalReadCache : public LogicalNode {
public:
    LogicalReadCache(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<CacheContent> cache_content, Vector<SizeT> column_map);

public:
    virtual Vector<ColumnBinding> GetColumnBindings() const;

    virtual SharedPtr<Vector<String>> GetOutputNames() const;

    virtual SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const;

    virtual String ToString(i64 &space) const;

    virtual String name() { return "LogicalReadCache"; }

public:
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<CacheContent> cache_content_;
    Vector<SizeT> column_map_; // result column id -> cache column id
};

} // namespace infinity
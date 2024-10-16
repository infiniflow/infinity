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

#include <type_traits>

export module result_cache_manager;

import stl;
import cached_node_base;
import data_block;
import logical_read_cache;

namespace infinity {

class LogicalNode;
class PhysicalOperator;

export struct CacheContent {
public:
    CacheContent(Vector<UniquePtr<DataBlock>> data_blocks, SharedPtr<Vector<String>> column_names)
        : data_blocks_(std::move(data_blocks)), column_names_(std::move(column_names)) {}

    bool AppendColumns(const CacheContent &other, const Vector<SizeT> &column_idxes);

    Vector<UniquePtr<DataBlock>> data_blocks_;
    SharedPtr<Vector<String>> column_names_;
};

export struct CacheOutput {
    SharedPtr<CacheContent> cache_content_;
    Vector<SizeT> column_map_;
};

export class ResultCacheManager {
public:
    struct CachedLogicalMatchBaseHash {
        using is_transparent = std::true_type;

        SizeT operator()(const UniquePtr<CachedNodeBase> &key) const { return key->Hash(); }
        SizeT operator()(const CachedNodeBase *key) const { return key->Hash(); }
    };

    struct CachedLogicalMatchBaseEq {
        using is_transparent = std::true_type;

        bool operator()(const UniquePtr<CachedNodeBase> &key1, const UniquePtr<CachedNodeBase> &key2) const { return key1->Eq(*key2); }
        bool operator()(const UniquePtr<CachedNodeBase> &key1, const CachedNodeBase *key2) const { return key1->Eq(*key2); }
    };

public:
    ResultCacheManager() = default;

    bool AddCache(UniquePtr<CachedNodeBase> cached_node, Vector<UniquePtr<DataBlock>> data_blocks);

    Optional<CacheOutput> GetCache(const CachedNodeBase &cached_node);

    SizeT DropTable(const String &schema_name, const String &table_name);

private:
    HashMap<UniquePtr<CachedNodeBase>, SharedPtr<CacheContent>, CachedLogicalMatchBaseHash, CachedLogicalMatchBaseEq> cache_map_;
    std::mutex mtx_;
};

} // namespace infinity
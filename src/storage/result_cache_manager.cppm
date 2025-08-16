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

export module infinity_core:result_cache_manager;

import :cached_node_base;
import :data_block;
import :logical_read_cache;

import std;

import global_resource_usage;

namespace infinity {

class LogicalNode;
class PhysicalOperator;

export struct CacheContent {
public:
    CacheContent(std::vector<std::unique_ptr<DataBlock>> data_blocks, std::shared_ptr<std::vector<std::string>> column_names)
        : data_blocks_(std::move(data_blocks)), column_names_(std::move(column_names)) {}

    std::unique_ptr<CacheContent> AppendColumns(const CacheContent &other, const std::vector<size_t> &column_idxes) const;

    std::unique_ptr<CacheContent> Clone() const;

    std::vector<std::unique_ptr<DataBlock>> data_blocks_;
    std::shared_ptr<std::vector<std::string>> column_names_;
};

export struct CacheOutput {
    std::shared_ptr<CacheContent> cache_content_;
    std::vector<size_t> column_map_;
};

class CacheResultMap {
public:
    struct CachedLogicalMatchBaseHash {
        using is_transparent = std::true_type;

        size_t operator()(const CachedNodeBase *key) const { return key->Hash(); }
    };

    struct CachedLogicalMatchBaseEq {
        using is_transparent = std::true_type;

        bool operator()(const CachedNodeBase *key1, const CachedNodeBase *key2) const { return key1->Eq(*key2); }
    };

    CacheResultMap(size_t cache_num_capacity) : cache_num_capacity_(cache_num_capacity) {}

    bool AddCache(std::unique_ptr<CachedNodeBase> cached_node,
                  std::vector<std::unique_ptr<DataBlock>> data_blocks,
                  const std::function<void(std::unique_ptr<CachedNodeBase>, CacheContent &, std::vector<std::unique_ptr<DataBlock>>)> &update_content_func);

    std::shared_ptr<CacheContent> GetCache(const CachedNodeBase &cached_node);

    size_t DropIF(std::function<bool(const CachedNodeBase &)> pred);

    void ResetCacheNumCapacity(size_t cache_num_capacity);

    void ClearCache();

    size_t cache_num_capacity() const { return cache_num_capacity_; }

    size_t cache_num_used() {
        std::lock_guard<std::mutex> lock(mtx_);
        return lru_map_.size();
    }

private:
    struct LRUEntry {
        std::unique_ptr<CachedNodeBase> cached_node_;
        std::shared_ptr<CacheContent> cache_content_;
    };
    using LRUList = std::list<LRUEntry>;
    using LRUMap = std::unordered_map<CachedNodeBase *, LRUList::iterator, CachedLogicalMatchBaseHash, CachedLogicalMatchBaseEq>;

    std::mutex mtx_;

    size_t cache_num_capacity_;
    LRUList lru_list_;
    LRUMap lru_map_;
};

export class ResultCacheManager {
public:
    ResultCacheManager(size_t cache_num_capacity) : cache_map_(cache_num_capacity) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("ResultCacheManager");
#endif
    }
    ~ResultCacheManager() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("ResultCacheManager");
#endif
    }

    bool AddCache(std::unique_ptr<CachedNodeBase> cached_node, std::vector<std::unique_ptr<DataBlock>> data_blocks);

    std::optional<CacheOutput> GetCache(const CachedNodeBase &cached_node);

    size_t DropTable(const std::string &schema_name, const std::string &table_name);

    void ResetCacheNumCapacity(size_t cache_num_capacity) { cache_map_.ResetCacheNumCapacity(cache_num_capacity); }

    void ClearCache() { cache_map_.ClearCache(); }

    size_t cache_num_capacity() const { return cache_map_.cache_num_capacity(); }

    size_t cache_num_used() { return cache_map_.cache_num_used(); }

private:
    CacheResultMap cache_map_;
};

} // namespace infinity
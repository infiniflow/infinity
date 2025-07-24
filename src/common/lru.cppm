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

#include <cassert>

export module lru;

import stl;

namespace infinity {
template<typename K, typename V>
class lru_cache {
    struct kvp {
        K key;
        V value;
    };
    using list_type = std::list<kvp>;
    using list_iterator = typename list_type::iterator;
    mutable list_type lru;
    std::unordered_map<K, list_iterator> map_;
    std::optional<std::size_t> max_size_;

public:
    lru_cache() = default;
    lru_cache(std::optional<std::size_t> max_size) : max_size_(max_size) {
        assert(max_size_.has_value());
    }

    void set_max_size(std::optional<std::size_t> size) {
        assert(max_size_.has_value() && *size > 0);
        max_size_ = size;
        maybe_trim();
    }

    void maybe_touch(const K& key) {
        auto it = map_.find(key);
        if(it == map_.end()) {
            return;
        }
        auto list_it = it->second;
        touch(list_it);
    }

    std::optional<V&> maybe_get(const K& key) {
        auto it = map_.find(key);
        if(it == map_.end()) {
            return nullptr;
        } else {
            touch(it->second);
            return it->second->value;
        }
    }

    std::optional<const V&> maybe_get(const K& key) const {
        auto it = map_.find(key);
        if(it == map_.end()) {
            return nullptr;
        } else {
            touch(it->second);
            return it->second->value;
        }
    }

    void set(const K& key, V value) {
        auto it = map_.find(key);
        if(it == map_.end()) {
            insert(key, std::move(value));
        } else {
            touch(it->second);
            it->second->value = std::move(value);
        }
    }

    std::optional<V&> insert(const K& key, V value) {
        auto pair = map_.insert({key, lru.end()});
        if(!pair.second) {
            // didn't insert
            return nullptr;
        }
        auto map_it = pair.first;
        lru.push_front({key, std::move(value)});
        map_it->second = lru.begin();
        maybe_trim();
        return lru.front().value;
    }

    std::size_t size() const {
        return lru.size();
    }

private:
    void touch(list_iterator list_it) const {
        lru.splice(lru.begin(), lru, list_it);
    }

    void maybe_trim() {
        while(max_size_ && lru.size() > max_size_) {
            const auto& to_remove = lru.back();
            map_.erase(to_remove.key);
            lru.pop_back();
        }
    }
};

}
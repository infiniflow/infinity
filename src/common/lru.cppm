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

export module infinity_core:lru;

import :stl;

namespace infinity {

export template <typename K, typename V>
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

    mutable std::mutex lock_;

public:
    lru_cache() = default;
    lru_cache(std::optional<std::size_t> max_size) : max_size_(max_size) { assert(max_size_.has_value()); }

    void set_max_size(std::optional<std::size_t> size) {
        std::unique_lock<std::mutex> lk(lock_);
        assert(max_size_.has_value() && *size > 0);
        max_size_ = size;
        maybe_trim();
    }

    void maybe_touch(const K &key) {
        std::unique_lock<std::mutex> lk(lock_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return;
        }
        auto list_it = it->second;
        touch(list_it);
    }

    std::optional<V> maybe_get(const K &key) {
        std::unique_lock<std::mutex> lk(lock_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        } else {
            touch(it->second);
            return it->second->value;
        }
    }

    std::optional<V> maybe_get(const K &key) const {
        std::unique_lock<std::mutex> lk(lock_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        } else {
            touch(it->second);
            return it->second->value;
        }
    }

    void set(const K &key, V value) {
        std::unique_lock<std::mutex> lk(lock_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            insert(key, std::move(value));
        } else {
            touch(it->second);
            it->second->value = std::move(value);
        }
    }

    void unset(const K &key) {
        std::unique_lock<std::mutex> lk(lock_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            // Doesn't remove the kv from the list, just from the map
            map_.erase(it);
        }
    }

    std::size_t size() const {
        std::unique_lock<std::mutex> lk(lock_);
        return lru.size();
    }

    std::vector<std::string> get_all_keys() const {
        std::unique_lock<std::mutex> lk(lock_);
        std::vector<std::string> keys;
        for (const auto &item : lru) {
            if (map_.contains(item.key)) {
                keys.push_back(item.key);
            }
        }
        return keys;
    }

private:
    std::optional<V> insert(const K &key, V value) {
        auto pair = map_.insert({key, lru.end()});
        if (!pair.second) {
            // didn't insert
            return std::nullopt;
        }
        auto map_it = pair.first;
        lru.push_front({key, std::move(value)});
        map_it->second = lru.begin();
        maybe_trim();
        return lru.front().value;
    }

    void touch(list_iterator list_it) const { lru.splice(lru.begin(), lru, list_it); }

    void maybe_trim() {
        while (max_size_ && lru.size() > *max_size_) {
            const auto &to_remove = lru.back();
            map_.erase(to_remove.key);
            lru.pop_back();
        }
    }
};

} // namespace infinity
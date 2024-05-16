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

export module map_with_lock;
import stl;

namespace infinity {

export template <typename KeyType, typename ValueType>
class MapWithLock {
private:
    std::shared_mutex mutex_;
    Map<KeyType, ValueType> map_;

public:
    MapWithLock() = default;

    ~MapWithLock() = default;

    bool Get(const KeyType &key, ValueType &value) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        value = it->second;
        return true;
    }

    // Get or add a value to the map.
    // Returns true if found.
    // Returns false if not found, and add the key-value pair into the map.
    bool GetOrAdd(const KeyType &key, ValueType &value, const ValueType &new_value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto result = map_.emplace(key, new_value);
        if (result.second) {
            // added
            value = new_value;
            return false;
        } else {
            // found
            value = result.first->second;
            return true;
        }
    }

    void Clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_.clear();
    }

    void Range(const KeyType &key_min, const KeyType &key_max, Vector<Pair<KeyType, ValueType>> &items) {
        items.clear();
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it1 = map_.lower_bound(key_min);
        auto it2 = map_.upper_bound(key_max);
        while (it1 != it2) {
            items.push_back(MakePair(it1->first, it1->second));
            ++it1;
        }
    }

    // WARN: Caller shall ensure there's no concurrent write access
    Map<KeyType, ValueType>::iterator UnsafeBegin() { return map_.begin(); }

    // WARN: Caller shall ensure there's no concurrent write access
    Map<KeyType, ValueType>::iterator UnsafeEnd() { return map_.end(); }
};
} // namespace infinity

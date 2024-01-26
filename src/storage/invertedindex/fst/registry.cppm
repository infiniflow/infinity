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
export module fst:registry;
import stl;

namespace infinity {

/**
A registry that can be used to store the pairs of (Key, SizeT). The registry should be able to store a maximum number of pairs. If the maximum number
is reached, the least recently used pair should be removed from the registry.

The member function Find(Key) should be return the SizeT for a given Key, and moved the found pairT to the head of the lru list. If the Key is not in
the registry, the registry should return zero.

The member function Insert(Key, SizeT) should insert the pair (Key, SizeT) into the registry. If the Key is already in the registry, the SizeT should
be updated. If the registry is full, the least recently used pair should be removed from the registry.

*/

export template <typename Key>
class Registry {
private:
    using Entry = Pair<Key, SizeT>;
    using ListIterator = List<Entry>::iterator;
    HashMap<Key, ListIterator> map_;
    List<Entry> lru_;
    SizeT max_size_;

public:
    explicit Registry(SizeT max_size = 10000) : map_(max_size), max_size_(max_size) {}

    SizeT Find(const Key &key) {
        auto iter = map_.find(key);
        if (iter == map_.end()) {
            return 0;
        }

        Entry pair = *iter->second;
        lru_.erase(iter->second);
        lru_.push_front(pair);
        map_[key] = lru_.begin();

        return pair.second;
    }

    void Insert(const Key &key, SizeT value) {
        auto iter = map_.find(key);
        if (iter != map_.end()) {
            lru_.erase(iter->second);
        } else if (lru_.size() >= max_size_) {
            auto last = lru_.back();
            map_.erase(last.first);
            lru_.pop_back();
        }

        lru_.push_front(Entry(key, value));
        map_[key] = lru_.begin();
    }

    void Clear() {
        map_.clear();
        lru_.clear();
    }

    SizeT Size() { return map_.size(); }
};

} // namespace infinity
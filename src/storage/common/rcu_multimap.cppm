module;

#include <cassert>
#include <cstdio>

export module rcu_multimap;

import stl;

namespace infinity {

export template <typename Key, typename Value>
class RcuMultiMap {
private:
    struct MapValue {
        Value value_;
        volatile u64 used_time_;

        // Comparison operators needed for Set storage
        bool operator<(const MapValue &other) const {
            if (value_ != other.value_) {
                return value_ < other.value_;
            }
            return used_time_ < other.used_time_;
        }

        bool operator==(const MapValue &other) const { return value_ == other.value_ && used_time_ == other.used_time_; }

        bool operator!=(const MapValue &other) const { return !(*this == other); }
    };

    typedef MultiMap<Key, MapValue> InnerMultiMap;

    typedef void (*ValueOp)(const Value &value);

    struct DeletedMap {
        InnerMultiMap *map_;
        Set<Pair<Key, MapValue>> *deleted_entries_;
        u64 delete_time_;
    };

public:
    explicit RcuMultiMap();

    ~RcuMultiMap();

    Vector<Value> Get(const Key &key, bool update_access_time = true);

    // Add lowercase alias for compatibility
    Vector<Value> get(const Key &key, bool update_access_time = true) { return Get(key, update_access_time); }

    Vector<Value> GetWithRcuTime(const Key &key);

    void Insert(const Key &key, const Value &value);

    // Add lowercase alias for compatibility
    void insert(const Key &key, const Value &value) { Insert(key, value); }

    void Delete(const Key &key);

    // Add lowercase alias for compatibility
    void delete_key(const Key &key) { Delete(key); }

    void Delete(const Key &key, const Value &value);

    void CheckGc(u64 min_delete_time);

    // Add lowercase alias for compatibility
    void checkGc(u64 min_delete_time) { CheckGc(min_delete_time); }

    void CheckExpired(u64 min_access_time, Vector<Key> &keys_need_expired);

    void GetAllValuesWithRef(Vector<Value> &values);

    void GetAllKeyValuePairs(Vector<Pair<Key, Value>> &pairs) const;

    size_t size() const;

    template <typename... Args>
    void emplace(const Key &key, Args &&...args);

    Vector<Pair<Key, Value>> lower_bound(const Key &key);

    Vector<Pair<Key, Value>> upper_bound(const Key &key);

    u32 range(const Key &key_min, const Key &key_max, Vector<Value> &result) const;

private:
    void CheckSwapInLock();
    MapValue CreateMapValue(const Value &value);

    // Helper function to get current time in milliseconds
    u64 GetCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

private:
    InnerMultiMap *volatile read_map_;
    std::size_t miss_time_;

    mutable std::mutex dirty_lock_;
    InnerMultiMap *dirty_map_;

    Set<Pair<Key, MapValue>> deleted_entries_;

    List<MapValue> deleted_value_list_;
    List<DeletedMap> deleted_map_list_;
};

template <typename Key, typename Value>
RcuMultiMap<Key, Value>::RcuMultiMap() {
    read_map_ = new InnerMultiMap();
    miss_time_ = 0;
    dirty_map_ = new InnerMultiMap();
}

template <typename Key, typename Value>
RcuMultiMap<Key, Value>::~RcuMultiMap() {
    delete dirty_map_;
    delete read_map_;

    while (!deleted_value_list_.empty()) {
        deleted_value_list_.pop_front();
    }

    while (!deleted_map_list_.empty()) {
        DeletedMap &deleted_map = deleted_map_list_.front();
        delete deleted_map.deleted_entries_;
        delete deleted_map.map_;
        deleted_map_list_.pop_front();
    }
}

template <typename Key, typename Value>
typename RcuMultiMap<Key, Value>::MapValue RcuMultiMap<Key, Value>::CreateMapValue(const Value &value) {
    MapValue map_value;
    map_value.value_ = value;
    map_value.used_time_ = GetCurrentTimeMs();
    return map_value;
}

template <typename Key, typename Value>
Vector<Value> RcuMultiMap<Key, Value>::Get(const Key &key, bool update_access_time) {
    Vector<Value> results;

    InnerMultiMap *current_read = read_map_;
    auto range = current_read->equal_range(key);

    for (auto it = range.first; it != range.second; ++it) {
        if (update_access_time) {
            it->second.used_time_ = GetCurrentTimeMs();
        }
        results.push_back(it->second.value_);
    }

    if (results.empty()) {
        std::lock_guard<std::mutex> lock(dirty_lock_);
        auto dirty_range = dirty_map_->equal_range(key);

        for (auto it = dirty_range.first; it != dirty_range.second; ++it) {
            if (update_access_time) {
                it->second.used_time_ = GetCurrentTimeMs();
            }
            results.push_back(it->second.value_);
        }

        if (!results.empty() && read_map_ == current_read) {
            miss_time_ += results.size();
            CheckSwapInLock();
        }
    }

    return results;
}

template <typename Key, typename Value>
Vector<Value> RcuMultiMap<Key, Value>::GetWithRcuTime(const Key &key) {
    return Get(key, false);
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::CheckSwapInLock() {
    if (miss_time_ < dirty_map_->size()) {
        return;
    }

    InnerMultiMap *new_dirty_map = new InnerMultiMap(*dirty_map_);
    DeletedMap deleted_map;
    deleted_map.map_ = read_map_;
    read_map_ = dirty_map_;
    deleted_map.delete_time_ = GetCurrentTimeMs();
    deleted_map.deleted_entries_ = new Set<Pair<Key, MapValue>>();
    deleted_map.deleted_entries_->swap(deleted_entries_);
    dirty_map_ = new_dirty_map;
    deleted_map_list_.push_back(deleted_map);
    miss_time_ = 0;
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Insert(const Key &key, const Value &value) {
    MapValue new_value = CreateMapValue(value);

    std::lock_guard<std::mutex> lock(dirty_lock_);
    dirty_map_->insert(MakePair(key, new_value));
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Delete(const Key& key) {
    std::lock_guard<std::mutex> lock(dirty_lock_);

    auto range = dirty_map_->equal_range(key);
    for (auto it = range.first; it != range.second;) {
        MapValue map_value = it->second;

        it = dirty_map_->erase(it);

        map_value.used_time_ = GetCurrentTimeMs();
        deleted_value_list_.push_back(map_value);

        auto read_range = read_map_->equal_range(key);
        for (auto rit = read_range.first; rit != read_range.second; ++rit) {
            if (rit->second.value_ == map_value.value_) {
                deleted_entries_.insert(MakePair(key, map_value));
                break;
            }
        }
    }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Delete(const Key &key, const Value &value) {
    std::lock_guard<std::mutex> lock(dirty_lock_);

    auto range = dirty_map_->equal_range(key);
    for (auto it = range.first; it != range.second;) {
        if (it->second.value_ == value) {
            MapValue map_value = it->second;

            it = dirty_map_->erase(it);

            map_value.used_time_ = GetCurrentTimeMs();
            deleted_value_list_.push_back(map_value);

            auto read_range = read_map_->equal_range(key);
            for (auto rit = read_range.first; rit != read_range.second; ++rit) {
                if (rit->second.value_ == map_value.value_) {
                    deleted_entries_.insert(MakePair(key, map_value));
                    break;
                }
            }
        } else {
            ++it;
        }
    }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::CheckGc(u64 min_delete_time) {
    Vector<Value> values_need_delete;
    {
        std::lock_guard<std::mutex> lock(dirty_lock_);
        while (!deleted_value_list_.empty()) {
            MapValue &oldest_value = deleted_value_list_.front();
            if (oldest_value.used_time_ >= min_delete_time) {
                break;
            }
            values_need_delete.push_back(oldest_value.value_);
            deleted_value_list_.pop_front();
        }
    }

    Vector<DeletedMap> map_need_delete;
    {
        std::lock_guard<std::mutex> lock(dirty_lock_);
        while (!deleted_map_list_.empty()) {
            DeletedMap &oldest_value = deleted_map_list_.front();
            if (oldest_value.delete_time_ >= min_delete_time) {
                break;
            }
            map_need_delete.push_back(oldest_value);
            deleted_map_list_.pop_front();
        }
    }

    for (auto &deleted_map : map_need_delete) {
        delete deleted_map.deleted_entries_;
        delete deleted_map.map_;
    }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::CheckExpired(u64 min_access_time, Vector<Key> &keys_need_expired) {
    std::lock_guard<std::mutex> lock(dirty_lock_);
    Set<Key> expired_keys;

    for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
        if (it->second.used_time_ <= min_access_time) {
            expired_keys.insert(it->first);
        }
    }

    keys_need_expired.assign(expired_keys.begin(), expired_keys.end());
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::GetAllValuesWithRef(Vector<Value> &values) {
    std::lock_guard<std::mutex> lock(dirty_lock_);
    for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
        values.push_back(it->second.value_);
    }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::GetAllKeyValuePairs(Vector<Pair<Key, Value>> &pairs) const {
    std::lock_guard<std::mutex> lock(dirty_lock_);

    // Get pairs from dirty_map
    for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
        pairs.emplace_back(it->first, it->second.value_);
    }

    // Also get pairs from read_map if it exists and is different from dirty_map
    if (read_map_ != nullptr && read_map_ != dirty_map_) {
        for (auto it = read_map_->begin(); it != read_map_->end(); ++it) {
            pairs.emplace_back(it->first, it->second.value_);
        }
    }
}

template <typename Key, typename Value>
size_t RcuMultiMap<Key, Value>::size() const {
    std::lock_guard<std::mutex> lock(dirty_lock_);
    return dirty_map_->size();
}

template <typename Key, typename Value>
template <typename... Args>
void RcuMultiMap<Key, Value>::emplace(const Key& key, Args&&... args) {
    Value value(std::forward<Args>(args)...);
    Insert(key, value);
}

template <typename Key, typename Value>
Vector<Pair<Key, Value>> RcuMultiMap<Key, Value>::lower_bound(const Key &key) {
    Vector<Pair<Key, Value>> result;

    InnerMultiMap *current_read = read_map_;
    auto it = current_read->lower_bound(key);

    for (; it != current_read->end(); ++it) {
        result.emplace_back(it->first, it->second.value_);
    }

    return result;
}

template <typename Key, typename Value>
Vector<Pair<Key, Value>> RcuMultiMap<Key, Value>::upper_bound(const Key &key) {
    Vector<Pair<Key, Value>> result;

    InnerMultiMap *current_read = read_map_;
    auto it = current_read->upper_bound(key);

    for (; it != current_read->end(); ++it) {
        result.emplace_back(it->first, it->second.value_);
    }

    return result;
}

template <typename Key, typename Value>
u32 RcuMultiMap<Key, Value>::range(const Key &key_min, const Key &key_max, Vector<Value> &result) const {

    InnerMultiMap *current_read = read_map_;
    auto read_begin = current_read->lower_bound(key_min);
    auto read_end = current_read->upper_bound(key_max);

    InnerMultiMap *dirty_snapshot = nullptr;
    {
        std::lock_guard<std::mutex> lock(dirty_lock_);
        dirty_snapshot = dirty_map_;
    }

    auto dirty_begin = dirty_snapshot->lower_bound(key_min);
    auto dirty_end = dirty_snapshot->upper_bound(key_max);

    // merge
    u32 count = 0;
    auto read_it = read_begin;
    auto dirty_it = dirty_begin;

    while (read_it != read_end || dirty_it != dirty_end) {
        bool use_read = false;

        if (read_it != read_end && dirty_it != dirty_end) {
            if (read_it->first < dirty_it->first) {
                use_read = true;
            } else if (dirty_it->first < read_it->first) {
                use_read = false;
            } else {
                // Use dirty_map if key is the same
                use_read = false;
            }
        } else if (read_it != read_end) {
            use_read = true;
        } else {
            use_read = false;
        }

        if (use_read) {
            result.push_back(read_it->second.value_);
            ++read_it;
            ++count;
        } else {
            result.push_back(dirty_it->second.value_);
            ++dirty_it;
            ++count;
        }
    }
    return count;
}

} // namespace infinity

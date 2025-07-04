module;

#include <cassert>
#include <chrono>
#include <mutex>

export module rcu_multimap;

import stl;

namespace infinity {

export template <typename Value>
void ValueNoOp(Value * /*value*/) {}

export template <typename Value>
void ValueIncrementRef(Value *value) {
    value->IncrementRef();
}

export template <typename Value>
void ValueDecrementRef(Value *value) {
    value->DecrementRef();
}

export template <typename Value>
void ValueDelete(Value *value) {
    delete value;
}

export template <typename Key, typename Value>
class RcuMultiMap {
private:
    struct MapValue {
        Value *value_;
        volatile uint64_t used_time_;
    };

    typedef MultiMap<Key, MapValue *> InnerMultiMap;

    typedef void (*ValueOp)(Value *value);

    struct DeletedMap {
        InnerMultiMap *map_;
        Set<Pair<Key, MapValue *>> *deleted_entries_;
        uint64_t delete_time_;
    };

public:
    explicit RcuMultiMap(ValueOp allocator = ValueIncrementRef, ValueOp deallocator = ValueDecrementRef);

    ~RcuMultiMap();

    Vector<Value *> Get(const Key &key, bool update_access_time = true);

    // Add lowercase alias for compatibility
    Vector<Value *> get(const Key &key, bool update_access_time = true) { return Get(key, update_access_time); }

    Vector<Value *> GetWithRcuTime(const Key &key);

    void Insert(const Key &key, Value *value);

    // Add lowercase alias for compatibility
    void insert(const Key &key, Value *value) { Insert(key, value); }

    void Delete(const Key &key);

    // Add lowercase alias for compatibility
    void delete_key(const Key &key) { Delete(key); }

    void Delete(const Key &key, Value *value);

    void CheckGc(uint64_t min_delete_time);

    // Add lowercase alias for compatibility
    void checkGc(uint64_t min_delete_time) { CheckGc(min_delete_time); }

    void CheckExpired(uint64_t min_access_time, Vector<Key> &keys_need_expired);

    void GetAllValuesWithRef(Vector<Value *> &values);

    size_t size() const;

    template <typename... Args>
    void emplace(const Key &key, Args &&...args);

    Vector<Pair<Key, Value *>> lower_bound(const Key &key);

    Vector<Pair<Key, Value *>> upper_bound(const Key &key);

private:
    void CheckSwapInLock();
    MapValue *CreateMapValue(Value *value);

    // Helper function to get current time in milliseconds
    uint64_t GetCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

private:
    InnerMultiMap *volatile read_map_;
    std::size_t miss_time_;

    mutable std::mutex dirty_lock_;
    InnerMultiMap *dirty_map_;

    Set<Pair<Key, MapValue *>> deleted_entries_;

    List<MapValue> deleted_value_list_;
    List<DeletedMap> deleted_map_list_;

    ValueOp allocator_;
    ValueOp deallocator_;
};

template <typename Key, typename Value>
RcuMultiMap<Key, Value>::RcuMultiMap(ValueOp allocator, ValueOp deallocator) {
  read_map_ = new InnerMultiMap();
  miss_time_ = 0;
  dirty_map_ = new InnerMultiMap();
  allocator_ = allocator;
  deallocator_ = deallocator;
}

template <typename Key, typename Value>
RcuMultiMap<Key, Value>::~RcuMultiMap() {
  for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
    deallocator_(it->second->value_);
    delete it->second;
  }
  delete dirty_map_;

  for (const auto& entry : deleted_entries_) {
    auto it = read_map_->find(entry.first);
    if (it != read_map_->end() && it->second == entry.second) {
      delete it->second;
    }
  }
  delete read_map_;

  while (!deleted_value_list_.empty()) {
    deallocator_(deleted_value_list_.front().value_);
    deleted_value_list_.pop_front();
  }

  while (!deleted_map_list_.empty()) {
    DeletedMap& deleted_map = deleted_map_list_.front();
    for (const auto& entry : *deleted_map.deleted_entries_) {
      auto it = deleted_map.map_->find(entry.first);
      if (it != deleted_map.map_->end() && it->second == entry.second) {
        delete it->second;
      }
    }
    delete deleted_map.deleted_entries_;
    delete deleted_map.map_;
    deleted_map_list_.pop_front();
  }
}

template <typename Key, typename Value>
typename RcuMultiMap<Key, Value>::MapValue* RcuMultiMap<Key, Value>::CreateMapValue(Value* value) {
  MapValue* map_value = new MapValue();
  map_value->value_ = value;
  map_value->used_time_ = GetCurrentTimeMs();
  return map_value;
}

template <typename Key, typename Value>
Vector<Value *> RcuMultiMap<Key, Value>::Get(const Key &key, bool update_access_time) {
    Vector<Value *> results;

    InnerMultiMap *current_read = read_map_;
    auto range = current_read->equal_range(key);

    for (auto it = range.first; it != range.second; ++it) {
        if (it->second->value_ != nullptr) {
            if (update_access_time) {
                it->second->used_time_ = GetCurrentTimeMs();
            }
            results.push_back(it->second->value_);
            allocator_(it->second->value_);
        }
    }

    if (results.empty()) {
        std::lock_guard<std::mutex> lock(dirty_lock_);
        auto dirty_range = dirty_map_->equal_range(key);

        for (auto it = dirty_range.first; it != dirty_range.second; ++it) {
            if (it->second->value_ != nullptr) {
                if (update_access_time) {
                    it->second->used_time_ = GetCurrentTimeMs();
                }
                results.push_back(it->second->value_);
                allocator_(it->second->value_);
            }
        }

        if (!results.empty() && read_map_ == current_read) {
            miss_time_ += results.size();
            CheckSwapInLock();
        }
    }

    return results;
}

template <typename Key, typename Value>
Vector<Value *> RcuMultiMap<Key, Value>::GetWithRcuTime(const Key &key) {
    return Get(key, false);
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::CheckSwapInLock() {
  if (miss_time_ < dirty_map_->size()) {
    return;
  }

  InnerMultiMap* new_dirty_map = new InnerMultiMap(*dirty_map_);
  DeletedMap deleted_map;
  deleted_map.map_ = read_map_;
  read_map_ = dirty_map_;
  deleted_map.delete_time_ = GetCurrentTimeMs();
  deleted_map.deleted_entries_ = new Set<Pair<Key, MapValue *>>();
  deleted_map.deleted_entries_->swap(deleted_entries_);
  dirty_map_ = new_dirty_map;
  deleted_map_list_.push_back(deleted_map);
  miss_time_ = 0;
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Insert(const Key& key, Value* value) {
  if (value == nullptr) return;
  
  MapValue* new_value = CreateMapValue(value);

  std::lock_guard<std::mutex> lock(dirty_lock_);
  dirty_map_->insert(MakePair(key, new_value));
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Delete(const Key& key) {
    std::lock_guard<std::mutex> lock(dirty_lock_);

    auto range = dirty_map_->equal_range(key);
    for (auto it = range.first; it != range.second;) {
        MapValue *map_value = it->second;
        assert(map_value != nullptr);

        it = dirty_map_->erase(it);

        map_value->used_time_ = GetCurrentTimeMs();
        deleted_value_list_.push_back(*map_value);

        auto read_range = read_map_->equal_range(key);
        for (auto rit = read_range.first; rit != read_range.second; ++rit) {
            if (rit->second == map_value) {
                deleted_entries_.insert(MakePair(key, map_value));
                break;
            }
        }
    }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::Delete(const Key& key, Value* value) {
  if (value == nullptr) return;

  std::lock_guard<std::mutex> lock(dirty_lock_);

  auto range = dirty_map_->equal_range(key);
  for (auto it = range.first; it != range.second; ) {
    if (it->second->value_ == value) {
      MapValue* map_value = it->second;
      
      it = dirty_map_->erase(it);
      
      map_value->used_time_ = GetCurrentTimeMs();
      deleted_value_list_.push_back(*map_value);
      
      auto read_range = read_map_->equal_range(key);
      for (auto rit = read_range.first; rit != read_range.second; ++rit) {
        if (rit->second == map_value) {
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
void RcuMultiMap<Key, Value>::CheckGc(uint64_t min_delete_time) {
    Vector<Value *> values_need_delete;
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

  for (auto value : values_need_delete) {
    deallocator_(value);
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
  
  for (auto& deleted_map : map_need_delete) {
    for (const auto& entry : *deleted_map.deleted_entries_) {
      auto it = deleted_map.map_->find(entry.first);
      if (it != deleted_map.map_->end() && it->second == entry.second) {
        delete it->second;
      }
    }
    delete deleted_map.deleted_entries_;
    delete deleted_map.map_;
  }
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::CheckExpired(uint64_t min_access_time, Vector<Key> &keys_need_expired) {
    std::lock_guard<std::mutex> lock(dirty_lock_);
    Set<Key> expired_keys;

    for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
        if (it->second->used_time_ <= min_access_time) {
            expired_keys.insert(it->first);
        }
    }

    keys_need_expired.assign(expired_keys.begin(), expired_keys.end());
}

template <typename Key, typename Value>
void RcuMultiMap<Key, Value>::GetAllValuesWithRef(Vector<Value *> &values) {
    std::lock_guard<std::mutex> lock(dirty_lock_);
    for (auto it = dirty_map_->begin(); it != dirty_map_->end(); ++it) {
        if (it->second->value_ != nullptr) {
            allocator_(it->second->value_);
            values.push_back(it->second->value_);
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
  Value* value = new Value(std::forward<Args>(args)...);
  Insert(key, value);
}

template <typename Key, typename Value>
Vector<Pair<Key, Value *>> RcuMultiMap<Key, Value>::lower_bound(const Key &key) {
    Vector<Pair<Key, Value *>> result;

    InnerMultiMap *current_read = read_map_;
    auto it = current_read->lower_bound(key);

    for (; it != current_read->end(); ++it) {
        if (it->second->value_ != nullptr) {
            allocator_(it->second->value_);
            result.emplace_back(it->first, it->second->value_);
        }
    }

    return result;
}

template <typename Key, typename Value>
Vector<Pair<Key, Value *>> RcuMultiMap<Key, Value>::upper_bound(const Key &key) {
    Vector<Pair<Key, Value *>> result;

    InnerMultiMap *current_read = read_map_;
    auto it = current_read->upper_bound(key);

    for (; it != current_read->end(); ++it) {
        if (it->second->value_ != nullptr) {
            allocator_(it->second->value_);
            result.emplace_back(it->first, it->second->value_);
        }
    }

    return result;
}

} // namespace infinity

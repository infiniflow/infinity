// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:obj_stat_accessor;

import :obj_status;

import std;
import std.compat;
import third_party;

namespace infinity {

class KVInstance;

struct LRUListEntry {
    LRUListEntry(std::string key, ObjStat obj_stat) : key_(std::move(key)), obj_stat_(std::move(obj_stat)) {}

    std::string key_{};
    ObjStat obj_stat_{};
};

class ObjectStatMap {
    using LRUList = std::list<LRUListEntry>;
    using LRUListIter = LRUList::iterator;
    using LRUMap = std::unordered_map<std::string, LRUListIter>;

public:
    ~ObjectStatMap();

    // Get stat of object[key], if not in cache, return nullptr
    // called when read object from local cache, ref count by 1, if ref count 0 -> 1, move from lru_list to using_list
    ObjStat *Get(const std::string &key);

    // Get stat of object[key], and not update lru and ref count
    ObjStat *GetNoCount(const std::string &key);

    // Release object[key], decrease ref count by 1, if ref count 1 -> 0, move from using_list to lru_list
    // called when object file is closed, return true if object[key] exists and ref count -> 0
    std::pair<bool, ObjStat *> Release(const std::string &key);

    // Add new object to cache.
    // the object[key] is not in obj_map, add key->obj_stat mapping. called by checkpoint.
    void PutNew(const std::string &key, ObjStat obj_stat);

    // Recover old object
    // the object[key] must in obj_map, and must in cleanuped_list. called when download cleanuped object from remote storage to local cache
    void Recover(const std::string &key);

    // Invalidate object[key]
    // called when the object[key] should be cleaned up in remote storage. remove mapping and return the obj_stat if exists
    std::optional<ObjStat> Invalidate(const std::string &key);

    // Envict old object
    // move the last object in lru_list to cleanuped_list. called when disk used over limit, return nullptr if lru_list is empty
    LRUListEntry *EnvictLast();

    const LRUMap &obj_map() const { return obj_map_; }

private:
    LRUMap obj_map_{};
    LRUList lru_list_{};
    LRUList using_list_{};
    LRUList cleanuped_list_{};
};

// export class

export class ObjectStatAccessorBase {
public:
    virtual ~ObjectStatAccessorBase() = default;

    virtual std::optional<ObjStat> Get(const std::string &key) = 0;

    virtual std::optional<ObjStat> GetNoCount(const std::string &key) = 0;

    virtual std::optional<ObjStat> Release(const std::string &key, std::vector<std::string> &drop_keys) = 0;

    virtual void PutNew(const std::string &key, ObjStat obj_stat, std::vector<std::string> &drop_keys) = 0;

    virtual void PutNoCount(const std::string &key, ObjStat obj_stat) = 0;

    virtual std::optional<ObjStat> Invalidate(const std::string &key) = 0;

    virtual void CheckValid(size_t current_object_size) = 0;

    virtual nlohmann::json Serialize() = 0;

    virtual void Deserialize(std::string_view obj_str) = 0;

    virtual void Deserialize(KVInstance *kv_instance) = 0;

    virtual std::unordered_map<std::string, ObjStat> GetAllObjects() const = 0;

protected:
    void AddObjStatToKVStore(const std::string &key, const ObjStat &obj_stat);

    void RemoveObjStatFromKVStore(const std::string &key);
};

export class ObjectStatAccessor_LocalStorage : public ObjectStatAccessorBase {
public:
    ~ObjectStatAccessor_LocalStorage() override;

    std::optional<ObjStat> Get(const std::string &key) override;

    std::optional<ObjStat> GetNoCount(const std::string &key) override;

    std::optional<ObjStat> Release(const std::string &key, std::vector<std::string> &drop_keys) override;

    void PutNew(const std::string &key, ObjStat obj_stat, std::vector<std::string> &drop_keys) override;

    void PutNoCount(const std::string &key, ObjStat obj_stat) override;

    std::optional<ObjStat> Invalidate(const std::string &key) override;

    void CheckValid(size_t current_object_size) override;

    nlohmann::json Serialize() override;

    void Deserialize(std::string_view obj_str) override;

    void Deserialize(KVInstance *kv_instance) override;

    std::unordered_map<std::string, ObjStat> GetAllObjects() const override;

private:
    mutable std::mutex mutex_{}; // protect obj_map_
    std::unordered_map<std::string, ObjStat> obj_map_{};
};

// envict and recover is encapsulated
export class ObjectStatAccessor_ObjectStorage : public ObjectStatAccessorBase {
public:
    ObjectStatAccessor_ObjectStorage(size_t disk_capacity_limit);

    ~ObjectStatAccessor_ObjectStorage() override;

    std::optional<ObjStat> Get(const std::string &key) override;

    std::optional<ObjStat> GetNoCount(const std::string &key) override;

    std::optional<ObjStat> Release(const std::string &key, std::vector<std::string> &drop_keys) override;

    void PutNew(const std::string &key, ObjStat obj_stat, std::vector<std::string> &drop_keys) override;

    void PutNoCount(const std::string &key, ObjStat obj_stat) override;

    std::optional<ObjStat> Invalidate(const std::string &key) override;

    void CheckValid(size_t current_object_size) override;

    nlohmann::json Serialize() override;

    void Deserialize(std::string_view obj_str) override;

    void Deserialize(KVInstance *kv_instance) override;

    std::unordered_map<std::string, ObjStat> GetAllObjects() const override;

    size_t disk_used() const { return disk_used_; }

private:
    bool EnvictNoLock(std::vector<std::string> &drop_keys);

private:
    const size_t disk_capacity_limit_{};

    mutable std::shared_mutex mutex_{}; // protect obj_map_, disk_used_
    ObjectStatMap obj_map_{};
    size_t disk_used_{};
};

} // namespace infinity

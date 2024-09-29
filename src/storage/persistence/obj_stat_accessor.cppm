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

module;

export module obj_stat_accessor;

import stl;
import obj_status;
import third_party;

namespace infinity {

struct LRUListEntry {
    String path_{};
    ObjStat obj_stat_{};
};

class ObjectStatMap {
    using LRUList = List<LRUListEntry>;
    using LRUListIter = LRUList::iterator;
    using LRUMap = HashMap<String, LRUListIter>;

public:
    // Get stat of object[key], if not in cache, return nullptr
    // called when read object from local cache, ref count by 1, if ref count 0 -> 1, move from lru_list to using_list
    ObjStat *Get(const String &key);

    // the same with Get, but not increase ref count
    ObjStat *GetNoCount(const String &key);

    // Release object[key], decrease ref count by 1, if ref count 1 -> 0, move from using_list to lru_list
    // called when object file is closed
    bool Release(const String &key);

    // Add new object to cache.
    // the object[key] is not in obj_map, add key->obj_stat mapping. called by checkpoint.
    void PutNew(const String &key, ObjStat obj_stat);

    // Recover old object
    // the object[key] must in obj_map, and must in cleanuped_list. called when download cleanuped object from remote storage to local cache
    void Recover(const String &key);

    // Invalidate object[key]
    // called when the object[key] should be cleaned up in remote storage. remove mapping and return the obj_stat if exists
    Optional<ObjStat> Invalidate(const String &key);

    // Envict old object
    // move the last object in lru_list to cleanuped_list. called when disk used over limit, return nullptr if lru_list is empty
    ObjStat *EnvictLast();

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

    virtual ObjStat *Get(const String &key) = 0;

    virtual ObjStat *GetNoCount(const String &key) = 0;

    virtual ObjStat *Release(const String &key) = 0;

    virtual void PutNew(const String &key, ObjStat obj_stat) = 0;

    virtual Optional<ObjStat> Invalidate(const String &key) = 0;

    virtual void CheckValid(SizeT current_object_size) = 0;

    virtual nlohmann::json Serialize() = 0;

    virtual void Deserialize(const nlohmann::json &obj) = 0;

    virtual HashMap<String, ObjStat> GetAllObjects() const = 0;
};

export class ObjectStatAccessor_LocalStorage : public ObjectStatAccessorBase {
public:
    ~ObjectStatAccessor_LocalStorage() override;

    ObjStat *Get(const String &key) override;

    ObjStat *GetNoCount(const String &key) override;

    ObjStat *Release(const String &key) override;

    void PutNew(const String &key, ObjStat obj_stat) override;

    Optional<ObjStat> Invalidate(const String &key) override;

    void CheckValid(SizeT current_object_size) override;

    nlohmann::json Serialize() override;

    void Deserialize(const nlohmann::json &obj) override;

    HashMap<String, ObjStat> GetAllObjects() const override;

private:
    HashMap<String, ObjStat> obj_map_{};
};

// envict and recover is encapsulated
export class ObjectStatAccessor_ObjectStorage : public ObjectStatAccessorBase {
public:
    ObjStat *Get(const String &key) override;

    ObjStat *GetNoCount(const String &key) override;

    ObjStat *Release(const String &key) override;

    void PutNew(const String &key, ObjStat obj_stat) override;

    Optional<ObjStat> Invalidate(const String &key) override;

    void CheckValid(SizeT current_object_size) override;

    nlohmann::json Serialize() override;

    void Deserialize(const nlohmann::json &obj) override;

    HashMap<String, ObjStat> GetAllObjects() const override;

private:
    ObjectStatMap obj_map_{};

    const SizeT disk_capacity_limit_{};
    SizeT disk_used_{};
};

} // namespace infinity

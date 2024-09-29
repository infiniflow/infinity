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

#include <__iterator/prev.h>
#include <cassert>

module obj_stat_accessor;

import infinity_exception;
import logger;
import third_party;

namespace infinity {

// ObjectStatMap

ObjectStatMap::~ObjectStatMap() {
    [[maybe_unused]] SizeT sum_ref_count = 0;
    for (const auto &lru_iter : lru_list_) {
        if (lru_iter.obj_stat_.ref_count_ > 0) {
            LOG_ERROR(fmt::format("Object {} still has ref count {}", lru_iter.key_, lru_iter.obj_stat_.ref_count_));
        }
        sum_ref_count += lru_iter.obj_stat_.ref_count_;
    }
    assert(sum_ref_count == 0);
}

ObjStat *ObjectStatMap::Get(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return nullptr;
    }
    LRUListIter lru_iter = map_iter->second;
    ObjStat *obj_stat = &lru_iter->obj_stat_;
    if (obj_stat->ref_count_ == 0) {
        using_list_.splice(using_list_.begin(), lru_list_, lru_iter);
    }
    ++obj_stat->ref_count_;
    return obj_stat;
}

ObjStat *ObjectStatMap::GetNoCount(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return nullptr;
    }
    return &map_iter->second->obj_stat_;
}

Pair<bool, ObjStat *> ObjectStatMap::Release(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return {false, nullptr};
    }
    LRUListIter lru_iter = map_iter->second;
    ObjStat *obj_stat = &lru_iter->obj_stat_;
    if (obj_stat->ref_count_ <= 0) {
        UnrecoverableError(fmt::format("Release object {} ref count is {}", key, obj_stat->ref_count_));
    }
    --obj_stat->ref_count_;
    if (obj_stat->ref_count_ > 0) {
        return {false, obj_stat};
    }
    lru_list_.splice(lru_list_.begin(), using_list_, lru_iter);
    return {true, obj_stat};
}

void ObjectStatMap::PutNew(const String &key, ObjStat obj_stat) {
    auto map_iter = obj_map_.find(key);
    if (map_iter != obj_map_.end()) {
        map_iter->second->obj_stat_ = std::move(obj_stat);
        LOG_DEBUG(fmt::format("PutNew: {} is already in object map", key));
        return;
    }
    lru_list_.emplace_front(key, std::move(obj_stat), false);
    obj_map_.emplace(key, lru_list_.begin());
}

void ObjectStatMap::Recover(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        UnrecoverableError(fmt::format("Recover object {} not found", key));
    }
    LRUListIter lru_iter = map_iter->second;
    if (lru_iter->obj_stat_.ref_count_ > 0) {
        UnrecoverableError(fmt::format("Recover object {} ref count is {}", key, lru_iter->obj_stat_.ref_count_));
    }
    lru_list_.splice(lru_list_.begin(), cleanuped_list_, lru_iter);
    if (lru_iter->cleaned_ == false) {
        UnrecoverableError(fmt::format("Recover object {} not cleaned", key));
    }
    lru_iter->cleaned_ = false;
}

Optional<ObjStat> ObjectStatMap::Invalidate(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return None;
    }
    LRUListIter lru_iter = map_iter->second;
    ObjStat obj_stat = std::move(lru_iter->obj_stat_);
    if (obj_stat.ref_count_ > 0) {
        UnrecoverableError(fmt::format("Invalidate object {} ref count is {}", key, obj_stat.ref_count_));
    }
    if (lru_iter->cleaned_) {
        cleanuped_list_.erase(lru_iter);
    } else {
        lru_list_.erase(lru_iter);
    }
    obj_map_.erase(map_iter);
    return obj_stat;
}

LRUListEntry *ObjectStatMap::EnvictLast() {
    if (lru_list_.empty()) {
        return nullptr;
    }
    LRUListIter lru_iter = std::prev(lru_list_.end());
    ObjStat *obj_stat = &lru_iter->obj_stat_;
    if (obj_stat->ref_count_ > 0) {
        UnrecoverableError(fmt::format("EnvictLast object {} ref count is {}", lru_iter->key_, obj_stat->ref_count_));
    }
    if (lru_iter->cleaned_) {
        UnrecoverableError(fmt::format("EnvictLast object {} is already cleaned", lru_iter->key_));
    }
    lru_iter->cleaned_ = true;
    cleanuped_list_.splice(cleanuped_list_.begin(), lru_list_, lru_iter);
    return &(*lru_iter);
}

// ObjectStatAccessor_LocalStorage

ObjectStatAccessor_LocalStorage::~ObjectStatAccessor_LocalStorage() {
    [[maybe_unused]] SizeT sum_ref_count = 0;
    for (const auto &[key, obj_stat] : obj_map_) {
        if (obj_stat.ref_count_ > 0) {
            LOG_ERROR(fmt::format("Object {} still has ref count {}", key, obj_stat.ref_count_));
        }
        sum_ref_count += obj_stat.ref_count_;
    }
    assert(sum_ref_count == 0);
}

ObjStat *ObjectStatAccessor_LocalStorage::Get(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return nullptr;
    }
    ObjStat *obj_stat = &map_iter->second;
    ++obj_stat->ref_count_;
    return obj_stat;
}

ObjStat *ObjectStatAccessor_LocalStorage::GetNoCount(const String &key) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return nullptr;
    }
    ObjStat *obj_stat = &map_iter->second;
    return obj_stat;
}

ObjStat *ObjectStatAccessor_LocalStorage::Release(const String &key, Vector<String> &) {
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return nullptr;
    }
    ObjStat *obj_stat = &map_iter->second;
    if (obj_stat->ref_count_ <= 0) {
        UnrecoverableError(fmt::format("Release object {} ref count is {}", key, obj_stat->ref_count_));
    }
    --obj_stat->ref_count_;
    return obj_stat;
}

void ObjectStatAccessor_LocalStorage::PutNew(const String &key, ObjStat obj_stat, Vector<String> &) {
    auto map_iter = obj_map_.find(key);
    if (map_iter != obj_map_.end()) {
        UnrecoverableError(fmt::format("PutNew object {} is already in object map", key));
    }
    obj_map_.emplace_hint(map_iter, key, std::move(obj_stat));
}

void ObjectStatAccessor_LocalStorage::PutNoCount(const String &key, ObjStat obj_stat) {
    auto [iter, insert_ok] = obj_map_.insert_or_assign(key, std::move(obj_stat));
    if (!insert_ok) {
        LOG_DEBUG(fmt::format("PutNew: {} is already in object map", key));
    }
}

Optional<ObjStat> ObjectStatAccessor_LocalStorage::Invalidate(const String &key) {
    auto iter = obj_map_.find(key);
    if (iter == obj_map_.end()) {
        return None;
    }
    ObjStat obj_stat = std::move(iter->second);
    obj_map_.erase(iter);
    return obj_stat;
}

void ObjectStatAccessor_LocalStorage::CheckValid(SizeT current_object_size) {
    for (auto &[obj_key, obj_stat] : obj_map_) {
        obj_stat.CheckValid(obj_key, current_object_size);
    }
}

nlohmann::json ObjectStatAccessor_LocalStorage::Serialize() {
    nlohmann::json json_obj;
    json_obj["obj_stat_size"] = obj_map_.size();
    json_obj["obj_stat_array"] = nlohmann::json::array();
    for (auto &[obj_key, obj_stat] : obj_map_) {
        nlohmann::json pair;
        pair["obj_key"] = obj_key;
        pair["obj_stat"] = obj_stat.Serialize();
        json_obj["obj_stat_array"].emplace_back(pair);
    }
    return json_obj;
}

void ObjectStatAccessor_LocalStorage::Deserialize(const nlohmann::json &obj) {
    SizeT len = 0;
    if (obj.contains("obj_stat_size")) {
        len = obj["obj_stat_size"];
    }
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["obj_stat_array"][i];
        String obj_key = json_pair["obj_key"];
        ObjStat obj_stat;
        obj_stat.Deserialize(json_pair["obj_stat"]);
        obj_map_.emplace(obj_key, obj_stat);
        LOG_TRACE(fmt::format("Deserialize added object {}", obj_key));
    }
}

HashMap<String, ObjStat> ObjectStatAccessor_LocalStorage::GetAllObjects() const { return obj_map_; }

// ObjectStatAccessor_ObjectStorage

ObjectStatAccessor_ObjectStorage::ObjectStatAccessor_ObjectStorage(SizeT disk_capacity_limit) : disk_capacity_limit_(disk_capacity_limit) {}

ObjectStatAccessor_ObjectStorage::~ObjectStatAccessor_ObjectStorage() = default;

ObjStat *ObjectStatAccessor_ObjectStorage::Get(const String &key) {
    ObjStat *obj_stat = obj_map_.Get(key);
    if (obj_stat == nullptr) {
        return nullptr;
    }
    if (disk_used_ < obj_stat->obj_size_) {
        UnrecoverableError(fmt::format("Object {} size {} is larger than disk used {}", key, obj_stat->obj_size_, disk_used_));
    }
    disk_used_ -= obj_stat->obj_size_;
    return obj_stat;
}

ObjStat *ObjectStatAccessor_ObjectStorage::GetNoCount(const String &key) { return obj_map_.GetNoCount(key); }

ObjStat *ObjectStatAccessor_ObjectStorage::Release(const String &key, Vector<String> &drop_keys) {
    auto [release_ok, obj_stat] = obj_map_.Release(key);
    if (!release_ok) {
        return obj_stat;
    }
    disk_used_ += obj_stat->obj_size_;
    if (disk_used_ > disk_capacity_limit_) {
        Envict(drop_keys);
    }
    return obj_stat;
}

void ObjectStatAccessor_ObjectStorage::PutNew(const String &key, ObjStat obj_stat, Vector<String> &drop_keys) {
    obj_map_.PutNew(key, std::move(obj_stat));
    disk_used_ += obj_stat.obj_size_;
    if (disk_used_ > disk_capacity_limit_) {
        Envict(drop_keys);
    }
}

void ObjectStatAccessor_ObjectStorage::PutNoCount(const String &key, ObjStat obj_stat) { obj_map_.PutNew(key, std::move(obj_stat)); }

Optional<ObjStat> ObjectStatAccessor_ObjectStorage::Invalidate(const String &key) {
    Optional<ObjStat> obj_stat = obj_map_.Invalidate(key);
    if (!obj_stat.has_value()) {
        return None;
    }
    disk_used_ -= obj_stat->obj_size_;
    return obj_stat;
}

void ObjectStatAccessor_ObjectStorage::CheckValid(SizeT current_object_size) {
    for (const auto &[obj_key, lru_iter] : obj_map_.obj_map()) {
        lru_iter->obj_stat_.CheckValid(obj_key, current_object_size);
    }
}

nlohmann::json ObjectStatAccessor_ObjectStorage::Serialize() {
    nlohmann::json json_obj;
    json_obj["obj_stat_size"] = obj_map_.obj_map().size();
    json_obj["obj_stat_array"] = nlohmann::json::array();
    for (const auto &[obj_key, lru_iter] : obj_map_.obj_map()) {
        nlohmann::json pair;
        pair["obj_key"] = obj_key;
        pair["obj_stat"] = lru_iter->obj_stat_.Serialize();
        json_obj["obj_stat_array"].emplace_back(pair);
    }
    return json_obj;
}

void ObjectStatAccessor_ObjectStorage::Deserialize(const nlohmann::json &obj) {
    SizeT len = 0;
    if (obj.contains("obj_stat_size")) {
        len = obj["obj_stat_size"];
    }
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["obj_stat_array"][i];
        String obj_key = json_pair["obj_key"];
        ObjStat obj_stat;
        obj_stat.Deserialize(json_pair["obj_stat"]);
        obj_map_.PutNew(obj_key, std::move(obj_stat));
        LOG_TRACE(fmt::format("Deserialize added object {}", obj_key));
    }
}

HashMap<String, ObjStat> ObjectStatAccessor_ObjectStorage::GetAllObjects() const {
    HashMap<String, ObjStat> res;
    for (const auto &[key, lru_iter] : obj_map_.obj_map()) {
        res.emplace(key, lru_iter->obj_stat_);
    }
    return res;
}

bool ObjectStatAccessor_ObjectStorage::Envict(Vector<String> &drop_keys) {
    while (disk_used_ > disk_capacity_limit_) {
        LRUListEntry *lru_entry = obj_map_.EnvictLast();
        if (lru_entry == nullptr) {
            break;
        }
        drop_keys.push_back(lru_entry->key_);
        disk_used_ -= lru_entry->obj_stat_.obj_size_;
    }
    if (disk_used_ > disk_capacity_limit_) {
        LOG_WARN(fmt::format("Envict disk used {} is larger than disk capacity limit {}", disk_used_, disk_capacity_limit_));
        return false;
    }
    return true;
}

} // namespace infinity

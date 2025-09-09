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

#include <cassert>

module infinity_core:obj_stat_accessor.impl;

import :obj_stat_accessor;
import :infinity_exception;
import :logger;
import :obj_status;
import :kv_store;
import :storage;
import :kv_code;
import :status;
import :infinity_context;

import std;
import third_party;

namespace infinity {

// ObjectStatAccessor

ObjectStatAccessor::~ObjectStatAccessor() {
    std::unique_lock<std::mutex> lock(mutex_);
    [[maybe_unused]] size_t sum_ref_count = 0;
    for (const auto &[key, obj_stat] : obj_map_) {
        if (obj_stat.ref_count_ > 0) {
            LOG_ERROR(fmt::format("ObjectStatAccessor {} still has ref count {}", key, obj_stat.ref_count_));
        }
        sum_ref_count += obj_stat.ref_count_;
    }
    assert(sum_ref_count == 0);
}

std::optional<ObjStat> ObjectStatAccessor::Get(const std::string &key) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return std::nullopt;
    }
    ++map_iter->second.ref_count_;
    return map_iter->second;
}

std::optional<ObjStat> ObjectStatAccessor::GetNoCount(const std::string &key) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return std::nullopt;
    }
    return map_iter->second;
}

std::optional<ObjStat> ObjectStatAccessor::Release(const std::string &key, std::vector<std::string> &drop_keys) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto map_iter = obj_map_.find(key);
    if (map_iter == obj_map_.end()) {
        return std::nullopt;
    }
    if (map_iter->second.ref_count_ <= 0) {
        UnrecoverableError(fmt::format("Release object {} ref count is {}", key, map_iter->second.ref_count_));
    }
    --map_iter->second.ref_count_;
    return map_iter->second;
}

void ObjectStatAccessor::PutNew(const std::string &key, ObjStat obj_stat, std::vector<std::string> &drop_keys) {
    this->AddObjStatToKVStore(key, obj_stat);

    std::unique_lock<std::mutex> lock(mutex_);
    auto map_iter = obj_map_.find(key);
    if (map_iter != obj_map_.end()) {
        UnrecoverableError(fmt::format("PutNew object {} is already in object map", key));
    }
    obj_stat.cached_ = ObjCached::kCached;
    obj_map_.emplace_hint(map_iter, key, std::move(obj_stat));
}

void ObjectStatAccessor::PutNoCount(const std::string &key, ObjStat obj_stat) {
    obj_stat.cached_ = ObjCached::kCached;
    this->AddObjStatToKVStore(key, obj_stat); // obj_stat.ref_count_ isn't update if key can be found in obj_map_

    std::unique_lock<std::mutex> lock(mutex_);
    auto map_iter = obj_map_.find(key);
    if (map_iter != obj_map_.end()) {
        obj_stat.ref_count_ = map_iter->second.ref_count_;
        LOG_DEBUG(fmt::format("PutNew: {} is already in object map", key));
    }
    obj_map_.insert_or_assign(key, std::move(obj_stat));
}

std::optional<ObjStat> ObjectStatAccessor::Invalidate(const std::string &key) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = obj_map_.find(key);
    if (iter == obj_map_.end()) {
        return std::nullopt;
    }
    ObjStat obj_stat = std::move(iter->second);
    obj_map_.erase(iter);

    this->RemoveObjStatFromKVStore(key);
    return obj_stat;
}

void ObjectStatAccessor::CheckValid(size_t current_object_size) {
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto &[obj_key, obj_stat] : obj_map_) {
        obj_stat.CheckValid(obj_key, current_object_size);
    }
}

void ObjectStatAccessor::Deserialize(KVInstance *kv_instance) {
    const std::string &obj_stat_prefix = KeyEncode::PMObjectStatPrefix();
    size_t obj_stat_prefix_len = obj_stat_prefix.size();

    auto iter = kv_instance->GetIterator();
    iter->Seek(obj_stat_prefix);
    std::unique_lock<std::mutex> lock(mutex_);
    while (iter->Valid() && iter->Key().starts_with(obj_stat_prefix)) {
        std::string obj_key = iter->Key().ToString().substr(obj_stat_prefix_len);
        std::string obj_value = iter->Value().ToString();
        ObjStat obj_stat;
        obj_stat.Deserialize(obj_value);
        obj_stat.cached_ = ObjCached::kCached;
        LOG_TRACE(fmt::format("Deserialize added object {}", obj_key));
        obj_map_.emplace(std::move(obj_key), std::move(obj_stat));
        iter->Next();
    }
}

std::unordered_map<std::string, ObjStat> ObjectStatAccessor::GetAllObjects() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return obj_map_;
}

void ObjectStatAccessor::AddObjStatToKVStore(const std::string &key, const ObjStat &obj_stat) {
    Storage *storage = InfinityContext::instance().storage();
    if (!storage) {
        return;
    }
    KVStore *kv_store = storage->kv_store();
    if (!kv_store) {
        return;
    }
    Status status = kv_store->Put(KeyEncode::PMObjectStatKey(key), obj_stat.ToString());
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

void ObjectStatAccessor::RemoveObjStatFromKVStore(const std::string &key) {
    Storage *storage = InfinityContext::instance().storage();
    if (!storage) {
        return;
    }
    KVStore *kv_store = storage->kv_store();
    if (!kv_store) {
        return;
    }
    Status status = kv_store->Delete(KeyEncode::PMObjectStatKey(key));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

} // namespace infinity

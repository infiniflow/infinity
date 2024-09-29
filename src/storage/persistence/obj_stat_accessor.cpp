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
#include <__iterator/next.h>

module obj_stat_accessor;

import infinity_exception;
import logger;
import third_party;

namespace infinity {

// ObjectStatMap

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

ObjStat *ObjectStatAccessor_LocalStorage::Release(const String &key) {
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

void ObjectStatAccessor_LocalStorage::PutNew(const String &key, ObjStat obj_stat) {
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
        Set<Range> &deleted_ranges = obj_stat.deleted_ranges_;
        if (deleted_ranges.size() >= 2) {
            auto it1 = deleted_ranges.begin();
            auto it2 = std::next(it1);
            while (it2 != deleted_ranges.end()) {
                if (it1->end_ >= it2->start_) {
                    String error_message = fmt::format("CurrentObjFinalize Object {} deleted ranges intersect: [{}, {}), [{}, {})",
                                                       obj_key,
                                                       it1->start_,
                                                       it1->end_,
                                                       it2->start_,
                                                       it2->end_);
                    LOG_ERROR(error_message);
                }
                it1 = it2;
                it2 = std::next(it2);
            }
        } else if (deleted_ranges.size() == 1) {
            auto it1 = deleted_ranges.begin();
            if (it1->start_ == 0 && it1->end_ == current_object_size) {
                String error_message = fmt::format("CurrentObjFinalize Object {} is fully deleted", obj_key);
                LOG_ERROR(error_message);
            }
        }
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
    auto result = MakeUnique<ObjectStatAccessor_LocalStorage>();
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

HashMap<String, ObjStat> ObjectStatAccessor_LocalStorage::GetAllObjects() const {
    return obj_map_;
}


// ObjectStatAccessor_ObjectStorage

}

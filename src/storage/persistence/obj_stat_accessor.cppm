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

import std.compat;
import third_party;

namespace infinity {

class KVInstance;

export class ObjectStatAccessor {
public:
    virtual ~ObjectStatAccessor();

    std::optional<ObjStat> Get(const std::string &key);

    std::optional<ObjStat> GetNoCount(const std::string &key);

    std::optional<ObjStat> Release(const std::string &key);

    void PutNew(const std::string &key, ObjStat obj_stat);

    void PutNoCount(const std::string &key, ObjStat obj_stat);

    std::optional<ObjStat> Invalidate(const std::string &key);

    void CheckValid(size_t current_object_size);

    void Deserialize(KVInstance *kv_instance);

    std::unordered_map<std::string, ObjStat> GetAllObjects() const;

private:
    void AddObjStatToKVStore(const std::string &key, const ObjStat &obj_stat);

    void RemoveObjStatFromKVStore(const std::string &key);

private:
    mutable std::mutex mutex_{}; // protect obj_map_
    std::unordered_map<std::string, ObjStat> obj_map_{};
};

} // namespace infinity

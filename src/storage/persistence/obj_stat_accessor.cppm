// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
export module infinity_core:object_stats;

import :obj_status;

import std;

namespace infinity {

class Storage;
class KVInstance;

export class ObjectStats {
public:
    explicit ObjectStats(Storage *storage) : storage_(storage) {}

    virtual ~ObjectStats();

    std::shared_ptr<ObjStat> Get(const std::string &key);

    std::shared_ptr<ObjStat> GetNoCount(const std::string &key);

    std::shared_ptr<ObjStat> Release(const std::string &key);

    void PutNew(const std::string &key, std::shared_ptr<ObjStat> obj_stat);

    void PutNoCount(const std::string &key, std::shared_ptr<ObjStat> obj_stat);

    std::shared_ptr<ObjStat> Invalidate(const std::string &key);

    void CheckValid(size_t current_object_size);

    void Deserialize(KVInstance *kv_instance);

    std::unordered_map<std::string, std::shared_ptr<ObjStat>> GetAllObjects() const;

private:
    static void AddObjStatToKVStore(const std::string &key, const std::shared_ptr<ObjStat> &obj_stat);
    static void RemoveObjStatFromKVStore(const std::string &key);

    Storage *storage_{};

    mutable std::mutex mutex_{}; // protect obj_map_
    std::unordered_map<std::string, std::shared_ptr<ObjStat>> obj_map_{};
};

} // namespace infinity
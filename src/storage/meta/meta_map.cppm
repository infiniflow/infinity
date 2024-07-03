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

#include <type_traits>
#include <vector>

export module meta_map;

import stl;
import local_file_system;
import cleanup_scanner;
import meta_entry_interface;
import third_party;
import logger;
import txn_manager;
import extra_ddl_info;
import status;
import infinity_exception;

namespace infinity {

class TableMeta;
class DBMeta;
class TableIndexMeta;

export template <MetaConcept Meta>
class MetaMap {
public:
    using Map = HashMap<String, UniquePtr<Meta>>;

    struct MapGuard {
        Map &operator*() { return meta_map_; }

        Map &meta_map_;
        std::shared_lock<std::shared_mutex> r_lock_;
    };

public:
    Tuple<Meta *, std::shared_lock<std::shared_mutex>> GetMeta(const String &name, std::function<UniquePtr<Meta>()> &&init_func);

    Meta *GetMetaNoLock(const String &name, std::function<UniquePtr<Meta>()> &&init_func) {
        return std::get<0>(this->GetMeta(name, std::move(init_func)));
    }

    Tuple<Meta *, Status, std::shared_lock<std::shared_mutex>> GetExistMeta(const String &name, ConflictType conflict_type);

    Tuple<Meta *, Status> GetExistMetaNoLock(const String &name, ConflictType conflict_type) {
        auto [meta, status, r_lock] = this->GetExistMeta(name, conflict_type);
        return {meta, status};
    }

    MapGuard GetMetaMap() { return {meta_map_, std::shared_lock(rw_locker_)}; }

    void PickCleanup(CleanupScanner *scanner);

    void Cleanup();

public:                                     // TODO: make both private
    mutable std::shared_mutex rw_locker_{}; // FIX

    Map meta_map_;
};

template <MetaConcept Meta>
Tuple<Meta *, std::shared_lock<std::shared_mutex>> MetaMap<Meta>::GetMeta(const String &name, std::function<UniquePtr<Meta>()> &&init_func) {
    std::shared_lock r_lock(rw_locker_);
    auto iter = meta_map_.find(name);
    if (iter == meta_map_.end()) {
        r_lock.unlock();
        {
            UniquePtr<Meta> new_meta = init_func();
            std::unique_lock w_lock(rw_locker_);
            if (auto iter1 = meta_map_.find(name); iter1 != meta_map_.end()) {
                LOG_TRACE("Add new entry in new meta_map");
                iter = iter1;
            } else {
                LOG_TRACE("Add new entry in existed meta_map");
                iter = meta_map_.emplace(name, std::move(new_meta)).first;
            }
        }
        r_lock.lock();
    } else {
        LOG_TRACE("Add new entry in existed meta_map");
    }
    return {iter->second.get(), std::move(r_lock)};
}

template <MetaConcept Meta>
Tuple<Meta *, Status, std::shared_lock<std::shared_mutex>> MetaMap<Meta>::GetExistMeta(const String &name, ConflictType conflict_type) {
    std::shared_lock r_lock(rw_locker_);
    if (auto iter = meta_map_.find(name); iter != meta_map_.end()) {
        return {iter->second.get(), Status::OK(), std::move(r_lock)};
    }
    if (conflict_type == ConflictType::kIgnore) {
        LOG_TRACE(fmt::format("Ignore drop a non-exist meta: {}", name));
        return {nullptr, Status::Ignore(), std::move(r_lock)};
    }

    if constexpr (std::is_same_v<Meta, TableMeta>) {
        auto err_msg = MakeUnique<String>(fmt::format("Table {} doesn't exist", name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg)), std::move(r_lock)};
    } else if constexpr (std::is_same_v<Meta, DBMeta>) {
        auto err_msg = MakeUnique<String>(fmt::format("Database {} doesn't exist", name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kDBNotExist, std::move(err_msg)), std::move(r_lock)};
    } else if constexpr (std::is_same_v<Meta, TableIndexMeta>) {
        auto err_msg = MakeUnique<String>(fmt::format("Index {} doesn't exist", name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg)), std::move(r_lock)};
    } else {
        String error_message = "Unimplemented";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

template <MetaConcept Meta>
void MetaMap<Meta>::PickCleanup(CleanupScanner *scanner) {
    Vector<Meta *> metas;
    {
        std::unique_lock w_lock(rw_locker_);
        for (auto &[name, meta] : meta_map_) {
            metas.push_back(meta.get());
        }
    }
    bool may_empty = false;
    for (auto *meta : metas) {
        may_empty |= meta->PickCleanup(scanner);
    }
    if (may_empty) {
        std::unique_lock w_lock(rw_locker_);
        for (auto iter = meta_map_.begin(); iter != meta_map_.end();) {
            if (iter->second->Empty()) {
                LOG_DEBUG(fmt::format("PickCleanup: all_delete: {}", iter->first));
                iter = meta_map_.erase(iter);
            } else {
                ++iter;
            }
        }
    }
}

template <MetaConcept Meta>
void MetaMap<Meta>::Cleanup() {
    for (auto &[name, meta] : meta_map_) {
        meta->Cleanup();
    }
}

} // namespace infinity
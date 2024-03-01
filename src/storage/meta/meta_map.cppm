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

export module meta_map;

import stl;
import local_file_system;
import cleanup_scanner;
import meta_entry_interface;
import third_party;
import logger;
import txn_manager;

namespace infinity {

export template <MetaConcept Meta>
class MetaMap {
public:
    using Map = HashMap<String, SharedPtr<Meta>>;

public:
    Tuple<Meta *, std::shared_lock<std::shared_mutex>>
    GetMeta(const String &name, std::function<UniquePtr<Meta>()> &&init_func, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    void Iterate(std::function<void(Meta *)> func);

    void PickCleanup(CleanupScanner *scanner);

    void Cleanup();

public:                                     // TODO: make both private
    mutable std::shared_mutex rw_locker_{}; // FIX

    Map meta_map_;
};

template <MetaConcept Meta>
Tuple<Meta *, std::shared_lock<std::shared_mutex>> MetaMap<Meta>::GetMeta(const String &name,
                                                                          std::function<UniquePtr<Meta>()> &&init_func,
                                                                          TransactionID txn_id,
                                                                          TxnTimeStamp begin_ts,
                                                                          TxnManager *txn_mgr) {
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
                // When replay database creation,if the txn_manger is nullptr, represent the txn manager not running, it is reasonable.
                // In replay phase, not need to recording the catalog delta operation.
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<typename Meta::MetaOp>(new_meta.get(), begin_ts);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
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
void MetaMap<Meta>::Iterate(std::function<void(Meta *)> func) {
    std::unique_lock lock(rw_locker_);
    for (auto &[name, meta] : meta_map_) {
        func(meta.get());
    }
}

template <MetaConcept Meta>
void MetaMap<Meta>::PickCleanup(CleanupScanner *scanner) {
    Map copy_meta_map;
    {
        std::unique_lock lock(rw_locker_);
        copy_meta_map = meta_map_;
    }

    for (auto iter = copy_meta_map.begin(); iter != copy_meta_map.end();) {
        SharedPtr<Meta> &meta = iter->second;
        bool all_delete = meta->PickCleanup(scanner);
        // if (all_delete) {
        //     LOG_INFO(fmt::format("PickCleanup: all_delete: {}", iter->first));
        //     scanner->AddMeta(std::move(meta));
        //     iter = copy_meta_map.erase(iter);
        // } else {
        //     ++iter;
        // }
        ++iter;
    }
}

template <MetaConcept Meta>
void MetaMap<Meta>::Cleanup() {
    for (auto &[name, meta] : meta_map_) {
        meta->SetCleanuped();
        meta->Cleanup();
    }
}

} // namespace infinity
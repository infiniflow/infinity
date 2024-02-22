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

import stl;
import base_meta;

export module meta_map;

namespace infinity {

export template <MetaConcept Meta>
class MetaMap {
public:
    using InnerEntry = typename Meta::EntryT;

public:
    void Cleanup();

    void CleanupDelete(TxnTimeStamp oldest_txn_ts);

public: // TODO: make both private
    mutable std::shared_mutex rw_locker_{};

    HashMap<String, UniquePtr<Meta>> meta_map_;
};

template <MetaConcept Meta>
void MetaMap<Meta>::Cleanup() {
    //
}

template <MetaConcept Meta>
void MetaMap<Meta>::CleanupDelete(TxnTimeStamp oldest_txn_ts) {
    //
}

// template <MetaConcept Meta>
// void BaseMetaEntry<Meta>::CleanupDelete(TxnTimeStamp oldest_txn_ts) {
//     // Vector<SharedPtr<InnerEntry>> cleanup_entries;
//     // Vector<UniquePtr<Meta>> cleanup_metas;
//     // Vector<Meta *> other_metas;
//     // {
//     //     std::unique_lock wlock(rw_locker_);
//     //     for (auto iter = meta_map_.begin(); iter != meta_map_.end();) {
//     //         auto &[name, meta] = *iter;
//     //         auto [deleted_entries, all_deleted] = meta->PickCleanup(oldest_txn_ts);
//     //         if (all_deleted) {
//     //             cleanup_metas.push_back(std::move(meta));
//     //             iter = meta_map_.erase(iter);
//     //         } else {
//     //             other_metas.push_back(meta.get());
//     //             ++iter;
//     //         }
//     //         cleanup_entries.insert(cleanup_entries.end(), deleted_entries.begin(), deleted_entries.end());
//     //     }
//     // }
//     // for (auto &inner_entry : cleanup_entries) {
//     //     inner_entry->Cleanup();
//     // }
//     // for (auto &meta : cleanup_metas) {
//     //     meta->CleanupMeta();
//     // }
//     // for (auto &meta : other_metas) {
//     //     meta->CleanupDelete(oldest_txn_ts);
//     // }
// }

// template <MetaConcept Meta>
// void BaseMetaEntry<Meta>::Cleanup() {
//     for (auto &[name, meta] : meta_map_) {
//         meta->Cleanup();
//     }
// }

} // namespace infinity
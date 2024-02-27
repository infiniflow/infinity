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

namespace infinity {

export template <MetaConcept Meta>
class MetaMap {
public:
    void PickCleanup(CleanupScanner *scanner);

    void Cleanup() &&;

public:                                     // TODO: make both private
    mutable std::shared_mutex rw_locker_{}; // FIX

    HashMap<String, UniquePtr<Meta>> meta_map_;
};

template <MetaConcept Meta>
void MetaMap<Meta>::PickCleanup(CleanupScanner *scanner) {
    std::unique_lock lock(rw_locker_);

    for (auto iter = meta_map_.begin(); iter != meta_map_.end();) {
        UniquePtr<Meta> &meta = iter->second;
        lock.unlock();
        bool all_delete = meta->PickCleanup(scanner);
        lock.lock();
        if (all_delete) {
            // scanner->AddMeta(std::move(meta));
            // iter = meta_map_.erase(iter);
            ++iter;
        } else {
            ++iter;
        }
    }
}

template <MetaConcept Meta>
void MetaMap<Meta>::Cleanup() && {
    for (auto &[name, meta] : meta_map_) {
        std::move(*meta).Cleanup();
    }
}

} // namespace infinity
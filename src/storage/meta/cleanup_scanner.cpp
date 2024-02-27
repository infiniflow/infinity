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

#include <vector>

module cleanup_scanner;

import stl;
import catalog;
import base_entry;

namespace infinity {

CleanupScanner::CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts) : catalog_(catalog), visible_ts_(visible_ts) {}

void CleanupScanner::AddMeta(UniquePtr<MetaInterface> meta) { metas_.emplace_back(std::move(meta)); }

void CleanupScanner::AddEntry(SharedPtr<EntryInterface> entry) { entries_.emplace_back(std::move(entry)); }

void CleanupScanner::Scan() { catalog_->PickCleanup(this); }

void CleanupScanner::Cleanup() && {
    for (auto &entry : entries_) {
        std::move(*entry).Cleanup();
    }
    for (auto &meta : metas_) {
        std::move(*meta).Cleanup();
    }
}

} // namespace infinity

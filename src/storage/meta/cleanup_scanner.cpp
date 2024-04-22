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
import local_file_system;
import infinity_exception;
import status;
import logger;
import third_party;

namespace infinity {

CleanupScanner::CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr) : catalog_(catalog), visible_ts_(visible_ts), buffer_mgr_(buffer_mgr) {}

void CleanupScanner::AddEntry(SharedPtr<EntryInterface> entry) { entries_.emplace_back(std::move(entry)); }

void CleanupScanner::Scan() { catalog_->PickCleanup(this); }

void CleanupScanner::Cleanup() && {
    for (auto &entry : entries_) {
        std::move(*entry).Cleanup();
    }
    buffer_mgr_->RemoveClean();
}

void CleanupScanner::CleanupDir(const String &dir) {
    LocalFileSystem fs;
    try {
        fs.DeleteDirectory(dir);
    } catch (const RecoverableException &e) {
        if (e.ErrorCode() == ErrorCode::kDirNotFound) {
            // this happen when delta checkpoint records "drop table/db/...", and cleanup is called.
            // then restart the system, table entry will be created but no directory will be found
            LOG_INFO(fmt::format("Cleanup: Dir {} not found. Skip", dir));
        } else {
            throw;
        }
    }
}

} // namespace infinity

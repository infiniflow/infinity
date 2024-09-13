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
import infinity_context;

namespace infinity {

String CleanupInfoTracer::GetCleanupInfo() const {
    String info;
    info += fmt::format("Cleanup ts: {}\n", cleanup_ts_);
    for (const auto &path : cleanup_info_) {
        info += fmt::format("Cleanup: {}\n", path);
    }
    return info;
}

CleanupScanner::CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr)
    : catalog_(catalog), visible_ts_(visible_ts), buffer_mgr_(buffer_mgr) {}

void CleanupScanner::AddEntry(SharedPtr<EntryInterface> entry) { entries_.emplace_back(std::move(entry)); }

void CleanupScanner::Scan() {
    LOG_DEBUG(fmt::format("CleanupScanner: Start scanning, ts: {}", visible_ts_));
    catalog_->PickCleanup(this);
}

void CleanupScanner::Cleanup(CleanupInfoTracer *info_tracer) && {
    for (auto &entry : entries_) {
        std::move(*entry).Cleanup(info_tracer);
    }
    buffer_mgr_->RemoveClean();
}

void CleanupScanner::CleanupDir(const String &abs_dir) {
    LocalFileSystem fs;
    try {
        fs.DeleteDirectory(abs_dir);
    } catch (const RecoverableException &e) {
        if (e.ErrorCode() == ErrorCode::kDirNotFound) {
            // this happens when delta checkpoint records "drop table/db/...", and cleanup is called.
            // then restart the system, table entry will be created but no directory will be found
            LOG_WARN(fmt::format("Cleanup: Dir {} not found. Skip", abs_dir));
        } else {
            String error_message = fmt::format("Cleanup {} encounter unexpected error: {}", abs_dir, e.what());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

} // namespace infinity

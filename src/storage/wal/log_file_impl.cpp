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

module infinity_core:log_file.impl;

import :log_file;
import :stl;
import :virtual_store;
import third_party;
import :infinity_exception;
import :logger;
import :default_values;
import :infinity_context;
import :status;
import admin_statement;

namespace infinity {

Pair<Optional<TempWalFileInfo>, Vector<WalFileInfo>> WalFile::ParseWalFilenames(const String &wal_dir) {

    if (!VirtualStore::Exists(wal_dir)) {
        return {None, Vector<WalFileInfo>{}};
    }

    auto [entries, status] = VirtualStore::ListDirectory(wal_dir);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    if (entries.empty()) {
        return {None, Vector<WalFileInfo>{}};
    }
    Optional<TempWalFileInfo> cur_wal_info;
    Vector<WalFileInfo> wal_infos;
    for (const auto &entry : entries) {
        if (!entry->is_regular_file()) {
            LOG_WARN(fmt::format("Wal file {} is not a regular file", entry->path().string()));
            continue;
        }
        const auto &filename = entry->path().filename().string();
        if (IsEqual(filename, WalFile::TempWalFilename())) {
            if (cur_wal_info.has_value()) {
                String error_message = fmt::format("Multiple current wal files found in the wal directory: {}", wal_dir);
                UnrecoverableError(error_message);
            }
            cur_wal_info = TempWalFileInfo{entry->path().string()};
        } else {
            auto dot_pos = filename.rfind('.');
            if (dot_pos == String::npos) {
                LOG_WARN(fmt::format("Wal file {} has wrong file name", entry->path().string()));
                continue;
            }
            TxnTimeStamp checkpoint_ts;
            try {
                checkpoint_ts = std::stoll(filename.substr(dot_pos + 1));
            } catch (...) {
                LOG_WARN(fmt::format("Wal file {} has wrong file name", entry->path().string()));
                continue;
            }
            auto file_prefix = filename.substr(0, dot_pos);
            if (!IsEqual(file_prefix, String(WAL_FILE_PREFIX))) {
                LOG_WARN(fmt::format("Wal file {} has wrong file name", entry->path().string()));
                continue;
            }
            wal_infos.push_back({entry->path().string(), checkpoint_ts});
        }
    }
    if (!cur_wal_info.has_value()) {
        // this happens when temp wal file is swapped and new wal is not created yet
        LOG_INFO(fmt::format("Current wal file not found in the wal directory: {}", wal_dir));
    }
    return {cur_wal_info, wal_infos};
}

String WalFile::WalFilename(TxnTimeStamp max_commit_ts) { return fmt::format("{}.{}", String(WAL_FILE_PREFIX), max_commit_ts); }

String WalFile::TempWalFilename() { return String(WAL_FILE_TEMP_FILE); }

// /**
//  * @brief Gc the old wal files.
//  * Only delete the wal.log.* files. the wal.log file is current wal file.
//  * Check if the wal.log.* files are too old.
//  * if * is less than the max_commit_ts, we will delete it.
//  */
void WalFile::RecycleWalFile(TxnTimeStamp max_commit_ts, const String &wal_dir) {
    auto [cur_wal_info, wal_infos] = ParseWalFilenames(wal_dir);
    for (const auto &wal_info : wal_infos) {
        if (wal_info.max_commit_ts_ <= max_commit_ts) {
            LOG_INFO(fmt::format("WalManager::Checkpoint delete wal file: {}", wal_info.path_));
            VirtualStore::DeleteFile(wal_info.path_);
        }
    }
}

} // namespace infinity
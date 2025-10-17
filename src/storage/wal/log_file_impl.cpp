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

module infinity_core:log_file.impl;

import :log_file;
import :virtual_store;
import :infinity_exception;
import :logger;
import :default_values;
import :infinity_context;
import :status;

import std;
import third_party;

import admin_statement;

namespace infinity {

std::pair<std::optional<TempWalFileInfo>, std::vector<WalFileInfo>> WalFile::ParseWalFilenames(const std::string &wal_dir) {

    if (!VirtualStore::Exists(wal_dir)) {
        return {std::nullopt, std::vector<WalFileInfo>{}};
    }

    auto [entries, status] = VirtualStore::ListDirectory(wal_dir);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    if (entries.empty()) {
        return {std::nullopt, std::vector<WalFileInfo>{}};
    }
    std::optional<TempWalFileInfo> cur_wal_info;
    std::vector<WalFileInfo> wal_infos;
    for (const auto &entry : entries) {
        if (!entry->is_regular_file()) {
            LOG_WARN(fmt::format("Wal file {} is not a regular file", entry->path().string()));
            continue;
        }
        const auto &filename = entry->path().filename().string();
        if (filename == WalFile::TempWalFilename()) {
            if (cur_wal_info.has_value()) {
                UnrecoverableError(fmt::format("Multiple current wal files found in the wal directory: {}", wal_dir));
            }
            cur_wal_info = TempWalFileInfo{entry->path().string()};
        } else {
            auto dot_pos = filename.rfind('.');
            if (dot_pos == std::string::npos) {
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
            if (file_prefix != std::string(WAL_FILE_PREFIX)) {
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

std::string WalFile::WalFilename(TxnTimeStamp max_commit_ts) { return fmt::format("{}.{}", std::string(WAL_FILE_PREFIX), max_commit_ts); }

std::string WalFile::TempWalFilename() { return std::string(WAL_FILE_TEMP_FILE); }

} // namespace infinity
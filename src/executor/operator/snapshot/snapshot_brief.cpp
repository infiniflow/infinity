// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <chrono>
#include <filesystem>
#include <ctime>
#include <sys/stat.h>

module snapshot_brief;

import stl;
import status;
import virtual_store;
import local_file_handle;
import infinity_exception;
import third_party;
import logger;

namespace infinity {

Vector<String> find_files_with_suffix(const String &path, const String &suffix) {
    Vector<String> files;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            // Don't search the directory recursively
        } else {
            if (entry.path().extension() == suffix) {
                files.emplace_back(entry.path().string());
            }
        }
    }
    return files;
}


// TODO: not atomic
Vector<SnapshotBrief> SnapshotBrief::GetSnapshots(const String &dir) {
    Vector<SnapshotBrief> briefs;

    // Debug: Check if directory exists and log the path
    LOG_INFO(fmt::format("Scanning snapshot directory: {}", dir));
    
    if (!VirtualStore::Exists(dir)) {
        LOG_ERROR(fmt::format("Snapshot directory does not exist: {}", dir));
        return briefs;
    }
    
    // Scan for snapshot directories instead of JSON files directly
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_directory()) {
            continue; // Skip non-directory entries
        }
        
        String dir_name = entry.path().filename().string();
        
        // Skip deleted directories (directories starting with "deleted_")
        if (dir_name.starts_with("deleted_")) {
            continue; // Skip deleted snapshots
        }
        
        // Look for JSON file inside the snapshot directory
        String snapshot_dir_path = entry.path().string();
        String expected_json_path = fmt::format("{}/{}.json", snapshot_dir_path, dir_name);
        
        if (!VirtualStore::Exists(expected_json_path)) {
            LOG_WARN(fmt::format("Snapshot directory {} does not contain expected JSON file", dir_name));
            continue;
        }
        
        try {
            auto [snapshot_file_handle, status] = VirtualStore::Open(expected_json_path, FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            i64 file_size = snapshot_file_handle->FileSize();
            String json_str(file_size, 0);
            auto [n_bytes, status_read] = snapshot_file_handle->Read(json_str.data(), file_size);
            if (!status_read.ok()) {
                UnrecoverableError(status_read.message());
            }
            if ((SizeT)file_size != n_bytes) {
                UnrecoverableError(status_read.message());
            }

            nlohmann::json snapshot_json = nlohmann::json::parse(json_str);
            SnapshotBrief snapshot_brief;
            snapshot_brief.snapshot_name_ = snapshot_json["snapshot_name"];
            snapshot_brief.scope_ = snapshot_json["snapshot_scope"];


            // Calculate size of the entire snapshot directory
            snapshot_brief.size_ = VirtualStore::GetDirectorySize(snapshot_dir_path);

            struct stat statbuf;
            stat(expected_json_path.c_str(), &statbuf);
            std::time_t creationTime = statbuf.st_ctime;
            std::tm *ptm = std::localtime(&creationTime);
            char time_buffer[80];
            std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", ptm);
            snapshot_brief.create_time_ = time_buffer;
            briefs.emplace_back(snapshot_brief);
        } catch (const std::exception &e) {
            LOG_WARN(fmt::format("Fail to parse snapshot directory: {}", dir_name));
        }
    }
    
    return briefs;
}

} // namespace infinity

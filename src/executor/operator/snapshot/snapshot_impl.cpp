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

#include <filesystem>

module infinity_core:snapshot.impl;

import :snapshot;

import :stl;
import :query_context;
import :status;
import :third_party;
import :config;
import :virtual_store;
import :logger;

namespace infinity {

// Status Snapshot::DropSnapshot(QueryContext *query_context, const String &snapshot_name) {

//     String snapshot_dir = query_context->global_config()->SnapshotDir();
//     bool found = false;
//     for (const auto &entry : std::filesystem::directory_iterator(snapshot_dir)) {
//         if (entry.is_directory()) {
//             // Don't search the directory recursively
//         } else {
//             // Just the file base name
//             if (entry.path().stem() == snapshot_name) {
//                 String extension = entry.path().extension();
//                 if (extension == ".json" or extension == ".lz4") {
//                     LOG_INFO(fmt::format("Delete file: {}", entry.path().string()));
//                     VirtualStore::DeleteFile(entry.path().string());
//                     found = true;
//                 }
//             } else {
//                 String filename = entry.path().filename();
//                 LOG_WARN(fmt::format("Invalid snapshot file name: {}", filename));
//             }
//         }
//     }

//     if (!found) {
//         return Status::NotFound(fmt::format("Snapshot: {} not found", snapshot_name));
//     }

//     return Status::OK();
// }

Status Snapshot::DropSnapshot(QueryContext *query_context, const String &snapshot_name) {
    String snapshot_dir = query_context->global_config()->SnapshotDir();
    String snapshot_path = fmt::format("{}/{}", snapshot_dir, snapshot_name);

    // Check if snapshot directory exists
    if (!VirtualStore::Exists(snapshot_path)) {
        return Status::NotFound(fmt::format("Snapshot: {} not found", snapshot_name));
    }

    // ATOMIC RENAME - Move entire directory to deleted location
    String deleted_path = fmt::format("{}/deleted_{}", snapshot_dir, snapshot_name);
    if (VirtualStore::Exists(deleted_path)) {
        LOG_WARN(fmt::format("Snapshot: {} already deleted", snapshot_name));
        VirtualStore::RemoveDirectory(deleted_path);
    }
    Status rename_status = VirtualStore::Rename(snapshot_path, deleted_path);

    if (!rename_status.ok()) {
        return Status::IOError(fmt::format("Failed to delete snapshot: {}", rename_status.message()));
    }

    LOG_INFO(fmt::format("Atomically moved snapshot directory: {} -> {}", snapshot_path, deleted_path));

    // Remove the entire deleted snapshot directory and all its contents
    LOG_INFO(fmt::format("Removing snapshot directory: {}", deleted_path));
    VirtualStore::RemoveDirectory(deleted_path);

    return Status::OK();
}

} // namespace infinity

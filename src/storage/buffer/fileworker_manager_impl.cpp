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

module infinity_core:fileworker_manager.impl;

import :fileworker_manager;
import :file_worker;
import :logger;
import :infinity_exception;
import :file_worker_type;
import :var_file_worker;
import :persistence_manager;
import :virtual_store;
import :kv_store;
import :status;
import :infinity_context;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

FileWorkerManager::FileWorkerManager(std::shared_ptr<std::string> data_dir, std::shared_ptr<std::string> temp_dir)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)) {
    persistence_manager_ = InfinityContext::instance().storage()->persistence_manager();
}

void FileWorkerManager::Start() {
    if (!VirtualStore::Exists(*data_dir_)) {
        VirtualStore::MakeDirectory(*data_dir_);
    }
}

void FileWorkerManager::Stop() {
    RemoveClean(nullptr);
    LOG_INFO("FileWorker manager is stopped.");
}

FileWorker *FileWorkerManager::EmplaceFileWorker(std::unique_ptr<FileWorker> file_worker) {
    std::string file_path = file_worker->GetFilePath();
    std::unique_lock lock(w_locker_);
    if (auto iter = fileworker_map_.find(file_path); iter != fileworker_map_.end()) {
        return iter->second.get();
    }
    auto [iter, _] = fileworker_map_.emplace(file_path, std::move(file_worker));
    return iter->second.get();
}

FileWorker *FileWorkerManager::EmplaceFileWorkerTemp(std::unique_ptr<FileWorker> file_worker) {
    std::string file_path = file_worker->GetFilePathTmp();
    std::unique_lock lock(w_locker_);
    if (auto iter = fileworker_map_.find(file_path); iter != fileworker_map_.end()) {
        return iter->second.get();
    }
    auto [iter, _] = fileworker_map_.emplace(file_path, std::move(file_worker));
    return iter->second.get();
}

FileWorker *FileWorkerManager::GetFileWorker(const std::string &file_path) {
    std::unique_lock lock(w_locker_);
    if (auto iter = fileworker_map_.find(file_path); iter != fileworker_map_.end()) {
        return iter->second.get();
    }
    LOG_TRACE(fmt::format("FileWorkerManager::GetFileWorker: file {} not found.", file_path));
    return nullptr;
}

size_t FileWorkerManager::BufferedObjectCount() {
    std::unique_lock lock(w_locker_);
    return fileworker_map_.size();
}

Status FileWorkerManager::RemoveClean(KVInstance *kv_instance) {
    LOG_TRACE(fmt::format("FileWorkerManager::RemoveClean, start to clean objects"));
    Status status;
    std::vector<FileWorker *> clean_list;
    {
        std::unique_lock lock(clean_locker_);
        clean_list.swap(clean_list_);
    }
    for (auto *buffer_obj : clean_list) {
        status = buffer_obj->CleanupFile();
        if (!status.ok()) {
            return status;
        }
    }
    std::unordered_set<FileWorker *> clean_temp_set;
    {
        std::unique_lock lock(temp_locker_);
        clean_temp_set.swap(clean_temp_set_);
    }
    for (auto *buffer_obj : clean_temp_set) {
        buffer_obj->CleanupTempFile(); // cleanup_temp status?
    }

    {
        std::unique_lock lock(w_locker_);
        for (auto *fileworker : clean_list) {
            auto file_path = *(fileworker->file_path_);
            [[maybe_unused]] size_t remove_n = fileworker_map_.erase(file_path);
            // if (remove_n != 1) {
            //     UnrecoverableError(fmt::format("FileWorkerManager::RemoveClean: file {} not found.", file_path.c_str()));
            // }
        }
        fileworker_map_.rehash(fileworker_map_.size());
    }
    return Status::OK();
}

void FileWorkerManager::AddToCleanList(FileWorker *fileworker) {
    std::unique_lock lock(clean_locker_);
    clean_list_.emplace_back(fileworker);
}

void FileWorkerManager::RemoveBufferObjects(const std::vector<std::string> &object_paths) {
    std::unique_lock lock(w_locker_);
    // size_t erase_object = 0;
    for (auto &object_path : object_paths) {
        // erase_object = buffer_map_.erase(object_path);
        fileworker_map_.erase(object_path);
        // if (erase_object != 1) {
        //     UnrecoverableError(fmt::format("FileWorkerManager::RemoveBufferObjects: object {} not found.", object_path));
        // }
    }
}

} // namespace infinity
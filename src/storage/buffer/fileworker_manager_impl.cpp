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

#include <sys/mman.h>

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
    RemoveCleanList(nullptr);
    LOG_INFO("FileWorker manager is stopped.");
}

FileWorker *FileWorkerManager::EmplaceFileWorker(std::unique_ptr<FileWorker> file_worker) {
    std::unique_lock lock(w_locker_);
    auto rel_file_path = file_worker->rel_file_path_;
    if (auto iter = fileworker_map_.find(*rel_file_path); iter != fileworker_map_.end()) {
        return iter->second.get();
    }
    auto [iter, _] = fileworker_map_.emplace(*rel_file_path, std::move(file_worker));
    return iter->second.get();
}

FileWorker *FileWorkerManager::GetFileWorker(const std::string &rel_file_path) {
    std::unique_lock lock(w_locker_);
    if (auto iter = fileworker_map_.find(rel_file_path); iter != fileworker_map_.end()) {
        return iter->second.get();
    }
    LOG_TRACE(fmt::format("FileWorkerManager::GetFileWorker: file {} not found.", rel_file_path));
    return nullptr;
}

size_t FileWorkerManager::BufferedObjectCount() {
    std::unique_lock lock(w_locker_);
    return fileworker_map_.size();
}

Status FileWorkerManager::RemoveCleanList(KVInstance *kv_instance) {
    LOG_TRACE(fmt::format("FileWorkerManager::RemoveClean, start to clean objects"));
    std::vector<FileWorker *> clean_list;
    {
        std::unique_lock lock(clean_locker_);
        clean_list.swap(clean_list_);
    }
    for (auto *file_worker : clean_list) {
        Status status = file_worker->CleanupFile();
        if (!status.ok()) {
            return status;
        }
    }
    {
        std::unique_lock lock(w_locker_);
        for (auto *file_worker : clean_list) {
            auto fileworker_key = *(file_worker->rel_file_path_);
            // if (fileworker_key.find("version") !=std::string::npos) {
            //     some_map_.erase(fileworker_key);
            // }
            [[maybe_unused]] size_t remove_n = fileworker_map_.erase(fileworker_key);
            // if (remove_n != 1) {
            //     UnrecoverableError(fmt::format("FileWorkerManager::RemoveClean: file {} not found.", file_path.c_str()));
            // }
        }
        // some_map_.rehash(some_map_.size());
        fileworker_map_.rehash(fileworker_map_.size());
    }
    return Status::OK();
}

void FileWorkerManager::AddToCleanList(FileWorker *file_worker) {
    std::unique_lock lock(clean_locker_);
    clean_list_.emplace_back(file_worker);
}

void FileWorkerManager::RemoveImport(TransactionID txn_id) {
    std::unique_lock lock(w_locker_);
    for (auto it = fileworker_map_.begin(); it != fileworker_map_.end();) {
        auto pat = fmt::format("import{}", txn_id);
        if (it->first.find(pat) != std::string::npos) {
            it = fileworker_map_.erase(it);
        } else {
            ++it;
        }
    }
}

void FileWorkerManager::MoveFiles() {
    std::unique_lock lock(w_locker_);
    for (auto it = fileworker_map_.begin(); it != fileworker_map_.end();) {
        const auto &ptr = it->second;
        if (ptr->rel_file_path_->find("import") != std::string::npos) {
            ++it;
            continue;
        }
        msync(ptr->mmap_, ptr->mmap_size_, MS_SYNC);
        ptr->MoveFile();
        ++it;
    }
}

} // namespace infinity
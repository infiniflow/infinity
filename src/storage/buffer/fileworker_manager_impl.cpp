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

namespace infinity {

template <typename FileWorkerT>
FileWorkerT *FileWorkerMap<FileWorkerT>::EmplaceFileWorker(std::unique_ptr<FileWorkerT> file_worker) {
    std::unique_lock lock(rw_mtx_);
    auto rel_file_path = file_worker->rel_file_path_;
    active_dic_.emplace(*rel_file_path);
    if (auto iter = map_.find(*rel_file_path); iter != map_.end()) {
        return iter->second.get();
    }
    auto [iter, _] = map_.emplace(*rel_file_path, std::move(file_worker));
    return iter->second.get();
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::RemoveImport(TransactionID txn_id) {
    std::unique_lock lock(rw_mtx_);
    for (auto it = map_.begin(); it != map_.end();) {
        auto &[rel_file_path, _] = *it;
        auto pat = fmt::format("import{}", txn_id);
        if (rel_file_path.find(pat) != std::string::npos) {
            active_dic_.erase(rel_file_path);
            it = map_.erase(it);
        } else {
            ++it;
        }
    }
}

template <typename FileWorkerT>
FileWorkerT *FileWorkerMap<FileWorkerT>::GetFileWorker(const std::string &rel_file_path) {
    {
        std::shared_lock lock(rw_mtx_);
        if (auto iter = map_.find(rel_file_path); iter != map_.end()) {
            return iter->second.get();
        }
    }
    LOG_TRACE(fmt::format("FileWorkerManager::GetFileWorker: file {} not found.", rel_file_path));
    return nullptr;
}

template <typename FileWorkerT>
FileWorkerT *FileWorkerMap<FileWorkerT>::GetFileWorkerNoLock(const std::string &rel_file_path) {
    if (auto iter = map_.find(rel_file_path); iter != map_.end()) {
        return iter->second.get();
    }
    LOG_TRACE(fmt::format("FileWorkerManager::GetFileWorker: file {} not found.", rel_file_path));
    return nullptr;
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::ClearCleans() {
    decltype(cleans_) cleans;
    {
        std::unique_lock l(rw_clean_mtx_);
        cleans_.swap(cleans);
    }

    std::vector<std::future<Status>> futs;
    futs.reserve(cleans.size());
    for (auto *file_worker : cleans) {
        futs.emplace_back(std::async(&FileWorkerT::template CleanupFile<FileWorkerT>, file_worker));
    }
    for (auto &fut : futs) {
        auto status = fut.get();
        if (!status.ok()) {
            // return status;
        }
    }

    {
        std::unique_lock lock(rw_mtx_);
        for (auto *file_worker : cleans) {
            auto fileworker_key = *file_worker->rel_file_path_;
            map_.erase(fileworker_key);
            active_dic_.erase(fileworker_key);
        }
        map_.rehash(map_.size());
    }
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::MoveToCleans(FileWorkerT *file_worker) {
    std::unique_lock lock(rw_clean_mtx_);
    cleans_.emplace_back(file_worker);
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::MoveFiles() {
    // std::shared_lock lock(rw_temp_mtx_);
    std::unique_lock l(rw_mtx_);
    std::vector<std::future<void>> futs;
    futs.reserve(map_.size());
    for (auto it = active_dic_.begin(); it != active_dic_.end();) {
        auto &rel_file_path = *it;
        if (rel_file_path.find("import") != std::string::npos) {
            ++it;
            continue;
        }
        auto file_worker = GetFileWorkerNoLock(rel_file_path);
        // assert(file_worker);
        futs.emplace_back(std::async(&FileWorkerT::template MoveFile<FileWorkerT>, file_worker));
        ++it;
    }

    active_dic_.clear();
    for (auto &fut : futs) {
        fut.wait();
    }
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::MoveFilesByPaths(const std::vector<std::string> &rel_file_paths) {
    std::unique_lock l(rw_mtx_);

    std::vector<std::future<void>> futs;
    std::vector<FileWorkerT *> file_workers;
    file_workers.reserve(rel_file_paths.size());

    for (const auto &rel_file_path : rel_file_paths) {
        auto it = active_dic_.find(rel_file_path);
        if (it == active_dic_.end()) {
            LOG_WARN(fmt::format("FileWorker {} not in active_dic", rel_file_path));
            continue;
        }

        auto file_worker = GetFileWorkerNoLock(rel_file_path);
        if (file_worker == nullptr) {
            LOG_WARN(fmt::format("FileWorker {} is nullptr", rel_file_path));
            active_dic_.erase(it);
            continue;
        }

        file_workers.push_back(file_worker);
    }

    for (const auto &rel_file_path : rel_file_paths) {
        active_dic_.erase(rel_file_path);
    }

    futs.reserve(file_workers.size());
    for (auto file_worker : file_workers) {
        futs.emplace_back(std::async(&FileWorkerT::template MoveFile<FileWorkerT>, file_worker));
    }

    for (auto &fut : futs) {
        fut.wait();
    }
}

template struct FileWorkerMap<BMPIndexFileWorker>;
template struct FileWorkerMap<DataFileWorker>;
template struct FileWorkerMap<EMVBIndexFileWorker>;
template struct FileWorkerMap<HnswFileWorker>;
template struct FileWorkerMap<IVFIndexFileWorker>;
template struct FileWorkerMap<RawFileWorker>;
template struct FileWorkerMap<SecondaryIndexFileWorker>;
template struct FileWorkerMap<VarFileWorker>;
template struct FileWorkerMap<VersionFileWorker>;

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
    RemoveCleanList();
    LOG_INFO("FileWorker manager is stopped.");
}

// Get size in concurrency environment is meaningless
size_t FileWorkerManager::FileWorkerCount() {
    std::unique_lock lock(mutex_);
    return 0;
}

Status FileWorkerManager::RemoveCleanList() {
    std::lock_guard l(mutex_); // Because of async problem
    std::vector<std::future<void>> futs;

    futs.emplace_back(std::async(&FileWorkerMap<BMPIndexFileWorker>::ClearCleans, &bmp_map_));
    futs.emplace_back(std::async(&FileWorkerMap<DataFileWorker>::ClearCleans, &data_map_));
    futs.emplace_back(std::async(&FileWorkerMap<EMVBIndexFileWorker>::ClearCleans, &emvb_map_));
    futs.emplace_back(std::async(&FileWorkerMap<HnswFileWorker>::ClearCleans, &hnsw_map_));
    futs.emplace_back(std::async(&FileWorkerMap<RawFileWorker>::ClearCleans, &raw_map_));
    futs.emplace_back(std::async(&FileWorkerMap<SecondaryIndexFileWorker>::ClearCleans, &secondary_map_));
    futs.emplace_back(std::async(&FileWorkerMap<VarFileWorker>::ClearCleans, &var_map_));
    futs.emplace_back(std::async(&FileWorkerMap<VersionFileWorker>::ClearCleans, &version_map_));

    for (auto &fut : futs) {
        fut.wait();
    }

    return Status::OK();
}

void FileWorkerManager::RemoveImport(TransactionID txn_id) {
    std::lock_guard l(mutex_); // Because of async problem
    std::vector<std::future<void>> futs;

    futs.emplace_back(std::async(&FileWorkerMap<BMPIndexFileWorker>::RemoveImport, &bmp_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<DataFileWorker>::RemoveImport, &data_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<EMVBIndexFileWorker>::RemoveImport, &emvb_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<HnswFileWorker>::RemoveImport, &hnsw_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<RawFileWorker>::RemoveImport, &raw_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<SecondaryIndexFileWorker>::RemoveImport, &secondary_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<VarFileWorker>::RemoveImport, &var_map_, txn_id));
    futs.emplace_back(std::async(&FileWorkerMap<VersionFileWorker>::RemoveImport, &version_map_, txn_id));

    for (auto &fut : futs) {
        fut.wait();
    }
}

void FileWorkerManager::MoveFiles() {
    std::lock_guard l(mutex_); // Because of async problem
    std::vector<std::future<void>> futs;

    futs.emplace_back(std::async(&FileWorkerMap<BMPIndexFileWorker>::MoveFiles, &bmp_map_));
    futs.emplace_back(std::async(&FileWorkerMap<DataFileWorker>::MoveFiles, &data_map_));
    futs.emplace_back(std::async(&FileWorkerMap<EMVBIndexFileWorker>::MoveFiles, &emvb_map_));
    futs.emplace_back(std::async(&FileWorkerMap<HnswFileWorker>::MoveFiles, &hnsw_map_));
    futs.emplace_back(std::async(&FileWorkerMap<RawFileWorker>::MoveFiles, &raw_map_));
    futs.emplace_back(std::async(&FileWorkerMap<SecondaryIndexFileWorker>::MoveFiles, &secondary_map_));
    futs.emplace_back(std::async(&FileWorkerMap<VarFileWorker>::MoveFiles, &var_map_));
    futs.emplace_back(std::async(&FileWorkerMap<VersionFileWorker>::MoveFiles, &version_map_));

    for (auto &fut : futs) {
        fut.wait();
    }
}

void FileWorkerManager::MoveFiles(const std::vector<std::string> &rel_file_paths) {
    std::lock_guard l(mutex_);
    std::vector<std::future<void>> futs;

    futs.emplace_back(std::async(&FileWorkerMap<BMPIndexFileWorker>::MoveFilesByPaths, &bmp_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<DataFileWorker>::MoveFilesByPaths, &data_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<EMVBIndexFileWorker>::MoveFilesByPaths, &emvb_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<HnswFileWorker>::MoveFilesByPaths, &hnsw_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<RawFileWorker>::MoveFilesByPaths, &raw_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<SecondaryIndexFileWorker>::MoveFilesByPaths, &secondary_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<VarFileWorker>::MoveFilesByPaths, &var_map_, rel_file_paths));
    futs.emplace_back(std::async(&FileWorkerMap<VersionFileWorker>::MoveFilesByPaths, &version_map_, rel_file_paths));

    for (auto &fut : futs) {
        fut.wait();
    }
}

} // namespace infinity
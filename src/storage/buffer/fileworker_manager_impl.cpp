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
        auto pat = fmt::format("import{}", txn_id);
        if (it->first.find(pat) != std::string::npos) {
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
void FileWorkerMap<FileWorkerT>::ClearCleans() {
    decltype(cleans_) cleans;
    {
        std::unique_lock l(rw_clean_mtx_);
        cleans_.swap(cleans);
    }
    std::vector<std::future<Status>> futs;
    futs.reserve(cleans.size());
    for (auto *file_worker : cleans) {
        futs.emplace_back(std::async(&FileWorkerT::CleanupFile, file_worker));
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
            auto fileworker_key = *(file_worker->rel_file_path_);
            [[maybe_unused]] size_t remove_n = map_.erase(fileworker_key);
            // if (remove_n != 1) {
            //     UnrecoverableError(fmt::format("FileWorkerManager::RemoveClean: file {} not found.", file_path.c_str()));
            // }
        }
        map_.rehash(map_.size());
    }
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::AddToCleanList(FileWorkerT *file_worker) {
    std::unique_lock lock(rw_clean_mtx_);
    cleans_.emplace_back(file_worker);
}

template <typename FileWorkerT>
void FileWorkerMap<FileWorkerT>::MoveFiles() {
    std::shared_lock lock(rw_mtx_);
    std::vector<std::future<void>> futs;
    // std::vector<std::future<int>> futs1;
    futs.reserve(map_.size());
    for (auto it = map_.begin(); it != map_.end();) {
        const auto &ptr = it->second;
        if (ptr->rel_file_path_->find("import") != std::string::npos) {
            ++it;
            continue;
        }
        // futs1.emplace_back(std::async(msync, ptr->mmap_, ptr->mmap_size_, MS_SYNC));
        msync(ptr->mmap_, ptr->mmap_size_, MS_SYNC);
        futs.emplace_back(std::async(&FileWorkerT::MoveFile, ptr.get()));
        ++it;
    }
    for (auto &fut : futs) {
        fut.wait();
    }
    // for (auto &fut : futs1) {
    //     fut.wait();
    // }
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
    // std::unique_lock lock(w_locker_);
    return 0;
}

Status FileWorkerManager::RemoveCleanList() {
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

} // namespace infinity
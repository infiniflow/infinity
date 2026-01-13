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
#include <unistd.h>

module infinity_core:hnsw_file_worker.impl;

import :hnsw_file_worker;
import :infinity_exception;
import :index_file_worker;
import :hnsw_alg;
import :hnsw_common;
import :index_hnsw;
import :index_base;
import :hnsw_handler;
import :virtual_store;
import :persistence_manager;
import :local_file_handle;
import :fileworker_manager;

import std;
import third_party;

import column_def;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;

namespace infinity {
HnswFileWorker::HnswFileWorker(std::shared_ptr<std::string> file_path,
                               std::shared_ptr<IndexBase> index_base,
                               std::shared_ptr<ColumnDef> column_def,
                               size_t index_size)
    : IndexFileWorker(std::move(file_path), std::move(index_base), std::move(column_def)) {
    // if (index_size == 0) {
    //
    //     std::string index_path = GetFilePath();
    //     auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kReadWrite);
    //     if (status.ok()) {
    //         // When replay by checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
    //         index_size = file_handle->FileSize();
    //     }
    // }
    // index_size_ = index_size;
}

HnswFileWorker::~HnswFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

// bool HnswFileWorker::Write(std::shared_ptr<HnswHandler> &data,
//                            std::unique_ptr<LocalFileHandle> &file_handle,
//                            bool &prepare_success,
//                            const FileWorkerSaveCtx &ctx) {
//     std::unique_lock l(mutex_);
//
//     auto fd = file_handle->fd();
//     mmap_size_ = data->CalcSize();
//     ftruncate(fd, mmap_size_);
//
//     mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
//
//     size_t offset{};
//     data->SaveToPtr(mmap_, offset);
//
//     auto &path = *rel_file_path_;
//     auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->hnsw_map_.cache_manager_;
//     cache_manager.Set(path, data, data->MemUsage());
//     prepare_success = true;
//     return true;
// }

void HnswFileWorker::Read(HnswHandler *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    std::unique_lock l(mutex_);
    auto &path = *rel_file_path_;
    auto tmp_path = GetFilePathTemp();
    if (!inited_) {
        if (!VirtualStore::Exists("/var/infinity/tmp")) {
            VirtualStore::MakeDirectory("/var/infinity/tmp");
        }

        if (!VirtualStore::Exists(tmp_path.c_str())) {
            auto [handle, status] = VirtualStore::Open(tmp_path, FileAccessMode::kReadWrite);
            close(handle->fd());
            VirtualStore::DeleteFile(tmp_path.c_str());
        }
        segment_ = boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create_infinity, tmp_path.c_str(), 1145141);
        auto *sm = segment_.get_segment_manager();
        data = segment_.find_or_construct<HnswHandler>(path.c_str())(index_base_.get(), column_def_, sm);
        inited_ = true;
        return;
    }
    auto result = segment_.find<HnswHandler>(path.c_str());
    data = result.first;

    // std::unique_lock l(mutex_);
    // if (!file_handle) {
    //     data = HnswHandler::Make(index_base_.get(), column_def_).release();
    //     return;
    // }
    // auto &path = *rel_file_path_;
    // auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->hnsw_map_.cache_manager_;
    // bool flag = cache_manager.Get(path, data);
    // if (!flag) {
    //     data = HnswHandler::Make(index_base_.get(), column_def_).release();
    //     if (!mmap_) {
    //         mmap_size_ = file_handle->FileSize();
    //         auto fd = file_handle->fd();
    //         mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    //     }
    //     data->LoadFromPtr(mmap_, mmap_size_, file_size);
    //     cache_manager.Set(path, data, data->MemUsage());
    // }
}

} // namespace infinity
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
    if (index_size == 0) {

        std::string index_path = GetFilePath();
        auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kRead);
        if (status.ok()) {
            // When replay by checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
            index_size = file_handle->FileSize();
        }
    }
    index_size_ = index_size;
}

HnswFileWorker::~HnswFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
    delete data_;
}

bool HnswFileWorker::Write(HnswHandlerPtr &data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto hnsw_handler = data;

    if (data_) {
        UnrecoverableError("dsdsdsdsdsdsdsd");
    }

    data_ = data;

    (hnsw_handler)->SaveToPtr(*file_handle);

    auto fd = file_handle->fd();
    mmap_size_ = file_handle->FileSize();
    mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);

    // VirtualStore::MmapFile("/var/infinity/tmp/" + *rel_file_path_, tmp_ptr_, mmap_size_);

    prepare_success = true;
    return true;
}

void HnswFileWorker::Read(HnswHandlerPtr &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    // if (!data_) {
    // data = std::shared_ptr<HnswHandlerPtr>(new HnswHandlerPtr{HnswHandler::Make(index_base_.get(), column_def_).release()}, [](HnswHandlerPtr *ptr)
    // {
    //     delete *ptr;
    //     delete ptr;
    // });
    // }
    {
        std::unique_lock l(mutex_);
        if (data_ == nullptr) {
            data_ = HnswHandlerPtr{HnswHandler::Make(index_base_.get(), column_def_).release()};
            // UnrecoverableError("asdasdasdasda");
        }
    }
    // data = std::make_shared<HnswHandlerPtr>(HnswHandlerPtr{HnswHandler::Make(index_base_.get(), column_def_).release()});
    data = data_;
    // data_ may be nullptr
    if (!file_handle) {
        // data = data_;
        return;
    }
    if (!mmap_) {
        // auto *hnsw_handler = data.get();

        // (*hnsw_handler)->LoadFromPtr(*file_handle, file_size);
        (data)->LoadFromPtr(*file_handle, file_size);

        // u8 *tmp_ptr_{};
        // auto fd = file_handle->fd();
        // mmap_size_ = file_handle->FileSize();
        // mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
        // if (mmap_ == MAP_FAILED) {
        //     std::println("why failed: {}", mmap_size_);
        // }
        // data_ = *hnsw_handler;
        // data = data_;
        // data = std::shared_ptr<HnswHandlerPtr>(reinterpret_cast<HnswHandlerPtr *>(mmap_), [](HnswHandlerPtr *ptr) {});

        auto fd = file_handle->fd();
        mmap_size_ = file_handle->FileSize();
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    }
}

} // namespace infinity
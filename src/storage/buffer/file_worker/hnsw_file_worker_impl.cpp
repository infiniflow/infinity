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

    HnswFileWorker::AllocateInMemory();
}

HnswFileWorker::~HnswFileWorker() {
    HnswFileWorker::FreeInMemory();
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

void HnswFileWorker::AllocateInMemory() { data_ = static_cast<void *>(new HnswHandlerPtr()); }

void HnswFileWorker::FreeInMemory() {
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    delete *hnsw_handler;
    delete hnsw_handler;
    data_ = nullptr;
}

bool HnswFileWorker::Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    (*hnsw_handler)->SaveToPtr(*file_handle_);

    file_handle_->Sync();
    auto fd = file_handle_->fd();
    mmap_size_ = file_handle_->FileSize();
    mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    prepare_success = true;
    return true;
}

void HnswFileWorker::Read(size_t file_size, bool other) {
    if (!mmap_) {
        FreeInMemory();
        data_ = static_cast<void *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_).release()));
        auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);

        (*hnsw_handler)->LoadFromPtr(*file_handle_, file_size);

        // return true;

        auto fd = file_handle_->fd();
        mmap_size_ = file_handle_->FileSize();
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
        // (*hnsw_handler)->LoadFromPtr(*file_handle_, (char *)mmap_, file_size);
    }
    // else {
    //     FreeInMemory();
    //     data_ = static_cast<void *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_, false).release()));
    //     auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    //     (*hnsw_handler)->LoadFromPtr(*file_handle_, static_cast<const char *>(mmap_), mmap_size_);
    // }
}

} // namespace infinity
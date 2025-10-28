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

module infinity_core:bmp_index_file_worker.impl;

import :bmp_index_file_worker;
import :index_bmp;
import :infinity_exception;
import :bmp_util;
import :bmp_alg;
import :bmp_handler;
import :virtual_store;
import :persistence_manager;
import :local_file_handle;

import std.compat;
import third_party;

import column_def;
import internal_types;

namespace infinity {

BMPIndexFileWorker::BMPIndexFileWorker(std::shared_ptr<std::string> file_path,
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

    AllocateInMemory();
}

BMPIndexFileWorker::~BMPIndexFileWorker() {
    // if (data_ != nullptr) {
    //     FreeInMemory();
    //     data_ = nullptr;
    // }
    // if (mmap_data_ != nullptr) {
    //     FreeFromMmapImpl();
    //     mmap_data_ = nullptr;
    // }
    FreeInMemory();

    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

void BMPIndexFileWorker::AllocateInMemory() {
    // if (data_) {
    //     UnrecoverableError("Data is already allocated.");
    // }
    data_ = static_cast<void *>(new BMPHandlerPtr());
}

void BMPIndexFileWorker::FreeInMemory() {
    // if (!data_) {
    //     UnrecoverableError("Data is not allocated.");
    // }
    // auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    // delete *bmp_handler;
    // delete bmp_handler;
    data_ = nullptr;
}

bool BMPIndexFileWorker::Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    (*bmp_handler)->SaveToPtr(*file_handle_);

    file_handle_->Sync();

    // auto fd = file_handle_->fd();
    // mmap_size_ = index_size_;
    // mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);

    prepare_success = true;
    return true;
}

void BMPIndexFileWorker::Read(size_t file_size, bool other) {
    FreeInMemory();
    data_ = static_cast<void *>(new BMPHandlerPtr(BMPHandler::Make(index_base_.get(), column_def_.get()).release()));

    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    (*bmp_handler)->LoadFromPtr(*file_handle_, file_size);

    // auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(mmap_);
    // (*bmp_handler)->LoadFromPtr((char *)mmap_, file_size);
}

} // namespace infinity

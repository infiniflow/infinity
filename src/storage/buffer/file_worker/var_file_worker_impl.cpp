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

module infinity_core:var_file_worker.impl;

import :var_file_worker;
import :infinity_exception;
import :var_buffer;
import :local_file_handle;
import :persistence_manager;
import :status;

import std;
import third_party;

namespace infinity {

VarFileWorker::VarFileWorker(std::shared_ptr<std::string> file_path, size_t buffer_size) : FileWorker(std::move(file_path)) {
    VarFileWorker::AllocateInMemory();
}

VarFileWorker::~VarFileWorker() {
    VarFileWorker::FreeInMemory();
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

void VarFileWorker::AllocateInMemory() {
    auto *buffer = new VarBuffer(this);
    data_ = static_cast<void *>(buffer);
}

void VarFileWorker::FreeInMemory() {
    auto *buffer = static_cast<VarBuffer *>(data_);
    delete buffer;
    data_ = nullptr;

    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool VarFileWorker::Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto old_mmap_size = mmap_size_;
    if (mmap_) {
        munmap(mmap_, mmap_size_);
    }
    if (data_ == nullptr) {
        UnrecoverableError("Data is not allocated.");
    }
    const auto *buffer = static_cast<const VarBuffer *>(data_);
    mmap_size_ = buffer->TotalSize();
    auto buffer_data = std::make_unique<char[]>(mmap_size_);
    char *ptr = buffer_data.get();
    buffer->Write(ptr);

    auto fd = file_handle_->fd();
    ftruncate(fd, mmap_size_);

    mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_ == MAP_FAILED) {
        mmap_ = nullptr;
    } else {
        auto diff = mmap_size_ - old_mmap_size;
        // std::println("mmap_diff: {}", diff);
        std::memcpy((char *)mmap_ + old_mmap_size, ptr + old_mmap_size, diff);
    }
    prepare_success = true;

    return true;
}

void VarFileWorker::Read(size_t file_size, bool other) {
    // std::println("R var");
    if (!mmap_) {
        // if (file_size < buffer_size_) {
        //     UnrecoverableError(fmt::format("File: {} size {} is smaller than buffer size {}.", GetFilePath(), file_size, buffer_size_));
        // } else {
        size_t buffer_size = file_size;
        // }

        auto buffer = std::make_unique<char[]>(buffer_size);

        auto [nbytes, status] = file_handle_->Read(buffer.get(), buffer_size);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        // if (nbytes != buffer_size_) {
        //     UnrecoverableError(fmt::format("Read {} bytes from file failed, only {} bytes read.", buffer_size_, nbytes));
        // }
        FreeInMemory();
        auto *var_buffer = new VarBuffer(this, std::move(buffer), buffer_size);
        data_ = static_cast<void *>(var_buffer);

        auto fd = file_handle_->fd();
        mmap_size_ = buffer_size;
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
        if (mmap_ == MAP_FAILED) {
            std::println("that code: {}", mmap_size_);
            mmap_ = nullptr;
        }
    }
}

} // namespace infinity
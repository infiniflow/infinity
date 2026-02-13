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
import :fileworker_manager;
import :infinity_context;

import std;
import third_party;

namespace infinity {

VarFileWorker::VarFileWorker(std::shared_ptr<std::string> file_path, size_t buffer_size) : FileWorker(std::move(file_path)) {}

VarFileWorker::~VarFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool VarFileWorker::Write(std::span<VarBuffer> data,
                          std::unique_ptr<LocalFileHandle> &file_handle,
                          bool &prepare_success,
                          const FileWorkerSaveCtx &ctx) {
    std::unique_lock l(mutex_);
    const auto *buffer = data.data();
    auto old_mmap_size = mmap_size_;
    mmap_size_ = buffer->TotalSize();
    if (!mmap_size_) {
        VirtualStore::DeleteFile(file_handle->Path());
        return true;
    }
    auto buffer_data = std::make_unique<char[]>(mmap_size_);
    char *ptr = buffer_data.get();
    buffer->Write(ptr);

    auto fd = file_handle->fd();
    ftruncate(fd, mmap_size_);

    if (mmap_ == nullptr) {
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    } else {
        mmap_ = mremap(mmap_, old_mmap_size, mmap_size_, MREMAP_MAYMOVE);
    }
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

bool VarFileWorker::WriteSnapshot(std::span<VarBuffer> data,
                                  std::unique_ptr<LocalFileHandle> &file_handle,
                                  size_t rel_size,
                                  bool &prepare_success,
                                  const FileWorkerSaveCtx &ctx) {
    if (data.data() == nullptr) {
        RecoverableError(Status::SyntaxError("VarFileWorker::WriteSnapshot: Data is empty."));
    }

    const auto *buffer = static_cast<const VarBuffer *>(data.data());
    if (buffer->TotalSize() == 0) {
        return false;
    }

    auto buffer_data = std::make_unique<char[]>(rel_size);
    size_t total_size = buffer->Write(buffer_data.get(), rel_size);
    Status status = file_handle->Append(buffer_data.get(), total_size);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    prepare_success = true;
    return true;
}

void VarFileWorker::Read(std::shared_ptr<VarBuffer> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    auto &path = *rel_file_path_;
    auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->var_map_.cache_manager_;

    // Try to get from cache
    bool flag = cache_manager.Get(path, data);
    if (flag) {
        return;
    }

    // std::unique_lock l(mutex_);
    data = std::make_shared<VarBuffer>(this);

    if (!mmap_) {
        if (!file_handle) {
            return;
        }
        mmap_size_ = file_size;
        if (mmap_size_ == 0) {
            return;
        }
        auto fd = file_handle->fd();
        // mmap_size_ = mmap_size_;
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 /*align_offset*/);
        if (mmap_ == MAP_FAILED) {
            // std::println("that code var: {}", mmap_size_);
            mmap_ = nullptr;
        }
        auto buffer = std::make_unique_for_overwrite<char[]>(mmap_size_);

        size_t offset{};
        std::memcpy(buffer.get(), (char *)mmap_ + offset, mmap_size_);
        offset += mmap_size_;

        data = std::make_shared<VarBuffer>(this, std::move(buffer), mmap_size_);

    } else {
        auto buffer = std::make_unique_for_overwrite<char[]>(mmap_size_);
        std::memcpy(buffer.get(), mmap_, mmap_size_);
        data = std::make_shared<VarBuffer>(this, std::move(buffer), mmap_size_);
    }

    cache_manager.Set(path, data, data->TotalSize());
}

} // namespace infinity

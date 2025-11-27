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

#include <cassert>
#include <sys/mman.h>
#include <unistd.h>

module infinity_core:raw_file_worker.impl;

import :raw_file_worker;
import :infinity_exception;
import :local_file_handle;
import :status;

import std;
import third_party;

namespace infinity {

RawFileWorker::RawFileWorker(std::shared_ptr<std::string> file_path, u32 file_size)
    : IndexFileWorker(std::move(file_path), {}, {}), buffer_size_(file_size) {}

RawFileWorker::~RawFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool RawFileWorker::Write(std::span<char> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto old_mmap_size = mmap_size_;
    mmap_size_ = buffer_size_;

    if (mmap_size_ == 0) {
        prepare_success = true;
        return true;
    }

    auto fd = file_handle->fd();
    ftruncate(fd, mmap_size_);
    if (mmap_ == nullptr) {
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    } else {
        mmap_ = mremap(mmap_, old_mmap_size, mmap_size_, MREMAP_MAYMOVE);
    }

    size_t offset{};
    std::memcpy((char *)mmap_ + offset, data.data(), buffer_size_);
    offset += buffer_size_;

    prepare_success = true; // Not run defer_fn
    return true;
}

void RawFileWorker::Read(std::shared_ptr<char[]> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    buffer_size_ = file_handle ? file_handle->FileSize() : 0;
    data = std::make_shared<char[]>(buffer_size_);
    if (!file_handle) {
        return;
    }
    if (!mmap_) {
        // buffer_size_ = file_handle_->FileSize();
        // data_ = static_cast<void *>(new char[buffer_size_]);
        auto fd = file_handle->fd();
        auto [nbytes, status1] = file_handle->Read(data.get(), buffer_size_);

        mmap_size_ = buffer_size_;
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 /*align_offset*/);
        if (mmap_ == MAP_FAILED) {
            mmap_ = nullptr;
        }
    } else {
        std::memcpy(data.get(), (char *)mmap_, buffer_size_);
    }
}

} // namespace infinity
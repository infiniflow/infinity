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
import :raw_file_worker;

import std;
import third_party;

namespace infinity {

RawFileWorker::RawFileWorker(std::shared_ptr<std::string> file_path, u32 file_size) : FileWorker(std::move(file_path)), buffer_size_(file_size) {
    RawFileWorker::AllocateInMemory();
}

RawFileWorker::~RawFileWorker() {
    RawFileWorker::FreeInMemory();
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

void RawFileWorker::AllocateInMemory() { data_ = static_cast<void *>(new char[buffer_size_]); }

void RawFileWorker::FreeInMemory() {
    delete[] static_cast<char *>(data_);
    data_ = nullptr;
}

bool RawFileWorker::Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    assert(data_ != nullptr && buffer_size_ > 0);
    if (mmap_) {
        munmap(mmap_, mmap_size_);
        mmap_ = nullptr;
    }

    mmap_size_ = buffer_size_;
    auto fd = file_handle_->fd();
    ftruncate(fd, mmap_size_);
    mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);

    size_t offset{};

    std::memcpy((char *)mmap_ + offset, data_, buffer_size_);
    offset += buffer_size_;
    // auto status = file_handle_->Append(data_, buffer_size_);
    prepare_success = true; // Not run defer_fn
    return true;
}

void RawFileWorker::Read(size_t file_size, bool other) {
    if (!mmap_) {
        buffer_size_ = file_handle_->FileSize();
        FreeInMemory();
        data_ = static_cast<void *>(new char[buffer_size_]);
        auto fd = file_handle_->fd();
        auto [nbytes, status1] = file_handle_->Read(data_, buffer_size_);

        mmap_size_ = buffer_size_;
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    }
}

} // namespace infinity
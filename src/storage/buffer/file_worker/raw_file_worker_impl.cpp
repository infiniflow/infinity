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

// std::atomic_int cnt_raw, cnt_raw_n;

void RawFileWorker::AllocateInMemory() {
    // cnt_raw.fetch_add(buffer_size_);
    // cnt_raw_n.fetch_add(1);
    // std::println("+, cnt_raw: {}, cnt_raw_n: {}, buffer_size: {}", cnt_raw.load(), cnt_raw_n.load(), buffer_size_);
    data_ = static_cast<void *>(new char[buffer_size_]);
}

void RawFileWorker::FreeInMemory() {
    // cnt_raw.fetch_sub(buffer_size_);
    // cnt_raw_n.fetch_sub(1);
    // std::println("-, cnt_raw: {}, cnt_raw_n: {}, buffer_size: {}", cnt_raw.load(), cnt_raw_n.load(), buffer_size_);

    delete[] static_cast<char *>(data_);
    data_ = nullptr;
}

bool RawFileWorker::Write(bool &prepare_success, size_t data_size, const FileWorkerSaveCtx &ctx) {
    assert(data_ != nullptr && buffer_size_ > 0);

    auto old_mmap_size = mmap_size_;
    mmap_size_ = buffer_size_;

    if (mmap_size_ == 0) {
        prepare_success = true;
        return true;
    }

    auto fd = file_handle_->fd();
    ftruncate(fd, mmap_size_);
    if (mmap_ == nullptr) {
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    } else {
        mmap_ = mremap(mmap_, old_mmap_size, mmap_size_, MREMAP_MAYMOVE);
    }

    size_t offset{};
    std::memcpy((char *)mmap_ + offset, data_, buffer_size_);
    offset += buffer_size_;

    prepare_success = true; // Not run defer_fn
    return true;
}

void RawFileWorker::Read(size_t file_size, bool other) {
    if (!mmap_) {
        FreeInMemory();
        buffer_size_ = file_handle_->FileSize();
        AllocateInMemory();
        // data_ = static_cast<void *>(new char[buffer_size_]);
        auto fd = file_handle_->fd();
        auto [nbytes, status1] = file_handle_->Read(data_, buffer_size_);

        mmap_size_ = buffer_size_;
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 /*align_offset*/);
        if (mmap_ == MAP_FAILED) {
            mmap_ = nullptr;
        }
    }
}

} // namespace infinity
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

module infinity_core:data_file_worker.impl;

import :data_file_worker;
import :infinity_exception;
import :status;
import :logger;
import :persistence_manager;
import :local_file_handle;
import :virtual_store;

import std;
import third_party;

import serialize;

namespace infinity {

DataFileWorker::DataFileWorker(std::shared_ptr<std::string> file_path, size_t buffer_size)
    : FileWorker(std::move(file_path)), buffer_size_(buffer_size) {}

DataFileWorker::~DataFileWorker() {
    madvise(mmap_, mmap_size_, MADV_FREE);
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool DataFileWorker::Write(std::span<char> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum

    // auto old_mmap_size = mmap_size_;
    // buffer_size_ += data.size()
    mmap_size_ = sizeof(u64) + sizeof(buffer_size_) + buffer_size_ + sizeof(u64);
    if (mmap_size_ == 0) {
        prepare_success = true;
        return true;
    }

    auto fd = file_handle->fd();
    VirtualStore::Truncate(GetFilePathTemp(), mmap_size_);
    if (mmap_ == nullptr) {
        mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 /*align_offset*/);
    }
    size_t offset{};

    u64 magic_number = 0x00dd3344;
    std::memcpy((char *)mmap_ + offset, &magic_number, sizeof(u64));
    offset += sizeof(u64);

    std::memcpy((char *)mmap_ + offset, &buffer_size_, sizeof(buffer_size_));
    offset += sizeof(buffer_size_);

    size_t data_size = data.size();
    // data_size = 99 * 16;
    std::memcpy((char *)mmap_ + offset, data.data(), data_size); // data size in span
    offset += data_size;

    size_t unused_size = buffer_size_ - data_size;
    if (unused_size > 0) {
        // std::string str(unused_size, '\0');
        // std::memcpy((char *)mmap_ + offset, str.c_str(), unused_size);
        offset += unused_size;
    }

    u64 checksum{};
    std::memcpy((char *)mmap_ + offset, &checksum, sizeof(checksum));
    offset += sizeof(u64);

    prepare_success = true; // Not run defer_fn
    return true;
}

void DataFileWorker::Read(std::shared_ptr<char[]> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    // data = std::make_shared_for_overwrite<char[]>(buffer_size_);
    data = std::make_shared<char[]>(buffer_size_);
    if (!file_handle) {
        return;
    }
    if (!mmap_) {
        if (file_size < sizeof(u64) * 3) {
            RecoverableError(Status::DataIOError(fmt::format("Incorrect file length {}.", file_size)));
        }
        // file header: magic number, buffer_size
        u64 magic_number{};
        auto [nbytes1, status1] = file_handle->Read(&magic_number, sizeof(magic_number));
        if (!status1.ok()) {
            RecoverableError(status1);
        }
        if (nbytes1 != sizeof(magic_number)) {
            RecoverableError(Status::DataIOError(fmt::format("Read magic number which length isn't {}.", nbytes1)));
        }
        if (magic_number != 0x00dd3344) {
            RecoverableError(Status::DataIOError(fmt::format("Read magic error, {} != 0x00dd3344.", magic_number)));
        }

        // u64 buffer_size_{};
        auto [nbytes2, status2] = file_handle->Read(&buffer_size_, sizeof(buffer_size_));
        if (nbytes2 != sizeof(buffer_size_)) {
            Status status = Status::DataIOError(fmt::format("Unmatched buffer length: {} / {}", nbytes2, buffer_size_));
            RecoverableError(status2);
        }

        if (file_size != buffer_size_ + 3 * sizeof(u64)) {
            Status status = Status::DataIOError(fmt::format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)));
            RecoverableError(status);
        }

        // file body
        // data_ = static_cast<void *>(new char[buffer_size_]);
        auto [nbytes3, status3] = file_handle->Read(data.get(), buffer_size_);
        if (nbytes3 != buffer_size_) {
            Status status = Status::DataIOError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes3));
            RecoverableError(status);
        }

        // file footer: checksum
        u64 checksum{0};
        auto [nbytes4, status4] = file_handle->Read(&checksum, sizeof(checksum));
        if (nbytes4 != sizeof(checksum)) {
            Status status = Status::DataIOError(fmt::format("Incorrect file checksum length: {}.", nbytes4));
            RecoverableError(status);
        }
        //
        auto fd = file_handle->fd();
        mmap_size_ = sizeof(u64) + sizeof(buffer_size_) + buffer_size_ + sizeof(u64);
        // std::memcpy((char *)mmap_true_, data_, mmap_true_size_);
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 /*align_offset*/);
        if (mmap_ == MAP_FAILED) {
            std::println("that code data: {}", mmap_size_);
            mmap_ = nullptr;
        }
    } else {
        std::memcpy(data.get(), (char *)mmap_ + sizeof(u64) /* magic_num */ + sizeof(buffer_size_), buffer_size_);
    }
}

} // namespace infinity

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

#include <tuple>

module infinity_core:data_file_worker.impl;

import :data_file_worker;
import :stl;
import :infinity_exception;
import :third_party;
import :status;
import :logger;
import :persistence_manager;
import serialize;
import :local_file_handle;

namespace infinity {

DataFileWorker::DataFileWorker(SharedPtr<String> data_dir,
                               SharedPtr<String> temp_dir,
                               SharedPtr<String> file_dir,
                               SharedPtr<String> file_name,
                               SizeT buffer_size,
                               PersistenceManager *persistence_manager)
    : FileWorker(std::move(data_dir), std::move(temp_dir), std::move(file_dir), std::move(file_name), persistence_manager),
      buffer_size_(buffer_size) {}

DataFileWorker::~DataFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void DataFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    if (buffer_size_ == 0) {
        String error_message = "Buffer size is 0.";
        UnrecoverableError(error_message);
    }
    data_ = static_cast<void *>(new char[buffer_size_]{});
}

void DataFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        String error_message = "Data is already freed.";
        UnrecoverableError(error_message);
    }
    delete[] static_cast<char *>(data_);
    data_ = nullptr;
}

// FIXME: to_spill
bool DataFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum

    u64 magic_number = 0x00dd3344;
    Status status = file_handle_->Append(&magic_number, sizeof(magic_number));
    if (!status.ok()) {
        RecoverableError(status);
    }

    status = file_handle_->Append(const_cast<SizeT *>(&buffer_size_), sizeof(buffer_size_));
    if (!status.ok()) {
        RecoverableError(status);
    }

    status = file_handle_->Append(data_, buffer_size_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    u64 checksum{};
    status = file_handle_->Append(&checksum, sizeof(checksum));
    if (!status.ok()) {
        RecoverableError(status);
    }
    prepare_success = true; // Not run defer_fn
    return true;
}

void DataFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {

    if (file_size < sizeof(u64) * 3) {
        Status status = Status::DataIOError(fmt::format("Incorrect file length {}.", file_size));
        RecoverableError(status);
    }

    // file header: magic number, buffer_size
    u64 magic_number{0};
    auto [nbytes1, status1] = file_handle_->Read(&magic_number, sizeof(magic_number));
    if (!status1.ok()) {
        RecoverableError(status1);
    }
    if (nbytes1 != sizeof(magic_number)) {
        Status status = Status::DataIOError(fmt::format("Read magic number which length isn't {}.", nbytes1));
        RecoverableError(status);
    }
    if (magic_number != 0x00dd3344) {
        Status status = Status::DataIOError(fmt::format("Read magic error, {} != 0x00dd3344.", magic_number));
        RecoverableError(status);
    }

    u64 buffer_size_{};
    auto [nbytes2, status2] = file_handle_->Read(&buffer_size_, sizeof(buffer_size_));
    if (nbytes2 != sizeof(buffer_size_)) {
        Status status = Status::DataIOError(fmt::format("Unmatched buffer length: {} / {}", nbytes2, buffer_size_));
        RecoverableError(status2);
    }

    if (file_size != buffer_size_ + 3 * sizeof(u64)) {
        Status status = Status::DataIOError(fmt::format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)));
        RecoverableError(status);
    }

    // file body
    data_ = static_cast<void *>(new char[buffer_size_]);
    auto [nbytes3, status3] = file_handle_->Read(data_, buffer_size_);
    if (nbytes3 != buffer_size_) {
        Status status = Status::DataIOError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes3));
        RecoverableError(status);
    }

    // file footer: checksum
    u64 checksum{0};
    auto [nbytes4, status4] = file_handle_->Read(&checksum, sizeof(checksum));
    if (nbytes4 != sizeof(checksum)) {
        Status status = Status::DataIOError(fmt::format("Incorrect file checksum length: {}.", nbytes4));
        RecoverableError(status);
    }
}

bool DataFileWorker::ReadFromMmapImpl(const void *p, SizeT file_size) {
    const char *ptr = static_cast<const char *>(p);
    u64 magic_number = ReadBufAdv<u64>(ptr);
    if (magic_number != 0x00dd3344) {
        Status status = Status::DataIOError(fmt::format("Read magic error: {} != 0x00dd3344.", magic_number));
        RecoverableError(status);
    }
    u64 buffer_size = ReadBufAdv<u64>(ptr);
    if (file_size != buffer_size + 3 * sizeof(u64)) {
        Status status = Status::DataIOError(fmt::format("File size: {} isn't matched with {}.", file_size, buffer_size + 3 * sizeof(u64)));
        RecoverableError(status);
    }
    mmap_data_ = const_cast<u8 *>(reinterpret_cast<const u8 *>(ptr));
    ptr += buffer_size;
    [[maybe_unused]] u64 checksum = ReadBufAdv<u64>(ptr);
    return true;
}

void DataFileWorker::FreeFromMmapImpl() {}

} // namespace infinity

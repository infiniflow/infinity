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

module infinity_core:var_file_worker.impl;

import :var_file_worker;
import :stl;
import :infinity_exception;
import :var_buffer;
import :third_party;
import :local_file_handle;
import :persistence_manager;
import :status;

namespace infinity {

VarFileWorker::VarFileWorker(SharedPtr<String> data_dir,
                             SharedPtr<String> temp_dir,
                             SharedPtr<String> file_dir,
                             SharedPtr<String> file_name,
                             SizeT buffer_size,
                             PersistenceManager *persistence_manager)
    : FileWorker(std::move(data_dir), std::move(temp_dir), std::move(file_dir), std::move(file_name), persistence_manager),
      buffer_size_(buffer_size) {}

VarFileWorker::~VarFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }

    if (mmap_data_ != nullptr) {
        auto *var_buffer = reinterpret_cast<VarBuffer *>(mmap_data_);
        delete var_buffer;
        mmap_data_ = nullptr;
    }
}

void VarFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    auto *buffer = new VarBuffer(buffer_obj_);
    data_ = static_cast<void *>(buffer);
}

void VarFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        String error_message = "Data is already freed.";
        UnrecoverableError(error_message);
    }
    auto *buffer = static_cast<VarBuffer *>(data_);
    buffer_size_ = buffer->TotalSize();
    delete buffer;
    data_ = nullptr;
}

SizeT VarFileWorker::GetMemoryCost() const {
    if (data_ == nullptr) {
        return buffer_size_;
    }
    auto *buffer = static_cast<VarBuffer *>(data_);
    return buffer->TotalSize();
}

bool VarFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        String error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    const auto *buffer = static_cast<const VarBuffer *>(data_);
    SizeT data_size = buffer->TotalSize();
    auto buffer_data = MakeUnique<char[]>(data_size);
    char *ptr = buffer_data.get();
    buffer->Write(ptr);

    Status status = file_handle_->Append(buffer_data.get(), data_size);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    prepare_success = true;
    buffer_size_ = data_size;
    return true;
}

void VarFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {
    if (data_ != nullptr) {
        String error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    if (file_size < buffer_size_) {
        String error_message = fmt::format("File: {} size {} is smaller than buffer size {}.", GetFilePath(), file_size, buffer_size_);
        UnrecoverableError(error_message);
    } else {
        buffer_size_ = file_size;
    }

    auto buffer = MakeUnique<char[]>(buffer_size_);
    auto [nbytes, status] = file_handle_->Read(buffer.get(), buffer_size_);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    if (nbytes != buffer_size_) {
        String error_message = fmt::format("Read {} bytes from file failed, only {} bytes read.", buffer_size_, nbytes);
        UnrecoverableError(error_message);
    }
    auto *var_buffer = new VarBuffer(buffer_obj_, std::move(buffer), buffer_size_);
    data_ = static_cast<void *>(var_buffer);
}

bool VarFileWorker::ReadFromMmapImpl(const void *ptr, SizeT file_size) {
    if (file_size < buffer_size_) {
        String error_message = fmt::format("File size {} is smaller than buffer size {}.", file_size, buffer_size_);
        UnrecoverableError(error_message);
    } else {
        buffer_size_ = file_size;
    }
    auto *var_buffer = new VarBuffer(buffer_obj_, static_cast<const char *>(ptr), buffer_size_);
    mmap_data_ = reinterpret_cast<u8 *>(var_buffer);
    return true;
}

void VarFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        String error_message = "Data is already freed.";
        UnrecoverableError(error_message);
    }
    auto *var_buffer = reinterpret_cast<VarBuffer *>(mmap_data_);
    delete var_buffer;
    mmap_data_ = nullptr;
}

} // namespace infinity
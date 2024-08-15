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

module var_file_worker;

import stl;
import infinity_exception;
import var_buffer;
import third_party;
import local_file_system;

namespace infinity {

VarFileWorker::VarFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT buffer_size)
    : FileWorker(std::move(file_dir), std::move(file_name)), buffer_size_(buffer_size) {}

VarFileWorker::~VarFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
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
    delete buffer;
    data_ = nullptr;
}

void VarFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (data_ == nullptr) {
        String error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    const auto *buffer = static_cast<const VarBuffer *>(data_);
    SizeT data_size = buffer->TotalSize();
    auto buffer_data = MakeUnique<char[]>(data_size);
    char *ptr = buffer_data.get();
    buffer->Write(ptr);

    LocalFileSystem fs;
    u64 nbytes = fs.Write(*file_handler_, buffer_data.get(), data_size);
    if (nbytes != data_size) {
        String error_message = fmt::format("Write {} bytes to file failed, only {} bytes written.", data_size, nbytes);
        UnrecoverableError(error_message);
    }
    prepare_success = true;
}

void VarFileWorker::ReadFromFileImpl(SizeT file_size) {
    if (data_ != nullptr) {
        String error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    if (file_size < buffer_size_) {
        String error_message = fmt::format("File size {} is smaller than buffer size {}.", file_size, buffer_size_);
        UnrecoverableError(error_message);
    }

    LocalFileSystem fs;
    auto buffer = MakeUnique<char[]>(buffer_size_);
    u64 nbytes = fs.Read(*file_handler_, buffer.get(), buffer_size_);
    if (nbytes != buffer_size_) {
        String error_message = fmt::format("Read {} bytes from file failed, only {} bytes read.", buffer_size_, nbytes);
        UnrecoverableError(error_message);
    }
    auto *var_buffer = new VarBuffer(buffer_obj_, std::move(buffer), buffer_size_);
    data_ = static_cast<void *>(var_buffer);
}

} // namespace infinity
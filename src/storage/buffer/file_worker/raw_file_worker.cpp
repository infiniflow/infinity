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

module raw_file_worker;

import stl;
import infinity_exception;
import local_file_system;
import third_party;
import status;

namespace infinity {

RawFileWorker::RawFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, u32 file_size)
    : FileWorker(std::move(file_dir), std::move(file_name)), buffer_size_(file_size) {}

RawFileWorker::~RawFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void RawFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    if (buffer_size_ == 0) {
        UnrecoverableError("Buffer size is 0.");
    }
    data_ = static_cast<void *>(new char[buffer_size_]);
}

void RawFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        UnrecoverableError("Data is already freed.");
    }
    delete[] static_cast<char *>(data_);
    data_ = nullptr;
}

void RawFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    assert(data_ != nullptr && buffer_size_ > 0);
    LocalFileSystem fs;
    i64 nbytes = fs.Write(*file_handler_, data_, buffer_size_);
    if (nbytes != (i64)buffer_size_) {
        RecoverableError(Status::DataIOError(fmt::format("Expect to write buffer with size: {}, but {} bytes is written", buffer_size_, nbytes)));
    }
    prepare_success = true; // Not run defer_fn
}

void RawFileWorker::ReadFromFileImpl() {
    LocalFileSystem fs;
    buffer_size_ = fs.GetFileSize(*file_handler_);
    data_ = static_cast<void *>(new char[buffer_size_]);
    i64 nbytes = fs.Read(*file_handler_, data_, buffer_size_);
    if (nbytes != (i64)buffer_size_) {
        RecoverableError(Status::DataIOError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes)));
    }
}

} // namespace infinity
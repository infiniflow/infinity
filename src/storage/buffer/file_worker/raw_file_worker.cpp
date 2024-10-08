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
#include <tuple>

module raw_file_worker;

import stl;
import infinity_exception;
import local_file_handle;
import third_party;
import status;
import logger;

namespace infinity {

RawFileWorker::RawFileWorker(SharedPtr<String> data_dir,
                             SharedPtr<String> temp_dir,
                             SharedPtr<String> file_dir,
                             SharedPtr<String> file_name,
                             u32 file_size,
                             PersistenceManager* persistence_manager)
    : FileWorker(std::move(data_dir), std::move(temp_dir), std::move(file_dir), std::move(file_name), persistence_manager), buffer_size_(file_size) {}

RawFileWorker::~RawFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void RawFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    if (buffer_size_ == 0) {
        String error_message = "Buffer size is 0.";
        UnrecoverableError(error_message);
    }
    data_ = static_cast<void *>(new char[buffer_size_]);
}

void RawFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        String error_message = "Data is already freed.";
        UnrecoverableError(error_message);
    }
    delete[] static_cast<char *>(data_);
    data_ = nullptr;
}

bool RawFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    assert(data_ != nullptr && buffer_size_ > 0);
    auto status = file_handle_->Append(data_, buffer_size_);
    if(!status.ok()) {
        RecoverableError(status);
    }
    prepare_success = true; // Not run defer_fn
    return true;
}

void RawFileWorker::ReadFromFileImpl(SizeT file_size) {
    buffer_size_ = file_handle_->FileSize();
    data_ = static_cast<void *>(new char[buffer_size_]);
    auto [nbytes, status1] = file_handle_->Read(data_, buffer_size_);
    if(!status1.ok()) {
        RecoverableError(status1);
    }
    if (nbytes != buffer_size_) {
        Status status = Status::DataIOError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes));
        RecoverableError(status);
    }
}

} // namespace infinity
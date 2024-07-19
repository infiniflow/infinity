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

module version_file_worker;

import stl;
import file_worker;
import block_version;
import infinity_exception;
import logger;

namespace infinity {

VersionFileWorker::VersionFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT capacity)
    : FileWorker(std::move(file_dir), std::move(file_name)), capacity_(capacity) {}

VersionFileWorker::~VersionFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void VersionFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    if (capacity_ == 0) {
        String error_message = "Capacity is 0.";
        UnrecoverableError(error_message);
    }
    auto *data = new BlockVersion(capacity_);
    data_ = static_cast<void *>(data);
}

void VersionFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        String error_message = "Data is already freed.";
        UnrecoverableError(error_message);
    }
    auto *data = static_cast<BlockVersion *>(data_);
    delete data;
    data_ = nullptr;
}

// FIXME
SizeT VersionFileWorker::GetMemoryCost() const { return capacity_ * sizeof(TxnTimeStamp); }

void VersionFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (data_ == nullptr) {
        String error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    auto *data = static_cast<BlockVersion *>(data_);
    if (to_spill) {
        data->SpillToFile(*file_handler_);
    } else {
        data->SaveToFile(checkpoint_ts_, *file_handler_);
    }
}

void VersionFileWorker::ReadFromFileImpl() {
    if (data_ != nullptr) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    auto *data = BlockVersion::LoadFromFile(*file_handler_).release();
    data_ = static_cast<void *>(data);
}

} // namespace infinity
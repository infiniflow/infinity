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
#include <unistd.h>

module infinity_core:version_file_worker.impl;

import :version_file_worker;
import :file_worker;
import :block_version;
import :infinity_exception;
import :logger;
import :persistence_manager;

import third_party;

namespace infinity {

VersionFileWorker::VersionFileWorker(std::shared_ptr<std::string> data_dir,
                                     std::shared_ptr<std::string> temp_dir,
                                     std::shared_ptr<std::string> file_dir,
                                     std::shared_ptr<std::string> file_name,
                                     size_t capacity,
                                     PersistenceManager *persistence_manager)
    : FileWorker(std::move(data_dir), std::move(temp_dir), std::move(file_dir), std::move(file_name), persistence_manager), capacity_(capacity) {
    VersionFileWorker::AllocateInMemory();
}

VersionFileWorker::~VersionFileWorker() {
    // if (data_ != nullptr) {
    //     FreeInMemory();
    //     data_ = nullptr;
    // }
    VersionFileWorker::FreeInMemory();
}

void VersionFileWorker::AllocateInMemory() {
    // if (data_ != nullptr) {
    //     UnrecoverableError("Data is already allocated.");
    // }
    // if (capacity_ == 0) {
    //     UnrecoverableError("Capacity is 0.");
    // }
    auto *data = new BlockVersion(capacity_);
    data_ = static_cast<void *>(data);
}

void VersionFileWorker::FreeInMemory() {
    // if (data_ == nullptr) {
    //     UnrecoverableError("Data is already freed.");
    // }
    auto *data = static_cast<BlockVersion *>(data_);
    delete data;
    data_ = nullptr;
}

// FIXME
size_t VersionFileWorker::GetMemoryCost() const { return capacity_ * sizeof(TxnTimeStamp); }

bool VersionFileWorker::WriteToTempImpl(bool &prepare_success, const FileWorkerSaveCtx &base_ctx) {
    if (mmap_true_) {
        // return true;
        munmap(mmap_true_, mmap_true_size_);
    }

    if (data_ == nullptr) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *data = static_cast<BlockVersion *>(data_);

    const auto &ctx = static_cast<const VersionFileWorkerSaveCtx &>(base_ctx);
    TxnTimeStamp ckp_ts = ctx.checkpoint_ts_;
    bool is_full = data->SaveToFile(mmap_true_, mmap_true_size_, ckp_ts, *file_handle_);
    if (is_full) {
        LOG_TRACE(fmt::format("Version file is full: {}", GetFilePath()));
        // if the version file is full, return true to spill to file
        return true;
    }
    return false;
}

bool VersionFileWorker::CopyToMmapImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) { return true; }

void VersionFileWorker::ReadFromFileImpl(size_t file_size, bool from_spill) {
    // if (data_ != nullptr) {
    //     UnrecoverableError("Data is already allocated.");
    // }
    auto *data = BlockVersion::LoadFromFile(mmap_true_, file_handle_.get()).release();
    data_ = static_cast<void *>(data);
}

} // namespace infinity
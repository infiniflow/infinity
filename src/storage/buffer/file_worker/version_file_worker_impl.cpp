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

module infinity_core:version_file_worker.impl;

import :version_file_worker;
import :stl;
import :file_worker;
import :block_version;
import :infinity_exception;
import :logger;
import :persistence_manager;

import third_party;

namespace infinity {

VersionFileWorker::VersionFileWorker(SharedPtr<String> data_dir,
                                     SharedPtr<String> temp_dir,
                                     SharedPtr<String> file_dir,
                                     SharedPtr<String> file_name,
                                     SizeT capacity,
                                     PersistenceManager *persistence_manager)
    : FileWorker(std::move(data_dir), std::move(temp_dir), std::move(file_dir), std::move(file_name), persistence_manager), capacity_(capacity) {}

VersionFileWorker::~VersionFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void VersionFileWorker::AllocateInMemory() {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    if (capacity_ == 0) {
        UnrecoverableError("Capacity is 0.");
    }
    auto *data = new BlockVersion(capacity_);
    data_ = static_cast<void *>(data);
}

void VersionFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        UnrecoverableError("Data is already freed.");
    }
    auto *data = static_cast<BlockVersion *>(data_);
    delete data;
    data_ = nullptr;
}

// FIXME
SizeT VersionFileWorker::GetMemoryCost() const { return capacity_ * sizeof(TxnTimeStamp); }

bool VersionFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &base_ctx) {
    if (data_ == nullptr) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *data = static_cast<BlockVersion *>(data_);

    // if spill to file, return true if success
    if (to_spill) {
        data->SpillToFile(file_handle_.get());
        return true;
    } else {
        const auto &ctx = static_cast<const VersionFileWorkerSaveCtx &>(base_ctx);
        bool is_full = data->SaveToFile(ctx.checkpoint_ts_, *file_handle_);
        if (is_full) {
            LOG_TRACE(fmt::format("Version file is full: {}", GetFilePath()));
            // if the version file is full, return true to spill to file
            return true;
        }
    }
    return false;
}

void VersionFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    auto *data = BlockVersion::LoadFromFile(file_handle_.get()).release();
    data_ = static_cast<void *>(data);
}

} // namespace infinity
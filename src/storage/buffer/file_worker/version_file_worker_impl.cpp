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
import :infinity_context;

import third_party;

namespace infinity {

VersionFileWorker::VersionFileWorker(std::shared_ptr<std::string> file_path, size_t capacity)
    : FileWorker(std::move(file_path)), capacity_(capacity) {}

VersionFileWorker::~VersionFileWorker() {
    madvise(mmap_, mmap_size_, MADV_FREE);
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool VersionFileWorker::Write(std::span<BlockVersion> data,
                              std::unique_ptr<LocalFileHandle> &file_handle,
                              bool &prepare_success,
                              const FileWorkerSaveCtx &base_ctx) {
    const auto &ctx = static_cast<const VersionFileWorkerSaveCtx &>(base_ctx);
    TxnTimeStamp ckp_ts = ctx.checkpoint_ts_;
    bool is_full = data.data()->SaveToFile(mmap_, mmap_size_, *rel_file_path_, ckp_ts, *file_handle);
    if (is_full) {
        LOG_TRACE(fmt::format("Version file is full: {}", GetFilePath()));
        // if the version file is full, return true to spill to file
        return true;
    }
    return false;
}

void VersionFileWorker::Read(std::shared_ptr<BlockVersion> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    if (!file_handle) {
        data = std::make_shared<BlockVersion>(8192);
        return;
    }
    BlockVersion::LoadFromFile(data, mmap_size_, mmap_, file_handle.get());
}

} // namespace infinity
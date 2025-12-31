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
import :fileworker_manager;

import third_party;

namespace infinity {

VersionFileWorker::VersionFileWorker(std::shared_ptr<std::string> file_path, size_t capacity)
    : FileWorker(std::move(file_path)), capacity_(capacity) {}

VersionFileWorker::~VersionFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool VersionFileWorker::Write(std::shared_ptr<BlockVersion> &data,
                              std::unique_ptr<LocalFileHandle> &file_handle,
                              bool &prepare_success,
                              const FileWorkerSaveCtx &base_ctx) {
    std::unique_lock l(mutex_);
    const auto &ctx = static_cast<const VersionFileWorkerSaveCtx &>(base_ctx);
    TxnTimeStamp ckp_ts = ctx.checkpoint_ts_;
    bool is_full = data->SaveToFile(mmap_, mmap_size_, *rel_file_path_, ckp_ts, *file_handle);
    auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->version_map_.cache_manager_;
    cache_manager.Set(*rel_file_path_, data, mmap_size_);
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

    auto &path = *rel_file_path_;
    auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->version_map_.cache_manager_;
    bool flag = cache_manager.Get(path, data);
    if (!flag) {
        auto fd = file_handle->fd();
        std::unique_lock l(mutex_);
        mmap_size_ = file_handle->FileSize();
        if (!mmap_) {
            mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
        }
        data = std::make_shared<BlockVersion>(8192);
        BlockVersion::LoadFromFile(data, mmap_size_, mmap_, file_handle.get());
        size_t request_space = file_handle->FileSize();
        cache_manager.Set(path, data, request_space);
    }
}

} // namespace infinity
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

#include <cerrno>
#include <optional>
#include <sys/mman.h>
#include <unistd.h>

module infinity_core:plaid_index_file_worker.impl;

import :plaid_index_file_worker;
import :index_file_worker;
import :file_worker;
import :index_base;
import :plaid_index;
import :index_plaid;
import :infinity_exception;
import :file_worker_type;
import :status;
import :local_file_handle;
import :virtual_store;
import :fileworker_manager;
import :infinity_context;

import third_party;

import logical_type;
import embedding_info;
import create_index_info;
import knn_expr;
import column_def;
import internal_types;

namespace infinity {

PlaidIndexFileWorker::~PlaidIndexFileWorker() {
    if (mmap_ != nullptr && mmap_ != MAP_FAILED) {
        munmap(mmap_, mmap_size_);
        mmap_ = nullptr;
    }
}

bool PlaidIndexFileWorker::Write(std::span<PlaidIndex> data,
                                 std::unique_ptr<LocalFileHandle> &file_handle,
                                 bool &prepare_success,
                                 const FileWorkerSaveCtx &) {
    std::unique_lock lock(mutex_);

    if (data.empty()) {
        UnrecoverableError("PlaidIndexFileWorker::Write: empty data");
    }

    auto *index = &data[0];

    // Calculate size and mmap
    mmap_size_ = index->CalcSize();
    auto fd = file_handle->fd();

    if (ftruncate(fd, mmap_size_) != 0) {
        UnrecoverableError(fmt::format("PlaidIndexFileWorker::Write: ftruncate failed: {}", strerror(errno)));
    }

    mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (mmap_ == MAP_FAILED) {
        UnrecoverableError(fmt::format("PlaidIndexFileWorker::Write: mmap failed: {}", strerror(errno)));
    }

    // Write index to mmap
    size_t offset = 0;
    index->SaveToPtr(mmap_, offset);

    // TODO: Add to cache manager for fast access when plaid_map_ is added to FileWorkerManager
    // auto &path = *rel_file_path_;
    // auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->plaid_map_.cache_manager_;
    // cache_manager.Set(path, *index, index->MemUsage());

    prepare_success = true;
    return true;
}

void PlaidIndexFileWorker::Read(std::shared_ptr<PlaidIndex> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    // Note: Caller (FileWorker::Read) already holds mutex_ lock
    // std::unique_lock lock(mutex_);

    // TODO: Try to get from cache when plaid_map_ is added to FileWorkerManager
    // const auto &path = *rel_file_path_;
    // auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->plaid_map_.cache_manager_;
    // std::optional<PlaidIndex> cached_index;
    // bool found = cache_manager.Get(path, cached_index);
    // if (found && cached_index.has_value()) {
    //     data = std::make_shared<PlaidIndex>(std::move(*cached_index));
    //     return;
    // }

    // Get index parameters
    const auto column_embedding_dim = GetEmbeddingInfo()->Dimension();
    const auto *index_plaid = static_cast<IndexPLAID *>(index_base_.get());
    const auto nbits = index_plaid->nbits_;
    const auto n_centroids = index_plaid->n_centroids_;

    // Create index with mmap
    if (!mmap_) {
        // Load from file
        if (!file_handle) {
            return;
        }
        mmap_size_ = file_handle->FileSize();
        auto fd = file_handle->fd();
        mmap_ = mmap(nullptr, mmap_size_, PROT_READ, MAP_SHARED, fd, 0);
        if (mmap_ == MAP_FAILED) {
            UnrecoverableError(fmt::format("PlaidIndexFileWorker::Read: mmap failed: {}", strerror(errno)));
        }
    }

    // Create index and load from mmap
    data = std::make_shared<PlaidIndex>(start_segment_offset_, column_embedding_dim, nbits, n_centroids, mmap_, mmap_size_);
    data->LoadFromPtr(mmap_, mmap_size_, file_size);

    // TODO: Add to cache when plaid_map_ is added to FileWorkerManager
    // cache_manager.Set(path, *data, data->MemUsage());
}

std::shared_ptr<PlaidIndex> PlaidIndexFileWorker::GetIndexFromCache() {
    // TODO: Implement when plaid_map_ is added to FileWorkerManager
    // const auto &path = *rel_file_path_;
    // auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->plaid_map_.cache_manager_;
    // std::optional<PlaidIndex> cached_index;
    // bool found = cache_manager.Get(path, cached_index);
    // if (found && cached_index.has_value()) {
    //     return std::make_shared<PlaidIndex>(std::move(*cached_index));
    // }
    return nullptr;
}

const EmbeddingInfo *PlaidIndexFileWorker::GetEmbeddingInfo() const { return static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get()); }

} // namespace infinity

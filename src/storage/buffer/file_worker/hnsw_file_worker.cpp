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

#include <set>

module infinity_core;

import :infinity_exception;
import :stl;
import :index_file_worker;
import :hnsw_alg;
import :hnsw_common;
import :index_hnsw;

import :index_base;
import :third_party;
import :logger;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;
#ifdef INDEX_HANDLER
import :hnsw_handler;
#else
import :abstract_hnsw;
#endif
import :virtual_store;
import :persistence_manager;
import :local_file_handle;

namespace infinity {

HnswFileWorker::HnswFileWorker(SharedPtr<String> data_dir,
                               SharedPtr<String> temp_dir,
                               SharedPtr<String> file_dir,
                               SharedPtr<String> file_name,
                               SharedPtr<IndexBase> index_base,
                               SharedPtr<ColumnDef> column_def,
                               PersistenceManager *persistence_manager,
                               SizeT index_size)
    : IndexFileWorker(std::move(data_dir),
                      std::move(temp_dir),
                      std::move(file_dir),
                      std::move(file_name),
                      std::move(index_base),
                      std::move(column_def),
                      persistence_manager) {
    if (index_size == 0) {

        String index_path = GetFilePath();
        auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kRead);
        if (status.ok()) {
            // When replay by checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
            index_size = file_handle->FileSize();
        }
    }
    index_size_ = index_size;
}

HnswFileWorker::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
    if (mmap_data_ != nullptr) {
        FreeFromMmapImpl();
        mmap_data_ = nullptr;
    }
}

void HnswFileWorker::AllocateInMemory() {
    if (data_) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
#ifdef INDEX_HANDLER
    data_ = static_cast<void *>(new HnswHandlerPtr());
#else
    data_ = static_cast<void *>(new AbstractHnsw());
#endif
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        String error_message = "FreeInMemory: Data is not allocated.";
        UnrecoverableError(error_message);
    }
#ifdef INDEX_HANDLER
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    delete *hnsw_handler;
    delete hnsw_handler;
#else
    auto *p = reinterpret_cast<AbstractHnsw *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                if (index != nullptr) {
                    delete index;
                }
            }
        },
        *p);
    delete p;
#endif
    data_ = nullptr;
}

bool HnswFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (!data_) {
        String error_message = "WriteToFileImpl: Data is not allocated.";
        UnrecoverableError(error_message);
    }
#ifdef INDEX_HANDLER
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    (*hnsw_handler)->SaveToPtr(*file_handle_);
#else
    auto *hnsw_index = reinterpret_cast<AbstractHnsw *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->SaveToPtr(*file_handle_);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        *hnsw_index);
#endif
    prepare_success = true;
    return true;
}

void HnswFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
#ifdef INDEX_HANDLER
    data_ = static_cast<void *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_.get()).release()));
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    if (from_spill) {
        (*hnsw_handler)->Load(*file_handle_);
    } else {
        (*hnsw_handler)->LoadFromPtr(*file_handle_, file_size);
    }
#else
    data_ = static_cast<void *>(new AbstractHnsw(HnswIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get())));
    auto *hnsw_index = reinterpret_cast<AbstractHnsw *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    if (from_spill) {
                        index = IndexT::Load(*file_handle_).release();
                    } else {
                        index = IndexT::LoadFromPtr(*file_handle_, file_size).release();
                    }
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        *hnsw_index);
#endif
}

bool HnswFileWorker::ReadFromMmapImpl(const void *ptr, SizeT size) {
    if (mmap_data_ != nullptr) {
        UnrecoverableError("Mmap data is already allocated.");
    }
#ifdef INDEX_HANDLER
    mmap_data_ = reinterpret_cast<u8 *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_.get(), false).release()));
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(mmap_data_);
    (*hnsw_handler)->LoadFromPtr(static_cast<const char *>(ptr), size);
#else
    mmap_data_ = reinterpret_cast<u8 *>(new AbstractHnsw(HnswIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get(), false)));
    auto *hnsw_index = reinterpret_cast<AbstractHnsw *>(mmap_data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (!IndexT::kOwnMem) {
                    const auto *p = static_cast<const char *>(ptr);
                    index = IndexT::LoadFromPtr(p, size).release();
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        *hnsw_index);
#endif
    return true;
}

void HnswFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        UnrecoverableError("Mmap data is not allocated.");
    }
#ifdef INDEX_HANDLER
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(mmap_data_);
    delete *hnsw_handler;
    delete hnsw_handler;
#else
    auto *hnsw_index = reinterpret_cast<AbstractHnsw *>(mmap_data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                delete index;
            }
        },
        *hnsw_index);
    delete hnsw_index;
#endif
    mmap_data_ = nullptr;
}

} // namespace infinity
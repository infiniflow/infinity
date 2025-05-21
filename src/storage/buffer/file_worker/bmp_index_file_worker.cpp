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

module bmp_index_file_worker;

import index_bmp;
import logger;
import infinity_exception;
import third_party;
import internal_types;
import bmp_util;
import bmp_alg;
#ifdef INDEX_HANDLER
import bmp_handler;
#else
import abstract_bmp;
#endif
import virtual_store;
import persistence_manager;
import local_file_handle;

namespace infinity {

BMPIndexFileWorker::BMPIndexFileWorker(SharedPtr<String> data_dir,
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
            // When replay by full checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
            index_size = file_handle->FileSize();
        }
    }
    index_size_ = index_size;
}

BMPIndexFileWorker::~BMPIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void BMPIndexFileWorker::AllocateInMemory() {
    if (data_) {
        const auto error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
#ifdef INDEX_HANDLER
    data_ = static_cast<void *>(new BMPHandlerPtr());
#else
    data_ = static_cast<void *>(new AbstractBMP());
#endif
}

void BMPIndexFileWorker::FreeInMemory() {
    if (!data_) {
        const auto error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
#ifdef INDEX_HANDLER
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    delete *bmp_handler;
    delete bmp_handler;
#else
    auto *p = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                if (arg != nullptr) {
                    delete arg;
                }
            }
        },
        *p);
    delete p;
#endif
    data_ = nullptr;
}

bool BMPIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
#ifdef INDEX_HANDLER
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    if (to_spill) {
        (*bmp_handler)->Save(*file_handle_);
    } else {
        (*bmp_handler)->SaveToPtr(*file_handle_);
    }
#else
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    if (to_spill) {
                        index->Save(*file_handle_);
                    } else {
                        index->SaveToPtr(*file_handle_);
                    }
                } else {
                    UnrecoverableError("BMPIndexFileWorker::WriteToFileImpl: index does not own memory");
                }
            }
        },
        *bmp_index);
#endif
    prepare_success = true;
    return true;
}

void BMPIndexFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
#ifdef INDEX_HANDLER
    data_ = static_cast<void *>(new BMPHandlerPtr(BMPHandler::Make(index_base_.get(), column_def_.get()).release()));
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(data_);
    if (from_spill) {
        (*bmp_handler)->Load(*file_handle_);
    } else {
        (*bmp_handler)->LoadFromPtr(*file_handle_, file_size);
    }
#else
    data_ = static_cast<void *>(new AbstractBMP(BMPIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get())));
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    if (from_spill) {
                        index = new IndexT(IndexT::Load(*file_handle_));
                    } else {
                        index = new IndexT(IndexT::LoadFromPtr(*file_handle_, file_size));
                    }
                } else {
                    UnrecoverableError("BMPIndexFileWorker::ReadFromFileImpl: index does not own memory");
                }
            }
        },
        *bmp_index);
#endif
}

bool BMPIndexFileWorker::ReadFromMmapImpl(const void *ptr, SizeT size) {
    if (mmap_data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
#ifdef INDEX_HANDLER
    mmap_data_ = reinterpret_cast<u8 *>(new BMPHandlerPtr(BMPHandler::Make(index_base_.get(), column_def_.get()).release()));
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(mmap_data_);
    (*bmp_handler)->LoadFromPtr(static_cast<const char *>(ptr), size);
#else
    mmap_data_ = reinterpret_cast<u8 *>(new AbstractBMP(BMPIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get(), false)));
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(mmap_data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (!IndexT::kOwnMem) {
                    const auto *p = static_cast<const char *>(ptr);
                    index = new IndexT(IndexT::LoadFromPtr(p, size));
                } else {
                    UnrecoverableError("BMPIndexFileWorker::ReadFromMmapImpl: index own memory");
                }
            }
        },
        *bmp_index);
#endif
    return true;
}

void BMPIndexFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        return;
    }
#ifdef INDEX_HANDLER
    auto *bmp_handler = reinterpret_cast<BMPHandlerPtr *>(mmap_data_);
    delete *bmp_handler;
    delete bmp_handler;
#else
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(mmap_data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (!IndexT::kOwnMem) {
                    delete index;
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        *bmp_index);
    delete bmp_index;
#endif
    mmap_data_ = nullptr;
}

} // namespace infinity

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

module infinity_core:hnsw_file_worker.impl;

import :hnsw_file_worker;
import :infinity_exception;
import :stl;
import :index_file_worker;
import :hnsw_alg;
import :hnsw_common;
import :index_hnsw;
import :index_base;
import :hnsw_handler;
import :virtual_store;
import :persistence_manager;
import :local_file_handle;

import std;
import third_party;

import column_def;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;

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
        UnrecoverableError("Data is already allocated.");
    }
    data_ = static_cast<void *>(new HnswHandlerPtr());
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    delete *hnsw_handler;
    delete hnsw_handler;
    data_ = nullptr;
}

bool HnswFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (!data_) {
        UnrecoverableError("WriteToFileImpl: Data is not allocated.");
    }
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    (*hnsw_handler)->SaveToPtr(*file_handle_);
    prepare_success = true;
    return true;
}

void HnswFileWorker::ReadFromFileImpl(SizeT file_size, bool from_spill) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    data_ = static_cast<void *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_.get()).release()));
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(data_);
    if (from_spill) {
        (*hnsw_handler)->Load(*file_handle_);
    } else {
        (*hnsw_handler)->LoadFromPtr(*file_handle_, file_size);
    }
}

bool HnswFileWorker::ReadFromMmapImpl(const void *ptr, SizeT size) {
    if (mmap_data_ != nullptr) {
        UnrecoverableError("Mmap data is already allocated.");
    }
    mmap_data_ = reinterpret_cast<u8 *>(new HnswHandlerPtr(HnswHandler::Make(index_base_.get(), column_def_.get(), false).release()));
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(mmap_data_);
    (*hnsw_handler)->LoadFromPtr(static_cast<const char *>(ptr), size);
    return true;
}

void HnswFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        UnrecoverableError("Mmap data is not allocated.");
    }
    auto *hnsw_handler = reinterpret_cast<HnswHandlerPtr *>(mmap_data_);
    delete *hnsw_handler;
    delete hnsw_handler;
    mmap_data_ = nullptr;
}

} // namespace infinity
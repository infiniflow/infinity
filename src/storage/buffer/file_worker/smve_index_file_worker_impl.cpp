// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:smve_index_file_worker.impl;

import :smve_index_file_worker;
import :smve_index;
import :index_smve;
import :bmp_handler;
import :infinity_exception;
import :local_file_handle;
import :persistence_manager;

import std;
import column_def;
import internal_types;

namespace infinity {

SMVEIndexFileWorker::SMVEIndexFileWorker(std::shared_ptr<std::string> data_dir,
                                         std::shared_ptr<std::string> temp_dir,
                                         std::shared_ptr<std::string> file_dir,
                                         std::shared_ptr<std::string> file_name,
                                         std::shared_ptr<IndexBase> index_base,
                                         std::shared_ptr<ColumnDef> column_def,
                                         PersistenceManager *persistence_manager,
                                         size_t index_size)
    : IndexFileWorker(std::move(data_dir),
                      std::move(temp_dir),
                      std::move(file_dir),
                      std::move(file_name),
                      std::move(index_base),
                      std::move(column_def),
                      persistence_manager) {
    if (index_size == 0) {
        std::string index_path = GetFilePath();
        auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kRead);
        if (status.ok()) {
            index_size = file_handle->FileSize();
        }
    }
    index_size_ = index_size;
}

SMVEIndexFileWorker::~SMVEIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
    if (mmap_data_ != nullptr) {
        FreeFromMmapImpl();
        mmap_data_ = nullptr;
    }
}

void SMVEIndexFileWorker::AllocateInMemory() {
    if (data_) {
        UnrecoverableError("Data is already allocated.");
    }
    data_ = static_cast<void *>(new SMVESerializedData());
}

void SMVEIndexFileWorker::FreeInMemory() {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *smve_data = static_cast<SMVESerializedData *>(data_);
    if (smve_data->bmp_handler) {
        delete smve_data->bmp_handler;
    }
    delete smve_data;
    data_ = nullptr;
}

bool SMVEIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *smve_data = static_cast<SMVESerializedData *>(data_);

    // Write SMVE metadata
    file_handle_->Append(&smve_data->width, sizeof(smve_data->width));
    file_handle_->Append(&smve_data->topk, sizeof(smve_data->topk));
    file_handle_->Append(&smve_data->embedding_dim, sizeof(smve_data->embedding_dim));
    file_handle_->Append(&smve_data->n_docs, sizeof(smve_data->n_docs));

    // Write projection matrix [dim × width] f32
    // NOTE: projection matrix must have been stored in smve_data by Dump
    // If not present, we skip it (backward compat or missing path)
    u32 dim = smve_data->embedding_dim;
    u32 width = smve_data->width;
    if (dim > 0 && width > 0 && smve_data->projection_matrix_copy) {
        file_handle_->Append(smve_data->projection_matrix_copy.get(), dim * width * sizeof(f32));
    }

    // Write BMP handler data
    if (smve_data->bmp_handler) {
        if (to_spill) {
            smve_data->bmp_handler->Save(*file_handle_);
        } else {
            smve_data->bmp_handler->SaveToPtr(*file_handle_);
        }
    }

    prepare_success = true;
    return true;
}

void SMVEIndexFileWorker::ReadFromFileImpl(size_t file_size, bool from_spill) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }

    auto *smve_data = new SMVESerializedData();
    data_ = static_cast<void *>(smve_data);

    // Read SMVE metadata
    file_handle_->Read(&smve_data->width, sizeof(smve_data->width));
    file_handle_->Read(&smve_data->topk, sizeof(smve_data->topk));
    file_handle_->Read(&smve_data->embedding_dim, sizeof(smve_data->embedding_dim));
    file_handle_->Read(&smve_data->n_docs, sizeof(smve_data->n_docs));

    // Read projection matrix
    u32 dim = smve_data->embedding_dim;
    u32 width = smve_data->width;
    if (dim > 0 && width > 0) {
        smve_data->projection_matrix_copy = std::make_unique<f32[]>(dim * width);
        file_handle_->Read(smve_data->projection_matrix_copy.get(), dim * width * sizeof(f32));
    }

    // Read BMP handler data
    smve_data->bmp_handler = BMPHandler::Make(index_base_.get(), column_def_.get()).release();
    if (!smve_data->bmp_handler) {
        UnrecoverableError("SMVEIndexFileWorker: Failed to create BMPHandler");
    }
    if (from_spill) {
        smve_data->bmp_handler->Load(*file_handle_);
    } else {
        // Calculate remaining data size
        size_t remaining = file_size - sizeof(u32) * 4 - dim * width * sizeof(f32);
        smve_data->bmp_handler->LoadFromPtr(*file_handle_, remaining);
    }
}

bool SMVEIndexFileWorker::ReadFromMmapImpl(const void *ptr, size_t size) {
    if (mmap_data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    mmap_data_ = reinterpret_cast<u8 *>(new SMVESerializedData());
    auto *smve_data = reinterpret_cast<SMVESerializedData *>(mmap_data_);

    const char *cursor = static_cast<const char *>(ptr);
    std::memcpy(&smve_data->width, cursor, sizeof(u32)); cursor += sizeof(u32);
    std::memcpy(&smve_data->topk, cursor, sizeof(u32)); cursor += sizeof(u32);
    std::memcpy(&smve_data->embedding_dim, cursor, sizeof(u32)); cursor += sizeof(u32);
    std::memcpy(&smve_data->n_docs, cursor, sizeof(u32)); cursor += sizeof(u32);

    u32 dim = smve_data->embedding_dim;
    u32 width = smve_data->width;
    if (dim > 0 && width > 0) {
        smve_data->projection_matrix_copy = std::make_unique<f32[]>(dim * width);
        std::memcpy(smve_data->projection_matrix_copy.get(), cursor, dim * width * sizeof(f32));
        cursor += dim * width * sizeof(f32);
    }

    size_t bmp_remaining = size - (cursor - static_cast<const char *>(ptr));
    smve_data->bmp_handler = BMPHandler::Make(index_base_.get(), column_def_.get(), false).release();
    smve_data->bmp_handler->LoadFromPtr(cursor, bmp_remaining);

    return true;
}

void SMVEIndexFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        return;
    }
    auto *smve_data = reinterpret_cast<SMVESerializedData *>(mmap_data_);
    if (smve_data->bmp_handler) {
        delete smve_data->bmp_handler;
    }
    delete smve_data;
    mmap_data_ = nullptr;
}

} // namespace infinity

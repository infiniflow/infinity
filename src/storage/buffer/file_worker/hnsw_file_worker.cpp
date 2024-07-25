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

module hnsw_file_worker;

import infinity_exception;
import stl;
import index_file_worker;
import hnsw_alg;
import hnsw_common;
import index_hnsw;

import index_base;
import third_party;
import logger;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;
import abstract_hnsw;
import local_file_system;
import file_system_type;

namespace infinity {

HnswFileWorker::HnswFileWorker(SharedPtr<String> file_dir,
                               SharedPtr<String> file_name,
                               SharedPtr<IndexBase> index_base,
                               SharedPtr<ColumnDef> column_def,
                               SizeT index_size)
    : IndexFileWorker(file_dir, file_name, index_base, column_def) {
    if (index_size == 0) {
        LocalFileSystem fs;

        String index_path = GetFilePath();
        auto [file_handler, status] = fs.OpenFile(index_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        index_size = fs.GetFileSize(*file_handler);
    }
    index_size_ = index_size;
}

HnswFileWorker::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void HnswFileWorker::AllocateInMemory() {
    if (data_) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    data_ = static_cast<void *>(new AbstractHnsw());
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        String error_message = "FreeInMemory: Data is not allocated.";
        UnrecoverableError(error_message);
    }
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
    data_ = nullptr;
}

void HnswFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (!data_) {
        String error_message = "WriteToFileImpl: Data is not allocated.";
        UnrecoverableError(error_message);
    }
    auto *hnsw_index = reinterpret_cast<AbstractHnsw *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                index->Save(*file_handler_);
            }
        },
        *hnsw_index);
    prepare_success = true;
}

void HnswFileWorker::ReadFromFileImpl(SizeT file_size) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    data_ = static_cast<void *>(new AbstractHnsw(HnswIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get())));
    auto *bmp_index = reinterpret_cast<AbstractHnsw *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                index = IndexT::Load(*file_handler_).release();
            }
        },
        *bmp_index);
}

} // namespace infinity
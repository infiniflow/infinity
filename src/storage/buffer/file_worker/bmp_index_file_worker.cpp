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

module bmp_index_file_worker;

import index_bmp;
import logger;
import infinity_exception;
import third_party;
import internal_types;
import bmp_util;
import bmp_alg;
import abstract_bmp;
import local_file_system;
import file_system_type;

namespace infinity {

BMPIndexFileWorker::BMPIndexFileWorker(SharedPtr<String> file_dir,
                                       SharedPtr<String> file_name,
                                       SharedPtr<IndexBase> index_base,
                                       SharedPtr<ColumnDef> column_def,
                                       SizeT index_size)
    : IndexFileWorker(file_dir, file_name, index_base, column_def) {
    if (index_size == 0) {
        LocalFileSystem fs;

        String index_path = GetFilePath();
        auto [file_handler, status] = fs.OpenFile(index_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
        if (status.ok()) {
            // When replay by full checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
            index_size = fs.GetFileSize(*file_handler);
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
    data_ = static_cast<void *>(new AbstractBMP());
}

void BMPIndexFileWorker::FreeInMemory() {
    if (!data_) {
        const auto error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
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
    data_ = nullptr;
}

bool BMPIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                index->Save(*file_handler_);
            }
        },
        *bmp_index);
    prepare_success = true;
    return true;
}

void BMPIndexFileWorker::ReadFromFileImpl(SizeT file_size) {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    data_ = static_cast<void *>(new AbstractBMP(BMPIndexInMem::InitAbstractIndex(index_base_.get(), column_def_.get())));
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                index = new IndexT(IndexT::Load(*file_handler_));
            }
        },
        *bmp_index);
}

} // namespace infinity

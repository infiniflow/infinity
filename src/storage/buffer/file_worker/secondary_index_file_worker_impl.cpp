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

module infinity_core:secondary_index_file_worker.impl;

import :secondary_index_file_worker;
import :index_file_worker;
import :file_worker;
import :logger;
import :index_base;
import :index_secondary;
import :index_secondary_functional;
import :secondary_index_data;
import :infinity_exception;
import :persistence_manager;

import third_party;

namespace infinity {

SecondaryIndexFileWorker::~SecondaryIndexFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool SecondaryIndexFileWorker::Write(SecondaryIndexDataBase<HighCardinalityTag> *data,
                                     std::unique_ptr<LocalFileHandle> &file_handle,
                                     bool &prepare_success,
                                     const FileWorkerSaveCtx &ctx) {
    std::unique_lock l(mutex_);
    auto index = data;
    index->SaveIndexInner(*file_handle);
    prepare_success = true;
    file_handle->Sync();
    LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");

    return true;
}

bool SecondaryIndexFileWorker::Write(SecondaryIndexDataBase<LowCardinalityTag> *data,
                                     std::unique_ptr<LocalFileHandle> &file_handle,
                                     bool &prepare_success,
                                     const FileWorkerSaveCtx &ctx) {
    std::unique_lock l(mutex_);
    auto index = data;
    index->SaveIndexInner(*file_handle);
    prepare_success = true;
    file_handle->Sync();
    LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");

    return true;
}

void SecondaryIndexFileWorker::Read(SecondaryIndexDataBase<HighCardinalityTag> *&data,
                                    std::unique_ptr<LocalFileHandle> &file_handle,
                                    size_t file_size) {
    std::unique_lock l(mutex_);
    auto index = GetSecondaryIndexData(std::make_shared<DataType>(index_data_type_), row_count_, false);
    // data = std::shared_ptr<SecondaryIndexDataBase<HighCardinalityTag>>(index);
    data = index;
    if (!file_handle) {
        return;
    }
    data->ReadIndexInner(*file_handle);
    LOG_TRACE("Finished Read().");
}

void SecondaryIndexFileWorker::Read(SecondaryIndexDataBase<LowCardinalityTag> *&data,
                                    std::unique_ptr<LocalFileHandle> &file_handle,
                                    size_t file_size) {
    std::unique_lock l(mutex_);
    auto index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(std::make_shared<DataType>(index_data_type_), row_count_, false);
    data = index;
    if (!file_handle) {
        return;
    }
    data->ReadIndexInner(*file_handle);
    LOG_TRACE("Finished Read().");
}

} // namespace infinity

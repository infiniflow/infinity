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

module infinity_core:ivf_index_file_worker.impl;

import :ivf_index_file_worker;
import :index_file_worker;
import :file_worker;
import :logger;
import :index_base;
import :ivf_index_data;
import :infinity_exception;
import :persistence_manager;

import third_party;

namespace infinity {

IVFIndexFileWorker::~IVFIndexFileWorker() {
    FreeInMemory();

    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

void IVFIndexFileWorker::AllocateInMemory() {
    data_ = static_cast<void *>(IVFIndexInChunk::GetNewIVFIndexInChunk(index_base_.get(), column_def_.get()));
}

void IVFIndexFileWorker::FreeInMemory() {
    if (data_) {
        auto index = static_cast<IVFIndexInChunk *>(data_);
        delete index;
        data_ = nullptr;
        LOG_TRACE("Finished IVFIndexFileWorker::FreeInMemory(), deleted data_ ptr.");
    }
}

bool IVFIndexFileWorker::Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (data_) {
        auto index = static_cast<IVFIndexInChunk *>(data_);
        index->SaveIndexInner(*file_handle_);
        prepare_success = true;
        LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
    } else {
        UnrecoverableError("WriteToFileImpl: data_ is nullptr");
    }
    return true;
}

void IVFIndexFileWorker::Read(size_t file_size, bool other) {
    auto index = IVFIndexInChunk::GetNewIVFIndexInChunk(index_base_.get(), column_def_.get());
    index->ReadIndexInner(*file_handle_);
    data_ = static_cast<void *>(index);
    LOG_TRACE("Finished Read().");
}

} // namespace infinity
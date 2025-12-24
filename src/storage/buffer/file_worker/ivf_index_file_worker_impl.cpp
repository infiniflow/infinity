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
import :fileworker_manager;

import third_party;

namespace infinity {

IVFIndexFileWorker::~IVFIndexFileWorker() {
    munmap(mmap_, mmap_size_);
    mmap_ = nullptr;
}

bool IVFIndexFileWorker::Write(IVFIndexInChunk *&data,
                               std::unique_ptr<LocalFileHandle> &file_handle,
                               bool &prepare_success,
                               const FileWorkerSaveCtx &ctx) {
    auto *index = data;
    index->SaveIndexInner(*file_handle);
    auto fd = file_handle->fd();
    mmap_size_ = file_handle->FileSize();
    mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->ivf_map_.cache_manager_;
    cache_manager.Set(*rel_file_path_, data, mmap_size_);
    prepare_success = true;
    file_handle->Sync();
    LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
    cache_manager.UnPin(*rel_file_path_);
    return true;
}

void IVFIndexFileWorker::Read(IVFIndexInChunk *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    if (!file_handle) {
        auto *index = IVFIndexInChunk::GetNewIVFIndexInChunk(index_base_.get(), column_def_.get());
        data = index;
        return;
    }
    auto &path = *rel_file_path_;
    auto &cache_manager = InfinityContext::instance().storage()->fileworker_manager()->ivf_map_.cache_manager_;
    cache_manager.Pin(path);
    bool flag = cache_manager.Get(path, data);
    if (!flag) {
        auto *index = IVFIndexInChunk::GetNewIVFIndexInChunk(index_base_.get(), column_def_.get());
        // data = std::shared_ptr<IVFIndexInChunk>(index);
        data = index;

        auto fd = file_handle->fd();
        mmap_size_ = file_handle->FileSize();
        if (!mmap_) {
            mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
        }
        data->ReadIndexInner(*file_handle);
        LOG_TRACE("Finished Read().");
        size_t request_space = file_handle->FileSize();
        cache_manager.Set(path, data, request_space);
        // data = HnswHandlerPtr{HnswHandler::Make(index_base_.get(), column_def_).release()};
        // auto fd = file_handle->fd();
        // mmap_size_ = file_handle->FileSize();
        // if (!mmap_) {
        //     mmap_ = mmap(nullptr, mmap_size_, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
        // }
        // data->LoadFromPtr(mmap_, mmap_size_, *file_handle, file_size);
        // size_t request_space = file_handle->FileSize();
        // cache_manager.Set(path, data, request_space);
    }
}

} // namespace infinity
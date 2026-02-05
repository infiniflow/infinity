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

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

module infinity_core:hnsw_file_worker.impl;

import :hnsw_file_worker;
import :infinity_exception;
import :index_file_worker;
import :hnsw_alg;
import :hnsw_common;
import :index_hnsw;
import :index_base;
import :hnsw_handler;
import :virtual_store;
import :persistence_manager;
import :local_file_handle;
import :fileworker_manager;

import std;
import third_party;

import column_def;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;

namespace infinity {

using segment_manager_t = boost::interprocess::managed_mapped_file::segment_manager;

HnswFileWorker::HnswFileWorker(std::shared_ptr<std::string> file_path,
                               std::shared_ptr<IndexBase> index_base,
                               std::shared_ptr<ColumnDef> column_def,
                               size_t index_size)
    : IndexFileWorker(std::move(file_path), std::move(index_base), std::move(column_def)) {
    // if (index_size == 0) {
    //
    //     std::string index_path = GetPath();
    //     auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kReadWrite);
    //     if (status.ok()) {
    //         // When replay by checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
    //         index_size = file_handle->FileSize();
    //     }
    // }
    // index_size_ = index_size;
}

void HnswFileWorker::Read(HnswHandler *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    // std::unique_lock l(mutex_);
    auto &path = *rel_file_path_;
    auto tmp_path = GetWorkingPath();
    auto data_path = GetPath();
    if (!inited_) {
        boost::interprocess::file_mapping::remove(tmp_path.c_str());
        auto file_path = GetPath();
        if (persistence_manager_) {
            std::println("????A");
            auto result = persistence_manager_->GetObjCache(file_path);
            auto obj_addr = result.obj_addr_;
            if (obj_addr.Valid()) {
                auto true_file_path = fmt::format("{}/{}", persistence_manager_->workspace(), obj_addr.obj_key_);
                auto [file_handle, status] = VirtualStore::Open(true_file_path, FileAccessMode::kReadWrite);

                VirtualStore::CopyRange(true_file_path.c_str(), tmp_path.c_str(), obj_addr.part_offset_, 0, obj_addr.part_size_);
            }
        } else if (VirtualStore::Exists(file_path, true)) {
            std::println("????B");
            auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kReadWrite);
            if (status.ok()) {
                VirtualStore::CopyRange(data_path, tmp_path, 0, 0, file_size);
                // VirtualStore::Copy(data_path, tmp_path);
            }
        }

        segment_ = boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create_infinity, tmp_path.c_str(), 500 * 1024 * 1024);
        boost::interprocess::allocator<void, segment_manager_t> alloc_inst(segment_.get_segment_manager());
        data = segment_.find_or_construct<HnswHandler>(path.c_str())(index_base_.get(), column_def_, alloc_inst);
        inited_ = true;
        return;
    }
    auto result = segment_.find<HnswHandler>(path.c_str());
    data = result.first;
}

} // namespace infinity
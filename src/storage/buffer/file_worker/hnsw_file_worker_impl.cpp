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
HnswFileWorker::HnswFileWorker(std::shared_ptr<std::string> file_path,
                               std::shared_ptr<IndexBase> index_base,
                               std::shared_ptr<ColumnDef> column_def,
                               size_t index_size)
    : IndexFileWorker(std::move(file_path), std::move(index_base), std::move(column_def)) {
    // if (index_size == 0) {
    //
    //     std::string index_path = GetFilePath();
    //     auto [file_handle, status] = VirtualStore::Open(index_path, FileAccessMode::kReadWrite);
    //     if (status.ok()) {
    //         // When replay by checkpoint, the data is deleted, but catalog is recovered. Do not read file in recovery.
    //         index_size = file_handle->FileSize();
    //     }
    // }
    // index_size_ = index_size;
}

HnswFileWorker::~HnswFileWorker() {}

void HnswFileWorker::Read(HnswHandler *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    std::unique_lock l(mutex_);
    auto &path = *rel_file_path_;
    std::println("fuck path: {}", path);
    auto tmp_path = GetFilePathTemp();
    auto data_path = GetFilePath();
    if (!inited_) {
        if (!VirtualStore::Exists("/var/infinity/tmp")) {
            VirtualStore::MakeDirectory("/var/infinity/tmp");
        }

        if (!VirtualStore::Exists(tmp_path.c_str())) {
            std::println("fuck clean tmp");
            auto [handle, status] = VirtualStore::Open(tmp_path, FileAccessMode::kReadWrite);
            close(handle->fd());
            // VirtualStore::DeleteFile(tmp_path.c_str());
        }

        boost::interprocess::file_mapping::remove(tmp_path.c_str());

        auto copy_range = [](const char *src, const char *dst, off_t src_off, off_t dst_off, size_t len) {
            int in = open(src, O_RDONLY);
            int out = open(dst, O_WRONLY | O_CREAT, 0644);
            if (in < 0 || out < 0)
                return -1;

            ssize_t copied = copy_file_range(in, &src_off, out, &dst_off, len, 0);
            close(in);
            close(out);
            return (copied == (ssize_t)len) ? 0 : -1;
        };

        auto file_path = GetFilePath();
        if (persistence_manager_) {
            std::println("A");
            auto result = persistence_manager_->GetObjCache(file_path);
            obj_addr_ = result.obj_addr_;
            if (obj_addr_.Valid()) {
                auto true_file_path = fmt::format("{}/{}", persistence_manager_->workspace(), obj_addr_.obj_key_);
                auto [file_handle, status] = VirtualStore::Open(true_file_path, FileAccessMode::kReadWrite);

                copy_range(true_file_path.c_str(), tmp_path.c_str(), obj_addr_.part_offset_, 0, obj_addr_.part_size_);
            }
        } else if (VirtualStore::Exists(file_path, true)) {
            std::println("B");
            auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kReadWrite);
            if (status.ok()) {
                VirtualStore::Copy(tmp_path, data_path);
            }
        }

        segment_ = boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create_infinity, tmp_path.c_str(), 1145141919ull * 2);
        auto *sm = segment_.get_segment_manager();
        data = segment_.find_or_construct<HnswHandler>("")(index_base_.get(), column_def_, sm);
        data->ReBindAllocator(sm);
        inited_ = true;

        std::println("fuck GetRowCnt special: {}", data->GetRowCount());
        return;
    }
    auto result = segment_.find<HnswHandler>("");
    data = result.first;
    std::println("fuck GetRowCnt normal: {}", data->GetRowCount());
}

} // namespace infinity
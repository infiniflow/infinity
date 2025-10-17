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

#include <cerrno>

module infinity_core:file_worker.impl;

import :file_worker;
import :utility;
import :infinity_exception;
import :local_file_handle;
import :defer_op;
import :status;
import :virtual_store;
import :persistence_manager;
import :infinity_context;
import :logger;
import :persist_result_handler;
import :kv_code;
import :kv_store;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

FileWorker::FileWorker(std::shared_ptr<std::string> rel_file_path) : rel_file_path_(std::move(rel_file_path)) {
    mmap_ = nullptr;
    persistence_manager_ = InfinityContext::instance().storage()->persistence_manager();
}

FileWorker::~FileWorker() {}

bool FileWorker::Write(const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        UnrecoverableError("No data will be written.");
    }

    auto [file_handle, status] = VirtualStore::Open(GetFilePathTemp(), FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    file_handle_ = std::move(file_handle);

    bool prepare_success = false;

    bool all_save = Write(prepare_success, ctx);
    if (prepare_success) {
        file_handle_->Sync();
    }
    return all_save;
}

void FileWorker::Read() {
    size_t file_size = 0;
    auto [file_handle, status] = VirtualStore::Open(GetFilePathTemp(), FileAccessMode::kRead);
    if (!status.ok()) {
        // convert to GetFilePath
        return;
    }
    if (persistence_manager_) {
        file_handle->Seek(obj_addr_.part_offset_);
        file_size = obj_addr_.part_size_;
    } else {
        file_size = file_handle->FileSize();
    }
    file_handle_ = std::move(file_handle);
    DeferFn defer_fn2([&]() { file_handle_ = nullptr; });
    Read(file_size);
}

void FileWorker::MoveFile() {
    auto temp_path = GetFilePathTemp();
    auto data_path = GetFilePath();
    if (persistence_manager_ == nullptr) {
        if (!VirtualStore::Exists(temp_path)) {
            return;
        }
        if (!VirtualStore::Exists(data_path)) {
            VirtualStore::MakeDirectory(data_path);
        }
        VirtualStore::Copy(temp_path, data_path); // sys call, may need copy
    } else {
        PersistResultHandler handler(persistence_manager_);
        if (!VirtualStore::Exists(temp_path)) {
            return;
        }
        PersistWriteResult persist_result = persistence_manager_->Persist(data_path, temp_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;
    }
}

void FileWorker::Load() {
    std::unique_lock<std::mutex> locker(l_); // lockl?
    if (VirtualStore::Exists(GetFilePathTemp()) || VirtualStore::Exists(GetFilePath())) {
        Read();
    }
}

void FileWorker::SetData(void *data) {
    data_ = data;
    [[maybe_unused]] auto foo = Write();
}

void FileWorker::SetDataSize(size_t size) { UnrecoverableError("Not implemented"); }

// Get absolute file path. As key of buffer handle.
std::string FileWorker::GetFilePath() const { return fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *rel_file_path_); }

std::string FileWorker::GetFilePathTemp() const { return fmt::format("{}/{}", InfinityContext::instance().config()->TempDir(), *rel_file_path_); }

Status FileWorker::CleanupFile() const {
    if (persistence_manager_ != nullptr) {
        PersistResultHandler handler{persistence_manager_};
        auto result_temp = persistence_manager_->Cleanup(GetFilePathTemp());
        if (!result_temp.obj_addr_.Valid()) {
            return Status::OK();
        }
        auto result_data = persistence_manager_->Cleanup(GetFilePath());
        if (!result_data.obj_addr_.Valid()) {
            return Status::OK();
        }
        // Delete files
        handler.HandleWriteResult(result_temp);
        handler.HandleWriteResult(result_data);
        return Status::OK();
    }

    auto status = VirtualStore::DeleteFile(GetFilePathTemp());
    if (!status.ok()) {
        return status;
    }
    status = VirtualStore::DeleteFile(GetFilePath());
    return status;
}

} // namespace infinity
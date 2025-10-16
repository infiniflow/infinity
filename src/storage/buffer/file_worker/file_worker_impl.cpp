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

FileWorker::FileWorker(std::shared_ptr<std::string> file_path) : file_path_(std::move(file_path)) {
    mmap_true_ = nullptr;
    persistence_manager_ = InfinityContext::instance().storage()->persistence_manager();
}

FileWorker::~FileWorker() {}

bool FileWorker::Write(const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        UnrecoverableError("No data will be written.");
    }

    auto [file_handle, status] = VirtualStore::Open(*file_path_, FileAccessMode::kWrite);
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
    auto read_path = *file_path_;
    bool use_object_cache = persistence_manager_ != nullptr;
    size_t file_size = 0;
    auto [file_handle, status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
    if (!status.ok()) {
        return;
    }
    if (use_object_cache) {
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
    std::string dest_dir = ChooseFileDir(false);
    std::string dest_path = fmt::format("{}/{}", dest_dir, *file_name_);
    if (persistence_manager_ == nullptr) {
        if (!VirtualStore::Exists(*file_path_)) {
            return;
        }
        if (!VirtualStore::Exists(dest_dir)) {
            VirtualStore::MakeDirectory(dest_dir);
        }
        VirtualStore::Rename(*file_path_, dest_path);
    } else {
        PersistResultHandler handler(persistence_manager_);
        if (!VirtualStore::Exists(*file_path_)) {
            return;
        }
        PersistWriteResult persist_result = persistence_manager_->Persist(dest_path, *file_path_);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;
    }
}

void FileWorker::Load() {
    std::unique_lock<std::mutex> locker(l_); // lockl?
    if (VirtualStore::Exists(*file_path_)) {
        Read(true);
    }
}

void FileWorker::SetData(void *data) {
    data_ = data;
    [[maybe_unused]] auto foo = Write();
}

void FileWorker::SetDataSize(size_t size) { UnrecoverableError("Not implemented"); }

// Get absolute file path. As key of buffer handle.
std::string FileWorker::GetFilePath() const { return std::filesystem::path(*data_dir_) / *file_dir_ / *file_name_; }

std::string FileWorker::GetFilePathTmp() const { return std::filesystem::path(*temp_dir_) / *file_dir_ / *file_name_; }

std::string FileWorker::ChooseFileDir(bool is_temp) const {
    return is_temp ? std::filesystem::path(*temp_dir_) / *file_dir_ : std::filesystem::path(*data_dir_) / *file_dir_;
}

std::pair<std::optional<DeferFn<std::function<void()>>>, std::string> FileWorker::GetFilePathInner(bool from_spill) {
    bool use_object_cache = !from_spill && persistence_manager_ != nullptr;
    std::optional<DeferFn<std::function<void()>>> defer_fn;
    std::string read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    if (use_object_cache) {
        PersistReadResult result = persistence_manager_->GetObjCache(read_path);
        defer_fn.emplace(([=, this]() {
            if (use_object_cache && obj_addr_.Valid()) {
                std::string read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
                PersistWriteResult res = persistence_manager_->PutObjCache(read_path);
                PersistResultHandler handler = PersistResultHandler(persistence_manager_);
                handler.HandleWriteResult(res);
            }
        }));
        PersistResultHandler handler = PersistResultHandler(persistence_manager_);
        obj_addr_ = handler.HandleReadResult(result);
        if (!obj_addr_.Valid()) {
            UnrecoverableError(fmt::format("Failed to find object for local path {}", read_path));
        }
        read_path = persistence_manager_->GetObjPath(obj_addr_.obj_key_);
    }
    return {std::move(defer_fn), read_path};
}

Status FileWorker::CleanupFile() const {
    if (persistence_manager_ != nullptr) {
        PersistResultHandler handler(persistence_manager_);
        std::string path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
        PersistWriteResult result = persistence_manager_->Cleanup(path);
        if (!result.obj_addr_.Valid()) {
            return Status::OK();
        }
        handler.HandleWriteResult(result); // Delete files
        return Status::OK();
    }

    std::string path_str = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);

    Status status;

    status = VirtualStore::DeleteFile(path_str);

    path_str = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);

    status = VirtualStore::DeleteFile(path_str);

    return Status::OK();
}

void FileWorker::CleanupTempFile() const {
    std::string path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    if (VirtualStore::Exists(path)) {
        LOG_TRACE(fmt::format("Clean temp file: {}", path));
        VirtualStore::DeleteFile(path);
    } else {
        UnrecoverableError(fmt::format("Cleanup: File {} not found for deletion", path));
    }
}

} // namespace infinity
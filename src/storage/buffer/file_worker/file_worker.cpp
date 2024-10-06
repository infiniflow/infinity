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

#include <tuple>

module file_worker;

import stl;
import utility;
import infinity_exception;
import local_file_handle;
import third_party;
import defer_op;
import status;
import virtual_store;
import persistence_manager;
import infinity_context;
import logger;
import persist_result_handler;
import abstract_file_handle;

namespace infinity {

FileWorker::~FileWorker() = default;

bool FileWorker::WriteToFile(bool to_spill, const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        String error_message = "No data will be written.";
        UnrecoverableError(error_message);
    }

    if(persistence_manager_ != nullptr && !to_spill) {
        String write_dir = *file_dir_;
        String write_path = Path(*data_dir_) / write_dir / *file_name_;
        String tmp_write_path = Path(*temp_dir_) / StringTransform(write_path, "/", "_");

        auto [file_handle, status] = LocalStore::Open(tmp_write_path, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle_ = std::move(file_handle);
        bool prepare_success = false;

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            file_handle_->Sync();
        }

        file_handle_->Sync();

        PersistResultHandler handler(persistence_manager_);
        PersistWriteResult persist_result = persistence_manager_->Persist(write_path, tmp_write_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;

        return all_save;
    } else {
        String write_dir = ChooseFileDir(to_spill);
        if (!LocalStore::Exists(write_dir)) {
            LocalStore::MakeDirectory(write_dir);
        }
        String write_path = fmt::format("{}/{}", write_dir, *file_name_);

        auto [file_handle, status] = LocalStore::Open(write_path, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle_ = std::move(file_handle);

        if (to_spill) {
            LOG_TRACE(fmt::format("Open spill file: {}, fd: {}", write_path, file_handle_->FileDescriptor()));
        }
        bool prepare_success = false;

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            if (to_spill) {
                LOG_TRACE(fmt::format("Write to spill file {} finished. success {}", write_path, prepare_success));
            }
            file_handle_->Sync();
        }
        return all_save;
    }
}

void FileWorker::ReadFromFile(bool from_spill) {
    bool use_object_cache = !from_spill && persistence_manager_ != nullptr;
    PersistResultHandler handler;
    if (use_object_cache) {
        handler = PersistResultHandler(persistence_manager_);
    }
    String read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    if (use_object_cache) {
        PersistReadResult result = persistence_manager_->GetObjCache(read_path);
        obj_addr_ = handler.HandleReadResult(result);
        if (!obj_addr_.Valid()) {
            String error_message = fmt::format("Failed to find object for local path {}", read_path);
            UnrecoverableError(error_message);
        }
        read_path = persistence_manager_->GetObjPath(obj_addr_.obj_key_);
    }
    SizeT file_size = 0;
    auto [file_handle, status] = LocalStore::Open(read_path, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    if (use_object_cache) {
        file_handle->Seek(obj_addr_.part_offset_);
        file_size = obj_addr_.part_size_;
    } else {
        file_size = file_handle->FileSize();
    }
    file_handle_ = std::move(file_handle);
    DeferFn defer_fn([&]() {
        if (use_object_cache && obj_addr_.Valid()) {
            String read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
            PersistWriteResult res = persistence_manager_->PutObjCache(read_path);
            handler.HandleWriteResult(res);
        }
    });
    ReadFromFileImpl(file_size);
}

void FileWorker::MoveFile() {
    String src_path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    String dest_dir = ChooseFileDir(false);
    String dest_path = fmt::format("{}/{}", dest_dir, *file_name_);
    if (persistence_manager_ == nullptr) {
        if (!LocalStore::Exists(src_path)) {
            Status status = Status::FileNotFound(src_path);
            RecoverableError(status);
        }
        if (!LocalStore::Exists(dest_dir)) {
            LocalStore::MakeDirectory(dest_dir);
        }
        // if (fs.Exists(dest_path)) {
        //     UnrecoverableError(fmt::format("File {} was already been created before.", dest_path));
        // }
        LocalStore::Rename(src_path, dest_path);
    } else {
        PersistResultHandler handler(persistence_manager_);
        PersistWriteResult persist_result = persistence_manager_->Persist(dest_path, src_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;
    }
}

// Get absolute file path. As key of buffer handle.
String FileWorker::GetFilePath() const { return Path(*data_dir_) / *file_dir_ / *file_name_; }

String FileWorker::ChooseFileDir(bool spill) const {
    return spill ? (Path(*temp_dir_) / *file_dir_)
                 : (Path(*data_dir_) / *file_dir_);
}

void FileWorker::CleanupFile() const {
    if (persistence_manager_ != nullptr) {
        PersistResultHandler handler(persistence_manager_);
        String path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
        PersistWriteResult result = persistence_manager_->Cleanup(path);
        handler.HandleWriteResult(result);
        return;
    }

    String path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
    if (LocalStore::Exists(path)) {
        LocalStore::DeleteFile(path);
        LOG_INFO(fmt::format("Cleaned file: {}", path));
    }
}

void FileWorker::CleanupTempFile() const {
    String path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    if (LocalStore::Exists(path)) {
        LocalStore::DeleteFile(path);
        LOG_INFO(fmt::format("Cleaned file: {}", path));
    } else {
        String error_message = fmt::format("Cleanup: File {} not found for deletion", path);
        UnrecoverableError(error_message);
    }
}
} // namespace infinity
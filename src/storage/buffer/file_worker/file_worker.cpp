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

module file_worker;

import stl;
import utility;
import infinity_exception;
import local_file_system;
import third_party;
import file_system_type;
import defer_op;
import status;
import local_file_system;
import persistence_manager;
import infinity_context;
import logger;

namespace infinity {

FileWorker::~FileWorker() = default;

bool FileWorker::WriteToFile(bool to_spill, const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        String error_message = "No data will be written.";
        UnrecoverableError(error_message);
    }

    LocalFileSystem fs;

    if(persistence_manager_ != nullptr && !to_spill) {
        String write_dir = *file_dir_;
        String write_path = Path(*data_dir_) / write_dir / *file_name_;
        String tmp_write_path = Path(*temp_dir_) / StringTransform(write_path, "/", "_");

        u8 flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        auto [file_handler, status] = fs.OpenFile(tmp_write_path, flags, FileLockType::kWriteLock);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handler_ = std::move(file_handler);
        bool prepare_success = false;

        DeferFn defer_fn([&]() {
            fs.Close(*file_handler_);
            file_handler_ = nullptr;
        });

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            fs.SyncFile(*file_handler_);
        }

        fs.SyncFile(*file_handler_);
        obj_addr_ = persistence_manager_->Persist(write_path, tmp_write_path);
        fs.DeleteFile(tmp_write_path);
        return all_save;
    } else {
        String write_dir = ChooseFileDir(to_spill);
        if (!fs.Exists(write_dir)) {
            fs.CreateDirectory(write_dir);
        }
        String write_path = fmt::format("{}/{}", write_dir, *file_name_);

        u8 flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        auto [file_handler, status] = fs.OpenFile(write_path, flags, FileLockType::kWriteLock);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handler_ = std::move(file_handler);

        if (to_spill) {
            auto local_file_handle = static_cast<LocalFileHandler *>(file_handler_.get());
            LOG_TRACE(fmt::format("Open spill file: {}, fd: {}", write_path, local_file_handle->fd_));
        }
        bool prepare_success = false;

        DeferFn defer_fn([&]() {
            fs.Close(*file_handler_);
            file_handler_ = nullptr;
        });

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            if (to_spill) {
                LOG_TRACE(fmt::format("Write to spill file {} finished. success {}", write_path, prepare_success));
            }
            fs.SyncFile(*file_handler_);
        }
        return all_save;
    }
}

void FileWorker::ReadFromFile(bool from_spill) {
    LocalFileSystem fs;
    String read_path;
    bool use_object_cache = !from_spill && persistence_manager_ != nullptr;
    read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    if (use_object_cache) {
        obj_addr_ = persistence_manager_->GetObjCache(read_path);
        if (!obj_addr_.Valid()) {
            String error_message = fmt::format("Failed to find object for local path {}", read_path);
            UnrecoverableError(error_message);
        }
        read_path = persistence_manager_->GetObjPath(obj_addr_.obj_key_);
    }
    SizeT file_size = 0;
    u8 flags = FileFlags::READ_FLAG;
    auto [file_handler, status] = fs.OpenFile(read_path, flags, FileLockType::kReadLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    if (use_object_cache) {
        fs.Seek(*file_handler, obj_addr_.part_offset_);
        file_size = obj_addr_.part_size_;
    } else {
        file_size = fs.GetFileSize(*file_handler);
    }
    file_handler_ = std::move(file_handler);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
        if (use_object_cache && obj_addr_.Valid()) {
            read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
            persistence_manager_->PutObjCache(read_path);
        }
    });
    ReadFromFileImpl(file_size);
}

void FileWorker::MoveFile() {
    LocalFileSystem fs;

    String src_path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    String dest_dir = ChooseFileDir(false);
    String dest_path = fmt::format("{}/{}", dest_dir, *file_name_);
    if (persistence_manager_ == nullptr) {
        if (!fs.Exists(src_path)) {
            Status status = Status::FileNotFound(src_path);
            RecoverableError(status);
        }
        if (!fs.Exists(dest_dir)) {
            fs.CreateDirectory(dest_dir);
        }
        // if (fs.Exists(dest_path)) {
        //     UnrecoverableError(fmt::format("File {} was already been created before.", dest_path));
        // }
        fs.Rename(src_path, dest_path);
    } else {
        obj_addr_ = persistence_manager_->Persist(dest_path, src_path);
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
        String path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
        persistence_manager_->Cleanup(path);
        return;
    }
    LocalFileSystem fs;

    String path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
    if (fs.Exists(path)) {
        fs.DeleteFile(path);
        LOG_INFO(fmt::format("Cleaned file: {}", path));
    }
}

void FileWorker::CleanupTempFile() const {
    LocalFileSystem fs;
    String path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    if (fs.Exists(path)) {
        fs.DeleteFile(path);
        LOG_INFO(fmt::format("Cleaned file: {}", path));
    } else {
        String error_message = fmt::format("Cleanup: File {} not found for deletion", path);
        UnrecoverableError(error_message);
    }
}
} // namespace infinity
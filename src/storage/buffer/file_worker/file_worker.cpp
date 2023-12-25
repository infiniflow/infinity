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

import stl;

import infinity_exception;
import local_file_system;
import third_party;
import file_system_type;
import defer_op;

import local_file_system;
import logger;

module file_worker;

namespace infinity {

FileWorker::~FileWorker() = default;

void FileWorker::Sync() {
    LocalFileSystem fs;
    fs.SyncFile(*file_handler_);
}

void FileWorker::CloseFile() {
    LocalFileSystem fs;
    fs.Close(*file_handler_);
}

void FileWorker::WriteToFile(bool to_spill) {
    if (data_ == nullptr) {
        Error<StorageException>("No data will be written.");
    }
    LocalFileSystem fs;

    String write_dir = ChooseFileDir(to_spill);
    if (!fs.Exists(write_dir)) {
        fs.CreateDirectory(write_dir);
    }
    String write_path = Format("{}/{}", write_dir, *file_name_);

    u8 flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
    file_handler_ = fs.OpenFile(write_path, flags, FileLockType::kWriteLock);
    if (to_spill) {
        auto local_file_handle_ = static_cast<LocalFileHandler *>(file_handler_.get());
        LOG_WARN(Format("Open spill file: {}, fd: {}", write_path, local_file_handle_->fd_));
    }
    bool prepare_success = false;
    DeferFn defer_fn([&]() {
        if (to_spill) {
            LOG_WARN(Format("Write to spill file {} finished. success {}", write_path, prepare_success));
        }
        if (!prepare_success) {
            file_handler_->Close();
            file_handler_ = nullptr;
        }
    });
    WriteToFileImpl(prepare_success);
}

void FileWorker::ReadFromFile(bool from_spill) {
    LocalFileSystem fs;

    String read_path = Format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    u8 flags = FileFlags::READ_FLAG;
    file_handler_ = fs.OpenFile(read_path, flags, FileLockType::kReadLock);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
    });
    ReadFromFileImpl();
}

void FileWorker::MoveFile() {
    LocalFileSystem fs;

    String src_path = Format("{}/{}", ChooseFileDir(true), *file_name_);
    String dest_dir = ChooseFileDir(false);
    String dest_path = Format("{}/{}", dest_dir, *file_name_);
    if (!fs.Exists(src_path)) {
        Error<StorageException>(Format("File {} doesn't exist.", src_path));
    }
    if (!fs.Exists(dest_dir)) {
        fs.CreateDirectory(dest_dir);
    }
    // if (fs.Exists(dest_path)) {
    //     Error<StorageException>(Format("File {} was already been created before.", dest_path));
    // }
    fs.Rename(src_path, dest_path);
}

} // namespace infinity
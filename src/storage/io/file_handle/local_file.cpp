// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>

module local_file;

import third_party;
import status;
import virtual_storage;
import infinity_exception;

namespace infinity {

LocalFile::LocalFile(VirtualStorage *storage_system) : AbstractFileHandle(storage_system, StorageType::kLocal) {}

LocalFile::~LocalFile() = default;

Status LocalFile::Open(const String &path, FileAccessMode access_mode) {
    if(!path_.empty()) {
        return Status::FileIsOpen(path);
    }
    path_ = path;
    access_mode_ = access_mode;
    switch (access_mode_) {
        case FileAccessMode::kRead: {
            fd_ = open(path.c_str(), O_RDONLY, 0666);
            open_ = true;
            break;
        }
        case FileAccessMode::kWrite: {
            break;
        }
        case FileAccessMode::kMmapRead: {
            break;
        }
        case FileAccessMode::kInvalid: {
            break;
        }
    }
    return Status::OK();
}

Status LocalFile::Close() {
    if(access_mode_ == FileAccessMode::kWrite) {
        if(!sync_) {
            Status status = Sync();
            if(!status.ok()) {
                return status;
            }
        }
    }
    if(open_) {
        close(fd_);
        fd_ = -1;
        open_ = false;
    }
    return Status::OK();
}

Status LocalFile::Append(const char *buffer, u64 nbytes) { return Status::OK(); }

Status LocalFile::Append(const String &buffer, u64 nbytes) { return Status::OK(); }

Tuple<SizeT, Status> LocalFile::Read(char *buffer, u64 nbytes) {

    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        SizeT a = nbytes - read_n;
        i64 read_count = read(fd_, (char *)buffer + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            UnrecoverableError(error_message);
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

Tuple<SizeT, Status> LocalFile::Read(String &buffer, u64 nbytes) {
    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        SizeT a = nbytes - read_n;
        i64 read_count = read(fd_, buffer.data() + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            UnrecoverableError(error_message);
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

SizeT LocalFile::FileSize() {
    struct stat s {};
    if (fstat(fd_, &s) == -1) {
        return -1;
    }
    return s.st_size;
}

Tuple<char *, SizeT, Status> LocalFile::MmapRead(const String &name) { return {nullptr, 0, Status::OK()}; }

Status LocalFile::Unmmap(const String &name) { return Status::OK(); }

Status LocalFile::Sync() {
    if(access_mode_ != FileAccessMode::kWrite) {
        return Status::InvalidCommand("Non-write access mode, shouldn't call Sync()");
    }
    if(!sync_) {
        sync_ = true;
        fsync(fd_);
    }
    return Status::OK();
}

} // namespace infinity

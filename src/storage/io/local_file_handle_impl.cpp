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

#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

module infinity_core:local_file_handle.impl;

import :local_file_handle;
import :status;
import :virtual_store;
import :infinity_exception;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

LocalFileHandle::~LocalFileHandle() {
    Status status = Sync();
    if (!status.ok()) {
        return;
    }

    if (fd_ == -1) {
        UnrecoverableError(fmt::format("File was closed before or not open"));
    }

    i32 ret = close(fd_);
    if (ret == -1) {
        UnrecoverableError(fmt::format("Close file: {}, error: {}", path_, strerror(errno)));
    }

    fd_ = -1;
    path_.clear();
    access_mode_ = FileAccessMode::kInvalid;
}

Status LocalFileHandle::Close() {
    Status status = Sync();
    if (!status.ok()) {
        return status;
    }

    if (fd_ == -1) {
        UnrecoverableError(fmt::format("File was closed before"));
    }

    if (i32 ret = close(fd_); ret == -1) {
        UnrecoverableError(fmt::format("Close file: {}, error: {}", path_, strerror(errno)));
    }

    fd_ = -1;
    path_.clear();
    access_mode_ = FileAccessMode::kInvalid;
    return Status::OK();
}

Status LocalFileHandle::Append(const void *buffer, u64 nbytes) {
    if (access_mode_ != FileAccessMode::kWrite) {
        UnrecoverableError(fmt::format("File: {} isn't open.", path_));
    }
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = write(fd_, (char *)buffer + written, nbytes - written);
        if (write_count == -1) {
            UnrecoverableError(fmt::format("Can't write file: {}: {}. fd: {}", path_, strerror(errno), fd_));
        }
        written += write_count;
    }
    return Status::OK();
}

Status LocalFileHandle::Append(const std::string &buffer, u64 nbytes) { return Append(buffer.data(), nbytes); }

std::tuple<size_t, Status> LocalFileHandle::Read(void *buffer, u64 nbytes) {
    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        size_t a = nbytes - read_n;
        i64 read_count = read(fd_, (char *)buffer + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            UnrecoverableError(fmt::format("Can't read file: {}: {}", path_, strerror(errno)));
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

std::tuple<size_t, Status> LocalFileHandle::Read(std::string &buffer, u64 nbytes) {
    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        size_t a = nbytes - read_n;
        i64 read_count = read(fd_, buffer.data() + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            UnrecoverableError(fmt::format("Can't read file: {}: {}", path_, strerror(errno)));
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

Status LocalFileHandle::Seek(u64 nbytes) {
    if ((off_t)-1 == lseek(fd_, nbytes, SEEK_SET)) {
        UnrecoverableError(fmt::format("Can't seek file: {}: {}", path_, strerror(errno)));
    }
    return Status::OK();
}

i64 LocalFileHandle::FileSize() {
    struct stat s{};
    if (fstat(fd_, &s) == -1) {
        return -1;
    }
    return s.st_size;
}

std::tuple<char *, size_t, Status> LocalFileHandle::MmapRead(const std::string &name) { return {nullptr, 0, Status::OK()}; }

Status LocalFileHandle::Unmmap(const std::string &name) { return Status::OK(); }

Status LocalFileHandle::Sync() {
    if (access_mode_ != FileAccessMode::kWrite) {
        return Status::OK();
    }

    fsync(fd_);
    return Status::OK();
}

} // namespace infinity

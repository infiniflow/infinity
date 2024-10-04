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

module local_file_handle;

import third_party;
import status;
import virtual_store;
import infinity_exception;
import logger;
import abstract_file_handle;

namespace infinity {

LocalFileHandle::~LocalFileHandle() {
    Status status = Sync();
    if(!status.ok()) {
        return ;
    }

    if(fd_ == -1) {
        String error_message = fmt::format("File was closed before or not open");
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    i32 ret = close(fd_);
    if(ret == -1) {
        String error_message = fmt::format("Close file: {}, error: {}", path_, strerror(errno));
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    fd_ = -1;
    path_.clear();
    access_mode_ = FileAccessMode::kInvalid;
}

Status LocalFileHandle::Close() {
    Status status = Sync();
    if(!status.ok()) {
        return status;
    }

    if(fd_ == -1) {
        String error_message = fmt::format("File was closed before");
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    i32 ret = close(fd_);
    if(ret == -1) {
        String error_message = fmt::format("Close file: {}, error: {}", path_, strerror(errno));
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    fd_ = -1;
    path_.clear();
    access_mode_ = FileAccessMode::kInvalid;
    return Status::OK();
}

Status LocalFileHandle::Append(const void *buffer, u64 nbytes) {
    if(access_mode_ != FileAccessMode::kWrite) {
        String error_message = fmt::format("File: {} isn't open.", path_);
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = write(fd_, (char*)buffer + written, nbytes - written);
        if (write_count == -1) {
            String error_message = fmt::format("Can't write file: {}: {}. fd: {}", path_, strerror(errno), fd_);
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        written += write_count;
    }
    return Status::OK();
}

Status LocalFileHandle::Append(const String &buffer, u64 nbytes) {
    return Append(buffer.data(), nbytes);
}

Tuple<SizeT, Status> LocalFileHandle::Read(void *buffer, u64 nbytes) {
    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        SizeT a = nbytes - read_n;
        i64 read_count = read(fd_, (char *)buffer + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

Tuple<SizeT, Status> LocalFileHandle::Read(String &buffer, u64 nbytes) {
    i64 read_n = 0;
    while (read_n < (i64)nbytes) {
        SizeT a = nbytes - read_n;
        i64 read_count = read(fd_, buffer.data() + read_n, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        read_n += read_count;
    }
    return {read_n, Status::OK()};
}

Status LocalFileHandle::Seek(u64 nbytes) {
    if ((off_t)-1 == lseek(fd_, nbytes, SEEK_SET)) {
        String error_message = fmt::format("Can't seek file: {}: {}", path_, strerror(errno));
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

i64 LocalFileHandle::FileSize() {
    struct stat s {};
    if (fstat(fd_, &s) == -1) {
        return -1;
    }
    return s.st_size;
}

Tuple<char *, SizeT, Status> LocalFileHandle::MmapRead(const String &name) { return {nullptr, 0, Status::OK()}; }

Status LocalFileHandle::Unmmap(const String &name) { return Status::OK(); }

Status LocalFileHandle::Sync() {
    if(access_mode_ != FileAccessMode::kWrite) {
        return Status::OK();
    }

    fsync(fd_);
    return Status::OK();
}

} // namespace infinity

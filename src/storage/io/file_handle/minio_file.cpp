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

#include <cassert>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

import stl;
import status;
import virtual_storage_system;
import third_party;
import infinity_context;
import abstract_file_handle;

module minio_file;

namespace infinity {

namespace fs = std::filesystem;

std::mutex MinioFile::mtx_{};
HashMap<String, MinoMmapInfo> MinioFile::mapped_files_{};

MinioFile::MinioFile(VirtualStorageSystem *storage_system)
    : ObjectFile(storage_system, StorageType::kMinio) {}

MinioFile::~MinioFile() = default;

Status MinioFile::Open(const String &path, FileAccessMode access_mode) {
    String bucket_name = InfinityContext::instance().config()->ObjectStorageBucket();

    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        return Status::SyntaxError(error_message);
    }

    if(access_mode != FileAccessMode::kWrite && !std::filesystem::exists(path)){
        minio::s3::Client * client = storage_system_->GetMinioClient();
        // Create download object arguments.
        minio::s3::DownloadObjectArgs args;
        args.bucket = bucket_name;
        args.object = path;
        args.filename = path;
        
        // Call download object.
        minio::s3::DownloadObjectResponse resp = client->DownloadObject(args);

        // Handle response.
        if (!resp) {
            return Status::IOError(resp.Error().String());
        }
    }

    switch (access_mode)
    {
        case FileAccessMode::kRead:
            fd_ = open(path.c_str(), O_RDONLY, 0666);
            if (fd_ == -1) {
                return Status::IOError(fmt::format("Can't open file: {}: {}", path, strerror(errno)));
            }
            break;
        
        case FileAccessMode::kMmapRead:
            return std::get<2>(MmapRead(path));
            break;

        case FileAccessMode::kWrite:
            fd_ = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (fd_ == -1) {
                return Status::IOError(fmt::format("Can't open file: {}: {}", path, strerror(errno)));
            }
            break;
        default:
            return Status::IOError("Unsupported access mode for s3 storage.");
            break;
    }

    open_ = true;
    path_ = path;
    access_mode_ = access_mode;
    return Status::OK();
}

Status MinioFile::Close() { 
    i32 fd = fd_;
    fd_ = -1;

    if (close(fd) != 0) {
        String error_message = fmt::format("Can't close file: {}: {}", path_, strerror(errno));
        return Status::IOError(error_message);
    }

    open_ = false;
    return Status::OK(); 
}

Status MinioFile::Append(const char *buffer, u64 nbytes) { 
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = write(fd_, (char *)buffer + written, nbytes - written);
        if (write_count == -1) {
            String error_message = fmt::format("Can't write file: {}: {}. fd: {}", path_, strerror(errno), fd_);
            return Status::IOError(error_message);
        }
        written += write_count;
    }

    return Status::OK(); 
}

Status MinioFile::Append(const String &buffer, u64 nbytes) { 
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = write(fd_, (char *)buffer.c_str() + written, nbytes - written);
        if (write_count == -1) {
            String error_message = fmt::format("Can't write file: {}: {}. fd: {}", path_, strerror(errno), fd_);
            return Status::IOError(error_message);
        }
        written += write_count;
    }
    return Status::OK(); 
}

Tuple<SizeT, Status> MinioFile::Read(char *buffer, u64 nbytes) { 
    i64 readen = 0;
    while (readen < (i64)nbytes) {
        SizeT a = nbytes - readen;
        i64 read_count = read(fd_, buffer + readen, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            return {0, Status::IOError(error_message)};
        }
        readen += read_count;
    }

    return {readen, Status::OK()}; 
}

Tuple<SizeT, Status> MinioFile::Read(String &buffer, u64 nbytes) { 
    i64 readen = 0;
    char *helper_buffer = new char[nbytes];
    while (readen < (i64)nbytes) {
        SizeT a = nbytes - readen;
        i64 read_count = read(fd_, helper_buffer + readen, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", path_, strerror(errno));
            delete [] helper_buffer;
            return {0, Status::IOError(error_message)};
        }
        readen += read_count;
    }
    buffer = String(std::move(helper_buffer), readen);
    return {readen, Status::OK()}; 
}

SizeT MinioFile::FileSize() { 
    return std::filesystem::file_size(path_);
}

Tuple<char *, SizeT, Status> MinioFile::MmapRead(const String &file_path) { 
    if (!std::filesystem::path(file_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_path);
        return {nullptr, 0, Status::SyntaxError(error_message)}; 
    }

    char *data_ptr = nullptr;
    SizeT data_len = 0;

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = mapped_files_.find(file_path);
    if (it != mapped_files_.end()) {
        auto &mmap_info = it->second;
        data_ptr = mmap_info.data_ptr_;
        data_len = mmap_info.data_len_;
        mmap_info.rc_++;
        return {data_ptr, data_len, Status::OK()};
    }
    long len_f = fs::file_size(file_path);
    if (len_f == 0)
        return {nullptr, 0, Status::IOError("MAP_FILE_SIZE_ZERO")};
    int f = open(file_path.c_str(), O_RDONLY);
    void *tmpd = mmap(NULL, len_f, PROT_READ, MAP_SHARED, f, 0);
    if (tmpd == MAP_FAILED)
        return {nullptr, 0, Status::IOError("MAP_FAILED")};
    close(f);
    int rc = madvise(tmpd,
                     len_f,
                     MADV_NORMAL
#if defined(linux) || defined(__linux) || defined(__linux__)
                         | MADV_DONTDUMP
#endif
    );
    if (rc < 0)
        return {nullptr, 0, Status::IOError("MAP_MADVISE_FAILED")};
    data_ptr = (char *)tmpd;
    data_len = len_f;
    mapped_files_.emplace(file_path, MinoMmapInfo{data_ptr, data_len, 1});

    path_ = file_path;
    access_mode_ = FileAccessMode::kMmapRead;
    return {data_ptr, data_len, Status::OK()}; 
}

Status MinioFile::Unmmap(const String &file_path) { 
        if(!std::filesystem::path(file_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_path);
        return Status::SyntaxError(error_message);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = mapped_files_.find(file_path);
    if (it == mapped_files_.end()) {
        return Status::IOError("MAP_FILE_NOT_FOUND");
    }
    auto &mmap_info = it->second;
    mmap_info.rc_--;
    if (mmap_info.rc_ == 0) {
        munmap(mmap_info.data_ptr_, mmap_info.data_len_);
        mapped_files_.erase(it);
    }

    return Status::OK(); 
}

Status MinioFile::Sync() {
    if (fsync(fd_) != 0) {
        String error_message = fmt::format("fsync failed: {}, {}", path_, strerror(errno));
        return Status::IOError(error_message);
    }

    String bucket_name = InfinityContext::instance().config()->ObjectStorageBucket();
    minio::s3::Client * client = storage_system_->GetMinioClient();
    // Create upload object arguments.
    minio::s3::UploadObjectArgs args;
    args.bucket = bucket_name;
    args.object = path_;
    args.filename = path_;

    // Call upload object.
    minio::s3::UploadObjectResponse resp = client->UploadObject(args);

    // Handle response.
    if (!resp) {
        return Status::IOError(resp.Error().String());
    } 

    return Status::OK();
}

} 
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

std::mutex MinioFile::mtx_{};

MinioFile::MinioFile(VirtualStorageSystem *storage_system)
    : ObjectFile(storage_system, StorageType::kMinio) {}

MinioFile::~MinioFile() = default;

Status MinioFile::Open(const String &path, FileAccessMode access_mode) {
    String bucket_name = InfinityContext::instance().config()->ObjectStorageBucket();

    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        return Status::SyntaxError(error_message);
    }

    if(!std::filesystem::exists(path)){
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
            open_ = true;
            path_ = path;
            access_mode_ = access_mode;
            break;
        
        case FileAccessMode::kMmapRead:
            return std::get<2>(MmapRead(path));
            break;

        default:
            return Status::IOError("Unsupported access mode for s3 storage.");
            break;
    }

    return Status::OK();
}

Status MinioFile::Close() { 
    i32 fd = fd_;
    fd_ = -1;

    if (close(fd) != 0) {
        String error_message = fmt::format("Can't close file: {}: {}", path_, strerror(errno));
        return Status::IOError(error_message);
    }

    return Status::OK(); 
}

Status MinioFile::Append(const char *buffer) { return Status::OK(); }

Status MinioFile::Append(const String &buffer) { return Status::OK(); }

Tuple<SizeT, Status> MinioFile::Read(char *buffer) { return {0, Status::OK()}; }

Tuple<SizeT, Status> MinioFile::Read(String &buffer) { return {0, Status::OK()}; }

SizeT MinioFile::FileSize() { return 0; }

Tuple<char *, SizeT, Status> MinioFile::MmapRead(const String &file_path) { 
    if (!std::filesystem::path(file_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_path);
        return {nullptr, 0, Status::SyntaxError(error_message)}; 
    }

    char *data_ptr = nullptr;
    SizeT data_len = 0;

    return {data_ptr, data_len, Status::OK()}; 
}

Status MinioFile::Unmmap(const String &name) { return Status::OK(); }

} 
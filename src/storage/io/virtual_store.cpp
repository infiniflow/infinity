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

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>

module virtual_store;

import stl;
import third_party;
import virtual_storage_type;
import logger;
import infinity_exception;
import default_values;
import abstract_file_handle;

namespace infinity {

Status RemoteStore::Init(StorageType storage_type, Map<String, String> &config) {
    // Init remote filesystem and local disk cache
    storage_type_ = storage_type;
    switch (storage_type) {
        case StorageType::kMinio: {
            auto iter = config.find("url");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'URL'");
            }
            String url = iter->second;

            iter = config.find("access_key");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'access_key'");
            }
            String access_key = iter->second;

            iter = config.find("secret_key");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'secret_key'");
            }
            String secret_key = iter->second;

            iter = config.find("enable_https");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'enable_https'");
            }
            String enable_https_str = iter->second;
            bool enable_https{false};
            if (enable_https_str == "true") {
                enable_https = true;
            } else if (enable_https_str == "false") {
                enable_https = false;
            } else {
                return Status::InvalidConfig(fmt::format("Invalid MINIO 'enable_https' value: {}", enable_https_str));
            }

            minio_base_url_ = MakeUnique<minio::s3::BaseUrl>(url, enable_https);
            minio_provider_ = MakeUnique<minio::creds::StaticProvider>(access_key, secret_key);
            minio_client_ = MakeUnique<minio::s3::Client>(*minio_base_url_, minio_provider_.get());
            break;
        }
        default: {
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type)));
        }
    }
    return Status::OK();
}

Status RemoteStore::UnInit() {
    switch (storage_type_) {
        case StorageType::kMinio: {
            minio_base_url_.reset();
            minio_provider_.reset();
            minio_client_.reset();
            break;
        }
        default: {
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type_)));
        }
    }
    return Status::OK();
}

Tuple<UniquePtr<LocalFileHandle>, Status> LocalStore::Open(const String& path, FileAccessMode access_mode) {
    i32 fd = -1;
    switch (access_mode) {
        case FileAccessMode::kRead: {
            fd = open(path.c_str(), O_RDONLY, 0666);
            break;
        }
        case FileAccessMode::kWrite: {
            fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
            break;
        }
        case FileAccessMode::kMmapRead: {
            UnrecoverableError("Unsupported now.");
            break;
        }
        case FileAccessMode::kInvalid: {
            break;
        }
    }
    if(fd == -1) {
        String error_message = fmt::format("File open failed: {}", strerror(errno));
        return {nullptr, Status::IOError(error_message)};
    }
    return {MakeUnique<LocalFileHandle>(fd, path, access_mode), Status::OK()};
}


// For local disk filesystem, such as temp file, disk cache and WAL
bool LocalStore::Exists(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    bool is_exists = std::filesystem::exists(p, error_code);
    if (error_code.value() == 0) {
        return is_exists;
    } else {
        String error_message = fmt::format("{} exists exception: {}", path, strerror(errno));
        UnrecoverableError(error_message);
    }
    return false;
}

Status LocalStore::DeleteFile(const String &file_name) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{file_name};
    bool is_deleted = std::filesystem::remove(p, error_code);
    if (error_code.value() == 0) {
        if (!is_deleted) {
            String error_message = fmt::format("Failed to delete file: {}: {}", file_name, strerror(errno));
            LOG_WARN(error_message);
            Status status = Status::IOError(error_message);
            return status;
        }
    } else {
        String error_message = fmt::format("Delete file {} exception: {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::MakeDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    std::filesystem::create_directories(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("{} create exception: {}", path, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::RemoveDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    std::filesystem::remove_all(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Delete directory {} exception: {}", path, error_code.message());
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::CleanupDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    if (!std::filesystem::exists(p)) {
        std::filesystem::create_directories(p, error_code);
        if (error_code.value() != 0) {
            String error_message = fmt::format("CleanupDirectory create {} exception: {}", path, error_code.message());
            UnrecoverableError(error_message);
        }
        return Status::OK();
    }
    try {
        std::ranges::for_each(std::filesystem::directory_iterator{path}, [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry); });
    } catch (const std::filesystem::filesystem_error &e) {
        String error_message = fmt::format("CleanupDirectory cleanup {} exception: {}", path, e.what());
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::Rename(const String &old_path, const String &new_path) {
    if (!std::filesystem::path(old_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", old_path);
        UnrecoverableError(error_message);
    }
    if (!std::filesystem::path(new_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", new_path);
        UnrecoverableError(error_message);
    }
    if (rename(old_path.c_str(), new_path.c_str()) != 0) {
        String error_message = fmt::format("Can't rename file: {}, {}", old_path, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::Truncate(const String &file_name, SizeT new_length) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    std::filesystem::resize_file(file_name, new_length, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Failed to truncate {} to size {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::Merge(const String &dst_path, const String &src_path) {
    if (!std::filesystem::path(dst_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", dst_path);
        UnrecoverableError(error_message);
    }
    if (!std::filesystem::path(src_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", src_path);
        UnrecoverableError(error_message);
    }
    Path dst{dst_path};
    Path src{src_path};
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile.is_open()) {
        String error_message = fmt::format("Failed to open source file {}", src_path);
        UnrecoverableError(error_message);
        return Status::OK();
    }
    std::ofstream dstFile(dst, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        String error_message = fmt::format("Failed to open destination file {}", dst_path);
        UnrecoverableError(error_message);
        return Status::OK();
    }
    char buffer[DEFAULT_READ_BUFFER_SIZE];
    while (srcFile.read(buffer, DEFAULT_READ_BUFFER_SIZE)) {
        dstFile.write(buffer, srcFile.gcount());
    }
    dstFile.write(buffer, srcFile.gcount());
    srcFile.close();
    dstFile.close();
    return Status::OK();
}

Tuple<Vector<SharedPtr<DirEntry>>, Status> LocalStore::ListDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    Path dir_path(path);
    if (!is_directory(dir_path)) {
        String error_message = fmt::format("{} isn't a directory", path);
        UnrecoverableError(error_message);
    }

    Vector<SharedPtr<DirEntry>> file_array;
    std::ranges::for_each(std::filesystem::directory_iterator{path},
                          [&](const auto &dir_entry) { file_array.emplace_back(MakeShared<DirEntry>(dir_entry)); });
    return {file_array, Status::OK()};
}

SizeT LocalStore::GetFileSize(const String& path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    return std::filesystem::file_size(path);
}

} // namespace infinity

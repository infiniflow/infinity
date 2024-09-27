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

module virtual_storage;

import stl;
import third_party;
import virtual_storage_type;
import logger;
import local_file;
import minio_file;
import infinity_exception;
import default_values;

namespace infinity {

Status VirtualStorage::Init(StorageType storage_type, Map<String, String> &config) {
    // Init remote filesystem and local disk cache
    storage_type_ = storage_type;
    switch (storage_type) {
        case StorageType::kLocal: {
            if (!config.empty()) {
                return Status::InvalidConfig("Local filesystem won't access any config");
            }
            break;
        }
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

            iter = config.find("disk_cache_dir");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'cache_dir'");
            }
            SharedPtr<String> cache_dir_ptr = MakeShared<String>(iter->second);

            iter = config.find("disk_cache_limit");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'cache_dir'");
            }
            String disk_cache_limit_str = iter->second;
            SizeT disk_cache_limit = std::stoull(disk_cache_limit_str);

            iter = config.find("disk_cache_lru_count");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'cache_dir'");
            }
            String disk_cache_lru_count_str = iter->second;
            SizeT disk_cache_lru_count = std::stoull(disk_cache_lru_count_str);

            minio_base_url_ = MakeUnique<minio::s3::BaseUrl>(url, enable_https);
            minio_provider_ = MakeUnique<minio::creds::StaticProvider>(access_key, secret_key);
            minio_client_ = MakeUnique<minio::s3::Client>(*minio_base_url_, minio_provider_.get());
            local_disk_cache_ = MakeUnique<LocalDiskCache>(disk_cache_limit, cache_dir_ptr, disk_cache_lru_count);
            break;
        }
        default: {
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type)));
        }
    }
    return Status::OK();
}

Status VirtualStorage::UnInit() {
    minio_client_.reset();
    minio_provider_.reset();
    minio_base_url_.reset();
    local_disk_cache_.reset();
    return Status::OK();
}

Tuple<UniquePtr<AbstractFileHandle>, Status> VirtualStorage::BuildFileHandle() {
    // Open the file according to the path and access_mode
    switch (storage_type_) {
        case StorageType::kLocal: {
            UniquePtr<LocalFile> local_file = MakeUnique<LocalFile>(this);
            return {std::move(local_file), Status::OK()};
        }
        case StorageType::kMinio: {
            UniquePtr<MinioFile> minio_file = MakeUnique<MinioFile>(this);
            return {std::move(minio_file), Status::OK()};
        }
        default: {
            Status status = Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type_)));
            LOG_ERROR(status.message());
            return {nullptr, status};
        }
    }
    return {nullptr, Status::OK()};
}

LocalDiskCache *VirtualStorage::GetLocalDiskCache() const { return local_disk_cache_.get(); }

Status VirtualStorage::DeleteFile(const String &file_name) {
    switch (storage_type_) {
        case StorageType::kLocal: {
            return DeleteLocalFile(file_name);
        }
        default: {
            Status status = Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type_)));
            LOG_ERROR(status.message());
            return status;
        }
    }
    return Status::OK();
}

bool VirtualStorage::Exists(const String &path) {
    switch (storage_type_) {
        case StorageType::kLocal: {
            return LocalExists(path);
        }
        default: {
            Status status = Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type_)));
            LOG_ERROR(status.message());
            return false;
        }
    }
    return false;
}

Tuple<Vector<String>, Status> VirtualStorage::ListDirectory(const String &path) {
    Vector<String> result;
    return {result, Status::OK()};
}

bool VirtualStorage::IsRegularFile(const String &path) { return false; }

// For local disk filesystem, such as temp file, disk cache and WAL
bool VirtualStorage::LocalExists(const String &path) {
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

Status VirtualStorage::DeleteLocalFile(const String &file_name) {
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

Status VirtualStorage::MakeLocalDirectory(const String &path) {
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

Status VirtualStorage::RemoveLocalDirectory(const String &path) {
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

Status VirtualStorage::CleanupLocalDirectory(const String &path) {
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

Status VirtualStorage::RenameLocal(const String &old_path, const String &new_path) {
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

Status VirtualStorage::TruncateLocal(const String& file_name, SizeT new_length) {
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

Status VirtualStorage::MergeLocal(const String& dst_path, const String& src_path) {
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

} // namespace infinity

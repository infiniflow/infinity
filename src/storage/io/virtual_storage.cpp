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

Status VirtualStorage::DeleteFile(const String &path) { return Status::OK(); }

Status VirtualStorage::Rename(const String &old_path, const String &new_path) { return Status::OK(); }

Status VirtualStorage::Exists(const String &path) { return Status::OK(); }

Tuple<Vector<String>, Status> VirtualStorage::ListDirectory(const String &path) {
    Vector<String> result;
    return {result, Status::OK()};
}

bool VirtualStorage::IsRegularFile(const String &path) { return false; }

// For local disk filesystem, such as temp file, disk cache and WAL
Status VirtualStorage::CreateLocalDirectory(const String &path) { return Status::OK(); }
Status VirtualStorage::DeleteLocalDirectory(const String &path) { return Status::OK(); }
Status VirtualStorage::CleanupLocalDirectory(const String &path) { return Status::OK(); }

} // namespace infinity

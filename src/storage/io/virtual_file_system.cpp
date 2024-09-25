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

module virtual_file_system;

import stl;
import third_party;
import virtual_file_system_type;

namespace infinity {

Status VirtualFileSystem::Init(FSType fs_type, Map<String, String> &config) {
    // Init remote filesystem and local disk cache
    fs_type_ = fs_type;
    switch (fs_type) {
        case FSType::kLocal: {
            if (!config.empty()) {
                return Status::InvalidConfig("Local filesystem won't access any config");
            }
            break;
        }
        case FSType::kMinio: {
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
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(fs_type)));
        }
    }
    return Status::OK();
}

Status VirtualFileSystem::UnInit() {
    minio_client_.reset();
    minio_provider_.reset();
    minio_base_url_.reset();
    local_disk_cache_.reset();
    return Status::OK();
}

UniquePtr<AbstractFileHandle> VirtualFileSystem::BuildFileHandle(const String &path, FileAccessMode access_mode) const {
    // Open the file according to the path and access_mode
    return nullptr;
}

LocalDiskCache *VirtualFileSystem::GetLocalDiskCache() const { return local_disk_cache_.get(); }

} // namespace infinity

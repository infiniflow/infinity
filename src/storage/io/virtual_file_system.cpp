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

import stl;

module virtual_file_system;

namespace infinity {

LocalDiskCache::LocalDiskCache(u64 disk_capacity_limit, SharedPtr<String> temp_dir, SizeT lru_count) {

}

LocalDiskCache::~LocalDiskCache() = default;

Status VirtualFileSystem::Init(FSType fs_type, Map<String, String>& config) {
    // Init remote filesystem and local disk cache
    return Status::OK();
}

Status VirtualFileSystem::UnInit() {
    return Status::OK();
}

UniquePtr<AbstractFileHandle> VirtualFileSystem::BuildFileHandle(const String &path, FileAccessMode access_mode) const {
    // Open the file according to the path and access_mode
    return nullptr;
}

LocalDiskCache* VirtualFileSystem::GetLocalDiskCache() const {
    return local_disk_cache_.get();
}

}

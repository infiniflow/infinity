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

export module virtual_file_system;

import stl;
import file_system_type;
import status;

namespace infinity {

export class VirtualFileSystem;

export enum class FSType {
    kLocal,
    kMinio,
    kAwsS3,
    kAzureBlob, // Azure object store
    kGCS,       // Google cloud storage
    kOSS,       // Aliyun OSS
    kCOS,       // Tencent object store
    kOBS,       // Huawei object store
    kHDFS,
    kNFS,
};

export enum class FileAccessMode { kWrite, kRead, kInvalid };

export class AbstractFileHandle {
public:
    virtual Status Close() = 0;
    virtual Status Append(const char *buffer) = 0;
    virtual Status Append(const String &buffer) = 0;
    virtual Tuple<SizeT, Status> Read(char *buffer) = 0;
    virtual Tuple<SizeT, Status> Read(String &buffer) = 0;
    virtual SizeT FileSize() = 0;

protected:
    VirtualFileSystem *file_system_{};
};

export class LocalFile : public AbstractFileHandle {
    Status Close() override;
    Status Append(const char *buffer) override;
    Status Append(const String &buffer) override;
    Tuple<SizeT, Status> Read(char *buffer) override;
    Tuple<SizeT, Status> Read(String &buffer) override;
    SizeT FileSize() override;
};

export class ObjectFile : public AbstractFileHandle {
    Status Close() override;
    Status Append(const char *buffer) override;
    Status Append(const String &buffer) override;
    Tuple<SizeT, Status> Read(char *buffer) override;
    Tuple<SizeT, Status> Read(String &buffer) override;
    SizeT FileSize() override;
};

export class LocalDiskCache {
public:
    explicit LocalDiskCache(u64 disk_capacity_limit, SharedPtr<String> temp_dir, SizeT lru_count);

    ~LocalDiskCache();

private:
    SharedPtr<String> temp_dir_{};
};

class VirtualFileSystem {
    // Factory class to generate different type of file handle
public:
    Status Init(FSType fs_type, Map<String, String> &config);
    Status UnInit();
    UniquePtr<AbstractFileHandle> BuildFileHandle(const String &path, FileAccessMode access_mode) const;
    LocalDiskCache *GetLocalDiskCache() const;

private:
    FSType fs_type_{FSType::kLocal};
    UniquePtr<LocalDiskCache> local_disk_cache_{};
};

} // namespace infinity

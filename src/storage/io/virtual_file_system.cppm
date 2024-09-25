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
import third_party;
import virtual_file_system_type;

namespace infinity {

export class VirtualFileSystem;

//export enum class FileType {
//    kLocal,
//    kMmap,
//    kObject
//};

export enum class FileAccessMode { kWrite, kRead, kMmapRead, kInvalid };

export class AbstractFileHandle {
public:
    AbstractFileHandle(VirtualFileSystem* file_system, FSType fs_type): file_system_(file_system), fs_type_(fs_type) {}
    virtual ~AbstractFileHandle() = 0;
    virtual Status Close() = 0;
    virtual Status Append(const char *buffer) = 0;
    virtual Status Append(const String &buffer) = 0;
    virtual Tuple<SizeT, Status> Read(char *buffer) = 0;
    virtual Tuple<SizeT, Status> Read(String &buffer) = 0;
    virtual Tuple<char*, SizeT, Status> MmapRead(const String &name) = 0;
    virtual Status Unmmap(const String &name) = 0;
    virtual SizeT FileSize() = 0;

protected:
    VirtualFileSystem *file_system_{};
    FSType fs_type_{FSType::kLocal};
    atomic_bool open_{false};
};

export class LocalFile final : public AbstractFileHandle {
public:
    LocalFile(VirtualFileSystem* file_system) : AbstractFileHandle(file_system, FSType::kLocal) {}
    ~LocalFile() final = default;
    Status Close() final;
    Status Append(const char *buffer) final;
    Status Append(const String &buffer) final;
    Tuple<SizeT, Status> Read(char *buffer) final;
    Tuple<SizeT, Status> Read(String &buffer) final;
    SizeT FileSize() final;
    Tuple<char*, SizeT, Status> MmapRead(const String &name) final;
    Status Unmmap(const String &name) final;
};

export class ObjectFile : public AbstractFileHandle {
public:
    ObjectFile(VirtualFileSystem* file_system, FSType type) : AbstractFileHandle(file_system, type) {}
    ~ObjectFile() override = default;
    Status Close() override;
    Status Append(const char *buffer) override;
    Status Append(const String &buffer) override;
    Tuple<SizeT, Status> Read(char *buffer) override;
    Tuple<SizeT, Status> Read(String &buffer) override;
    SizeT FileSize() override;
    Tuple<char*, SizeT, Status> MmapRead(const String &name) final;
    Status Unmmap(const String &name) final;
};

export class MinioFile final : public ObjectFile {
public:
    MinioFile(VirtualFileSystem* file_system) : ObjectFile(file_system, FSType::kMinio) {}
    ~MinioFile() final = default;
    Status Close() final;
    Status Append(const char *buffer) final;
    Status Append(const String &buffer) final;
    Tuple<SizeT, Status> Read(char *buffer) final;
    Tuple<SizeT, Status> Read(String &buffer) final;
    SizeT FileSize() final;
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

    UniquePtr<minio::s3::BaseUrl> minio_base_url_{};
    UniquePtr<minio::creds::StaticProvider> minio_provider_{};
    UniquePtr<minio::s3::Client> minio_client_{};

};

} // namespace infinity

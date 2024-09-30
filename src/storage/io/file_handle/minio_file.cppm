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

export module minio_file;

import stl;
import virtual_storage_type;
import status;
import object_file;
import abstract_file_handle;

namespace infinity {

class VirtualStorage;
struct MinioMmapInfo {
    char *data_ptr_{};
    SizeT data_len_{};
    SizeT rc_{};
};

export class MinioFile final : public ObjectFile {
public:
    MinioFile(VirtualStorage *storage_system);
    ~MinioFile() final;
    Status Open(const String &path, FileAccessMode access_mode) final;
    Status Close() final;
    Status Append(const void *buffer, u64 nbytes) final;
    Status Append(const String &buffer, u64 nbytes) final;
    Tuple<SizeT, Status> Read(void *buffer, u64 nbytes) final;
    Tuple<SizeT, Status> Read(String &buffer, u64 nbytes) final;
    Status Seek(u64 nbytes) final;
    Status Download(const String &url, const String &path) final;
    Status Upload(const String &path, const String &url) final;
    SizeT FileSize() final;
    Tuple<char *, SizeT, Status> MmapRead(const String &name) final;
    Status Unmmap(const String &name) final;
    Status Sync() final;

private:
    i32 fd_{-1};
    static std::mutex mtx_;
    static HashMap<String, MinioMmapInfo> mapped_files_;
};

} // namespace infinity
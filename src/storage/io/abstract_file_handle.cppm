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

export module abstract_file_handle;

import stl;
import virtual_storage_type;
import status;

namespace infinity {

class VirtualStorage;

export enum class FileAccessMode { kWrite, kRead, kMmapRead, kInvalid };

export class AbstractFileHandle {
public:
    explicit AbstractFileHandle(VirtualStorage *storage_system, StorageType storage_type);
    virtual ~AbstractFileHandle() = 0;
    virtual Status Open(const String &path, FileAccessMode access_mode) = 0;
    virtual Status Close() = 0;
    virtual Status Append(const char *buffer, u64 nbytes) = 0;
    virtual Status Append(const String &buffer, u64 nbytes) = 0;
    virtual Tuple<SizeT, Status> Read(char *buffer, u64 nbytes) = 0;
    virtual Tuple<SizeT, Status> Read(String &buffer, u64 nbytes) = 0;
    virtual SizeT FileSize() = 0;
    virtual Tuple<char *, SizeT, Status> MmapRead(const String &name) = 0;
    virtual Status Unmmap(const String &name) = 0;
    virtual Status Sync() = 0;

protected:
    VirtualStorage *storage_system_{};
    StorageType storage_type_{StorageType::kLocal};
    atomic_bool open_{false};
    String path_;
    FileAccessMode access_mode_;
    atomic_bool sync_{false};
};

} // namespace infinity

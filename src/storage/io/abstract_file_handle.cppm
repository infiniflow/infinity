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
import virtual_file_system_type;
import status;

namespace infinity {

class VirtualFileSystem;

export enum class FileAccessMode { kWrite, kRead, kMmapRead, kInvalid };

export class AbstractFileHandle {
public:
    AbstractFileHandle(VirtualFileSystem *file_system, FSType fs_type) : file_system_(file_system), fs_type_(fs_type) {}
    virtual ~AbstractFileHandle() = 0;
    virtual Status Close() = 0;
    virtual Status Append(const char *buffer) = 0;
    virtual Status Append(const String &buffer) = 0;
    virtual Tuple<SizeT, Status> Read(char *buffer) = 0;
    virtual Tuple<SizeT, Status> Read(String &buffer) = 0;
    virtual Tuple<char *, SizeT, Status> MmapRead(const String &name) = 0;
    virtual Status Unmmap(const String &name) = 0;
    virtual SizeT FileSize() = 0;

protected:
    VirtualFileSystem *file_system_{};
    FSType fs_type_{FSType::kLocal};
    atomic_bool open_{false};
};

}


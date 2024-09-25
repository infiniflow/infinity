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

export module object_file;

import stl;
import virtual_file_system_type;
import status;
import abstract_file_handle;
import virtual_file_system;

namespace infinity {

export class ObjectFile : public AbstractFileHandle {
public:
    ObjectFile(VirtualFileSystem *file_system, FSType type) : AbstractFileHandle(file_system, type) {}
    ~ObjectFile() override = default;
    Status Open(const String &path, FileAccessMode access_mode) override;
    Status Close() override;
    Status Append(const char *buffer) override;
    Status Append(const String &buffer) override;
    Tuple<SizeT, Status> Read(char *buffer) override;
    Tuple<SizeT, Status> Read(String &buffer) override;
    SizeT FileSize() override;
    Tuple<char *, SizeT, Status> MmapRead(const String &name) final;
    Status Unmmap(const String &name) final;
};

}
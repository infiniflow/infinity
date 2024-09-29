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
import virtual_storage_type;
import status;
import abstract_file_handle;

namespace infinity {

class VirtualStorage;

export class ObjectFile : public AbstractFileHandle {
public:
    ObjectFile(VirtualStorage *storage_system, StorageType type);
    ~ObjectFile() override;
    Status Open(const String &path, FileAccessMode access_mode) override;
    Status Close() override;
    Status Append(const char *buffer, u64 nbytes) override;
    Status Append(const String &buffer, u64 nbytes) override;
    Tuple<SizeT, Status> Read(char *buffer, u64 nbytes) override;
    Tuple<SizeT, Status> Read(String &buffer, u64 nbytes) override;
    Status Seek(u64 nbytes) override;
    Status Download(const String& url, const String& path) override;
    Status Upload(const String& path, const String& url) override;
    SizeT FileSize() override;
    Tuple<char *, SizeT, Status> MmapRead(const String &name) override;
    Status Unmmap(const String &name) override;
    Status Sync() override;
};

}
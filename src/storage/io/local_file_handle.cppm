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

export module local_file_handle;

import stl;
import status;

namespace infinity {

export enum class FileAccessMode { kWrite, kRead, kMmapRead, kInvalid };

export class LocalFileHandle {
public:
    LocalFileHandle(i32 fd, const String &path, FileAccessMode file_access_mode) : fd_(fd), path_(path), access_mode_(file_access_mode) {}
    ~LocalFileHandle();


    Status Append(const void *buffer, u64 nbytes);
    Status Append(const String &buffer, u64 nbytes);
    Tuple<SizeT, Status> Read(void *buffer, u64 nbytes);
    Tuple<SizeT, Status> Read(String &buffer, u64 nbytes);
    Status Seek(u64 nbytes);
    i64 FileSize();
    Tuple<char *, SizeT, Status> MmapRead(const String &name);
    Status Unmmap(const String &name);
    Status Sync();

public:
    i32 FileDescriptor() const {
        return fd_;
    }

    String Path() const {
        return path_;
    }

private:
    Status Close();

private:
    i32 fd_{-1};
    String path_{};
    FileAccessMode access_mode_{FileAccessMode::kInvalid};
};

} // namespace infinity
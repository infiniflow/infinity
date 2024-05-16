// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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
import file_system_type;

export module file_system;

namespace infinity {

export class FileSystem;

export class FileHandler {
public:
    FileHandler(FileSystem &file_system, String path) : file_system_(file_system), path_(std::move(path)) {}

    FileHandler(const FileHandler &) = delete;

    virtual ~FileHandler() = default;

    i64 Read(void *data, u64 nbytes);

    i64 Write(const void *data, u64 nbytes);

    void Rename(const String& old_name, const String& new_name);

    void Sync();

    void Close();

public:
    FileSystem &file_system_;
    Path path_;
};

class FileSystem {
public:
    explicit FileSystem(FileSystemType file_system_type) : file_system_type_(file_system_type) {}

    virtual ~FileSystem() = default;

    // File related methods
    virtual UniquePtr<FileHandler> OpenFile(const String &path, u8 flags, FileLockType lock_type) = 0;

    virtual i64 Read(FileHandler &file_handler, void *data, u64 nbytes) = 0;

    virtual i64 Write(FileHandler &file_handler, const void *data, u64 nbytes) = 0;

    virtual i64 ReadAt(FileHandler &file_handler, i64 file_offset, void *data, u64 nbytes) = 0;

    virtual i64 WriteAt(FileHandler &file_handler, i64 file_offset, const void *data, u64 nbytes) = 0;

    virtual void Rename(const String &old_path, const String &new_path) = 0;

    virtual void Seek(FileHandler &file_handler, i64 pos) = 0;

    virtual SizeT GetFileSize(FileHandler &file_handler) = 0;

    virtual void DeleteFile(const String &file_name) = 0;

    virtual void SyncFile(FileHandler &file_handler) = 0;

    virtual void Close(FileHandler &file_handler) = 0;

    virtual void AppendFile(const String &dst_path, const String &src_path) = 0;

    // Directory related methods
    virtual bool Exists(const String &path) = 0; // if file or directory exists

    virtual void CreateDirectory(const String &path) = 0;

    virtual u64 DeleteDirectory(const String &path) = 0;

    virtual void CleanupDirectory(const String &path) = 0;

    virtual Vector<SharedPtr<DirEntry>> ListDirectory(const String &path) = 0;

    inline FileSystemType file_system_type() const {
        return file_system_type_;
    }
private:
    FileSystemType file_system_type_{FileSystemType::kPosix};
};

} // namespace infinity

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
import file_system;
import file_system_type;
import status;

export module local_file_system;

namespace infinity {

export class LocalFileHandler : public FileHandler {
public:
    LocalFileHandler(FileSystem &file_system, String path, i32 fd) : FileHandler(file_system, std::move(path)), fd_(fd) {}

    ~LocalFileHandler() override;

public:
    i32 fd_{-1};
};

export struct MmapInfo {
    u8 *data_ptr_{};
    SizeT data_len_{};
    SizeT rc_{};
};

export class LocalFileSystem final : public FileSystem {
public:
    LocalFileSystem() : FileSystem(FileSystemType::kPosix) {}

    ~LocalFileSystem() override = default;

    Pair<UniquePtr<FileHandler>, Status> OpenFile(const String &path, u8 flags, FileLockType lock_type) final;

    i64 Read(FileHandler &file_handler, void *data, u64 nbytes) final;

    i64 Write(FileHandler &file_handler, const void *data, u64 nbytes) final;

    i64 ReadAt(FileHandler &file_handler, i64 file_offset, void *data, u64 nbytes) final;

    i64 WriteAt(FileHandler &file_handler, i64 file_offset, const void *data, u64 nbytes) final;

    void Rename(const String &old_path, const String &new_path) final;

    void Seek(FileHandler &file_handler, i64 pos) final;

    SizeT GetFileSize(FileHandler &file_handler) final;

    void DeleteFile(const String &file_name) final;

    void SyncFile(FileHandler &file_handler) final;

    void Close(FileHandler &file_handler) final;

    void AppendFile(const String &dst_path, const String &src_path) final;

    // Directory related methods
    bool Exists(const String &path) final; // if file or directory exists

    void CreateDirectory(const String &path) final;

    // return true if successfully created directory
    bool CreateDirectoryNoExp(const String &path);

    u64 DeleteDirectory(const String &path) final;

    // Remove everything recursively under given directory. Create it if it doesn't exist.
    void CleanupDirectory(const String &path) final;

    Vector<SharedPtr<DirEntry>> ListDirectory(const String &path) final;

    String GetAbsolutePath(const String &path);

    static u64 GetFileSizeByPath(const String& path);

    static u64 GetFolderSizeByPath(const String& path);

    static String ConcatenateFilePath(const String& dir_path, const String& file_path);

    int MmapFile(const String &file_path, u8 *&data_ptr, SizeT &data_len);

    int MunmapFile(const String &file_path);

private:
    static std::mutex mtx_;
    static HashMap<String, MmapInfo> mapped_files_;
};

} // namespace infinity
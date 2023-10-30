//
// Created by jinhai on 23-5-17.
//

module;

import stl;
import file_system;
import file_system_type;

export module local_file_system;

namespace infinity {

class LocalFileHandler : public FileHandler {
public:
    LocalFileHandler(FileSystem &file_system, String path, i32 fd) : FileHandler(file_system, Move(path)), fd_(fd) {}

    ~LocalFileHandler() override;

public:
    i32 fd_{-1};
};

export class LocalFileSystem final : public FileSystem {
public:
    LocalFileSystem() : FileSystem(FileSystemType::kPosix) {}

    ~LocalFileSystem() override = default;

    UniquePtr<FileHandler> OpenFile(const String &path, u8 flags, FileLockType lock_type) final;

    i64 Read(FileHandler &file_handler, void *data, u64 nbytes) final;

    i64 Write(FileHandler &file_handler, const void *data, u64 nbytes) final;

    void Rename(const String &old_path, const String &new_path) final;

    void Seek(FileHandler &file_handler, i64 pos) final;

    SizeT GetFileSize(FileHandler &file_handler) final;

    void DeleteFile(const String &file_name) final;

    void SyncFile(FileHandler &file_handler) final;

    void Close(FileHandler &file_handler) final;

    // Directory related methods
    bool Exists(const String &path) final; // if file or directory exists

    void CreateDirectory(const String &path) final;

    // return true if successfully created directory
    bool CreateDirectoryNoExp(const String &path);

    void DeleteDirectory(const String &path) final;

    Vector<SharedPtr<DirEntry>> ListDirectory(const String &path) final;

    String GetAbsolutePath(const String &path);
};

} // namespace infinity
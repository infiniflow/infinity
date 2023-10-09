//
// Created by jinhai on 23-5-16.
//

#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "file_system_type.h"

namespace infinity {

class FileSystem;

class FileHandler {
public:
    FileHandler(FileSystem &file_system, String path) : file_system_(file_system), path_(std::move(path)) {}

    FileHandler(const FileHandler &) = delete;

    virtual ~FileHandler() = default;

    i64 Read(void *data, u64 nbytes);

    i64 Write(void *data, u64 nbytes);

    void Sync();

    void Close();

public:
    FileSystem &file_system_;
    std::filesystem::path path_;
};

class FileSystem {
public:
    explicit FileSystem(FileSystemType file_system_type) : file_system_type_(file_system_type) {}

    virtual ~FileSystem() = default;

    // File related methods
    virtual UniquePtr<FileHandler> OpenFile(const String &path, u8 flags, FileLockType lock_type) = 0;

    virtual i64 Read(FileHandler &file_handler, void *data, u64 nbytes) = 0;

    virtual i64 Write(FileHandler &file_handler, void *data, u64 nbytes) = 0;

    virtual void Seek(FileHandler &file_handler, i64 pos) = 0;

    virtual SizeT GetFileSize(FileHandler &file_handler) = 0;

    virtual void DeleteFile(const String &file_name) = 0;

    virtual void SyncFile(FileHandler &file_handler) = 0;

    virtual void Close(FileHandler &file_handler) = 0;

    // Directory related methods
    virtual bool Exists(const String &path) = 0; // if file or directory exists

    virtual void CreateDirectory(const String &path) = 0;

    virtual void DeleteDirectory(const String &path) = 0;

    virtual Vector<SharedPtr<DirEntry>> ListDirectory(const String &path) = 0;

private:
    FileSystemType file_system_type_{FileSystemType::kPosix};
};

} // namespace infinity

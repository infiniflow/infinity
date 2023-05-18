//
// Created by jinhai on 23-5-16.
//

#pragma once

#include <filesystem>
#include "common/types/internal_types.h"

namespace infinity {

class FileSystem;

class FileHandler {
public:
    FileHandler(FileSystem &file_system, String path) : file_system_(file_system), path_(std::move(path)) {}
    FileHandler(const FileHandler &) = delete;

    virtual
    ~FileHandler() = default;

    i64
    Read(void* data, u64 nbytes);

    i64
    Write(void* data, u64 nbytes);

    void
    Sync();

public:
    FileSystem& file_system_;
    std::filesystem::path path_;
};

enum class FileSystemType {
    kPosix,
    kS3,
    kHDFS,
};

enum class FileLockType {
    kNoLock,
    kReadLock,
    kWriteLock
};

class FileFlags {
public:
    static constexpr u8 READ_FLAG = 1 << 0;
    static constexpr u8 WRITE_FLAG = 1 << 1;
    static constexpr u8 DIRECT_IO = 1 << 2;
    static constexpr u8 CREATE_FLAG = 1 << 3;
    static constexpr u8 TRUNCATE_CREATE = 1 << 4;
    static constexpr u8 APPEND_FLAG = 1 << 5;
};

class FileSystem {
public:
    explicit
    FileSystem(FileSystemType file_system_type) : file_system_type_(file_system_type) {}

    // File related methods
    virtual UniquePtr<FileHandler>
    OpenFile(const String& path, u8 flags, FileLockType lock_type) = 0;

    virtual i64
    Read(FileHandler& file_handler, void* data, u64 nbytes) = 0;

    virtual i64
    Write(FileHandler& file_handler, void* data, u64 nbytes) = 0;

    virtual i64
    GetFileSize(FileHandler& file_handler) = 0;

    virtual void
    DeleteFile(const String& file_name) = 0;

    virtual void
    SyncFile(FileHandler& file_handler) = 0;

    // Directory related methods
    virtual bool
    Exists(const String& path) = 0; // if file or directory exists

    virtual void
    CreateDirectory(const String& path) = 0;

    virtual void
    DeleteDirectory(const String& path) = 0;

private:
    FileSystemType file_system_type_{FileSystemType::kPosix};
};

}

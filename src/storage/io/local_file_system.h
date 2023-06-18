//
// Created by jinhai on 23-5-17.
//

#pragma once

#include "file_system.h"

namespace infinity {

class LocalFileHandler: public FileHandler {
public:
    LocalFileHandler(FileSystem& file_system, String path, i32 fd) : FileHandler(file_system, std::move(path)), fd_(fd) {}

    ~LocalFileHandler() override;

public:
    i32 fd_{-1};
};

class LocalFileSystem final : public FileSystem {
public:
    LocalFileSystem(): FileSystem(FileSystemType::kPosix) {}

    UniquePtr<FileHandler>
    OpenFile(const String& path, u8 flags, FileLockType lock_type) final;

    i64
    Read(FileHandler& file_handler, void* data, u64 nbytes) final;

    i64
    Write(FileHandler& file_handler, void* data, u64 nbytes) final;

    i64
    GetFileSize(FileHandler& file_handler) final;

    void
    DeleteFile(const String& file_name) final;

    void
    SyncFile(FileHandler& file_handler) final;

    // Directory related methods
    bool
    Exists(const String& path) final; // if file or directory exists

    void
    CreateDirectory(const String& path) final;

    void
    DeleteDirectory(const String& path) final;
};

}
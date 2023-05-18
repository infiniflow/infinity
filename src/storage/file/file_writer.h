//
// Created by jinhai on 23-5-16.
//

#pragma once

#include "file_system.h"

namespace infinity {

class FileWriter {
public:
    explicit
    FileWriter(FileSystem& fs, const String& path, SizeT buffer_size, u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG)
    : fs_(fs), path_(path), buffer_size_(buffer_size), data_(MakeUnique<char_t[]>(buffer_size)), offset_(0), total_written_(0) {
        file_handler_ = fs.OpenFile(path, file_flags, FileLockType::kWriteLock);
    }

    FileSystem& fs_;
    String path_;
    UniquePtr<char_t[]> data_;
    i64 offset_{};
    SizeT total_written_{};
    SizeT buffer_size_{};
    UniquePtr<FileHandler> file_handler_{};

public:
    void
    Write(const char_t* buffer, SizeT bytes_count);

    void
    Sync();

    void
    Flush();

    i64
    GetFileSize();

    SizeT
    TotalWrittenBytes() const;
};

}


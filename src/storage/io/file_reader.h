//
// Created by jinhai on 23-5-16.
//

#pragma once

#include "file_system.h"

namespace infinity {

class FileReader {
public:
    explicit
    FileReader(FileSystem& fs, const String& path, SizeT buffer_size);

    FileSystem& fs_;
    String path_;
    UniquePtr<char_t[]> data_;
    i64 offset_{};
    i64 already_read_size_{};
    SizeT total_read_{};
    SizeT buffer_size_{};
    SizeT file_size_{};
    UniquePtr<FileHandler> file_handler_{};

public:
    u8 
    ReadByte();

    i16 
    ReadShort();

    i32
    ReadInt();

    i32
    ReadVInt();

    i64
    ReadLong();

    i64
    ReadVLong();

    void
    Read(char_t* buffer, SizeT read_size);

    bool
    Finished() const;

    i64
    GetFilePointer() const;
};

}


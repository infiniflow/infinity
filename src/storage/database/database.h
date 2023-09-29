//
// Created by jinhai on 23-5-27.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/io/file_system_type.h"

namespace infinity {

class FileSystem;

class Database {
public:
    static inline SharedPtr<Database>
    Open(const String& dir_name, FileSystemType file_system_type);

public:
    Database(String dir_name, FileSystemType file_system_type);

    ~Database();

private:
    i32 locker_fd_{};
    String dir_name_{};
    UniquePtr<FileSystem> file_system_{};
};

}



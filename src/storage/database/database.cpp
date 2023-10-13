//
// Created by jinhai on 23-5-27.
//

#include "database.h"
#include "database_helper.h"
#include "common/utility/infinity_assert.h"
#include "storage/io/local_file_system.h"

namespace infinity {

Database::Database(String dir_name, FileSystemType file_system_type) : dir_name_(std::move(dir_name)) {
    switch (file_system_type) {

        case FileSystemType::kPosix: {
            file_system_ = MakeUnique<LocalFileSystem>();
            break;
        }
        case FileSystemType::kS3:
        case FileSystemType::kHDFS:
        case FileSystemType::kNFS: {
            NotImplementError("S3/HDFS/NFS filesystem isn't implemented yet.");
        }
    }
}

Database::~Database() {
    if (locker_fd_ != 0) {
        // Unlock the locker file
        close(locker_fd_);
    }
}

SharedPtr<Database> Database::Open(const String &dir_name, FileSystemType file_system_type) {
    SharedPtr<Database> database_ptr = MakeShared<Database>(dir_name, file_system_type);
    database_ptr->locker_fd_ = DatabaseHelper::CreateLockerFile(dir_name);

    return database_ptr;
}

} // namespace infinity
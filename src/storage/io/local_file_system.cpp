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

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>

import stl;
import file_system;
import file_system_type;

import infinity_exception;
import third_party;
import logger;
import status;

module local_file_system;

namespace infinity {

namespace fs = std::filesystem;

constexpr std::size_t BUFFER_SIZE = 4096; // Adjust buffer size as needed

LocalFileHandler::~LocalFileHandler() {
    if (fd_ != -1) {
        int ret = close(fd_);
        if (ret != 0) {
            UnrecoverableError(fmt::format("Close file failed: {}", strerror(errno)));
        }
        fd_ = -1;
    }
}

UniquePtr<FileHandler> LocalFileSystem::OpenFile(const String &path, u8 flags, FileLockType lock_type) {
    i32 file_flags{O_RDWR};
    bool read_flag = flags & FileFlags::READ_FLAG;
    bool write_flag = flags & FileFlags::WRITE_FLAG;
    if (read_flag && write_flag) {
        file_flags = O_RDWR;
    } else if (read_flag) {
        file_flags = O_RDONLY;
    } else if (write_flag) {
        file_flags = O_WRONLY;
    } else {
        UnrecoverableError("Please specify a read or write flag on file open");
    }

    if (write_flag) {
        file_flags |= O_CLOEXEC; // Close fd when fork the process
        if (flags & FileFlags::CREATE_FLAG) {
            file_flags |= O_CREAT;
        } else if (flags & FileFlags::TRUNCATE_CREATE) {
            file_flags |= O_CREAT | O_TRUNC;
        }

        if (flags & FileFlags::APPEND_FLAG) {
            file_flags |= O_APPEND;
        }
#if defined(__linux__)
        if (flags & FileFlags::DIRECT_IO) {
            file_flags |= O_DIRECT | O_SYNC;
        }
#endif
    }

    i32 fd = open(path.c_str(), file_flags, 0666);
    if (fd == -1) {
        UnrecoverableError(fmt::format("Can't open file: {}: {}", path, strerror(errno)));
    }

    if (lock_type != FileLockType::kNoLock) {
        struct flock file_lock {};
        memset(&file_lock, 0, sizeof(file_lock));
        if (lock_type == FileLockType::kReadLock) {
            file_lock.l_type = F_RDLCK;
        } else {
            file_lock.l_type = F_WRLCK;
        }
        file_lock.l_whence = SEEK_SET;
        file_lock.l_start = 0;
        file_lock.l_len = 0;
        if (fcntl(fd, F_SETLK, &file_lock) == -1) {
            UnrecoverableError(fmt::format("Can't lock file: {}: {}", path, strerror(errno)));
        }
    }
    return MakeUnique<LocalFileHandler>(*this, path, fd);
}

void LocalFileSystem::Close(FileHandler &file_handler) {
    auto local_file_handler = dynamic_cast<LocalFileHandler *>(&file_handler);
    i32 fd = local_file_handler->fd_;
    // set fd to -1 so that destructor of `LocalFileHandler` will not close the file.
    local_file_handler->fd_ = -1;
    if (close(fd) != 0) {
        UnrecoverableError(fmt::format("Can't close file: {}: {}", file_handler.path_.string(), strerror(errno)));
    }
}

void LocalFileSystem::Rename(const String &old_path, const String &new_path) {
    if (rename(old_path.c_str(), new_path.c_str()) != 0) {
        UnrecoverableError(fmt::format("Can't rename file: {}, {}", old_path, strerror(errno)));
    }
}

i64 LocalFileSystem::Read(FileHandler &file_handler, void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 read_count = read(fd, data, nbytes);
    if (read_count == -1) {
        UnrecoverableError(fmt::format("Can't read file: {}: {}", file_handler.path_.string(), strerror(errno)));
    }
    return read_count;
}

i64 LocalFileSystem::Write(FileHandler &file_handler, const void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 write_count = write(fd, data, nbytes);
    if (write_count == -1) {
        UnrecoverableError(fmt::format("Can't write file: {}: {}. fd: {}", file_handler.path_.string(), strerror(errno), fd));
    }
    return write_count;
}

void LocalFileSystem::Seek(FileHandler &file_handler, i64 pos) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    if ((off_t)-1 == lseek(fd, pos, SEEK_SET)) {
        UnrecoverableError(fmt::format("Can't seek file: {}: {}", file_handler.path_.string(), strerror(errno)));
    }
}

SizeT LocalFileSystem::GetFileSize(FileHandler &file_handler) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    struct stat s {};
    if (fstat(fd, &s) == -1) {
        return -1;
    }
    return s.st_size;
}

void LocalFileSystem::DeleteFile(const String &file_name) {
    std::error_code error_code;
    Path p{file_name};
    bool is_deleted = std::filesystem::remove(p, error_code);
    if (error_code.value() == 0) {
        if (!is_deleted) {
            UnrecoverableError(fmt::format("Can't delete file: {}: {}", file_name, strerror(errno)));
        }
    } else {
        UnrecoverableError(fmt::format("Delete file {} exception: {}", file_name, strerror(errno)));
    }
}

void LocalFileSystem::SyncFile(FileHandler &file_handler) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    if (fsync(fd) != 0) {
        UnrecoverableError(fmt::format("fsync failed: {}, {}", file_handler.path_.string(), strerror(errno)));
    }
}

void LocalFileSystem::AppendFile(const String &dst_path, const String &src_path) {
    Path dst{dst_path};
    Path src{src_path};
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile.is_open()) {
        UnrecoverableError(fmt::format("Failed to open source file {}", src_path));
        return;
    }
    std::ofstream dstFile(dst, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        UnrecoverableError(fmt::format("Failed to open destination file {}", dst_path));
        return;
    }
    char buffer[BUFFER_SIZE];
    while (srcFile.read(buffer, BUFFER_SIZE)) {
        dstFile.write(buffer, srcFile.gcount());
    }
    dstFile.write(buffer, srcFile.gcount());
    srcFile.close();
    dstFile.close();
}

// Directory related methods
bool LocalFileSystem::Exists(const String &path) {
    std::error_code error_code;
    Path p{path};
    bool is_exists = std::filesystem::exists(p, error_code);
    if (error_code.value() == 0) {
        return is_exists;
    } else {
        UnrecoverableError(fmt::format("{} exists exception: {}", path, strerror(errno)));
    }
    return false;
}

bool LocalFileSystem::CreateDirectoryNoExp(const String &path) {
    std::error_code error_code;
    Path p{path};
    return std::filesystem::create_directories(p, error_code);
}

void LocalFileSystem::CreateDirectory(const String &path) {
    std::error_code error_code;
    Path p{path};
    std::filesystem::create_directories(p, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("{} create exception: {}", path, strerror(errno)));
    }
}

u64 LocalFileSystem::DeleteDirectory(const String &path) {
    std::error_code error_code;
    Path p{path};
    u64 removed_count = std::filesystem::remove_all(p, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("Delete directory {} exception: {}", path, error_code.message()));
    }
    return removed_count;
}

void LocalFileSystem::DeleteEmptyDirectory(const String &path) {
    std::error_code error_code;
    Path p{path};
    if (!std::filesystem::exists(p, error_code)) {
        RecoverableError(Status::DirNotFound(path));
    }
    std::filesystem::remove(p, error_code);
    if (error_code.value() != 0) {
        // log all files in path
        // std::stringstream ss;
        // // recursively traverse the directory
        // std::function<void(const Path &)> list_dir = [&](const Path path) {
        //     for (const auto &entry : std::filesystem::directory_iterator{path}) {
        //         ss << entry.path().string() << " ";
        //         if (entry.is_directory()) {
        //             list_dir(entry.path());
        //         }
        //     }
        // };
        // list_dir(p);

        DeleteDirectory(path);
        // LOG_ERROR(fmt::format("DeleteEmptyDirectory: {} is not empty, files: {}, force deleted: {}", path, ss.str(), removed_count));
    }
}

void LocalFileSystem::CleanupDirectory(const String &path) {
    std::error_code error_code;
    Path p{path};
    if (!fs::exists(p)) {
        std::filesystem::create_directories(p, error_code);
        if (error_code.value() != 0) {
            UnrecoverableError(fmt::format("CleanupDirectory create {} exception: {}", path, error_code.message()));
        }
        return;
    }
    try {
        std::ranges::for_each(std::filesystem::directory_iterator{path}, [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry); });
    } catch (const std::filesystem::filesystem_error &e) {
        UnrecoverableError(fmt::format("CleanupDirectory cleanup {} exception: {}", path, e.what()));
    }
}

Vector<SharedPtr<DirEntry>> LocalFileSystem::ListDirectory(const String &path) {
    Path dir_path(path);
    if (!is_directory(dir_path)) {
        UnrecoverableError(fmt::format("{} isn't a directory", path));
    }

    Vector<SharedPtr<DirEntry>> file_array;
    std::ranges::for_each(std::filesystem::directory_iterator{path},
                          [&](const auto &dir_entry) { file_array.emplace_back(MakeShared<DirEntry>(dir_entry)); });
    return file_array;
}

String LocalFileSystem::GetAbsolutePath(const String &path) {
    Path p{path};
    return std::filesystem::absolute(p).string();
}

u64 LocalFileSystem::GetFileSizeByPath(const String &path) { return std::filesystem::file_size(path); }

u64 LocalFileSystem::GetFolderSizeByPath(const String &path) {
    u64 totalSize = 0;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            totalSize += std::filesystem::file_size(entry);
        }
    }

    return totalSize;
}

String LocalFileSystem::ConcatenateFilePath(const String &dir_path, const String &file_path) {
    std::filesystem::path full_path = std::filesystem::path(dir_path) / file_path;
    return full_path.string();
}

} // namespace infinity
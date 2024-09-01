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

#include <cassert>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
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

std::mutex LocalFileSystem::mtx_{};
HashMap<String, MmapInfo> LocalFileSystem::mapped_files_{};

LocalFileHandler::~LocalFileHandler() {
    if (fd_ != -1) {
        int ret = close(fd_);
        if (ret != 0) {
            String error_message = fmt::format("Close file failed: {}", strerror(errno));
            UnrecoverableError(error_message);
        }
        fd_ = -1;
    }
}

Pair<UniquePtr<FileHandler>, Status> LocalFileSystem::OpenFile(const String &path, u8 flags, FileLockType lock_type) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
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
        return {nullptr, Status::InvalidFileFlag(file_flags)};
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
            file_flags |= O_DIRECT;
        }
#endif
    }

    i32 fd = open(path.c_str(), file_flags, 0666);
    if (fd == -1) {
        return {nullptr, Status::IOError(fmt::format("Can't open file: {}: {}", path, strerror(errno)))};
    }
    // LOG_TRACE(fmt::format("[+] OPEN FILE: {}", path));

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
            return {nullptr, Status::LockFileError(path, strerror(errno))};
        }
    }
    return {MakeUnique<LocalFileHandler>(*this, path, fd), Status::OK()};
}

void LocalFileSystem::Close(FileHandler &file_handler) {
    auto local_file_handler = dynamic_cast<LocalFileHandler *>(&file_handler);
    i32 fd = local_file_handler->fd_;
    // set fd to -1 so that destructor of `LocalFileHandler` will not close the file.
    local_file_handler->fd_ = -1;
    if (close(fd) != 0) {
        String error_message = fmt::format("Can't close file: {}: {}", file_handler.path_.string(), strerror(errno));
        UnrecoverableError(error_message);
    }
    // LOG_TRACE(fmt::format("[-] CLOSE FILE: {}", file_handler.path_.string()));
}

void LocalFileSystem::Rename(const String &old_path, const String &new_path) {
    if (!std::filesystem::path(old_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", old_path);
        UnrecoverableError(error_message);
    }
    if (!std::filesystem::path(new_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", new_path);
        UnrecoverableError(error_message);
    }
    if (rename(old_path.c_str(), new_path.c_str()) != 0) {
        String error_message = fmt::format("Can't rename file: {}, {}", old_path, strerror(errno));
        UnrecoverableError(error_message);
    }
}

i64 LocalFileSystem::Read(FileHandler &file_handler, void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 readen = 0;
    while (readen < (i64)nbytes) {
        SizeT a = nbytes - readen;
        i64 read_count = read(fd, (char *)data + readen, a);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", file_handler.path_.string(), strerror(errno));
            UnrecoverableError(error_message);
        }
        readen += read_count;
    }
    return readen;
}

i64 LocalFileSystem::Write(FileHandler &file_handler, const void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = write(fd, (char *)data + written, nbytes - written);
        if (write_count == -1) {
            String error_message = fmt::format("Can't write file: {}: {}. fd: {}", file_handler.path_.string(), strerror(errno), fd);
            UnrecoverableError(error_message);
        }
        written += write_count;
    }
    return written;
}

i64 LocalFileSystem::ReadAt(FileHandler &file_handler, i64 file_offset, void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 readen = 0;
    while (readen < (i64)nbytes) {
        i64 read_count = pread(fd, (char *)data + readen, nbytes - readen, file_offset + readen);
        if (read_count == 0) {
            break;
        }
        if (read_count == -1) {
            String error_message = fmt::format("Can't read file: {}: {}", file_handler.path_.string(), strerror(errno));
            UnrecoverableError(error_message);
        }
        readen += read_count;
    }
    return readen;
}

i64 LocalFileSystem::WriteAt(FileHandler &file_handler, i64 file_offset, const void *data, u64 nbytes) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    i64 written = 0;
    while (written < (i64)nbytes) {
        i64 write_count = pwrite(fd, (char *)data + written, nbytes - written, file_offset + written);
        if (write_count == -1) {
            String error_message = fmt::format("Can't write file: {}: {}. fd: {}", file_handler.path_.string(), strerror(errno), fd);
            UnrecoverableError(error_message);
        }
        written += write_count;
    }
    return written;
}

void LocalFileSystem::Seek(FileHandler &file_handler, i64 pos) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    if ((off_t)-1 == lseek(fd, pos, SEEK_SET)) {
        String error_message = fmt::format("Can't seek file: {}: {}", file_handler.path_.string(), strerror(errno));
        UnrecoverableError(error_message);
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
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{file_name};
    bool is_deleted = std::filesystem::remove(p, error_code);
    if (error_code.value() == 0) {
        if (!is_deleted) {
            LOG_WARN(fmt::format("Failed to delete file: {}: {}", file_name, strerror(errno)));
        }
    } else {
        String error_message = fmt::format("Delete file {} exception: {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
}

void LocalFileSystem::SyncFile(FileHandler &file_handler) {
    i32 fd = ((LocalFileHandler &)file_handler).fd_;
    if (fsync(fd) != 0) {
        String error_message = fmt::format("fsync failed: {}, {}", file_handler.path_.string(), strerror(errno));
        UnrecoverableError(error_message);
    }
}

void LocalFileSystem::AppendFile(const String &dst_path, const String &src_path) {
    if (!std::filesystem::path(dst_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", dst_path);
        UnrecoverableError(error_message);
    }
    if (!std::filesystem::path(src_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", src_path);
        UnrecoverableError(error_message);
    }
    Path dst{dst_path};
    Path src{src_path};
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile.is_open()) {
        String error_message = fmt::format("Failed to open source file {}", src_path);
        UnrecoverableError(error_message);
        return;
    }
    std::ofstream dstFile(dst, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        String error_message = fmt::format("Failed to open destination file {}", dst_path);
        UnrecoverableError(error_message);
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

void LocalFileSystem::Truncate(const String &file_name, SizeT length) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    std::filesystem::resize_file(file_name, length, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Failed to truncate {} to size {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
}

// Directory related methods
bool LocalFileSystem::Exists(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    bool is_exists = std::filesystem::exists(p, error_code);
    if (error_code.value() == 0) {
        return is_exists;
    } else {
        String error_message = fmt::format("{} exists exception: {}", path, strerror(errno));
        UnrecoverableError(error_message);
    }
    return false;
}

bool LocalFileSystem::CreateDirectoryNoExp(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    return std::filesystem::create_directories(p, error_code);
}

void LocalFileSystem::CreateDirectory(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    std::filesystem::create_directories(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("{} create exception: {}", path, strerror(errno));
        UnrecoverableError(error_message);
    }
}

u64 LocalFileSystem::DeleteDirectory(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    u64 removed_count = std::filesystem::remove_all(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Delete directory {} exception: {}", path, error_code.message());
        UnrecoverableError(error_message);
    }
    return removed_count;
}

void LocalFileSystem::CleanupDirectory(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    if (!fs::exists(p)) {
        std::filesystem::create_directories(p, error_code);
        if (error_code.value() != 0) {
            String error_message = fmt::format("CleanupDirectory create {} exception: {}", path, error_code.message());
            UnrecoverableError(error_message);
        }
        return;
    }
    try {
        std::ranges::for_each(std::filesystem::directory_iterator{path}, [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry); });
    } catch (const std::filesystem::filesystem_error &e) {
        String error_message = fmt::format("CleanupDirectory cleanup {} exception: {}", path, e.what());
        UnrecoverableError(error_message);
    }
}

Vector<SharedPtr<DirEntry>> LocalFileSystem::ListDirectory(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    Path dir_path(path);
    if (!is_directory(dir_path)) {
        String error_message = fmt::format("{} isn't a directory", path);
        UnrecoverableError(error_message);
    }

    Vector<SharedPtr<DirEntry>> file_array;
    std::ranges::for_each(std::filesystem::directory_iterator{path},
                          [&](const auto &dir_entry) { file_array.emplace_back(MakeShared<DirEntry>(dir_entry)); });
    return file_array;
}

String LocalFileSystem::GetAbsolutePath(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    Path p{path};
    return std::filesystem::absolute(p).string();
}

u64 LocalFileSystem::GetFileSizeByPath(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    return std::filesystem::file_size(path);
}

u64 LocalFileSystem::GetFolderSizeByPath(const String &path) {
    if(!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
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

int LocalFileSystem::MmapFile(const String &file_path, u8 *&data_ptr, SizeT &data_len) {
    if (!std::filesystem::path(file_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_path);
        UnrecoverableError(error_message);
    }
    data_ptr = nullptr;
    data_len = 0;
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = mapped_files_.find(file_path);
    if (it != mapped_files_.end()) {
        auto &mmap_info = it->second;
        data_ptr = mmap_info.data_ptr_;
        data_len = mmap_info.data_len_;
        mmap_info.rc_++;
        return 0;
    }
    long len_f = fs::file_size(file_path);
    if (len_f == 0)
        return -1;
    int f = open(file_path.c_str(), O_RDONLY);
    void *tmpd = mmap(NULL, len_f, PROT_READ, MAP_SHARED, f, 0);
    if (tmpd == MAP_FAILED)
        return -1;
    close(f);
    int rc = madvise(tmpd,
                     len_f,
                     MADV_NORMAL
#if defined(linux) || defined(__linux) || defined(__linux__)
                         | MADV_DONTDUMP
#endif
    );
    if (rc < 0)
        return -1;
    data_ptr = (u8 *)tmpd;
    data_len = len_f;
    mapped_files_.emplace(file_path, MmapInfo{data_ptr, data_len, 1});
    return 0;
}

int LocalFileSystem::MunmapFile(const String &file_path) {
    if(!std::filesystem::path(file_path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_path);
        UnrecoverableError(error_message);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = mapped_files_.find(file_path);
    if (it == mapped_files_.end()) {
        return -1;
    }
    auto &mmap_info = it->second;
    mmap_info.rc_--;
    if (mmap_info.rc_ == 0) {
        munmap(mmap_info.data_ptr_, mmap_info.data_len_);
        mapped_files_.erase(it);
    }
    return 0;
}

} // namespace infinity
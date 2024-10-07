// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

module virtual_store;

import stl;
import third_party;
import logger;
import infinity_exception;
import default_values;
import stream_reader;
import s3_client_minio;

namespace infinity {

StorageType String2StorageType(const String &storage_type) {
    if (storage_type == "local") {
        return StorageType::kLocal;
    }

    if (storage_type == "minio") {
        return StorageType::kMinio;
    }

    if (storage_type == "aws_s3") {
        return StorageType::kLocal;
    }

    if (storage_type == "azure_blob") {
        return StorageType::kLocal;
    }

    if (storage_type == "gcs") {
        return StorageType::kLocal;
    }

    if (storage_type == "oss") {
        return StorageType::kLocal;
    }

    if (storage_type == "cos") {
        return StorageType::kLocal;
    }
    if (storage_type == "obs") {
        return StorageType::kLocal;
    }

    if (storage_type == "hdfs") {
        return StorageType::kLocal;
    }

    if (storage_type == "nfs") {
        return StorageType::kLocal;
    }

    return StorageType::kInvalid;
}

String ToString(StorageType storage_type) {
    switch (storage_type) {
        case StorageType::kLocal: {
            return "local";
        }
        case StorageType::kMinio: {
            return "minio";
        }
        case StorageType::kAwsS3: {
            return "aws s3";
        }
        case StorageType::kAzureBlob: {
            return "azure blob";
        }
        case StorageType::kGCS: {
            return "google cloud storage";
        }
        case StorageType::kOSS: {
            return "aliyun object storage service";
        }
        case StorageType::kCOS: {
            return "tencent cloud object storage";
        }
        case StorageType::kOBS: {
            return "huawei object storage service";
        }
        case StorageType::kHDFS: {
            return "hadoop file system";
        }
        case StorageType::kNFS: {
            return "network file system";
        }
        default: {
            return "invalid";
        }
    }
}

Status RemoteStore::Init(StorageType storage_type, Map<String, String> &config) {
    // Init remote filesystem and local disk cache
    storage_type_ = storage_type;
    switch (storage_type) {
        case StorageType::kMinio: {
            auto iter = config.find("url");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'URL'");
            }
            String url = iter->second;

            iter = config.find("access_key");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'access_key'");
            }
            String access_key = iter->second;

            iter = config.find("secret_key");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'secret_key'");
            }
            String secret_key = iter->second;

            iter = config.find("enable_https");
            if (iter == config.end()) {
                return Status::InvalidConfig("Missing MINIO 'enable_https'");
            }
            String enable_https_str = iter->second;
            bool enable_https{false};
            if (enable_https_str == "true") {
                enable_https = true;
            } else if (enable_https_str == "false") {
                enable_https = false;
            } else {
                return Status::InvalidConfig(fmt::format("Invalid MINIO 'enable_https' value: {}", enable_https_str));
            }

            minio_base_url_ = MakeUnique<minio::s3::BaseUrl>(url, enable_https);
            minio_provider_ = MakeUnique<minio::creds::StaticProvider>(access_key, secret_key);
            minio_client_ = MakeUnique<minio::s3::Client>(*minio_base_url_, minio_provider_.get());
            break;
        }
        default: {
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type)));
        }
    }
    return Status::OK();
}

Status RemoteStore::UnInit() {
    switch (storage_type_) {
        case StorageType::kMinio: {
            minio_base_url_.reset();
            minio_provider_.reset();
            minio_client_.reset();
            break;
        }
        default: {
            return Status::NotSupport(fmt::format("{} isn't support in virtual filesystem", ToString(storage_type_)));
        }
    }
    return Status::OK();
}

Tuple<UniquePtr<LocalFileHandle>, Status> LocalStore::Open(const String &path, FileAccessMode access_mode) {
    i32 fd = -1;
    switch (access_mode) {
        case FileAccessMode::kRead: {
            fd = open(path.c_str(), O_RDONLY, 0666);
            break;
        }
        case FileAccessMode::kWrite: {
            fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
            break;
        }
        case FileAccessMode::kMmapRead: {
            UnrecoverableError("Unsupported now.");
            break;
        }
        case FileAccessMode::kInvalid: {
            break;
        }
    }
    if (fd == -1) {
        String error_message = fmt::format("File open failed: {}", strerror(errno));
        return {nullptr, Status::IOError(error_message)};
    }
    return {MakeUnique<LocalFileHandle>(fd, path, access_mode), Status::OK()};
}

UniquePtr<StreamReader> LocalStore::OpenStreamReader(const String &path) {
    auto res = MakeUnique<StreamReader>();
    Status status = res->Init(path);
    if (!status.ok()) {
        RecoverableError(status);
    }
    return res;
}

// For local disk filesystem, such as temp file, disk cache and WAL
bool LocalStore::Exists(const String &path) {
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

Status LocalStore::DeleteFile(const String &file_name) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{file_name};
    bool is_deleted = std::filesystem::remove(p, error_code);
    if (error_code.value() == 0) {
        if (!is_deleted) {
            String error_message = fmt::format("Failed to delete file: {}: {}", file_name, strerror(errno));
            LOG_WARN(error_message);
            Status status = Status::IOError(error_message);
            return status;
        }
    } else {
        String error_message = fmt::format("Delete file {} exception: {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::MakeDirectory(const String &path) {
    if (LocalStore::Exists(path)) {
        if (std::filesystem::is_directory(path)) {
            return Status::OK();
        } else {
            String error_message = fmt::format("Exists file: {}", path);
            LOG_ERROR(error_message);
            Status status = Status::IOError(error_message);
            return status;
        }
    }

    std::error_code error_code;
    Path p{path};
    std::filesystem::create_directories(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("{} create exception: {}", path, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::RemoveDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    std::filesystem::remove_all(p, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Delete directory {} exception: {}", path, error_code.message());
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::CleanupDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    Path p{path};
    if (!std::filesystem::exists(p)) {
        std::filesystem::create_directories(p, error_code);
        if (error_code.value() != 0) {
            String error_message = fmt::format("CleanupDirectory create {} exception: {}", path, error_code.message());
            UnrecoverableError(error_message);
        }
        return Status::OK();
    }
    try {
        std::ranges::for_each(std::filesystem::directory_iterator{path}, [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry); });
    } catch (const std::filesystem::filesystem_error &e) {
        String error_message = fmt::format("CleanupDirectory cleanup {} exception: {}", path, e.what());
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::Rename(const String &old_path, const String &new_path) {
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
    return Status::OK();
}

Status LocalStore::Truncate(const String &file_name, SizeT new_length) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", file_name);
        UnrecoverableError(error_message);
    }
    std::error_code error_code;
    std::filesystem::resize_file(file_name, new_length, error_code);
    if (error_code.value() != 0) {
        String error_message = fmt::format("Failed to truncate {} to size {}", file_name, strerror(errno));
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status LocalStore::Merge(const String &dst_path, const String &src_path) {
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
        return Status::OK();
    }
    std::ofstream dstFile(dst, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        String error_message = fmt::format("Failed to open destination file {}", dst_path);
        UnrecoverableError(error_message);
        return Status::OK();
    }
    char buffer[DEFAULT_READ_BUFFER_SIZE];
    while (srcFile.read(buffer, DEFAULT_READ_BUFFER_SIZE)) {
        dstFile.write(buffer, srcFile.gcount());
    }
    dstFile.write(buffer, srcFile.gcount());
    srcFile.close();
    dstFile.close();
    return Status::OK();
}

Tuple<Vector<SharedPtr<DirEntry>>, Status> LocalStore::ListDirectory(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
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
    return {file_array, Status::OK()};
}

SizeT LocalStore::GetFileSize(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        UnrecoverableError(error_message);
    }
    return std::filesystem::file_size(path);
}

String LocalStore::GetParentPath(const String &path) { return Path(path).parent_path().string(); }

SizeT LocalStore::GetDirectorySize(const String &path) {
    if (!std::filesystem::path(path).is_absolute()) {
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

String LocalStore::ConcatenatePath(const String &dir_path, const String &file_path) {
    std::filesystem::path full_path = std::filesystem::path(dir_path) / file_path;
    return full_path.string();
}

std::mutex LocalStore::mtx_;
HashMap<String, MmapInfo> LocalStore::mapped_files_;

i32 LocalStore::MmapFile(const String &file_path, u8 *&data_ptr, SizeT &data_len) {
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
    long len_f = std::filesystem::file_size(file_path);
    if (len_f == 0)
        return -1;
    i32 f = open(file_path.c_str(), O_RDONLY);
    void *tmpd = mmap(NULL, len_f, PROT_READ, MAP_SHARED, f, 0);
    if (tmpd == MAP_FAILED)
        return -1;
    close(f);
    i32 rc = madvise(tmpd,
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

i32 LocalStore::MunmapFile(const String &file_path) {
    if (!std::filesystem::path(file_path).is_absolute()) {
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

// Remote storage
StorageType LocalStore::storage_type_ = StorageType::kMinio;
String LocalStore::bucket_ = "infinity_bucket";
UniquePtr<S3Client> LocalStore::s3_client_ = nullptr;

Status LocalStore::InitRemoteStore(StorageType storage_type,
                                   const String &URL,
                                   bool HTTPS,
                                   const String &access_key,
                                   const String &secret_key,
                                   const String &bucket) {
    switch (storage_type) {
        case StorageType::kMinio: {
            s3_client_ = MakeUnique<S3ClientMinio>(URL, HTTPS, access_key, secret_key);
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }
    bucket_ = bucket;
    return Status::OK();
}

Status LocalStore::DownloadObject(const String &file_path, const String &object_name) {
    if (LocalStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (LocalStore::storage_type_) {
        case StorageType::kMinio: {
            return s3_client_->DownloadObject(LocalStore::bucket_, object_name, file_path);
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

Status LocalStore::UploadObject(const String &file_path, const String &object_name) {
    if (LocalStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (LocalStore::storage_type_) {
        case StorageType::kMinio: {
            return s3_client_->UploadObject(LocalStore::bucket_, object_name, file_path);
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

Status LocalStore::RemoveObject(const String &object_name) {
    if (LocalStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (LocalStore::storage_type_) {
        case StorageType::kMinio: {
            return s3_client_->RemoveObject(LocalStore::bucket_, object_name);
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}
Status LocalStore::CopyObject(const String &src_object_name, const String &dst_object_name) {
    if (LocalStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (LocalStore::storage_type_) {
        case StorageType::kMinio: {
            return s3_client_->CopyObject(LocalStore::bucket_, src_object_name, LocalStore::bucket_, dst_object_name);
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

} // namespace infinity

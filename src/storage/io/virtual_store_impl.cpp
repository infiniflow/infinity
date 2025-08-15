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
#include <fcntl.h>
#include <lz4.h>
#include <openssl/md5.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

module infinity_core:virtual_store.impl;

import :virtual_store;
import :stl;
import :s3_client_minio;
import :infinity_context;
import :utility;

import std;
import std.compat;

namespace fs = std::filesystem;

namespace infinity {

StorageType String2StorageType(const std::string &storage_type) {
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

std::string ToString(StorageType storage_type) {
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

std::tuple<std::unique_ptr<LocalFileHandle>, Status> VirtualStore::Open(const std::string &path, FileAccessMode access_mode) {
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
        return {nullptr, Status::IOError(fmt::format("File: {} open failed: {}", path, strerror(errno)))};
    }
    return {std::make_unique<LocalFileHandle>(fd, path, access_mode), Status::OK()};
}

std::unique_ptr<StreamReader> VirtualStore::OpenStreamReader(const std::string &path) {
    auto res = std::make_unique<StreamReader>();
    auto status = res->Init(path);
    if (!status.ok()) {
        RecoverableError(status);
    }
    return res;
}

// For local disk filesystem, such as temp file, disk cache and WAL
bool VirtualStore::Exists(const std::string &path) {
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

Status VirtualStore::DeleteFile(const std::string &file_name) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", file_name));
    }
    std::error_code error_code;
    Path p{file_name};
    if (!Exists(p)) {
        LOG_WARN(fmt::format("The {} to be deleted does not exists ", file_name));
        return Status::OK();
    }
    bool is_deleted = std::filesystem::remove(p, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("Delete file {} exception: {}", file_name, strerror(errno)));
    }
    if (!is_deleted) {
        std::string error_message = fmt::format("Failed to delete file: {}: {}", file_name, strerror(errno));
        LOG_ERROR(error_message);
        Status status = Status::IOError(error_message);
        return status;
    }
    LOG_DEBUG(fmt::format("Clean file: {}", file_name));
    return Status::OK();
}

Status VirtualStore::DeleteFileBG(const std::string &path) {
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            auto drop_task = std::make_shared<LocalDropTask>(path);
            auto object_storage_processor = infinity::InfinityContext::instance().storage()->object_storage_processor();
            object_storage_processor->Submit(drop_task);
            drop_task->Wait();
            break;
        }
        default: {
            fs::remove(path);
            break;
        }
    }

    return Status::OK();
}

Status VirtualStore::MakeDirectory(const std::string &path) {
    if (VirtualStore::Exists(path)) {
        if (std::filesystem::is_directory(path)) {
            return Status::OK();
        } else {
            std::string error_message = fmt::format("Exists file: {}", path);
            LOG_ERROR(error_message);
            return Status::IOError(error_message);
        }
    }

    std::error_code error_code;
    Path p{path};
    std::filesystem::create_directories(p, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("{} create exception: {}", path, strerror(errno)));
    }
    return Status::OK();
}

Status VirtualStore::RemoveDirectory(const std::string &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", path));
    }
    std::error_code error_code;
    Path p{path};
    std::filesystem::remove_all(p, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("Delete directory {} exception: {}", path, error_code.message()));
    }
    return Status::OK();
}

Status VirtualStore::CleanupDirectory(const std::string &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", path));
    }
    std::error_code error_code;
    Path p{path};
    if (!std::filesystem::exists(p)) {
        std::filesystem::create_directories(p, error_code);
        if (error_code.value() != 0) {
            UnrecoverableError(fmt::format("CleanupDirectory create {} exception: {}", path, error_code.message()));
        }
        return Status::OK();
    }
    try {
        std::ranges::for_each(std::filesystem::directory_iterator{path}, [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry); });
    } catch (const std::filesystem::filesystem_error &e) {
        UnrecoverableError(fmt::format("CleanupDirectory cleanup {} exception: {}", path, e.what()));
    }
    return Status::OK();
}

void VirtualStore::RecursiveCleanupAllEmptyDir(const std::string &path) {
    if (!VirtualStore::Exists(path) || !std::filesystem::is_directory(path)) {
        return;
    }

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        RecursiveCleanupAllEmptyDir(entry.path());
    }

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        auto entry_path = entry.path();

        // Skip removing empty pos files of fulltext index, because they might be used.
        if (std::filesystem::is_empty(entry_path) && entry_path.extension().string() != ".pos") {
            std::filesystem::remove(entry_path);
        }
    }
}

Status VirtualStore::Rename(const std::string &old_path, const std::string &new_path) {
    if (!std::filesystem::path(old_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", old_path));
    }
    if (!std::filesystem::path(new_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", new_path));
    }
    if (rename(old_path.c_str(), new_path.c_str()) != 0) {
        UnrecoverableError(fmt::format("Can't rename file: {}, {}", old_path, strerror(errno)));
    }
    return Status::OK();
}

Status VirtualStore::Truncate(const std::string &file_name, size_t new_length) {
    if (!std::filesystem::path(file_name).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", file_name));
    }
    std::error_code error_code;
    if (!std::filesystem::exists(file_name)) {
        std::ofstream file(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
    }
    std::filesystem::resize_file(file_name, new_length, error_code);
    if (error_code.value() != 0) {
        UnrecoverableError(fmt::format("Failed to truncate {} to size {}", file_name, strerror(errno)));
    }
    return Status::OK();
}

Status VirtualStore::Merge(const std::string &dst_path, const std::string &src_path) {
    if (!std::filesystem::path(dst_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", dst_path));
    }
    if (!std::filesystem::path(src_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", src_path));
    }
    Path dst{dst_path};
    Path src{src_path};
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile.is_open()) {
        UnrecoverableError(fmt::format("Failed to open source file {}", src_path));
        return Status::OK();
    }
    std::ofstream dstFile(dst, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        UnrecoverableError(fmt::format("Failed to open destination file {}", dst_path));
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

Status VirtualStore::Copy(const std::string &dst_path, const std::string &src_path) {
    if (!std::filesystem::path(dst_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", dst_path));
    }
    if (!std::filesystem::path(src_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", src_path));
    }

    std::string dst_dir = GetParentPath(dst_path);
    if (!VirtualStore::Exists(dst_dir)) {
        VirtualStore::MakeDirectory(dst_dir);
    }

    try {
        std::filesystem::copy(src_path, dst_path, fs::copy_options::update_existing);
    } catch (const std::filesystem::filesystem_error &e) {
        return Status::IOError(fmt::format("Failed to copy file: {}", e.what()));
    }
    return Status::OK();
}

std::tuple<std::vector<std::shared_ptr<DirEntry>>, Status> VirtualStore::ListDirectory(const std::string &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", path));
    }
    Path dir_path(path);
    if (!is_directory(dir_path)) {
        UnrecoverableError(fmt::format("{} isn't a directory", path));
    }

    std::vector<std::shared_ptr<DirEntry>> file_array;
    std::ranges::for_each(std::filesystem::directory_iterator{path},
                          [&](const auto &dir_entry) { file_array.emplace_back(std::make_shared<DirEntry>(dir_entry)); });
    return {file_array, Status::OK()};
}

size_t VirtualStore::GetFileSize(const std::string &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", path));
    }
    return std::filesystem::file_size(path);
}

std::string VirtualStore::GetParentPath(const std::string &path) { return Path(path).parent_path().string(); }

size_t VirtualStore::GetDirectorySize(const std::string &path) {
    if (!std::filesystem::path(path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", path));
    }
    u64 totalSize = 0;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            totalSize += std::filesystem::file_size(entry);
        }
    }

    return totalSize;
}

std::string VirtualStore::ConcatenatePath(const std::string &dir_path, const std::string &file_path) {
    std::filesystem::path full_path = std::filesystem::path(dir_path) / file_path;
    return full_path.string();
}

std::mutex VirtualStore::mtx_;
std::unordered_map<std::string, MmapInfo> VirtualStore::mapped_files_;

i32 VirtualStore::MmapFile(const std::string &file_path, u8 *&data_ptr, size_t &data_len) {
    if (!std::filesystem::path(file_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", file_path));
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
    close(f);
    if (tmpd == MAP_FAILED)
        return -1;
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

std::ofstream VirtualStore::BeginCompress(const std::string &compressed_file) {
    std::ofstream output(compressed_file, std::ios::binary);
    return output;
}

Status VirtualStore::AddFileCompress(std::ofstream &ofstream, const std::string &input_file) {
    std::ifstream input(input_file, std::ios::binary);
    if (!input.is_open()) {
        return Status::IOError(fmt::format("Unable to open input file: {}", input_file));
    }

    input.seekg(0, input.end);
    size_t raw_file_size = input.tellg();
    input.seekg(0, input.beg);

    std::vector<char> raw_data(raw_file_size);
    input.read(raw_data.data(), raw_file_size);
    input.close();

    // Write the filename size
    size_t filename_size = input_file.size();
    ofstream.write(reinterpret_cast<char *>(&filename_size), sizeof(filename_size));

    // Write filename
    ofstream.write(input_file.c_str(), input_file.size());

    // Write raw file size
    ofstream.write(reinterpret_cast<char *>(&raw_file_size), sizeof(raw_file_size));

    // Write raw file md5
    std::string md5 = CalcMD5(raw_data.data(), raw_file_size);
    ofstream.write(md5.c_str(), MD5_DIGEST_LENGTH);

    size_t max_compressed_size = LZ4_compressBound(raw_file_size);
    std::vector<char> compressed_data(max_compressed_size);

    i32 compressed_size = LZ4_compress_default(raw_data.data(), compressed_data.data(), raw_file_size, max_compressed_size);
    if (compressed_size < 0) {
        return Status::IOError("Compression failed");
    }

    // Write compressed file size
    ofstream.write(reinterpret_cast<char *>(&compressed_size), sizeof(compressed_size));

    // Write compressed data
    ofstream.write(compressed_data.data(), compressed_size);
    return Status::OK();
}

void VirtualStore::EndCompress(std::ofstream &ofstream) { ofstream.close(); }

i32 VirtualStore::MunmapFile(const std::string &file_path) {
    if (!std::filesystem::path(file_path).is_absolute()) {
        UnrecoverableError(fmt::format("{} isn't absolute path.", file_path));
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

i32 VirtualStore::MmapFilePart(const std::string &file_path, size_t offset, size_t length, u8 *&data_ptr) {
    size_t align_offset = offset / getpagesize() * getpagesize();
    size_t diff = offset - align_offset;
    size_t align_length = length + diff;
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    void *ret = mmap(NULL, align_length, PROT_READ, MAP_SHARED, fd, align_offset);
    close(fd);
    if (ret == MAP_FAILED) {
        return -1;
    }
    data_ptr = reinterpret_cast<u8 *>(ret) + diff;
    return 0;
}

i32 VirtualStore::MunmapFilePart(u8 *data_ptr, size_t offset, size_t length) {
    size_t align_offset = offset / getpagesize() * getpagesize();
    u8 *aligned_ptr = data_ptr - offset + align_offset;
    size_t align_length = length + data_ptr - aligned_ptr;
    return munmap(aligned_ptr, align_length);
}

void VirtualStore::MunmapAllFiles() {
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto it = mapped_files_.begin(); it != mapped_files_.end(); ++it) {
        auto &mmap_info = it->second;
        munmap(mmap_info.data_ptr_, mmap_info.data_len_);
    }
    mapped_files_.clear();
}

// Remote storage
StorageType VirtualStore::storage_type_ = StorageType::kInvalid;
std::string VirtualStore::bucket_ = "infinity";
std::unique_ptr<S3Client> VirtualStore::s3_client_ = nullptr;
Atomic<u64> VirtualStore::total_request_count_ = 0;
Atomic<u64> VirtualStore::cache_miss_count_ = 0;

Status VirtualStore::InitRemoteStore(StorageType storage_type,
                                     const std::string &URL,
                                     bool HTTPS,
                                     const std::string &access_key,
                                     const std::string &secret_key,
                                     const std::string &bucket) {
    switch (storage_type) {
        case StorageType::kMinio: {
            storage_type_ = StorageType::kMinio;
            s3_client_ = std::make_unique<S3ClientMinio>(URL, HTTPS, access_key, secret_key);
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    bucket_ = bucket;
    return Status::OK();
}

Status VirtualStore::CreateBucket() {
    auto bucket_check_status = s3_client_->BucketExists(bucket_);
    if (!bucket_check_status.ok()) {
        if (bucket_check_status.code() == ErrorCode::kMinioBucketNotExists) {
            return s3_client_->MakeBucket(bucket_);
        } else {
            return bucket_check_status;
        }
    }
    return Status::OK();
}

Status VirtualStore::UnInitRemoteStore() {
    VirtualStore::storage_type_ = StorageType::kInvalid;
    s3_client_.reset();
    return Status::OK();
}

bool VirtualStore::IsInit() { return s3_client_.get() != nullptr; }

Status VirtualStore::DownloadObject(const std::string &file_path, const std::string &object_name) {
    if (VirtualStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            auto download_task = std::make_shared<DownloadTask>(file_path, object_name);
            auto object_storage_processor = infinity::InfinityContext::instance().storage()->object_storage_processor();
            object_storage_processor->Submit(download_task);
            download_task->Wait();
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

Status VirtualStore::UploadObject(const std::string &file_path, const std::string &object_name) {
    if (VirtualStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            auto upload_task = std::make_shared<UploadTask>(file_path, object_name);
            auto object_storage_processor = infinity::InfinityContext::instance().storage()->object_storage_processor();
            object_storage_processor->Submit(upload_task);
            upload_task->Wait();
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

Status VirtualStore::RemoveObject(const std::string &object_name) {
    if (VirtualStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            auto remove_task = std::make_shared<RemoveTask>(object_name);
            auto object_storage_processor = infinity::InfinityContext::instance().storage()->object_storage_processor();
            object_storage_processor->Submit(remove_task);
            remove_task->Wait();
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}
Status VirtualStore::CopyObject(const std::string &src_object_name, const std::string &dst_object_name) {
    if (VirtualStore::storage_type_ == StorageType::kLocal) {
        return Status::OK();
    }
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            auto copy_task = std::make_shared<CopyTask>(src_object_name, dst_object_name);
            auto object_storage_processor = infinity::InfinityContext::instance().storage()->object_storage_processor();
            object_storage_processor->Submit(copy_task);
            copy_task->Wait();
            break;
        }
        default: {
            return Status::NotSupport("Not support storage type");
        }
    }

    return Status::OK();
}

Status VirtualStore::BucketExists() {
    if (VirtualStore::storage_type_ == StorageType::kLocal) {
        return Status::InvalidStorageType("object_storage", "local");
    }
    switch (VirtualStore::storage_type_) {
        case StorageType::kMinio: {
            return s3_client_->BucketExists(VirtualStore::bucket_);
        }
        default: {
            return Status::InvalidStorageType("Unknown", "Unknown");
        }
    }
}

} // namespace infinity

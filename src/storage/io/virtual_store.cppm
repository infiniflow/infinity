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

export module infinity_core:virtual_store;

import :status;
import :local_file_handle;
import :stream_reader;
import :s3_client;
import :object_storage_process;

import std;
import third_party;

namespace infinity {

export enum class StorageType {
    kInvalid,
    kLocal,
    kMinio,
    kAwsS3,
    kAzureBlob, // Azure object store
    kGCS,       // Google cloud storage
    kOSS,       // Aliyun OSS
    kCOS,       // Tencent object store
    kOBS,       // Huawei object store
    kHDFS,
    kNFS,
};

export StorageType String2StorageType(const std::string &storage_type);

export std::string ToString(StorageType storage_type);

export struct MmapInfo {
    u8 *data_ptr_{};
    size_t data_len_{};
    size_t rc_{};
};

export class VirtualStore {
public:
    static std::tuple<std::unique_ptr<LocalFileHandle>, Status> Open(const std::string &path, FileAccessMode access_mode);
    static std::unique_ptr<StreamReader> OpenStreamReader(const std::string &path);
    static bool IsRegularFile(const std::string &path);
    static bool Exists(const std::string &path);
    static Status DeleteFile(const std::string &path);
    static Status DeleteFileBG(const std::string &path);
    static Status MakeDirectory(const std::string &path);
    static Status RemoveDirectory(const std::string &path);
    static Status CleanupDirectory(const std::string &path);
    static void RecursiveCleanupAllEmptyDir(const std::string &path);
    static Status Rename(const std::string &old_path, const std::string &new_path);
    static Status Truncate(const std::string &file_name, size_t new_length);
    static Status Merge(const std::string &dst_file, const std::string &src_file);
    static Status Copy(const std::string &dst_file, const std::string &src_file);
    static std::tuple<std::vector<std::shared_ptr<DirEntry>>, Status> ListDirectory(const std::string &path);
    static size_t GetFileSize(const std::string &path);
    static std::string GetParentPath(const std::string &path);
    static size_t GetDirectorySize(const std::string &path);
    static std::string ConcatenatePath(const std::string &dir_path, const std::string &file_path);

    static std::ofstream BeginCompress(const std::string& compressed_file);
    static Status AddFileCompress(std::ofstream& ofstream, const std::string &filename);
    static void EndCompress(std::ofstream& ofstream);

    static i32 MmapFile(const std::string &file_path, u8 *&data_ptr, size_t &data_len);
    static i32 MunmapFile(const std::string &file_path);

    static i32 MmapFilePart(const std::string &file_path, size_t offset, size_t length, u8 *&data_ptr);
    static i32 MunmapFilePart(u8 *data_ptr, size_t offset, size_t length);

    static void MunmapAllFiles();

    static Status InitRemoteStore(StorageType storage_type = StorageType::kMinio,
                                  const std::string &URL = "http://localhost:9000",
                                  bool HTTPS = false,
                                  const std::string &access_key = "minioadmin",
                                  const std::string &secret_key = "minioadmin",
                                  const std::string &bucket = "infinity");

    static Status UnInitRemoteStore();

    static bool IsInit();
    static Status CreateBucket();
    static Status DownloadObject(const std::string &file_dir, const std::string &object_name);
    static Status UploadObject(const std::string &file_dir, const std::string &object_name);
    static Status RemoveObject(const std::string &object_name);
    static Status CopyObject(const std::string &src_object_name, const std::string &dst_object_name);
    //
    static Status BucketExists();

    static void AddRequestCount() { ++total_request_count_; }
    static void AddCacheMissCount() { ++cache_miss_count_; }
    static u64 TotalRequestCount() { return total_request_count_; }
    static u64 CacheMissCount() { return cache_miss_count_; }

private:
    static std::mutex mtx_;
    static std::unordered_map<std::string, MmapInfo> mapped_files_;

    static StorageType storage_type_;
    static std::string bucket_;
    static std::unique_ptr<S3Client> s3_client_;

    static std::atomic<u64> total_request_count_;
    static std::atomic<u64> cache_miss_count_;

    friend class ObjectStorageProcess;
};

} // namespace infinity

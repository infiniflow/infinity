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

export module virtual_store;

import stl;
import status;
import third_party;
import local_file_handle;
import stream_reader;
import s3_client;

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

export StorageType String2StorageType(const String &storage_type);

export String ToString(StorageType storage_type);

export struct MmapInfo {
    u8 *data_ptr_{};
    SizeT data_len_{};
    SizeT rc_{};
};

export class VirtualStore {
public:
    static Tuple<UniquePtr<LocalFileHandle>, Status> Open(const String &path, FileAccessMode access_mode);
    static UniquePtr<StreamReader> OpenStreamReader(const String &path);
    static bool IsRegularFile(const String &path);
    static bool Exists(const String &path);
    static Status DeleteFile(const String &path);
    static Status MakeDirectory(const String &path);
    static Status RemoveDirectory(const String &path);
    static Status CleanupDirectory(const String &path);
    static Status Rename(const String &old_path, const String &new_path);
    static Status Truncate(const String &file_name, SizeT new_length);
    static Status Merge(const String &dst_file, const String &src_file);
    static Tuple<Vector<SharedPtr<DirEntry>>, Status> ListDirectory(const String &path);
    static SizeT GetFileSize(const String &path);
    static String GetParentPath(const String &path);
    static SizeT GetDirectorySize(const String &path);
    static String ConcatenatePath(const String &dir_path, const String &file_path);
    static i32 MmapFile(const String &file_path, u8 *&data_ptr, SizeT &data_len);
    static i32 MunmapFile(const String &file_path);

    static Status InitRemoteStore(StorageType storage_type = StorageType::kMinio,
                                  const String &URL = "http://localhost:9000",
                                  bool HTTPS = false,
                                  const String &access_key = "minioadmin",
                                  const String &secret_key = "minioadmin",
                                  const String &bucket = "infinity_bucket");

    static Status UnInitRemoteStore();

    static bool IsInit();
    static Status DownloadObject(const String &file_dir, const String& object_name);
    static Status UploadObject(const String &file_dir, const String& object_name);
    static Status RemoveObject(const String &object_name);
    static Status CopyObject(const String &src_object_name, const String &dst_object_name);

private:
    static std::mutex mtx_;
    static HashMap<String, MmapInfo> mapped_files_;

    static StorageType storage_type_;
    static String bucket_;
    static UniquePtr<S3Client> s3_client_;
};

} // namespace infinity

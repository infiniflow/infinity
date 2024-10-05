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
import virtual_storage_type;
import abstract_file_handle;

namespace infinity {

// Only one instance;
export class RemoteStore {
public:
    Status Init(StorageType storage_type, Map<String, String> &config);
    Status UnInit();

private:
    // Using by minio
    StorageType storage_type_{StorageType::kLocal};

    UniquePtr<minio::s3::BaseUrl> minio_base_url_{};
    UniquePtr<minio::creds::StaticProvider> minio_provider_{};
    UniquePtr<minio::s3::Client> minio_client_{};
};

export class LocalStore {
public:
    static Tuple<UniquePtr<LocalFileHandle>, Status> Open(const String& path, FileAccessMode access_mode);
    static bool IsRegularFile(const String& path);
    static bool Exists(const String& path);
    static Status DeleteFile(const String& path);
    static Status MakeDirectory(const String& path);
    static Status RemoveDirectory(const String& path);
    static Status CleanupDirectory(const String& path);
    static Status Rename(const String& old_path, const String& new_path);
    static Status Truncate(const String& file_name, SizeT new_length);
    static Status Merge(const String& dst_file, const String& src_file);
    static Tuple<Vector<SharedPtr<DirEntry>>, Status> ListDirectory(const String& path);
    static SizeT GetFileSize(const String& path);
    static String GetParentPath(const String& path);
    static SizeT GetDirectorySize(const String &path);
    static String ConcatenatePath(const String &dir_path, const String &file_path);
};

}

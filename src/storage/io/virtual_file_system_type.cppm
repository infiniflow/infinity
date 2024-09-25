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

import stl;

export module virtual_file_system_type;

namespace infinity {

export enum class FSType {
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

export FSType String2FSType(const String &fs_type) {
    if (std::strcmp(fs_type.c_str(), "local")) {
        return FSType::kLocal;
    } else if (std::strcmp(fs_type.c_str(), "minio")) {
        return FSType::kMinio;
    } else if (std::strcmp(fs_type.c_str(), "aws_s3")) {
        return FSType::kAwsS3;
    } else if (std::strcmp(fs_type.c_str(), "azure_blob")) {
        return FSType::kAzureBlob;
    } else if (std::strcmp(fs_type.c_str(), "gcs")) {
        return FSType::kGCS;
    } else if (std::strcmp(fs_type.c_str(), "oss")) {
        return FSType::kOSS;
    } else if (std::strcmp(fs_type.c_str(), "cos")) {
        return FSType::kCOS;
    } else if (std::strcmp(fs_type.c_str(), "obs")) {
        return FSType::kOBS;
    } else if (std::strcmp(fs_type.c_str(), "hdfs")) {
        return FSType::kHDFS;
    } else if (std::strcmp(fs_type.c_str(), "nfs")) {
        return FSType::kNFS;
    } else {
        return FSType::kInvalid;
    }
}

}
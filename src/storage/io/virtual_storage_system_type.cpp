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

#include <string>

module virtual_storage_system_type ;

import stl;

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
}

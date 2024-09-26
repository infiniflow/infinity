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

module minio_file;

import status;
import stl;
import infinity_context;

namespace infinity {

Status MinioFile::Open(const String &path, FileAccessMode access_mode){
    String bucket_name = InfinityContext::instance().config()->ObjectStorageBucket();

    if (!std::filesystem::path(path).is_absolute()) {
        String error_message = fmt::format("{} isn't absolute path.", path);
        return Status::SyntaxError(error_message);
    }

    if(!std::filesystem::exists(path)){
        minio::s3::Client * client = storage_system_->GetMinioClient();
        
    }


    return Status::OK();
}

}
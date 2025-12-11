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

#include "unit_test/gtest_expand.h"

module infinity_core:ut.s3_client_minio;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :file_writer;
import :file_reader;
import :infinity_context;
import :virtual_store;

import global_resource_usage;

using namespace infinity;

class S3ClientMinioTest : public BaseTest {};

TEST_F(S3ClientMinioTest, test1) {
    // std::string url("http://localhost:9000");
    // std::string access_key("minioadmin");
    // std::string secret_key("minioadmin");
    // std::string bucket("infinity");
    // Status status = VirtualStore::InitRemoteStore(StorageType::kMinio, url, false, access_key, secret_key, bucket);
    // ASSERT_TRUE(status.ok());
}
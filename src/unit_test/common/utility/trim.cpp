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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import stl;
import global_resource_usage;
import infinity_context;

using namespace infinity;
class TrimTest : public BaseTest {};

TEST_F(TrimTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");
}

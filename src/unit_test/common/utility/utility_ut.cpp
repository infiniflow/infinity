// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.utility;

import :ut.base_test;
import :infinity_exception;
import :utility;
import :infinity_context;
#endif

using namespace infinity;
class UtilityTest : public BaseTest {};

TEST_F(UtilityTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");

    String key = "idx_seg|1|45|3|10";
    String prefix = "idx_seg|1|45|3|";
    auto [segment_id, is_segment_id] = infinity::ExtractU64FromStringSuffix(key, prefix.size());
    EXPECT_EQ(segment_id, 10);
    EXPECT_EQ(is_segment_id, true);
}

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

#include "gtest/gtest.h"

import base_test;

import infinity_exception;
import third_party;
import stl;
import utility;
import infinity_context;

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

TEST_F(UtilityTest, test_rapidjson) {

    // 1. Parse a JSON string into DOM.
    const char *json = "{\"project\":\"rapidjson\",\"stars\":10}";
    rapidjson::Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    rapidjson::Value &s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
}

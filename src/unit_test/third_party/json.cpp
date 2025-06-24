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
import third_party;
import logger;
import compilation_config;
import infinity_context;

using namespace infinity;

class JsonTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(JsonTest, test1) {
    using namespace infinity;

    String json_path = String(test_data_path()) + "/json/twitter.json";
    std::ifstream f(json_path);

    nlohmann::json data = nlohmann::json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}

TEST_F(JsonTest, test2) {
    using namespace infinity;

    String json_path = String(test_data_path()) + "/json/twitter.json";
    //    LOG_TRACE("JSON Path: {}", json_path);
    simdjson::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load(json_path);
    simdjson::document tweets = parser.iterate(json);
    EXPECT_EQ((uint64_t)tweets["search_metadata"]["count"].get<uint64_t>(), 100);
}

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

TEST_F(JsonTest, nlohmann_test) {
    using namespace infinity;

    String json_path = String(test_data_path()) + "/json/twitter.json";
    std::ifstream f(json_path);

    nlohmann::json data = nlohmann::json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}

TEST_F(JsonTest, simdjson_test) {
    using namespace infinity;
    {
        String json_path = String(test_data_path()) + "/json/twitter.json";
        //    LOG_TRACE("JSON Path: {}", json_path);
        simdjson::parser parser;
        simdjson::padded_string json = simdjson::padded_string::load(json_path);
        simdjson::document tweets = parser.iterate(json);
        EXPECT_EQ((uint64_t)tweets["search_metadata"]["count"].get<uint64_t>(), 100);
    }
    {
        std::function<void(std::string_view)> PrintDocument = [&](std::string_view json_str) -> void {
            simdjson::padded_string json_pad(json_str);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            switch (doc.type()) {
                case simdjson::json_type::boolean: {
                    bool res = doc.get<bool>();
                    std::cout << res << std::endl;
                    break;
                }
                case simdjson::json_type::string: {
                    String res = doc.get<std::string>();
                    std::cout << res << std::endl;
                    break;
                }
                case simdjson::json_type::number: {
                    switch (doc.get_number_type()) {
                        case simdjson::number_type::signed_integer:
                        case simdjson::number_type::unsigned_integer: {
                            int64_t res = doc.get<int64_t>();
                            std::cout << res << " (int)" << std::endl;
                            break;
                        }
                        case simdjson::number_type::floating_point_number: {
                            double res = doc.get<double>();
                            std::cout << res << " (double)" << std::endl;
                            break;
                        }
                        default: {
                            throw std::runtime_error("Unsupported json type");
                        }
                    }
                    break;
                }
                case simdjson::json_type::null: {
                    std::cout << "null" << std::endl;
                    break;
                }
                case simdjson::json_type::array: {
                    std::cout << "[";
                    for (int i = 0; auto item : doc.get_array()) {
                        if (i++) std::cout << ",";
                        std::cout << (i64)item.get<i64>();
                    }
                    std::cout << "]" << std::endl;
                    break;
                }
                case simdjson::json_type::object: {
                    for (auto field : doc.get_object()) {
                        String field_key = String((std::string_view)field.unescaped_key());
                        std::cout << field_key << ": ";
                        PrintDocument(field.value().raw_json());
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Unsupported json type");
                }
            }
        };

        String text = "{\"hello\":\"world\",\"t\":true,\"f\":false,\"n\":null,\"i\":123,\"pi\":3.1416,\"a\":[0,1,2],"
                      "\"o\":{\"v0\":\"v0\",\"v1\":\"v1\",\"v2\":\"v2\"}}";
        PrintDocument(text);
    }
}


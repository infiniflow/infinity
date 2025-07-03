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

TEST_F(JsonTest, nlohmannjson_text) {
    using namespace infinity;

    String json_path = String(test_data_path()) + "/json/twitter.json";
    std::ifstream f(json_path);

    nlohmann::json data = nlohmann::json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}

TEST_F(JsonTest, simdjson_test) {
    using namespace infinity;

    String json_path = String(test_data_path()) + "/json/twitter.json";
    //    LOG_TRACE("JSON Path: {}", json_path);
    simdjson::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load(json_path);
    simdjson::document tweets = parser.iterate(json);
    EXPECT_EQ((uint64_t)tweets["search_metadata"]["count"].get<uint64_t>(), 100);
}

TEST_F(JsonTest, rapidjson_test) {
    using namespace infinity;

    // test writer
    String text;
    {
        class ArrayList {
        public:
            void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
                writer.StartArray();
                for (unsigned i = 0; i < 3; i++) {
                    writer.Uint(i);
                }
                writer.EndArray();
            }
        };

        class KVList {
        public:
            void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
                writer.StartObject();
                for (unsigned i = 0; i < 3; i++) {
                    String key = "k" + std::to_string(i);
                    String val = "v" + std::to_string(i);
                    writer.Key(val.c_str());
                    writer.String(val.c_str());
                }
                writer.EndObject();
            }
        };

        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.Key("hello");
        writer.String("world");
        writer.Key("t");
        writer.Bool(true);
        writer.Key("f");
        writer.Bool(false);
        writer.Key("n");
        writer.Null();
        writer.Key("i");
        writer.Uint(123);
        writer.Key("pi");
        writer.Double(3.1416);
        writer.Key("a");
        ArrayList arrls;
        arrls.Serialize(writer);
        writer.Key("o");
        KVList kvl;
        kvl.Serialize(writer);
        writer.EndObject();

        text = sb.GetString();
        std::cout << "text: " << text << std::endl;
    }

    // read writer test
    {
        std::function<void(const rapidjson::Value&)> PrintDocument = [&](const rapidjson::Value& val) -> void {
            switch (val.GetType()) {
                case rapidjson::Type::kTrueType: {
                    auto res = val.GetBool();
                    ASSERT_EQ(res, true);
                    std::cout << res << std::endl;
                    break;
                }
                case rapidjson::Type::kFalseType: {
                    auto res = val.GetBool();
                    ASSERT_EQ(res, false);
                    std::cout << res << std::endl;
                    break;
                }
                case rapidjson::Type::kStringType: {
                    auto res = val.GetString();
                    std::cout << res << std::endl;
                    break;
                }
                case rapidjson::Type::kNumberType: {
                    if (val.IsInt()) {
                        auto res = val.GetInt();
                        std::cout << res << " (int)" << std::endl;
                    } else if (val.IsFloat()) {
                        auto res = val.GetFloat();
                        std::cout << res << " (float)" << std::endl;
                    } else if (val.IsDouble()) {
                        auto res = val.GetDouble();
                        std::cout << res << " (double)" << std::endl;
                    }
                    break;
                }
                case rapidjson::Type::kNullType: {
                    std::cout << "null" << std::endl;
                    break;
                }
                case rapidjson::Type::kArrayType: {
                    rapidjson::StringBuffer sb;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
                    val.Accept(writer);
                    std::cout << sb.GetString();
                    int sum = 0;
                    for (auto &arr_obj: val.GetArray()) {
                        sum += arr_obj.GetInt();
                    }
                    std::cout << ", sum = " << sum << std::endl;
                    break;
                }
                case rapidjson::Type::kObjectType: {
                    for (auto &obj: val.GetObject()) {
                        std::cout << obj.name.GetString() << ": ";
                        PrintDocument(obj.value);
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Unsupported json type");
                }
            }
        };

        rapidjson::Document doc;
        doc.Parse(text.c_str());
        ASSERT_EQ(doc.IsObject(), true);
        if (doc.FindMember("None") == doc.MemberEnd()) {
            std::cout << "member \"None\" not exists." << std::endl;
        }

        // Count
        std::cout << "size(doc): " << doc.Size() << std::endl;
        std::cout << "size(doc::array): " << doc["a"].Size() << std::endl;

        // Random Access
        const auto &item = doc[0];
        std::cout << "idx0: " << item.GetString() << std::endl;

        // Print document
        PrintDocument(doc);
    }

    // read from file
    {
        String json_path = String(test_data_path()) + "/json/twitter.json";
        std::ifstream f(json_path);
        std::stringstream buffer;
        buffer << f.rdbuf();

        rapidjson::Document tweets;
        tweets.Parse(buffer.str().c_str());
        EXPECT_EQ(tweets["search_metadata"]["count"].GetInt64(), 100);
    }
}
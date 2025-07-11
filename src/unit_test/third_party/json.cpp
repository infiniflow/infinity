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


TEST_F(JsonTest, rapidjson_test) {
    using namespace infinity;

    // test writer
    String text;
    {
        class KVList {
        public:
            void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
                for (unsigned i = 0; i < 3; i++) {
                    String key = "k" + std::to_string(i);
                    String val = "v" + std::to_string(i);
                    writer.Key(val.c_str());
                    writer.String(val.c_str());
                }
            }
        };

        auto fun = [&](rapidjson::Writer<rapidjson::StringBuffer> &writer) -> void {
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
            writer.StartArray();
            for (unsigned i = 0; i < 3; i++) {
                writer.Uint(i);
            }
            writer.EndArray();

            writer.Key("o");
            writer.StartObject();
            KVList kvl;
            kvl.Serialize(writer);
            writer.EndObject();
        };

        rapidjson::StringBuffer sb;
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            fun(writer);
            writer.EndObject();
        }
        {
            rapidjson::Document doc;
            doc.Parse(sb.GetString());
            sb.Clear();
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
            doc.Accept(writer);
            writer.SetIndent(' ', 4);
        }
        text = sb.GetString();
        std::cout << "text: " << text << std::endl;
    }

    // read writer test
    {
        std::function<void(const rapidjson::Value &)> PrintDocument = [&](const rapidjson::Value &val) -> void {
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
                    for (auto &arr_obj : val.GetArray()) {
                        sum += arr_obj.GetInt();
                    }
                    std::cout << ", sum = " << sum << std::endl;
                    break;
                }
                case rapidjson::Type::kObjectType: {
                    for (auto &obj : val.GetObject()) {
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
        std::cout << "size(doc): " << doc.MemberCount() << std::endl;
        std::cout << "size(doc::array): " << doc["a"].Size() << std::endl;

        // Random Access
        const auto &item = doc["a"][0];
        std::cout << "array[0]: " << item.GetInt() << std::endl;

        // Print document
        PrintDocument(doc);
    }

    // Modify context
    {
        rapidjson::Document doc;
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        doc.Parse("{}");
        {
            rapidjson::Document doc_inner;
            rapidjson::Document::AllocatorType& allocator_inner = doc.GetAllocator();
            doc_inner.Parse("[]");
            doc_inner.PushBack(0, allocator_inner);
            doc_inner.PushBack(1, allocator_inner);
            doc_inner.PushBack(2, allocator_inner);
            doc.AddMember("i", doc_inner, allocator);
        }
        {
            rapidjson::Document doc_inner;
            rapidjson::Document::AllocatorType& allocator_inner = doc.GetAllocator();
            doc_inner.Parse("[]");
            doc_inner.PushBack(0.1, allocator_inner);
            doc_inner.PushBack(1.1, allocator_inner);
            doc_inner.PushBack(2.1, allocator_inner);
            doc.AddMember("f", doc_inner, allocator);
        }
        {
            rapidjson::Document doc_inner;
            rapidjson::Document::AllocatorType& allocator_inner = doc.GetAllocator();
            doc_inner.Parse("[]");
            doc_inner.PushBack("a", allocator_inner);
            doc_inner.PushBack("b", allocator_inner);
            doc_inner.PushBack("c", allocator_inner);
            doc.AddMember("s", doc_inner, allocator);
        }

        rapidjson::StringBuffer sb;
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            doc.Accept(writer);
        }
        String json_str = sb.GetString();
        std::cout << json_str << std::endl;
        EXPECT_EQ(json_str, "{\"i\":[0,1,2],\"f\":[0.1,1.1,2.1],\"s\":[\"a\",\"b\",\"c\"]}");
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

    // test mix set
    {
        rapidjson::StringBuffer sb;
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            {
                writer.Key("res");
                rapidjson::Document doc;
                doc.Parse("{\"name\":\"hello\",\"value\":123}");
                doc.Accept(writer);
            }
            writer.EndObject();
        }
        std::cout << sb.GetString() << std::endl;
    }
}
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

#include "type/number/bfloat16.h"
#include "type/number/float16.h"
#include "unit_test/gtest_expand.h"

module infinity_core:ut.value_embedding;

import :ut.base_test;
import :infinity_exception;
import :logger;
import :column_vector;
import :value;
import :default_values;
import :selection;
import :vector_buffer;
import :infinity_context;

import embedding_type;
import third_party;
import global_resource_usage;
import internal_types;
import logical_type;
import embedding_info;
import knn_expr;
import data_type;
import compilation_config;

using namespace infinity;

class Value2JsonTest : public BaseTest {};

TEST_F(Value2JsonTest, test_embedding) {
    using namespace infinity;
    nlohmann::json json;

    {
        std::string name = "embedding";
        nlohmann::json json_embedding;
        json[name] = json_embedding;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemBit, 16);
        std::vector<bool> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            data[j] = true;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "int8";
        nlohmann::json json_int8;
        json[name] = json_int8;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemInt8, 16);
        std::vector<int8_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<int8_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "int16";
        nlohmann::json json_int16;
        json[name] = json_int16;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemInt16, 16);
        std::vector<int16_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<int16_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "int32";
        nlohmann::json json_int32;
        json[name] = json_int32;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemInt32, 16);
        std::vector<int32_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<int32_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "int64";
        nlohmann::json json_int64;
        json[name] = json_int64;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, 16);
        std::vector<int64_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<int64_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "float";
        nlohmann::json json_float;
        json[name] = json_float;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
        std::vector<float> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<float>(0.1 * j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "double";
        nlohmann::json json_double;
        json[name] = json_double;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemDouble, 16);
        std::vector<double> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<double>(0.1 * j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "uint8";
        nlohmann::json json_uint8;
        json[name] = json_uint8;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemUInt8, 16);
        std::vector<uint8_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<uint8_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "float16";
        nlohmann::json json_float16;
        json[name] = json_float16;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat16, 16);
        std::vector<float16_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<uint16_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    {
        std::string name = "bfloat16";
        nlohmann::json json_bfloat16;
        json[name] = json_bfloat16;

        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemBFloat16, 16);
        std::vector<bfloat16_t> data((i64)embedding_info->Dimension());

        for (i64 j = 0; j < (i64)embedding_info->Dimension(); ++j) {
            auto tmp = std::make_shared<uint16_t>(j);
            data[j] = *tmp;
        }
        Value v = Value::MakeEmbedding(data);
        v.AppendToJson(name, json);
    }

    LOG_INFO(fmt::format("test_embedding's json: {}", json.dump()));
}

TEST_F(Value2JsonTest, test_sparse) {
    using namespace infinity;
    nlohmann::json json;

    {
        std::string name = "float_int32_t";
        nlohmann::json json_float_int32_t;
        json[name] = json_float_int32_t;

        std::pair<std::vector<float>, std::vector<int32_t>> vec{std::vector<float>{1.0, 2.0, 3.0, 4.0},
                                                                std::vector<int32_t>{100, 1000, 10000, 20000}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }

    {
        std::string name = "double_int64_t";
        nlohmann::json json_double_int64_t;
        json[name] = json_double_int64_t;

        std::pair<std::vector<double>, std::vector<int64_t>> vec{std::vector<double>{1.0, 2.0, 3.0, 4.0},
                                                                 std::vector<int64_t>{100, 1000, 10000, 20000}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemDouble, EmbeddingDataType::kElemInt64, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }

    {
        std::string name = "double_int8_t";
        nlohmann::json json_double;
        json[name] = json_double;

        std::pair<std::vector<double>, std::vector<int8_t>> vec{std::vector<double>{1.0, 2.0, 3.0, 4.0}, std::vector<int8_t>{1, 2, 3, 4}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemDouble, EmbeddingDataType::kElemInt8, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }

    {
        std::string name = "double_int16_t";
        nlohmann::json json_double;
        json[name] = json_double;

        std::pair<std::vector<double>, std::vector<int16_t>> vec{std::vector<double>{1.0, 2.0, 3.0, 4.0}, std::vector<int16_t>{1, 2, 3, 4}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemDouble, EmbeddingDataType::kElemInt16, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }

    {
        std::string name = "double_int32_t";
        nlohmann::json json_double;
        json[name] = json_double;

        std::pair<std::vector<double>, std::vector<int32_t>> vec{std::vector<double>{1.0, 2.0, 3.0, 4.0}, std::vector<int32_t>{1, 2, 3, 4}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemDouble, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }

    {
        std::string name = "double_int16_t";
        nlohmann::json json_double;
        json[name] = json_double;

        std::pair<std::vector<double>, std::vector<int64_t>> vec{std::vector<double>{1.0, 2.0, 3.0, 4.0}, std::vector<int64_t>{1, 2, 3, 4}};

        auto column_typeinfo =
            std::make_shared<SparseInfo>(EmbeddingDataType::kElemDouble, EmbeddingDataType::kElemInt64, 30000, SparseStoreType::kSort);
        auto v = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                   reinterpret_cast<const char *>(vec.second.data()),
                                   vec.first.size(),
                                   column_typeinfo);
        v.AppendToJson(name, json);
        LOG_INFO(fmt::format("test_sparse's json: {}", json.dump()));
    }
}

TEST_F(Value2JsonTest, test_tensor) {
    using namespace infinity;
    nlohmann::json json;

    {
        nlohmann::json json_tensor;
        std::string name = "kElemFloat";
        json[name] = json_tensor;

        auto type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);

        std::vector<float> vec1 = {1.0, 2.0, 3.0, 4.0};
        std::vector<float> vec2 = {5.0, 6.0, 7.0, 8.0};

        std::vector<std::pair<char *, size_t>> embedding_data;
        embedding_data.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
        embedding_data.emplace_back(std::make_pair(reinterpret_cast<char *>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
        Value v = Value::MakeTensor(embedding_data, type_info);
        LOG_INFO(v.ToString());

        v.AppendToJson(name, json);
    }

    {
        nlohmann::json json_tensor;
        std::string name = "kElemDouble";
        json[name] = json_tensor;

        auto type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemDouble, 4);

        std::vector<double> vec1 = {1.0, 2.0, 3.0, 4.0};
        std::vector<double> vec2 = {5.0, 6.0, 7.0, 8.0};

        std::vector<std::pair<char *, size_t>> embedding_data;
        embedding_data.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
        embedding_data.emplace_back(std::make_pair(reinterpret_cast<char *>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
        Value v = Value::MakeTensor(embedding_data, type_info);
        LOG_INFO(v.ToString());

        v.AppendToJson(name, json);
    }

    LOG_INFO(fmt::format("test_tensor's json: {}", json.dump()));
}

TEST_F(Value2JsonTest, test_tensor_array) {
    using namespace infinity;

    std::vector<EmbeddingDataType> types = {EmbeddingDataType::kElemBit,
                                            EmbeddingDataType::kElemInt8,
                                            EmbeddingDataType::kElemInt16,
                                            EmbeddingDataType::kElemInt32,
                                            EmbeddingDataType::kElemInt64,
                                            EmbeddingDataType::kElemFloat,
                                            EmbeddingDataType::kElemDouble,
                                            EmbeddingDataType::kElemUInt8,
                                            EmbeddingDataType::kElemFloat16,
                                            EmbeddingDataType::kElemBFloat16};

    for (const auto &type : types) {
        nlohmann::json json;
        nlohmann::json json_tensor_array;
        std::string name = "test_tensor_array";
        json[name] = json_tensor_array;
        auto type_info_ptr = EmbeddingInfo::Make(type, 16);
        Value v = Value::MakeTensorArray(std::move(type_info_ptr));
        v.AppendToJson(name, json);

        LOG_INFO(fmt::format("Convert tensor array to string", v.ToString()));
    }
}

TEST_F(Value2JsonTest, test_basic_type) {
    using namespace infinity;
    nlohmann::json json;

    {
        nlohmann::json test_basic_type;
        std::string name = "float";
        json[name] = test_basic_type;

        float origin = 0.01;
        Value v = Value::MakeFloat(origin);
        v.AppendToJson(name, json);

        float now = v.ToFloat();
        ASSERT_NEAR(origin, now, 0.01);
    }

    {
        nlohmann::json test_basic_type;
        std::string name = "double";
        json[name] = test_basic_type;

        double origin = 0.01;
        Value v = Value::MakeDouble(origin);
        v.AppendToJson(name, json);

        double now = v.ToDouble();
        ASSERT_NEAR(origin, now, 0.01);
    }

    LOG_INFO(fmt::format("test_basic_type's json: {}", json.dump()));
}
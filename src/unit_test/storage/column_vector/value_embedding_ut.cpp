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

class ValueEmbeddingTest : public BaseTest {};

TEST_F(ValueEmbeddingTest, test_json) {
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

    // // Value::MakeTensorArray(type_info_ptr={}) is not supported!
    // {
    //     std::string name = "tensor_array";
    //     nlohmann::json json_tensor_array;
    //     json[name] = json_tensor_array;
    //
    //     DataType datatype(LogicalType::kTensorArray);
    //     auto info = ArrayInfo::Make(datatype);
    //     Value v = Value::MakeTensorArray(info);
    //     v.AppendToJson(name, json);
    // }
}
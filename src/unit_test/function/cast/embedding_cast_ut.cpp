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

#ifdef CI
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.embedding_cast;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :function_set;
import :aggregate_function_set;
import :aggregate_function;
import :function;
import :column_expression;
import :value;
import :default_values;
import :data_block;
import :embedding_cast;
import :column_vector;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import embedding_info;
import knn_expr;
import data_type;

using namespace infinity;
class EmbeddingCastTest : public BaseTest {
    void SetUp() override { BaseTest::SetUp(); }

    void TearDown() override { BaseTest::TearDown(); }
};

TEST_F(EmbeddingCastTest, embedding_cast1) {
    using namespace infinity;

    // Call BindEmbeddingCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindEmbeddingCast(source_type, target_type), RecoverableException);
    }

    auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
    std::shared_ptr<DataType> source_type = std::make_shared<DataType>(LogicalType::kEmbedding, embedding_info);
    auto col_source = std::make_shared<ColumnVector>(source_type);
    col_source->Initialize();
    Vector<float> data(embedding_info->Dimension());
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        for (size_t j = 0; j < embedding_info->Dimension(); ++j) {
            data[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        Value v = Value::MakeEmbedding(data);
        col_source->AppendValue(v);
    }

    for (i64 i = 0; i < 1; ++i) {
        for (size_t j = 0; j < embedding_info->Dimension(); ++j) {
            data[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        Value v = Value::MakeEmbedding(data);
        Value vx = col_source->GetValueByIndex(i);
        EXPECT_EQ(v == vx, true);
    }

    // cast embedding float column vector to embedding double column vector
    {
        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemDouble, 16);
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kEmbedding, embedding_info);
        auto source2target_ptr = BindEmbeddingCast(*source_type, *target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        auto col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_TRUE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters));

        Vector<double> data2(embedding_info->Dimension());
        for (i64 i = 0; i < 1; ++i) {
            for (size_t j = 0; j < embedding_info->Dimension(); ++j) {
                data2[j] = double(static_cast<float>(i) + static_cast<float>(j) + 0.5f);
            }
            Value v = Value::MakeEmbedding(data2);
            Value vx = col_target->GetValueByIndex(i);
            EXPECT_EQ(v == vx, true);
        }
    }
}

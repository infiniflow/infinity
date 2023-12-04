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

#include "unit_test/base_test.h"

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import new_catalog;
import parser;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import embedding_cast;
import column_vector;

class EmbeddingCastTest : public BaseTest {};

// TEST_F(EmbeddingCastTest, uuid_cast0) {
//     using namespace infinity;
//
//     // Try to cast uuid type to wrong type.
//     {
//         auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
//         DataType source_data_type(LogicalType::kEmbedding, embedding_info);
//
//         EmbeddingT source = EmbeddingType(EmbeddingDataType::kElemFloat, 16);
//         for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
//             ((float*)(source.ptr))[j] = static_cast<float>(j) + 0.5f;
//         }
//
//         TinyIntT target;
//         DataType target_data_type(LogicalType::kTinyInt);
//         EXPECT_THROW(EmbeddingTryCastToVarlen::Run(source, source_data_type, target, target_data_type, nullptr), FunctionException);
//     }
//     {
//         auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
//         DataType source_data_type(LogicalType::kEmbedding, embedding_info);
//
//         EmbeddingT source = EmbeddingType(EmbeddingDataType::kElemFloat, 16);
//         for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
//             ((float*)(source.ptr))[j] = static_cast<float>(j) + 0.5f;
//         }
//
//         VarcharT target;
//
//         DataType target_data_type(LogicalType::kVarchar);
//
//         ColumnVector col_varchar(target_data_type);
//         col_varchar.Initialize();
//
//         EXPECT_TRUE(EmbeddingTryCastToVarlen::Run(source, source_data_type, target, target_data_type, &col_varchar));
//
//         target.Reset(false);
//     }
// }

TEST_F(EmbeddingCastTest, embedding_cast1) {
#if 0
    using namespace infinity;

    // Call BindEmbeddingCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindEmbeddingCast(target_type), TypeException);
    }

    auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kEmbedding, embedding_info);
    ColumnVector col_source(source_type);
    col_source.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for (SizeT j = 0; j < embedding_info->Dimension(); ++j) {
            ((float *)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        col_source.AppendValue(v);
        v.value_.embedding.Reset();
    }

    for (i64 i = 0; i < 1; ++i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for (SizeT j = 0; j < embedding_info->Dimension(); ++j) {
            ((float *)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }

        Value vx = col_source.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->Size(), 64);

        for (SizeT j = 0; j < embedding_info->Dimension(); ++j) {
            EXPECT_FLOAT_EQ(((float *)(vx.value_.embedding.ptr))[j], ((float *)(v.value_.embedding.ptr))[j]);
        }

        v.value_.embedding.Reset();
    }

    // cast uuid column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindEmbeddingCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type);
        col_target.Initialize();

// FIXME: Embedding type width isn't sizeof(EmbeddingT)
// But UnaryExecute will only use the sizeof(type) to iterate each value.
        CastParameters cast_parameters;
        EXPECT_TRUE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters));

        for(i64 i = 0; i < 1; ++ i) {
            Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
            for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
                ((float*)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
            }
            String source_str = EmbeddingT::Embedding2String(v.value_.embedding,
                                                             embedding_info->Type(),
                                                             embedding_info->Dimension());
            v.value_.embedding.Reset();

            Value vx = col_target.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_FALSE(vx.is_null());
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), source_str.c_str());
        }
    }
#endif
}

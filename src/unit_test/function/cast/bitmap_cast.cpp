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

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;


import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import cast_table;
import column_vector;
import bitmap_cast;
import bound_cast_func;

#if 0
using namespace infinity;
class BitmapCastTest : public BaseTest {};

TEST_F(BitmapCastTest, bitmap_cast0) {
    using namespace infinity;

    // Try to cast bitmap type to wrong type.
    {
        BitmapT source;
        source.Initialize(128);
        for (i64 j = 0; j < 128; ++j) {
            if (j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }

        TinyIntT target;
        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        BitmapT source;
        source.Initialize(128);
        for (i64 j = 0; j < 128; ++j) {
            if (j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
}

TEST_F(BitmapCastTest, bitmap_cast1) {
    using namespace infinity;

    // Call BindBitmapCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindBitmapCast(target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kBitmap);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for (i64 j = 0; j <= i; ++j) {
            if (j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for (i64 j = 0; j <= i; ++j) {
            if (j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBitmap);
        EXPECT_EQ(vx.value_.bitmap, bitmap);
    }
    // cast bitmap column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindBitmapCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }
}
#endif
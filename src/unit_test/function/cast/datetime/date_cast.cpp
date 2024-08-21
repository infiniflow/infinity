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
import date_cast;
import bound_cast_func;
import internal_types;
import logical_type;
import data_type;

class DateCastTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
    }

    void TearDown() override {
        BaseTest::TearDown();
    }
};

TEST_F(DateCastTest, date_cast0) {
    using namespace infinity;

    // Try to cast date type to wrong type.
    {
        DateT source;
        TinyIntT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
        EXPECT_THROW(DateTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        DateT source;
        DateTimeT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        DateT source;
        TimestampT target;
        EXPECT_THROW(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        DateT source;
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(DateTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
}

TEST_F(DateCastTest, date_cast1) {
    using namespace infinity;

    // Call BindDateCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindDateCast(target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kDate);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDate(DateT(static_cast<i32>(i)));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_FLOAT_EQ(vx.value_.date.value, static_cast<i32>(i));
    }

    // cast date column vector to datetime column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDateTime);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }
    // cast date column vector to timestamp column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTimestamp);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }

    // cast date column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }
}

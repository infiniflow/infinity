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

module infinity_core:ut.time_cast;

import :ut.base_test;
import :infinity_exception;
import :third_party;
import :logger;
import :stl;
import :infinity_context;
import :function_set;
import :aggregate_function_set;
import :aggregate_function;
import :function;
import :column_expression;
import :value;
import :default_values;
import :data_block;
import :cast_table;
import :column_vector;
import :time_cast;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;

class TimeCastTest : public BaseTest {};

TEST_F(TimeCastTest, date_cast0) {
    using namespace infinity;

    // Try to cast time type to wrong type.
    {
        TimeT source;
        TinyIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(TimeTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        TimeT source;
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar = MakeShared<ColumnVector>(data_type);
        col_varchar->Initialize();

        EXPECT_THROW_WITHOUT_STACKTRACE(TimeTryCastToVarlen::Run(source, target, col_varchar.get()), UnrecoverableException);
    }
}

TEST_F(TimeCastTest, date_cast1) {
    using namespace infinity;

    // Call BindDateCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindTimeCast(target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kTime);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeTime(TimeT(static_cast<i32>(i)));
        col_source->AppendValue(v);
        Value vx = col_source->GetValueByIndex(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTime);
        EXPECT_FLOAT_EQ(vx.value_.time.value, static_cast<i32>(i));
    }
    // cast time column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindTimeCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target(MakeShared<ColumnVector>(target_type));
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }
}

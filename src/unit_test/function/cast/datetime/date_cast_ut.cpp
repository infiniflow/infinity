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

module infinity_core:ut.data_cast;

import :ut.base_test;
import :infinity_exception;
import third_party;
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
import :date_cast;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;

class DateCastTest : public BaseTest {};

TEST_F(DateCastTest, date_cast0) {
    using namespace infinity;

    // Try to cast date type to wrong type.
    {
        DateT source;
        TinyIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
        EXPECT_THROW_WITHOUT_STACKTRACE(DateTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        DateT source;
        DateTimeT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        DateT source;
        TimestampT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DateTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        DateT source;
        VarcharT target;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kVarchar);
        std::shared_ptr<ColumnVector> col_varchar_ptr = std::make_shared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW_WITHOUT_STACKTRACE(DateTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
}

TEST_F(DateCastTest, date_cast1) {
    using namespace infinity;

    // Call BindDateCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDateCast(target_type), UnrecoverableException);
    }

    std::shared_ptr<DataType> source_type = std::make_shared<DataType>(LogicalType::kDate);
    std::shared_ptr<ColumnVector> col_source = std::make_shared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDate(DateT(static_cast<i32>(i)));
        col_source->AppendValue(v);
        Value vx = col_source->GetValueByIndex(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_FLOAT_EQ(vx.value_.date.value, static_cast<i32>(i));
    }

    // cast date column vector to datetime column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kDateTime);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }
    // cast date column vector to timestamp column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kTimestamp);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast date column vector to varchar column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindDateCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }
}

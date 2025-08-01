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

module infinity_core:ut.hugeint_cast;

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
import :integer_cast;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;
class HugeIntCastTest : public BaseTest {};

TEST_F(HugeIntCastTest, hugeint_cast0) {
    using namespace infinity;

    // HugeInt to HugeInt, throw exception
    {
        HugeIntT source{0, 0};
        HugeIntT target{0, 0};

        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        TinyIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        SmallIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        IntegerT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        BigIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        FloatT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        DoubleT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        DecimalT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
    // HugeInt to Date, throw exception
    {
        HugeIntT source{0, 0};
        DateT target{0};

        EXPECT_THROW_WITHOUT_STACKTRACE(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
}

TEST_F(HugeIntCastTest, hugeint_cast1) {
    using namespace infinity;

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kHugeInt);

    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeHugeInt(HugeIntT(i, i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValueByIndex(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, HugeIntT(i, i));
    }

    // cast hugeint column vector to tiny int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTinyInt);
        auto hugeint2tiny_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast hugeint column vector to small integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kSmallInt);
        auto hugeint2small_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2small_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast big int column vector to int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kInteger);
        auto hugeint2int_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2int_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2int_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast hugeint column vector to big int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kBigInt);
        auto hugeint2bigint_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2bigint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast hugeint column vector to float column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kFloat);
        auto hugeint2float_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2float_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast hugeint column vector to double column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDouble);
        auto hugeint2double_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2double_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2double_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast big int column vector to decimal column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDecimal);
        auto big2decimal_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(big2decimal_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast hugeint column vector to Varchar vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto hugeint2varchar_ptr = BindIntegerCast<HugeIntT>(*source_type, *target_type);
        EXPECT_NE(hugeint2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW_WITHOUT_STACKTRACE(hugeint2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kBigInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindIntegerCast<IntegerT>(source, target), RecoverableException);
    }
}

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
import integer_cast;
import bound_cast_func;
import internal_types;
import logical_type;
import data_type;

class SmallIntegerCastTest : public BaseTest {
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(SmallIntegerCastTest, small_integer_cast0) {
    using namespace infinity;

    // TinyInt to SmallInt, throw exception
    {
        SmallIntT source = 0;
        SmallIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }
    // SmallInt to TinyInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        TinyIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // SmallInt to Integer
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        IntegerT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // SmallInt to BigInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        BigIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // SmallInt to HugeInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);
    }
    // SmallInt to Float
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // SmallInt to Double
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // TODO:
    // Cast to decimal
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::lowest();
        DecimalT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // SmallInt to VarcharT
    {
        SmallIntT source;
        VarcharT target;
        String src_str, tgt_str;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3u);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());
    }
}

TEST_F(SmallIntegerCastTest, small_integer_cast1) {
    using namespace infinity;

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kSmallInt);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeSmallInt(static_cast<SmallIntT>(i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
    }

    // cast small int column vector to tiny int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTinyInt);
        auto small2tiny_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
            i32 check_value = 0;
            if (i >= std::numeric_limits<i8>::min() && i <= std::numeric_limits<i8>::max()) {
                check_value = static_cast<i32>(i);
            }
            EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(check_value));
        }
    }
    // cast small int column vector to integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kInteger);
        auto small2integer_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2integer_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2integer_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i16 check_value = static_cast<i16>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }
    // cast small int column vector to big int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kBigInt);
        auto small2bigint_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2bigint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i16 check_value = static_cast<i16>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // cast small int column vector to huge int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kHugeInt);
        auto small2hugeint_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2hugeint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2hugeint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i16>(i) < 0) * -1, static_cast<i16>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast small int column vector to float column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kFloat);
        auto small2float_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2float_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast small int column vector to double column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDouble);
        auto small2double_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2double_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2double_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast small int column vector to decimal column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDecimal);
        auto small2decimal_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2decimal_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(small2decimal_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }

    // cast small int column vector to Varchar vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto small2varchar_ptr = BindIntegerCast<SmallIntT>(*source_type, *target_type);
        EXPECT_NE(small2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = small2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            i16 check_value = static_cast<i16>(i);
            String check_str(std::to_string(check_value));
            Value vx = col_target->GetValue(i);
            const String &s2 = vx.GetVarchar();
            EXPECT_STREQ(s2.c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast small int to other types.
    {
        DataType source(LogicalType::kSmallInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<SmallIntT>(source, target), RecoverableException);
    }
}

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
import catalog;
import parser;
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
import float_cast;
import bound_cast_func;

class FloatCastTest : public BaseTest {};

TEST_F(FloatCastTest, float_cast0) {
    using namespace infinity;

    // FloatT to FloatT, throw exception
    {
        FloatT source = 0;
        FloatT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), FunctionException);
    }

    // FloatT to TinyInt
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        TinyIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::min();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 128;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));
    }

    // FloatT to SmallInt
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        SmallIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // FloatT to IntegerT
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        IntegerT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = static_cast<FloatT>(std::numeric_limits<IntegerT>::max());
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));
        //        EXPECT_FLOAT_EQ(source, target);

        source = static_cast<FloatT>(std::numeric_limits<IntegerT>::min());
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // FloatT to BigInt
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        BigIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = static_cast<FloatT>(std::numeric_limits<BigIntT>::max());
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        //        EXPECT_FLOAT_EQ(source, target);

        source = static_cast<FloatT>(std::numeric_limits<BigIntT>::min());
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO: need to implement FloatT to HugeInt
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        HugeIntT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // FloatT to Double
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        DoubleT target;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = static_cast<FloatT>(std::numeric_limits<IntegerT>::max());
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    // Cast to decimal16/32/64/128
    {
        FloatT source = std::numeric_limits<FloatT>::lowest();
        DecimalT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // FloatT to VarcharT
    {
        FloatT source;
        VarcharT target;
        String src_str, tgt_str;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));

//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 47);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 46);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 8);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 8);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 9);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 9);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 11);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 10);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());
    }
}

TEST_F(FloatCastTest, float_cast1) {
    using namespace infinity;

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kFloat);

    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i));
    }

    // cast float column vector to tiny int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTinyInt);
        auto float2tiny_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast float column vector to small integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kSmallInt);
        auto float2small_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2small_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
            i32 check_value = 0;
            if (i >= std::numeric_limits<i16>::min() && i <= std::numeric_limits<i16>::max()) {
                check_value = static_cast<i32>(i);
            }
            EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(check_value));
        }
    }

    // cast float column vector to integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kInteger);
        auto float2integer_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2integer_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2integer_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast float column vector to big int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kBigInt);
        auto float2bigint_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2bigint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // TODO: cast float column vector to huge int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kHugeInt);
        auto float2hugeint_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2hugeint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;

        //        bool result = float2hugeint_ptr.function(col_source, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_THROW(float2hugeint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast float column vector to double column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDouble);
        auto float2double_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2double_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2double_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast float column vector to Varchar vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto float2varchar_ptr = BindFloatCast<FloatT>(*source_type, *target_type);
        EXPECT_NE(float2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = float2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            f32 check_value = static_cast<f32>(i);
            String check_str(ToStr(check_value));
            Value vx = col_target->GetValue(i);
            const String &s2 = vx.GetVarchar();
            EXPECT_STREQ(s2.c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kFloat);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindFloatCast<FloatT>(source, target), TypeException);
    }
}

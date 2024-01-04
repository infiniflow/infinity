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
import integer_cast;
import bound_cast_func;

class BigIntCastTest : public BaseTest {};

TEST_F(BigIntCastTest, bigint_cast0) {
    using namespace infinity;

    // Integer to Integer, throw exception
    {
        BigIntT source = 0;
        BigIntT target;

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }

    // BigIntT to TinyInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        TinyIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
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

    // BigIntT to SmallInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        SmallIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // BigIntT to IntegerT
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        IntegerT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // BigIntT to HugeInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);
    }
    // BigIntT to Float
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // BigIntT to Double
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    // Cast to decimal
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        DecimalT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // BigIntT to VarcharT
    {
        BigIntT source;
        VarcharT target;
        String src_str, tgt_str;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));

//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 20);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 19);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 1);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 1);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 2);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 2);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 4);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 3);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());
    }
}

TEST_F(BigIntCastTest, bigint_cast1) {
    using namespace infinity;

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBigInt(static_cast<BigIntT>(i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
    }

    // cast bigint column vector to tiny int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTinyInt);
        auto bigint2tiny_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast bigint column vector to small integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kSmallInt);
        auto bigint2small_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2small_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast big int column vector to int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kInteger);
        auto bigint2int_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2int_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2int_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i64 check_value = static_cast<i64>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast bigint column vector to huge int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kHugeInt);
        auto bigint2hugeint_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2hugeint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2hugeint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i64>(i) < 0) * -1, static_cast<i64>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast bigint column vector to float column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kFloat);
        auto bigint2float_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2float_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast bigint column vector to double column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDouble);
        auto bigint2double_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2double_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2double_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast big int column vector to decimal column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDecimal);
        auto big2decimal_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast bigint column vector to Varchar vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto bigint2varchar_ptr = BindIntegerCast<BigIntT>(*source_type, *target_type);
        EXPECT_NE(bigint2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = bigint2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            i64 check_value = static_cast<i64>(i);
            String check_str(ToStr(check_value));
            Value vx = col_target->GetValue(i);
            const String &s2 = vx.GetVarchar();
            EXPECT_STREQ(s2.c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kBigInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<IntegerT>(source, target), TypeException);
    }
}

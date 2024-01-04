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

class DoubleCastTest : public BaseTest {};

TEST_F(DoubleCastTest, double_cast0) {
    using namespace infinity;

    // DoubleT to DoubleT, throw exception
    {
        DoubleT source = 0;
        DoubleT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), FunctionException);
    }

    // DoubleT to TinyInt
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        TinyIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<DoubleT>::max();
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

    // DoubleT to SmallInt
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        SmallIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<DoubleT>::max();
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

    // DoubleT to IntegerT
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        IntegerT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<DoubleT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::min();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // DoubleT to BigInt
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        BigIntT target;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<DoubleT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = static_cast<DoubleT>(std::numeric_limits<BigIntT>::max());
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = static_cast<DoubleT>(std::numeric_limits<BigIntT>::min());
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO: need to implement DoubleT to HugeInt
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        HugeIntT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // DoubleT to FloatT
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        FloatT target;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::min();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    {
        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        DecimalT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Double to VarcharT
    {
        DoubleT source;
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

        // col_varchar will release extra memory. Therefore,

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr));
//        src_str = ToStr(source);
//        EXPECT_EQ(src_str.size(), 46);
//        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        // Not release the heap memory in varchar, due to the heap will be release in col_varchar's destructor
        target.Reset(false);
    }
}

TEST_F(DoubleCastTest, double_cast1) {
    using namespace infinity;

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kDouble);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i));
    }

    // cast double column vector to tiny int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTinyInt);
        auto double2tiny_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast double column vector to small integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kSmallInt);
        auto double2small_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2small_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast double column vector to integer column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kInteger);
        auto double2integer_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2integer_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2integer_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast double column vector to big int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kBigInt);
        auto double2bigint_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2bigint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // TODO: cast double column vector to huge int column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kHugeInt);
        auto double2hugeint_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2hugeint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;

        //        bool result = double2hugeint_ptr.function(col_float, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_THROW(double2hugeint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast double column vector to float column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kFloat);
        auto double2float_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2float_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast double column vector to Varchar vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto double2varchar_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            f64 check_value = static_cast<f64>(i);
            String check_str(ToStr(check_value));
            Value vx = col_target->GetValue(i);
            const String &s2 = vx.GetVarchar();
            EXPECT_STREQ(s2.c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kDouble);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindFloatCast<DoubleT>(source, target), TypeException);
    }
}

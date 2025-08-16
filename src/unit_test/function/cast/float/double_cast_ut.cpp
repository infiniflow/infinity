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
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.double_cast;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
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
import :float_cast;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;

class DoubleCastTest : public BaseTest {};

TEST_F(DoubleCastTest, double_cast0) {
    using namespace infinity;

    // DoubleT to DoubleT, throw exception
    {
        DoubleT source = 0;
        DoubleT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(FloatTryCastToFixlen::Run(source, target), UnrecoverableException);
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
        EXPECT_THROW_WITHOUT_STACKTRACE(FloatTryCastToFixlen::Run(source, target), UnrecoverableException);
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
        EXPECT_THROW_WITHOUT_STACKTRACE(FloatTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // Double to VarcharT
    {
        DoubleT source;
        VarcharT target;
        std::string src_str, tgt_str;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kVarchar);

        std::shared_ptr<ColumnVector> col_varchar_ptr = std::make_shared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));

        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 47);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 46);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 8);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 8);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 9);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 9);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 11);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 10);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        // col_varchar will release extra memory. Therefore,

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        //        src_str = std::to_string(source);
        //        EXPECT_EQ(src_str.size(), 46);
        //        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        // Not release the heap memory in varchar, due to the heap will be release in col_varchar's destructor
    }
}

TEST_F(DoubleCastTest, double_cast1) {
    using namespace infinity;

    std::shared_ptr<DataType> source_type = std::make_shared<DataType>(LogicalType::kDouble);
    std::shared_ptr<ColumnVector> col_source = std::make_shared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValueByIndex(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i));
    }

    // cast double column vector to tiny int column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kTinyInt);
        auto double2tiny_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2tiny_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValueByIndex(i);
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
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kSmallInt);
        auto double2small_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2small_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValueByIndex(i);
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
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kInteger);
        auto double2integer_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2integer_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2integer_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValueByIndex(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast double column vector to big int column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kBigInt);
        auto double2bigint_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2bigint_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValueByIndex(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // TODO: cast double column vector to huge int column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kHugeInt);
        auto double2hugeint_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2hugeint_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;

        //        bool result = double2hugeint_ptr.function(col_float, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_THROW_WITHOUT_STACKTRACE(double2hugeint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters),
                                        UnrecoverableException);
    }

    // cast double column vector to float column vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kFloat);
        auto double2float_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2float_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            Value vx = col_target->GetValueByIndex(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast double column vector to Varchar vector
    {
        std::shared_ptr<DataType> target_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto double2varchar_ptr = BindFloatCast<DoubleT>(*source_type, *target_type);
        EXPECT_NE(double2varchar_ptr.function, nullptr);

        std::shared_ptr<ColumnVector> col_target = std::make_shared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        bool result = double2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
            f64 check_value = static_cast<f64>(i);
            std::string check_str(std::to_string(check_value));
            Value vx = col_target->GetValueByIndex(i);
            const std::string &s2 = vx.GetVarchar();
            EXPECT_STREQ(s2.c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kDouble);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindFloatCast<DoubleT>(source, target), UnrecoverableException);
    }
}

//
// Created by JinHai on 2022/12/17.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/integer_cast.h"

class IntegerCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(IntegerCastTest, tiny_integer_cast1) {
    using namespace infinity;

    DataType tinyint_data_type(LogicalType::kTinyInt);
    DataType tinyint_type(LogicalType::kTinyInt);
    ColumnVector col_tinyint(tinyint_type, ColumnVectorType::kFlat);
    col_tinyint.Initialize();
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(i));
        col_tinyint.AppendValue(v);
        Value vx = col_tinyint.GetValue(i);
    }
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
    }

    // cast tiny column vector to small column vector
    {
        DataType smallint_data_type(LogicalType::kSmallInt);
        auto tiny2small_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, smallint_data_type);
        EXPECT_NE(tiny2small_ptr.function, nullptr);

        ColumnVector col_smallint(smallint_data_type, ColumnVectorType::kFlat);
        col_smallint.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2small_ptr.function(col_tinyint, col_smallint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_smallint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
            i8 check_value = static_cast<i8>(i);
            EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(check_value));
        }
    }

    // cast tiny column vector to integer column vector
    {
        DataType integer_data_type(LogicalType::kInteger);
        auto tiny2integer_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, integer_data_type);
        EXPECT_NE(tiny2integer_ptr.function, nullptr);

        ColumnVector col_int(integer_data_type, ColumnVectorType::kFlat);
        col_int.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2integer_ptr.function(col_tinyint, col_int, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_int.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i8 check_value = static_cast<i8>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast tiny column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto tiny2bigint_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, bigint_data_type);
        EXPECT_NE(tiny2bigint_ptr.function, nullptr);

        ColumnVector col_bigint(bigint_data_type, ColumnVectorType::kFlat);
        col_bigint.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2bigint_ptr.function(col_tinyint, col_bigint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_bigint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i8 check_value = static_cast<i8>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // cast tiny column vector to huge int column vector
    {
        DataType hugeint_data_type(LogicalType::kHugeInt);
        auto tiny2hugeint_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, hugeint_data_type);
        EXPECT_NE(tiny2hugeint_ptr.function, nullptr);

        ColumnVector col_hugeint(hugeint_data_type, ColumnVectorType::kFlat);
        col_hugeint.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2hugeint_ptr.function(col_tinyint, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_hugeint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i8>(i) < 0) * -1, static_cast<i8>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast tiny column vector to big int column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto tiny2float_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, float_data_type);
        EXPECT_NE(tiny2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2float_ptr.function(col_tinyint, col_float, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_float.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i8 check_value = static_cast<i8>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast tiny column vector to big int column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto tiny2double_ptr = BindIntegerCast<TinyIntT>(tinyint_data_type, double_data_type);
        EXPECT_NE(tiny2double_ptr.function, nullptr);

        ColumnVector col_double(double_data_type, ColumnVectorType::kFlat);
        col_double.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2double_ptr.function(col_tinyint, col_double, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_double.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i8 check_value = static_cast<i8>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }
//    {
//        DataType source(LogicalType::kSmallInt);
//        DataType target(LogicalType::kBigInt);
//        auto func_ptr = BindIntegerCast<SmallIntT>(source, target);
//        EXPECT_NE(func_ptr.function, nullptr);
//    }
//
//    {
//        DataType source(LogicalType::kSmallInt);
//        DataType target(LogicalType::kArray);
//        EXPECT_THROW(BindIntegerCast<SmallIntT>(source, target), TypeException);
//    }

}
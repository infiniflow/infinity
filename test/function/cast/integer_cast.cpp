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

        DataType smallint_type(LogicalType::kSmallInt);
        ColumnVector col_smallint(smallint_type, ColumnVectorType::kFlat);
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
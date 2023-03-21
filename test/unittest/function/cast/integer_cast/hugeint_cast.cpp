//
// Created by jinhai on 22-12-21.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "function/cast/integer_cast.h"

class HugeIntCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(HugeIntCastTest, hugeint_cast0) {
    using namespace infinity;

    // HugeInt to HugeInt, throw exception
    {
        HugeIntT source{0, 0};
        HugeIntT target{0, 0};

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }

    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        TinyIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        SmallIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        IntegerT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        BigIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        FloatT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        DoubleT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        DecimalT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        VarcharT target;

        DataType data_type(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(IntegerTryCastToVarlen::Run(source, target, col_varchar_ptr), NotImplementException);
    }
    // HugeInt to Date, throw exception
    {
        HugeIntT source{0, 0};
        DateT target{0};

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }
}

TEST_F(HugeIntCastTest, hugeint_cast1) {
    using namespace infinity;

    DataType source_type(LogicalType::kHugeInt);

    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeHugeInt(HugeIntT(i, i));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, HugeIntT(i, i));
    }

    // cast hugeint column vector to tiny int column vector
    {
        DataType target_type(LogicalType::kTinyInt);
        auto hugeint2tiny_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2tiny_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2tiny_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to small integer column vector
    {
        DataType target_type(LogicalType::kSmallInt);
        auto hugeint2small_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2small_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2small_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to int column vector
    {
        DataType target_type(LogicalType::kInteger);
        auto hugeint2int_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2int_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2int_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to big int column vector
    {
        DataType target_type(LogicalType::kBigInt);
        auto hugeint2bigint_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2bigint_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2bigint_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to float column vector
    {
        DataType target_type(LogicalType::kFloat);
        auto hugeint2float_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2float_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2float_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to double column vector
    {
        DataType target_type(LogicalType::kDouble);
        auto hugeint2double_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2double_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2double_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to decimal column vector
    {
        DataType target_type(LogicalType::kDecimal);
        auto big2decimal_ptr = BindIntegerCast<BigIntT>(source_type, target_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to Varchar vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto hugeint2varchar_ptr = BindIntegerCast<HugeIntT>(source_type, target_type);
        EXPECT_NE(hugeint2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2varchar_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kBigInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<IntegerT>(source, target), TypeException);
    }
}
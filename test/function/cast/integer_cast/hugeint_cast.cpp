//
// Created by jinhai on 22-12-21.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/integer_cast.h"
#include "common/types/info/varchar_info.h"

class HugeIntCastTest : public BaseTest {
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
        Decimal16T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal32T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal64T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal128T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        EXPECT_THROW(IntegerTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
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

    DataType hugeint_type(LogicalType::kHugeInt);
    ColumnVector col_hugeint(hugeint_type, ColumnVectorType::kFlat);
    col_hugeint.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeHugeInt(HugeIntT(i, i));
        col_hugeint.AppendValue(v);
        Value vx = col_hugeint.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_hugeint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, HugeIntT(i, i));
    }

    // cast hugeint column vector to tiny int column vector
    {
        DataType tinyint_data_type(LogicalType::kTinyInt);
        auto hugeint2tiny_ptr = BindIntegerCast<HugeIntT>(hugeint_type, tinyint_data_type);
        EXPECT_NE(hugeint2tiny_ptr.function, nullptr);

        ColumnVector col_tinyint(tinyint_data_type, ColumnVectorType::kFlat);
        col_tinyint.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2tiny_ptr.function(col_hugeint, col_tinyint, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to small integer column vector
    {
        DataType small_data_type(LogicalType::kSmallInt);
        auto hugeint2small_ptr = BindIntegerCast<HugeIntT>(hugeint_type, small_data_type);
        EXPECT_NE(hugeint2small_ptr.function, nullptr);

        ColumnVector col_smallint(small_data_type, ColumnVectorType::kFlat);
        col_smallint.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2small_ptr.function(col_hugeint, col_smallint, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to int column vector
    {
        DataType int_data_type(LogicalType::kInteger);
        auto hugeint2int_ptr = BindIntegerCast<HugeIntT>(hugeint_type, int_data_type);
        EXPECT_NE(hugeint2int_ptr.function, nullptr);

        ColumnVector col_int(int_data_type, ColumnVectorType::kFlat);
        col_int.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2int_ptr.function(col_hugeint, col_int, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto hugeint2bigint_ptr = BindIntegerCast<HugeIntT>(hugeint_type, bigint_data_type);
        EXPECT_NE(hugeint2bigint_ptr.function, nullptr);

        ColumnVector col_bigint(bigint_data_type, ColumnVectorType::kFlat);
        col_bigint.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2bigint_ptr.function(col_hugeint, col_bigint, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto hugeint2float_ptr = BindIntegerCast<HugeIntT>(hugeint_type, float_data_type);
        EXPECT_NE(hugeint2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2float_ptr.function(col_hugeint, col_float, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to double column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto hugeint2double_ptr = BindIntegerCast<HugeIntT>(hugeint_type, double_data_type);
        EXPECT_NE(hugeint2double_ptr.function, nullptr);

        ColumnVector col_double(double_data_type, ColumnVectorType::kFlat);
        col_double.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2double_ptr.function(col_hugeint, col_double, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to decimal16 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal16);
        auto big2decimal_ptr = BindIntegerCast<TinyIntT>(hugeint_type, decimal_data_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_hugeint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to decimal32 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal32);
        auto big2decimal_ptr = BindIntegerCast<TinyIntT>(hugeint_type, decimal_data_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_hugeint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to decimal64 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal64);
        auto big2decimal_ptr = BindIntegerCast<TinyIntT>(hugeint_type, decimal_data_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_hugeint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast big int column vector to decimal128 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal128);
        auto big2decimal_ptr = BindIntegerCast<TinyIntT>(hugeint_type, decimal_data_type);
        EXPECT_NE(big2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(big2decimal_ptr.function(col_hugeint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast hugeint column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto hugeint2varchar_ptr = BindIntegerCast<HugeIntT>(hugeint_type, varchar_data_type);
        EXPECT_NE(hugeint2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(hugeint2varchar_ptr.function(col_hugeint, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kBigInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<IntegerT>(source, target), TypeException);
    }
}
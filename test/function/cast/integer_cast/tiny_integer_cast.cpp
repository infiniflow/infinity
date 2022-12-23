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
#include "common/types/info/varchar_info.h"

class TinyIntegerCastTest : public BaseTest {
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

TEST_F(TinyIntegerCastTest, tiny_integer_cast0) {
    using namespace infinity;

    // TinyInt to SmallInt, throw exception
    {
        TinyIntT source = 0;
        TinyIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }
    // TinyInt to SmallInt
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        SmallIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // TinyInt to Integer
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        IntegerT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // TinyInt to BigInt
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        BigIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // TinyInt to HugeInt
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target.upper);
    }
    // TinyInt to Float
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // TinyInt to Double
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    // Cast to decimal16/32/64/128
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::lowest();
        Decimal16T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::lowest();
        Decimal32T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::lowest();
        Decimal64T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TinyIntT source = std::numeric_limits<TinyIntT>::lowest();
        Decimal128T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // TinyInt to VarcharT
    {
        TinyIntT source;
        VarcharT target;
        String src_str, tgt_str;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        source = std::numeric_limits<TinyIntT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<TinyIntT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());
    }

}

TEST_F(TinyIntegerCastTest, tiny_integer_cast1) {
    using namespace infinity;

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

    // cast tiny int column vector to small column vector
    {
        DataType smallint_data_type(LogicalType::kSmallInt);
        auto tiny2small_ptr = BindIntegerCast<TinyIntT>(tinyint_type, smallint_data_type);
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

    // cast tiny int column vector to integer column vector
    {
        DataType integer_data_type(LogicalType::kInteger);
        auto tiny2integer_ptr = BindIntegerCast<TinyIntT>(tinyint_type, integer_data_type);
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

    // cast tiny int column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto tiny2bigint_ptr = BindIntegerCast<TinyIntT>(tinyint_type, bigint_data_type);
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

    // cast tiny int column vector to huge int column vector
    {
        DataType hugeint_data_type(LogicalType::kHugeInt);
        auto tiny2hugeint_ptr = BindIntegerCast<TinyIntT>(tinyint_type, hugeint_data_type);
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

    // cast tiny int column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto tiny2float_ptr = BindIntegerCast<TinyIntT>(tinyint_type, float_data_type);
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

    // cast tiny int column vector to double column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto tiny2double_ptr = BindIntegerCast<TinyIntT>(tinyint_type, double_data_type);
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

    // cast tiny int column vector to decimal16 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal16);
        auto tiny2decimal_ptr = BindIntegerCast<TinyIntT>(tinyint_type, decimal_data_type);
        EXPECT_NE(tiny2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(tiny2decimal_ptr.function(col_tinyint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast tiny int column vector to decimal32 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal32);
        auto tiny2decimal_ptr = BindIntegerCast<TinyIntT>(tinyint_type, decimal_data_type);
        EXPECT_NE(tiny2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(tiny2decimal_ptr.function(col_tinyint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast tiny int column vector to decimal64 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal64);
        auto tiny2decimal_ptr = BindIntegerCast<TinyIntT>(tinyint_type, decimal_data_type);
        EXPECT_NE(tiny2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(tiny2decimal_ptr.function(col_tinyint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast tiny int column vector to decimal128 column vector
    {
        DataType decimal_data_type(LogicalType::kDecimal128);
        auto tiny2decimal_ptr = BindIntegerCast<TinyIntT>(tinyint_type, decimal_data_type);
        EXPECT_NE(tiny2decimal_ptr.function, nullptr);

        ColumnVector col_decimal(decimal_data_type, ColumnVectorType::kFlat);
        col_decimal.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(tiny2decimal_ptr.function(col_tinyint, col_decimal, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast tiny int column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto tiny2varchar_ptr = BindIntegerCast<TinyIntT>(tinyint_type, varchar_data_type);
        EXPECT_NE(tiny2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        bool result = tiny2varchar_ptr.function(col_tinyint, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            i8 check_value = static_cast<i8>(i);
            EXPECT_FALSE(vx.is_null());
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast tiny int to other types.
    {
        DataType source(LogicalType::kTinyInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<SmallIntT>(source, target), TypeException);
    }
}
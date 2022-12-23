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

TEST_F(IntegerCastTest, integer_cast0) {
    using namespace infinity;

    // Integer to Integer, throw exception
    {
        IntegerT source = 0;
        IntegerT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), TypeException);
    }

    // IntegerT to TinyInt
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        TinyIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<IntegerT>::max();
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

    // Integer to SmallInt
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        SmallIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<IntegerT>::max();
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
    // Integer to BigInt
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        BigIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // IntegerT to HugeInt
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i32>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i32>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i32>(source) < 0) * -1, target.upper);
    }
    // IntegerT to Float
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // IntegerT to Double
    {
        IntegerT source = std::numeric_limits<IntegerT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    // Cast to decimal16/32/64/128
    {
        IntegerT source = std::numeric_limits<IntegerT>::lowest();
        Decimal16T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        IntegerT source = std::numeric_limits<IntegerT>::lowest();
        Decimal32T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        IntegerT source = std::numeric_limits<IntegerT>::lowest();
        Decimal64T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        IntegerT source = std::numeric_limits<IntegerT>::lowest();
        Decimal128T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // IntegerT to VarcharT
    {
        IntegerT source;
        VarcharT target;
        String src_str, tgt_str;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        source = std::numeric_limits<IntegerT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));

        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 11);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 10);
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

TEST_F(IntegerCastTest, integer_cast1) {
    using namespace infinity;

    DataType int_type(LogicalType::kInteger);
    ColumnVector col_int(int_type, ColumnVectorType::kFlat);
    col_int.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeInt(static_cast<IntegerT>(i));
        col_int.AppendValue(v);
        Value vx = col_int.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_int.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
        EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(i));
    }

    // cast integer column vector to tiny int column vector
    {
        DataType tinyint_data_type(LogicalType::kTinyInt);
        auto int2tiny_ptr = BindIntegerCast<IntegerT>(int_type, tinyint_data_type);
        EXPECT_NE(int2tiny_ptr.function, nullptr);

        ColumnVector col_tinyint(tinyint_data_type, ColumnVectorType::kFlat);
        col_tinyint.Initialize();

        CastParameters cast_parameters;
        bool result = int2tiny_ptr.function(col_int, col_tinyint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_tinyint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
            i32 check_value = static_cast<i32>(i);
            if (check_value >= std::numeric_limits<i8>::min() && check_value <= std::numeric_limits<i8>::max()) {
                EXPECT_FALSE(vx.is_null());
                EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(check_value));
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }

    // cast int column vector to small integer column vector
    {
        DataType small_data_type(LogicalType::kSmallInt);
        auto int2small_ptr = BindIntegerCast<IntegerT>(int_type, small_data_type);
        EXPECT_NE(int2small_ptr.function, nullptr);

        ColumnVector col_smallint(small_data_type, ColumnVectorType::kFlat);
        col_smallint.Initialize();

        CastParameters cast_parameters;
        bool result = int2small_ptr.function(col_int, col_smallint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_smallint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(check_value));
        }
    }

    // cast int column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto int2bigint_ptr = BindIntegerCast<IntegerT>(int_type, bigint_data_type);
        EXPECT_NE(int2bigint_ptr.function, nullptr);

        ColumnVector col_bigint(bigint_data_type, ColumnVectorType::kFlat);
        col_bigint.Initialize();

        CastParameters cast_parameters;
        bool result = int2bigint_ptr.function(col_int, col_bigint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_bigint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // cast int column vector to huge int column vector
    {
        DataType hugeint_data_type(LogicalType::kHugeInt);
        auto int2hugeint_ptr = BindIntegerCast<IntegerT>(int_type, hugeint_data_type);
        EXPECT_NE(int2hugeint_ptr.function, nullptr);

        ColumnVector col_hugeint(hugeint_data_type, ColumnVectorType::kFlat);
        col_hugeint.Initialize();

        CastParameters cast_parameters;
        bool result = int2hugeint_ptr.function(col_int, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_hugeint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i32>(i) < 0) * -1, static_cast<i32>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast int column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto int2float_ptr = BindIntegerCast<IntegerT>(int_type, float_data_type);
        EXPECT_NE(int2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        bool result = int2float_ptr.function(col_int, col_float, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_float.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast int column vector to double column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto int2double_ptr = BindIntegerCast<IntegerT>(int_type, double_data_type);
        EXPECT_NE(int2double_ptr.function, nullptr);

        ColumnVector col_double(double_data_type, ColumnVectorType::kFlat);
        col_double.Initialize();

        CastParameters cast_parameters;
        bool result = int2double_ptr.function(col_int, col_double, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_double.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast int column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto int2varchar_ptr = BindIntegerCast<IntegerT>(int_type, varchar_data_type);
        EXPECT_NE(int2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        bool result = int2varchar_ptr.function(col_int, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FALSE(vx.is_null());
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kInteger);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<IntegerT>(source, target), TypeException);
    }
}

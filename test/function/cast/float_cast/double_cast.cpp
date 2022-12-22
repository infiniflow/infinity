//
// Created by JinHai on 2022/12/17.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/float_cast.h"
#include "common/types/info/varchar_info.h"

class DoubleCastTest : public BaseTest {
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

TEST_F(DoubleCastTest, double_cast0) {
    using namespace infinity;

    // DoubleT to DoubleT, throw exception
    {
        DoubleT source = 0;
        DoubleT target;
        EXPECT_THROW(FloatTryCastToFixlen::Run(source, target), TypeException);
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

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO: need to implement FloatT to HugeInt

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
    // Cast to decimal16/32/64/128


    // FloatT can't be cast to Char1T
    // FloatT can't be cast to Char2T
    // FloatT can't be cast to Char4T


    // DoubleT to Char8T [0 - 10)
    {
        DoubleT source;
        Char8T target;
        String src_str, tgt_str;

        source = std::numeric_limits<DoubleT>::lowest();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<DoubleT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 10;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 99;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = -100;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = -1;
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));
    }

    // DoubleT to Char16T
    {
        FloatT source;
        Char16T target;
        String src_str, tgt_str;

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(tgt_str.c_str(), src_str.c_str());

        source = 10;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        tgt_str = String(target.value, 9);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        tgt_str = String(target.value, 9);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 11);
        tgt_str = String(target.value, 11);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 10);
        tgt_str = String(target.value, 10);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100000000;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 16);
        tgt_str = String(target.value, 16);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -99999900;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 16);
        tgt_str = String(target.value, 16);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // IntegerT to Char32T
    {
        FloatT source;
        Char32T target;
        String src_str, tgt_str;

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<FloatT>::max();
        EXPECT_FALSE(FloatTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(tgt_str.c_str(), src_str.c_str());

        source = 10;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        tgt_str = String(target.value, 9);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        tgt_str = String(target.value, 9);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 11);
        tgt_str = String(target.value, 11);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 10);
        tgt_str = String(target.value, 10);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // IntegerT to Char64T
    {
        FloatT source;
        Char64T target;
        String src_str, tgt_str;

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 47);
        tgt_str = String(target.value, 47);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 46);
        tgt_str = String(target.value, 46);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 0;
        EXPECT_TRUE(FloatTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

    }

    // IntegerT to VarcharT
    {
        FloatT source;
        VarcharT target;
        String src_str, tgt_str;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        source = std::numeric_limits<FloatT>::lowest();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));

        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 47);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<FloatT>::max();
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 46);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 0;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 9;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 10;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 99;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 9);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = -100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 11);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = 100;
        EXPECT_TRUE(FloatTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 10);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());
    }
}

TEST_F(DoubleCastTest, double_cast1) {
    using namespace infinity;

    DataType double_type(LogicalType::kDouble);
    ColumnVector col_double(double_type, ColumnVectorType::kFlat);
    col_double.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i));
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i));
    }

    // cast double column vector to tiny int column vector
    {
        DataType tinyint_data_type(LogicalType::kTinyInt);
        auto double2tiny_ptr = BindFloatCast<DoubleT>(double_type, tinyint_data_type);
        EXPECT_NE(double2tiny_ptr.function, nullptr);

        ColumnVector col_tinyint(tinyint_data_type, ColumnVectorType::kFlat);
        col_tinyint.Initialize();

        CastParameters cast_parameters;
        bool result = double2tiny_ptr.function(col_double, col_tinyint, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast double column vector to small integer column vector
    {
        DataType small_data_type(LogicalType::kSmallInt);
        auto double2small_ptr = BindFloatCast<DoubleT>(double_type, small_data_type);
        EXPECT_NE(double2small_ptr.function, nullptr);

        ColumnVector col_smallint(small_data_type, ColumnVectorType::kFlat);
        col_smallint.Initialize();

        CastParameters cast_parameters;
        bool result = double2small_ptr.function(col_double, col_smallint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_smallint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(check_value));
        }
    }

    // cast double column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto double2bigint_ptr = BindFloatCast<DoubleT>(double_type, bigint_data_type);
        EXPECT_NE(double2bigint_ptr.function, nullptr);

        ColumnVector col_bigint(bigint_data_type, ColumnVectorType::kFlat);
        col_bigint.Initialize();

        CastParameters cast_parameters;
        bool result = double2bigint_ptr.function(col_double, col_bigint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_bigint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i32 check_value = static_cast<i32>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // TODO: cast double column vector to huge int column vector

    // cast double column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto double2float_ptr = BindFloatCast<DoubleT>(double_type, float_data_type);
        EXPECT_NE(double2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        bool result = double2float_ptr.function(col_double, col_float, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_float.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i32 check_value = static_cast<i32>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // can't cast double column vector to Char1 vector
    // can't cast double column vector to Char2 vector
    // can't cast int column vector to Char4 vector

    // cast int column vector to Char8 vector
    {
        DataType char8_data_type(LogicalType::kChar8);
        auto double2char8_ptr = BindFloatCast<DoubleT>(double_type, char8_data_type);
        EXPECT_NE(double2char8_ptr.function, nullptr);

        ColumnVector col_char8(char8_data_type, ColumnVectorType::kFlat);
        col_char8.Initialize();

        CastParameters cast_parameters;
        bool result = double2char8_ptr.function(col_double, col_char8, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char8.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar8);
            f64 check_value = static_cast<f64>(i);
            if (check_value >= 0 && check_value <= 9) {
                EXPECT_FALSE(vx.is_null());
                String output_str(vx.value_.char8.value, 8);
                String check_str(std::to_string(check_value));
                EXPECT_STREQ(output_str.c_str(), check_str.c_str());
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }

    // cast double column vector to Char16 vector
    {
        DataType char16_data_type(LogicalType::kChar16);
        auto double2char16_ptr = BindFloatCast<DoubleT>(double_type, char16_data_type);
        EXPECT_NE(double2char16_ptr.function, nullptr);

        ColumnVector col_char16(char16_data_type, ColumnVectorType::kFlat);
        col_char16.Initialize();

        CastParameters cast_parameters;
        bool result = double2char16_ptr.function(col_double, col_char16, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char16.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar16);
            f64 check_value = static_cast<f64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char16.value, 16);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast double column vector to Char32 vector
    {
        DataType char32_data_type(LogicalType::kChar32);
        auto double2char32_ptr = BindFloatCast<DoubleT>(double_type, char32_data_type);
        EXPECT_NE(double2char32_ptr.function, nullptr);

        ColumnVector col_char32(char32_data_type, ColumnVectorType::kFlat);
        col_char32.Initialize();

        CastParameters cast_parameters;
        bool result = double2char32_ptr.function(col_double, col_char32, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char32.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar32);
            f64 check_value = static_cast<f64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char32.value, 32);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast double column vector to Char64 vector
    {
        DataType char64_data_type(LogicalType::kChar64);
        auto double2char64_ptr = BindFloatCast<DoubleT>(double_type, char64_data_type);
        EXPECT_NE(double2char64_ptr.function, nullptr);

        ColumnVector col_char64(char64_data_type, ColumnVectorType::kFlat);
        col_char64.Initialize();

        CastParameters cast_parameters;
        bool result = double2char64_ptr.function(col_double, col_char64, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char64.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar64);
            f64 check_value = static_cast<f64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char64.value, 64);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast double column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto double2varchar_ptr = BindFloatCast<DoubleT>(double_type, varchar_data_type);
        EXPECT_NE(double2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        bool result = double2varchar_ptr.function(col_double, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            f64 check_value = static_cast<f64>(i);
            EXPECT_FALSE(vx.is_null());
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kDouble);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindFloatCast<DoubleT>(source, target), TypeException);
    }
}

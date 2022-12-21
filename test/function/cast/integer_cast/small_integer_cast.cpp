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

class SmallIntegerCastTest : public BaseTest {
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

TEST_F(SmallIntegerCastTest, small_integer_cast0) {
    using namespace infinity;

    // TinyInt to SmallInt, throw exception
    {
        SmallIntT source = 0;
        SmallIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), TypeException);
    }
    // SmallInt to TinyInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        TinyIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<SmallIntT>::max();
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
    // SmallInt to Integer
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        IntegerT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // SmallInt to BigInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        BigIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // SmallInt to HugeInt
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i16>(source) < 0) * -1, target.upper);
    }
    // SmallInt to Float
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // SmallInt to Double
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // TODO:
    // Cast to decimal16/32/64/128

    // SmallInt to Char1T
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Char1T target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        String src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_EQ(src_str[0], target.value);

        source = 1;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_EQ(src_str[0], target.value);
    }

    // SmallInt to Char2T
    {
        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Char2T target;
        String src_str, tgt_str;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to Char4T
    {
        SmallIntT source;
        Char4T target;
        String src_str, tgt_str;

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        tgt_str = String(target.value, 3);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to Char8T
    {
        SmallIntT source;
        Char8T target;
        String src_str, tgt_str;

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6);
        tgt_str = String(target.value, 6);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5);
        tgt_str = String(target.value, 5);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        tgt_str = String(target.value, 3);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to Char16T
    {
        SmallIntT source;
        Char16T target;
        String src_str, tgt_str;

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6);
        tgt_str = String(target.value, 6);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5);
        tgt_str = String(target.value, 5);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        tgt_str = String(target.value, 3);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to Char32T
    {
        SmallIntT source;
        Char32T target;
        String src_str, tgt_str;

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6);
        tgt_str = String(target.value, 6);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5);
        tgt_str = String(target.value, 5);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        tgt_str = String(target.value, 3);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to Char64T
    {
        SmallIntT source;
        Char64T target;
        String src_str, tgt_str;

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6);
        tgt_str = String(target.value, 6);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5);
        tgt_str = String(target.value, 5);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        tgt_str = String(target.value, 1);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 1);
        EXPECT_STREQ(target.value, src_str.c_str());

        source = 10;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 2);
        tgt_str = String(target.value, 2);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = -100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 4);
        tgt_str = String(target.value, 4);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 100;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 3);
        tgt_str = String(target.value, 3);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // SmallInt to VarcharT
    {
        SmallIntT source;
        VarcharT target;
        String src_str, tgt_str;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        source = std::numeric_limits<SmallIntT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 6);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<SmallIntT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 5);
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

TEST_F(SmallIntegerCastTest, small_integer_cast1) {
    using namespace infinity;

    DataType smallint_type(LogicalType::kSmallInt);
    ColumnVector col_smallint(smallint_type, ColumnVectorType::kFlat);
    col_smallint.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeSmallInt(static_cast<SmallIntT>(i));
        col_smallint.AppendValue(v);
        Value vx = col_smallint.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_smallint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
    }

    // cast small int column vector to tiny int column vector
    {
        DataType tinyint_data_type(LogicalType::kTinyInt);
        auto small2tiny_ptr = BindIntegerCast<SmallIntT>(smallint_type, tinyint_data_type);
        EXPECT_NE(small2tiny_ptr.function, nullptr);

        ColumnVector col_tinyint(tinyint_data_type, ColumnVectorType::kFlat);
        col_tinyint.Initialize();

        CastParameters cast_parameters;
        bool result = small2tiny_ptr.function(col_smallint, col_tinyint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_tinyint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
            i16 check_value = static_cast<i16>(i);
            if (check_value >= std::numeric_limits<i8>::min() && check_value <= std::numeric_limits<i8>::max()) {
                EXPECT_FALSE(vx.is_null());
                EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(check_value));
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }
    // cast small int column vector to integer column vector
    {
        DataType integer_data_type(LogicalType::kInteger);
        auto small2integer_ptr = BindIntegerCast<SmallIntT>(smallint_type, integer_data_type);
        EXPECT_NE(small2integer_ptr.function, nullptr);

        ColumnVector col_int(integer_data_type, ColumnVectorType::kFlat);
        col_int.Initialize();

        CastParameters cast_parameters;
        bool result = small2integer_ptr.function(col_smallint, col_int, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_int.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i16 check_value = static_cast<i16>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }
    // cast small int column vector to big int column vector
    {
        DataType bigint_data_type(LogicalType::kBigInt);
        auto small2bigint_ptr = BindIntegerCast<SmallIntT>(smallint_type, bigint_data_type);
        EXPECT_NE(small2bigint_ptr.function, nullptr);

        ColumnVector col_bigint(bigint_data_type, ColumnVectorType::kFlat);
        col_bigint.Initialize();

        CastParameters cast_parameters;
        bool result = small2bigint_ptr.function(col_smallint, col_bigint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_bigint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
            i16 check_value = static_cast<i16>(i);
            EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(check_value));
        }
    }

    // cast small int column vector to huge int column vector
    {
        DataType hugeint_data_type(LogicalType::kHugeInt);
        auto small2hugeint_ptr = BindIntegerCast<SmallIntT>(smallint_type, hugeint_data_type);
        EXPECT_NE(small2hugeint_ptr.function, nullptr);

        ColumnVector col_hugeint(hugeint_data_type, ColumnVectorType::kFlat);
        col_hugeint.Initialize();

        CastParameters cast_parameters;
        bool result = small2hugeint_ptr.function(col_smallint, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_hugeint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i16>(i) < 0) * -1, static_cast<i16>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast small int column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto small2float_ptr = BindIntegerCast<SmallIntT>(smallint_type, float_data_type);
        EXPECT_NE(small2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        bool result = small2float_ptr.function(col_smallint, col_float, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_float.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast small int column vector to double column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto small2double_ptr = BindIntegerCast<SmallIntT>(smallint_type, double_data_type);
        EXPECT_NE(small2double_ptr.function, nullptr);

        ColumnVector col_double(double_data_type, ColumnVectorType::kFlat);
        col_double.Initialize();

        CastParameters cast_parameters;
        bool result = small2double_ptr.function(col_smallint, col_double, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_double.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast small int column vector to Char1 vector
    {
        DataType char1_data_type(LogicalType::kChar1);
        auto small2char1_ptr = BindIntegerCast<SmallIntT>(smallint_type, char1_data_type);
        EXPECT_NE(small2char1_ptr.function, nullptr);

        ColumnVector col_char1(char1_data_type, ColumnVectorType::kFlat);
        col_char1.Initialize();

        CastParameters cast_parameters;
        bool result = small2char1_ptr.function(col_smallint, col_char1, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char1.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
            i16 check_value = static_cast<i16>(i);
            if (check_value >= 0 && check_value <= 9) {
                EXPECT_FALSE(vx.is_null());
                EXPECT_EQ(vx.value_.char1.value, char(check_value % 10 + '0'));
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }
    // cast small int column vector to Char2 vector
    {
        DataType char2_data_type(LogicalType::kChar2);
        auto small2char2_ptr = BindIntegerCast<SmallIntT>(smallint_type, char2_data_type);
        EXPECT_NE(small2char2_ptr.function, nullptr);

        ColumnVector col_char2(char2_data_type, ColumnVectorType::kFlat);
        col_char2.Initialize();

        CastParameters cast_parameters;
        bool result = small2char2_ptr.function(col_smallint, col_char2, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char2.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar2);
            i16 check_value = static_cast<i16>(i);
            if (check_value >= -9 && check_value <= 99) {
                EXPECT_FALSE(vx.is_null());
                String output_str(vx.value_.char2.value, 2);
                String check_str(std::to_string(check_value));
                EXPECT_STREQ(output_str.c_str(), check_str.c_str());
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }
    // cast tiny int column vector to Char4 vector
    {
        DataType char4_data_type(LogicalType::kChar4);
        auto small2char4_ptr = BindIntegerCast<SmallIntT>(smallint_type, char4_data_type);
        EXPECT_NE(small2char4_ptr.function, nullptr);

        ColumnVector col_char4(char4_data_type, ColumnVectorType::kFlat);
        col_char4.Initialize();

        CastParameters cast_parameters;
        bool result = small2char4_ptr.function(col_smallint, col_char4, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char4.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar4);
            i16 check_value = static_cast<i16>(i);
            if (check_value >= -999 && check_value <= 9999) {
                EXPECT_FALSE(vx.is_null());
                String output_str(vx.value_.char4.value, 4);
                String check_str(std::to_string(check_value));
                EXPECT_STREQ(output_str.c_str(), check_str.c_str());
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }
    // cast small int column vector to Char8 vector
    {
        DataType char8_data_type(LogicalType::kChar8);
        auto small2char8_ptr = BindIntegerCast<SmallIntT>(smallint_type, char8_data_type);
        EXPECT_NE(small2char8_ptr.function, nullptr);

        ColumnVector col_char8(char8_data_type, ColumnVectorType::kFlat);
        col_char8.Initialize();

        CastParameters cast_parameters;
        bool result = small2char8_ptr.function(col_smallint, col_char8, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char8.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar8);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char8.value, 8);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }
    // cast small int column vector to Char16 vector
    {
        DataType char16_data_type(LogicalType::kChar16);
        auto small2char16_ptr = BindIntegerCast<SmallIntT>(smallint_type, char16_data_type);
        EXPECT_NE(small2char16_ptr.function, nullptr);

        ColumnVector col_char16(char16_data_type, ColumnVectorType::kFlat);
        col_char16.Initialize();

        CastParameters cast_parameters;
        bool result = small2char16_ptr.function(col_smallint, col_char16, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char16.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar16);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char16.value, 16);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }
    // cast small int column vector to Char32 vector
    {
        DataType char32_data_type(LogicalType::kChar32);
        auto small2char32_ptr = BindIntegerCast<SmallIntT>(smallint_type, char32_data_type);
        EXPECT_NE(small2char32_ptr.function, nullptr);

        ColumnVector col_char32(char32_data_type, ColumnVectorType::kFlat);
        col_char32.Initialize();

        CastParameters cast_parameters;
        bool result = small2char32_ptr.function(col_smallint, col_char32, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char32.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar32);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char32.value, 32);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }
    // cast small int column vector to Char64 vector
    {
        DataType char64_data_type(LogicalType::kChar64);
        auto small2char64_ptr = BindIntegerCast<SmallIntT>(smallint_type, char64_data_type);
        EXPECT_NE(small2char64_ptr.function, nullptr);

        ColumnVector col_char64(char64_data_type, ColumnVectorType::kFlat);
        col_char64.Initialize();

        CastParameters cast_parameters;
        bool result = small2char64_ptr.function(col_smallint, col_char64, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char64.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar64);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char64.value, 64);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }
    // cast tiny int column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto small2varchar_ptr = BindIntegerCast<SmallIntT>(smallint_type, varchar_data_type);
        EXPECT_NE(small2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        bool result = small2varchar_ptr.function(col_smallint, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            i16 check_value = static_cast<i16>(i);
            EXPECT_FALSE(vx.is_null());
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast tiny int to other types.
    {
        DataType source(LogicalType::kSmallInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<SmallIntT>(source, target), TypeException);
    }
}

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

class BigIntCastTest : public BaseTest {
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

TEST_F(BigIntCastTest, bigint_cast0) {
    using namespace infinity;

    // Integer to Integer, throw exception
    {
        BigIntT source = 0;
        BigIntT target;

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), TypeException);
    }

    // BigIntT to TinyInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        TinyIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
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

    // BigIntT to SmallInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        SmallIntT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
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
    // BigIntT to IntegerT
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        IntegerT target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = std::numeric_limits<IntegerT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target);
    }
    // BigIntT to HugeInt
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        HugeIntT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_EQ(source, target.lower);
        EXPECT_EQ((static_cast<i64>(source) < 0) * -1, target.upper);
    }
    // BigIntT to Float
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        FloatT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }
    // BigIntT to Double
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        DoubleT target;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);

        source = 0;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        EXPECT_FLOAT_EQ(source, target);
    }

    // TODO:
    // Cast to decimal16/32/64/128
    // BigIntT to Char1T
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        Char1T target;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<BigIntT>::max();
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
    // BigIntT to Char2T
    {
        BigIntT source = std::numeric_limits<BigIntT>::min();
        Char2T target;
        String src_str, tgt_str;
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<BigIntT>::max();
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

    // BigIntT to Char4T
    {
        BigIntT source;
        Char4T target;
        String src_str, tgt_str;

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<BigIntT>::max();
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

    // BigIntT to Char8T
    {
        BigIntT source;
        Char8T target;
        String src_str, tgt_str;

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<BigIntT>::max();
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

        source = -9999999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 99999999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 8);
        tgt_str = String(target.value, 8);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // BigIntT to Char16T
    {
        BigIntT source;
        Char16T target;
        String src_str, tgt_str;

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_FALSE(IntegerTryCastToFixlen::Run(source, target));
        source = std::numeric_limits<BigIntT>::max();
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

        source = -999999999999999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 16);
        tgt_str = String(target.value, 16);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = 9999999999999999;
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 16);
        tgt_str = String(target.value, 16);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());
    }

    // BigIntT to Char32T
    {
        BigIntT source;
        Char32T target;
        String src_str, tgt_str;

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 20);
        tgt_str = String(target.value, 20);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 19);
        tgt_str = String(target.value, 19);
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

    // BigIntT to Char64T
    {
        BigIntT source;
        Char64T target;
        String src_str, tgt_str;

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 20);
        tgt_str = String(target.value, 20);
        EXPECT_STREQ(src_str.c_str(), tgt_str.c_str());

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToFixlen::Run(source, target));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 19);
        tgt_str = String(target.value, 19);
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

    // BigIntT to VarcharT
    {
        BigIntT source;
        VarcharT target;
        String src_str, tgt_str;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        source = std::numeric_limits<BigIntT>::min();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));

        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 20);
        EXPECT_STREQ(src_str.c_str(), target.ToString().c_str());

        source = std::numeric_limits<BigIntT>::max();
        EXPECT_TRUE(IntegerTryCastToVarlen::Run(source, target, &col_varchar));
        src_str = std::to_string(source);
        EXPECT_EQ(src_str.size(), 19);
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

TEST_F(BigIntCastTest, bigint_cast1) {
    using namespace infinity;

    DataType bigint_type(LogicalType::kBigInt);
    ColumnVector col_bigint(bigint_type, ColumnVectorType::kFlat);
    col_bigint.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBigInt(static_cast<BigIntT>(i));
        col_bigint.AppendValue(v);
        Value vx = col_bigint.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_bigint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
    }

    // cast bigint column vector to tiny int column vector
    {
        DataType tinyint_data_type(LogicalType::kTinyInt);
        auto bigint2tiny_ptr = BindIntegerCast<BigIntT>(bigint_type, tinyint_data_type);
        EXPECT_NE(bigint2tiny_ptr.function, nullptr);

        ColumnVector col_tinyint(tinyint_data_type, ColumnVectorType::kFlat);
        col_tinyint.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2tiny_ptr.function(col_bigint, col_tinyint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_tinyint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
            i64 check_value = static_cast<i64>(i);
            if (check_value >= std::numeric_limits<i8>::min() && check_value <= std::numeric_limits<i8>::max()) {
                EXPECT_FALSE(vx.is_null());
                EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(check_value));
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }

    // cast bigint column vector to small integer column vector
    {
        DataType small_data_type(LogicalType::kSmallInt);
        auto bigint2small_ptr = BindIntegerCast<BigIntT>(bigint_type, small_data_type);
        EXPECT_NE(bigint2small_ptr.function, nullptr);

        ColumnVector col_smallint(small_data_type, ColumnVectorType::kFlat);
        col_smallint.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2small_ptr.function(col_bigint, col_smallint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_smallint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
            i64 check_value = static_cast<i64>(i);
            EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(check_value));
        }
    }

    // cast big int column vector to int column vector
    {
        DataType int_data_type(LogicalType::kInteger);
        auto bigint2int_ptr = BindIntegerCast<BigIntT>(bigint_type, int_data_type);
        EXPECT_NE(bigint2int_ptr.function, nullptr);

        ColumnVector col_int(int_data_type, ColumnVectorType::kFlat);
        col_int.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2int_ptr.function(col_bigint, col_int, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_int.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
            i64 check_value = static_cast<i64>(i);
            EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(check_value));
        }
    }

    // cast bigint column vector to huge int column vector
    {
        DataType hugeint_data_type(LogicalType::kHugeInt);
        auto bigint2hugeint_ptr = BindIntegerCast<BigIntT>(bigint_type, hugeint_data_type);
        EXPECT_NE(bigint2hugeint_ptr.function, nullptr);

        ColumnVector col_hugeint(hugeint_data_type, ColumnVectorType::kFlat);
        col_hugeint.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2hugeint_ptr.function(col_bigint, col_hugeint, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_hugeint.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
            HugeIntT check_value((static_cast<i64>(i) < 0) * -1, static_cast<i64>(i));
            EXPECT_EQ(vx.value_.huge_int, check_value);
        }
    }

    // cast bigint column vector to float column vector
    {
        DataType float_data_type(LogicalType::kFloat);
        auto bigint2float_ptr = BindIntegerCast<BigIntT>(bigint_type, float_data_type);
        EXPECT_NE(bigint2float_ptr.function, nullptr);

        ColumnVector col_float(float_data_type, ColumnVectorType::kFlat);
        col_float.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2float_ptr.function(col_bigint, col_float, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_float.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(check_value));
        }
    }

    // cast bigint column vector to double column vector
    {
        DataType double_data_type(LogicalType::kDouble);
        auto bigint2double_ptr = BindIntegerCast<BigIntT>(bigint_type, double_data_type);
        EXPECT_NE(bigint2double_ptr.function, nullptr);

        ColumnVector col_double(double_data_type, ColumnVectorType::kFlat);
        col_double.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2double_ptr.function(col_bigint, col_double, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_double.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(check_value));
        }
    }

    // cast bigint column vector to Char1 vector
    {
        DataType char1_data_type(LogicalType::kChar1);
        auto bigint2char1_ptr = BindIntegerCast<BigIntT>(bigint_type, char1_data_type);
        EXPECT_NE(bigint2char1_ptr.function, nullptr);

        ColumnVector col_char1(char1_data_type, ColumnVectorType::kFlat);
        col_char1.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char1_ptr.function(col_bigint, col_char1, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_FALSE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char1.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
            i64 check_value = static_cast<i64>(i);
            if (check_value >= 0 && check_value <= 9) {
                EXPECT_FALSE(vx.is_null());
                EXPECT_EQ(vx.value_.char1.value, char(check_value % 10 + '0'));
            } else {
                EXPECT_TRUE(vx.is_null());
            }
        }
    }

    // cast bigint column vector to Char2 vector
    {
        DataType char2_data_type(LogicalType::kChar2);
        auto bigint2char2_ptr = BindIntegerCast<BigIntT>(bigint_type, char2_data_type);
        EXPECT_NE(bigint2char2_ptr.function, nullptr);

        ColumnVector col_char2(char2_data_type, ColumnVectorType::kFlat);
        col_char2.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char2_ptr.function(col_bigint, col_char2, DEFAULT_VECTOR_SIZE, cast_parameters);
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

    // cast bigint column vector to Char4 vector
    {
        DataType char4_data_type(LogicalType::kChar4);
        auto bigint2char4_ptr = BindIntegerCast<BigIntT>(bigint_type, char4_data_type);
        EXPECT_NE(bigint2char4_ptr.function, nullptr);

        ColumnVector col_char4(char4_data_type, ColumnVectorType::kFlat);
        col_char4.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char4_ptr.function(col_bigint, col_char4, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char4.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar4);
            i64 check_value = static_cast<i64>(i);
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

    // cast bigint column vector to Char8 vector
    {
        DataType char8_data_type(LogicalType::kChar8);
        auto bigint2char8_ptr = BindIntegerCast<BigIntT>(bigint_type, char8_data_type);
        EXPECT_NE(bigint2char8_ptr.function, nullptr);

        ColumnVector col_char8(char8_data_type, ColumnVectorType::kFlat);
        col_char8.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char8_ptr.function(col_bigint, col_char8, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char8.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar8);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char8.value, 8);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast bigint column vector to Char16 vector
    {
        DataType char16_data_type(LogicalType::kChar16);
        auto bigint2char16_ptr = BindIntegerCast<BigIntT>(bigint_type, char16_data_type);
        EXPECT_NE(bigint2char16_ptr.function, nullptr);

        ColumnVector col_char16(char16_data_type, ColumnVectorType::kFlat);
        col_char16.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char16_ptr.function(col_bigint, col_char16, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char16.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar16);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char16.value, 16);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast bigint column vector to Char32 vector
    {
        DataType char32_data_type(LogicalType::kChar32);
        auto bigint2char32_ptr = BindIntegerCast<BigIntT>(bigint_type, char32_data_type);
        EXPECT_NE(bigint2char32_ptr.function, nullptr);

        ColumnVector col_char32(char32_data_type, ColumnVectorType::kFlat);
        col_char32.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char32_ptr.function(col_bigint, col_char32, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char32.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar32);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char32.value, 32);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast bigint column vector to Char64 vector
    {
        DataType char64_data_type(LogicalType::kChar64);
        auto bigint2char64_ptr = BindIntegerCast<BigIntT>(bigint_type, char64_data_type);
        EXPECT_NE(bigint2char64_ptr.function, nullptr);

        ColumnVector col_char64(char64_data_type, ColumnVectorType::kFlat);
        col_char64.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2char64_ptr.function(col_bigint, col_char64, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_char64.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kChar64);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FALSE(vx.is_null());
            String output_str(vx.value_.char64.value, 64);
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(output_str.c_str(), check_str.c_str());
        }
    }

    // cast bigint column vector to Varchar vector
    {
        DataType varchar_data_type(LogicalType::kVarchar);
        auto bigint2varchar_ptr = BindIntegerCast<BigIntT>(bigint_type, varchar_data_type);
        EXPECT_NE(bigint2varchar_ptr.function, nullptr);

        ColumnVector col_varchar(varchar_data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        CastParameters cast_parameters;
        bool result = bigint2varchar_ptr.function(col_bigint, col_varchar, DEFAULT_VECTOR_SIZE, cast_parameters);
        // Not all values are cast, then it's false.
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar.GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            i64 check_value = static_cast<i64>(i);
            EXPECT_FALSE(vx.is_null());
            String check_str(std::to_string(check_value));
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_str.c_str());
        }
    }

    // Throw exception when cast int to other types.
    {
        DataType source(LogicalType::kBigInt);
        DataType target(LogicalType::kTimestamp);
        EXPECT_THROW(BindIntegerCast<IntegerT>(source, target), TypeException);
    }
}


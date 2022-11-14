//
// Created by JinHai on 2022/11/13.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"

class ValueTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
    }
};

TEST_F(ValueTest, MakeAndGet) {

    using namespace infinity;

    // Boolean
    Value value = Value::MakeBool(true);
    EXPECT_EQ(value.GetValue<BooleanT>(), true);

    value = Value::MakeBool(false);
    EXPECT_EQ(value.GetValue<BooleanT>(), false);

    // TinyInt
    for(i8 input = std::numeric_limits<i8>::min(); ; ++ input) {
//        printf("%d\n", input);
        value = Value::MakeTinyInt(input);
        EXPECT_EQ(value.GetValue<TinyIntT>(), input);
        if(input == std::numeric_limits<i8>::max()) break;
    }

    // SmallInt
    for(i16 input = std::numeric_limits<i16>::min(); ; ++ input) {
//        printf("%d\n", input);
        value = Value::MakeSmallInt(input);
        EXPECT_EQ(value.GetValue<SmallIntT>(), input);
        if(input == std::numeric_limits<i16>::max()) break;
    }

    // Integer
    for(i32 input = std::numeric_limits<i32>::min(); ; input += std::numeric_limits<u16>::max()) {
        value = Value::MakeInt(input);
        EXPECT_EQ(value.GetValue<IntegerT>(), input);
        if(input == std::numeric_limits<i32>::max()) break;
    }

    // Big Integer
    value = Value::MakeBigInt(std::numeric_limits<i64>::min());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::min());

    value = Value::MakeBigInt(0);
    EXPECT_EQ(value.GetValue<BigIntT>(), 0);

    value = Value::MakeBigInt(std::numeric_limits<i64>::max());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::max());

    // Huge Integer
    HugeInt huge_int(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max());
    value = Value::MakeHugeInt(huge_int);
    EXPECT_EQ(value.GetValue<HugeIntT>(), HugeInt(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max()));

    // Float32
    value = Value::MakeFloat(0);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 0);
    value = Value::MakeFloat(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), -1.5);
    value = Value::MakeFloat(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 3.232);

    // Float64
    value = Value::MakeDouble(0);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 0);
    value = Value::MakeDouble(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), -1.5);
    value = Value::MakeDouble(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 3.232);

    // Decimal16
    Decimal16T decimal_16(100);
    value = Value::MakeDecimal16(decimal_16);
    EXPECT_EQ(value.GetValue<Decimal16T>(), decimal_16);

    // Decimal32
    Decimal32T decimal_32(1000);
    value = Value::MakeDecimal32(decimal_32);
    EXPECT_EQ(value.GetValue<Decimal32T>(), decimal_32);

    // Decimal64
    Decimal64T decimal_64(10000);
    value = Value::MakeDecimal64(decimal_64);
    EXPECT_EQ(value.GetValue<Decimal64T>(), decimal_64);

    // Decimal128
    Decimal128T decimal_128(10000, 10001);
    value = Value::MakeDecimal128(decimal_128);
    EXPECT_EQ(value.GetValue<Decimal128T>(), decimal_128);

    // Varchar (inline)
    value = Value::MakeVarchar("Hello World!");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), true);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World!");

    // Varchar (heap allocation)
    value = Value::MakeVarchar("Hello World, Hello World");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), false);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World, Hello World");

    // Char1
    for(char input = std::numeric_limits<char>::min(); ; input += std::numeric_limits<char>::max()) {
        Char1T char1(input);
        value = Value::MakeChar1(char1);
        EXPECT_EQ(value.GetValue<Char1T>().value, input);
        if(input == std::numeric_limits<char>::max()) break;
    }
}

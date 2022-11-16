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
    value.Reset();

    value = Value::MakeBool(false);
    EXPECT_EQ(value.GetValue<BooleanT>(), false);
    value.Reset();

    // TinyInt
    for(i8 input = std::numeric_limits<i8>::min(); ; ++ input) {
//        printf("%d\n", input);
        value = Value::MakeTinyInt(input);
        EXPECT_EQ(value.GetValue<TinyIntT>(), input);
        value.Reset();
        if(input == std::numeric_limits<i8>::max()) break;
    }

    // SmallInt
    for(i16 input = std::numeric_limits<i16>::min(); ; ++ input) {
//        printf("%d\n", input);
        value = Value::MakeSmallInt(input);
        EXPECT_EQ(value.GetValue<SmallIntT>(), input);
        value.Reset();
        if(input == std::numeric_limits<i16>::max()) break;
    }

    // Integer
    for(i32 input = std::numeric_limits<i32>::min(); ; input += std::numeric_limits<u16>::max()) {
        value = Value::MakeInt(input);
        EXPECT_EQ(value.GetValue<IntegerT>(), input);
        value.Reset();
        if(input == std::numeric_limits<i32>::max()) break;
    }

    // Big Integer
    value = Value::MakeBigInt(std::numeric_limits<i64>::min());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::min());
    value.Reset();

    value = Value::MakeBigInt(0);
    EXPECT_EQ(value.GetValue<BigIntT>(), 0);
    value.Reset();

    value = Value::MakeBigInt(std::numeric_limits<i64>::max());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::max());
    value.Reset();

    // Huge Integer
    HugeInt huge_int(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max());
    value = Value::MakeHugeInt(huge_int);
    EXPECT_EQ(value.GetValue<HugeIntT>(), HugeInt(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max()));
    value.Reset();

    // Float32
    value = Value::MakeFloat(0);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 0);
    value = Value::MakeFloat(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), -1.5);
    value = Value::MakeFloat(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 3.232);
    value.Reset();

    // Float64
    value = Value::MakeDouble(0);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 0);
    value = Value::MakeDouble(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), -1.5);
    value = Value::MakeDouble(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 3.232);
    value.Reset();

    // Decimal16
    Decimal16T decimal_16(100);
    value = Value::MakeDecimal16(decimal_16);
    EXPECT_EQ(value.GetValue<Decimal16T>(), decimal_16);
    value.Reset();

    // Decimal32
    Decimal32T decimal_32(1000);
    value = Value::MakeDecimal32(decimal_32);
    EXPECT_EQ(value.GetValue<Decimal32T>(), decimal_32);
    value.Reset();

    // Decimal64
    Decimal64T decimal_64(10000);
    value = Value::MakeDecimal64(decimal_64);
    EXPECT_EQ(value.GetValue<Decimal64T>(), decimal_64);
    value.Reset();

    // Decimal128
    Decimal128T decimal_128(10000, 10001);
    value = Value::MakeDecimal128(decimal_128);
    EXPECT_EQ(value.GetValue<Decimal128T>(), decimal_128);
    value.Reset();

    // Varchar (inline)
    value = Value::MakeVarchar("Hello World!");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), true);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World!");
    value.Reset();

    // Varchar (heap allocation)
    value = Value::MakeVarchar("Hello World, Hello World");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), false);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World, Hello World");
    value.Reset();

    // Char1
    for(char input = std::numeric_limits<char>::min(); ; input += std::numeric_limits<char>::max()) {
        Char1T char1(input);
        value = Value::MakeChar1(char1);
        EXPECT_EQ(value.GetValue<Char1T>().value, input);
        value.Reset();
        if(input == std::numeric_limits<char>::max()) break;
    }

    // Char2
    {
        String s;
        for(size_t i = 0; i < Char2T::CHAR_LENGTH + 1; ++ i) {
            Char2T char1(s);
            value = Value::MakeChar2(char1);
            EXPECT_EQ(value.GetValue<Char2T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char2T char2(s);
        value = Value::MakeChar2(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char2T>().ToString(), s);
        value.Reset();
    }

    // Char4
    {
        String s;
        for(size_t i = 0; i < Char4T::CHAR_LENGTH + 1; ++ i) {
            Char4T char1(s);
            value = Value::MakeChar4(char1);
            EXPECT_EQ(value.GetValue<Char4T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char4T char2(s);
        value = Value::MakeChar4(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char4T>().ToString(), s);
        value.Reset();
    }

    // Char8
    {
        String s;
        for(size_t i = 0; i < Char8T::CHAR_LENGTH + 1; ++ i) {
            Char8T char1(s);
            value = Value::MakeChar8(char1);
            EXPECT_EQ(value.GetValue<Char8T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char8T char2(s);
        value = Value::MakeChar8(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char8T>().ToString(), s);
        value.Reset();
    }

    // Char15
    {
        String s;
        for(size_t i = 0; i < Char15T::CHAR_LENGTH + 1; ++ i) {
            Char15T char1(s);
            value = Value::MakeChar15(char1);
            EXPECT_EQ(value.GetValue<Char15T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char15T char2(s);
        value = Value::MakeChar15(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char15T>().ToString(), s);
        value.Reset();
    }

    // Char31
    {
        String s;
        for(size_t i = 0; i < Char31T::CHAR_LENGTH + 1; ++ i) {
            Char31T char1(s);
            value = Value::MakeChar31(char1);
            EXPECT_EQ(value.GetValue<Char31T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char31T char2(s);
        value = Value::MakeChar31(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char31T>().ToString(), s);
        value.Reset();
    }

    // Char63
    {
        String s;
        for(size_t i = 0; i < Char63T::CHAR_LENGTH + 1; ++ i) {
            Char63T char1(s);
            value = Value::MakeChar63(char1);
            EXPECT_EQ(value.GetValue<Char63T>().ToString(), s);
            s.push_back('a');
            value.Reset();
        }

        Char63T char2(s);
        value = Value::MakeChar63(char2);
        s.pop_back();
        EXPECT_EQ(value.GetValue<Char63T>().ToString(), s);
        value.Reset();
    }

    // Date
    {
        DateT date;
        for(i32 i = 0; i < 365; ++ i) {
            date.value = i;
            value = Value::MakeDate(date);
            EXPECT_EQ(value.GetValue<DateT>().value, i);
            value.Reset();
        }
    }

    // Time
    {
        TimeT time;
        for(i32 i = 0; i < 3600 * 24; ++ i) {
            time.value = i;
            value = Value::MakeTime(time);
            EXPECT_EQ(value.GetValue<TimeT>().value, i);
            value.Reset();
        }
    }

    // Datetime
    {
        DateTimeT datetime;
        for(i32 i = 0; i < 365; ++ i) {
            datetime.date = i;
            datetime.time = 3600 - i;
            value = Value::MakeDateTime(datetime);
            EXPECT_EQ(value.GetValue<DateTimeT>().date, i);
            EXPECT_EQ(value.GetValue<DateTimeT>().time, 3600 - i);
            value.Reset();
        }
    }

    // Timestamp
    {
        TimestampT timestamp;
        for(i32 i = 0; i < 365; ++ i) {
            timestamp.date = i;
            timestamp.time = 3600 - i;
            value = Value::MakeTimestamp(timestamp);
            EXPECT_EQ(value.GetValue<TimestampT>().date, i);
            EXPECT_EQ(value.GetValue<TimestampT>().time, 3600 - i);
            value.Reset();
        }
    }

    // TimestampTz
    {
        TimestampTZT timestamp_tz;
        for(i32 i = 0; i < 365; ++ i) {
            timestamp_tz.date = i;
            timestamp_tz.time = 3600 - i;
            value = Value::MakeTimestampTz(timestamp_tz);
            EXPECT_EQ(value.GetValue<TimestampTZT>().date, i);
            EXPECT_EQ(value.GetValue<TimestampTZT>().time, 3600 - i);
            value.Reset();
        }
    }

    // Interval
    {
        IntervalT interval;
        for(i32 unit = 1; unit < 7; ++ unit) {
            interval.unit = unit;
            for(i32 v = 1; v < 3600; ++ v) {
                interval.value = v;
                value = Value::MakeInterval(interval);
                EXPECT_EQ(value.GetValue<IntervalT>().unit, unit);
                EXPECT_EQ(value.GetValue<IntervalT>().value, v);
                value.Reset();
            }
        }
    }

    // Array
    {
        LOG_TRACE("TODO: need array type test");
    }

    // Tuple
    {
        LOG_TRACE("TODO: need tuple type test");
    }

    // PointT
    {
        LOG_TRACE("TODO: need point type test");
    }

    // Mixed
    {
        LOG_TRACE("TODO: need heterogeneous type test");
    }

}



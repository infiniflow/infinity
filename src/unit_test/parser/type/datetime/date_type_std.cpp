#include "type/datetime/interval_type.h"
#include "gtest/gtest.h"

import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import internal_types;
import parser_assert;

using namespace infinity;
class DateTypeStdTest : public BaseTest {};

TEST_F(DateTypeStdTest, TestSameSize) {
    using namespace infinity;
    EXPECT_EQ(sizeof(DateT), sizeof(DateStdT));
}

TEST_F(DateTypeStdTest, TestFromString) {
    using namespace infinity;
    DateStdT date_std;
    EXPECT_NO_THROW(date_std.FromString("2020-01-30"));
    EXPECT_NO_THROW(date_std.FromString("4535-02-11"));
    EXPECT_NO_THROW(date_std.FromString("2002-05-26"));
    EXPECT_NO_THROW(date_std.FromString("-1587-8-1"));
    EXPECT_NO_THROW(date_std.FromString("-4532/1/23"));
    EXPECT_NO_THROW(date_std.FromString("2024/9/4"));
    EXPECT_NO_THROW(date_std.FromString("2024-9/4"));
    EXPECT_NO_THROW(date_std.FromString("2024/9-4"));

    EXPECT_THROW(date_std.FromString("2018/2/29"), ParserException);
    EXPECT_THROW(date_std.FromString("20-1233/45"), ParserException);
    EXPECT_THROW(date_std.FromString("-12354--56f1--ade"), ParserException);
    EXPECT_THROW(date_std.FromString("1234@56.789"), ParserException);
    EXPECT_THROW(date_std.FromString("qwlssmabz"), ParserException);
}

TEST_F(DateTypeStdTest, TestAddSubstract) {
    using namespace infinity;
    DateStdT date_std_input;
    DateStdT date_std_output;
    IntervalT interval;

    EXPECT_NO_THROW(date_std_input.FromString("2020-01-30"));
    EXPECT_EQ(date_std_input.ToString(), "2020-01-30");

    interval.value = 1;
    interval.unit = kMonth;
    EXPECT_TRUE(DateStdT::Add(date_std_input, interval, date_std_output));
    EXPECT_EQ(date_std_output.ToString(), "2020-02-29");

    interval.value = 1;
    interval.unit = kYear;
    EXPECT_TRUE(DateStdT::Add(date_std_input, interval, date_std_output));
    EXPECT_EQ(date_std_output.ToString(), "2021-01-29");
}

// TEST_F(DateTypeStdTest, TestLeapYear) {
//     using namespace infinity;
// }

TEST_F(DateTypeStdTest, TestNegativeYears) {
    using namespace infinity;
    DateStdT date;
    DateStdT date_shift;
    EXPECT_NO_THROW(date.FromString("-1/5/4"));

    IntervalT interval;
    interval.unit = kYear;
    interval.value = 3;

    EXPECT_EQ(date.ToString(), "-001-05-04");
    EXPECT_EQ(DateStdT::GetDatePart(date, kYear), -1);
    EXPECT_EQ(DateStdT::GetDatePart(date, kMonth), 5);
    EXPECT_EQ(DateStdT::GetDatePart(date, kDay), 4);

    EXPECT_TRUE(DateStdT::Add(date, interval, date_shift));
    EXPECT_EQ(date_shift.ToString(), "0002-05-03");

    EXPECT_TRUE(DateStdT::Subtract(date_shift, interval, date_shift));
    EXPECT_EQ(date_shift.ToString(), "-001-05-04");
}

// TEST_F(DateTypeStdTest, TestComparator) {
//     using namespace infinity;
//     DateStdT d1;
//     DateStdT d2;

// }
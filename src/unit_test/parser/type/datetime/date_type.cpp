// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
class DateTypeTest : public BaseTest {};


//
TEST_F(DateTypeTest, test1) {
    using namespace infinity;

    DateT date1;

    EXPECT_EQ(date1.ToString(), "1970-01-01");

    date1.FromString("2000-01-01");
    EXPECT_EQ(date1.ToString(), "2000-01-01");

    date1.FromString("2000/10/31");
    EXPECT_EQ(date1.ToString(), "2000-10-31");

    date1.FromString("1-1-1");
    EXPECT_EQ(date1.ToString(), "0001-01-01");

    date1.FromString("9999-12-31");
    EXPECT_EQ(date1.ToString(), "9999-12-31");

    EXPECT_THROW(date1.FromString("0-0-0"), ParserException);
    EXPECT_THROW(date1.FromString("0/0/0"), ParserException);
}

TEST_F(DateTypeTest, TestEqStdChronoForward) {
    using namespace infinity;
    using namespace std::chrono;

    //2020-1-31
    DateT date;
    date.FromString("2020-01-31");

    tm tmdate = {};
    tmdate.tm_year = 2020 - 1900;
    tmdate.tm_mon = 1 - 1;
    tmdate.tm_mday = 31;
    time_t time_c = mktime(&tmdate);
    system_clock::time_point tp = system_clock::from_time_t(time_c);
    sys_days sysdays = ceil<days>(tp);
    
    for(i32 i = 0; i < 30000; i++) {
        year_month_day ymd = year_month_day(sysdays);
        String ymd_s = format("{:%Y-%m-%d}", ymd);
        EXPECT_STREQ(date.ToString().c_str(), ymd_s.c_str());

        sysdays += days{1};

        DateT date_output;
        IntervalT oneday_interval(1);
        oneday_interval.unit = TimeUnit::kDay;
        EXPECT_TRUE(date.Add(date, oneday_interval, date_output));
        date = date_output;
    }

}

TEST_F(DateTypeTest, TestEqStdChronoBackward) {
    using namespace infinity; 
    using namespace std::chrono;

    //2020-1-31
    //std::chrono::
    DateT date;
    date.FromString("2020-01-31");

    tm tmdate = {};
    tmdate.tm_year = 2020 - 1900;
    tmdate.tm_mon = 1 - 1;
    tmdate.tm_mday = 31;
    time_t time_c = mktime(&tmdate);
    system_clock::time_point tp = system_clock::from_time_t(time_c);
    sys_days sysdays = ceil<days>(tp);
    
    for(i32 i = 0; i < 30000; i++) {
        year_month_day ymd = year_month_day(sysdays);
        String ymd_s = format("{:%Y-%m-%d}", ymd);
        EXPECT_STREQ(date.ToString().c_str(), ymd_s.c_str());

        sysdays -= days{1};

        DateT date_output;
        IntervalT oneday_interval(1);
        oneday_interval.unit = TimeUnit::kDay;
        EXPECT_TRUE(date.Subtract(date, oneday_interval, date_output));
        date = date_output;
    }

}
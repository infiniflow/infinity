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
import null_value;
import stl;
import infinity_context;
import internal_types;

using namespace infinity;
class NullValueTest : public BaseTest {};

TEST_F(NullValueTest, null_value_test1) {
    using namespace infinity;

    {
        auto null_value = NullValue<BooleanT>();
        EXPECT_EQ(null_value, false);
    }
    {
        auto null_value = NullValue<TinyIntT>();
        EXPECT_EQ(null_value, std::numeric_limits<TinyIntT>::infinity());
    }
    {
        auto null_value = NullValue<SmallIntT>();
        EXPECT_EQ(null_value, std::numeric_limits<SmallIntT>::infinity());
    }
    {
        auto null_value = NullValue<IntegerT>();
        EXPECT_EQ(null_value, std::numeric_limits<IntegerT>::infinity());
    }
    {
        auto null_value = NullValue<BigIntT>();
        EXPECT_EQ(null_value, std::numeric_limits<BigIntT>::infinity());
    }
    {
        auto null_value = NullValue<HugeIntT>();
        EXPECT_EQ(null_value.lower, std::numeric_limits<BigIntT>::infinity());
        EXPECT_EQ(null_value.upper, std::numeric_limits<BigIntT>::infinity());
    }
    {
        auto null_value = NullValue<FloatT>();
        EXPECT_EQ(null_value, std::numeric_limits<FloatT>::infinity());
    }
    {
        auto null_value = NullValue<DoubleT>();
        EXPECT_EQ(null_value, std::numeric_limits<DoubleT>::infinity());
    }
    {
        auto null_value = NullValue<DecimalT>();
        EXPECT_EQ(null_value.upper, std::numeric_limits<i64>::infinity());
        EXPECT_EQ(null_value.lower, std::numeric_limits<i64>::infinity());
    }
    /*
     * using DateT = DateType;
using TimeT = TimeType;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
using TimestampTZT = TimestampTZType;
using IntervalT = IntervalType;
     */

    {
        auto null_value = NullValue<DateT>();
        EXPECT_EQ(null_value.value, std::numeric_limits<i32>::infinity());
    }
    {
        auto null_value = NullValue<TimeT>();
        EXPECT_EQ(null_value.value, std::numeric_limits<i32>::infinity());
    }
    {
        auto null_value = NullValue<DateTimeT>();
        EXPECT_EQ(null_value.date, std::numeric_limits<i32>::infinity());
        EXPECT_EQ(null_value.time, std::numeric_limits<i32>::infinity());
    }
    {
        auto null_value = NullValue<TimestampT>();
        EXPECT_EQ(null_value.date, std::numeric_limits<i32>::infinity());
        EXPECT_EQ(null_value.time, std::numeric_limits<i32>::infinity());
    }
    {
        auto null_value = NullValue<IntervalT>();
        EXPECT_EQ(null_value.unit, TimeUnit::kInvalidUnit);
        EXPECT_EQ(null_value.value, std::numeric_limits<i32>::infinity());
    }
    {
        auto null_value = NullValue<PointT>();
        EXPECT_EQ(null_value.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.y, std::numeric_limits<f64>::infinity());
    }
    {
        auto null_value = NullValue<LineT>();
        EXPECT_EQ(null_value.a, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.b, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.c, std::numeric_limits<f64>::infinity());
    }
    {
        auto null_value = NullValue<LineSegT>();
        EXPECT_EQ(null_value.point1.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.point1.y, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.point2.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.point2.y, std::numeric_limits<f64>::infinity());
    }
    {
        auto null_value = NullValue<BoxT>();
        EXPECT_EQ(null_value.lower_right.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.lower_right.y, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.upper_left.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.upper_left.y, std::numeric_limits<f64>::infinity());
    }
//    {
//        auto null_value = NullValue<PathT>();
//        EXPECT_EQ(null_value.point_count, std::numeric_limits<u32>::infinity());
//        EXPECT_EQ(null_value.closed, std::numeric_limits<i32>::infinity());
//        EXPECT_EQ(null_value.ptr, std::numeric_limits<ptr_t>::infinity());
//    }
//    {
//        auto null_value = NullValue<PolygonT>();
//        EXPECT_EQ(null_value.point_count, std::numeric_limits<u32>::infinity());
//        EXPECT_EQ(null_value.ptr, std::numeric_limits<ptr_t>::infinity());
//        EXPECT_EQ(null_value.bounding_box.upper_left.x, std::numeric_limits<f64>::infinity());
//        EXPECT_EQ(null_value.bounding_box.upper_left.y, std::numeric_limits<f64>::infinity());
//        EXPECT_EQ(null_value.bounding_box.lower_right.x, std::numeric_limits<f64>::infinity());
//        EXPECT_EQ(null_value.bounding_box.lower_right.y, std::numeric_limits<f64>::infinity());
//    }
    {
        auto null_value = NullValue<CircleT>();
        EXPECT_EQ(null_value.radius, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.center.x, std::numeric_limits<f64>::infinity());
        EXPECT_EQ(null_value.center.y, std::numeric_limits<f64>::infinity());
    }
    {
        auto null_value = NullValue<MixedT>();
        EXPECT_EQ(null_value.type, MixedValueType::kNull);
    }
}

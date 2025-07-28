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
import parser_assert;

#if 0
class PolygonTypeTest : public BaseTest {};

TEST_F(PolygonTypeTest, polygon1) {
    using namespace infinity;

    PolygonT polygon1;
    EXPECT_EQ(polygon1.PointCount(), 0);
    EXPECT_THROW(polygon1.GetPoint(0), ParserException);
    EXPECT_THROW(polygon1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    polygon1.Initialize(2);
    EXPECT_THROW(polygon1.Initialize(0), ParserException);
    polygon1.SetPoint(0, PointT(1.0, 2.1));
    polygon1.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_THROW(polygon1.SetPoint(2, PointT(1.3, 2.5)), ParserException);
    EXPECT_EQ(polygon1.PointCount(), 2);
    EXPECT_EQ(polygon1.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon1.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon1.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon1.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon1.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon1.GetPoint(0).y, PointT(1.0, 2.1).y);
    //    path1.SetPointCount(2);
    EXPECT_EQ(polygon1.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon1.GetPoint(1).y, PointT(1.1, 2.2).y);
    polygon1.Reset();

    EXPECT_EQ(polygon1.PointCount(), 0);

    EXPECT_THROW(polygon1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    PolygonT polygon2(2);
    polygon2.SetPoint(0, PointT(1.0, 2.1));
    polygon2.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_EQ(polygon2.PointCount(), 2);

    PolygonT polygon3(3);
    EXPECT_EQ(polygon3.PointCount(), 3);
    EXPECT_EQ(polygon1.bounding_box.upper_left.x, std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.upper_left.y, -std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.lower_right.x, -std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.lower_right.y, std::numeric_limits<f64>::max());

    polygon3 = polygon2;
    EXPECT_EQ(polygon3.PointCount(), 2);
    EXPECT_EQ(polygon3.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon3.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon3.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon3.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon3.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon3.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon3.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon3.bounding_box.lower_right.y, 2.1);

    PolygonT polygon4(2);
    polygon4 = polygon2;
    EXPECT_EQ(polygon4.PointCount(), 2);
    EXPECT_EQ(polygon4.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon4.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon4.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon4.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon4.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon4.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon4.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon4.bounding_box.lower_right.y, 2.1);

    PolygonT polygon5(1);
    polygon5 = std::move(polygon2);
    EXPECT_EQ(polygon5.PointCount(), 2);
    EXPECT_EQ(polygon5.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon5.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon5.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon5.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon5.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon5.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon5.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon5.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon2.PointCount(), 0);
    EXPECT_EQ(polygon2.ptr, nullptr);

    PolygonT polygon6(std::move(polygon4));
    EXPECT_EQ(polygon6.PointCount(), 2);
    EXPECT_EQ(polygon6.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon6.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon6.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon6.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon4.PointCount(), 0);
    EXPECT_EQ(polygon4.ptr, nullptr);

    EXPECT_EQ(polygon6.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon6.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon6.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon6.bounding_box.lower_right.y, 2.1);

    PolygonT polygon7(polygon5);
    EXPECT_EQ(polygon7.PointCount(), 2);
    EXPECT_EQ(polygon7.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon7.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon7.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon7.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon7.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon7.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon7.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon7.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon5.PointCount(), 2);
    EXPECT_NE(polygon5.ptr, nullptr);

    EXPECT_EQ(polygon6 == polygon7, true);
}
#endif
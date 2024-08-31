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

using namespace infinity;
class PathTypeTest : public BaseTest {};

TEST_F(PathTypeTest, path1) {
    using namespace infinity;

    PathT path1;
    EXPECT_EQ(path1.PointCount(), 0);
    EXPECT_THROW(path1.GetPoint(0), ParserException);
    EXPECT_THROW(path1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    path1.Initialize(2);
    EXPECT_THROW(path1.Initialize(0), ParserException);
    path1.SetPoint(0, PointT(1.0, 2.1));
    path1.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_THROW(path1.SetPoint(2, PointT(1.3, 2.5)), ParserException);
    EXPECT_EQ(path1.PointCount(), 2);

    EXPECT_EQ(path1.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path1.GetPoint(0).y, PointT(1.0, 2.1).y);
    //    path1.SetPointCount(2);
    EXPECT_EQ(path1.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path1.GetPoint(1).y, PointT(1.1, 2.2).y);
    path1.Reset();

    EXPECT_EQ(path1.PointCount(), 0);

    EXPECT_THROW(path1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    PathT path2(2);
    path2.SetPoint(0, PointT(1.0, 2.1));
    path2.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_EQ(path2.PointCount(), 2);

    PathT path3(3);
    EXPECT_EQ(path3.PointCount(), 3);
    path3 = path2;
    EXPECT_EQ(path3.PointCount(), 2);
    EXPECT_EQ(path3.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path3.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path3.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path3.GetPoint(1).y, PointT(1.1, 2.2).y);

    PathT path4(2);
    path4 = path2;
    EXPECT_EQ(path4.PointCount(), 2);
    EXPECT_EQ(path4.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path4.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path4.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path4.GetPoint(1).y, PointT(1.1, 2.2).y);

    PathT path5(1);
    path5 = std::move(path2);
    EXPECT_EQ(path5.PointCount(), 2);
    EXPECT_EQ(path5.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path5.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path5.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path5.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path2.PointCount(), 0);
    EXPECT_EQ(path2.ptr, nullptr);

    PathT path6(std::move(path4));
    EXPECT_EQ(path6.PointCount(), 2);
    EXPECT_EQ(path6.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path6.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path6.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path6.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path4.PointCount(), 0);
    EXPECT_EQ(path4.ptr, nullptr);

    PathT path7(path5);
    EXPECT_EQ(path7.PointCount(), 2);
    EXPECT_EQ(path7.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path7.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path7.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path7.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path5.PointCount(), 2);
    EXPECT_NE(path5.ptr, nullptr);

    EXPECT_EQ(path6, path5);
}
#endif
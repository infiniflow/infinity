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

using namespace infinity;
class PointTypeTest : public BaseTest {};

TEST_F(PointTypeTest, point1) {
    using namespace infinity;

    PointT p1;
    p1.x = 1.0f;
    p1.y = 2.0f;

    PointT p2(1.0f, 2.0f);

    EXPECT_EQ(p1.x, 1.0f);
    EXPECT_EQ(p1.y, 2.0f);
    EXPECT_EQ(p2.x, 1.0f);
    EXPECT_EQ(p2.y, 2.0f);

    EXPECT_EQ(p1, p2);

    p2.Reset();

    EXPECT_EQ(p2.x, 0);
    EXPECT_EQ(p2.y, 0);
}

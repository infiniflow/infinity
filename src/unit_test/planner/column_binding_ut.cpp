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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.column_binding;

import :ut.base_test;
import :infinity_exception;
import :third_party;
import :logger;
import :stl;
import :infinity_context;
import :column_binding;
#endif

import global_resource_usage;

using namespace infinity;
class ColumnBindingTest : public BaseTest {};

TEST_F(ColumnBindingTest, test1) {
    using namespace infinity;

    ColumnBinding column1(0, 1);
    ColumnBinding column2(0, 1);
    ColumnBinding column3(1, 2);

    EXPECT_TRUE(column1 == column2);
    EXPECT_TRUE(column1 != column3);
}

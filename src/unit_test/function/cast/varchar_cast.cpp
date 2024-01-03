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

#include "unit_test/base_test.h"

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import catalog;
import parser;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import varchar_cast;

class VarcharTest : public BaseTest {};

TEST_F(VarcharTest, varchar_cast0) {
    using namespace infinity;
    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "true";

        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{false};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(target);
            EXPECT_TRUE(result);
        }

        s1 = "false";
        {
            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(target);
            EXPECT_TRUE(result);
        }

        s1 = "falsk";
        {
            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
        }
    }

    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "-128";

        {
            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, -128);
        }

        s1 = "127";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, 127);
        }

        s1 = "190";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
        }

        s1 = "abc";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.InitAsValue(s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
            EXPECT_EQ(target, 0);
        }
    }
}

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


import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import cast_table;
import column_vector;
import logical_type;

using namespace infinity;
class CastTableTest : public BaseTest {};

TEST_F(CastTableTest, casttable_boolean) {
    using namespace infinity;
    for (i8 to = to_underlying_val(LogicalType::kBoolean); to < to_underlying_val(LogicalType::kInvalid); ++to) {
        switch (to) {
            case to_underlying_val(LogicalType::kBoolean): {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kBoolean), 0);
                break;
            }
            case to_underlying_val(LogicalType::kVarchar): {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kVarchar), 100);
                break;
            }
            default: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, static_cast<LogicalType>(to)), -1);
            }
        }
    }
}

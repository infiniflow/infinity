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
import new_catalog;
import parser;
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

class CastTableTest : public BaseTest {};

TEST_F(CastTableTest, casttable_boolean) {
    using namespace infinity;
    for (i8 to = LogicalType::kBoolean; to < LogicalType::kInvalid; ++to) {
        switch (to) {
            case LogicalType::kBoolean: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kBoolean), 0);
                break;
            }
            case LogicalType::kVarchar: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kVarchar), 1);
                break;
            }
            default: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, static_cast<LogicalType>(to)), -1);
            }
        }
    }
}

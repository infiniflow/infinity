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

module infinity_core:ut.column_def;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import column_def;
import data_type;

using namespace infinity;
class ColumnDefTest : public BaseTest {};

TEST_F(ColumnDefTest, test1) {
    using namespace infinity;

    std::set<ConstraintType> constraints;
    constraints.insert(ConstraintType::kUnique);
    constraints.insert(ConstraintType::kNotNull);
    auto column_def_ptr = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(DataType(LogicalType::kTinyInt)), "c1", constraints);
    EXPECT_EQ(*column_def_ptr->type(), DataType(LogicalType::kTinyInt));
    EXPECT_EQ(column_def_ptr->id(), 0);
    EXPECT_STREQ(column_def_ptr->name().c_str(), "c1");
    EXPECT_EQ(column_def_ptr->ToString(), "c1 TinyInt Not nullable Unique default Null");
}

TEST_F(ColumnDefTest, test2) {
    using namespace infinity;

    std::set<ConstraintType> constraints;
    constraints.insert(ConstraintType::kPrimaryKey);
    auto column_def_ptr = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "c2", constraints);
    EXPECT_EQ(*column_def_ptr->type(), DataType(LogicalType::kVarchar));
    EXPECT_EQ(column_def_ptr->id(), 3);
    EXPECT_EQ(column_def_ptr->name(), "c2");
    EXPECT_EQ(column_def_ptr->ToString(), "c2 Varchar PrimaryKey default Null");
}

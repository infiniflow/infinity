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

import stl;
import global_resource_usage;
import third_party;
import logger;

import table_def;
import infinity_context;
import internal_types;
import logical_type;
import column_def;
import data_type;

using namespace infinity;
class TableDefTest : public BaseTest {};

TEST_F(TableDefTest, test1) {
    using namespace infinity;

    Vector<SharedPtr<ColumnDef>> columns;

    i64 column_id = 0;
    {
        std::set<ConstraintType> constraints;
        constraints.insert(ConstraintType::kUnique);
        constraints.insert(ConstraintType::kNotNull);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "c1", constraints);
        columns.emplace_back(column_def_ptr);
    }
    {
        std::set<ConstraintType> constraints;
        constraints.insert(ConstraintType::kPrimaryKey);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kVarchar)), "c2", constraints);
        columns.emplace_back(column_def_ptr);
    }

    TableDef table_def(MakeShared<String>("default_db"), MakeShared<String>("t1"), MakeShared<String>("t1_comment"), columns);

    EXPECT_EQ(*table_def.table_name(), "t1");
    EXPECT_EQ(*table_def.schema_name(), "default_db");
    EXPECT_EQ(*table_def.table_comment(), "t1_comment");
    EXPECT_EQ(table_def.column_count(), 2u);
    EXPECT_EQ(table_def.GetColIdByName("c1"), 0u);
    EXPECT_EQ(table_def.GetColIdByName("c2"), 1u);
}

TEST_F(TableDefTest, ReadWrite) {
    using namespace infinity;

    Vector<SharedPtr<ColumnDef>> columns;

    i64 column_id = 0;
    {
        std::set<ConstraintType> constraints;
        constraints.insert(ConstraintType::kUnique);
        constraints.insert(ConstraintType::kNotNull);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "c1", constraints);
        columns.emplace_back(column_def_ptr);
    }
    {
        std::set<ConstraintType> constraints;
        constraints.insert(ConstraintType::kPrimaryKey);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(LogicalType::kVarchar), "c2", constraints);
        columns.emplace_back(column_def_ptr);
    }

    {
        TableDef table_def(MakeShared<String>("default_db"), MakeShared<String>("t1"), MakeShared<String>(), columns);

        int32_t exp_size = table_def.GetSizeInBytes();
        Vector<char> buf(exp_size, char(0));
        char *buf_beg = buf.data();
        char *ptr = buf_beg;
        table_def.WriteAdv(ptr);
        EXPECT_EQ(ptr - buf_beg, exp_size);

        const char *ptr_r = buf_beg;
        int32_t maxbytes = exp_size;
        SharedPtr<TableDef> table_def2 = table_def.ReadAdv(ptr_r, maxbytes);
        EXPECT_EQ(ptr_r - buf_beg, exp_size);
        EXPECT_NE(table_def2, nullptr);
        EXPECT_EQ(*table_def2, table_def);
    }

    {
        TableDef table_def(MakeShared<String>("default_db"), MakeShared<String>("t1"), MakeShared<String>("t1_comment"), columns);

        int32_t exp_size = table_def.GetSizeInBytes();
        Vector<char> buf(exp_size, char(0));
        char *buf_beg = buf.data();
        char *ptr = buf_beg;
        table_def.WriteAdv(ptr);
        EXPECT_EQ(ptr - buf_beg, exp_size);

        const char *ptr_r = buf_beg;
        int32_t maxbytes = exp_size;
        SharedPtr<TableDef> table_def2 = table_def.ReadAdv(ptr_r, maxbytes);
        EXPECT_EQ(ptr_r - buf_beg, exp_size);
        EXPECT_NE(table_def2, nullptr);
        EXPECT_EQ(*table_def2, table_def);
    }
}

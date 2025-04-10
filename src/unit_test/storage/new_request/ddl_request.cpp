// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
import stl;
import infinity_context;
import request_test;
import query_context;
import query_result;

using namespace infinity;

class TestDDLRequest : public NewRequestTest {
protected:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestDDLRequest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestDDLRequest, test_create_db) {
    String create_db_sql = "create database db1";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_drop_db) {
    {
        String create_db_sql = "create database db1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String drop_db_sql = "drop database db1";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_create_table) {
    String create_table_sql = "create table t1(c1 int, c2 varchar)";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    { // create table with same name
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_drop_table) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String drop_table_sql = "drop table t1";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    { // create table with same name
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_create_index) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String create_index_sql = "create index idx1 on t1 (c1)";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_drop_index) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String create_index_sql = "create index idx1 on t1 (c1)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String drop_index_sql = "drop index idx1 on t1";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_add_column) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String add_column_sql = "alter table t1 add column (c3 int default 0)";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(add_column_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(add_column_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_drop_column) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    String drop_column_sql = "alter table t1 drop column (c1)";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_column_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(drop_column_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDDLRequest, test_use_schema) {
    {
        String create_db_sql = "create database db1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String use_db_sql = "use db1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(use_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String use_db_sql = "use db2";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(use_db_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.explain_basic;

import :ut.base_test;
import :ut.request_test;
import :ut.sql_runner;
import :infinity_exception;
import :logger;
import :infinity_context;
import :status;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :data_block;
import :column_vector;
import :table_def;
import :config;
import :query_result;
import :query_context;
import :logical_node;
import :logical_planner;

import third_party;

import global_resource_usage;

using namespace infinity;
class ExplainBasicTest : public NewRequestTest {
public:
    void ExplainSql(std::string sql_statement, bool read_only) {
        std::vector<std::string> sql_vector;
        sql_vector.emplace_back(fmt::format("explain ast {}", sql_statement));
        sql_vector.emplace_back(fmt::format("explain raw {}", sql_statement));
        sql_vector.emplace_back(fmt::format("explain logical {}", sql_statement));
        sql_vector.emplace_back(fmt::format("explain physical {}", sql_statement));
        sql_vector.emplace_back(fmt::format("explain fragment {}", sql_statement));
        sql_vector.emplace_back(fmt::format("explain {}", sql_statement));

        if (read_only) {
            sql_vector.emplace_back(fmt::format("explain analyze {}", sql_statement));
            sql_vector.emplace_back(fmt::format("explain pipeline {}", sql_statement));
        }

        for (const auto &sql : sql_vector) {
            LOG_INFO(fmt::format("sql: {}", sql));
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(sql);
            bool ok = HandleQueryResult(query_result);

            ASSERT_TRUE(ok);
            LOG_INFO(fmt::format("explain: {}", query_result.ToString()));
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, ExplainBasicTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(ExplainBasicTest, test1) {
    // Create database
    {
        std::string sql = "create database db1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        std::string sql = "create collection collection1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    // Create table
    {
        std::string create_table_sql = "create table tb(col1 int, col2 varchar)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    for (size_t i = 0; i < 200; i++) {
        std::string sql = fmt::format("insert into tb values({}, 'abc')", i);
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string create_table_sql = "create table tb_embedding(col1 int, col2 embedding(float, 4))";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into tb_embedding values(1, [0.1, 0.1, 0.1, 0.1]), (2, [0.2, 0.2, 0.2, 0.2])";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    // Explain select
    ExplainSql("select * from tb where col1 > 100 and 1 = 1", true);
    ExplainSql("select * from tb order by col1", true);
    ExplainSql("select * from tb limit 100", true);
    ExplainSql("select min(col1), max(col1), avg(col1) from tb where 1 = 1 group by col1", true);
    ExplainSql("select * from tb where col1 > 100 and 1 = 1 order by col1 limit 100", true);
    ExplainSql("select * from tb where col1 in (100, 150, 200)", true);
    ExplainSql("select * from tb where col1 between 100 and 200", true);
    ExplainSql("select col1 from tb_embedding search match vector (col2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)", true);

    // Explain create
    ExplainSql("create index idx on tb(col1) using secondary", false);
    ExplainSql("create index idx on tb(col2) using fulltext", false);
    ExplainSql("create table tb1(c1 int, c2 varchar)", false);
    ExplainSql("create database db1", false);
    ExplainSql("create collection c1", false);

    // Explain insert
    ExplainSql("insert into tb values(1000, 'xyz')", false);

    // Explain update
    ExplainSql("update tb set col1 = 999 where col1 > 100", false);
    ExplainSql("update tb set col1 = 999 where col1 between 100 and 200", false);

    // Explain drop
    ExplainSql("drop table tb", false);
    ExplainSql("drop database db1", false);
    ExplainSql("drop collection collection1", false);

    // Explain show
    ExplainSql("show databases", true);
    ExplainSql("show tables", true);
    ExplainSql("show database default_db", true);
    ExplainSql("show table tb", true);
    ExplainSql("show table tb columns", true);
    ExplainSql("show table tb segments", true);
    ExplainSql("show table tb segment 0", true);
    ExplainSql("show table tb segment 0 blocks", true);
    ExplainSql("show table tb segment 0 block 0", true);
    ExplainSql("show table tb segment 0 block 0 column 0", true);
    ExplainSql("show table tb indexes", true);
    ExplainSql("show checkpoint", true);
    // ExplainSql("show buffer", true); // deprecated
    ExplainSql("show tasks", true);
    ExplainSql("show configs", true);
    ExplainSql("show config version", true);
    ExplainSql("show profiles", true);
    // ExplainSql("show memindex", true);
    ExplainSql("show queries", true);
    ExplainSql("show transactions", true);
    ExplainSql("show transaction history", true);
    ExplainSql("show session variables", true);
    ExplainSql("show session variable query_count", true);
    ExplainSql("show snapshots", true);

    // Explain flush
    // ExplainSql("flush data", true);
    // ExplainSql("flush log", true);
    // ExplainSql("flush buffer", true);
    ExplainSql("flush catalog", true);
}
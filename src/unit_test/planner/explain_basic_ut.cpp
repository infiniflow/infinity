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

#include "base_statement.h"
#include "parser.h"
#include "unit_test/gtest_expand.h"

module infinity_core:ut.explain_basic;

import :ut.base_test;
import :ut.sql_runner;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :ut.request_test;
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

import global_resource_usage;

using namespace infinity;
class ExplainBasicTest : public NewRequestTest {
public:
    std::shared_ptr<std::string> db_name;
    std::shared_ptr<ColumnDef> column_def1;
    std::shared_ptr<ColumnDef> column_def2;
    std::shared_ptr<std::string> table_name;
    std::shared_ptr<TableDef> table_def;

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
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(sql);
            bool ok = HandleQueryResult(query_result);
            ASSERT_TRUE(ok);

            LOG_INFO(fmt::format("sql: {}", sql));
            LOG_INFO(fmt::format("explain: {}", query_result.ToString()));
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, ExplainBasicTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(ExplainBasicTest, test1) {
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    db_name = std::make_shared<std::string>("default_db");
    column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    table_name = std::make_shared<std::string>("tb");
    table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

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
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (size_t i = 0; i < 200; i++) {
        std::string sql = fmt::format("insert into {} values({}, 'abc')", *table_name, i);
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    // Explain select
    ExplainSql("select * from tb where col1 > 100 and 1 = 1", true);
    ExplainSql("select * from tb order by col1", true);
    ExplainSql("select * from tb limit 100", true);
    ExplainSql("select * from tb where col1 > 100 and 1 = 1 order by col1 limit 100", true);

    // Explain create
    ExplainSql("create table tb1(c1 int, c2 varchar)", false);
    ExplainSql("create database db1", false);
    ExplainSql("create collection c1", false);

    // Explain insert
    ExplainSql("insert into tb values(1000, 'xyz')", false);

    // Explain drop
    ExplainSql("drop table tb", false);
    ExplainSql("drop database db1", false);
    ExplainSql("drop collection collection1", false);

    // Explain show
    ExplainSql("show database default_db", true);
    ExplainSql("show table tb", true);
    ExplainSql("show checkpoint", true);
}
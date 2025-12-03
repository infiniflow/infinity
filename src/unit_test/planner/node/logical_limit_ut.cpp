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

module infinity_core:ut.logical_limit;

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
class LogicalLimitTest : public NewRequestTest {
public:
    std::shared_ptr<std::string> db_name;
    std::shared_ptr<ColumnDef> column_def1;
    std::shared_ptr<ColumnDef> column_def2;
    std::shared_ptr<std::string> table_name;
    std::shared_ptr<TableDef> table_def;

    void CheckLogicalNode(const std::shared_ptr<LogicalNode> &node, LogicalNodeType type) {
        if (!node) {
            return;
        }

        if (node->operator_type() == type) {
            i64 space = 4;
            LOG_INFO(fmt::format("ToString: {}", node->ToString(space)));

            [[maybe_unused]] auto column_bindings = node->GetColumnBindings();
            [[maybe_unused]] auto output_names = node->GetOutputNames();
            [[maybe_unused]] auto output_types = node->GetOutputTypes();
        }

        CheckLogicalNode(node->left_node(), type);
        CheckLogicalNode(node->right_node(), type);
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, LogicalLimitTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(LogicalLimitTest, test1) {
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    db_name = std::make_shared<std::string>("default_db");
    column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    table_name = std::make_shared<std::string>("tb");
    table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    // Create table
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (size_t i = 0; i < 200; i++) {
        std::string sql = fmt::format("insert into tb values({}, 'abc')", i);
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        std::string sql = "select col1 from tb limit 100";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);

        auto nodes = query_context->logical_planner()->LogicalPlans();
        for (const auto &node : nodes) {
            CheckLogicalNode(node, LogicalNodeType::kLimit);
        }

        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        std::string sql = "select col1 from tb order by col1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);

        auto nodes = query_context->logical_planner()->LogicalPlans();
        for (const auto &node : nodes) {
            CheckLogicalNode(node, LogicalNodeType::kSort);
        }

        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        std::string sql = "select col1 from tb order by col1 limit 100";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);

        auto nodes = query_context->logical_planner()->LogicalPlans();
        for (const auto &node : nodes) {
            CheckLogicalNode(node, LogicalNodeType::kTop);
        }

        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
}
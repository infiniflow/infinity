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

module infinity_core:ut.logical_match_scan_base;

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
class LogicalMatchScanTest : public NewRequestTest {
public:
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

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, LogicalMatchScanTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(LogicalMatchScanTest, test1) {
    // Create table
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

    // {
    //     std::string sql = "select col1 from tb_embedding search match vector (col2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //
    //     auto nodes = query_context->logical_planner()->LogicalPlans();
    //     for (const auto &node : nodes) {
    //         CheckLogicalNode(node, LogicalNodeType::kKnnScan);
    //     }
    //
    //     bool ok = HandleQueryResult(query_result);
    //     EXPECT_TRUE(ok);
    // }
}
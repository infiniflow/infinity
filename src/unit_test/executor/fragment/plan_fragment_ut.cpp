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

module infinity_core:ut.plan_fragment;

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
class PlanFragmentTest : public NewRequestTest {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, PlanFragmentTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(PlanFragmentTest, test1) {
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

    std::string sql = "select * from tb where col1 < 100";
    auto [physical_plan, plan_fragment] = SQLRunner::GetPhysicalPlan(sql);
    auto plans = plan_fragment->ToString();
    for (const auto &plan : *plans) {
        LOG_INFO(fmt::format("Plan fragment: {}", plan));
    }
}
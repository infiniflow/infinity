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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.logical_insert;

import :ut.base_test;
import :ut.sql_runner;
import :logical_insert;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :column_binding;
import :ut.request_test;
import :status;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :snapshot_info;
import :data_block;
import :column_vector;
import :table_def;
import :value;
import :config;
import :persistence_manager;
import :logger;
import :query_result;
import :query_context;

import global_resource_usage;

using namespace infinity;
class LogicalInsertTest : public NewRequestTest {
public:
    std::shared_ptr<std::string> db_name;
    std::shared_ptr<ColumnDef> column_def1;
    std::shared_ptr<ColumnDef> column_def2;
    std::shared_ptr<std::string> table_name;
    std::shared_ptr<TableDef> table_def;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, LogicalInsertTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(LogicalInsertTest, test1) {
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    db_name = std::make_shared<std::string>("default_db");
    column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    table_name = std::make_shared<std::string>("tb1");
    table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    // Create table
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("read"), TransactionType::kRead);
        auto [table_info, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        auto insert_node = std::make_shared<LogicalInsert>(0, table_info, 0, std::vector<std::vector<std::shared_ptr<BaseExpression>>>{});

        i64 space = 4;
        LOG_INFO(fmt::format("ToString: {}", insert_node->ToString(space)));

        [[maybe_unused]] auto column_bindings = insert_node->GetColumnBindings();
        [[maybe_unused]] auto output_names = insert_node->GetOutputNames();
        [[maybe_unused]] auto output_types = insert_node->GetOutputTypes();

        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
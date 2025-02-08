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
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import infinity_exception;

using namespace infinity;

class NewCatalogTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewCatalogTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(NewCatalogTest, db_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    {
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());
        std::cout << status.message() << std::endl;

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // get db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}
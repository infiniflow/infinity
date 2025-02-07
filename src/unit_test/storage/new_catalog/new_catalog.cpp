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
    //    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();

    {
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);

        Status status = txn1->CreateDatabase(MakeShared<String>("db1"), ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = txn2->CreateDatabase(MakeShared<String>("db1"), ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        //        Status status = new_catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1);
        //        EXPECT_TRUE(status.ok());
        //        bool db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn1);
        //        EXPECT_TRUE(db1_exist);
        //        db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn2);
        //        EXPECT_FALSE(db1_exist);
        //
        //        status = new_catalog->DropDatabase(MakeShared<String>("db1"), txn1);
        //        EXPECT_TRUE(status.ok());
        //        db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn1);
        //        EXPECT_FALSE(db1_exist);
        //        db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn2);
        //        EXPECT_FALSE(db1_exist);

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());
        std::cout << status.message() << std::endl;
    }

    {
        //        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        //        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        //
        //        Status status = new_catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1);
        //        EXPECT_TRUE(status.ok());
        //        bool db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn1);
        //        EXPECT_TRUE(db1_exist);
        //        db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn2);
        //        EXPECT_FALSE(db1_exist);
        //
        //        new_txn_mgr->CommitTxn(txn1);
        //        db1_exist = new_catalog->CheckDatabaseExists(MakeShared<String>("db1"), txn2);
        //        EXPECT_FALSE(db1_exist);
        //        status = new_catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn2);
        //        EXPECT_FALSE(status.ok());
        //        new_txn_mgr->RollBackTxn(txn2);
    }
}
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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.database;

import :ut.base_test;
import :stl;
import third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :data_block;
import :column_vector;
import :value;
import :new_txn;
import :db_meeta;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#endif

import extra_ddl_info;

using namespace infinity;

class TestTxnDatabase : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnDatabase,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnDatabase, DISABLED_createdb_createdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Create db conflict

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |----------------------|
        //       t2                  create(fail)

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |----------|
        //       t2       create (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1            create (fail)
        //    |---------------|
        //       |----------|----------|
        //       t2       create      commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1      create    commit (success)
        //          |-------|---------|
        //       |----------------------|
        //       t2                  create (fail)

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //               t1      create      commit (success)
        //                |----------|---------|
        //       |----------------------|
        //       t2                  create (fail)
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //               t1               create (fail)
        //                |-----------------|
        //       |----------------------|-----------------|
        //       t2                  create           commit (success)

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status.ok());
        EXPECT_STREQ(db_info->db_name_->c_str(), "db1");
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<String>("check db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnDatabase, db_test2) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    {
        // drop with error
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        Status status = txn1->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop with ignore
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnDatabase, db_test3) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create duplicated db1 with error
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn2->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create duplicated db1 with ignore
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn3->CreateDatabase("db1", ConflictType::kIgnore, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnDatabase, dropdb_dropdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    // Drop db conflict
    {
        //    t1      drop db     commit (success)
        //    |----------|---------|
        //       |-------------------|----------| (fail)
        //       t2               drop db     commit
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    {
        //    t1      drop db     commit (success)
        //    |----------|---------|
        //       |-------------|----------| (fail)
        //       t2         drop db     commit
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    {
        //    t1              drop db   commit (success)
        //    |------------------|-------|
        //       |-------------|----------|
        //       t2         drop db     commit (fail)
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    {
        //          t1      drop db  commit (success)
        //          |--------|---------|
        //       |-----------------------|-----------------------|
        //       t2                 drop db               commit (fail)
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kError);
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        EXPECT_TRUE(status.ok());
        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    {
        //          t1      drop db  commit (success)
        //          |--------|---------|
        //       |-------------|----------| (fail)
        //       t2         drop db     commit
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    {
        //          t1         drop db    commit (success)
        //          |--------------|-------|
        //       |-------------|---------------| (fail)
        //       t2         drop db         commit
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnDatabase, createdb_dropdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |-------------------|----------|
        //       t2               drop(fail)  rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |----------|----------|
        //       t2       drop(fail)  rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1                        create      commit (success)
        //    |--------------------------|---------|
        //       |---------------------|--------------|
        //       t2                drop(fail)     rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1      create      commit (success)
        //           |----------|---------|
        //       |---------------------------|----------|
        //       t2                     drop(fail)  rollback
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1      create      commit (success)
        //           |----------|---------|
        //       |---------------------|----------|
        //       t2                drop(fail)  rollback
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1      create      commit (success)
        //           |----------|---------|
        //       |---------|-----------------------|
        //       t2     drop(fail)            rollback
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //  t1                         create                commit (success)
        //  |----------------------------|-----------------------|
        //       |---------|----------|
        //       t2     drop(fail) rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //  t1                create                commit (success)
        //  |-------------------|-----------------------|
        //       |---------|-----------------------|
        //       t2     drop(fail)            rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //  t1                create                commit (success)
        //  |-------------------|-----------------------|
        //       |------------------|---------------|
        //       t2           drop(fail)        rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

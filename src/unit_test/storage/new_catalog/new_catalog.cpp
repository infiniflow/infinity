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
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_full_text;
import index_hnsw;
import embedding_info;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import data_access_state;

using namespace infinity;

class NewCatalogTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewCatalogTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(NewCatalogTest, createdb_createdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Create db conflict

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |----------------------|----------|
        //       t2                  create      commit (fail)

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |----------|----------|
        //       t2       create      commit (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());
        std::cout << status.message() << std::endl;

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1            create  commit (success)
        //    |---------------|------|
        //       |----------|----------|
        //       t2       create      commit (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);

        Status status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());
        std::cout << status.message() << std::endl;

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1      create    commit (success)
        //          |-------|---------|
        //       |----------------------|----------|
        //       t2                  create      commit (fail)

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //               t1      create      commit (success)
        //                |----------|---------|
        //       |----------------------|----------|
        //       t2                  create      commit (fail)

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //               t1               create      commit (success)
        //                |-----------------|----------|
        //       |----------------------|-----------------|
        //       t2                  create           commit (fail)

        // start txn2
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kRead);
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Conflict, can't commit
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // Check if db1 exists
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

        // Check if db1 exists
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info2, db_status1] = txn5->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_info2 == nullptr);
        EXPECT_FALSE(db_status1.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, db_test2) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    {
        // drop with error
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn1->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop with ignore
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, db_test3) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create duplicated db1 with error
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create duplicated db1 with ignore
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn3->CreateDatabase("db1", ConflictType::kIgnore, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, db_test4) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = txn1->CreateDatabase("db2", ConflictType::kError, MakeShared<String>("db_comment"));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        auto [db_info1, db_status1] = txn3->GetDatabaseInfo("db1");
        EXPECT_TRUE(db_status1.ok());
        std::cout << *db_info1->db_entry_dir_ << std::endl;
        EXPECT_EQ(db_info1->db_comment_, nullptr);

        auto [db_info2, db_status2] = txn3->GetDatabaseInfo("db2");
        EXPECT_TRUE(db_status2.ok());
        std::cout << *db_info2->db_entry_dir_ << std::endl;
        EXPECT_STREQ(db_info2->db_comment_->c_str(), "db_comment");

        auto db_list = txn3->ListDatabase();
        EXPECT_EQ(db_list.size(), 2);
        for (auto &db : db_list) {
            std::cout << db << std::endl;
        }

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn4->DropDatabase("db2", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, dropdb_dropdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    // Drop db conflict
    {
        //    t1      drop db     commit (success)
        //    |----------|---------|
        //       |-------------------|----------| (fail)
        //       t2               drop db     commit
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db2
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

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

TEST_P(NewCatalogTest, createdb_dropdb_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |-------------------|----------|
        //       t2               drop(fail)  rollback
        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // start txn1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // drop db1
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase("db1", ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, table_test1) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // - list tables
        Vector<String> tables;
        status = txn3->ListTable(*db_name, tables);
        EXPECT_TRUE(status.ok());

        for (const auto &table_name : tables) {
            std::cout << String("Table name: ") << table_name << std::endl;
            auto [table_info, table_status] = txn3->GetTableInfo(*db_name, table_name);
            std::cout << *table_info->table_name_ << std::endl;
        }

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, table_test2) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create table tb2
        auto table_def2 = TableDef::Make(db_name, std::make_shared<std::string>("tb2"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn3->CreateTable(*db_name, std::move(table_def2), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create table tb3
        auto table_def3 = TableDef::Make(db_name, std::make_shared<std::string>("tb3"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn4->CreateTable(*db_name, std::move(table_def3), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, createtable_createtable) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    {
        //   t1    create table    commit (success)
        //   |--------|-------------|
        //       |-----------------------|------------------------|
        //       t2                  create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //   t1         create table    commit (success)
        //   |---------------|------------------|
        //       |-------------|------------------------|
        //       t2         create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //     t1                create table    commit (success)
        //     |------------------|------------------|
        //       |-------------|------------------------|
        //       t2         create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1     create table    commit (success)
        //          |--------|------------------|
        //       |--------------------------------|------------------------|
        //       t2                        create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1     create table    commit (success)
        //          |--------|------------------|
        //       |-------------|------------------------|
        //       t2         create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1        create table    commit (success)
        //          |-------------|------------------|
        //       |-------------|------------------------|
        //       t2         create table             commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>("tb1"), MakeShared<String>(), {column_def1, column_def2});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn3->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, createdb_createtable_test) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        //    t1      create db    commit (success)
        //    |----------|-----------|
        //       |---------------------|------------------|
        //       t2                create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create db        commit (success)
        //    |----------|-----------------|
        //       |---------------------|------------------|
        //       t2                create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1              create db  commit (success)
        //    |--------------------|------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create db                  commit (success)
        //    |----------|-------------------------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1                create db                  commit (success)
        //    |--------------------|-------------------------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1                                  create db                  commit (success)
        //    |---------------------------------------|-------------------------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1                create db              commit (success)
        //           |--------------------|------------------------|
        //       |---------------------------|------------------|
        //       t2                    create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1                create db              commit (success)
        //           |--------------------|----------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1      create db        commit (success)
        //           |----------|----------------|
        //       |-----------------------------------|------------------|
        //       t2                            create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1                create db        commit (success)
        //           |--------------------|----------------|
        //       |---------------------------|------------------|
        //       t2                    create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //           t1                create db        commit (success)
        //           |--------------------|----------------|
        //       |---------------------|------------------------|
        //       t2                    create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, createtable_dropdb_test) {
    using namespace infinity;
    // Conflict between drop db and create table
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->PrintAllKeyValue();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        //    t1      create table                  commit (success)
        //    |----------|-----------------------------|
        //         |--------------|-------------------------|
        //        t2           drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1                    create table     commit (success)
        //    |-------------------------|----------------|
        //         |--------------|-------------------------|
        //        t2           drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create table  commit (success)
        //    |----------|------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create table                  commit (success)
        //    |----------|-----------------------------|
        //                    |--------------|------------------|
        //                   t2           drop db         commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create table                           commit (success)
        //    |----------|--------------------------------------------|
        //                    |--------------|------------------|
        //                   t2           drop db         commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1                           create table                           commit (success)
        //    |---------------------------------|--------------------------------------------|
        //                    |--------------|------------------|
        //                   t2           drop db         commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }

    {
        //              t1      create table  commit (success)
        //              |----------|------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //              t1           create table  commit (success)
        //              |-----------------|------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //              t1           create table           commit (success)
        //              |-----------------|---------------------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }

    {
        //              t1       create table                commit (success)
        //              |------------|--------------------------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }

    {
        //              t1                                      create table           commit (success)
        //              |-------------------------------------------|---------------------------|
        //         |--------------------|-------------------------|
        //        t2                    drop db              commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, dropdb_droptable_test) {
    using namespace infinity;

    // Conflict between drop db and drop table
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->PrintAllKeyValue();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        //    t1      drop table                  commit (success)
        //    |----------|-----------------------------|
        //                    |--------------|------------------|
        //                   t2           drop db         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      drop table                  commit (success)
        //    |----------|-----------------------------|
        //             |--------------|------------------|
        //            t2           drop db         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //                   t1      drop table       commit (success)
        //                   |----------|--------------|
        //             |--------------|------------------|
        //            t2           drop db         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //                   t1      drop table       commit (success)
        //                   |----------|-------------------|
        //             |--------------|------------------|
        //            t2           drop db         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        //                                 t1      drop table       commit (success)
        //                                 |----------|-------------------|
        //             |--------------|------------------|
        //            t2           drop db         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, createtable_droptable_test) {
    using namespace infinity;

    // Conflict between drop db and drop table
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->PrintAllKeyValue();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        //    t1      create table  commit (success)
        //    |----------|-----------|
        //                                 |--------------|------------------|
        //                                t2           drop table         commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      create      commit (success)
        //    |----------|---------|
        //       |-------------------|----------|
        //       t2               drop(fail)  rollback
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, droptable_droptable_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->PrintAllKeyValue();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    // Drop table conflict
    {
        //    t1      drop     commit (success)
        //    |----------|---------|
        //       |-------------------|----------|
        //       t2               drop     commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      drop      commit (success)
        //    |----------|---------|
        //       |-------------|----------| (fail)
        //       t2         drop      commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1              drop   commit (success)
        //    |------------------|-------|
        //       |-------------|----------|
        //       t2          drop     commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1      drop  commit (success)
        //          |--------|---------|
        //       |-----------------------|-----------------------|
        //       t2                 drop                  commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1      drop    commit (success)
        //          |--------|---------|
        //       |-------------|----------|
        //       t2         drop       commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //          t1         drop       commit (success)
        //          |--------------|-------|
        //       |-------------|---------------|
        //       t2         drop             commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // drop table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, index_test1) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name = std::make_shared<String>("idx1");
    auto index_def = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // get index def
        auto *txn3_1 = new_txn_mgr->BeginTxn(MakeUnique<String>("get index"), TransactionType::kNormal);
        SharedPtr<IndexBase> index_def1;
        Status status = txn3_1->GetIndexDefByName(*db_name, *table_name, *index_name, index_def1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3_1);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_def1, *index_def);
    }
    {
        // list and drop index
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Vector<String> indexes;
        Status status = txn4->ListIndex(*db_name, *table_name, indexes);
        EXPECT_TRUE(status.ok());

        for (const auto &index_name : indexes) {
            std::cout << String("Index name: ") << index_name << std::endl;
        }

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewCatalogTest, index_test2) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
    {
        // create fulltext index idx2
        auto index_name2 = std::make_shared<String>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    {
        // create hnsw index idx3
        auto index_name3 = std::make_shared<String>("idx3");
        Vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, MakeShared<String>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewCatalogTest, index_segment_test) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name = std::make_shared<String>("idx1");
    auto index_def = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});
    SegmentID idx_segment_id = 0;
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add index segment"), TransactionType::kNormal);
        Status status = txn4->AddIndexSegment(*db_name, *table_name, *index_name, idx_segment_id);
        EXPECT_TRUE(status.ok());

        {
            ChunkIndexInfo chunk_info;
            chunk_info.base_name_ = "chunk1";
            chunk_info.paths_.emplace_back("file1");
            chunk_info.base_rowid_ = RowID(idx_segment_id, 0);
            chunk_info.row_count_ = 10000;
            status = txn4->AddIndexChunk(*db_name, *table_name, *index_name, idx_segment_id, chunk_info);
        }
        {
            ChunkIndexInfo chunk_info;
            chunk_info.base_name_ = "chunk2";
            chunk_info.paths_.emplace_back("file2");
            chunk_info.base_rowid_ = RowID(idx_segment_id, 10000);
            chunk_info.row_count_ = 10000;
            status = txn4->AddIndexChunk(*db_name, *table_name, *index_name, idx_segment_id, chunk_info);
        }
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get index chunk"), TransactionType::kNormal);
        Vector<ChunkIndexInfo> chunk_infos;
        Status status = txn5->GetIndexChunks(*db_name, *table_name, *index_name, idx_segment_id, chunk_infos);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(chunk_infos.size(), 2);
        for (const auto &chunk_info : chunk_infos) {
            std::cout << "chunk name: " << chunk_info.base_name_ << ", row count: " << chunk_info.row_count_ << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact index"), TransactionType::kNormal);
        {
            ChunkIndexInfo chunk_info;
            chunk_info.base_name_ = "chunk3";
            chunk_info.paths_.emplace_back("file3");
            chunk_info.base_rowid_ = RowID(idx_segment_id, 0);
            chunk_info.row_count_ = 20000;
            Status status = txn6->AddIndexChunk(*db_name, *table_name, *index_name, idx_segment_id, chunk_info);
            EXPECT_TRUE(status.ok());
        }
        {
            Vector<ChunkID> chunk_ids{0, 1};
            Status status = txn6->DeprecateIndexChunk(*db_name, *table_name, *index_name, idx_segment_id, chunk_ids);
            EXPECT_TRUE(status.ok());
        }
        Status status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get index chunk2"), TransactionType::kNormal);
        Vector<ChunkIndexInfo> chunk_infos;
        Status status = txn7->GetIndexChunks(*db_name, *table_name, *index_name, idx_segment_id, chunk_infos);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(chunk_infos.size(), 1);
        for (const auto &chunk_info : chunk_infos) {
            std::cout << "chunk name: " << chunk_info.base_name_ << ", row count: " << chunk_info.row_count_ << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
}

TEST_P(NewCatalogTest, lock_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // another lock table, conflicts
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop db
        //                         t1            lock table (fail)  commit
        //                         |--------------|-------------------|
        //       |------|-------|
        //       t2    drop   commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // lock table (fail)
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop db
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |------|-------|
        //       t2    drop   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //                          t1            lock table    commit (success)
        //                          |--------------|---------------|
        //       |------|-------|
        //       t2    drop   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |------|-------|
        //       t2    drop   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |-------------|-------|
        //       t2          drop   commit (failed)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |------------------|-------|
        //       t2               drop   commit (failed)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn10);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, alter_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, test_append) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        auto input_block = MakeShared<DataBlock>();
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

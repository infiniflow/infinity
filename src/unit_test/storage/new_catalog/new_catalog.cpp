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
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import data_access_state;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import constant_expr;

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
        EXPECT_EQ(*db_info1->db_comment_, "");

        auto [db_info2, db_status2] = txn3->GetDatabaseInfo("db2");
        EXPECT_TRUE(db_status2.ok());
        std::cout << *db_info2->db_entry_dir_ << std::endl;
        EXPECT_STREQ(db_info2->db_comment_->c_str(), "db_comment");

        Vector<String> db_names;
        status = txn3->ListDatabase(db_names);
        EXPECT_EQ(db_names.size(), 2);
        for (auto &db : db_names) {
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
        Vector<String> table_names;
        status = txn3->ListTable(*db_name, table_names);
        EXPECT_TRUE(status.ok());

        for (const auto &table_name : table_names) {
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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
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
        //    t1                create db                  commit (success)
        //    |--------------------|-------------------------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
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
        //    t1                                  create db                  commit (success)
        //    |---------------------------------------|-------------------------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
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
        //           |--------------------|----------------|
        //       |--------------|------------------|
        //       t2          create table      commit (fail)
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

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

TEST_P(NewCatalogTest, rename_table_test) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->PrintAllKeyValue();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto new_table_name = "table2";
    // Success rename
    {
        //    t1      create  commit (success)
        //    |----------|------------|
        //                              |-------------------|----------|
        //                             t2               rename     commit (success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn4->DropTable(*db_name, new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      rename  commit (success)
        //    |----------|------------|
        //                              |------------|------------------|
        //                             t2           rename (fail)    rollback (success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename table again
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn4->DropTable(*db_name, new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      rename  commit (success)
        //    |----------|------------|
        //                    |----------------|------------------|
        //                    t2           rename           commit (fail)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        // rename table again
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn4->DropTable(*db_name, new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      rename  commit (success)
        //    |----------|------------|
        //            |-------|------------------|
        //           t2     rename           commit (fail)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // rename table again
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn4->DropTable(*db_name, new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        //    t1      rename            commit (fail)
        //    |----------|--------------------|
        //            |-------|-----------|
        //           t2     rename      commit (success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // rename table again
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn4->DropTable(*db_name, new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with drop db
    {
        //    t1      rename            commit (success)
        //    |----------|--------------------|
        //            |-------|-----------|
        //           t2     drop      commit (success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // drop db1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    // with drop table 1
    {
        //    t1      rename            commit (fail)
        //    |----------|--------------------|
        //            |-------|-----------|
        //           t2     drop      commit (success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with drop table 2
    {
        //    t1      rename            commit (success)
        //    |----------|--------------------|
        //            |-------|-----------------|
        //           t2     drop         commit (fail)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        // drop table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn4->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with add columns 1
    {
        //    t1      rename            commit (success)
        //    |----------|--------------------|
        //            |-------|-----------------|
        //           t2     add columns (fail) rollback
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with add columns 2
    {
        //    t1                rename (fail)       rollback
        //    |-------------------|--------------------|
        //            |-------|-----------------|
        //           t2     add columns    commit(success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with drop columns 1
    {
        //    t1                rename (fail)       rollback
        //    |-------------------|--------------------|
        //            |-------|-----------------|
        //           t2     drop columns    commit(success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with drop columns 2
    {
        //    t1          rename (success)                     commit
        //    |-------------|------------------------------------|
        //            |-------|---------------------------|
        //           t2     drop columns (fail)       commit(success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with lock table 1
    {
        //    t1          rename (success)                     commit
        //    |-------------|------------------------------------|
        //            |-------|---------------------------|
        //           t2     lock table (fail)         rollback
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn4->LockTable(*db_name, *table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // with lock table 2
    {
        //    t1                rename (fail)                        rollback
        //    |--------------------|------------------------------------|
        //            |-------|---------------------------|
        //           t2     lock table           commit(success)
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

        // rename table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn4->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop db1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
    auto index_base = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});

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
        Status status = txn3->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // get index def
        auto *txn3_1 = new_txn_mgr->BeginTxn(MakeUnique<String>("get index"), TransactionType::kNormal);
        SharedPtr<IndexBase> index_def1;

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> index_meta;
        Status status = txn3_1->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, index_meta);
        EXPECT_TRUE(status.ok());

        std::tie(index_def1, status) = index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_def1, *index_base);
    }
    {
        // list and drop index
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        Vector<String> index_names;
        Status status = txn4->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());

        for (const auto &index_name : index_names) {
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
        // lock table and unlock table
        //      t1            lock table        commit
        //      |--------------|-------------------|
        //           |------------|-----------------------|
        //           t2         unlock (fail)            rollback
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

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
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

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
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

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
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

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
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
        //          t1            lock table (failed) rollback
        //          |--------------|-------------------|
        //       |-------------|-------|
        //       t2          drop   commit (success)
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
        //       |------------------|----------|
        //       t2               drop(fail)  rollback
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

TEST_P(NewCatalogTest, lock_table_and_lock_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // lock table and lock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //                         |------|--------------------|
        //                        t2    lock table(fail)   rollback (success)
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

        // lock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and unlock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //              |------|-----------------------|
        //              t2    unlock table (fail)   rollback (success)
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

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and unlock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //              |------|-----------------------|
        //              t2    unlock table (fail)   rollback (success)
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

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
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
        // lock table and drop table
        //  t1            drop table    commit (success)
        //  |--------------|---------------|
        //                                  |------|------------|
        //                                 t2    add column   commit (success)
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
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, add_column_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // lock table and drop table
        //  t1            add column     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop      commit
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

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            add column     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //         t2                drop      commit
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
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            add column     commit (success)
        //  |--------------|---------------|
        //         |----|----------|
        //         t2  drop      commit
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
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, add_column_and_drop_table) {
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            add column    commit (success)
        //  |--------------|---------------|
        //                            |------|------------|
        //                            t2    drop table  commit (success)
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            add column        commit (success)
        //  |--------------|---------------------|
        //                            |------|-----------------|
        //                            t2    drop table (fail) rollback
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            add column(fail)      commit (success)
        //  |--------------|---------------------|
        //       |------|---------------------------|
        //      t2    drop table              commit
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //                      t1         add column(fail)      commit (success)
        //                      |--------------|---------------------|
        //       |------|---------------------------|
        //      t2    drop table              commit
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //                      t1         add column(fail)      commit (success)
        //                      |--------------|---------------------|
        //       |------|------------------|
        //      t2    drop table        commit
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, add_column_and_add_column) {
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
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and add column
        //    t1         add column          commit (success)
        //    |--------------|---------------------|
        //                                   |------|------------------|
        //                                  t2    add column        commit (success)
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
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and add column
        //    t1         add column      commit (success)
        //    |--------------|---------------------|
        //                       |------|------------------|
        //                      t2    add column (fail)  commit (success)
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
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and add column
        //    t1         add column(fail)      rollback (success)
        //    |--------------|---------------------|
        //         |------|------------------|
        //        t2    add column        commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, lock_table_and_add_column) {
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

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, lock_table_and_drop_column) {
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

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, drop_column_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // lock table and drop table
        //  t1            drop column     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop      commit
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

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            drop column     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //         t2                drop      commit
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

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            drop column     commit (success)
        //  |--------------|---------------|
        //         |----|----------|
        //         t2  drop      commit
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

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, drop_column_and_drop_table) {
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
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
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            drop column    commit (success)
        //  |--------------|---------------|
        //                            |------|------------|
        //                            t2    drop table  commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            drop column        commit (success)
        //  |--------------|---------------------|
        //                            |------|-----------------|
        //                            t2    drop table (fail) rollback
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //  t1            drop column(fail)      commit (success)
        //  |--------------|---------------------|
        //       |------|---------------------------|
        //      t2    drop table              commit
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //                      t1         drop column(fail)      commit (success)
        //                      |--------------|---------------------|
        //       |------|---------------------------|
        //      t2    drop table              commit
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn3->AddColumns(*db_name, table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // lock table and drop table
        //                      t1         drop column(fail)      commit (success)
        //                      |--------------|---------------------|
        //       |------|------------------|
        //      t2    drop table        commit
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

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn5->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, drop_column_and_add_column) {
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

        // drop column
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn3->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn5->GetTableInfo(*db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        EXPECT_EQ(table_info->column_defs_.size(), 2);
        EXPECT_STREQ(table_info->column_defs_[0]->name_.c_str(), "col1");
        EXPECT_STREQ(table_info->column_defs_[1]->name_.c_str(), "col4");
        status = new_txn_mgr->CommitTxn(txn5);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop column
        //    t1         add column          commit (success)
        //    |--------------|---------------------|
        //                                   |------|------------------|
        //                                  t2    drop column        commit (success)
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
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop column
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn5->GetTableInfo(*db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        EXPECT_EQ(table_info->column_defs_.size(), 2);
        EXPECT_STREQ(table_info->column_defs_[0]->name_.c_str(), "col1");
        EXPECT_STREQ(table_info->column_defs_[1]->name_.c_str(), "col3");
        status = new_txn_mgr->CommitTxn(txn5);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop column
        //    t1         add column      commit (success)
        //    |--------------|---------------------|
        //                       |------|------------------|
        //                      t2    add column (fail)  commit (success)
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
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn3->AddColumns(*db_name, table_name, columns3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop column
        //    t1         add column(fail)      rollback (success)
        //    |--------------|---------------------|
        //         |------|------------------|
        //        t2    drop column        commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, drop_column_and_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 =
            TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2, column_def3});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn3->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col3");
            status = txn4->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn5->GetTableInfo(*db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        EXPECT_EQ(table_info->column_defs_.size(), 1);
        EXPECT_STREQ(table_info->column_defs_[0]->name_.c_str(), "col2");
        status = new_txn_mgr->CommitTxn(txn5);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
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
        auto table_def1 =
            TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2, column_def3});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn3->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn4->DropColumns(*db_name, table_name, column_names);
            EXPECT_FALSE(status.ok());
        }
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn5->GetTableInfo(*db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        EXPECT_EQ(table_info->column_defs_.size(), 2);
        EXPECT_STREQ(table_info->column_defs_[0]->name_.c_str(), "col2");
        EXPECT_STREQ(table_info->column_defs_[1]->name_.c_str(), "col3");
        status = new_txn_mgr->CommitTxn(txn5);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop column and drop column
        //    t1         drop column          commit (success)
        //    |--------------|---------------------|
        //                                   |------|------------------|
        //                                  t2    drop column        commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 =
            TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2, column_def3});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn3->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop column
        {
            Vector<String> column_names;
            column_names.push_back("col3");
            status = txn4->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn5->GetTableInfo(*db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        EXPECT_EQ(table_info->column_defs_.size(), 1);
        EXPECT_STREQ(table_info->column_defs_[0]->name_.c_str(), "col2");
        status = new_txn_mgr->CommitTxn(txn5);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop column and drop column
        //    t1         drop column      commit (success)
        //    |--------------|---------------------|
        //                       |------|------------------|
        //                      t2    drop column (fail)  commit (success)
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

        // drop column
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn3->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        // drop column
        {
            Vector<String> column_names;
            column_names.push_back("col3");
            status = txn4->DropColumns(*db_name, table_name, column_names);
            EXPECT_FALSE(status.ok());
        }

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop column and drop column
        //    t1         drop column(fail)      rollback (success)
        //    |--------------|---------------------|
        //         |------|------------------|
        //        t2    drop column        commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        // drop column
        {
            Vector<String> column_names;
            column_names.push_back("col1");
            status = txn3->DropColumns(*db_name, table_name, column_names);
            EXPECT_TRUE(status.ok());
        }

        // drop column
        {
            Vector<String> column_names;
            column_names.push_back("col3");
            status = txn4->DropColumns(*db_name, table_name, column_names);
            EXPECT_FALSE(status.ok());
        }

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, test_block_lock) {
    using namespace infinity;

    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    String db_id_str = "0";
    String table_id_str = "0";
    SegmentID segment_id = 0;
    BlockID block_id = 0;
    String block_key1 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
    String block_key2 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id + 1, "lock");

    Status status = new_catalog->AddBlockLock(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->AddBlockLock(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
        EXPECT_TRUE(status.ok());
    }
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        EXPECT_FALSE(status.ok());
    }
}

TEST_P(NewCatalogTest, test_import) {
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

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            Pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);
            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            SizeT row_count = state.block_offset_end();
            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
                check_block(block_meta);
            }
        };

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
            check_segment(segment_meta);
        }
    }
}

TEST_P(NewCatalogTest, test_delete) {
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

    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
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
    }
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check data
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
        BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());

        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, 1);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 2);
            EXPECT_EQ(range.second, 3);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }
}

TEST_P(NewCatalogTest, test_compact) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
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
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (u32 i = 1; i < block_row_cnt; i += 2) {
            row_ids.push_back(RowID(0, i));
        }
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({2}));

        SegmentMeta segment_meta((*segment_ids)[0], *table_meta, *txn->kv_instance());

        auto [block_ids, block_status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(block_status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1, 2, 3}));

        {
            BlockMeta block_meta(0, segment_meta, *txn->kv_instance());
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            Pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, state.block_offset_end(), ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(block_row_cnt / 2 - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(block_row_cnt / 2 - 1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(block_row_cnt / 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(block_row_cnt / 2 + 1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValue(block_row_cnt - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(block_row_cnt - 1), Value::MakeInt(2));
            }
        }
        for (BlockID block_id = 1; block_id < 4; ++block_id) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            Pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            if (block_id == 3) {
                EXPECT_EQ(range.second, block_row_cnt / 2);
            } else {
                EXPECT_EQ(range.second, block_row_cnt);
            }

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, state.block_offset_end(), ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
                if (block_id == 3) {
                    EXPECT_EQ(col.GetValue(block_row_cnt / 2 - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValue(block_row_cnt / 2 - 1), Value::MakeInt(2));
                } else {
                    EXPECT_EQ(col.GetValue(block_row_cnt - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValue(block_row_cnt - 1), Value::MakeInt(2));
                }
            }
        }
    }
}

TEST_P(NewCatalogTest, test_append_with_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def5 =
        std::make_shared<ColumnDef>(4, std::make_shared<DataType>(LogicalType::kTensor, column3_type_info), "col5", std::set<ConstraintType>());
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3, column_def4, column_def5});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
    auto index_name3 = std::make_shared<std::string>("index3");
    Vector<InitParameter *> index3_parameters;
    index3_parameters.emplace_back(new InitParameter("metric", "l2"));
    index3_parameters.emplace_back(new InitParameter("plain_storage_data_type", "float"));
    auto index_def3 = IndexIVF::Make(index_name3, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index3_parameters);
    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);
    auto index_name6 = std::make_shared<std::string>("index6");
    Vector<InitParameter *> index6_parameters;
    index6_parameters.emplace_back(new InitParameter("pq_subspace_num", "4"));
    index6_parameters.emplace_back(new InitParameter("pq_subspace_bits", "8"));
    auto index_def6 = IndexEMVB::Make(index_name6, MakeShared<String>(), "file_name", Vector<String>{column_def5->name()}, index6_parameters);
    DeferFn defer_fn([&] {
        for (auto *parameter : index3_parameters) {
            delete parameter;
        }
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
        for (auto *parameter : index6_parameters) {
            delete parameter;
        }
    });
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
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);
    create_index(index_def3);
    create_index(index_def4);
    create_index(index_def5);
    create_index(index_def6);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
    {
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 2);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
            auto v1 = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                        reinterpret_cast<const char *>(vec.second.data()),
                                        vec.first.size(),
                                        column4_typeinfo);
            auto v2 = Value::MakeSparse(reinterpret_cast<const char *>(vec2.first.data()),
                                        reinterpret_cast<const char *>(vec2.second.data()),
                                        vec2.first.size(),
                                        column4_typeinfo);
            append_to_col(*col4, std::move(v1), std::move(v2));
            input_block->InsertVector(col4, 3);
        }
        {
            Vector<float> vec1 = {1.0, 2.0, 3.0, 4.0};
            Vector<float> vec2 = {5.0, 6.0, 7.0, 8.0};
            Vector<float> vec3 = {9.0, 10.0, 11.0, 12.0};
            auto col5 = ColumnVector::Make(column_def5->type());
            col5->Initialize();

            Vector<Pair<ptr_t, SizeT>> embedding_data1;
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
            Value v1 = Value::MakeTensor(embedding_data1, column3_type_info);

            Vector<Pair<ptr_t, SizeT>> embedding_data2;
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec3.data()), vec3.size() * sizeof(*vec3.data())));
            Value v2 = Value::MakeTensor(embedding_data2, column3_type_info);

            append_to_col(*col5, std::move(v1), std::move(v2));

            input_block->InsertVector(col5, 4);
        }
        input_block->Finalize();
    }
    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto dump_index = [&](const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append_a_block();
    dump_index(*index_name1);
    dump_index(*index_name2);
    dump_index(*index_name3);
    dump_index(*index_name4);
    dump_index(*index_name5);
    dump_index(*index_name6);
    append_a_block();
    // new_txn_mgr->PrintAllKeyValue();

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(segment_ids->size(), 1);
            segment_id = segment_ids->at(0);
            EXPECT_EQ(segment_id, 0);
        }
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta, table_index_meta->kv_instance());

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        // int32_t begin_val = 2;
        // int32_t end_val = 3;
        // {
        //     SharedPtr<SecondaryIndexInMem> memory_secondary_index = mem_index->memory_secondary_index_;
        //     u32 segment_row_count = 2;
        //     Tuple<u32, int32_t, int32_t> arg_tuple = {segment_row_count, begin_val, end_val};
        //     [[maybe_unused]] Pair<u32, Bitmask> result = memory_secondary_index->RangeQuery(&arg_tuple);
        // }

        ChunkID chunk_id = 0;
        {
            Vector<ChunkID> *chunk_ids = nullptr;
            Status status = segment_index_meta.GetChunkIDs(chunk_ids);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_ids->size(), 1);
            chunk_id = chunk_ids->at(0);
            EXPECT_EQ(chunk_id, 0);
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, *txn->kv_instance());
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        BufferObj *buffer_obj = nullptr;
        status = NewCatalog::GetChunkIndex(chunk_index_meta, buffer_obj);
        EXPECT_TRUE(status.ok());

        // {
        //     BufferHandle buffer_handle = buffer_obj->Load();
        //     auto *index = static_cast<const SecondaryIndexData *>(buffer_handle.GetData());

        //     [[maybe_unused]] const auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        //     [[maybe_unused]] const auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        // }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_indexer_->GetBaseRowId();
        u32 row_cnt = mem_index->memory_indexer_->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name3, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_ivf_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_ivf_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name4, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_hnsw_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_hnsw_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name5, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_bmp_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_bmp_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name6, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_emvb_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_emvb_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });

    auto merge_index = [&](const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("merge index {}", index_name)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto check_index2 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("check merged index {}", index_name)), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta, table_index_meta->kv_instance());

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, 2 * block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        ChunkID chunk_id = 0;
        {
            Vector<ChunkID> *chunk_ids = nullptr;
            Status status = segment_index_meta.GetChunkIDs(chunk_ids);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_ids->size(), 1);
            chunk_id = chunk_ids->at(0);
            EXPECT_EQ(chunk_id, 2);
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, *txn->kv_instance());
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        // int32_t begin_val = 2;
        // int32_t end_val = 3;

        BufferObj *buffer_obj = nullptr;
        status = NewCatalog::GetChunkIndex(chunk_index_meta, buffer_obj);
        EXPECT_TRUE(status.ok());

        // {
        //     BufferHandle buffer_handle = buffer_obj->Load();
        //     auto *index = static_cast<const SecondaryIndexData *>(buffer_handle.GetData());

        //     [[maybe_unused]] const auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        //     [[maybe_unused]] const auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        // }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    dump_index(*index_name1);
    dump_index(*index_name2);
    dump_index(*index_name3);
    dump_index(*index_name4);
    dump_index(*index_name5);
    dump_index(*index_name6);

    append_a_block();

    merge_index(*index_name1);
    merge_index(*index_name2);
    merge_index(*index_name3);
    merge_index(*index_name4);
    merge_index(*index_name5);
    merge_index(*index_name6);

    check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name2, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_indexer_->GetBaseRowId();
        u32 row_cnt = mem_index->memory_indexer_->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name3, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_ivf_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_ivf_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name4, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_hnsw_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_hnsw_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name5, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_bmp_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_bmp_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name6, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_emvb_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_emvb_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

TEST_P(NewCatalogTest, test_populate_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def5 =
        std::make_shared<ColumnDef>(4, std::make_shared<DataType>(LogicalType::kTensor, column3_type_info), "col5", std::set<ConstraintType>());
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3, column_def4, column_def5});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
    auto index_name3 = std::make_shared<std::string>("index3");
    Vector<InitParameter *> index3_parameters;
    index3_parameters.emplace_back(new InitParameter("metric", "l2"));
    index3_parameters.emplace_back(new InitParameter("plain_storage_data_type", "float"));
    auto index_def3 = IndexIVF::Make(index_name3, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index3_parameters);
    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);
    auto index_name6 = std::make_shared<std::string>("index6");
    Vector<InitParameter *> index6_parameters;
    index6_parameters.emplace_back(new InitParameter("pq_subspace_num", "4"));
    index6_parameters.emplace_back(new InitParameter("pq_subspace_bits", "8"));
    auto index_def6 = IndexEMVB::Make(index_name6, MakeShared<String>(), "file_name", Vector<String>{column_def5->name()}, index6_parameters);
    DeferFn defer_fn([&] {
        for (auto *parameter : index3_parameters) {
            delete parameter;
        }
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
        for (auto *parameter : index6_parameters) {
            delete parameter;
        }
    });
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
    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
    {
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 2);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
            auto v1 = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                        reinterpret_cast<const char *>(vec.second.data()),
                                        vec.first.size(),
                                        column4_typeinfo);
            auto v2 = Value::MakeSparse(reinterpret_cast<const char *>(vec2.first.data()),
                                        reinterpret_cast<const char *>(vec2.second.data()),
                                        vec2.first.size(),
                                        column4_typeinfo);
            append_to_col(*col4, std::move(v1), std::move(v2));
            input_block->InsertVector(col4, 3);
        }
        {
            Vector<float> vec1 = {1.0, 2.0, 3.0, 4.0};
            Vector<float> vec2 = {5.0, 6.0, 7.0, 8.0};
            Vector<float> vec3 = {9.0, 10.0, 11.0, 12.0};
            auto col5 = ColumnVector::Make(column_def5->type());
            col5->Initialize();

            Vector<Pair<ptr_t, SizeT>> embedding_data1;
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
            Value v1 = Value::MakeTensor(embedding_data1, column3_type_info);

            Vector<Pair<ptr_t, SizeT>> embedding_data2;
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<ptr_t>(vec3.data()), vec3.size() * sizeof(*vec3.data())));
            Value v2 = Value::MakeTensor(embedding_data2, column3_type_info);

            append_to_col(*col5, std::move(v1), std::move(v2));

            input_block->InsertVector(col5, 4);
        }
        input_block->Finalize();
    }
    auto append_a_block = [&] {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };
    for (int i = 0; i < 2; ++i) {
        append_a_block();
    }
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);
    create_index(index_def3);
    create_index(index_def4);
    create_index(index_def5);
    create_index(index_def6);
    append_a_block();

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("check index {}", index_name)), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(segment_ids->size(), 1);
            segment_id = segment_ids->at(0);
            EXPECT_EQ(segment_id, 0);
        }
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta, table_index_meta->kv_instance());

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, 2 * block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        ChunkID chunk_id = 0;
        {
            Vector<ChunkID> *chunk_ids = nullptr;
            Status status = segment_index_meta.GetChunkIDs(chunk_ids);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_ids->size(), 1);
            chunk_id = chunk_ids->at(0);
            EXPECT_EQ(chunk_id, 0);
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, *txn->kv_instance());
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        BufferObj *buffer_obj = nullptr;
        status = NewCatalog::GetChunkIndex(chunk_index_meta, buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_indexer_->GetBaseRowId();
        u32 row_cnt = mem_index->memory_indexer_->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name3, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_ivf_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_ivf_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name4, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_hnsw_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_hnsw_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name5, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_bmp_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_bmp_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name6, [&](const SharedPtr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->memory_emvb_index_->GetBeginRowID();
        u32 row_cnt = mem_index->memory_emvb_index_->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

TEST_P(NewCatalogTest, test_insert_and_import) {
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

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](Value v1, Value v2) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(std::move(v));
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(3), Value::MakeVarchar("xyz"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import2"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(4), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(5), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append2"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(6), Value::MakeVarchar("xyz"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1, 2}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({3}));

        SegmentMeta segment_meta(3, *table_meta, table_meta->kv_instance());
        auto [block_ids, block_status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(block_status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1, 2, 3, 4, 5}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewCatalogTest, test_cleanup) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
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

    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
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
    }
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index1"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(NewCatalogTest, test_add_columns) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto default_value2 = std::make_shared<ConstantExpr>(LiteralType::kString);
    {
        const char *str = "abcdefghijklmnopqrstuvwxyz";
        size_t str_len = strlen(str);
        default_value2->str_value_ = (char *)malloc(str_len + 1);
        strncpy(default_value2->str_value_, str, str_len + 1);
    }
    auto column_def2 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>(), "", default_value2);
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1});

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

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](Value v1) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(std::move(v));
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        input_block->Finalize();
        return input_block;
    };
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto input_block = make_input_block(Value::MakeInt(1));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def2);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;

        SegmentMeta segment_meta(segment_id, *table_meta, table_meta->kv_instance());
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());

        SizeT row_count = 0;
        status = block_meta.GetRowCnt(row_count);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, block_row_cnt);

        ColumnMeta column_meta(1, block_meta, block_meta.kv_instance());

        ColumnVector col;
        status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
        EXPECT_TRUE(status.ok());

        for (u32 i = 0; i < row_count; ++i) {
            EXPECT_EQ(col.GetValue(i), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Vector<String> column_names;
        column_names.emplace_back(column_def2->name());
        Status status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewCatalogTest, test_checkpoint) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
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

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        // Initialize input block
        auto append_column = [&](const SharedPtr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            input_block->InsertVector(col, input_block->column_count());
        };
        append_column(column_def1, v1);
        append_column(column_def2, v2);
        input_block->Finalize();
        return input_block;
    };
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto input_block = make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
        CheckpointOption option;
        Status status = txn->Checkpoint(option);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr->PrintAllKeyValue();

    infinity::InfinityContext::instance().UnInit();

    InfinityContext::instance().InitPhase1(this->config_path);
    InfinityContext::instance().InitPhase2();

    new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    new_txn_mgr->PrintAllKeyValue();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Status status;

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        SegmentMeta segment_meta(segment_id, *table_meta, table_meta->kv_instance());

        auto check_block = [&](BlockID block_id, const Value &v1, const Value &v2) {
            BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());

            SizeT row_count = 0;
            status = block_meta.GetRowCnt(row_count);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_count, block_row_cnt);

            auto check_column = [&](ColumnID column_id, const Value &v) {
                ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());
                ColumnVector col1;
                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col1);
                EXPECT_TRUE(status.ok());

                for (u32 i = 0; i < row_count; ++i) {
                    EXPECT_EQ(col1.GetValue(i), v);
                }
            };

            check_column(0, v1);
            check_column(1, v2);
        };
        check_block(0, Value::MakeInt(1), Value::MakeVarchar("abc"));
        check_block(1, Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

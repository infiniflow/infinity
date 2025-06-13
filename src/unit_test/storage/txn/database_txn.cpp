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

#include "gtest/gtest.h"
import base_test;

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import value;

import data_block;
import default_values;
import txn;
import status;
import extra_ddl_info;
import txn_state;
import new_txn_manager;
import new_txn;

using namespace infinity;

class DBTxnTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DBTxnTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(DBTxnTest, test1) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create
    NewTxn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());

    // Txn1: Get db1, OK
    auto [db_info1, status1] = new_txn->GetDatabaseInfo("db1");
    EXPECT_FALSE(status1.ok());

    // Txn1: Commit OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"), TransactionType::kNormal);

    // Txn2: Get db1, OK
    auto [db_info2, status2] = new_txn->GetDatabaseInfo("db1");
    EXPECT_TRUE(status2.ok());

    // Txn2: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_EQ(status.ok(), true);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn3: Get db1, NOT OK
    auto [db_info3, status3] = new_txn->GetDatabaseInfo("db1");
    EXPECT_FALSE(status3.ok());

    // Txn3: Drop db1, NOT OK
    status3 = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status3.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_P(DBTxnTest, test20) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"), TransactionType::kNormal);

    // Txn2: Get db1, OK
    auto [db_info1, status1] = new_txn->GetDatabaseInfo("db1");
    EXPECT_TRUE(status1.ok());

    // Txn2: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kNormal);

    // Txn3: Get db1, OK
    auto [db_info3, status3] = new_txn->GetDatabaseInfo("db1");
    EXPECT_FALSE(status3.ok());

    // Txn3: Create db1, NOT OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_P(DBTxnTest, test2) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK

    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn2: Get db1, OK
    auto [db_info2, status2] = new_txn->GetDatabaseInfo("db1");
    EXPECT_TRUE(status2.ok());

    // Txn2: Create db1, OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_FALSE(status.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kNormal);

    // Txn3: Get db1, OK
    auto [db_info4, status4] = new_txn->GetDatabaseInfo("db1");
    EXPECT_TRUE(status4.ok());

    // Txn3: Create db1, NOT OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(!status.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Commit     |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)            TXN2 Commit
// The diagram above may be outdated.
TEST_P(DBTxnTest, test3) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn2: Create, OK
    NewTxn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN2 Begin       |      TXN2 Create db1              |                      |      TXN2 Commit
//                    TXN1 Begin                    TXN1 Create db1(WW-Conflict)  TXN1 Commit
// The diagram above may be outdated.
TEST_P(DBTxnTest, test4) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);
    // Txn2: Create, OK
    NewTxn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create db1, NOT OK
    status = new_txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_FALSE(status.ok());
}

TEST_P(DBTxnTest, test5) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Rollback, OK
    txn_mgr->RollBackTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kRead);

    // Txn2: Get db1, NOT OK
    auto [db_info1, status1] = new_txn->GetDatabaseInfo("db1");
    EXPECT_FALSE(status1.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Rollback   |                |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_P(DBTxnTest, test6) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn2: Create, OK
    NewTxn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK

    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->RollBackTxn(new_txn1);

    // Txn2: Since txn1 is rollback, txn2 db1, OK, no conflict
    status = new_txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);

    // Txn3: Create, OK
    NewTxn *new_txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kRead);

    // Txn3: Get db1, OK
    auto [db_info1, status1] = new_txn3->GetDatabaseInfo("db1");
    EXPECT_TRUE(status1.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Drop db1   |   TXN1 Commit   |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
// The diagram above may be outdated.
TEST_P(DBTxnTest, test7) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Txn1: Create, OK
    NewTxn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn2: Create, OK
    NewTxn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);

    // Txn1: Create db1, OK
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, OK
    status = new_txn2->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
    EXPECT_FALSE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn3: Create, OK
    NewTxn *new_txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kRead);

    // Txn3: Get db1, OK
    auto [db_info1, status1] = new_txn3->GetDatabaseInfo("db1");
    EXPECT_TRUE(status1.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

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

#include "unit_test/base_test.h"

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import status;

class DBTxnTest : public BaseTest {
    void SetUp() override {
        system("rm -rf /tmp/infinity");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        BaseTest::TearDown();
    }
};

TEST_F(DBTxnTest, test1) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Create db1 before txn start, NOT OK
    EXPECT_THROW(new_txn->CreateDatabase("db1", ConflictType::kError), UnrecoverableException);

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);

    // Txn1: Get db1, OK
    auto [db_entry1, status1] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry1, nullptr);

    // Txn1: Commit OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Get db1, OK
    auto [db_entry2, status2] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status2.ok());

    // Txn2: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_EQ(status.ok(), true);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->Begin();

    // Txn3: Get db1, NOT OK
    auto [db_entry3, status3] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(!status3.ok());
    EXPECT_EQ(db_entry3, nullptr);

    // Txn3: Drop db1, NOT OK
    status3 = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status3.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_F(DBTxnTest, test20) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Get db1, OK
    auto [db_entry1, status1] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry1, nullptr);

    // Txn2: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Get db1, OK
    auto [db_entry2, status2] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status2.ok());
    EXPECT_NE(db_entry2, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->Begin();

    // Txn3: Get db1, OK
    auto [db_entry3, status3] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status3.ok());
    EXPECT_NE(db_entry3, nullptr);

    // Txn3: Create db1, NOT OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_F(DBTxnTest, test2) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK

    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Create db1, duplicated，NOT OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn1: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Get db1, NOT OK
    auto [db_entry1, status1] = new_txn->GetDatabase("db1");
    EXPECT_FALSE(status1.ok());
    EXPECT_EQ(db_entry1, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Get db1, OK
    auto [db_entry2, status2] = new_txn->GetDatabase("db1");
    EXPECT_FALSE(status2.ok());

    // Txn2: Create db1, OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Get db1, OK
    auto [db_entry3, status3] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status3.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->Begin();

    // Txn3: Get db1, OK
    auto [db_entry4, status4] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status4.ok());
    EXPECT_NE(db_entry4, nullptr);

    // Txn3: Create db1, NOT OK
    status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Commit     |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)            TXN2 Commit
TEST_F(DBTxnTest, test3) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn1: Create db1, OK
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, NOT OK, WW-conflict
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN2 Begin       |      TXN2 Create db1              |                      |      TXN2 Commit
//                    TXN1 Begin                    TXN1 Create db1(WW-Conflict)  TXN1 Commit
TEST_F(DBTxnTest, test4) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();
    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();
    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create db1, NOT OK
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

TEST_F(DBTxnTest, test5) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Rollback, OK
    txn_mgr->RollBackTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Get db1, NOT OK
    auto [db_entry, status1] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(!status1.ok());
    EXPECT_EQ(db_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Rollback   |                |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(DBTxnTest, test6) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn1: Create db1, OK

    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, NOT OK, WW-conflict
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn1: Commit, OK
    txn_mgr->RollBackTxn(new_txn1);

    // Txn2: Since txn1 is rollback, txn2 db1, OK, no conflict
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn3: Get db1, OK
    auto [db_entry, status1] = new_txn3->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Drop db1   |   TXN1 Commit   |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(DBTxnTest, test7) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn1: Create db1, OK

    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, NOT OK, WW-conflict
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(!status.ok());

    // Txn1: Drop db1, OK
    status = new_txn1->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn2: Create db1, OK
    status = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->RollBackTxn(new_txn1);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn3: Get db1, OK
    auto [db_entry, status1] = new_txn3->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

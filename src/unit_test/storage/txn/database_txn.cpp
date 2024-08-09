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
import value;

import data_block;
import default_values;
import txn_manager;
import txn;
import status;
import extra_ddl_info;

class DBTxnTest : public BaseTestParamStr {
    void SetUp() override {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        RemoveDbDirs();
        system(("mkdir -p " + infinity::String(GetFullPersistDir())).c_str());
        system(("mkdir -p " + infinity::String(GetFullDataDir())).c_str());
        system(("mkdir -p " + infinity::String(GetFullTmpDir())).c_str());
        std::string config_path_str = GetParam();
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTestParamStr::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DBTxnTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH,
                                           BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(DBTxnTest, test1) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create
    Txn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);

    // Txn1: Get db1, OK
    auto [db_entry1, status1] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry1, nullptr);

    // Txn1: Commit OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"));

    // Txn2: Get db1, OK
    auto [db_entry2, status2] = new_txn->GetDatabase("db1");
    EXPECT_TRUE(status2.ok());

    // Txn2: Drop db1, OK
    status = new_txn->DropDatabase("db1", ConflictType::kError);
    EXPECT_EQ(status.ok(), true);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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

TEST_P(DBTxnTest, test20) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"));

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
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"));

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

TEST_P(DBTxnTest, test2) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"));

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
TEST_P(DBTxnTest, test3) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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
TEST_P(DBTxnTest, test4) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));
    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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

TEST_P(DBTxnTest, test5) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());

    // Txn1: Rollback, OK
    txn_mgr->RollBackTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"));

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
TEST_P(DBTxnTest, test6) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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
    Txn *new_txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db1"));

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
TEST_P(DBTxnTest, test7) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

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
    Txn *new_txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db1"));

    // Txn3: Get db1, OK
    auto [db_entry, status1] = new_txn3->GetDatabase("db1");
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(db_entry, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

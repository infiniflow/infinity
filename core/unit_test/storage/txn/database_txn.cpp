//
// Created by jinhai on 23-6-4.
//


#include "unit_test/base_test.h"

import infinity;
import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;

class DBTxnTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/data/db");
        system("rm -rf /tmp/infinity/data/wal");
        system("rm -rf /tmp/infinity/data/catalog/*");
        system("rm -rf /tmp/infinity/_tmp");
    }
};

TEST_F(DBTxnTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create
    Txn *new_txn = txn_mgr->CreateTxn();

    EntryResult create_res;

    // Txn1: Create db1 before txn start, NOT OK
    create_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create_res.entry_, nullptr);

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create_res = new_txn->CreateDatabase("db1", ConflictType::kError);

    // Txn1: Get db1, OK
    EntryResult same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);

    // Txn1: Commit OK
    new_txn->CommitTxn();
    EXPECT_NE(create_res.entry_, nullptr);
    EXPECT_EQ(create_res.entry_->Committed(), true);

    // Txn2: Create
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Get db1, OK
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);

    // Txn2: Drop db1, OK
    EntryResult drop_db_res = new_txn->DropDatabase("db1", ConflictType::kError);
    ;
    EXPECT_NE(drop_db_res.entry_, nullptr);
    EXPECT_EQ(drop_db_res.err_, nullptr);

    EXPECT_NE(drop_db_res.entry_, create_res.entry_);

    // Txn2: Commit, OK
    new_txn->CommitTxn();

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn();

    // Txn3: Get db1, NOT OK
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(same_create_res.entry_, nullptr);

    // Txn3: Drop db1, NOT OK
    drop_db_res = new_txn->DropDatabase("db1", ConflictType::kError);
    ;
    EXPECT_EQ(drop_db_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn();
}

TEST_F(DBTxnTest, test20) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Drop db1, OK
    create1_res = new_txn->DropDatabase("db1", ConflictType::kError);
    ;
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn();

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn();

    // Txn3: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Create db1, NOT OK
    create3_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn();
}

TEST_F(DBTxnTest, test2) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create db1, duplicated，NOT OK
    create2_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Drop db1, OK
    dropped_res = new_txn->DropDatabase("db1", ConflictType::kError);
    ;
    EXPECT_EQ(create1_res.entry_, dropped_res.entry_);

    // Txn1: Get db1, NOT OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, create1_res.entry_);

    // Txn2: Commit, OK
    new_txn->CommitTxn();

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn();

    // Txn3: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Create db1, NOT OK
    create3_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Commit     |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)            TXN2 Commit
TEST_F(DBTxnTest, test3) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Commit, OK
    new_txn2->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN2 Begin       |      TXN2 Create db1              |                      |      TXN2 Commit
//                    TXN1 Begin                    TXN1 Create db1(WW-Conflict)  TXN1 Commit
TEST_F(DBTxnTest, test4) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();
    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;
    // Txn2: Begin, OK
    new_txn2->BeginTxn();
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create db1, NOT OK
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();
}

TEST_F(DBTxnTest, test5) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    EntryResult create_res, get_res;
    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create_res = new_txn->CreateDatabase("db1", ConflictType::kError);

    // Txn1: Rollback, OK
    new_txn->RollbackTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Get db1, NOT OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Rollback   |                |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(DBTxnTest, test6) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn();

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Drop db1   |   TXN1 Commit   |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(DBTxnTest, test7) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, dropped1_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Drop db1, OK
    dropped1_res = new_txn1->DropDatabase("db1", ConflictType::kError);
    ;
    EXPECT_NE(dropped1_res.entry_, nullptr);

    // Txn2: Create db1, OK
    create2_res = new_txn2->CreateDatabase("db1", ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn();

    // Txn2: Commit, OK
    new_txn2->CommitTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

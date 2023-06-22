//
// Created by jinhai on 23-6-4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"
#include "storage/transaction/txn_manager.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class TransactionTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(TransactionTest, test1) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    // Txn1: Create
    Txn* new_txn = txn_mgr.CreateTxn();

    EntryResult create_res;

    // Txn1: Create db1 before txn start, NOT OK
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_, nullptr);

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create_res = new_txn->CreateDatabase("db1");

    // Txn1: Get db1, OK
    EntryResult same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);

    // Txn1: Commit OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    EXPECT_NE(create_res.entry_, nullptr);
    EXPECT_EQ(create_res.entry_->Committed(), true);

    // Txn2: Create
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Get db1, OK
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);

    // Txn2: Drop db1, OK
    EntryResult drop_db_res = new_txn->DropDatabase("db1");
    EXPECT_NE(drop_db_res.entry_, nullptr);
    EXPECT_EQ(drop_db_res.err_, nullptr);

    EXPECT_NE(drop_db_res.entry_, create_res.entry_);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get db1, NOT OK
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(same_create_res.entry_, nullptr);

    // Txn3: Drop db1, NOT OK
    drop_db_res = new_txn->DropDatabase("db1");
    EXPECT_EQ(drop_db_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test2) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create db1, duplicated，NOT OK
    create2_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Drop db1, OK
    dropped_res = new_txn->DropDatabase("db1");
    EXPECT_EQ(create1_res.entry_, dropped_res.entry_);

    // Txn1: Get db1, NOT OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, create1_res.entry_);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Commit     |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)            TXN2 Commit
TEST_F(TransactionTest, test3) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+->
//           |            |               |                   |                      |           |
//       TXN2 Begin       |      TXN2 Create db1              |                      |      TXN2 Commit
//                    TXN1 Begin                    TXN1 Create db1(WW-Conflict)  TXN1 Commit
TEST_F(TransactionTest, test4) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();
    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;
    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create db1, NOT OK
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test5) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    EntryResult create_res, get_res;
    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create_res = new_txn->CreateDatabase("db1");

    // Txn1: Rollback, OK
    new_txn->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Get db1, NOT OK
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Rollback   |                |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(TransactionTest, test6) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN1 Begin       |      TXN1 Create db1              |                  TXN1 Drop db1   |   TXN1 Commit   |
//                    TXN2 Begin                    TXN2 Create db1(WW-Conflict)         TXN2 Create db1 OK  TXN2 Commit
TEST_F(TransactionTest, test7) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped1_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create db1, NOT OK, WW-conflict
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Drop db1, OK
    dropped1_res = new_txn1->DropDatabase("db1");
    EXPECT_NE(dropped1_res.entry_, nullptr);

    // Txn2: Create db1, OK
    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

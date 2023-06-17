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
    Txn* new_txn = txn_mgr.CreateTxn();
    // Create db before txn start
    EntryResult create_res;
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_, nullptr);

    // Start txn, create db, then get the db
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create_res = new_txn->CreateDatabase("db1");
    EntryResult same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    EXPECT_NE(create_res.entry_, nullptr);
    EXPECT_EQ(create_res.entry_->Committed(), true);

    // New txn, get the db, after creation
    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);

    EntryResult drop_db_res = new_txn->DropDatabase("db1");
    EXPECT_NE(drop_db_res.entry_, nullptr);
    EXPECT_EQ(drop_db_res.err_, nullptr);

    EXPECT_NE(drop_db_res.entry_, create_res.entry_);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // New txn, get the db, after deletion
    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_NE(drop_db_res.entry_, same_create_res.entry_);

    drop_db_res = new_txn->DropDatabase("db1");
    EXPECT_EQ(drop_db_res.entry_, nullptr);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test2) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn* new_txn = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_, nullptr);
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);
    create2_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    dropped_res = new_txn->DropDatabase("db1");
    EXPECT_EQ(create1_res.entry_, dropped_res.entry_);

    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Another txn
    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, create1_res.entry_);

    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test3) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn *new_txn1 = txn_mgr.CreateTxn();
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test4) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn *new_txn1 = txn_mgr.CreateTxn();
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    create2_res = new_txn2->CreateDatabase("db1");
    EXPECT_EQ(create2_res.entry_, nullptr);

    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test5) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn* new_txn = txn_mgr.CreateTxn();
    // Create db before txn start
    EntryResult create_res, get_res;

    // Start txn, create db, then get the db
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create_res = new_txn->CreateDatabase("db1");
    new_txn->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // New txn, get the db, after creation
    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    get_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

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
    DBEntry* db_entry = nullptr;
    db_entry = new_txn->CreateDatabase("db1");
    EXPECT_EQ(db_entry, nullptr);
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    db_entry = new_txn->CreateDatabase("db1");
    DBEntry* same_db_entry = new_txn->GetDatabase("db1");
    EXPECT_EQ(db_entry, same_db_entry);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    EXPECT_NE(db_entry, nullptr);
    EXPECT_EQ(db_entry->Committed(), true);

    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    same_db_entry = new_txn->GetDatabase("db1");
    EXPECT_EQ(db_entry, same_db_entry);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test2) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn* new_txn = txn_mgr.CreateTxn();
    DBEntry* res {nullptr};
    res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(res, nullptr);
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(res->Committed(), false);
    EXPECT_NE(res, nullptr);
    res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(res, nullptr);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}


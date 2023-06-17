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
    EntryResult create_res;
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_, nullptr);
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create_res = new_txn->CreateDatabase("db1");
    EntryResult same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    EXPECT_NE(create_res.entry_, nullptr);
    EXPECT_EQ(create_res.entry_->Committed(), true);

    new_txn = txn_mgr.CreateTxn();
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    same_create_res = new_txn->GetDatabase("db1");
    EXPECT_EQ(create_res.entry_, same_create_res.entry_);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TransactionTest, test2) {
    using namespace infinity;
    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);
    Txn* new_txn = txn_mgr.CreateTxn();
    EntryResult create_res;
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_, nullptr);
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_->Committed(), false);
    EXPECT_NE(create_res.entry_, nullptr);
    create_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create_res.entry_, nullptr);
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}


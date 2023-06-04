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
    UniquePtr<Txn> new_txn = txn_mgr.CreateTxn();
    new_txn->CreateDatabase("db1");
}

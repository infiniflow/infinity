
#include "infinity_test.h"
#include "main/infinity.h"
#include "storage/data_block.h"
#include "storage/txn/txn_manager.h"
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

class WaReplayTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override { system("rm -rf /tmp/infinity"); }
};

using namespace infinity;
namespace fs = std::filesystem;

TEST_F(WaReplayTest, WalReplay) {

    {

        using namespace infinity;
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        NewCatalog *catalog = storage->catalog();

        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        txn->CreateDatabase("db1", ConflictType::kIgnore);
        txn->CommitTxn();

        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        txn2->CreateDatabase("db2", ConflictType::kIgnore);
        txn2->CommitTxn();

        auto *txn3 = txn_mgr->CreateTxn();
        txn3->BeginTxn();
        txn3->CreateDatabase("db3", ConflictType::kIgnore);
        txn3->CommitTxn();

        auto *txn4 = txn_mgr->CreateTxn();
        txn4->BeginTxn();
        txn4->CreateDatabase("db4", ConflictType::kIgnore);
        txn4->CommitTxn();

        auto *txn5 = txn_mgr->CreateTxn();
        txn5->BeginTxn();
        txn5->Checkpoint(3, true);
        txn5->CommitTxn();

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }

    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        NewCatalog *catalog = storage->catalog();

        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        txn->CreateDatabase("db4", ConflictType::kInvalid);
        txn->CommitTxn();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

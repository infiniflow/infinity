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
import infinity_core;
import global_resource_usage;
import compilation_config;
import extra_ddl_info;

using namespace infinity;

class RecycleLogTest : public BaseTestParamStr {
protected:
    static std::shared_ptr<std::string> test_ckp_recycle_config() {
        return GetParam() == BaseTestParamStr::NULL_CONFIG_PATH
                   ? std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp.toml")
                   : std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp_vfs_off.toml");
    }

    void SetUp() override { CleanupDbDirs(); }

    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         RecycleLogTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));

TEST_P(RecycleLogTest, recycle_wal_after_delta_checkpoint) {
    {
        // Earlier cases may leave a dirty infinity instance. Destroy it first.
        infinity::InfinityContext::instance().UnInit();
        CleanupDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = RecycleLogTest::test_ckp_recycle_config();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        Config *config = storage->config();
        //TxnManager *txn_mgr = storage->txn_manager();
        //BGTaskProcessor *bg_processor = storage->bg_processor();

        const String &wal_dir = config->WALDir();
        {
            time_t start = time(nullptr);
            while (true) {
                time_t now = time(nullptr);
                if (now - start > 20) {
                    String error_message = "Timeout";
                    UnrecoverableError(error_message);
                }
                // create and drop db to fill wal log
                {
                    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
                    auto status = txn->DropDatabase("db1", ConflictType::kIgnore);
                    EXPECT_TRUE(status.ok());
                    status = new_txn_mgr->CommitTxn(txn);
                    EXPECT_TRUE(status.ok());
                }
                { // put create after drop to prevent the merge delta result is empty
                    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
                    auto status = txn->CreateDatabase("db1", ConflictType::kIgnore, MakeShared<String>());
                    EXPECT_TRUE(status.ok());
                    status = new_txn_mgr->CommitTxn(txn);
                    EXPECT_TRUE(status.ok());
                }
                { // loop until the wal directory has a non-temp wal
                    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);
                    if (wal_infos.size() > 0) {
                        break;
                    }
                }
            }
        }

        std::shared_ptr<TxnTimeStamp> ckp_commit_ts = std::make_shared<TxnTimeStamp>(0);
        {
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            WalManager *wal_manager_{};
            wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check point"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn,ckp_commit_ts.get());
            EXPECT_TRUE(status.ok());
        }
        //        {
        //            // assert there is one log file
        //            auto [temp_wal_file, wal_files] = WalFile::ParseWalFilenames(wal_dir);
        //            EXPECT_TRUE(temp_wal_file.has_value());
        //            if (wal_files.size() >= 1) {
        //                EXPECT_EQ(*ckp_commit_ts, wal_files.back().max_commit_ts_ + 2);
        //            } else {
        //                ASSERT_TRUE(wal_files.empty());
        //            }
        //        }
        infinity::InfinityContext::instance().UnInit();

#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init(); // test replay
#endif
        std::shared_ptr<std::string> config_path = RecycleLogTest::test_ckp_recycle_config();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
            Status status = std::get<1>(txn->GetDatabaseInfo("db1"));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        infinity::InfinityContext::instance().UnInit();

#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}
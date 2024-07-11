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

import stl;
import global_resource_usage;
import storage;
import infinity_context;
import compilation_config;
import txn_manager;
import extra_ddl_info;
import infinity_exception;
import log_file;
import config;
import bg_task;
import background_process;
import local_file_system;
import default_values;
import status;
import logger;

using namespace infinity;

class RecycleLogTest : public BaseTest {
protected:
    static std::shared_ptr<std::string> test_ckp_recycle_config() {
        return std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp.toml");
    }

    void SetUp() override { RemoveDbDirs(); }

    void TearDown() override { RemoveDbDirs(); }
};

TEST_F(RecycleLogTest, recycle_wal_after_delta_checkpoint) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = RecycleLogTest::test_ckp_recycle_config();
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        Config *config = storage->config();
        TxnManager *txn_mgr = storage->txn_manager();
        BGTaskProcessor *bg_processor = storage->bg_processor();

        const String &wal_dir = config->WALDir();
        LocalFileSystem fs;
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
                    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));
                    auto status = txn->DropDatabase("db1", ConflictType::kIgnore);
                    ASSERT_TRUE(status.ok() || status.code() == ErrorCode::kIgnore);
                    txn_mgr->CommitTxn(txn);
                }
                { // put create after drop to prevent the merge delta result is empty
                    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"));
                    auto status = txn->CreateDatabase("db1", ConflictType::kIgnore);
                    ASSERT_TRUE(status.ok());
                    txn_mgr->CommitTxn(txn);
                }
                { // loop until the wal directory has a non-temp wal
                    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);
                    if (wal_infos.size() > 0) {
                        break;
                    }
                }
            }
        }
        TxnTimeStamp ckp_commit_ts = 0;
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("full ckp"));
            SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn, false /*full_check_point*/);
            bg_processor->Submit(force_ckp_task);
            force_ckp_task->Wait();
            ckp_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            // assert there is one log file
            auto [temp_wal_file, wal_files] = WalFile::ParseWalFilenames(wal_dir);
            ASSERT_TRUE(temp_wal_file.has_value());
            if (wal_files.size() == 1) {
                ASSERT_EQ(ckp_commit_ts, wal_files[0].max_commit_ts_);
            } else {
                ASSERT_TRUE(wal_files.empty());
            }
        }
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
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get db"));
            auto [db, status] = txn->GetDatabase("db1");
            ASSERT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        infinity::InfinityContext::instance().UnInit();

#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_F(RecycleLogTest, recycle_wal_after_full_checkpoint) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = RecycleLogTest::test_ckp_recycle_config();
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        Config *config = storage->config();
        TxnManager *txn_mgr = storage->txn_manager();
        BGTaskProcessor *bg_processor = storage->bg_processor();

        const String &wal_dir = config->WALDir();
        const String &catalog_dir = config->DataDir() + "/" + String(CATALOG_FILE_DIR);
        LocalFileSystem fs;
        for (int i = 0; i < 2; ++i) { // create 2 delta catalog file
            time_t start = time(nullptr);
            while (true) {
                time_t now = time(nullptr);
                if (now - start > 20) {
                    String error_message = "Timeout";
                    UnrecoverableError(error_message);
                }
                // create and drop db to fill wal log
                {
                    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));
                    auto status = txn->DropDatabase("db1", ConflictType::kIgnore);
                    ASSERT_TRUE(status.ok() || status.code() == ErrorCode::kIgnore);
                    txn_mgr->CommitTxn(txn);
                }
                { // put create after drop to prevent the merge delta result is empty
                    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"));
                    auto status = txn->CreateDatabase("db1", ConflictType::kIgnore);
                    ASSERT_TRUE(status.ok());
                    txn_mgr->CommitTxn(txn);
                }
                { // loop until the wal directory has a non-temp wal
                    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);
                    if (wal_infos.size() > 0) {
                        break;
                    }
                }
            }
            {
                auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("delta ckp"));
                SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn, false /*delta_check_point*/);
                bg_processor->Submit(force_ckp_task);
                force_ckp_task->Wait();
                txn_mgr->CommitTxn(txn);
            }
        }
        {
            auto [full_catalog_infos, delta_catalog_infos] = CatalogFile::ParseCheckpointFilenames(catalog_dir);
            ASSERT_EQ(full_catalog_infos.size(), 1ul); // initialize will write a full checkpoint
            ASSERT_EQ(delta_catalog_infos.size(), 2ul);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("full ckp"));
            SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn, true /*full_check_point*/);
            bg_processor->Submit(force_ckp_task);
            force_ckp_task->Wait();
            txn_mgr->CommitTxn(txn);
        }
        {
            // assert there is one full catalog file
            auto [full_catalog_infos, delta_catalog_infos] = CatalogFile::ParseCheckpointFilenames(catalog_dir);
            ASSERT_EQ(full_catalog_infos.size(), 1ul);
            ASSERT_TRUE(delta_catalog_infos.empty());
        }
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
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get db"));
            auto [db, status] = txn->GetDatabase("db1");
            ASSERT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        infinity::InfinityContext::instance().UnInit();

#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

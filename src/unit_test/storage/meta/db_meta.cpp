#include "unit_test/base_test.h"

import infinity;
import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import catalog;
import base_entry;
import local_file_system;
import status;
import logger;
import third_party;
import db_meta;

import txn_manager;
import txn;
import catalog;

using namespace infinity;

class DBMetaTest : public BaseTestParamStr {
    void SetUp() override {
        BaseTestParamStr::SetUp();
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
                         DBMetaTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(DBMetaTest, to_string_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        ASSERT_STREQ(base_entry->db_meta_->ToString()->c_str(), "DBMeta, data_dir: /var/infinity/data, db name: db1, entry count: 1");
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        ASSERT_STREQ(base_entry->db_meta_->ToString()->c_str(), "DBMeta, data_dir: /var/infinity/data, db name: db1, entry count: 0");
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(DBMetaTest, empty_db_entry_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        EXPECT_FALSE(base_entry->db_meta_->Empty());
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        EXPECT_TRUE(base_entry->db_meta_->Empty());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(DBMetaTest, get_all_db_entry_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        EXPECT_EQ(base_entry->db_meta_->GetAllEntries().size(), 1);
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout<<base_entry->db_meta_->ToString()->c_str()<<std::endl;
        EXPECT_EQ(base_entry->db_meta_->GetAllEntries().size(), 0);
    }

    txn_mgr->CommitTxn(txn1);
}

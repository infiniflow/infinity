#include "gtest/gtest.h"
import base_test;

import infinity;
import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import catalog;
import base_entry;
import status;
import logger;
import third_party;
import db_meta;

import txn_manager;
import txn;
import catalog;

using namespace infinity;

class DBMetaTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DBMetaTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(DBMetaTest, to_string_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
        ASSERT_STREQ(base_entry->db_meta_->ToString()->c_str(), "DBMeta, db name: db1, entry count: 1");
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
        ASSERT_STREQ(base_entry->db_meta_->ToString()->c_str(), "DBMeta, db name: db1, entry count: 0");
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
        auto [base_entry, status] = catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
        EXPECT_FALSE(base_entry->db_meta_->Empty());
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
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
        auto [base_entry, status] = catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
        EXPECT_EQ(base_entry->db_meta_->GetAllEntries().size(), 1);
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        std::cout << base_entry->db_meta_->ToString()->c_str() << std::endl;
        EXPECT_EQ(base_entry->db_meta_->GetAllEntries().size(), 0);
    }

    txn_mgr->CommitTxn(txn1);
}

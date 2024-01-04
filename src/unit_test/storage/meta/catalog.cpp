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

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;
import new_catalog;
import status;

class CatalogTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        BaseTest::TearDown();
    }
};

// txn1: create db1, get db1, delete db1, get db1, commit
// txn2:             get db1,             get db1, commit
TEST_F(CatalogTest, simple_test1) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    NewCatalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->CreateTxn();
    txn1->Begin();

    // start txn2
    auto *txn2 = txn_mgr->CreateTxn();
    txn2->Begin();
    HashMap<String, BaseEntry *> databases;

    // create db in empty catalog should be success
    {
        BaseEntry* base_entry{nullptr};
        Status status = NewCatalog::CreateDatabase(catalog, "db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr, base_entry);
        EXPECT_TRUE(status.ok());
        // store this entry
        databases["db1"] = base_entry;
    }

    {
        BaseEntry* base_db_entry{nullptr};
        Status status1 = NewCatalog::GetDatabase(catalog, "db1", txn1->TxnID(), txn1->BeginTS(), base_db_entry);
        // should be visible to same txn
        EXPECT_TRUE(status1.ok());
        EXPECT_EQ(base_db_entry, databases["db1"]);

        // should not be visible to other txn
        Status status2 = NewCatalog::GetDatabase(catalog, "db1", txn2->TxnID(), txn2->BeginTS(), base_db_entry);
        EXPECT_TRUE(!status2.ok());
    }

    // drop db should be success
    {
        BaseEntry* base_entry{nullptr};
        Status status = NewCatalog::DropDatabase(catalog, "db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr, base_entry);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(base_entry, databases["db1"]);
        // remove this entry
        databases.erase("db1");

        BaseEntry* base_db_entry{nullptr};
        Status status1 = NewCatalog::GetDatabase(catalog, "db1", txn1->TxnID(), txn1->BeginTS(), base_db_entry);
        // should not be visible to same txn
        EXPECT_TRUE(!status1.ok());

        // should not be visible to other txn
        Status status2 = NewCatalog::GetDatabase(catalog, "db1", txn2->TxnID(), txn2->BeginTS(), base_db_entry);
        EXPECT_TRUE(!status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
    txn_mgr->CommitTxn(txn2);
}

// txn1: create db1, commit.
// txn2: start,              get db1, commit
// txn3:                     start, get db1, delete db1, commit
TEST_F(CatalogTest, simple_test2) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    NewCatalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->CreateTxn();
    txn1->Begin();

    // start txn2
    auto *txn2 = txn_mgr->CreateTxn();
    txn2->Begin();

    HashMap<String, BaseEntry *> databases;

    // create db in empty catalog should be success
    {
        BaseEntry* base_entry{nullptr};
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
        EXPECT_TRUE(status.ok());
        // store this entry
        databases["db1"] = base_entry;
    }

    u64 txn1_id = txn1->TxnID();
    TxnTimeStamp txn1_begin_ts = txn1->BeginTS();
    txn_mgr->CommitTxn(txn1);

    // should not be visible to txn2
    {
        BaseEntry* base_db_entry{nullptr};
        Status status1 = NewCatalog::GetDatabase(catalog, "db1", txn1_id, txn1_begin_ts, base_db_entry);
        // should not be visible to same txn
        EXPECT_TRUE(!status1.ok());
    }

    txn_mgr->CommitTxn(txn2);

    auto *txn3 = txn_mgr->CreateTxn();
    txn3->Begin();

    // should be visible to txn3
    {
        BaseEntry* base_db_entry{nullptr};
        Status status1 = NewCatalog::GetDatabase(catalog, "db1", txn3->TxnID(), txn3->BeginTS(), base_db_entry);
        EXPECT_TRUE(status1.ok());
        EXPECT_EQ(base_db_entry, databases["db1"]);

        BaseEntry* base_entry{nullptr};
        Status status = txn3->DropDatabase("db1", ConflictType::kError, base_entry);
        EXPECT_TRUE(status.ok());
        // should be different db entry
        EXPECT_NE(base_entry, databases["db1"]);
        // remove this entry
        databases.erase("db1");

        // should not be visible to other txn
        base_db_entry = nullptr;
        Status status2 = NewCatalog::GetDatabase(catalog, "db1", txn3->TxnID(), txn3->BeginTS(), base_db_entry);
        EXPECT_TRUE(!status2.ok());
    }

    txn_mgr->CommitTxn(txn3);
}

TEST_F(CatalogTest, concurrent_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    NewCatalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    for (int loop = 0; loop < 1; ++loop) {
        // start txn1 && txn2
        auto *txn1 = txn_mgr->CreateTxn();
        txn1->Begin();
        auto *txn2 = txn_mgr->CreateTxn();
        txn2->Begin();

        // lock protect databases
        std::mutex lock;
        HashMap<String, BaseEntry *> databases;

        auto write_routine = [&](int start, Txn *txn) {
            for (int db_id = start; db_id < 1000; db_id += 2) {
                BaseEntry* base_entry{};
                String db_name = "db" + ToStr(db_id);
                Status status = txn->CreateDatabase(db_name, ConflictType::kError, base_entry);
                EXPECT_TRUE(status.ok());
                // store this entry
                lock.lock();
                databases[db_name] = base_entry;
                lock.unlock();
            }
        };

        Thread write_thread1(write_routine, 0, txn1);
        Thread write_thread2(write_routine, 1, txn2);

        write_thread1.join();
        write_thread2.join();

        txn_mgr->CommitTxn(txn1);
        txn_mgr->CommitTxn(txn2);

        // start txn3 && txn4
        auto *txn3 = txn_mgr->CreateTxn();
        txn3->Begin();
        auto *txn4 = txn_mgr->CreateTxn();
        txn4->Begin();

        auto read_routine = [&](Txn *txn) {
            for (int db_id = 0; db_id < 1000; ++db_id) {
                String db_name = "db" + ToStr(db_id);
                BaseEntry* base_entry{nullptr};
                Status status = NewCatalog::GetDatabase(catalog, db_name, txn->TxnID(), txn->BeginTS(), base_entry);
                EXPECT_TRUE(status.ok());
                // only read, don't need lock
                EXPECT_EQ(base_entry, databases[db_name]);
            }
        };

        std::thread read_thread1(read_routine, txn3);
        std::thread read_thread2(read_routine, txn4);
        read_thread1.join();
        read_thread2.join();

        txn_mgr->CommitTxn(txn3);
        txn_mgr->CommitTxn(txn4);

        // start txn5 && txn6
        auto *txn5 = txn_mgr->CreateTxn();
        txn5->Begin();
        auto *txn6 = txn_mgr->CreateTxn();
        txn6->Begin();

        auto drop_routine = [&](int start, Txn *txn) {
            for (int db_id = start; db_id < 1000; db_id += 2) {
                String db_name = "db" + ToStr(db_id);
                BaseEntry* base_entry{nullptr};
                Status status = txn->DropDatabase(db_name, ConflictType::kError, base_entry);
                EXPECT_TRUE(status.ok());
                // store this entry
                lock.lock();
                databases.erase(db_name);
                lock.unlock();
            }
        };

        std::thread drop_thread1(drop_routine, 0, txn5);
        std::thread drop_thread2(drop_routine, 1, txn6);
        drop_thread1.join();
        drop_thread2.join();

        txn_mgr->CommitTxn(txn5);
        txn_mgr->CommitTxn(txn6);

        // start txn7
        auto *txn7 = txn_mgr->CreateTxn();
        txn7->Begin();

        // check all has been dropped
        for (int db_id = 0; db_id < 1000; ++db_id) {
            String db_name = "db" + ToStr(db_id);
            BaseEntry* base_db_entry{nullptr};
            Status status = NewCatalog::GetDatabase(catalog, db_name, txn7->TxnID(), txn7->BeginTS(), base_db_entry);
            EXPECT_TRUE(!status.ok());
        }
    }
}

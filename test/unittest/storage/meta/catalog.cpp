//
// Created by tangdonghai on 23-9-5.
//

#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <utility>
#include "base_test.h"
#include "common/types/internal_types.h"
#include "main/infinity.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/meta/catalog.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/txn/txn.h"
#include "storage/txn/txn_manager.h"
class CatalogTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        system("rm -rf /tmp/infinity/data/db");
        system("rm -rf /tmp/infinity/_tmp");
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }

};

// txn1: create db1, get db1, delete db1, get db1, commit
// txn2:             get db1,             get db1, commit
TEST_F(CatalogTest, simple_test1) {
    using namespace infinity;

    // create bufferManager && TxnManager
    SizeT memory_limit = 1024 * 1024 * 1024;
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("db");
    NewCatalog new_catalog(std::move(dir));
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

    // start txn1
    auto* txn1 = txn_mgr.CreateTxn();
    txn1->BeginTxn();

    // start txn2
    auto* txn2 = txn_mgr.CreateTxn();
    txn2->BeginTxn();
    HashMap<String, BaseEntry*> databases;

    // create db in empty catalog should be success
    {
        EntryResult res;
        res = NewCatalog::CreateDatabase(&new_catalog, "db1", txn1->TxnID(), txn1->BeginTS(), &txn_mgr);
        EXPECT_TRUE(res.Success());
        // store this entry
        databases["db1"] = res.entry_;
    }

    {
        EntryResult res;
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn1->TxnID(), txn1->BeginTS());
        // should be visible to same txn
        EXPECT_TRUE(res.Success());
        EXPECT_EQ(res.entry_, databases["db1"]);

        // should not be visible to other txn
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(res.Fail());
    }

    // drop db should be success
    {
        EntryResult res;
        res = NewCatalog::DropDatabase(&new_catalog, "db1", txn1->TxnID(), txn1->BeginTS(), &txn_mgr);
        EXPECT_TRUE(res.Success());
        EXPECT_EQ(res.entry_, databases["db1"]);
        // remove this entry
        databases.erase("db1");

        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn1->TxnID(), txn1->BeginTS());
        // should not be visible to same txn
        EXPECT_TRUE(res.Fail());

        // should not be visible to other txn
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(res.Fail());
    }

    txn1->CommitTxn();
    txn2->CommitTxn();
}

// txn1: create db1, commit.
// txn2: start,              get db1, commit       
// txn3:                     start, get db1, delete db1, commit
TEST_F(CatalogTest, simple_test2) {
    using namespace infinity;

    // create bufferManager && TxnManager
    SizeT memory_limit = 1024 * 1024 * 1024;
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("db");
    NewCatalog new_catalog(std::move(dir));
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

    // start txn1
    auto* txn1 = txn_mgr.CreateTxn();
    txn1->BeginTxn();

    // start txn2
    auto* txn2 = txn_mgr.CreateTxn();
    txn2->BeginTxn();

    HashMap<String, BaseEntry*> databases;

    // create db in empty catalog should be success
    {
        EntryResult res;
        res = txn1->CreateDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(res.Success());
        // store this entry
        databases["db1"] = res.entry_;
    }

    txn1->CommitTxn();

    // should not be visible to txn2
    {
        EntryResult res;
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(res.Fail());
    }

    txn2->CommitTxn();

    auto* txn3 = txn_mgr.CreateTxn();
    txn3->BeginTxn();


    // should be visible to txn3
    {
        EntryResult res;
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn3->TxnID(), txn3->BeginTS());
        EXPECT_TRUE(res.Success());
        EXPECT_EQ(res.entry_, databases["db1"]);

        res = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(res.Success());
        // should be different db entry
        EXPECT_NE(res.entry_, databases["db1"]);
        // remove this entry
        databases.erase("db1");

        // should not be visible to other txn
        res = NewCatalog::GetDatabase(&new_catalog, "db1", txn3->TxnID(), txn3->BeginTS());
        EXPECT_TRUE(res.Fail());
    }

    txn3->CommitTxn();
}

TEST_F(CatalogTest, concurrent_test) {
    using namespace infinity;

    // create bufferManager && TxnManager
    SizeT memory_limit = 1024 * 1024 * 1024;
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("db");
    NewCatalog new_catalog(std::move(dir));
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

    // start txn1 && txn2
    auto* txn1 = txn_mgr.CreateTxn();
    txn1->BeginTxn();
    auto* txn2 = txn_mgr.CreateTxn();
    txn2->BeginTxn();

    // lock protect databases
    std::mutex lock;
    HashMap<String, BaseEntry*> databases;

    auto write_routine = [&](int start, Txn* txn) {
        EntryResult res;
        for(int i = start; i < 1000; i += 2) {
            String db_name = "db" + std::to_string(i);
            res = txn1->CreateDatabase(db_name, ConflictType::kError);
            EXPECT_TRUE(res.Success());
            // store this entry
            lock.lock();
            databases[db_name] = res.entry_;
            lock.unlock();
        }
    };

    std::thread write_thread1(write_routine, 0, txn1);
    std::thread write_thread2(write_routine, 1, txn2);

    write_thread1.join();
    write_thread2.join();

    txn1->CommitTxn();
    txn2->CommitTxn();

    // start txn3 && txn4
    auto* txn3 = txn_mgr.CreateTxn();
    txn3->BeginTxn();
    auto* txn4 = txn_mgr.CreateTxn();
    txn4->BeginTxn();

    auto read_routine = [&](Txn* txn) {
        EntryResult res;
        for(int i = 0; i < 1000; i++) {
            String db_name = "db" + std::to_string(i);
            res = NewCatalog::GetDatabase(&new_catalog, db_name, txn->TxnID(), txn->BeginTS());
            EXPECT_TRUE(res.Success());
            // only read, don't need lock
            EXPECT_EQ(res.entry_, databases[db_name]);
        }
    };

    std::thread read_thread1(read_routine, txn3);
    std::thread read_thread2(read_routine, txn4);
    read_thread1.join();
    read_thread2.join();

    txn3->CommitTxn();
    txn4->CommitTxn();

    // start txn5 && txn6
    auto* txn5 = txn_mgr.CreateTxn();
    txn5->BeginTxn();
    auto* txn6 = txn_mgr.CreateTxn();
    txn6->BeginTxn();

    auto drop_routine = [&](int start, Txn* txn) {
        EntryResult res;
        for(int i = start; i < 1000; i += 2) {
            String db_name = "db" + std::to_string(i);
            res = txn->DropDatabase(db_name, ConflictType::kError);
            EXPECT_TRUE(res.Success());
            // store this entry
            lock.lock();
            databases.erase(db_name);
            lock.unlock();
        }
    };

    std::thread drop_thread1(drop_routine,0, txn5);
    std::thread drop_thread2(drop_routine,1, txn6);
    drop_thread1.join();
    drop_thread2.join();

    txn5->CommitTxn();
    txn6->CommitTxn();

    // start txn7
    auto* txn7 = txn_mgr.CreateTxn();
    txn7->BeginTxn();

    // check all has been dropped
    EntryResult res;
    for(int i = 0; i < 1000; i++) {
        String db_name = "db" + std::to_string(i);
        res = NewCatalog::GetDatabase(&new_catalog, db_name, txn7->TxnID(), txn7->BeginTS());
        EXPECT_TRUE(res.Fail());
    }
}


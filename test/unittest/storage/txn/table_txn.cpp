//
// Created by jinhai on 23-6-23.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"
#include "storage/txn/txn_manager.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class TableTxnTest : public BaseTest {
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

using namespace infinity;

UniquePtr<TableDesc>
MockTableDesc() {
    return MakeUnique<TableDesc>("tbl1", Vector<SharedPtr<ColumnDef>>());
}

TEST_F(TableTxnTest, test1) {

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create duplicated tbl1, NOT OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(table1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TableTxnTest, test2) {

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn1: Drop tbl1, OK
    dropped_res = new_txn->DropTableByName("db1", "tbl1");
    EXPECT_NE(dropped_res.entry_, nullptr);

    // Txn1: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create duplicated tbl1, NOT OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(table1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TableTxnTest, test3) {

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog);

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Drop tbl1, OK
    dropped_res = new_txn->DropTableByName("db1", "tbl1");
    EXPECT_NE(dropped_res.entry_, nullptr);

    // Txn2: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc());
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get tbl1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

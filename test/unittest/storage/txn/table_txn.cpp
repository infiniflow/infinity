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

UniquePtr<TableDef>
MockTableDesc() {
    return MakeUnique<TableDef>("tbl1", Vector<SharedPtr<ColumnDef>>());
}

TEST_F(TableTxnTest, test1) {

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

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

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

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

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

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

// ----------+------------+---------------+-------------------+----------------------+-----------+-----------------------+->
//           |            |               |                   |                      |           |                       |
//       TXN1 Begin       |      TXN1 Create db1 and tbl1     |                  TXN1 Commit     |                       |
//                    TXN2 Begin                    TXN2 Create tbl1(No DB found)     TXN2 create tbl1(No DB found)    TXN2 Commit
TEST_F(TableTxnTest, test4) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    create1_res = new_txn1->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create tbl1, NOT OK, DB not found
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, NOT OK, DB not found
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ------+------------+---------------+-------------+-----+------+-----------+-----------+-------------+-----------+->
//       |            |               |             |     |      |           |           |             |
//   TXN1 Begin  TXN1 Create db1  TXN1 Commit  TXN2 Begin | TXN2 create tbl1 |      TXN2 Commit        |
//                                                     TXN3 Begin    TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test5) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    create3_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}


// ------+------------+--------------+-----------------+-----------------------+------------------------+------------->
//       |            |              |                 |                       |                        |
//   TXN2 Begin       |      TXN2 Create tbl1          |                       |                TXN2 Commit
//               TXN3 Begin                TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test6) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    create3_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TableTxnTest, test7) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create tbl1, OK
    create2_res = new_txn1->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get db1, NOT OK
    get_res = new_txn2->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn3: Get tbl1, NOT OK
    get_res = new_txn2->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

TEST_F(TableTxnTest, test8) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Get tbl1, NOT OK
    get_res = new_txn3->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Rollback   |                |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)        TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test9) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create tbl1, NOT OK, WW conflict
    create3_res = new_txn3->CreateTable("db1", MockTableDesc());
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn2: Rollback, OK
    new_txn2->RollbackTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Create tbl1, OK
    create3_res = new_txn3->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Drop tbl1  |   TXN2 Commit   |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)      TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test10) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
    NewCatalog new_catalog(std::move(dir), nullptr);
    TxnManager txn_mgr(&new_catalog, &buffer_mgr);

    EntryResult create1_res, create2_res, create3_res, dropped1_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn1->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr.CreateTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Begin, OK
    new_txn3->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Drop tbl1, OK
    dropped1_res = new_txn2->DropTableByName("db1", "tbl1");
    EXPECT_NE(dropped1_res.entry_, nullptr);

    // Txn3: Create tbl1, OK
    create3_res = new_txn3->CreateTable("db1", MockTableDesc());
    EXPECT_NE(create3_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn3: Commit, OK
    new_txn3->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}


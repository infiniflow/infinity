//
// Created by jinhai on 23-6-23.
//

#include "unit_test/base_test.h"

import infinity;
import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;

class TableTxnTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/data/db");
        system("rm -rf /tmp/infinity/data/catalog/*");
        system("rm -rf /tmp/infinity/_tmp");
    }
};

using namespace infinity;

UniquePtr<TableDef> MockTableDesc() {
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    UniquePtr<TableDef> tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
    return MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
}

TEST_F(TableTxnTest, test1) {

    using namespace infinity;
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Create duplicated tbl1, NOT OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(table1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn();
}

TEST_F(TableTxnTest, test2) {

    using namespace infinity;
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn1: Drop tbl1, OK
    dropped_res = new_txn->DropTableCollectionByName("db1", "tbl1", ConflictType::kError);
    EXPECT_NE(dropped_res.entry_, nullptr);

    // Txn1: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Create duplicated tbl1, NOT OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(table1_res.entry_, nullptr);

    // Txn2: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn();
}

TEST_F(TableTxnTest, test3) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, table1_res, create2_res, dropped_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1", ConflictType::kError);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn();

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn();

    // Txn2: Drop tbl1, OK
    dropped_res = new_txn->DropTableCollectionByName("db1", "tbl1", ConflictType::kError);
    EXPECT_NE(dropped_res.entry_, nullptr);

    // Txn2: Create tbl1, OK
    table1_res = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn->CommitTxn();

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->BeginTxn();

    // Txn3: Get tbl1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-----------------------+->
//           |            |               |                   |                      |           |                       |
//       TXN1 Begin       |      TXN1 Create db1 and tbl1     |                  TXN1 Commit     |                       |
//                    TXN2 Begin                    TXN2 Create tbl1(No DB found)     TXN2 create tbl1(No DB found)    TXN2 Commit
TEST_F(TableTxnTest, test4) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, dropped_res, get_res;

    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    create1_res = new_txn1->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create tbl1, NOT OK, DB not found
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create tbl1, NOT OK, DB not found
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();
}

// ------+------------+---------------+-------------+-----+------+-----------+-----------+-------------+-----------+->
//       |            |               |             |     |      |           |           |             |
//   TXN1 Begin  TXN1 Create db1  TXN1 Commit  TXN2 Begin | TXN2 create tbl1 |      TXN2 Commit        |
//                                                     TXN3 Begin    TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test5) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    create3_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

// ------+------------+--------------+-----------------+-----------------------+------------------------+------------->
//       |            |              |                 |                       |                        |
//   TXN2 Begin       |      TXN2 Create tbl1          |                       |                TXN2 Commit
//               TXN3 Begin                TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test6) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    create3_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn();

    // Txn2: Commit, OK
    new_txn2->CommitTxn();
}

TEST_F(TableTxnTest, test7) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn2: Create tbl1, OK
    create2_res = new_txn1->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->RollbackTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn3: Get db1, NOT OK
    get_res = new_txn2->GetDatabase("db1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn3: Get tbl1, NOT OK
    get_res = new_txn2->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();
}

TEST_F(TableTxnTest, test8) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->RollbackTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn3: Get db1, OK
    get_res = new_txn3->GetDatabase("db1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Txn3: Get tbl1, NOT OK
    get_res = new_txn3->GetTableByName("db1", "tbl1");
    EXPECT_EQ(get_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Rollback   |                |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)        TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test9) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    EntryResult create1_res, create2_res, create3_res, dropped_res, get_res;
    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn3: Create tbl1, NOT OK, WW conflict
    create3_res = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_EQ(create3_res.entry_, nullptr);

    // Txn2: Rollback, OK
    new_txn2->RollbackTxn();

    // Txn3: Create tbl1, OK
    create3_res = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create3_res.entry_, nullptr);

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Drop tbl1  |   TXN2 Commit   |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)      TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test10) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    TxnManager *txn_mgr = infinity::Infinity::instance().storage()->txn_manager();

    EntryResult create1_res, create2_res, create3_res, dropped1_res, get_res;

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->BeginTxn();

    // Txn1: Create db1, OK
    create1_res = new_txn1->CreateDatabase("db1", ConflictType::kError);
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn1->CommitTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->BeginTxn();

    // Txn3: Begin, OK
    new_txn3->BeginTxn();

    // Txn2: Create tbl1, OK
    create2_res = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create2_res.entry_, nullptr);

    // Txn2: Drop tbl1, OK
    dropped1_res = new_txn2->DropTableCollectionByName("db1", "tbl1", ConflictType::kError);
    EXPECT_NE(dropped1_res.entry_, nullptr);

    // Txn3: Create tbl1, OK
    create3_res = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError);
    EXPECT_NE(create3_res.entry_, nullptr);

    // Txn2: Commit, OK
    new_txn2->CommitTxn();

    // Txn3: Commit, OK
    new_txn3->CommitTxn();
}

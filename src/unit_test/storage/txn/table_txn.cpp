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
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;
import status;

class TableTxnTest : public BaseTest {
    void SetUp() override {
        system("rm -rf /tmp/infinity");
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
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status status1 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Create duplicated tbl1, NOT OK
    table_entry = nullptr;
    Status status2 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_FALSE(status2.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn2: Get db1, OK
    auto [table_entry1, status3] = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(status3.ok());
    EXPECT_NE(table_entry1, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_F(TableTxnTest, test2) {

    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status status1 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Get db1, OK
    auto [table_entry2, status2] = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(status2.ok());
    EXPECT_NE(table_entry2, nullptr);

    // Txn1: Drop tbl1, OK
    table_entry = nullptr;
    Status status3 = new_txn->DropTableCollectionByName("db1", "tbl1", ConflictType::kError, table_entry);
    EXPECT_TRUE(status3.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Get db1, OK
    auto [table_entry4, status4] = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_FALSE(status4.ok());
    EXPECT_EQ(table_entry4, nullptr);

    // Txn1: Create tbl1, OK
    table_entry = nullptr;
    Status status5 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(status5.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Create duplicated tbl1, NOT OK
    table_entry = nullptr;
    Status status6 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!status6.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn2: Get db1, OK
    auto [table_entry7, status7] = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(status7.ok());
    EXPECT_NE(table_entry7, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

TEST_F(TableTxnTest, test3) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status status1 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(status1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn2: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn->Begin();

    // Txn2: Drop tbl1, OK
    table_entry = nullptr;
    Status status2 = new_txn->DropTableCollectionByName("db1", "tbl1", ConflictType::kError, table_entry);
    EXPECT_TRUE(status2.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Create tbl1, OK
    table_entry = nullptr;
    Status status3 = new_txn->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(status3.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    // Txn3: Create, OK
    new_txn = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn->Begin();

    // Txn3: Get tbl1, OK
    auto [table_entry4, status4] = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(status4.ok());
    EXPECT_NE(table_entry4, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-----------------------+->
//           |            |               |                   |                      |           |                       |
//       TXN1 Begin       |      TXN1 Create db1 and tbl1     |                  TXN1 Commit     |                       |
//                    TXN2 Begin                    TXN2 Create tbl1(No DB found)     TXN2 create tbl1(No DB found)    TXN2 Commit
TEST_F(TableTxnTest, test4) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn1->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Create tbl1, NOT OK, DB not found
    table_entry = nullptr;
    Status s2 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!s2.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create tbl1, NOT OK, DB not found
    table_entry = nullptr;
    Status s3 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!s3.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

// ------+------------+---------------+-------------+-----+------+-----------+-----------+-------------+-----------+->
//       |            |               |             |     |      |           |           |             |
//   TXN1 Begin  TXN1 Create db1  TXN1 Commit  TXN2 Begin | TXN2 create tbl1 |      TXN2 Commit        |
//                                                     TXN3 Begin    TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test5) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    table_entry = nullptr;
    Status s2 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!s2.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

// ------+------------+--------------+-----------------+-----------------------+------------------------+------------->
//       |            |              |                 |                       |                        |
//   TXN2 Begin       |      TXN2 Create tbl1          |                       |                TXN2 Commit
//               TXN3 Begin                TXN3 Create tbl1(WW-Conflict)  TXN3 Commit
TEST_F(TableTxnTest, test6) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn3: Create tbl1, NOT OK，W-W Conflict
    table_entry = nullptr;
    Status s2 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!s2.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

TEST_F(TableTxnTest, test7) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn1->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->RollBackTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn3: Get db1, NOT OK
    BaseEntry* db_entry{nullptr};
    Status s2 = new_txn2->GetDatabase("db1", db_entry);
    EXPECT_TRUE(!s2.ok());
    EXPECT_EQ(db_entry, nullptr);

    // Txn3: Get tbl1, NOT OK
    auto [table_entry3, s3] = new_txn2->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(!s3.ok());
    EXPECT_EQ(table_entry3, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);
}

TEST_F(TableTxnTest, test8) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);
    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->RollBackTxn(new_txn2);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn3: Get db1, OK
    BaseEntry* db_entry{nullptr};
    Status s2 = new_txn3->GetDatabase("db1", db_entry);
    EXPECT_TRUE(s2.ok());
    EXPECT_NE(db_entry, nullptr);

    // Txn3: Get tbl1, NOT OK
    auto [table_entry3, s3] = new_txn3->GetTableByName("db1", "tbl1");
    EXPECT_TRUE(!s3.ok());
    EXPECT_EQ(table_entry3, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+----------------+--->
//           |            |               |                   |                      |           |                |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Rollback   |                |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)        TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test9) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn3: Create tbl1, NOT OK, WW conflict
    table_entry = nullptr;
    Status s2 = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(!s2.ok());
    EXPECT_EQ(table_entry, nullptr);

    // Txn2: Rollback, OK
    txn_mgr->RollBackTxn(new_txn2);

    // Txn3: Create tbl1, OK
    table_entry = nullptr;
    Status s3 = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s3.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

// ----------+------------+---------------+-------------------+----------------------+-----------+-------+---------+--->
//           |            |               |                   |                      |           |       |         |
//       TXN2 Begin       |      TXN2 Create tbl1             |                  TXN2 Drop tbl1  |   TXN2 Commit   |
//                    TXN3 Begin                    TXN3 Create tbl1(WW-Conflict)      TXN3 Create tbl1 OK  TXN3 Commit
TEST_F(TableTxnTest, test10) {
    using namespace infinity;
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();


    // Txn1: Create, OK
    Txn *new_txn1 = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn1->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn1->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_EQ(base_entry->Committed(), false);
    EXPECT_NE(base_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn1);

    // Txn2: Create, OK
    Txn *new_txn2 = txn_mgr->CreateTxn();

    // Txn3: Create, OK
    Txn *new_txn3 = txn_mgr->CreateTxn();

    // Txn2: Begin, OK
    new_txn2->Begin();

    // Txn3: Begin, OK
    new_txn3->Begin();

    // Txn2: Create tbl1, OK
    BaseEntry* table_entry{nullptr};
    Status s1 = new_txn2->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Drop tbl1, OK
    table_entry = nullptr;
    Status s2 = new_txn2->DropTableCollectionByName("db1", "tbl1", ConflictType::kError, table_entry);
    EXPECT_TRUE(s2.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn3: Create tbl1, OK
    table_entry = nullptr;
    Status s3 = new_txn3->CreateTable("db1", MockTableDesc(), ConflictType::kError, table_entry);
    EXPECT_TRUE(s3.ok());
    EXPECT_NE(table_entry, nullptr);

    // Txn2: Commit, OK
    txn_mgr->CommitTxn(new_txn2);

    // Txn3: Commit, OK
    txn_mgr->CommitTxn(new_txn3);
}

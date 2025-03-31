// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import data_block;
import column_vector;
import value;
import new_txn;
import db_meeta;
import table_meeta;
import table_index_meeta;
import column_meta;
import column_def;
import data_type;
import logical_type;
import table_def;
import constant_expr;
import segment_meta;
import block_meta;
import index_secondary;
import internal_types;

using namespace infinity;

class TestLockTable : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestLockTable,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TestLockTable, lock_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // another lock table, conflicts
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and unlock table
        //      t1            lock table        commit
        //      |--------------|-------------------|
        //           |------------|-----------------------|
        //           t2         unlock (fail)            rollback
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop db
        //                         t1            lock table (fail)  commit
        //                         |--------------|-------------------|
        //       |------|-------|
        //       t2    drop   commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // lock table (fail)
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop db
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |------|-------|
        //       t2    drop   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                          t1            lock table    commit (success)
        //                          |--------------|---------------|
        //       |------|-------|
        //       t2    drop   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //          t1                     lock table    commit (success)
        //          |--------------------------|---------------|
        //       |------|----------------|
        //       t2    drop table   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //          t1            lock table (failed) rollback
        //          |--------------|-------------------|
        //       |-------------|-------------|
        //       t2          drop table   commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn10);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //          t1            lock table    commit (success)
        //          |--------------|---------------|
        //       |------------------|----------|
        //       t2           drop table(fail)  rollback
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table txn
        auto *txn10 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        // lock table txn
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // lock table
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // drop table
        status = txn10->DropTable(*db_name, table_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn10);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_lock_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // lock table and lock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //                         |------|--------------------|
        //                        t2    lock table(fail)   rollback (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and unlock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //              |------|-----------------------|
        //              t2    unlock table (fail)   rollback (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and unlock table
        //  t1            lock table     commit (success)
        //  |--------------|----------------|
        //              |------|-----------------------|
        //              t2    unlock table (fail)   rollback (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn4->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_add_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, table_name, columns4);
        EXPECT_TRUE(status.ok());

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn3->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_create_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    auto index_name = std::make_shared<String>("idx1");
    auto index_base = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});
    {
        // lock table and drop table
        //                                             t1            lock table    commit (success)
        //                                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                                 t1            lock table    commit (success)
        //                                 |--------------|---------------|
        //       |----------|-------------------|
        //       t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                       t1        lock table    commit (success)
        //                       |-----------|---------------|
        //       |----------|-------------------|
        //       t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        //        // unlock table
        //        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        //        status = txn5->UnlockTable(*db_name, table_name);
        //        EXPECT_TRUE(status.ok());
        //        status = new_txn_mgr->CommitTxn(txn5);
        //        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //  t1        lock table    commit (success)
        //   |-----------|---------------|
        //       |----------|-------------------|
        //       t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //  t1        lock table    commit (success)
        //   |-----------|---------------|
        //                  |----------|-------------------|
        //                 t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //  t1        lock table    commit (success)
        //   |-----------|---------------|
        //                                 |----------|-------------------|
        //                                t2     create index (fail)    rollback
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_drop_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    auto index_name = std::make_shared<String>("idx1");
    auto index_base = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});
    {
        // lock table and drop table
        //                                             t1            lock table    commit (success)
        //                                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     create index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                                  t1            lock table    commit (success)
        //                                  |--------------|---------------|
        //       |----------|-------------------|
        //       t2     drop index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                     t1            lock table (fail)    rollback
        //                     |--------------|-----------------------|
        //       |----------|-------------------|
        //       t2     drop index     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        //        // unlock table
        //        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        //        status = txn5->UnlockTable(*db_name, table_name);
        //        EXPECT_TRUE(status.ok());
        //        status = new_txn_mgr->CommitTxn(txn5);
        //        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                     t1            lock table           commit (success)
        //                     |--------------|----------------------------|
        //       |--------------------------------|-------------------|
        //       t2                              drop index (fail)  rollback (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //                     t1            lock table           commit (success)
        //                     |--------------|----------------------------|
        //                                        |--------------------------------|-------------------|
        //                                      t2                              drop index (fail)  rollback (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and drop table
        //    t1            lock table           commit (success)
        //    |--------------|----------------------------|
        //                                                     |--------------------------------|-------------------|
        //                                                    t2                              drop index (fail)  rollback (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, table_name, *index_name, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_import) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    auto index_name = std::make_shared<String>("idx1");
    u32 block_row_cnt = 8192;
    auto make_input_block = [&](Value v1, Value v2) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(std::move(v));
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    {
        // lock table and import
        //                                             t1            lock table    commit (success)
        //                                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     import     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                              t1            lock table    commit (success)
        //                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     import     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                        t1            lock table    commit (success)
        //                        |--------------|---------------|
        //       |----------|------------------------|
        //       t2     import             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        //        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        //        status = txn5->UnlockTable(*db_name, table_name);
        //        EXPECT_TRUE(status.ok());
        //        status = new_txn_mgr->CommitTxn(txn5);
        //        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //             t1            lock table    commit (success)
        //             |--------------|---------------|
        //       |------------------------|------------------------|
        //       t2                    import             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                     t1            lock table           commit (success)
        //                     |--------------|----------------------------|
        //                                        |--------------------------------|-------------------|
        //                                      t2                              import             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        // t1            lock table           commit (success)
        // |--------------|----------------------------|
        //                                                   |--------------------------------|-------------------|
        //                                                  t2                              import             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        status = txn3->Import(*db_name, table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_append) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    auto index_name = std::make_shared<String>("idx1");
    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }
    {
        // lock table and import
        //                                             t1            lock table    commit (success)
        //                                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     append     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                              t1            lock table    commit (success)
        //                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     append     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                        t1            lock table    commit (success)
        //                        |--------------|---------------|
        //       |----------|------------------------|
        //       t2     append             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        //        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        //        status = txn5->UnlockTable(*db_name, table_name);
        //        EXPECT_TRUE(status.ok());
        //        status = new_txn_mgr->CommitTxn(txn5);
        //        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //             t1            lock table    commit (success)
        //             |--------------|---------------|
        //       |------------------------|------------------------|
        //       t2                    append             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        //                     t1            lock table           commit (success)
        //                     |--------------|----------------------------|
        //                                        |--------------------------------|-------------------|
        //                                      t2                              append             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    {
        // lock table and import
        // t1            lock table           commit (success)
        // |--------------|----------------------------|
        //                                                   |--------------------------------|-------------------|
        //                                                  t2                              append             commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn4->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        status = txn3->Append(*db_name, table_name, input_block);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestLockTable, lock_table_and_delete) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    String table_name = "tb1";
    auto index_name = std::make_shared<String>("idx1");
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    {
        // lock table and delete
        //                                             t1            lock table    commit (success)
        //                                              |--------------|---------------|
        //       |----------|-------------------|
        //       t2     delete     commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table tb1
        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // lock table txn
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        // lock table
        status = txn7->LockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        check_data();

        // unlock table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn5->UnlockTable(*db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn11->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn11);
        EXPECT_TRUE(status.ok());

        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

//    {
//        // lock table and import
//        //                              t1            lock table    commit (success)
//        //                              |--------------|---------------|
//        //       |----------|-------------------|
//        //       t2     append     commit (success)
//        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn1);
//        EXPECT_TRUE(status.ok());
//
//        // create table tb1
//        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
//        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn2);
//        EXPECT_TRUE(status.ok());
//
//        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
//        status = txn3->Append(*db_name, table_name, input_block);
//        EXPECT_TRUE(status.ok());
//
//        // lock table txn
//        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
//
//        status = new_txn_mgr->CommitTxn(txn3);
//        EXPECT_TRUE(status.ok());
//
//        // lock table
//        status = txn4->LockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn4);
//        EXPECT_TRUE(status.ok());
//
//        // unlock table
//        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
//        status = txn5->UnlockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn5);
//        EXPECT_TRUE(status.ok());
//
//        // drop database
//        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//        status = txn11->DropDatabase("db1", ConflictType::kError);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn11);
//        EXPECT_TRUE(status.ok());
//
//        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
//        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
//    }
//
//    {
//        // lock table and import
//        //                        t1            lock table    commit (success)
//        //                        |--------------|---------------|
//        //       |----------|------------------------|
//        //       t2     append             commit (success)
//        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn1);
//        EXPECT_TRUE(status.ok());
//
//        // create table tb1
//        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
//        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn2);
//        EXPECT_TRUE(status.ok());
//
//        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
//        status = txn3->Append(*db_name, table_name, input_block);
//        EXPECT_TRUE(status.ok());
//
//        // lock table txn
//        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
//        // lock table
//        status = txn4->LockTable(*db_name, table_name);
//        EXPECT_FALSE(status.ok());
//
//        status = new_txn_mgr->CommitTxn(txn3);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->RollBackTxn(txn4);
//        EXPECT_TRUE(status.ok());
//
//        // unlock table
//        //        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
//        //        status = txn5->UnlockTable(*db_name, table_name);
//        //        EXPECT_TRUE(status.ok());
//        //        status = new_txn_mgr->CommitTxn(txn5);
//        //        EXPECT_TRUE(status.ok());
//
//        // drop database
//        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//        status = txn11->DropDatabase("db1", ConflictType::kError);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn11);
//        EXPECT_TRUE(status.ok());
//
//        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
//        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
//    }
//
//    {
//        // lock table and import
//        //             t1            lock table    commit (success)
//        //             |--------------|---------------|
//        //       |------------------------|------------------------|
//        //       t2                    append             commit (success)
//        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn1);
//        EXPECT_TRUE(status.ok());
//
//        // create table tb1
//        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
//        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn2);
//        EXPECT_TRUE(status.ok());
//
//        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
//
//        // lock table txn
//        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
//        // lock table
//        status = txn4->LockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//
//        status = txn3->Append(*db_name, table_name, input_block);
//        EXPECT_FALSE(status.ok());
//
//        status = new_txn_mgr->CommitTxn(txn4);
//        EXPECT_TRUE(status.ok());
//
//        status = new_txn_mgr->RollBackTxn(txn3);
//        EXPECT_TRUE(status.ok());
//
//        // unlock table
//        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
//        status = txn5->UnlockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn5);
//        EXPECT_TRUE(status.ok());
//
//        // drop database
//        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//        status = txn11->DropDatabase("db1", ConflictType::kError);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn11);
//        EXPECT_TRUE(status.ok());
//
//        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
//        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
//    }
//
//    {
//        // lock table and import
//        //                     t1            lock table           commit (success)
//        //                     |--------------|----------------------------|
//        //                                        |--------------------------------|-------------------|
//        //                                      t2                              append             commit (success)
//        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn1);
//        EXPECT_TRUE(status.ok());
//
//        // create table tb1
//        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
//        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn2);
//        EXPECT_TRUE(status.ok());
//
//        // lock table txn
//        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
//        // lock table
//        status = txn4->LockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//
//        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
//
//        status = new_txn_mgr->CommitTxn(txn4);
//        EXPECT_TRUE(status.ok());
//
//        // unlock table
//        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
//        status = txn5->UnlockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn5);
//        EXPECT_TRUE(status.ok());
//
//        status = txn3->Append(*db_name, table_name, input_block);
//        EXPECT_TRUE(status.ok());
//
//        status = new_txn_mgr->CommitTxn(txn3);
//        EXPECT_TRUE(status.ok());
//
//        // drop database
//        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//        status = txn11->DropDatabase("db1", ConflictType::kError);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn11);
//        EXPECT_TRUE(status.ok());
//
//        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
//        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
//    }
//
//    {
//        // lock table and import
//        // t1            lock table           commit (success)
//        // |--------------|----------------------------|
//        //                                                   |--------------------------------|-------------------|
//        //                                                  t2                              append             commit (success)
//        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn1);
//        EXPECT_TRUE(status.ok());
//
//        // create table tb1
//        auto table_def1 = TableDef::Make(db_name, std::make_shared<std::string>(table_name), MakeShared<String>(), {column_def1, column_def2});
//        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//        status = txn2->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn2);
//        EXPECT_TRUE(status.ok());
//
//        // lock table txn
//        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
//        // lock table
//        status = txn4->LockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn4);
//        EXPECT_TRUE(status.ok());
//
//        // unlock table
//        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
//        status = txn5->UnlockTable(*db_name, table_name);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn5);
//        EXPECT_TRUE(status.ok());
//
//        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
//        status = txn3->Append(*db_name, table_name, input_block);
//        EXPECT_TRUE(status.ok());
//
//        status = new_txn_mgr->CommitTxn(txn3);
//        EXPECT_TRUE(status.ok());
//
//        // drop database
//        auto *txn11 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//        status = txn11->DropDatabase("db1", ConflictType::kError);
//        EXPECT_TRUE(status.ok());
//        status = new_txn_mgr->CommitTxn(txn11);
//        EXPECT_TRUE(status.ok());
//
//        //        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
//        //  TODO:      EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
//    }

    new_txn_mgr->PrintAllKeyValue();
}

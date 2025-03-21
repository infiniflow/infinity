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
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import table_def;
import internal_types;
import defer_op;
import statement_common;

using namespace infinity;

class TestIndex : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestIndex,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TestIndex, index_test1) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name = std::make_shared<String>("idx1");
    auto index_base = IndexSecondary::Make(index_name, MakeShared<String>(), "file_name", {column_def1->name()});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // get index def
        auto *txn3_1 = new_txn_mgr->BeginTxn(MakeUnique<String>("get index"), TransactionType::kNormal);
        SharedPtr<IndexBase> index_def1;

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> index_meta;
        Status status = txn3_1->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, index_meta);
        EXPECT_TRUE(status.ok());

        std::tie(index_def1, status) = index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_def1, *index_base);
    }
    {
        // list and drop index
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        Vector<String> index_names;
        Status status = txn4->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());

        for (const auto &index_name : index_names) {
            std::cout << String("Index name: ") << index_name << std::endl;
        }

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestIndex, index_test2) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
    {
        // create fulltext index idx2
        auto index_name2 = std::make_shared<String>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    {
        // create hnsw index idx3
        auto index_name3 = std::make_shared<String>("idx3");
        Vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, MakeShared<String>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestIndex, create_index_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create fulltext index idx2
        auto index_name2 = std::make_shared<String>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create hnsw index idx3
        auto index_name3 = std::make_shared<String>("idx3");
        Vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, MakeShared<String>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
        new_txn_mgr->PrintAllKeyValue();

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Vector<String> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 3);
        for (const auto &index_name : index_names) {
            std::cout << String("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                  t1                     create index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                             t1                     create index   commit (success)
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestIndex, create_index_and_drop_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create fulltext index idx2
        auto index_name2 = std::make_shared<String>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create hnsw index idx3
        auto index_name3 = std::make_shared<String>("idx3");
        Vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, MakeShared<String>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
        new_txn_mgr->PrintAllKeyValue();

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Vector<String> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 3);
        for (const auto &index_name : index_names) {
            std::cout << String("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop table    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop table    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                  t1                     create index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                             t1                     create index   commit (success)
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestIndex, create_index_create_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});
    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                create index(fail)  rollback
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|------------------|
        //                        t2                create index      commit (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |------------------|------------------|
        //        t2                create index      commit (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index               commit (fail)
        //  |--------------|--------------------------------|
        //         |------------------|------------------|
        //        t2                create index      commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestIndex, create_index_add_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                add column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                        |------------------|------------------|
        //                       t2                add column       commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                        |------|------------------|
        //                       t2    add column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index             commit (success)
        //  |--------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1               create index             commit (fail)
        //  |-------------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1                                create index             commit (fail)
        //  |-------------------------------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestIndex, create_index_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                          |------------------|------------------|
        //                         t2                drop column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1                       create index   commit (success)
        //  |----------------------------|---------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column (fail)     rollback
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
    //
    //    {
    //        // create index and create index
    //        //  t1            create index             commit (success)
    //        //  |--------------|----------------------------|
    //        //       |-----------|------------------|
    //        //       t2       add column          commit
    //        // create db1
    //        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    //        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn1);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create table
    //        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
    //        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn2);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create index idx1
    //        auto index_name1 = std::make_shared<String>("idx1");
    //
    //        // add columns
    //        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
    //
    //        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
    //        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
    //        EXPECT_TRUE(status.ok());
    //
    //        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3",
    //        std::set<ConstraintType>()); auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar),
    //        "col4", std::set<ConstraintType>()); Vector<SharedPtr<ColumnDef>> columns; columns.emplace_back(column_def3);
    //        columns.emplace_back(column_def4);
    //        status = txn4->AddColumns(*db_name, *table_name, columns);
    //        EXPECT_FALSE(status.ok());
    //
    //        status = new_txn_mgr->CommitTxn(txn3);
    //        EXPECT_TRUE(status.ok());
    //
    //        status = new_txn_mgr->RollBackTxn(txn4);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop index
    //        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
    //        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn6);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop table
    //        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
    //        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn8);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop database
    //        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
    //        status = txn7->DropDatabase("db1", ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn7);
    //        EXPECT_TRUE(status.ok());
    //    }
    //
    //    {
    //        // create index and create index
    //        //  t1               create index             commit (fail)
    //        //  |-------------------|----------------------------|
    //        //       |-----------|------------------|
    //        //       t2       add column          commit (success)
    //        // create db1
    //        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    //        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn1);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create table
    //        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
    //        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn2);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create index idx1
    //        auto index_name1 = std::make_shared<String>("idx1");
    //
    //        // add columns
    //        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
    //
    //        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
    //
    //        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3",
    //        std::set<ConstraintType>()); auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar),
    //        "col4", std::set<ConstraintType>()); Vector<SharedPtr<ColumnDef>> columns; columns.emplace_back(column_def3);
    //        columns.emplace_back(column_def4);
    //        status = txn4->AddColumns(*db_name, *table_name, columns);
    //        EXPECT_TRUE(status.ok());
    //
    //        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
    //        EXPECT_FALSE(status.ok());
    //
    //        status = new_txn_mgr->CommitTxn(txn4);
    //        EXPECT_TRUE(status.ok());
    //
    //        status = new_txn_mgr->RollBackTxn(txn3);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop table
    //        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
    //        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn8);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop database
    //        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
    //        status = txn7->DropDatabase("db1", ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn7);
    //        EXPECT_TRUE(status.ok());
    //    }
    //
    //    {
    //        // create index and create index
    //        //  t1                                create index             commit (fail)
    //        //  |-------------------------------------|----------------------------|
    //        //       |-----------|------------------|
    //        //       t2       add column          commit (success)
    //        // create db1
    //        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    //        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn1);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create table
    //        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
    //        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn2);
    //        EXPECT_TRUE(status.ok());
    //
    //        // create index idx1
    //        auto index_name1 = std::make_shared<String>("idx1");
    //
    //        // add columns
    //        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
    //
    //        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
    //
    //        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3",
    //        std::set<ConstraintType>()); auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar),
    //        "col4", std::set<ConstraintType>()); Vector<SharedPtr<ColumnDef>> columns; columns.emplace_back(column_def3);
    //        columns.emplace_back(column_def4);
    //        status = txn4->AddColumns(*db_name, *table_name, columns);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn4);
    //        EXPECT_TRUE(status.ok());
    //
    //        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn3);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop index
    //        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
    //        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn6);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop table
    //        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
    //        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn8);
    //        EXPECT_TRUE(status.ok());
    //
    //        // drop database
    //        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
    //        status = txn7->DropDatabase("db1", ConflictType::kError);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn7);
    //        EXPECT_TRUE(status.ok());
    //    }

    new_txn_mgr->PrintAllKeyValue();
}

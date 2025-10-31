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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.index;

import :ut.base_test;

import third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :data_block;
import :column_vector;
import :value;
import :new_txn;
import :db_meta;
import :table_meta;
import :table_index_meta;
import :column_meta;
import :index_base;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_bmp;
import :table_def;
import :defer_op;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import embedding_info;
import sparse_info;
import internal_types;
import statement_common;

using namespace infinity;

class TestTxnIndex : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnIndex,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnIndex, index_test1) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_name = std::make_shared<std::string>("idx1");
    auto index_base = IndexSecondary::Make(index_name, std::make_shared<std::string>(), "file_name", {column_def1->name()});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create index idx1
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // get index def
        auto *txn3_1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get index"), TransactionType::kRead);
        std::shared_ptr<IndexBase> index_def1;

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn3_1->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        std::tie(index_def1, status) = index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_def1, *index_base);
    }
    {
        // list and drop index
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        std::vector<std::string> index_names;
        Status status = txn4->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());

        for (const auto &index_name : index_names) {
            std::cout << std::string("Index name: ") << index_name << std::endl;
        }

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kDropDB);
        Status status = txn5->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnIndex, index_test2) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }
    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
    {
        // create fulltext index idx2
        auto index_name2 = std::make_shared<std::string>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
    {
        // create hnsw index idx3
        auto index_name3 = std::make_shared<std::string>("idx3");
        std::vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, std::make_shared<std::string>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    {
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnIndex, createindex_wrong_conflicttype) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_name = std::make_shared<std::string>("idx1");

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kIgnore, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        auto index_def1 = IndexSecondary::Make(index_name, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kInvalid);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kReplace);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnIndex, dropindex_wrong_conflicttype) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_name = std::make_shared<std::string>("idx1");

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    {
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kIgnore, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        auto index_def1 = IndexSecondary::Make(index_name, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn4->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kInvalid);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn4->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kReplace);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnIndex, create_index_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create fulltext index idx2
        auto index_name2 = std::make_shared<std::string>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create hnsw index idx3
        auto index_name3 = std::make_shared<std::string>("idx3");
        std::vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, std::make_shared<std::string>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
        new_txn_mgr->PrintAllKeyValue();

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        std::vector<std::string> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 3);
        for (const auto &index_name : index_names) {
            std::cout << std::string("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

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
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

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
        //  t1            create index   commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                  t1                     create index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                             t1                     create index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, create_index_and_drop_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop db
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create fulltext index idx2
        auto index_name2 = std::make_shared<std::string>("idx2");
        auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn4->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create hnsw index idx3
        auto index_name3 = std::make_shared<std::string>("idx3");
        std::vector<InitParameter *> index_param_list;
        DeferFn defer([&index_param_list]() {
            for (auto *param : index_param_list) {
                delete param;
            }
        });
        index_param_list.push_back(new InitParameter("metric", "l2"));
        auto index_def3 = IndexHnsw::Make(index_name3, std::make_shared<std::string>(), "file_name", {column_def3->name()}, index_param_list);
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn5->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
        new_txn_mgr->PrintAllKeyValue();

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        std::vector<std::string> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 3);
        for (const auto &index_name : index_names) {
            std::cout << std::string("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            create index   commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                  t1                     create index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, SLOW_create_index_create_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                create index(fail)  rollback
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t3            create index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|
        //                        t4                create index (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |------------------|
        //        t2                create index (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and create index
        //  t1         create index           commit (success)
        //  |--------------|-------------------------|
        //         |------------------|
        //        t2            create index (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, create_index_add_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // create index and add column
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                add column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and add column
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                        |------------------|------------------|
        //                       t2                add column       commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and add column
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                        |------|------------------|
        //                       t2    add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and add column
        //  t1            create index             commit (success)
        //  |--------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and add column
        //  t1               create index             commit (success)
        //  |-------------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and add column
        //  t1                                create index             commit (fail)
        //  |-------------------------------------|----------------------------|
        //       |-----------|------------------|
        //       t2       add column          commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");

        // add columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
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
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, create_index_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // create index and drop column
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop column          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and drop column
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                          |------------------|------------------|
        //                         t2                drop column          commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and drop column
        //  t1                       create index   commit (success)
        //  |----------------------------|---------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column        commit (fail)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and drop column
        //  t1                       create index               commit (fail)
        //  |----------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column      commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok()); // column isn't existed, can't find the index
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and drop column
        //  t1                                 create index                    commit (fail)
        //  |---------------------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column       commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and drop column
        //                    t1                                 create index                      commit (fail)
        //                     |---------------------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column       commit (success)
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok()); // column isn't existed, can't find the index
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, create_index_rename_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto new_table_name = std::make_shared<std::string>("tb2");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // create index and rename table
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                rename          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                           |------------------|------------------|
        //                          t2                rename          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //               |---------------------|------------------|
        //              t2                 rename          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //               |---------------|------------------|
        //              t2              rename          commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //  t1                 create index   commit (success)
        //  |--------------------|---------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //  t1                 create index        commit (success)
        //  |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //            t1                 create index        commit (success)
        //             |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // create index and rename table
        //                                t1                 create index        commit (success)
        //                                |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *new_table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, drop_index_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop db
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        std::vector<std::string> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 0);
        for (const auto &index_name : index_names) {
            std::cout << std::string("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {

        // create index and drop db
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop db    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop db    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //  t1            drop index   commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                  t1                     drop index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }

    {

        // create index and drop db
        //                             t1                     drop index (fail)  rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
}

TEST_P(TestTxnIndex, drop_index_and_drop_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // drop index and drop table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table  commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // List index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        std::vector<std::string> index_names;
        status = txn6->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_names.size(), 0);
        for (const auto &index_name : index_names) {
            std::cout << std::string("Index name: ") << index_name << std::endl;
        }
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {

        // drop index and drop table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop table    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop table    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop table
        //  t1            drop index   commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop table
        //                  t1                     drop index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table  commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {

        // drop index and drop table
        //                             t1                     drop index (fail)  rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create secondary index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn4->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, create_index_and_drop_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // create index and drop index
        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop index       commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        //                        t2                drop index (fail)  rollback
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
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
        //        t2                drop index      commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, drop_index_and_drop_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(
        2,
        std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128)),
        "col3",
        std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});
    {
        // drop index and drop index
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop index (fail)  rollback
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn9 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn9->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn9);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop index
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|------------------|
        //                        t2                drop index         commit(fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn9 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn9->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn9);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop index
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //         |------------------|------------------|
        //        t2                drop index      commit (fail)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop index
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn9 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn9->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn9);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, drop_index_add_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // add column and drop index
        //  t1            add column   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop index          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop index
        //  t1            add column   commit (success)
        //  |--------------|---------------|
        //                        |------------------|------------------|
        //                       t2                drop index      commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop index
        //  t1            add column   commit (success)
        //  |--------------|---------------|
        //                        |------|------------------|
        //                       t2    drop index        commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop index
        //  t1            add column             commit (success)
        //  |--------------|----------------------------|
        //       |-----------|------------------|
        //       t2       drop index        commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop index
        //  t1               add column               commit (success)
        //  |-------------------|----------------------------|
        //       |-----------|------------------|
        //       t2      drop index          commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // add column and drop index
        //  t1                                add column             commit (fail)
        //  |-------------------------------------|----------------------------|
        //       |-----------|------------------|
        //       t2       drop index          commit (success)
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // add columns
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
        auto column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>());
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, drop_index_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // drop index and drop column
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                drop column          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop column
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                          |------------------|------------------|
        //                         t2                drop column          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop column
        //  t1                       drop index   commit (success)
        //  |----------------------------|---------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column (fail)     rollback
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop column
        //  t1                       drop index               commit (success)
        //  |----------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column (fail)     rollback
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop column
        //  t1                                 drop index                     commit
        //  |---------------------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column (fail)    commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and drop column
        //                    t1                                 drop index                      commit
        //                     |---------------------------------------|-----------------------------|
        //         |--------------------------|---------------------|
        //        t2                        drop column       commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnIndex, drop_index_and_rename_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto new_table_name = std::make_shared<std::string>("tb2");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        // drop index and rename table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                rename          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                           |------------------|------------------|
        //                          t2                rename          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //               |---------------------|------------------|
        //              t2                 rename          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //               |---------------|------------------|
        //              t2              rename          commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //  t1                 drop index   commit (success)
        //  |--------------------|---------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //  t1                 drop index        commit (success)
        //  |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //            t1                 drop index        commit (success)
        //             |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop index and rename table
        //                                t1                 drop index        commit (success)
        //                                |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn3->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn8 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn8->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();
}

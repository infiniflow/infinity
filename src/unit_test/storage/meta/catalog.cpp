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

#include "type/complex/embedding_type.h"
#include "gtest/gtest.h"
import base_test;

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import value;

import data_block;
import default_values;
import txn_manager;
import buffer_manager;
import txn;
import catalog;
import status;
import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import embedding_info;
import index_hnsw;
import statement_common;
import data_access_state;
import txn_store;

import base_entry;

using namespace infinity;

class CatalogTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CatalogTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

// txn1: create db1, get db1, delete db1, get db1, commit
// txn2:             get db1,             get db1, commit
TEST_P(CatalogTest, simple_test1) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // start txn2
    auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("get db"));

    HashMap<String, BaseEntry *> databases;

    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        // store this entry
        databases["db1"] = base_entry;
    }

    {
        auto [db_entry1, status1] = catalog->GetDatabase("db1", txn1->TxnID(), txn1->BeginTS());
        // should be visible to same txn
        EXPECT_TRUE(status1.ok());
        EXPECT_EQ(db_entry1, databases["db1"]);

        // should not be visible to other txn
        auto [db_entry2, status2] = catalog->GetDatabase("db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(!status2.ok());
        EXPECT_EQ(db_entry2, nullptr);
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(base_entry.get(), databases["db1"]);
        // remove this entry
        databases.erase("db1");

        auto [db_entry1, status1] = catalog->GetDatabase("db1", txn1->TxnID(), txn1->BeginTS());
        // should not be visible to same txn
        EXPECT_TRUE(!status1.ok());

        // should not be visible to other txn
        auto [db_entry2, status2] = catalog->GetDatabase("db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(!status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
    txn_mgr->CommitTxn(txn2);
}

// txn1: create db1, commit.
// txn2: start,              get db1, commit
// txn3:                     start, get db1, delete db1, commit
TEST_P(CatalogTest, simple_test2) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // start txn2
    auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("get db"));

    // create db in empty catalog should be success
    {
        Status status = txn1->CreateDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
    }

    u64 txn1_id = txn1->TxnID();
    TxnTimeStamp txn1_begin_ts = txn1->BeginTS();
    txn_mgr->CommitTxn(txn1);

    // should not be visible to txn2
    {
        auto [db_entry, status1] = catalog->GetDatabase("db1", txn1_id, txn1_begin_ts);
        // should not be visible to same txn
        EXPECT_TRUE(!status1.ok());
    }

    txn_mgr->CommitTxn(txn2);

    auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));

    // should be visible to txn3
    {
        auto [db_entry1, status1] = catalog->GetDatabase("db1", txn3->TxnID(), txn3->BeginTS());
        EXPECT_TRUE(status1.ok());
        EXPECT_NE(db_entry1, nullptr);

        Status status = txn3->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // should not be visible to other txn
        auto [db_entry2, status2] = catalog->Catalog::GetDatabase("db1", txn3->TxnID(), txn3->BeginTS());
        EXPECT_TRUE(!status2.ok());
    }

    txn_mgr->CommitTxn(txn3);
}

TEST_P(CatalogTest, concurrent_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    for (int loop = 0; loop < 1; ++loop) {
        // start txn1 && txn2
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));
        auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

        auto write_routine = [&](int start, Txn *txn) {
            for (int db_id = start; db_id < 1000; db_id += 2) {
                String db_name = "db" + std::to_string(db_id);
                Status status = txn->CreateDatabase(db_name, ConflictType::kError);
                EXPECT_TRUE(status.ok());
            }
        };

        Thread write_thread1(write_routine, 0, txn1);
        Thread write_thread2(write_routine, 1, txn2);

        write_thread1.join();
        write_thread2.join();

        txn_mgr->CommitTxn(txn1);
        txn_mgr->CommitTxn(txn2);

        // start txn3 && txn4
        auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db"));
        auto *txn4 = txn_mgr->BeginTxn(MakeUnique<String>("get db"));

        auto read_routine = [&](Txn *txn) {
            for (int db_id = 0; db_id < 1000; ++db_id) {
                String db_name = "db" + std::to_string(db_id);
                auto [db_entry, status] = catalog->Catalog::GetDatabase(db_name, txn->TxnID(), txn->BeginTS());
                EXPECT_TRUE(status.ok());
                // only read, don't need lock
                EXPECT_NE(db_entry, nullptr);
            }
        };

        std::thread read_thread1(read_routine, txn3);
        std::thread read_thread2(read_routine, txn4);
        read_thread1.join();
        read_thread2.join();

        txn_mgr->CommitTxn(txn3);
        txn_mgr->CommitTxn(txn4);

        // start txn5 && txn6
        auto *txn5 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));
        auto *txn6 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));

        auto drop_routine = [&](int start, Txn *txn) {
            for (int db_id = start; db_id < 1000; db_id += 2) {
                String db_name = "db" + std::to_string(db_id);
                Status status = txn->DropDatabase(db_name, ConflictType::kError);
                EXPECT_TRUE(status.ok());
            }
        };

        std::thread drop_thread1(drop_routine, 0, txn5);
        std::thread drop_thread2(drop_routine, 1, txn6);
        drop_thread1.join();
        drop_thread2.join();

        txn_mgr->CommitTxn(txn5);
        txn_mgr->CommitTxn(txn6);

        // start txn7
        auto *txn7 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"));

        // check all has been dropped
        for (int db_id = 0; db_id < 1000; ++db_id) {
            String db_name = "db" + std::to_string(db_id);
            auto [db_entry, status] = catalog->Catalog::GetDatabase(db_name, txn7->TxnID(), txn7->BeginTS());
            EXPECT_TRUE(!status.ok());
        }
        txn_mgr->CommitTxn(txn7);
    }
}

// txn1: create db1, get db1 info, delete db1, get db1, commit
// txn2:             get db1 info,             get db1, commit
TEST_P(CatalogTest, get_db_info_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"));

    // start txn2
    auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("get db"));


    // create db in empty catalog should be success
    {
        auto [base_entry, status] = catalog->CreateDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    {
        auto [db_info1, status1] = catalog->GetDatabaseInfo("db1", txn1->TxnID(), txn1->BeginTS());
        // should be visible to same txn
        EXPECT_TRUE(status1.ok());
        std::cout<<*(db_info1->db_name_)<<std::endl;
        EXPECT_STREQ("db1", db_info1->db_name_->c_str());

        // should not be visible to other txn
        auto [db_info2, status2] = catalog->GetDatabaseInfo("db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(!status2.ok());
    }

    // drop db should be success
    {
        auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());

        auto [db_info1, status1] = catalog->GetDatabaseInfo("db1", txn1->TxnID(), txn1->BeginTS());
        // should not be visible to same txn
        EXPECT_TRUE(!status1.ok());

        // should not be visible to other txn
        auto [db_info2, status2] = catalog->GetDatabaseInfo("db1", txn2->TxnID(), txn2->BeginTS());
        EXPECT_TRUE(!status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
    txn_mgr->CommitTxn(txn2);
}

TEST_P(CatalogTest, get_table_info_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

    //create table, get table info, drop table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());

        auto [table_info, status1] = catalog->GetTableInfo("default_db", "tbl1", txn1);
        EXPECT_TRUE(status1.ok());
        EXPECT_STREQ("tbl1", table_info->table_name_->c_str());

        auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(CatalogTest, get_table_index_info_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));

    //create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    // CreateIndex
    {
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        parameters1.emplace_back(new InitParameter("metric", "l2"));
        parameters1.emplace_back(new InitParameter("encode", "plain"));
        parameters1.emplace_back(new InitParameter("m", "16"));
        parameters1.emplace_back(new InitParameter("ef_construction", "200"));

        SharedPtr<String> index_name = MakeShared<String>("hnsw_index");
        auto index_base_hnsw = IndexHnsw::Make(index_name, "hnsw_index_test_hnsw", columns1, parameters1);
        for (auto *init_parameter : parameters1) {
            delete init_parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        ConflictType conflict_type = ConflictType::kError;
        auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
        EXPECT_EQ(table_status.ok(), true);
        auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_EQ(index_status.ok(), true);
    }

    //get index info
    {
        auto [index_info, status] = catalog->GetTableIndexInfo("default_db", "tbl1", "hnsw_index", txn1->TxnID(), txn1->BeginTS());
        EXPECT_TRUE(status.ok());
        EXPECT_STREQ("hnsw_index", index_info->index_name_->c_str());
    }

    //drop index
    {
        auto [index_entry, status] = catalog->DropIndex("default_db", "tbl1", "hnsw_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    //drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(CatalogTest, remove_index_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));

    //create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    // CreateIndex
    {
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        parameters1.emplace_back(new InitParameter("metric", "l2"));
        parameters1.emplace_back(new InitParameter("encode", "plain"));
        parameters1.emplace_back(new InitParameter("m", "16"));
        parameters1.emplace_back(new InitParameter("ef_construction", "200"));

        SharedPtr<String> index_name = MakeShared<String>("hnsw_index");
        auto index_base_hnsw = IndexHnsw::Make(index_name, "hnsw_index_test_hnsw", columns1, parameters1);
        for (auto *init_parameter : parameters1) {
            delete init_parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        ConflictType conflict_type = ConflictType::kError;
        auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
        EXPECT_EQ(table_status.ok(), true);
        auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_EQ(index_status.ok(), true);
    }

    //remove index
    {
        auto [index_entry, index_status] = catalog->GetIndexByName("default_db", "tbl1", "hnsw_index", txn1->TxnID(), txn1->BeginTS());
        EXPECT_TRUE(index_status.ok());
        auto status = catalog->RemoveIndexEntry(index_entry, txn1->TxnID());
        EXPECT_TRUE(status.ok());
    }

    //drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(CatalogTest, roll_back_append_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

    //create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    {
        auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
        EXPECT_TRUE(status.ok());


        auto txn_store = txn1->GetTxnTableStore(table_entry);
        txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
        Catalog::Append(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), buffer_mgr);
        EXPECT_THROW(Catalog::RollbackAppend(table_entry, txn1->TxnID(), txn1->CommitTS(), (void *)txn_store), UnrecoverableException);
    }
}


TEST_P(CatalogTest, roll_back_delete_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

    //create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    {
        auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
        EXPECT_TRUE(status.ok());

        auto txn_store = txn1->GetTxnTableStore(table_entry);
        Catalog::Delete(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), txn_store->GetDeleteStateRef());
        EXPECT_THROW(Catalog::RollbackDelete(table_entry, txn1->TxnID(), txn_store->GetDeleteStateRef(), buffer_mgr),UnrecoverableException);
    }
}

TEST_P(CatalogTest, roll_back_write_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

    //create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    {
        auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
        EXPECT_TRUE(status.ok());


        auto txn_store = txn1->GetTxnTableStore(table_entry);
        txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
        Catalog::Append(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), buffer_mgr);
        Catalog::CommitWrite(table_entry, txn1->TxnID(), txn1->CommitTS(), txn_store->txn_segments(), nullptr);
        Vector<TxnSegmentStore> segment_stores;
        auto status1 = Catalog::RollbackWrite(table_entry, txn1->CommitTS(), segment_stores);
        EXPECT_TRUE(status1.ok());
    }
}
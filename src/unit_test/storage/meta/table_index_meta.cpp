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

class TableIndexMetaTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TableIndexMetaTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TableIndexMetaTest, get_all_entries_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));

    // create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] =
            catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
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
        auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), "hnsw_index_test_hnsw", columns1, parameters1);
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

        EXPECT_EQ(index_entry->table_index_meta()->GetAllEntries().size(), 1);
        EXPECT_EQ(index_entry->table_index_meta()->EntryCount(), 1);
        EXPECT_FALSE(index_entry->table_index_meta()->Empty());
    }

    // drop index
    {
        auto [index_entry, status] =
            catalog->DropIndex("default_db", "tbl1", "hnsw_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_entry->table_index_meta()->GetAllEntries().size(), 0);
        EXPECT_EQ(index_entry->table_index_meta()->EntryCount(), 0);
        EXPECT_TRUE(index_entry->table_index_meta()->Empty());
    }

    // drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(TableIndexMetaTest, table_entry_test) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();

    // start txn1
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));

    // create table
    {
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
        auto [table_entry, status] =
            catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
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
        auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), "hnsw_index_test_hnsw", columns1, parameters1);
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

        auto table_entry_ptr = index_entry->table_index_meta()->table_entry();
        EXPECT_EQ(table_entry_ptr, table_entry);
    }

    // drop index
    {
        auto [index_entry, status] =
            catalog->DropIndex("default_db", "tbl1", "hnsw_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    // drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

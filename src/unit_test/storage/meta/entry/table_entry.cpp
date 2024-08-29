#include "unit_test/base_test.h"
#include "type/complex/embedding_type.h"
#include <regex>

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
import index_full_text;
import statement_common;
import data_access_state;
import txn_store;

import base_entry;
import table_entry;

using namespace infinity;

class TableEntryTest : public BaseTestParamStr {
    void SetUp() override {
        BaseTestParamStr::SetUp();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        RemoveDbDirs();
        system(("mkdir -p " + infinity::String(GetFullPersistDir())).c_str());
        system(("mkdir -p " + infinity::String(GetFullDataDir())).c_str());
        system(("mkdir -p " + infinity::String(GetFullTmpDir())).c_str());
        std::string config_path_str = GetParam();
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTestParamStr::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TableEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH,
                                           BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(TableEntryTest, decode_index_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog(); 
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("get table"));


    //create table, drop table
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
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());

        String encode_index = TableEntry::EncodeIndex("tbl1",  table_entry->GetTableMeta());
        EXPECT_TRUE(TableEntry::DecodeIndex(encode_index)[0] == "default_db");
        EXPECT_THROW(TableEntry::DecodeIndex("/default_db/tbl1"), UnrecoverableException);

        auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(TableEntryTest,  create_no_name_index_test){
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
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
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

        SharedPtr<String> index_name = MakeShared<String>("");
        auto index_base_hnsw = IndexHnsw::Make(index_name, "hnsw_index_test_hnsw", columns1, parameters1);
        for (auto *init_parameter : parameters1) {
            delete init_parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        ConflictType conflict_type = ConflictType::kError;
        auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
        EXPECT_EQ(table_status.ok(), true);
        EXPECT_THROW(table_entry->CreateIndex(index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(), txn_mgr), 
                    UnrecoverableException);
    }

    //drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(TableEntryTest,  remove_index_test){
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
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    // CreateIndex, remove index
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

        table_entry->RemoveIndexEntry("hnsw_index",txn1->TxnID());
    }

    //drop index
    {
        auto [index_entry, status] = catalog->DropIndex("default_db", "tbl1", "hnsw_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_FALSE(status.ok());
        EXPECT_STREQ(status.message() ,"Not existed entry.");
    }

    //drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(TableEntryTest,  table_indexes_test){
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
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
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

        EXPECT_EQ(table_entry->TableIndexes(txn1->TxnID(), txn1->BeginTS()).size(), 0);

        auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_EQ(index_status.ok(), true);

        EXPECT_EQ(table_entry->TableIndexes(txn1->TxnID(), txn1->BeginTS()).size(), 1);
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

        EXPECT_EQ(table_entry->TableIndexes(txn1->TxnID(), txn1->BeginTS()).size(), 0);
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(TableEntryTest,  get_fulltext_analyzers_test){
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
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kVarchar), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto [table_entry, status] = catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    // CreateIndex
    {
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        SharedPtr<String> index_name = MakeShared<String>("fulltext_index");
        auto index_base_fulltext = IndexFullText::Make(index_name, "fulltext_index_test_fulltext", columns1, parameters1);

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        ConflictType conflict_type = ConflictType::kError;
        auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_fulltext, conflict_type, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_EQ(index_status.ok(), true);

        Map<String, String> column2analyzer;
        table_entry->GetFulltextAnalyzers(txn1->TxnID(), txn1->BeginTS(),column2analyzer);
        for(auto pair : column2analyzer){
            EXPECT_STREQ(pair.first.c_str(), "col1");
            EXPECT_STREQ(pair.second.c_str(), "standard");
        }
    }

    //drop index
    {
        auto [index_entry, status] = catalog->DropIndex("default_db", "tbl1", "fulltext_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    //drop table
    {
        auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status.ok());
    }

    txn_mgr->CommitTxn(txn1);
}
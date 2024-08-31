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
import statement_common;
import data_access_state;
import txn_store;

import base_entry;
import db_entry;

using namespace infinity;

class DBEntryTest : public BaseTestParamStr {
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
                         DBEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(DBEntryTest, decode_index_test){
    String db_name = "untitled_db";
    String encode_ed_name = DBEntry::EncodeIndex(db_name);
    EXPECT_TRUE(DBEntry::DecodeIndex(encode_ed_name)[0] == "untitled_db");

    EXPECT_THROW(DBEntry::DecodeIndex(""), UnrecoverableException);
    EXPECT_THROW(DBEntry::DecodeIndex(db_name), UnrecoverableException);
}

TEST_P(DBEntryTest, to_string_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog(); 
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("get database"));

    auto [db_entry, status] = catalog->GetDatabase("default_db", txn1->TxnID(), txn1->BeginTS());
    std::cout << *(db_entry->ToString()) << std::endl;
    EXPECT_TRUE(std::regex_match(*(db_entry->ToString()), 
                    std::regex("DBEntry,\\sdb_entry_dir:\\s/var/infinity/data/(.*)_db_default_db,\\stxn\\sid:\\s0,\\stable\\scount:\\s0"))); 

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

        auto [db_entry, status3] = catalog->GetDatabase("default_db", txn1->TxnID(), txn1->BeginTS());
        std::cout << *(db_entry->ToString()) << std::endl;
        EXPECT_TRUE(std::regex_match(*(db_entry->ToString()), 
                        std::regex("DBEntry,\\sdb_entry_dir:\\s/var/infinity/data/(.*)_db_default_db,\\stxn\\sid:\\s0,\\stable\\scount:\\s1"))); 

        auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(DBEntryTest, get_all_table_metas_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog(); 
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("get database"));


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

        auto [db_entry, status3] = catalog->GetDatabase("default_db", txn1->TxnID(), txn1->BeginTS());
        auto [meta_name_array, meta_ptr_array, r_lock] = db_entry->GetAllTableMetas();
        EXPECT_EQ(meta_name_array.size(), 1);
        EXPECT_STREQ(meta_name_array[0].c_str(), "tbl1");

        auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_TRUE(status2.ok());
    }

    txn_mgr->CommitTxn(txn1);
}

TEST_P(DBEntryTest, remove_table_entry_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog(); 
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("get database"));


    //create table, remove table entry, drop table
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

        auto [db_entry, status3] = catalog->GetDatabase("default_db", txn1->TxnID(), txn1->BeginTS());
        db_entry->RemoveTableEntry("tbl1", txn1->TxnID());

        auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
        EXPECT_FALSE(status2.ok());
        EXPECT_STREQ(status2.message() ,"Not existed entry.");
    }

    txn_mgr->CommitTxn(txn1);
}
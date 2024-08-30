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
import bitmask;

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
import sparse_info;
import index_hnsw;
import index_full_text;
import index_ivfflat;
import index_bmp;
import statement_common;
import data_access_state;
import txn_store;
import column_vector;
import internal_types;
import constant_expr;
import parsed_expr;
import base_table_ref;
import index_base;
import chunk_index_entry;

import base_entry;
import table_entry;
import table_entry_type;
import segment_entry;
import segment_index_entry;
import block_entry;

using namespace infinity;

class SegmentIndexEntryTest : public BaseTestParamStr {
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
                         SegmentIndexEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH,
                                           BaseTestParamStr::VFS_CONFIG_PATH));

void CreateTable();
void CreateIndex();
void InsertData(const String& db_name, const String& table_name);
void DropIndex();
void DropTable();

TEST_P(SegmentIndexEntryTest, decode_index_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    CreateIndex();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("decode"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_FALSE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//get
        String encoded_index =  SegmentIndexEntry::EncodeIndex(0, index_entry);
        std::cout<<encoded_index<<std::endl;
        EXPECT_TRUE(encoded_index == "#default_db#tbl1#fulltext_index#0");
        Vector<std::string_view> decoded_index  = SegmentIndexEntry::DecodeIndex(encoded_index);
        EXPECT_TRUE(decoded_index[0] == "default_db");
        EXPECT_TRUE(decoded_index[1] == "tbl1");
        EXPECT_TRUE(decoded_index[2] == "fulltext_index");
        EXPECT_TRUE(decoded_index[3] == "0");
        EXPECT_THROW(SegmentEntry::DecodeIndex("/default_db/tbl1/fulltext_index/0"), UnrecoverableException);
    }

    DropIndex();
    DropTable();
}

TEST_P(SegmentIndexEntryTest, create_ivfflat_index_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns{"col1"};
        Vector<InitParameter *> parameters;
        parameters.emplace_back(new InitParameter("centroids_count", "100"));
        parameters.emplace_back(new InitParameter("metric", "l2"));

        auto index_base = IndexIVFFlat::Make(MakeShared<String>("idx1"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;
        for (auto parameter : parameters) {
            delete parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);
        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn1);
        auto [_, status3] = txn1->CreateIndexPrepare(index_entry, table_ref.get(), true, true);
        txn1->CreateIndexFinish(table_entry, index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn1); 
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create_file_worker"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "idx1";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//create
        txn_mgr->CommitTxn(txn1);
    }

    DropTable();
}

TEST_P(SegmentIndexEntryTest, index_file_name_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    CreateIndex();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("name"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//create
        String index_file_name = segment_index_entry->IndexFileName(0);
        std::cout<<index_file_name<<std::endl;
        EXPECT_STREQ(index_file_name.c_str(), "seg0.idx");
    }

    DropIndex();
    DropTable();
}

TEST_P(SegmentIndexEntryTest, opt_hnsw_index_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns{"col1"};
        Vector<InitParameter *> parameters;
        parameters.emplace_back(new InitParameter("metric", "l2"));
        parameters.emplace_back(new InitParameter("m", "16"));
        parameters.emplace_back(new InitParameter("ef_construction", "200"));
        parameters.emplace_back(new InitParameter("encode", "plain"));

        auto index_base = IndexHnsw::Make(MakeShared<String>("idx1"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;

        for (auto parameter : parameters) {
            delete parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);
        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn1);
        auto [_, status3] = txn1->CreateIndexPrepare(index_entry, table_ref.get(), true, true);
        txn1->CreateIndexFinish(table_entry, index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn1); 
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("opt"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "idx1";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        Vector<UniquePtr<InitParameter>> opt_params;
        opt_params.emplace_back(MakeUnique<InitParameter>("compress_to_lvq", ""));
        TxnTableStore *txn_table_store = txn1->GetTxnTableStore(table_entry);
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));
        segment_index_entry->OptIndex(const_cast<IndexBase *>(index_entry->index_base()), txn_table_store, opt_params, false);

        opt_params.clear();
        opt_params.emplace_back(MakeUnique<InitParameter>("lvq_avg", ""));
        segment_index_entry->OptIndex(const_cast<IndexBase *>(index_entry->index_base()), txn_table_store, opt_params, false);

        opt_params.clear();
        opt_params.emplace_back(MakeUnique<InitParameter>("compress_to_lvq", ""));
        opt_params.emplace_back(MakeUnique<InitParameter>("lvq_avg", ""));
        EXPECT_THROW(segment_index_entry->OptIndex(const_cast<IndexBase *>(index_entry->index_base()), txn_table_store, opt_params, false), RecoverableException);
        txn_mgr->CommitTxn(txn1);
    }

    DropTable();
}

TEST_P(SegmentIndexEntryTest, opt_bmp_index_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto sparseInfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kSparse, sparseInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns{"col1"};
        Vector<InitParameter *> parameters;
        parameters.emplace_back(new InitParameter("block_size", "16"));
        parameters.emplace_back(new InitParameter("compress_type", "compress"));

        auto index_base = IndexBMP::Make(MakeShared<String>("idx1"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;

        for (auto parameter : parameters) {
            delete parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);
        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn1);
        auto [_, status3] = txn1->CreateIndexPrepare(index_entry, table_ref.get(), true, true);
        txn1->CreateIndexFinish(table_entry, index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn1); 
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("opt"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "idx1";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        Vector<UniquePtr<InitParameter>> opt_params;
        opt_params.emplace_back(MakeUnique<InitParameter>("topk", "10"));
        opt_params.emplace_back(MakeUnique<InitParameter>("bp_reorder", ""));
        TxnTableStore *txn_table_store = txn1->GetTxnTableStore(table_entry);
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));
        segment_index_entry->OptIndex(const_cast<IndexBase *>(index_entry->index_base()), txn_table_store, opt_params, false);
        txn_mgr->CommitTxn(txn1);
    }

    DropTable();
}

TEST_P(SegmentIndexEntryTest, flush_fulltext_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    CreateIndex();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("flush"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//create
        EXPECT_FALSE(segment_index_entry->Flush(txn1->BeginTS()));
    }

    DropIndex();
    DropTable();
}

TEST_P(SegmentIndexEntryTest, flush_bmp_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto sparseInfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kSparse, sparseInfo), "col1", constraints);
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns{"col1"};
        Vector<InitParameter *> parameters;
        parameters.emplace_back(new InitParameter("block_size", "16"));
        parameters.emplace_back(new InitParameter("compress_type", "compress"));

        auto index_base = IndexBMP::Make(MakeShared<String>("idx1"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;

        for (auto parameter : parameters) {
            delete parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);
        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn1);
        auto [_, status3] = txn1->CreateIndexPrepare(index_entry, table_ref.get(), true, true);
        txn1->CreateIndexFinish(table_entry, index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn1); 
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("flush"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "idx1";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//create
        EXPECT_TRUE(segment_index_entry->Flush(txn1->BeginTS()));
    }

    DropTable();
}

TEST_P(SegmentIndexEntryTest, cleanup_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    CreateIndex();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("flush"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        SharedPtr<SegmentIndexEntry> segment_index_entry;
        EXPECT_TRUE(index_entry->GetOrCreateSegment(0, txn1, segment_index_entry));//create
        segment_index_entry->Cleanup();
    }

    DropIndex();
    DropTable();
}

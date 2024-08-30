#include "unit_test/base_test.h"
#include "type/complex/embedding_type.h"

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
import column_vector;
import internal_types;
import constant_expr;
import parsed_expr;

import base_entry;
import table_entry;
import table_entry_type;
import segment_entry;
import block_entry;

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

void InsertData(const String& db_name, const String& table_name) {
    Vector<Vector<String>> datas_ = {
        {
         R"#(Animalia is an illustrated children's book by anarchism Graeme Base. It was originally published in 1986, followed by a tenth anniversary edition in 1996, and a 25th anniversary edition in 2012. Over three million copies have been sold. A special numbered and signed anniversary edition was also published in 1996, with an embossed gold jacket.)#"},
        {
         R"#(The Academy Awards are the oldest awards ceremony for achievements in motion pictures. one of The add test Academy Award for Best Production Design recognizes achievement in art direction on a film. The category's original name was Best Art Direction, but was changed to its current name in 2012 for the 85th Academy Awards.  This change resulted from the Art Director's branch of the Academy being renamed the Designer's branch.)#"},
        {
         R"#(The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.)#"},
        {
         R"#(周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。)#"},
    };
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import data"));
    auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
    EXPECT_TRUE(status.ok());

    SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
    {
        for (BlockID block_id = 0; block_id < datas_.size(); ++block_id) {
            UniquePtr<BlockEntry> block_entry =
                BlockEntry::NewBlockEntry(segment_entry.get(), block_id, 0 /*checkpoint_ts*/, table_entry->ColumnCount(), txn);
            {
                Vector<ColumnVector> column_vectors;
                for (SizeT i = 0; i < table_entry->ColumnCount(); ++i) {
                    auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                    column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
                }
                auto& row = datas_[block_id];
                for (SizeT i = 0; i < column_vectors.size(); ++i) {
                    auto &column = row[i];
                    column_vectors[i].AppendByStringView(column);
                }
                block_entry->IncreaseRowCount(1);
            }
            segment_entry->AppendBlockEntry(std::move(block_entry));
        }

    }
    segment_entry->FlushNewData();
    txn->Import(table_entry, segment_entry);

    txn_mgr->CommitTxn(txn);
}

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
        EXPECT_TRUE(TableEntry::DecodeIndex(encode_index)[1] == "tbl1");
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

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    // CreateIndex
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        SharedPtr<String> index_name = MakeShared<String>("fulltext_index");
        auto index_base_fulltext = IndexFullText::Make(index_name, "fulltext_index_test_fulltext", columns1, parameters1);

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base_fulltext, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);

        Map<String, String> column2analyzer;
        table_entry->GetFulltextAnalyzers(txn1->TxnID(), txn1->BeginTS(),column2analyzer);
        for(auto pair : column2analyzer){
            EXPECT_STREQ(pair.first.c_str(), "col1");
            EXPECT_STREQ(pair.second.c_str(), "standard");
        }
        txn_mgr->CommitTxn(txn1);
    }

    //drop index
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop index"));
        auto status = txn1->DropIndexByName("default_db", "tbl1", "fulltext_index", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest,  optimize_fulltext_index_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    // CreateIndex
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        SharedPtr<String> index_name = MakeShared<String>("fulltext_index");
        auto index_base_fulltext = IndexFullText::Make(index_name, "fulltext_index_test_fulltext", columns1, parameters1);

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base_fulltext, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);

        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("optimize index"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);
        table_entry->OptimizeIndex(txn1);
        txn_mgr->CommitTxn(txn1);
    }

    //drop index
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop index"));
        auto status = txn1->DropIndexByName("default_db", "tbl1", "fulltext_index", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest,  roll_back_committed_write_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //
    {
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        Vector<Vector<String>> datas_ = {
            {
            R"#(Animalia is an illustrated children's book by anarchism Graeme Base. It was originally published in 1986, followed by a tenth anniversary edition in 1996, and a 25th anniversary edition in 2012. Over three million copies have been sold. A special numbered and signed anniversary edition was also published in 1996, with an embossed gold jacket.)#"},
            {
            R"#(The Academy Awards are the oldest awards ceremony for achievements in motion pictures. one of The add test Academy Award for Best Production Design recognizes achievement in art direction on a film. The category's original name was Best Art Direction, but was changed to its current name in 2012 for the 85th Academy Awards.  This change resulted from the Art Director's branch of the Academy being renamed the Designer's branch.)#"},
            {
            R"#(The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.)#"},
            {
            R"#(周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。)#"},
        };
        TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import data"));
        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
        SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
        {
            for (BlockID block_id = 0; block_id < datas_.size(); ++block_id) {
                UniquePtr<BlockEntry> block_entry =
                    BlockEntry::NewBlockEntry(segment_entry.get(), block_id, 0 /*checkpoint_ts*/, table_entry->ColumnCount(), txn);
                {
                    Vector<ColumnVector> column_vectors;
                    for (SizeT i = 0; i < table_entry->ColumnCount(); ++i) {
                        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
                    }
                    auto& row = datas_[block_id];
                    for (SizeT i = 0; i < column_vectors.size(); ++i) {
                        auto &column = row[i];
                        column_vectors[i].AppendByStringView(column);
                    }
                    block_entry->IncreaseRowCount(1);
                }
                segment_entry->AppendBlockEntry(std::move(block_entry));
            }

        }
        segment_entry->FlushNewData();
        txn->Import(table_entry, segment_entry);
        auto txn_store = txn->GetTxnTableStore(table_entry);
        txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
        Vector<TxnSegmentStore> segment_stores;
        for(auto pair : txn_store->txn_segments()){
            segment_stores.emplace_back(pair.second);
        }
        table_entry->CommitWrite(txn->TxnID(), txn->CommitTS(), txn_store->txn_segments(), nullptr);
        std::cout<<segment_stores.size()<<std::endl;
        auto status1 = table_entry->RollbackWrite(txn->CommitTS(), segment_stores);
        EXPECT_TRUE(status1.ok());
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest,  roll_back_uncommitted_write_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //
    {
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        Vector<Vector<String>> datas_ = {
            {
            R"#(Animalia is an illustrated children's book by anarchism Graeme Base. It was originally published in 1986, followed by a tenth anniversary edition in 1996, and a 25th anniversary edition in 2012. Over three million copies have been sold. A special numbered and signed anniversary edition was also published in 1996, with an embossed gold jacket.)#"},
            {
            R"#(The Academy Awards are the oldest awards ceremony for achievements in motion pictures. one of The add test Academy Award for Best Production Design recognizes achievement in art direction on a film. The category's original name was Best Art Direction, but was changed to its current name in 2012 for the 85th Academy Awards.  This change resulted from the Art Director's branch of the Academy being renamed the Designer's branch.)#"},
            {
            R"#(The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.)#"},
            {
            R"#(周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。)#"},
        };
        TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import data"));
        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
        SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
        {
            for (BlockID block_id = 0; block_id < datas_.size(); ++block_id) {
                UniquePtr<BlockEntry> block_entry =
                    BlockEntry::NewBlockEntry(segment_entry.get(), block_id, 0 /*checkpoint_ts*/, table_entry->ColumnCount(), txn);
                {
                    Vector<ColumnVector> column_vectors;
                    for (SizeT i = 0; i < table_entry->ColumnCount(); ++i) {
                        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
                    }
                    auto& row = datas_[block_id];
                    for (SizeT i = 0; i < column_vectors.size(); ++i) {
                        auto &column = row[i];
                        column_vectors[i].AppendByStringView(column);
                    }
                    block_entry->IncreaseRowCount(1);
                }
                segment_entry->AppendBlockEntry(std::move(block_entry));
            }

        }
        segment_entry->FlushNewData();
        txn->Import(table_entry, segment_entry);
        auto txn_store = txn->GetTxnTableStore(table_entry);
        txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
        Vector<TxnSegmentStore> segment_stores;
        for(auto pair : txn_store->txn_segments()){
            segment_stores.emplace_back(pair.second);
        }
        auto status1 = table_entry->RollbackWrite(txn->CommitTS(), segment_stores);
        EXPECT_TRUE(status1.ok());
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest,  compact_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("compact"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);
        EXPECT_TRUE(table_entry->CompactPrepare());
        table_entry->CheckCompaction(txn1->TxnID());
        table_entry->SetCompactionAlg(nullptr);
        EXPECT_FALSE(table_entry->CompactPrepare());
        table_entry->CheckCompaction(txn1->TxnID());
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest,  check_any_delete_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
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
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("check"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_FALSE(table_entry->CheckAnyDelete(txn1->BeginTS()));
        Vector<RowID> delete_row_ids;
        delete_row_ids.emplace_back(0, 0);
        txn1->Delete(table_entry, delete_row_ids);
        txn_mgr->CommitTxn(txn1);

        auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("check"));
        EXPECT_TRUE(table_entry->CheckAnyDelete(txn2->BeginTS()));
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest, table_entry_type_test){
    EXPECT_STREQ(ToString(TableEntryType::kTableEntry).c_str(), "Table");
    EXPECT_STREQ(ToString(TableEntryType::kCollectionEntry).c_str(), "Collection");
    EXPECT_THROW(ToString(static_cast<TableEntryType>(2)), UnrecoverableException);
}

TEST_P(TableEntryTest, serialize_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = strdup("DEFAULT");
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kVarchar), "col1", constraints, std::shared_ptr<ParsedExpr>(const_expr));
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    // CreateIndex
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        SharedPtr<String> index_name = MakeShared<String>("fulltext_index");
        auto index_base_fulltext = IndexFullText::Make(index_name, "fulltext_index_test_fulltext", columns1, parameters1);

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base_fulltext, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);

        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("serialize"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        auto json_res = table_entry->Serialize(txn1->BeginTS());
        EXPECT_TRUE(json_res["table_name"] == "tbl1");
        EXPECT_TRUE(json_res["column_definition"][0]["column_name"] == "col1");
        EXPECT_TRUE(json_res["column_definition"][0]["default"]["value"] == "DEFAULT");
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest, deserialize_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = strdup("DEFAULT");
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kVarchar), "col1", constraints, std::shared_ptr<ParsedExpr>(const_expr));
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    // CreateIndex
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        SharedPtr<String> index_name = MakeShared<String>("fulltext_index");
        auto index_base_fulltext = IndexFullText::Make(index_name, "fulltext_index_test_fulltext", columns1, parameters1);

        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_status.ok(), true);

        auto [index_entry, index_status] = txn1->CreateIndexDef(table_entry, index_base_fulltext, ConflictType::kInvalid);
        EXPECT_EQ(index_status.ok(), true);

        txn_mgr->CommitTxn(txn1);
    }

    //insert data
    {
        InsertData("default_db", "tbl1");
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("serialize"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        auto json_res = table_entry->Serialize(txn1->BeginTS());

        auto table_entry1 = TableEntry::Deserialize(json_res, table_entry->GetTableMeta(), buffer_mgr);
        auto json_res1 = table_entry->Serialize(txn1->BeginTS());
        EXPECT_TRUE(json_res == json_res1);
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}

TEST_P(TableEntryTest, get_colunm_id_by_name_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    //create table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Vector<SharedPtr<ColumnDef>> columns;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = strdup("DEFAULT");
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kVarchar), "col1", constraints, std::shared_ptr<ParsedExpr>(const_expr));
            columns.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
        auto status = txn1->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("serialize"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_EQ(table_entry->GetColumnIdByName("col1"), 0);
        EXPECT_THROW(table_entry->GetColumnIdByName("col2"), RecoverableException);
    }

    //drop table
    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn1);
    }
}
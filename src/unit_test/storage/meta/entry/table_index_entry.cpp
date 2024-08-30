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
import base_table_ref;

import base_entry;
import table_entry;
import table_index_entry;
import segment_entry;
import block_entry;

using namespace infinity;

class TableIndexEntryTest : public BaseTestParamStr {
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
                         TableIndexEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH,
                                           BaseTestParamStr::VFS_CONFIG_PATH));

void InsertData(const String& db_name, const String& table_name);

void CreateTable(){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
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

void CreateIndex(){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
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
    auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn1);
    auto [_, status3] = txn1->CreateIndexPrepare(index_entry, table_ref.get(), true, true);
    txn1->CreateIndexFinish(table_entry, index_entry);
    EXPECT_TRUE(status3.ok());

    txn_mgr->CommitTxn(txn1);    
}

void DropIndex(){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop index"));
    auto status = txn1->DropIndexByName("default_db", "tbl1", "fulltext_index", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(txn1);
}

void DropTable(){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
    auto status = txn1->DropTableCollectionByName("default_db", "tbl1", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(txn1);    
}

TEST_P(TableIndexEntryTest, decode_index_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    CreateTable();
    CreateIndex();

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("decode"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        String encoded_index = TableIndexEntry::EncodeIndex(index_name, index_entry->table_index_meta());
        EXPECT_TRUE(encoded_index == "#default_db#tbl1#fulltext_index");
        Vector<std::string_view> decoded_index = TableIndexEntry::DecodeIndex(encoded_index);
        EXPECT_TRUE(decoded_index[0] == "default_db");
        EXPECT_TRUE(decoded_index[1] == "tbl1");
        EXPECT_TRUE(decoded_index[2] == "fulltext_index");

        EXPECT_TRUE(TableIndexEntry::EncodeIndex(index_name, nullptr) == "");
        EXPECT_THROW(TableIndexEntry::DecodeIndex("/default_db/tbl1/fulltext_index"), UnrecoverableException);
    }

    DropIndex();
    DropTable();
}

TEST_P(TableIndexEntryTest, deserialize_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    CreateTable();
    CreateIndex();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("serialize"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto json_res = index_entry->Serialize(txn1->BeginTS());
        auto index_entry1 = TableIndexEntry::Deserialize(json_res, index_entry->table_index_meta(), buffer_mgr, table_entry);
        auto json_res1 = index_entry1->Serialize(txn1->BeginTS());
        EXPECT_TRUE(json_res == json_res1);
    }

    DropIndex();
    DropTable();
}

///TODO: non nullptr case
TEST_P(TableIndexEntryTest, get_mem_index_test){
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    CreateIndex();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("GetMemIndex"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        const String &index_name = "fulltext_index";
        auto [index_entry, index_status] = txn1->GetIndexByName(db_name, table_name, index_name);
        EXPECT_TRUE(index_status.ok());
        index_entry->GetMemIndex();
    }

    DropIndex();
    DropTable();
}
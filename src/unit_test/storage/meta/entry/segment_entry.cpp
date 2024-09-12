#include "type/complex/embedding_type.h"
#include "gtest/gtest.h"
#include <regex>
import base_test;

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import value;
import roaring_bitmap;

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

class SegmentEntryTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         SegmentEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

void CreateTable();
void CreateIndex();
void InsertData(const String& db_name, const String& table_name);
void DropIndex();
void DropTable();

TEST_P(SegmentEntryTest, decode_index_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("decode"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        EXPECT_TRUE(segment_entry != nullptr);
        String encoded_index = SegmentEntry::EncodeIndex(0, table_entry);
        std::cout<<"encoded_index: "<<encoded_index<<std::endl;
        EXPECT_TRUE(encoded_index == "#default_db#tbl1#0");
        Vector<std::string_view> decoded_index  = SegmentEntry::DecodeIndex(encoded_index);
        EXPECT_TRUE(decoded_index[0] == "default_db");
        EXPECT_TRUE(decoded_index[1] == "tbl1");
        EXPECT_TRUE(decoded_index[2] == "0");
        EXPECT_THROW(SegmentEntry::DecodeIndex("/default_db/tbl1/0"), UnrecoverableException);
    }

    DropTable();
}

TEST(ToStringTest, segment_status_test) {
    EXPECT_STREQ(ToString(SegmentStatus::kUnsealed).c_str(), "Unsealed");
    EXPECT_STREQ(ToString(SegmentStatus::kSealed).c_str(), "Sealed");
    EXPECT_STREQ(ToString(SegmentStatus::kCompacting).c_str(), "Compacting");
    EXPECT_STREQ(ToString(SegmentStatus::kNoDelete).c_str(), "NoDelete");
    EXPECT_STREQ(ToString(SegmentStatus::kDeprecated).c_str(), "Deprecated");
}

TEST(ToStringTest, segment_status_to_string_test) {
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(SegmentStatus::kUnsealed).c_str(), "Unsealed");
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(SegmentStatus::kSealed).c_str(), "Sealed");
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(SegmentStatus::kCompacting).c_str(), "Compacting");
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(SegmentStatus::kNoDelete).c_str(), "NoDelete");
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(SegmentStatus::kDeprecated).c_str(), "Deprecated");
    EXPECT_STREQ(SegmentEntry::SegmentStatusToString(static_cast<SegmentStatus>(5)).c_str(), "Invalid Status");
}

TEST_P(SegmentEntryTest, segment_entry_to_string_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        String segment_path = segment_entry->ToString();
        std::cout<<segment_path<<std::endl;
        EXPECT_TRUE(std::regex_match(segment_path, 
                    std::regex("Segment\\spath:\\s(.*)_db_default_db/(.*)_table_tbl1/seg_0,\\sid:\\s0,\\srow_count:\\s4,\\sblock_count:\\s4,\\sstatus:\\sSealed"))); 
    }

    DropTable();
}

TEST_P(SegmentEntryTest, set_sealed_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        std::cout<<"segment_entry_status: "<<ToString(segment_entry->status())<<std::endl;
        EXPECT_FALSE(segment_entry->SetSealed());
    }

    DropTable();
}

TEST_P(SegmentEntryTest, set_compacting_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        std::cout<<"segment_entry_status: "<<ToString(segment_entry->status())<<std::endl;
        EXPECT_TRUE(segment_entry->TrySetCompacting(nullptr));
        EXPECT_FALSE(segment_entry->TrySetCompacting(nullptr));
    }

    DropTable();
}

TEST_P(SegmentEntryTest, set_no_delete_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        std::cout<<"segment_entry_status: "<<ToString(segment_entry->status())<<std::endl;
        EXPECT_THROW(segment_entry->SetNoDelete(), UnrecoverableException);
        EXPECT_TRUE(segment_entry->TrySetCompacting(nullptr));
        EXPECT_TRUE(segment_entry->SetNoDelete());
    }

    DropTable();
}

TEST_P(SegmentEntryTest, set_deprecated_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        std::cout<<"segment_entry_status: "<<ToString(segment_entry->status())<<std::endl;
        EXPECT_THROW(segment_entry->SetDeprecated(txn1->BeginTS()), UnrecoverableException);
        EXPECT_TRUE(segment_entry->TrySetCompacting(nullptr));
        EXPECT_TRUE(segment_entry->SetNoDelete());
        segment_entry->SetDeprecated(txn1->BeginTS());
    }

    DropTable();
}

TEST_P(SegmentEntryTest, roll_back_compact_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        std::cout<<"segment_entry_status: "<<ToString(segment_entry->status())<<std::endl;
        EXPECT_THROW(segment_entry->RollbackCompact(), UnrecoverableException);
        EXPECT_TRUE(segment_entry->TrySetCompacting(nullptr));
        EXPECT_THROW(segment_entry->RollbackCompact(), UnrecoverableException);
        EXPECT_TRUE(segment_entry->SetNoDelete());
        segment_entry->RollbackCompact();
    }

    DropTable();
}

TEST_P(SegmentEntryTest, check_row_visible_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        EXPECT_TRUE(segment_entry->CheckRowVisible(0, txn1->BeginTS(), true));
    }

    DropTable();
}

TEST_P(SegmentEntryTest, check_rows_visible_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        Vector<u32> segment_offsets = {0};
        segment_entry->CheckRowsVisible(segment_offsets, txn1->BeginTS());
        SharedPtr<Bitmask> offsets = Bitmask::MakeSharedAllTrue(4);
        segment_entry->CheckRowsVisible(*offsets, txn1->BeginTS());
    }

    DropTable();
}

TEST_P(SegmentEntryTest, append_data_test) {
    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();
    CreateTable();
    InsertData("default_db", "tbl1");

    {
        auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("set"));
        const String &db_name = "default_db";
        const String &table_name = "tbl1";
        auto [table_entry, table_status] = txn1->GetTableByName(db_name, table_name);
        EXPECT_TRUE(table_status.ok());
        auto segment_entry = table_entry->GetSegmentEntry(0);
        auto txn_store = txn1->GetTxnTableStore(table_entry);
        txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
        auto append_state = txn_store->GetAppendState();
        EXPECT_THROW(segment_entry->AppendData(txn1->TxnID(), txn1->BeginTS(), append_state, buffer_mgr, txn1), UnrecoverableException);
    }

    DropTable();
}
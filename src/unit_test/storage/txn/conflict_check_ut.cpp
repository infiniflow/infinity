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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.conflict_check;

import :ut.base_test;
import :stl;
import :infinity_context;
import :table_def;
import :storage;
import :new_txn_manager;
import :new_txn;
import :column_vector;
import :data_block;
import :value;
import :infinity_exception;
import :status;
import :txn_state;
import :db_meeta;
import :table_meeta;
#endif

import compilation_config;
import column_def;
import data_type;
import logical_type;
import extra_ddl_info;
import row_id;

using namespace infinity;

class ConflictCheckTest : public BaseTest {

protected:
    void SetUp() override {
        // Earlier cases may leave a dirty infinity instance. Destroy it first.
        infinity::InfinityContext::instance().UnInit();
        RemoveDbDirs();
        auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_bgtask.toml");
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        storage_ = InfinityContext::instance().storage();
        txn_mgr_ = storage_->new_txn_manager();
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        CleanupDbDirs();
    }

    NewTxn *DeleteRow(const String &db_name, const String &table_name, Vector<SegmentOffset> segment_offsets) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Delete row"), TransactionType::kNormal);

        Vector<RowID> row_ids;
        for (auto segment_offset : segment_offsets) {
            row_ids.push_back(RowID(0 /*segment_id*/, segment_offset));
        }

        Status status = txn->Delete(db_name, table_name, row_ids);
        EXPECT_TRUE(status.ok());
        return txn;
    };

    void ExpectConflict(NewTxn *txn) {
        Status status = txn_mgr_->CommitTxn(txn);
        EXPECT_EQ(status.code(), ErrorCode::kTxnConflictNoRetry);
    };

    void InitTable(const String &db_name, const String &table_name, SharedPtr<TableDef> table_def, SizeT row_cnt) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Init table"), TransactionType::kNormal);

        Status status = txn->CreateTable(db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        txn = txn_mgr_->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
        Vector<SharedPtr<ColumnVector>> column_vectors;
        {
            auto column_vector = MakeShared<ColumnVector>(table_def->columns()[0]->type());
            column_vector->Initialize();
            for (SizeT i = 0; i < row_cnt; i++) {
                column_vector->AppendValue(Value::MakeInt(i));
            }
            column_vectors.push_back(column_vector);
        }
        auto data_block = DataBlock::Make();
        data_block->Init(column_vectors);

        status = txn->Append(db_name, table_name, data_block);
        EXPECT_TRUE(status.ok());

        txn_mgr_->CommitTxn(txn);
    }

    void CheckRowCnt(const String &db_name, const String &table_name, SizeT expected_row_cnt) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Check row count"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(db_name, table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        auto[row_cnt, status2] = table_meta->GetTableRowCount();
        EXPECT_TRUE(status2.ok());

        txn_mgr_->CommitTxn(txn);

        EXPECT_EQ(row_cnt, expected_row_cnt);
    }

protected:
    Storage *storage_;
    NewTxnManager *txn_mgr_;
};

TEST_F(ConflictCheckTest, conflict_check_delete) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto table_name = std::make_shared<std::string>("table1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1});

    SizeT row_cnt = 10;

    InitTable(*db_name, *table_name, table_def, row_cnt);
    {
        auto *txn1 = DeleteRow(*db_name, *table_name, {0});
        auto *txn2 = DeleteRow(*db_name, *table_name, {0});
        auto *txn3 = DeleteRow(*db_name, *table_name, {0});
        Vector<TransactionID> txn_ids{txn1->TxnID(), txn2->TxnID(), txn3->TxnID()};

        txn_mgr_->CommitTxn(txn1);
        ExpectConflict(txn2);
        ExpectConflict(txn3);

        --row_cnt;
        CheckRowCnt(*db_name, *table_name, row_cnt);
    }
    {
        auto *txn1 = DeleteRow(*db_name, *table_name, {1});
        auto *txn2 = DeleteRow(*db_name, *table_name, {1});
        auto *txn3 = DeleteRow(*db_name, *table_name, {1});

        txn_mgr_->CommitTxn(txn2);
        ExpectConflict(txn1);
        ExpectConflict(txn3);

        --row_cnt;
        CheckRowCnt(*db_name, *table_name, row_cnt);
    }
    {
        auto *txn1 = DeleteRow(*db_name, *table_name, {2});
        auto *txn2 = DeleteRow(*db_name, *table_name, {2});
        auto *txn3 = DeleteRow(*db_name, *table_name, {2});

        txn_mgr_->CommitTxn(txn3);
        ExpectConflict(txn2);
        ExpectConflict(txn1);

        --row_cnt;
        CheckRowCnt(*db_name, *table_name, row_cnt);
    }
    {
        auto *txn1 = DeleteRow(*db_name, *table_name, {3});
        auto *txn2 = DeleteRow(*db_name, *table_name, {3, 4});

        txn_mgr_->CommitTxn(txn1);
        ExpectConflict(txn2);

        --row_cnt;
        CheckRowCnt(*db_name, *table_name, row_cnt);
    }
    {
        auto *txn1 = DeleteRow(*db_name, *table_name, {5, 6});
        auto *txn2 = DeleteRow(*db_name, *table_name, {5});

        txn_mgr_->CommitTxn(txn1);
        ExpectConflict(txn2);

        row_cnt -= 2;
        CheckRowCnt(*db_name, *table_name, row_cnt);
    }
}

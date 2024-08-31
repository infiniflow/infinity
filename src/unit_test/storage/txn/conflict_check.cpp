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

#include "type/complex/row_id.h"
#include "gtest/gtest.h"
import base_test;

import stl;
import compilation_config;
import infinity_context;
import table_def;
import column_def;
import data_type;
import logical_type;
import storage;
import txn_manager;
import txn;
import extra_ddl_info;
import column_vector;
import data_block;
import value;
import infinity_exception;
import status;

using namespace infinity;

class ConflictCheckTest : public BaseTest {

protected:
    void SetUp() override {
        RemoveDbDirs();

        auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_bgtask.toml");
        infinity::InfinityContext::instance().Init(config_path);

        storage_ = InfinityContext::instance().storage();
        txn_mgr_ = storage_->txn_manager();
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
    }

    Txn *DeleteRow(const String &db_name, const String &table_name, Vector<SegmentOffset> segment_offsets) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Delete row"));

        Vector<RowID> row_ids;
        for (auto segment_offset : segment_offsets) {
            row_ids.push_back(RowID(0 /*segment_id*/, segment_offset));
        }

        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());
        status = txn->Delete(table_entry, row_ids, true);
        EXPECT_TRUE(status.ok());
        return txn;
    };

    void ExpectConflict(Txn *txn) {
        try {
            txn_mgr_->CommitTxn(txn);
            FAIL() << "Expected RecoverableException";
        } catch (const RecoverableException &e) {
            EXPECT_EQ(e.ErrorCode(), ErrorCode::kTxnConflict);
            txn_mgr_->RollBackTxn(txn);
        } catch (...) {
            FAIL() << "Expected RecoverableException";
        }
    };

    void InitTable(const String &db_name, const String &table_name, SharedPtr<TableDef> table_def, SizeT row_cnt) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Init table"));

        txn->CreateTable(db_name, table_def, ConflictType::kError);
        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());

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

        status = txn->Append(table_entry, data_block);
        EXPECT_TRUE(status.ok());

        txn_mgr_->CommitTxn(txn);
    }

    void CheckRowCnt(const String &db_name, const String &table_name, SizeT expected_row_cnt) {
        auto *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Check row count"));
        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());

        EXPECT_EQ(table_entry->row_count(), expected_row_cnt);

        txn_mgr_->CommitTxn(txn);
    }

protected:
    Storage *storage_;
    TxnManager *txn_mgr_;
};

TEST_F(ConflictCheckTest, conflict_check_delete) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto table_name = std::make_shared<std::string>("table1");
    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto table_def = TableDef::Make(db_name, table_name, {column_def1});

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

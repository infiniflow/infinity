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
#include "unit_test/base_test.h"

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

        auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_all_bgtask.toml");
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();

        RemoveDbDirs();
    }
};

TEST_F(ConflictCheckTest, conflict_check_delete) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto table_name = std::make_shared<std::string>("table1");
    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto table_def = TableDef::Make(db_name, table_name, {column_def1});

    SizeT row_cnt = 10;

    Storage *storage = InfinityContext::instance().storage();
    TxnManager *txn_mgr = storage->txn_manager();

    auto DeleteRow = [&](SegmentOffset delete_off) {
        auto *txn = txn_mgr->BeginTxn();

        RowID row_id(0 /*segment_id*/, delete_off);
        Vector<RowID> row_ids{row_id};
        auto status = txn->Delete(*db_name, *table_name, row_ids, true);
        EXPECT_TRUE(status.ok());
        return txn;
    };

    auto ExpectConflict = [&](Txn *txn) {
        try {
            txn_mgr->CommitTxn(txn);
            FAIL() << "Expected RecoverableException";
        } catch (const RecoverableException &e) {
            EXPECT_EQ(e.ErrorCode(), ErrorCode::kTxnConflict);
            txn_mgr->RollBackTxn(txn);
        } catch (...) {
            FAIL() << "Expected RecoverableException";
        }
    };

    {
        auto *txn = txn_mgr->BeginTxn();

        txn->CreateTable(*db_name, table_def, ConflictType::kError);
        auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn();

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

        auto status = txn->Append(*db_name, *table_name, data_block);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn1 = DeleteRow(0);
        auto *txn2 = DeleteRow(0);
        auto *txn3 = DeleteRow(0);
        Vector<TransactionID> txn_ids{txn1->TxnID(), txn2->TxnID(), txn3->TxnID()};

        txn_mgr->CommitTxn(txn1);
        ExpectConflict(txn2);
        ExpectConflict(txn3);
        for (auto txn_id : txn_ids) {
            EXPECT_THROW(txn_mgr->GetTxn(txn_id), std::out_of_range);
        }
    }
    {
        auto *txn1 = DeleteRow(0);
        auto *txn2 = DeleteRow(0);
        auto *txn3 = DeleteRow(0);

        txn_mgr->CommitTxn(txn2);
        ExpectConflict(txn1);
        ExpectConflict(txn3);
    }
    {
        auto *txn1 = DeleteRow(0);
        auto *txn2 = DeleteRow(0);
        auto *txn3 = DeleteRow(0);

        txn_mgr->CommitTxn(txn3);
        ExpectConflict(txn2);
        ExpectConflict(txn1);
    }
}
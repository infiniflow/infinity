// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include "gtest/gtest.h"

import replay_test;
import base_test;
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import kv_code;
import kv_store;
import new_txn;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import constant_expr;

using namespace infinity;

class TestTxnCheckpointAddColumnTest : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();

        db_name = std::make_shared<String>("db1");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    }

protected:
    SharedPtr<String> db_name;
    SharedPtr<ColumnDef> column_def1;
    SharedPtr<ColumnDef> column_def2;
    SharedPtr<String> table_name;
    SharedPtr<TableDef> table_def;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCheckpointAddColumnTest,
                         ::testing::Values(
                                           TestTxnCheckpointAddColumnTest::NEW_VFS_OFF_BG_OFF_PATH));

TEST_P(TestTxnCheckpointAddColumnTest, addcol_checkpoint_insert) {
    SharedPtr<String> db_name = std::make_shared<String>("default_db");

    auto make_input_block = [&](Vector<SharedPtr<ColumnDef>> column_defs, Vector<const Value> values, SizeT row_cnt) {
        auto make_column = [&](SharedPtr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (SizeT i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = MakeShared<DataBlock>();
        for (SizeT i = 0; i < column_defs.size(); ++i) {
            auto col = make_column(column_defs[i], values[i]);
            input_block->InsertVector(col, i);
        }
        input_block->Finalize();
        return input_block;
    };

    Status status;
    NewTxn *txn;
    txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
    status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    SharedPtr<DataBlock> input_block = make_input_block(Vector<SharedPtr<ColumnDef>>{column_def1, column_def2},
                                                        Vector<const Value>{Value::MakeInt(1), Value::MakeVarchar("abcde")},
                                                        10);
    txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
    status = txn->Append(*db_name, *table_name, input_block);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<ColumnDef> column_def3 =
        std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
    txn = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
    status = txn->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
    status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    input_block = make_input_block(Vector<SharedPtr<ColumnDef>>{column_def1, column_def2, column_def3},
                                   Vector<const Value>{Value::MakeInt(1), Value::MakeVarchar("abcde"), Value::MakeVarchar("abcde")},
                                   10);
    txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
    status = txn->Append(*db_name, *table_name, input_block);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

        // Check table row count is 20
        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status1 = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status1.ok());

        SizeT row_count = 0;
        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());

        for (SegmentID segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(block_status.ok());

            for (BlockID block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                auto [block_info, info_status] = block_meta.GetBlockInfo();
                EXPECT_TRUE(info_status.ok());
                row_count += block_info->row_count_;
            }
        }
        EXPECT_EQ(row_count, 20);
    }
}

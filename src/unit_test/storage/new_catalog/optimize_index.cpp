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
import data_access_state;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
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

class TestOptimizeIndex : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

        db_name = std::make_shared<String>("db1");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

        index_name1 = std::make_shared<std::string>("index1");
        index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

        segment_id = 0;
    }

    void TearDown() override {
        new_txn_mgr = nullptr;
        BaseTestParamStr::TearDown();
    }

    void PrepareForOptimizeIndex() {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        u32 block_row_cnt = 8192;
        auto make_input_block = [&](const Value &v1, const Value &v2) {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, const Value &v) {
                for (u32 i = 0; i < block_row_cnt; ++i) {
                    col.AppendValue(v);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(column_def1->type());
                col1->Initialize();
                append_to_col(*col1, v1);
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = ColumnVector::Make(column_def2->type());
                col2->Initialize();
                append_to_col(*col2, v2);
                input_block->InsertVector(col2, 1);
            }
            input_block->Finalize();
            return input_block;
        };
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
                SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
    }

    void DropDB() {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

protected:
    NewTxnManager *new_txn_mgr = nullptr;

    SharedPtr<String> db_name;
    SharedPtr<ColumnDef> column_def1;
    SharedPtr<ColumnDef> column_def2;
    SharedPtr<String> table_name;
    SharedPtr<TableDef> table_def;

    SharedPtr<String> index_name1;
    SharedPtr<IndexBase> index_def1;

    SegmentID segment_id = 0;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestOptimizeIndex,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestOptimizeIndex, optimize_index_and_drop_db) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestOptimizeIndex, optimize_index_and_drop_table) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestOptimizeIndex, optimize_index_and_drop_index) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestOptimizeIndex, optimize_index_and_optimize_index) {
    auto CheckTable = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        status = table_index_meta->GetSegmentIDs(index_segment_ids_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({2}));
    };

    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                compact          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //                         |------------------|------------------|
        //                        t2                compact (fail)    rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //         |------------------|------------------|
        //        t2                compact (fail)       rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1            compact                        commit
        //  |--------------|--------------------------------|
        //         |------------------|------------------|
        //        t2                compact (fail)      rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
}

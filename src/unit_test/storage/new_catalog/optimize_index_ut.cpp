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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.optimize_index;

import :ut.base_test;
import :stl;
import :third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :infinity_exception;
import :table_def;
import :index_base;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_bmp;
import :defer_op;
import :meta_info;
import :data_block;
import :column_vector;
import :value;
import :kv_code;
import :kv_store;
import :new_txn;
import :buffer_obj;
import :buffer_handle;
import :secondary_index_in_mem;
import :secondary_index_data;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
import :catalog_meta;
import :mem_index;
import :roaring_bitmap;
import :index_filter_evaluators;
import :index_emvb;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import embedding_info;
import sparse_info;
import internal_types;
import statement_common;
import constant_expr;

using namespace infinity;

class TestTxnOptimizeIndex : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

        db_name_ = std::make_shared<String>("db1");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name_, table_name, MakeShared<String>(), {column_def1, column_def2});

        index_name1 = std::make_shared<std::string>("index1");
        index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

        segment_id = 0;
    }

    void TearDown() override {
        new_txn_mgr = nullptr;
        BaseTestParamStr::TearDown();
    }

    auto MakeInputBlock(const Value &v1, const Value &v2) -> SharedPtr<DataBlock> {
        u32 block_row_cnt = 8192;

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

    void PrepareForOptimizeIndex() {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name_, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name_, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name_, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("append {}", i)), TransactionType::kNormal);
                SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name_, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
    }

    void DropDB() {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

protected:
    NewTxnManager *new_txn_mgr = nullptr;

    SharedPtr<String> db_name_;
    SharedPtr<ColumnDef> column_def1;
    SharedPtr<ColumnDef> column_def2;
    SharedPtr<String> table_name;
    SharedPtr<TableDef> table_def;

    SharedPtr<String> index_name1;
    SharedPtr<IndexBase> index_def1;

    SegmentID segment_id = 0;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnOptimizeIndex,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnOptimizeIndex, optimize_index_rollback) {
    Status status;
    {
        auto check_opt_index = [this](const Vector<ChunkID> &my_chunk_ids) { // check optimize index
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);
            SharedPtr<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Optional<TableIndexMeeta> table_index_meta;
            String table_key;
            String index_key;
            Status status =
                txn->GetTableIndexMeta(*db_name_, *table_name, *index_name1, db_meta, table_meta, table_index_meta, &table_key, &index_key);
            EXPECT_TRUE(status.ok());

            {
                auto [segment_ids, status] = table_meta->GetSegmentIDs1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
            }
            SegmentID segment_id = 0;
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

            {
                auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1(txn->kv_instance());
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*chunk_ids, my_chunk_ids);
            }
            for (const auto chunk_id : my_chunk_ids) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                BufferObj *buffer_obj = nullptr;
                status = chunk_index_meta.GetIndexBuffer(txn->kv_instance(), buffer_obj);
                EXPECT_TRUE(status.ok());
            }
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        };

        PrepareForOptimizeIndex();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        check_opt_index(Vector<ChunkID>{0, 1});

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn1->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_opt_index(Vector<ChunkID>{2});

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_drop_db) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
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
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
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

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }

    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_drop_table) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
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
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
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

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name_, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_drop_index) {
    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
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
        //                        t2                drop index   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
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
        //        t2                drop index   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1        optimize index    commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                  t1                     optimize index   commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //                             t1                  optimize index (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop index   commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn2->DropIndexByName(*db_name_, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_optimize_index) {
    auto CheckTable = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name_, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase(txn->kv_instance());
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1(txn->kv_instance());
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1(txn->kv_instance());
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({2}));
    };

    Status status;
    {
        PrepareForOptimizeIndex();

        //  t1            optimize   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                optimize          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1            optimize   commit (success)
        //  |--------------|---------------|
        //                         |------------------|------------------|
        //                        t2                optimize (fail)    rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForOptimizeIndex();

        //  t1            optimize   commit (success)
        //  |--------------|---------------|
        //         |------------------|------------------|
        //        t2                optimize (fail)       rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
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

        //  t1            optimize                        commit
        //  |--------------|--------------------------------|
        //         |------------------|------------------|
        //        t2                optimize (fail)      rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index 2"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_dump_index) {
    auto PrepareForOptimizeAndDumpIndex = [&] {
        PrepareForOptimizeIndex();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name_, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    Status status;
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                dump index   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2         dump index(fail)  rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|-------------|
        //        t2            dump index(fail)  rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index(fail)    rollback
        //  |--------------|-------------------|
        //         |-----|----------|
        //        t2  dump index  commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //                  t1                optimize index(fail)  rollback
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2  dump index  commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeAndDumpIndex();

        //                             t1                  optimize index      commit
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2  dump index  commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_add_column) {
    auto PrepareForOptimizeAndDumpIndex = [&] {
        PrepareForOptimizeIndex();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name_, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    Status status;
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                add column  commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2         add column       commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|-------------|
        //        t2            add column      commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index        commit (success)
        //  |--------------|-------------------|
        //         |-----|----------|
        //        t2   add column   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //           t1      optimize index        commit (success)
        //           |----------|-------------------|
        //         |-----|----------|
        //        t2   add column   commit (success)

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //                  t1                optimize index         commit
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2  add column  commit

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeAndDumpIndex();

        //                             t1                  optimize index      commit
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2  add column  commit

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns3;
        columns3.emplace_back(column_def3);
        status = txn4->AddColumns(*db_name_, *table_name, columns3);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_drop_column) {
    auto PrepareForOptimizeAndDumpIndex = [&] {
        PrepareForOptimizeIndex();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name_, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    Status status;
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop column  commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2         drop column       commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|-------------|
        //        t2            drop column      commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index        commit (success)
        //  |--------------|-------------------|
        //         |-----|----------|
        //        t2   drop column   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //           t1      optimize index        commit (success)
        //           |----------|-------------------|
        //         |-----|----------|
        //        t2   drop column   commit (success)

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //                  t1                optimize index         commit
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2  drop column  commit

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeAndDumpIndex();

        //                             t1                  optimize index      commit
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2  drop column  commit

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name_, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_rename_table) {
    auto PrepareForOptimizeAndDumpIndex = [&] {
        PrepareForOptimizeIndex();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name_, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    Status status;
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                rename  commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2               rename       commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|-------------|
        //        t2            rename         rollback

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //  t1        optimize index        rollback
        //  |--------------|-------------------|
        //         |-----|----------|
        //        t2   rename   commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //           t1      optimize index        commit
        //           |----------|-------------------|
        //         |-----|----------|
        //        t2   rename   commit

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForOptimizeAndDumpIndex();

        //                  t1                optimize index         commit
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2    rename  commit

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        DropDB();
    }

    {
        PrepareForOptimizeAndDumpIndex();

        //                             t1                  optimize index      rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2  drop column  commit

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name_, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnOptimizeIndex, optimize_index_and_compact_table) {
    auto PrepareForCompactAndOptimize = [&] {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name_, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name_, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name_, *table_name, index_def1, ConflictType::kError);
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

        // For compact
        for (int i = 0; i < 2; ++i) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
            Status status = txn->Import(*db_name_, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        //        new_txn_mgr->PrintAllKeyValue();
        // For optimize
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("append {}", i)), TransactionType::kNormal);
                SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name_, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }

            //            new_txn_mgr->PrintAllKeyValue();
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name_, *table_name, *index_name1, 2);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name_, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](Vector<ColumnID> column_idxes) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name_, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> expected_segments{2, 3};
        EXPECT_EQ(*segment_ids_ptr, expected_segments); // 0, 1, and 2 -> 2 (unsealed) and 3

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    Status status;
    {
        PrepareForCompactAndOptimize();

        //  t1        optimize index     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                compact  commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //  t1        optimize index    commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2               compact       commit (success)

        new_txn_mgr->PrintAllKeyValue();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //  t1       optimize index     commit (success)
        //  |--------------|---------------|
        //         |------------------|-------------|
        //        t2            compact         commit(fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //  t1        optimize index         commit (success)
        //  |--------------|-------------------|
        //         |-----|----------|
        //        t2   compact   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //           t1      optimize index        commit (success)
        //           |----------|-------------------|
        //         |-----|----------|
        //        t2   compact   commit (success)

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //                  t1                optimize index         commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2    compact  commit (success)

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
    {
        PrepareForCompactAndOptimize();

        //        t1  compact    commit (success)
        //         |-----|----------|
        //                             |--------------------------|---------------|
        //                             t2                  optimize index      commit (success)

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name_, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn->OptimizeIndex(*db_name_, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1});

        DropDB();
    }
}

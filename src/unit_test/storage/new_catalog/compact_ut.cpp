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

module infinity_core:ut.compact;

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

class TestTxnCompact : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

        db_name = std::make_shared<String>("db1");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    }

    void TearDown() override {
        new_txn_mgr = nullptr;
        BaseTestParamStr::TearDown();
    }

    void PrepareForCompact() {
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
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
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

    SharedPtr<String> db_name;
    SharedPtr<ColumnDef> column_def1;
    SharedPtr<ColumnDef> column_def2;
    SharedPtr<String> table_name;
    SharedPtr<TableDef> table_def;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCompact,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCompact, compact_with_index_rollback) {
    Status status;
    {
        PrepareForCompact();
        auto index_name1 = std::make_shared<std::string>("index1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto index_name2 = std::make_shared<String>("index2");
        auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        };
        create_index(index_def1);
        create_index(index_def2);

        //  t1            compact     commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }
}

TEST_P(TestTxnCompact, compact_and_drop_db) {
    Status status;
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
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
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
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
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }
    {
        PrepareForCompact();

        //                  t1                     compact (fail)    rollback
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        PrepareForCompact();

        //                             t1                     compact (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop db    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn2->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnCompact, compact_and_drop_table) {
    Status status;
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop table    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
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
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop table    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
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
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop table    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
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
        PrepareForCompact();

        //  t1            compact     commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop table    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //                  t1                     compact (fail)    rollback
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //                             t1                     compact (fail) rollback
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop table    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn2->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_add_columns) {
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
    auto CheckTable = [&](Vector<ColumnID> column_idxes) {
        auto check_column = [&](ColumnMeta &column_meta) {
            BufferObj *column_buffer = nullptr;
            BufferObj *outline_buffer = nullptr;
            Status status = column_meta.GetColumnBuffer(column_buffer, outline_buffer);
            EXPECT_TRUE(status.ok());
            EXPECT_NE(column_buffer, nullptr);
            if (column_meta.column_idx() != 0) {
                EXPECT_NE(outline_buffer, nullptr);
            }
        };

        auto check_block = [&](BlockMeta &block_meta) {
            auto [row_cnt, status] = block_meta.GetRowCnt1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_cnt, 8192);

            for (auto column_id : column_idxes) {
                ColumnMeta column_meta(column_id, block_meta);
                check_column(column_meta);
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>{2});

        {
            SegmentID segment_id = (*segment_ids_ptr)[0];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto CheckTable1 = [&](Vector<ColumnID> column_idxes) {
        auto check_column = [&](ColumnMeta &column_meta) {
            BufferObj *column_buffer = nullptr;
            BufferObj *outline_buffer = nullptr;
            Status status = column_meta.GetColumnBuffer(column_buffer, outline_buffer);
            EXPECT_TRUE(status.ok());
            EXPECT_NE(column_buffer, nullptr);
            if (column_meta.column_idx() != 0) {
                EXPECT_NE(outline_buffer, nullptr);
            }
        };

        auto check_block = [&](BlockMeta &block_meta) {
            auto [row_cnt, status] = block_meta.GetRowCnt1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_cnt, 8192);

            for (auto column_id : column_idxes) {
                ColumnMeta column_meta(column_id, block_meta);
                check_column(column_meta);
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

            for (auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 2);

        {
            SegmentID segment_id = (*segment_ids_ptr)[0];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }

        {
            SegmentID segment_id = (*segment_ids_ptr)[1];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    Status status;
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                add column    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1, 2});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                add column    commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable({0, 1});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                add column    commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable({0, 1});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   add column    commit (success)
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckTable1({0, 1, 2});
        DropDB();
    }
    {
        PrepareForCompact();

        //                  t1                     compact     commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   add column    commit (success)
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckTable1({0, 1, 2});
        DropDB();
    }
    {
        PrepareForCompact();

        //                             t1                     compact         commit (success)
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   add column    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        status = txn2->AddColumns(*db_name, *table_name, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0, 1, 2});
        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_drop_columns) {
    auto CheckTable = [&](const Vector<ColumnID> &column_idxes, const Vector<SegmentID> &segment_ids) {
        auto check_column = [&](ColumnMeta &column_meta) {
            BufferObj *column_buffer = nullptr;
            BufferObj *outline_buffer = nullptr;
            Status status = column_meta.GetColumnBuffer(column_buffer, outline_buffer);
            EXPECT_TRUE(status.ok());
            EXPECT_NE(column_buffer, nullptr);
            // EXPECT_NE(outline_buffer, nullptr);
        };

        auto check_block = [&](BlockMeta &block_meta) {
            auto [row_cnt, status] = block_meta.GetRowCnt1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_cnt, 8192);

            for (auto column_id : column_idxes) {
                ColumnMeta column_meta(column_id, block_meta);
                check_column(column_meta);
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());

            for (auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        {
            SegmentID segment_id = (*segment_ids_ptr)[0];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    Status status;
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                drop column    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                drop column    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable({0, 1}, {2});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                drop column    commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable({0, 1}, {2});
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact     commit (fail)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   drop column    commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckTable({0}, {0, 1});
        DropDB();
    }
    {
        PrepareForCompact();

        //                  t1                     compact     commit (fail)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop column    commit
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckTable({0}, {0, 1});
        DropDB();
    }
    {
        PrepareForCompact();

        //                             t1                     compact         commit (success)
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   drop column    commit
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});
        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_rename_table) {
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    {
        PrepareForCompact();

        //  t1            compact      commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                rename          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);
        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact      commit (success)
        //  |--------------|---------------|
        //                           |------------------|------------------|
        //                          t2                rename          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact      commit (success)
        //  |--------------|---------------|
        //               |---------------------|------------------|
        //              t2                 rename          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact      commit (success)
        //  |--------------|---------------|
        //               |---------------|------------------|
        //              t2              rename          commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //  t1                 compact      commit (success)
        //  |--------------------|---------------|
        //      |-------------|----------------------|
        //      t2            rename              commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);

        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //  t1                 compact        commit (success)
        //  |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);
        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //            t1                 compact        commit (success)
        //             |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);
        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
    {
        PrepareForCompact();

        //                                t1                 compact        commit (success)
        //                                |--------------------|-----------------------|
        //      |-------------|----------------------|
        //      t2            rename              commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename"), TransactionType::kNormal);
        status = txn2->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_compact) {
    auto CheckTable = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 1);
    };

    Status status;
    {
        PrepareForCompact();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|------------------|
        //                                    t2                compact (fail)     commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact1"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        //        new_txn_mgr->PrintAllKeyValue();

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact2"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //                         |------------------|------------------|
        //                        t2                compact           commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact1"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact2"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact   commit (success)
        //  |--------------|---------------|
        //         |------------------|------------------|
        //        t2                compact           commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact2"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            compact                        commit (fail)
        //  |--------------|--------------------------------|
        //         |------------------|------------------|
        //        t2                compact          commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact1"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact2"), TransactionType::kNormal);

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckTable();
        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_create_index) {
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

    auto CheckTable = [&](const Vector<SegmentID> &segment_ids = {2}) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        Vector<String> *index_ids_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_ids_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_names_ptr, Vector<String>({*index_name1}));
        TableIndexMeeta table_index_meta((*index_ids_strs_ptr)[0], *table_meta);

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta.GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, segment_ids);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto CheckWithNoIndex = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2}));

        Vector<String> *index_ids_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_ids_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_ids_strs_ptr, Vector<String>({}));
    };

    auto CheckWithNoCompact = [&] { return CheckTable(Vector<SegmentID>({0, 1})); };

    Status status;
    {
        PrepareForCompact();

        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                compact    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //                         |------------------|----------|
        //                        t2                compact    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckWithNoCompact();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |------------------|----------|
        //        t2                compact    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckWithNoCompact();
        DropDB();
    }
    {
        PrepareForCompact();

        //  t1            create index   commit (success)
        //  |--------------|---------------|
        //         |-----|----------|
        //        t2   compact    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckWithNoIndex();
        DropDB();
    }
    {
        PrepareForCompact();

        //                  t1                     create index   commit (success)
        //                  |--------------------------|---------------|
        //         |-----|----------|
        //        t2   compact    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckWithNoIndex();
        DropDB();
    }
    {
        PrepareForCompact();

        //                             t1                     create index     commit
        //                             |--------------------------|---------------|
        //         |-----|----------|
        //        t2   compact    commit

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable();
        DropDB();
    }
}

TEST_P(TestTxnCompact, compact_and_drop_index) {
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

    [[maybe_unused]] auto CheckTable = [&](const Vector<SegmentID> &segment_ids = {2}) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        Vector<String> *index_ids_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_ids_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_names_ptr, Vector<String>({*index_name1}));
        TableIndexMeeta table_index_meta((*index_ids_strs_ptr)[0], *table_meta);

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta.GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, segment_ids);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto CheckWithNoIndex = [&](const Vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        Vector<String> *index_ids_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_ids_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_ids_strs_ptr, Vector<String>({}));
    };

    [[maybe_unused]] auto CheckWithNoCompact = [&] { return CheckTable(Vector<SegmentID>({0, 1})); };

    auto PrepareForCompact = [&] {
        this->PrepareForCompact();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    Status status;
    {
        PrepareForCompact();

        //  t1            drop index   commit (success)
        //  |--------------|---------------|
        //                                     |------------------|----------|
        //                                    t2                compact    commit

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }

    {
        PrepareForCompact();

        //    t1      drop index       commit (success)
        //    |----------|---------|
        //                    |------------------|----------------|
        //                    t2             compact       commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckWithNoIndex({0,1});
        DropDB();
    }
    {
        PrepareForCompact();

        //    t1      drop index                       commit (success)
        //    |----------|--------------------------------|
        //                    |-----------------------|-------------------------|
        //                    t2                  compact              commit (fail)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckWithNoIndex({0,1});
        DropDB();
    }
    {
        PrepareForCompact();

        //    t1      drop index                                   commit (success)
        //    |----------|-----------------------------------------------|
        //                    |-------------|-----------------------|
        //                    t2        compact            commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }

    {
        PrepareForCompact();

        //    t1                                      drop index                              commit (success)
        //    |------------------------------------------|------------------------------------------|
        //                    |----------------------|------------------------------|
        //                    t2                  compact                    commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }
    {
        PrepareForCompact();

        //    t1                                                   drop index                                   commit (success)
        //    |------------------------------------------------------|------------------------------------------|
        //                    |----------------------|------------|
        //                    t2                  compact   commit (success)

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }
    {
        PrepareForCompact();

        //                                                  t1                drop index                                 commit (success)
        //                                                  |--------------------|------------------------------------------|
        //                    |----------------------|---------------|
        //                    t2                   compact     commit (success)

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }
    {
        PrepareForCompact();

        //                                                           t1               drop index                             commit (success)
        //                                                          |--------------------|------------------------------------------|
        //                    |-----------------|------------|
        //                    t2              compact     commit (success)

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckWithNoIndex({2});
        DropDB();
    }
}

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

#include "type/complex/embedding_type.h"
#include "gtest/gtest.h"
import base_test;

import stl;
import global_resource_usage;
import storage;
import infinity_context;

import txn_manager;
import table_def;
import data_block;
import value;
import txn_store;
import buffer_manager;
import meta_state;
import wal_entry;
import infinity_exception;
import status;
import column_vector;
import physical_import;
import txn;
import catalog;
import index_base;
import index_ivf;
import index_hnsw;
import index_full_text;
import bg_task;
import background_process;
import default_values;
import base_table_ref;
import internal_types;
import logical_type;
import embedding_info;
import extra_ddl_info;
import knn_expr;
import column_def;
import statement_common;
import data_type;

import segment_entry;
import block_entry;
import block_column_entry;
import table_index_entry;
import base_entry;
import compilation_config;
import compaction_process;
import txn_state;
import new_txn;
import new_txn_manager;
import wal_manager;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import db_meeta;
import new_catalog;

using namespace infinity;

class WalReplayTest : public BaseTestParamStr {
protected:
    static std::shared_ptr<std::string> config_path() {
        return GetParam() == BaseTestParamStr::NULL_CONFIG_PATH
                   ? std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp.toml")
                   : std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp_vfs_off.toml");
    }

    void SetUp() override {
        CleanupDbDirs();
        tree_cmd = "tree ";
        tree_cmd += GetFullDataDir();
    }

    void TearDown() override {
        // system(tree_cmd.c_str());
        //        RemoveDbDirs();
    }

    String tree_cmd;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         WalReplayTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(WalReplayTest, wal_replay_database) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();

        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            txn->CreateDatabase("db1", ConflictType::kIgnore, MakeShared<String>());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            txn->CreateDatabase("db2", ConflictType::kIgnore, MakeShared<String>());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            txn->CreateDatabase("db3", ConflictType::kIgnore, MakeShared<String>());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            txn->CreateDatabase("db4", ConflictType::kIgnore, MakeShared<String>());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            txn->CreateDatabase("db5", ConflictType::kIgnore, MakeShared<String>());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
            txn->DropDatabase("db1", ConflictType::kIgnore);
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }

    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
            Status status = txn->DropDatabase("db4", ConflictType::kError);
            EXPECT_EQ(status.ok(), true);
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
            EXPECT_EQ(status.ok(), true);
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_P(WalReplayTest, wal_replay_tables) {

    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl2"), MakeShared<String>(), columns);
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status2 = txn2->CreateTable("default_db", std::move(tbl2_def), ConflictType::kIgnore);
            EXPECT_TRUE(status2.ok());
            txn_mgr->CommitTxn(txn2);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->DropTable("default_db", "tbl2", ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl3"), MakeShared<String>(), columns);
            auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn3->CreateTable("default_db", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn3);
        }
        {
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl2"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl2_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->DropTable("default_db", "tbl3", ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_P(WalReplayTest, wal_replay_append) {
    SizeT row_count = 2;
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();

        Vector<SharedPtr<ColumnDef>> columns;
        {
            i64 column_id = 0;
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kUnique);
                constraints.insert(ConstraintType::kNotNull);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kPrimaryKey);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kNotNull);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
        }

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl3"), MakeShared<String>(), columns);
            auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn3->CreateTable("default_db", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn3);
        }
        {
            auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl4"), MakeShared<String>(), columns);
            auto *txn4 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn4->CreateTable("default_db", std::move(tbl4_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn4);
        }
        {
            auto *txn5 = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();

            // Prepare the input data block
            Vector<SharedPtr<DataType>> column_types;
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

            input_block->Init(column_types, row_count);
            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(0, Value::MakeTinyInt(5));
            }

            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(1, Value::MakeBigInt(1000));
            }

            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(2, Value::MakeDouble(0.1));
            }
            input_block->Finalize();
            EXPECT_EQ(input_block->Finalized(), true);
            txn5->Append("default_db", "tbl4", input_block);
            txn_mgr->CommitTxn(txn5);
        }
        {
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    // Restart the db instance
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        Vector<SharedPtr<ColumnDef>> columns;
        {
            i64 column_id = 0;
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kUnique);
                constraints.insert(ConstraintType::kNotNull);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
        }
        {
            auto tbl5_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl5"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

            Status status = txn->CreateTable("default_db", std::move(tbl5_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }

        auto check_table = [&] {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            Status status;

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            status = txn->GetTableMeta("default_db", "tbl4", db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            auto check_block = [&](BlockMeta &block_meta) {
                Value v1 = Value::MakeTinyInt(5);
                Value v2 = Value::MakeBigInt(1000);
                Value v3 = Value::MakeDouble(0.1);

                SizeT block_row_count = 0;
                // std::tie(row_count, status) = block_meta.GetRowCnt();
                std::tie(block_row_count, status) = block_meta.GetRowCnt1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_row_count, row_count);

                auto check_column = [&](ColumnID column_id, const Value &v) {
                    ColumnMeta column_meta(column_id, block_meta);
                    ColumnVector col1;
                    status = NewCatalog::GetColumnVector(column_meta, block_row_count, ColumnVectorTipe::kReadOnly, col1);
                    EXPECT_TRUE(status.ok());

                    for (u32 i = 0; i < block_row_count; ++i) {
                        EXPECT_EQ(col1.GetValue(i), v);
                    }
                };

                check_column(0, v1);
                check_column(1, v2);
                check_column(2, v3);
            };

            auto check_segment = [&](SegmentMeta &segment_meta) {
                Vector<BlockID> *block_ids_ptr = nullptr;
                std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

                for (BlockID block_id : *block_ids_ptr) {
                    BlockMeta block_meta(block_id, segment_meta);
                    check_block(block_meta);
                }
            };

            {
                Vector<SegmentID> *segment_ids_ptr = nullptr;
                std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
                SegmentID segment_id = (*segment_ids_ptr)[0];
                SegmentMeta segment_meta(segment_id, *table_meta);
                check_segment(segment_meta);
            }
        };

        check_table();
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_P(WalReplayTest, wal_replay_import) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();

        Vector<SharedPtr<ColumnDef>> columns;
        {
            i64 column_id = 0;
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kUnique);
                constraints.insert(ConstraintType::kNotNull);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kPrimaryKey);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
            {
                std::set<ConstraintType> constraints;
                constraints.insert(ConstraintType::kNotNull);
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
        }

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl2"), MakeShared<String>(), columns);
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn2->CreateTable("default_db", std::move(tbl2_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn2);
        }
        {
            auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl3"), MakeShared<String>(), columns);
            Status status = txn->CreateTable("default_db", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        u32 block_row_cnt = 8192;
        auto make_input_block = [&] {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, Value v1) {
                for (u32 i = 0; i < block_row_cnt; ++i) {
                    col.AppendValue(v1);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kTinyInt)));
                col1->Initialize();
                append_to_col(*col1, Value::MakeTinyInt(100));
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kBigInt)));
                col2->Initialize();
                append_to_col(*col2, Value::MakeBigInt(1000));
                input_block->InsertVector(col2, 1);
            }
            {
                auto col3 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kDouble)));
                col3->Initialize();
                append_to_col(*col3, Value::MakeDouble(0.1));
                input_block->InsertVector(col3, 2);
            }
            input_block->Finalize();
            return input_block;
        };

        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            Status status = txn->Import("default_db", "tbl3", input_blocks);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    // Restart the db instance
    //    system(tree_cmd.c_str());
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        {
            u32 row_count = 8192;
            auto check_table = [&] {
                auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
                Status status;

                Optional<DBMeeta> db_meta;
                Optional<TableMeeta> table_meta;
                status = txn->GetTableMeta("default_db", "tbl3", db_meta, table_meta);
                EXPECT_TRUE(status.ok());

                auto check_block = [&](BlockMeta &block_meta) {
                    Value v1 = Value::MakeTinyInt(100);
                    Value v2 = Value::MakeBigInt(1000);
                    Value v3 = Value::MakeDouble(0.1);

                    SizeT block_row_count = 0;
                    // std::tie(row_count, status) = block_meta.GetRowCnt();
                    std::tie(block_row_count, status) = block_meta.GetRowCnt1();
                    EXPECT_TRUE(status.ok());
                    EXPECT_EQ(block_row_count, row_count);

                    auto check_column = [&](ColumnID column_id, const Value &v) {
                        ColumnMeta column_meta(column_id, block_meta);
                        ColumnVector col1;
                        status = NewCatalog::GetColumnVector(column_meta, block_row_count, ColumnVectorTipe::kReadOnly, col1);
                        EXPECT_TRUE(status.ok());

                        for (u32 i = 0; i < block_row_count; ++i) {
                            EXPECT_EQ(col1.GetValue(i), v);
                        }
                    };

                    check_column(0, v1);
                    check_column(1, v2);
                    check_column(2, v3);
                };

                auto check_segment = [&](SegmentMeta &segment_meta) {
                    Vector<BlockID> *block_ids_ptr = nullptr;
                    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
                    EXPECT_TRUE(status.ok());
                    EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));

                    for (BlockID block_id : *block_ids_ptr) {
                        BlockMeta block_meta(block_id, segment_meta);
                        check_block(block_meta);
                    }
                };

                {
                    Vector<SegmentID> *segment_ids_ptr = nullptr;
                    std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
                    EXPECT_TRUE(status.ok());

                    EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
                    SegmentID segment_id = (*segment_ids_ptr)[0];
                    SegmentMeta segment_meta(segment_id, *table_meta);
                    check_segment(segment_meta);
                }
            };

            check_table();
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

// FIXME: The test case diverges from the original intent.
// TODO: Support compact for vfs
TEST_F(WalReplayTest, wal_replay_compact) {
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp.toml");
    u64 test_segment_n = 2;
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        Vector<SharedPtr<ColumnDef>> columns;
        std::set<ConstraintType> constraints;
        auto column_def_ptr = MakeShared<ColumnDef>(0, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
        columns.emplace_back(column_def_ptr);
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }

        u32 block_row_cnt = 8192;
        auto make_input_block = [&] {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
                for (u32 i = 0; i < block_row_cnt; i += 2) {
                    col.AppendValue(v1);
                    col.AppendValue(v2);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(column_def_ptr->type());
                col1->Initialize();
                append_to_col(*col1, Value::MakeTinyInt(1), Value::MakeTinyInt(2));
                input_block->InsertVector(col1, 0);
            }
            input_block->Finalize();
            return input_block;
        };

        for (u64 i = 0; i < test_segment_n; ++i) { // add 2 segments
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
            Status status = txn->Import("default_db", "tbl1", input_blocks);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            // add compact
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
            Status status = txn->Compact("default_db", "tbl1", {0, 1});
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        txn_mgr->PrintAllKeyValue();

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    // Restart db instance
    //    system(tree_cmd.c_str());
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        {
            auto txn = txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
            txn_mgr->PrintAllKeyValue();
            {
                auto [table_info, status] = txn->GetTableInfo("default_db", "tbl1");
                EXPECT_NE(table_info, nullptr);
                EXPECT_EQ(table_info->segment_count_, 1);
            }

            {
                auto [segment_info, status] = txn->GetSegmentInfo("default_db", "tbl1", 2);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(segment_info->row_count_, 16384);
            }

            {
                auto [block_info, status] = txn->GetBlockInfo("default_db", "tbl1", 2, 0);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_info->row_count_, 8192);
            }

            {
                auto [block_info, status] = txn->GetBlockInfo("default_db", "tbl1", 2, 1);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_info->row_count_, 8192);
            }
            txn_mgr->CommitTxn(txn);
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_P(WalReplayTest, wal_replay_create_index_IvfFlat) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        // CREATE TABLE test_annivfflat (col1 embedding(float,128));
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
            auto tbl1_def =
                MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("test_annivfflat"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        // CreateIndex
        {
            Vector<String> columns1{"col1"};
            Vector<InitParameter *> parameters1;
            parameters1.emplace_back(new InitParameter("metric", "l2"));
            parameters1.emplace_back(new InitParameter("plain_storage_data_type", "f32"));

            SharedPtr<String> index_name = MakeShared<String>("idx1");
            auto index_base_ivf = IndexIVF::Make(index_name, MakeShared<String>("test comment"), "idx1_tbl1", columns1, parameters1);
            for (auto *init_parameter : parameters1) {
                delete init_parameter;
            }

            const String &db_name = "default_db";
            const String &table_name = "test_annivfflat";
            ConflictType conflict_type = ConflictType::kError;

            auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn3->CreateIndex(db_name, table_name, index_base_ivf, conflict_type);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    ////////////////////////////////
    /// Restart the db instance...
    ////////////////////////////////
    //    system(tree_cmd.c_str());
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        NewTxnManager *txn_mgr = storage->new_txn_manager();

        {
            auto txn = txn_mgr->BeginTxn(MakeUnique<String>("get index info"), TransactionType::kRead);
            auto [table_index_info, status1] = txn->GetTableIndexInfo("default_db", "test_annivfflat", "idx1");
            EXPECT_TRUE(status1.ok());
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_P(WalReplayTest, wal_replay_create_index_hnsw) {
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        // BufferManager *buffer_manager = storage->buffer_manager();

        // CREATE TABLE test_hnsw (col1 embedding(float,128));
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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("test_hnsw"), MakeShared<String>(), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        // CreateIndex
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);

            Vector<String> columns1{"col1"};
            Vector<InitParameter *> parameters1;
            parameters1.emplace_back(new InitParameter("metric", "l2"));
            parameters1.emplace_back(new InitParameter("encode", "plain"));
            parameters1.emplace_back(new InitParameter("m", "16"));
            parameters1.emplace_back(new InitParameter("ef_construction", "200"));

            SharedPtr<String> index_name = MakeShared<String>("hnsw_index");
            auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), "hnsw_index_test_hnsw", columns1, parameters1);
            for (auto *init_parameter : parameters1) {
                delete init_parameter;
            }

            const String &db_name = "default_db";
            const String &table_name = "test_hnsw";
            ConflictType conflict_type = ConflictType::kError;
            bool prepare = false;
            auto [table_entry, table_status] = txn->GetTableByName(db_name, table_name);
            EXPECT_EQ(table_status.ok(), true);
            {
                auto table_ref = BaseTableRef::FakeTableRef(txn, db_name, table_name);
                auto result = txn->CreateIndexDef(table_entry, index_base_hnsw, conflict_type);
                auto *table_index_entry = std::get<0>(result);
                auto status = std::get<1>(result);
                EXPECT_EQ(status.ok(), true);
                txn->CreateIndexPrepare(table_index_entry, table_ref.get(), prepare);
                txn->CreateIndexFinish(table_entry, table_index_entry);
            }
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
    ////////////////////////////////
    /// Restart the db instance...
    ////////////////////////////////
    //    system(tree_cmd.c_str());
    {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = WalReplayTest::config_path();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto txn = txn_mgr->BeginTxn(MakeUnique<String>("check index"), TransactionType::kNormal);
            Vector<ColumnID> column_ids{0};
            auto [table_entry, status] = txn->GetTableByName("default_db", "test_hnsw");
            EXPECT_NE(table_entry, nullptr);

            auto table_index_meta = table_entry->GetIndexMetaPtrByName("hnsw_index");

            EXPECT_NE(table_index_meta, nullptr);
            EXPECT_EQ(*table_index_meta->index_name(), "hnsw_index");
            //            EXPECT_EQ(table_index_meta->index_entry_list().size(), 1u);
            //            auto table_index_entry_front = static_cast<TableIndexEntry *>(table_index_meta->index_entry_list().front().get());
            //            EXPECT_EQ(*table_index_entry_front->index_base()->index_name_, "hnsw_index");
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

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

#include "gtest/gtest.h"

import base_test;

import stl;
import global_resource_usage;
import storage;
import infinity_context;
import compilation_config;
import extra_ddl_info;
import infinity_exception;
import log_file;
import config;
import bg_task;
import background_process;
import default_values;
import status;
import logger;
import column_def;
import column_vector;
import data_type;
import logical_type;
import table_def;
import data_block;
import value;
import internal_types;
import buffer_manager;
import physical_import;
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

class RepeatReplayTest : public BaseTestParamStr {
protected:
    static std::shared_ptr<std::string> close_ckp_config() {
        return GetParam() == BaseTestParamStr::NULL_CONFIG_PATH
                   ? std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp.toml")
                   : std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_ckp_vfs_off.toml");
    }

    void SetUp() override { CleanupDbDirs(); }

    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         RepeatReplayTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(RepeatReplayTest, append) {
    std::shared_ptr<std::string> config_path = RepeatReplayTest::close_ckp_config();

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto TestAppend = [&]() {
        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);

        Vector<SharedPtr<ColumnVector>> column_vectors;
        for (SizeT i = 0; i < table_def->columns().size(); ++i) {
            SharedPtr<DataType> data_type = table_def->columns()[i]->type();
            column_vectors.push_back(MakeShared<ColumnVector>(data_type));
            column_vectors.back()->Initialize();
        }
        {
            int v1 = 1;
            column_vectors[0]->AppendByPtr(reinterpret_cast<const_ptr_t>(&v1));
        }
        {
            std::string v2 = "v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2";
            column_vectors[1]->AppendByStringView(v2);
        }
        auto data_block = DataBlock::Make();
        data_block->Init(column_vectors);

        auto [table_entry, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        status = txn->Append(*db_name, *table_name, data_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto CheckTable = [&](NewTxnManager *new_txn_mgr, size_t row_cnt) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kRead);
        Status status;

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());

        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];
        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            Value v1 = Value::MakeInt(1);
            Value v2 = Value::MakeVarchar("v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2");

            SizeT row_count = 0;
            // std::tie(row_count, status) = block_meta.GetRowCnt();
            std::tie(row_count, status) = block_meta.GetRowCnt1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_count, row_cnt);

            auto check_column = [&](ColumnID column_id, const Value &v) {
                ColumnMeta column_meta(column_id, block_meta);
                ColumnVector col1;
                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col1);
                EXPECT_TRUE(status.ok());

                for (u32 i = 0; i < row_count; ++i) {
                    EXPECT_EQ(col1.GetValue(i), v);
                }
            };

            check_column(0, v1);
            check_column(1, v2);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        // Ealier test may leave dirty infinity instance. Destroy it first.
        infinity::InfinityContext::instance().UnInit();
        CleanupDbDirs();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        {
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            Status status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
            auto [table_entry, status] = txn->GetTableInfo(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        TestAppend();
        infinity::InfinityContext::instance().UnInit();
    }
    { // replay with no checkpoint, only replay wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize
        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        CheckTable(new_txn_mgr, 1);
        TestAppend();
        CheckTable(new_txn_mgr, 2);
        infinity::InfinityContext::instance().UnInit();
    }
    { // replay with checkpoint + wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize

        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        CheckTable(new_txn_mgr, 2);
        {
            //  manually add checkpoint
            std::shared_ptr<TxnTimeStamp> ckp_commit_ts = std::make_shared<TxnTimeStamp>(0);
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            WalManager *wal_manager_{};
            wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check point"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn, ckp_commit_ts.get());
            EXPECT_TRUE(status.ok());
        }
        TestAppend();
        CheckTable(new_txn_mgr, 3);
        infinity::InfinityContext::instance().UnInit();
    }
    for (int i = 0; i < 2; ++i) { // replay with checkpoint + wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize
        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        CheckTable(new_txn_mgr, 3);
        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_P(RepeatReplayTest, import) {
    std::shared_ptr<std::string> config_path = RepeatReplayTest::close_ckp_config();

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    SizeT block_row_cnt = 8192;
    auto TestImport = [&](NewTxnManager *new_txn_mgr, BufferManager *buffer_mgr) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import table"), TransactionType::kNormal);
        auto make_input_block = [&](const Value &v1, const Value &v2) {
            auto input_block = MakeShared<DataBlock>();
            auto make_column = [&](const Value &v) {
                auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
                col->Initialize();
                for (SizeT i = 0; i < block_row_cnt; ++i) {
                    col->AppendValue(v);
                }
                return col;
            };
            {
                auto col1 = make_column(v1);
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = make_column(v2);
                input_block->InsertVector(col2, 1);
            }
            input_block->Finalize();
            return input_block;
        };

        Status status =
            txn->Import(*db_name,
                        *table_name,
                        Vector<SharedPtr<DataBlock>>{make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto CheckTable = [&](NewTxnManager *new_txn_mgr, size_t row_cnt) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        auto [table_entry, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        // Earlier cases may leave dirty infinity instance. Destroy it first.
        infinity::InfinityContext::instance().UnInit();
        CleanupDbDirs();
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();
        Storage *storage = InfinityContext::instance().storage();

        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        BufferManager *buffer_mgr = storage->buffer_manager();
        {
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            Status status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
            auto [table_entry, status] = txn->GetTableInfo(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        TestImport(new_txn_mgr, buffer_mgr);
        CheckTable(new_txn_mgr, 1);
        infinity::InfinityContext::instance().UnInit();
    }
    { // replay with no checkpoint, only replay wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize
        Storage *storage = InfinityContext::instance().storage();
        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        BufferManager *buffer_mgr = storage->buffer_manager();
        CheckTable(new_txn_mgr, 1);
        TestImport(new_txn_mgr, buffer_mgr);
        CheckTable(new_txn_mgr, 2);
        infinity::InfinityContext::instance().UnInit();
    }
    { // replay with checkpoint + wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize
        Storage *storage = InfinityContext::instance().storage();

        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        BufferManager *buffer_mgr = storage->buffer_manager();
        CheckTable(new_txn_mgr, 2);
        {
            //  manually add checkpoint
            std::shared_ptr<TxnTimeStamp> ckp_commit_ts = std::make_shared<TxnTimeStamp>(0);
            NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
            WalManager *wal_manager_{};
            wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn, ckp_commit_ts.get());
            EXPECT_TRUE(status.ok());
        }
        TestImport(new_txn_mgr, buffer_mgr);
        CheckTable(new_txn_mgr, 3);
        infinity::InfinityContext::instance().UnInit();
    }
    for (int i = 0; i < 2; ++i) { // replay with checkpoint + wal
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2(); // auto checkpoint when initialize

        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        CheckTable(new_txn_mgr, 3);
        infinity::InfinityContext::instance().UnInit();
    }
}
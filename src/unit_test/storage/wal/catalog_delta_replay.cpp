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

#include "unit_test/base_test.h"

import statement_common;
import internal_types;
import stl;
import infinity_context;
import storage;
import column_def;
import logical_type;
import data_type;
import txn_manager;
import table_def;
import extra_ddl_info;
import column_vector;
import value;
import catalog;
import segment_entry;
import block_entry;
import buffer_manager;
import physical_import;
import status;
import compilation_config;
import cleanup_task;
import compact_segments_task;
import index_base;
import index_base;
import third_party;
import base_table_ref;
import index_secondary;
import data_block;
import query_context;
import txn;
import index_base;
import index_full_text;
import fulltext_index_entry;
import bg_task;
import logger;

using namespace infinity;

class CatalogDeltaReplayTest : public BaseTest {
protected:
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override {}

    void WaitFlushDeltaOp(TxnManager *txn_mgr, TxnTimeStamp last_commit_ts) {
        TxnTimeStamp visible_ts = 0;
        do {
            visible_ts = txn_mgr->GetMinUnflushedTS();
            // // sleep for 1s
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (visible_ts <= last_commit_ts);
    }

    void AddSegments(TxnManager *txn_mgr, const String &table_name, const Vector<SizeT> &segment_sizes, BufferManager *buffer_mgr) {
        for (SizeT segment_size : segment_sizes) {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            auto [table_entry, status] = txn->GetTableByName("default", table_name);
            table_entry->SetCompactionAlg(nullptr); // close auto compaction to test manual compaction
            auto column_count = table_entry->ColumnCount();

            SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
            auto segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
            auto block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, column_count, txn);

            while (segment_size > 0) {
                SizeT write_size = std::min(SizeT(DEFAULT_BLOCK_CAPACITY), segment_size);
                segment_size -= write_size;
                Vector<ColumnVector> column_vectors;
                {
                    auto column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kTinyInt));
                    column_vector.Initialize();
                    Value v = Value::MakeTinyInt(static_cast<TinyIntT>(1));
                    for (int i = 0; i < (int)write_size; ++i) {
                        column_vector.AppendValue(v);
                    }
                    column_vectors.push_back(std::move(column_vector));
                }
                block_entry->AppendBlock(column_vectors, 0, write_size, buffer_mgr);
                segment_entry->AppendBlockEntry(std::move(block_entry));
                block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, 1, txn);
            }
            PhysicalImport::SaveSegmentData(table_entry, txn, segment_entry);
            txn_mgr->CommitTxn(txn);
        }
    }
};

TEST_F(CatalogDeltaReplayTest, replay_db_entry) {
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name1 = std::make_shared<std::string>("db1");
    auto db_name2 = std::make_shared<std::string>("db2");
    auto db_name3 = std::make_shared<std::string>("db3");

    std::shared_ptr<std::string> db_entry_dir1;
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateDatabase(*db_name1, ConflictType::kError);
            txn->CreateDatabase(*db_name2, ConflictType::kError);
            txn->DropDatabase(*db_name2, ConflictType::kError);

            auto [db_entry, status] = txn->GetDatabase(*db_name1);
            EXPECT_TRUE(status.ok());
            db_entry_dir1 = db_entry->db_entry_dir();

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
            txn->CreateDatabase(*db_name3, ConflictType::kError);
            txn_mgr->RollBackTxn(txn);
        }
        WaitFlushDeltaOp(txn_mgr, last_commit_ts);

        infinity::InfinityContext::instance().UnInit();
    }
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
            {
                auto [db_entry, status] = txn->GetDatabase(*db_name1);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*db_entry->db_name_ptr(), *db_name1);
                EXPECT_EQ(*db_entry->db_entry_dir(), *db_entry_dir1);
            }
            {
                auto [db_entry, status] = txn->GetDatabase(*db_name2);
                EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
            }
            {
                auto [db_entry, status] = txn->GetDatabase(*db_name3);
                EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
            }
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_F(CatalogDeltaReplayTest, replay_table_entry) {
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default");

    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto column_def2 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});
    auto table_name1 = std::make_shared<std::string>("tb1");
    auto table_name2 = std::make_shared<std::string>("tb2");
    auto table_name3 = std::make_shared<std::string>("tb3");
    auto table_def1 = TableDef::Make(db_name, table_name1, {column_def1, column_def2});
    auto table_def2 = TableDef::Make(db_name, table_name2, {column_def1, column_def2});
    auto table_def3 = TableDef::Make(db_name, table_name3, {column_def1, column_def2});

    std::shared_ptr<std::string> table_entry_dir1;
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def1, ConflictType::kError);
            txn->CreateTable(*db_name, table_def2, ConflictType::kError);
            txn->DropTableCollectionByName(*db_name, *table_name2, ConflictType::kError);

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name1);
            EXPECT_TRUE(status.ok());
            table_entry_dir1 = table_entry->TableEntryDir();

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def3, ConflictType::kError);
            txn_mgr->RollBackTxn(txn);
        }
        WaitFlushDeltaOp(txn_mgr, last_commit_ts);

        infinity::InfinityContext::instance().UnInit();
    }
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
            {
                auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name1);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*table_entry->GetTableName(), *table_name1);
                EXPECT_EQ(*table_entry->TableEntryDir(), *table_entry_dir1);
            }
            {
                auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name2);
                EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
            }
            {
                auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name3);
                EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
            }
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_F(CatalogDeltaReplayTest, replay_import) {
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default");

    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto column_def2 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, {column_def1, column_def2});

    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
            {
                BlockID block_id = 0;
                UniquePtr<BlockEntry> block_entry =
                    BlockEntry::NewBlockEntry(segment_entry.get(), block_id, 0 /*checkpoint_ts*/, table_entry->ColumnCount(), txn);
                {
                    Vector<ColumnVector> column_vectors;
                    for (SizeT i = 0; i < table_entry->ColumnCount(); ++i) {
                        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
                    }

                    {
                        int v1 = 1;
                        column_vectors[0].AppendByPtr(reinterpret_cast<const_ptr_t>(&v1));
                    }
                    {
                        std::string v2 = "v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2";
                        column_vectors[1].AppendByStringView(v2, ',');
                    }
                    block_entry->IncreaseRowCount(1);
                }
                segment_entry->AppendBlockEntry(std::move(block_entry));
            }

            TxnTimeStamp flush_ts = txn->BeginTS();
            segment_entry->FlushNewData(flush_ts);
            txn->Import(*db_name, *table_name, segment_entry);

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        WaitFlushDeltaOp(txn_mgr, last_commit_ts);

        infinity::InfinityContext::instance().UnInit();
    }
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
            {
                auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(table_entry->row_count(), 1ul);
                ASSERT_EQ(table_entry->segment_map().size(), 1ul);
                {
                    auto &segment_entry = table_entry->segment_map().begin()->second;
                    EXPECT_EQ(segment_entry->row_count(), 1ul);
                    ASSERT_EQ(segment_entry->block_entries().size(), 1ul);
                    {
                        BlockEntry *block_entry = segment_entry->block_entries()[0].get();
                        EXPECT_EQ(block_entry->row_count(), 1ul);
                        ASSERT_EQ(block_entry->columns().size(), 2ul);
                        {
                            auto &col2 = block_entry->columns()[1];
                            EXPECT_EQ(col2->OutlineBufferCount(), 1ul);
                        }
                    }
                }
            }
            txn_mgr->CommitTxn(txn);
        }

        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_F(CatalogDeltaReplayTest, replay_append) {
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default");

    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto column_def2 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, {column_def1, column_def2});
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

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
                column_vectors[1]->AppendByStringView(v2, ',');
            }
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(status.ok());
            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        WaitFlushDeltaOp(txn_mgr, last_commit_ts);

        infinity::InfinityContext::instance().UnInit();
    }
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        {
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(table_entry->row_count(), 1ul);
            ASSERT_EQ(table_entry->segment_map().size(), 1ul);
            {
                auto &segment_entry = table_entry->segment_map().begin()->second;
                EXPECT_EQ(segment_entry->row_count(), 1ul);
                ASSERT_EQ(segment_entry->block_entries().size(), 1ul);
                {
                    BlockEntry *block_entry = segment_entry->block_entries()[0].get();
                    EXPECT_EQ(block_entry->row_count(), 1ul);
                    ASSERT_EQ(block_entry->columns().size(), 2ul);
                    {
                        auto &col2 = block_entry->columns()[1];
                        EXPECT_EQ(col2->OutlineBufferCount(), 1ul);
                    }
                }
            }
        }
            txn_mgr->CommitTxn(txn);
    }
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(CatalogDeltaReplayTest, replay_delete) {

    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default");

    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto column_def2 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, {column_def1, column_def2});

    std::shared_ptr<std::string> table_entry_dir1;
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());
            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {

            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            Vector<SharedPtr<ColumnVector>> column_vectors;
            for (SizeT i = 0; i < table_def->columns().size(); ++i) {
                SharedPtr<DataType> data_type = table_def->columns()[i]->type();
                column_vectors.push_back(MakeShared<ColumnVector>(data_type));
                column_vectors.back()->Initialize();
            }
            int v1 = 1;
            column_vectors[0]->AppendByPtr(reinterpret_cast<const_ptr_t>(&v1));
            std::string v2 = "v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2v2";
            column_vectors[1]->AppendByStringView(v2, ',');

            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(status.ok());
            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());
            auto del_row = infinity::RowID(uint64_t(table_def->columns().size() - 1));

            Vector<infinity::RowID> del_row_ids{};
            del_row_ids.push_back(del_row);
            status = txn->Delete(*db_name, *table_name, del_row_ids, true);
            EXPECT_TRUE(status.ok());
        }

        WaitFlushDeltaOp(txn_mgr, last_commit_ts);
        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_F(CatalogDeltaReplayTest, replay_with_full_checkpoint) {
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default");

    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
    auto column_def2 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_name_committed = std::make_shared<std::string>("tb_committed");
    auto table_name_uncommitted = std::make_shared<std::string>("tb_uncommitted");
    auto table_def = TableDef::Make(db_name, table_name, {column_def1, column_def2});
    auto table_def_committed = TableDef::Make(db_name, table_name_committed, {column_def1, column_def2});
    auto table_def_uncommitted = TableDef::Make(db_name, table_name_uncommitted, {column_def1, column_def2});

    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp last_commit_ts = 0;
        // create table and insert two records
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            txn->CreateTable(*db_name, table_def, ConflictType::kError);

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

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
                column_vectors[1]->AppendByStringView(v2, ',');
            }
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(status.ok());
            last_commit_ts = txn_mgr->CommitTxn(txn);
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Vector<SharedPtr<ColumnVector>> column_vectors;
            for (SizeT i = 0; i < table_def->columns().size(); ++i) {
                SharedPtr<DataType> data_type = table_def->columns()[i]->type();
                column_vectors.push_back(MakeShared<ColumnVector>(data_type));
                column_vectors.back()->Initialize();
            }
            {
                int v1 = 2;
                column_vectors[0]->AppendByPtr(reinterpret_cast<const_ptr_t>(&v1));
            }
            {
                std::string v2 = "askljhfasfhuiwqeriqkldfnramgeasfklhllfjas";
                column_vectors[1]->AppendByStringView(v2, ',');
            }
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(status.ok());
            last_commit_ts = txn_mgr->CommitTxn(txn);
        }

        // 1. remain some uncommitted txn before force full checkpoint
        // 2. wait delta checkpoint
        {
            auto *txn_uncommitted = txn_mgr->CreateTxn();
            txn_uncommitted->Begin();
            txn_uncommitted->CreateTable(*db_name, table_def_uncommitted, ConflictType::kError);

            LOG_INFO("BEFORE FULL CKP");
            auto *txn_force_ckp = txn_mgr->CreateTxn();
            txn_force_ckp->Begin();
            auto force_ckp_task = MakeShared<ForceCheckpointTask>(txn_force_ckp, true);
            storage->bg_processor()->Submit(force_ckp_task);
            force_ckp_task->Wait();
            LOG_INFO("AFTER FULL CKP");
            txn_mgr->CommitTxn(txn_force_ckp);

            txn_mgr->CommitTxn(txn_uncommitted);

            // some txn in delta checkpoint
            auto *txn_committed = txn_mgr->CreateTxn();
            txn_committed->Begin();
            txn_committed->CreateTable(*db_name, table_def_committed, ConflictType::kError);
            txn_mgr->CommitTxn(txn_committed);

            auto *txn_record3 = txn_mgr->CreateTxn();
            txn_record3->Begin();
            Vector<SharedPtr<ColumnVector>> column_vectors;
            for (SizeT i = 0; i < table_def_committed->columns().size(); ++i) {
                SharedPtr<DataType> data_type = table_def_committed->columns()[i]->type();
                column_vectors.push_back(MakeShared<ColumnVector>(data_type));
                column_vectors.back()->Initialize();
            }
            {
                int v1 = 3;
                column_vectors[0]->AppendByPtr(reinterpret_cast<const_ptr_t>(&v1));
            }
            {
                std::string v2 = "this is a test for replay with full checkpoint";
                column_vectors[1]->AppendByStringView(v2, ',');
            }
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto status = txn_record3->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(status.ok());

            last_commit_ts = txn_mgr->CommitTxn(txn_record3);
            WaitFlushDeltaOp(txn_mgr, last_commit_ts);

            infinity::InfinityContext::instance().UnInit();
        }
    }

    // now restart and the table `tb_uncommitted` should exist
    {
        InfinityContext::instance().Init(config_path);
        Storage *storage = InfinityContext::instance().storage();

        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
            {
                auto [table_uncommitted, uncommitted_status] = txn->GetTableByName(*db_name, *table_name_uncommitted);
                EXPECT_TRUE(uncommitted_status.ok());

                auto [table_committed, committed_status] = txn->GetTableByName(*db_name, *table_name_committed);
                EXPECT_TRUE(committed_status.ok());

                auto [table_entry, table_status] = txn->GetTableByName(*db_name, *table_name);
                EXPECT_TRUE(table_status.ok());

                EXPECT_EQ(table_entry->row_count(), 3ul);
                ASSERT_EQ(table_entry->segment_map().size(), 1ul);
                {
                    auto &segment_entry = table_entry->segment_map().begin()->second;
                    EXPECT_EQ(segment_entry->row_count(), 3ul);
                    ASSERT_EQ(segment_entry->block_entries().size(), 1ul);
                    {
                        BlockEntry *block_entry = segment_entry->block_entries()[0].get();
                        EXPECT_EQ(block_entry->row_count(), 3ul);
                        ASSERT_EQ(block_entry->columns().size(), 2ul);
                        {
                            auto &col2 = block_entry->columns()[1];
                            EXPECT_EQ(col2->OutlineBufferCount(), 3ul);
                        }
                    }
                }
            }
        }
        infinity::InfinityContext::instance().UnInit();
    }
}

TEST_F(CatalogDeltaReplayTest, replay_compact_to_single_rollback) {
    String table_name = "tb1";
    std::shared_ptr<std::string> config_path = nullptr;
    infinity::InfinityContext::instance().Init(config_path);

    Storage *storage = infinity::InfinityContext::instance().storage();
    BufferManager *buffer_manager = storage->buffer_manager();
    TxnManager *txn_mgr = storage->txn_manager();

    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }
    {
        // create table
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
    this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

    {
        // add compact
        auto txn4 = txn_mgr->CreateTxn();
        txn4->Begin();

        auto [table_entry, status] = txn4->GetTableByName("default", table_name);
        EXPECT_NE(table_entry, nullptr);

        {
            auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn4);
            compact_task->Execute();
        }
        // rollback
        txn_mgr->RollBackTxn(txn4);
    }

    {
        auto txn5 = txn_mgr->CreateTxn();
        txn5->Begin();
        TxnTimeStamp begin_ts = txn5->BeginTS();

        auto [table_entry, status] = txn5->GetTableByName("default", table_name);
        EXPECT_NE(table_entry, nullptr);

        size_t test_segment_n = segment_sizes.size();

        for (size_t i = 0; i < test_segment_n; ++i) {
            auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
            EXPECT_NE(segment_entry, nullptr);
            EXPECT_NE(segment_entry->status(), SegmentStatus::kDeprecated);
        }

        auto compact_segment = table_entry->GetSegmentByID(test_segment_n, begin_ts);
        EXPECT_EQ(compact_segment, nullptr);

        txn_mgr->CommitTxn(txn5);
    }
    infinity::InfinityContext::instance().UnInit();
}
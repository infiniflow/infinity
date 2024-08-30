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
import index_base;
import third_party;
import base_table_ref;
import index_secondary;
import data_block;
import query_context;
import txn;
import index_base;
import index_full_text;
import bg_task;
import logger;
import infinity_exception;
import default_values;
import global_resource_usage;
import infinity;
import background_process;
import compaction_process;
import wal_manager;

using namespace infinity;

class CheckpointTest : public BaseTestParamStr {
protected:
    static std::shared_ptr<std::string> config_path() {
        return GetParam() == BaseTestParamStr::NULL_CONFIG_PATH
                   ? std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_bgtask.toml")
                   : std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_close_bgtask_vfs_off.toml");
    }

    void SetUp() override {
        RemoveDbDirs();
        system(("mkdir -p " + String(GetFullPersistDir())).c_str());
        system(("mkdir -p " + String(GetFullDataDir())).c_str());
        system(("mkdir -p " + String(GetFullTmpDir())).c_str());
    }

    void TearDown() override { RemoveDbDirs(); }

    void WaitCleanup(Storage *storage) {
        Catalog *catalog = storage->catalog();
        BufferManager *buffer_mgr = storage->buffer_manager();
        TxnManager *txn_mgr = storage->txn_manager();
        TxnTimeStamp visible_ts = txn_mgr->GetCleanupScanTS();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, buffer_mgr);
        cleanup_task->Execute();
    }

    void WaitFlushDeltaOp(Storage *storage) {
        WalManager *wal_mgr = storage->wal_manager();
        wal_mgr->Checkpoint(false /*is_full_checkpoint*/);
    }

    void AddSegments(TxnManager *txn_mgr, const String &table_name, const Vector<SizeT> &segment_sizes, BufferManager *buffer_mgr) {
        for (SizeT segment_size : segment_sizes) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import table"));

            auto [table_entry, status] = txn->GetTableByName("default_db", table_name);
            // table_entry->SetCompactionAlg(nullptr); // close auto compaction to test manual compaction
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

    void RemoveOldWal(const String &wal_dir) {
        for (const auto &entry : std::filesystem::directory_iterator(wal_dir)) {
            if (entry.path().extension() == ".log") {
                continue;
            }
            std::filesystem::remove(entry.path());
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CheckpointTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::CONFIG_PATH));

TEST_P(CheckpointTest, test_cleanup_and_checkpoint) {
    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("test_cleanup_and_checkpoint");
    auto column_name = MakeShared<String>("col1");

#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::Init();
#endif
    std::shared_ptr<std::string> config_path = CheckpointTest::config_path();
    infinity::InfinityContext::instance().Init(config_path);

    Storage *storage = infinity::InfinityContext::instance().storage();
    BufferManager *buffer_manager = storage->buffer_manager();
    TxnManager *txn_mgr = storage->txn_manager();
    CompactionProcessor *compaction_processor = storage->compaction_processor();

    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }
    { // create table
        auto tbl1_def = MakeUnique<TableDef>(db_name, table_name, columns);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

        Status status = txn->CreateTable(*db_name, std::move(tbl1_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
    this->AddSegments(txn_mgr, *table_name, segment_sizes, buffer_manager);

    { // add compact
        auto commit_ts = compaction_processor->ManualDoCompact(*db_name, *table_name, false);
        EXPECT_NE(commit_ts, 0u);
    }

    {
        auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("get table"));
        TxnTimeStamp begin_ts = txn5->BeginTS();
        auto [table_entry, status] = txn5->GetTableByName(*db_name, *table_name);
        EXPECT_NE(table_entry, nullptr);

        size_t test_segment_n = segment_sizes.size();
        size_t row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        for (size_t i = 0; i < test_segment_n; ++i) {
            auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
            EXPECT_NE(segment_entry, nullptr);
            EXPECT_EQ(segment_entry->status(), SegmentStatus::kDeprecated);
        }
        auto compact_segment = table_entry->GetSegmentByID(test_segment_n, begin_ts);
        EXPECT_NE(compact_segment, nullptr);
        EXPECT_NE(compact_segment->status(), SegmentStatus::kDeprecated);
        EXPECT_EQ(compact_segment->actual_row_count(), row_count);

        txn_mgr->CommitTxn(txn5);
    }
    WaitCleanup(storage);
    infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
#endif
}

TEST_P(CheckpointTest, test_index_replay_with_full_and_delta_checkpoint1) {
#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::Init();
#endif
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default_db");
    auto table_name = std::make_shared<std::string>("test_index_replay_with_full_and_delta_checkpoint1");
    auto column_name = std::make_shared<std::string>("col1");
    auto index_name = std::make_shared<std::string>("idx1");
    Vector<SharedPtr<ColumnDef>> columns;

    // create table and shutdown
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        {
            i64 column_id = 0;
            {
                std::set<ConstraintType> constraints;
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), *column_name, constraints);
                columns.emplace_back(column_def_ptr);
            }
        }
        {
            auto tbl1_def = MakeUnique<TableDef>(db_name, table_name, columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
            Status status = txn->CreateTable(*db_name, std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        infinity::InfinityContext::instance().UnInit();
    }
    // create index and shutdown
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        SharedPtr<IndexBase> index_base = IndexSecondary::Make(index_name, fmt::format("{}_{}", *table_name, *index_name), {*column_name});
        auto [table_entry, status1] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status1.ok());

        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn);
        auto [table_index_entry, status2] = txn->CreateIndexDef(table_entry, index_base, ConflictType::kError);
        EXPECT_TRUE(status2.ok());
        auto [_, status3] = txn->CreateIndexPrepare(table_index_entry, table_ref.get(), false);
        txn->CreateIndexFinish(table_entry, table_index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn);

        infinity::InfinityContext::instance().UnInit();
    }
    // drop index and shutdown
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop index"));
        auto [table_entry, status1] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status1.ok());

        auto status3 = txn->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kInvalid);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn);

        WaitFlushDeltaOp(storage);

        infinity::InfinityContext::instance().UnInit();
    }
    // now restart and recreate index should be ok
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        SharedPtr<IndexBase> index_base = IndexSecondary::Make(index_name, fmt::format("{}_{}", *table_name, *index_name), {*column_name});
        auto [table_entry, status1] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status1.ok());

        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn);
        auto [table_index_entry, status2] = txn->CreateIndexDef(table_entry, index_base, ConflictType::kError);
        EXPECT_TRUE(status2.ok());
        auto [_, status3] = txn->CreateIndexPrepare(table_index_entry, table_ref.get(), false);
        txn->CreateIndexFinish(table_entry, table_index_entry);
        EXPECT_TRUE(status3.ok());

        txn_mgr->CommitTxn(txn);

        infinity::InfinityContext::instance().UnInit();
    }
#ifdef INFINITY_DEBUG
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
#endif
}

TEST_P(CheckpointTest, test_index_replay_with_full_and_delta_checkpoint2) {
    constexpr u64 kInsertN = 10;
    constexpr u64 kInsertSize = 8192;

#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::Init();
#endif
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_catalog_delta.toml");

    auto db_name = std::make_shared<std::string>("default_db");
    auto table_name = std::make_shared<std::string>("test_index_replay_with_full_and_delta_checkpoint2");
    auto column_name = std::make_shared<std::string>("col1");
    auto index_name = std::make_shared<std::string>("idx1");
    // String analyzer = "standard";
    Vector<SharedPtr<ColumnDef>> columns;
    Vector<InitParameter *> index_param_list;

    // create table
    // create index, insert data and shutdown
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        BGTaskProcessor *bg_processor = storage->bg_processor();

        {
            i64 column_id = 0;
            {
                std::set<ConstraintType> constraints;
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kVarchar)), *column_name, constraints);
                columns.emplace_back(column_def_ptr);
            }
        }
        {
            auto tbl1_def = MakeUnique<TableDef>(db_name, table_name, columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
            Status status = txn->CreateTable(*db_name, std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        SharedPtr<IndexBase> index_base =
            IndexFullText::Make(index_name, fmt::format("{}_{}", *table_name, *index_name), {*column_name}, index_param_list);
        auto [table_entry, status1] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status1.ok());

        auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn);
        auto [table_index_entry, status2] = txn->CreateIndexDef(table_entry, index_base, ConflictType::kError);
        EXPECT_TRUE(status2.ok());
        auto [_, status3] = txn->CreateIndexPrepare(table_index_entry, table_ref.get(), false);
        txn->CreateIndexFinish(table_entry, table_index_entry);
        EXPECT_TRUE(status3.ok());
        txn_mgr->CommitTxn(txn);

        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("full ckp"));
            SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn, true /*full_check_point*/);
            bg_processor->Submit(force_ckp_task);
            force_ckp_task->Wait();
            txn_mgr->CommitTxn(txn);
        }

        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            Vector<SharedPtr<ColumnVector>> column_vectors;
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(columns[0]->type());
            column_vector->Initialize();
            for (SizeT j = 0; j < kInsertSize; ++j) {
                String str = fmt::format("{}", i * 10000 + j);
                Value v = Value::MakeVarchar(str);
                column_vector->AppendValue(v);
            }
            column_vectors.push_back(column_vector);
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto append_status = txn->Append(table_entry, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }

        WaitFlushDeltaOp(storage);

        infinity::InfinityContext::instance().UnInit();
    }
    // now restart and recreate index should be ok
    {
        infinity::InfinityContext::instance().Init(config_path);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        SharedPtr<IndexBase> index_base = IndexSecondary::Make(index_name, fmt::format("{}_{}", *table_name, *index_name), {*column_name});
        auto [table_entry, status1] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status1.ok());

        txn_mgr->CommitTxn(txn);

        infinity::InfinityContext::instance().UnInit();
    }
#ifdef INFINITY_DEBUG
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
#endif
}

TEST_P(CheckpointTest, test_fullcheckpoint_withsmallest_walfile) {
#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::Init();
#endif

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("tbl1");
    Vector<SharedPtr<ColumnDef>> columns;
    Vector<SharedPtr<DataType>> column_types;
    {
        std::set<ConstraintType> constraints;
        auto column_def_ptr = MakeShared<ColumnDef>(0, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints);
        columns.emplace_back(column_def_ptr);
        column_types.emplace_back(column_def_ptr->type());
    }

    String wal_dir;
    int insert_n = 100;
    {
        infinity::InfinityContext::instance().Init(nullptr /*config_path*/);
        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {

            auto tbl_def = MakeUnique<TableDef>(db_name, table_name, columns);

            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
            auto status = txn->CreateTable(*db_name, std::move(tbl_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        auto append = [&] {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));
            auto [table_entry, get_status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(get_status.ok());

            SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();
            input_block->Init(column_types);
            for (int i = 0; i < insert_n; ++i) {
                input_block->AppendValue(0 /*column_idx*/, Value::MakeInt(i));
            }
            input_block->Finalize();

            auto append_status = txn->Append(table_entry, input_block);
            EXPECT_TRUE(append_status.ok());
            txn_mgr->CommitTxn(txn);
        };
        append();
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("full ckp"), true);
            SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn, true /*full_check_point*/);
            storage->bg_processor()->Submit(force_ckp_task);
            append();
            force_ckp_task->Wait();

            txn_mgr->CommitTxn(txn);
        }
        wal_dir = storage->wal_manager()->wal_dir();

        infinity::InfinityContext::instance().UnInit();
    }
    RemoveOldWal(wal_dir);
    {
        infinity::InfinityContext::instance().Init(nullptr /*config_path*/);
        Storage *storage = infinity::InfinityContext::instance().storage();
        auto *txn_mgr = storage->txn_manager();

        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get table"));
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(table_entry->segment_map().size(), 1);
            auto segment_entry = table_entry->GetSegmentByID(0, txn->BeginTS());
            EXPECT_NE(segment_entry, nullptr);

            EXPECT_EQ(segment_entry->block_entries().size(), 1);
            auto block_entry = segment_entry->GetBlockEntryByID(0);
            EXPECT_NE(block_entry, nullptr);

            auto [start, end] = block_entry->GetVisibleRange(txn->BeginTS(), 0);
            EXPECT_EQ(int(end - start), insert_n * 2);
        }

        infinity::InfinityContext::instance().UnInit();
    }

#ifdef INFINITY_DEBUG
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
#endif
}
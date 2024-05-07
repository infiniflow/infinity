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

import infinity;
import infinity_exception;

import stl;
import logger;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import data_file_worker;
import global_resource_usage;
import infinity_context;
import storage;
import column_def;
import logical_type;
import data_type;
import data_block;
import txn_manager;
import table_def;
import extra_ddl_info;
import column_vector;
import value;
import catalog;
import segment_entry;
import block_entry;
import status;
import third_party;
import base_table_ref;
import compilation_config;
import index_hnsw;
import knn_expr;
import statement_common;
import embedding_info;
import bg_task;
import physical_import;
import chunk_index_entry;

using namespace infinity;

class BufferObjTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
        RemoveDbDirs();
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();

#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }

public:
    void SaveBufferObj(BufferObj *buffer_obj) { buffer_obj->Save(); };

    void WaitCleanup(Catalog *catalog, TxnManager *txn_mgr, TxnTimeStamp last_commit_ts) {
        LOG_INFO("Waiting cleanup");
        TxnTimeStamp visible_ts = 0;
        time_t start = time(nullptr);
        while (true) {
            visible_ts = txn_mgr->GetMinUnflushedTS();
            time_t end = time(nullptr);
            if (visible_ts >= last_commit_ts) {
                LOG_INFO(fmt::format("Cleanup finished after {}", end - start));
                break;
            }
            // wait for at most 10s
            if (end - start > 10) {
                UnrecoverableError("WaitCleanup timeout");
            }
            LOG_INFO(fmt::format("Before usleep. Wait cleanup for {} seconds", end - start));
            usleep(1000 * 1000);
        }

        auto buffer_mgr = txn_mgr->GetBufferMgr();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, buffer_mgr);
        cleanup_task->Execute();
    }
};

// Test status transfer of buffer handle.
// ?? status transfer in all
TEST_F(BufferObjTest, test1) {
    SizeT memory_limit = 1024;
    String data_dir(GetDataDir());
    auto temp_dir = MakeShared<String>(data_dir + "/spill");
    auto base_dir = MakeShared<String>(GetDataDir());

    BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

    SizeT test_size1 = 1024;
    auto file_dir1 = MakeShared<String>(data_dir + "/dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    auto buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1));

    SizeT test_size2 = 1024;
    auto file_dir2 = MakeShared<String>(data_dir + "/dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(file_dir2, test_fname2, test_size2);
    auto buf2 = buffer_manager.AllocateBufferObject(std::move(file_worker2));

    /// kEphemeral
    // kNew, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    EXPECT_EQ(buf1->type(), BufferType::kEphemeral);

    {
        auto handle1 = buf1->Load();
        // kNew, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kEphemeral -> kUnloaded, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kEphemeral -> kFreed, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
        // kFreed, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }
    { auto handle2 = buf2->Load(); }

    /// kTemp
    {
        auto handle1 = buf1->Load();
        // kFreed, kEphemeral -> kLoaded kTemp
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kTemp);
        buf1->CheckState();
    }

    // kLoaded, kTemp -> kUnloaded, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kTemp -> kLoaded, kTemp
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kTemp -> kFreed, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    /// kPersistent
    SaveBufferObj(buf1);
    // kFreed, kTemp -> kFreed, kPersistent
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);

    {
        auto handle1 = buf1->Load();
        // kFreed, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kPersistent -> kFreed, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    /// kEphemeral
    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
        // kFreed, kPersistent -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
        buf1->CheckState();
    }

    SaveBufferObj(buf1);
    // kUnloadedModified, kEphemeral -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
        // kUnloaded, kPersistent -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
        buf1->CheckState();

        SaveBufferObj(buf1);
        // kLoaded, kEphemeral -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
        buf1->CheckState();
    }

    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
    }

    {
        auto handle1 = buf1->Load();
        SaveBufferObj(buf1);
        // kLoaded, kEphemeral -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
        buf1->CheckState();
    }

    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
    }
    { auto handle2 = buf2->Load(); }
    {
        auto handle1 = buf1->Load();
        SaveBufferObj(buf1);
        // kLoaded, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
        buf1->CheckState();
    }
    {
        auto handle1 = buf1->Load();
        __attribute__((unused)) auto data1 = handle1.GetDataMut();
    }
    { auto handle2 = buf2->Load(); }
    { auto handle1 = buf1->Load(); }
    SaveBufferObj(buf1);
    // kUnloaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    buf1->CheckState();
}

// unit test for BufferStatus::kClean transformation
// TEST_F(BufferObjTest, test_status_clean) {
//     SizeT memory_limit = 1024;
//     String data_dir(GetDataDir());
//     auto temp_dir = MakeShared<String>(data_dir + "/spill");
//     auto base_dir = MakeShared<String>(data_dir);

//     BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

//     SizeT test_size1 = 1024;
//     auto file_dir1 = MakeShared<String>(data_dir + "/dir1");
//     auto test_fname1 = MakeShared<String>("test1");
//     auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//     auto *buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1));

//     SizeT test_size2 = 1024;
//     auto file_dir2 = MakeShared<String>(data_dir + "/dir2");
//     auto test_fname2 = MakeShared<String>("test2");
//     auto file_worker2 = MakeUnique<DataFileWorker>(file_dir2, test_fname2, test_size2);
//     auto *buf2 = buffer_manager.AllocateBufferObject(std::move(file_worker2));

//     /// kEphemeral
//     // kNew, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kNew);
//     EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
//     buf1->CheckState();

//     {
//         auto handle1 = buf1->Load();
//         // kNew, kEphemeral -> kLoaded, kEphemeral
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }

//     // kLoaded, kEphemeral -> kUnloaded, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     // kUnloaded, kEphemeral -> kClean, kEphemeral
//     buf1->Cleanup();
//     EXPECT_EQ(buf1->status(), BufferStatus::kClean);
//     buf1->CheckState();
//     buffer_manager.RemoveClean();
//     buf1 = nullptr;

//     // kClean, kEphemeral -> kLoaded, kEphemeral
//     { auto handle2 = buf2->Load(); }
//     {
//         auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//         buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new1));
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
//         buf1->CheckState();
//     }

//     // kLoaded, kEphemeral -> kUnloaded, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     { auto handle2 = buf2->Load(); }
//     // kUnloaded, kEphemeral -> kFreed, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
//     buf1->CheckState();

//     // kFreed, kEphemeral -> kNew, kEphemeral
//     buf1->Cleanup();
//     buffer_manager.RemoveClean();
//     buf1 = nullptr;

//     auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//     buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new1));
//     EXPECT_EQ(buf1->status(), BufferStatus::kNew);
//     buf1->CheckState();

//     {
//         auto handle1 = buf1->Load();
//         // kNew, kEphemeral -> kLoaded, kEphemeral
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }
//     // kLoaded, kEphemeral -> kUnloaded, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();
//     { auto handle2 = buf2->Load(); }
//     // kUnloaded, kEphemeral -> kFreed, kEphemeral
//     EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
//     buf1->CheckState();

//     /// kTemp
//     // kFreed, kEphemeral -> kLoaded, kTemp
//     {
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         EXPECT_EQ(buf1->type(), BufferType::kTemp);
//         buf1->CheckState();
//     }

//     // kLoaded, kTemp -> kUnloaded, kTemp
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     // kUnloaded, kTemp -> kClean, kTemp
//     buf1->Cleanup();
//     EXPECT_EQ(buf1->status(), BufferStatus::kClean);
//     buf1->CheckState();
//     buffer_manager.RemoveClean();
//     buf1 = nullptr;

//     // kClean, kTemp -> kLoaded, kTemp
//     { auto handle2 = buf2->Load(); }
//     {
//         auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//         buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new1));
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }
//     { auto handle2 = buf2->Load(); }
//     EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
//     {
//         auto handle1 = buf1->Load();
//         // kFreed, kTemp -> kLoaded, kTemp
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }

//     // kLoaded, kTemp -> kUnloaded, kTemp
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     { auto handle2 = buf2->Load(); }
//     // kUnloaded, kTemp -> kFreed, kTemp
//     EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
//     buf1->CheckState();

//     // kFreed, kTemp -> kNew, kTemp
//     buf1->Cleanup();
//     auto file_worker1_new2 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//     buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new2));
//     EXPECT_EQ(buf1->status(), BufferStatus::kNew);
//     buf1->CheckState();
//     buffer_manager.RemoveClean();
//     buf1 = nullptr;

//     {
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }

//     /// kPersistent
//     SaveBufferObj(buf1);
//     // kNew, kTemp -> kNew, kPersistent
//     EXPECT_EQ(buf1->type(), BufferType::kPersistent);

//     {
//         auto handle1 = buf1->Load();
//         // kNew, kPersistent -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }

//     // kLoaded, kPersistent -> kUnloaded, kPersistent
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     // kUnloaded, kPersistent -> kClean, kPersistent
//     buf1->Cleanup();
//     EXPECT_EQ(buf1->status(), BufferStatus::kClean);
//     buf1->CheckState();

//     // kClean, kPersistent -> kLoaded, kPersistent
//     { auto handle2 = buf2->Load(); }
//     {
//         auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//         buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new1));
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }
//     SaveBufferObj(buf1);
//     {
//         auto handle1 = buf1->Load();
//         EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
//         buf1->CheckState();
//     }

//     // kLoaded, kPersistent -> kUnloaded, kPersistent
//     EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
//     buf1->CheckState();

//     { auto handle2 = buf2->Load(); }

//     // kUnloaded, kPersistent -> kFreed, kPersistent
//     EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
//     buf1->CheckState();

//     // kFreed, kPersistent -> kClean, kPersistent
//     buf1->Cleanup();
//     buffer_manager.RemoveClean();
//     auto file_worker1_new3 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//     buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1_new3));
//     EXPECT_EQ(buf1->status(), BufferStatus::kNew);
//     buf1->CheckState();
// }

TEST_F(BufferObjTest, test_hnsw_index_buffer_obj_shutdown) {
    GTEST_SKIP(); // FIXME

#ifdef INFINITY_DEBUG
    infinity::InfinityContext::instance().UnInit();
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
    infinity::GlobalResourceUsage::Init();
#endif
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj_2.toml");
    RemoveDbDirs();
    infinity::InfinityContext::instance().Init(config_path);

    constexpr u64 kInsertN = 4;
    constexpr u64 kImportSize = 8192;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)8 * 4 * 128 * 8192);

    TxnManager *txn_mgr = storage->txn_manager();
    BufferManager *buffer_mgr = storage->buffer_manager();
    Catalog *catalog = storage->catalog();
    TxnTimeStamp last_commit_ts = 0;

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("test_hnsw");
    auto index_name = MakeShared<String>("hnsw_index");
    auto column_name = MakeShared<String>("col1");

    // CREATE TABLE test_hnsw (col1 embedding(float,128));
    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embedding_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embedding_info), "col1", constraints);
            column_defs.emplace_back(column_def_ptr);
        }
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("test_hnsw"), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    // CreateIndex
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        parameters1.emplace_back(new InitParameter("metric", "l2"));
        parameters1.emplace_back(new InitParameter("encode", "plain"));
        parameters1.emplace_back(new InitParameter("M", "16"));
        parameters1.emplace_back(new InitParameter("ef_construction", "200"));
        parameters1.emplace_back(new InitParameter("ef", "200"));

        auto index_base_hnsw = IndexHnsw::Make(index_name, "hnsw_index_test_hnsw", columns1, parameters1);
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
            auto table_ref = BaseTableRef::FakeTableRef(table_entry, txn->BeginTS());
            auto result = txn->CreateIndexDef(table_entry, index_base_hnsw, conflict_type);
            auto *table_index_entry = std::get<0>(result);
            auto status = std::get<1>(result);
            EXPECT_EQ(status.ok(), true);
            txn->CreateIndexPrepare(table_index_entry, table_ref.get(), prepare);
            txn->CreateIndexFinish(table_entry, table_index_entry);
        }
        txn_mgr->CommitTxn(txn);
    }
    // Insert data
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            Vector<SharedPtr<ColumnVector>> column_vectors;
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(column_defs[0]->type());
            column_vector->Initialize();
            for (SizeT j = 0; j < kImportSize; ++j) {
                Vector<float> vec;
                for (SizeT k = 0; k < 128; ++k) {
                    vec.push_back(j * 1000 + k);
                }
                Value v = Value::MakeEmbedding(vec);
                column_vector->AppendValue(v);
            }
            column_vectors.push_back(column_vector);
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto append_status = txn->Append(table_entry, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    // Get Index
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get index"));

        auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        auto &index_meta_map = table_entry->index_meta_map();
        for (const auto &[index_name, table_index_meta] : index_meta_map) {
            auto [table_index_entry, table_index_status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
            EXPECT_TRUE(table_index_status.ok());

            auto &index_by_segment = table_index_entry->index_by_segment();
            auto iter = index_by_segment.find(0);
            if (iter != index_by_segment.end()) {
                auto &segment_index_entry = iter->second;

                Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
                segment_index_entry->GetChunkIndexEntries(chunk_index_entries);
                for (auto &chunk_index_entry : chunk_index_entries) {
                    auto index_handle = chunk_index_entry->GetIndex();
                }
            }
        }

        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check table"));

        auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        EXPECT_EQ(table_entry->row_count(), kInsertN * kImportSize);
        ASSERT_EQ(table_entry->segment_map().size(), 1ul);
        {
            auto &segment_entry = table_entry->segment_map().begin()->second;
            EXPECT_EQ(segment_entry->row_count(), kInsertN * kImportSize);
            ASSERT_EQ(segment_entry->block_entries().size(), kInsertN);
            for (u64 i = 0; i < kInsertN; ++i) {
                auto block_entry = segment_entry->GetBlockEntryByID(i);
                EXPECT_EQ(block_entry->row_count(), kImportSize);
                auto *col = block_entry->GetColumnBlockEntry(0);
                auto column_vector = col->GetColumnVector(buffer_mgr);
                for (u64 j = 0; j < kImportSize; ++j) {
                    Value v1 = column_vector.GetValue(j);
                }
            }
        }
        txn_mgr->CommitTxn(txn);
    }
    // Drop Table
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        txn->DropTableCollectionByName(*db_name, *table_name, ConflictType::kError);
        last_commit_ts = txn_mgr->CommitTxn(txn);
    }
    WaitCleanup(catalog, txn_mgr, last_commit_ts);
}

TEST_F(BufferObjTest, test_big_with_gc_and_cleanup) {
    constexpr u64 kInsertN = 1024;
    constexpr u64 kImportSize = 8192;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)512 * 1024);

    TxnManager *txn_mgr = storage->txn_manager();
    BufferManager *buffer_mgr = storage->buffer_manager();

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    auto index_name = MakeShared<String>("idx1");
    auto column_name = MakeShared<String>("col1");

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        HashSet<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), *column_name, constraints));
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            Vector<SharedPtr<ColumnVector>> column_vectors;
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(MakeShared<DataType>(column_defs[0]->type()->type()));
            column_vector->Initialize();
            for (u64 j = 0; j < kImportSize; ++j) {
                Value v = Value::MakeBigInt(i * 1000 + j);
                column_vector->AppendValue(v);
            }
            column_vectors.push_back(column_vector);
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto append_status = txn->Append(table_entry, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check table"));

        auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        EXPECT_EQ(table_entry->row_count(), kInsertN * kImportSize);
        ASSERT_EQ(table_entry->segment_map().size(), 1ul);
        {
            auto &segment_entry = table_entry->segment_map().begin()->second;
            EXPECT_EQ(segment_entry->row_count(), kInsertN * kImportSize);
            ASSERT_EQ(segment_entry->block_entries().size(), kInsertN);
            for (u64 i = 0; i < kInsertN; ++i) {
                auto block_entry = segment_entry->GetBlockEntryByID(i);
                EXPECT_EQ(block_entry->row_count(), kImportSize);
                auto *col = block_entry->GetColumnBlockEntry(0);
                auto column_vector = col->GetColumnVector(buffer_mgr);
                for (u64 j = 0; j < kImportSize; ++j) {
                    Value v1 = column_vector.GetValue(j);
                    Value v2 = Value::MakeBigInt(i * 1000 + j);
                    EXPECT_EQ(v1, v2);
                }
            }
        }
        txn_mgr->CommitTxn(txn);
    }
}

TEST_F(BufferObjTest, test_multiple_threads_read) {
    constexpr u64 kInsertN = 1024;
    constexpr u64 kImportSize = 8192;
    constexpr u64 kThreadN = 2;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)512 * 1024);

    TxnManager *txn_mgr = storage->txn_manager();
    BufferManager *buffer_mgr = storage->buffer_manager();

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    auto index_name = MakeShared<String>("idx1");
    auto column_name = MakeShared<String>("col1");

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        HashSet<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), *column_name, constraints));
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));
            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            Vector<SharedPtr<ColumnVector>> column_vectors;
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(MakeShared<DataType>(column_defs[0]->type()->type()));
            column_vector->Initialize();
            for (u64 j = 0; j < kImportSize; ++j) {
                Value v = Value::MakeBigInt(i * 1000 + j);
                column_vector->AppendValue(v);
            }
            column_vectors.push_back(column_vector);
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto append_status = txn->Append(table_entry, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    Vector<std::thread> ths;
    for (SizeT i = 0; i < kThreadN; ++i) {
        std::thread th([&]() {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"));

            auto [table_entry, status] = txn->GetTableByName(*db_name, *table_name);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(table_entry->row_count(), kInsertN * kImportSize);
            ASSERT_EQ(table_entry->segment_map().size(), 1ul);
            {
                auto &segment_entry = table_entry->segment_map().begin()->second;
                EXPECT_EQ(segment_entry->row_count(), kInsertN * kImportSize);
                ASSERT_EQ(segment_entry->block_entries().size(), kInsertN);
                for (u64 i = 0; i < kInsertN; ++i) {
                    auto block_entry = segment_entry->GetBlockEntryByID(i);
                    EXPECT_EQ(block_entry->row_count(), kImportSize);
                    auto *col = block_entry->GetColumnBlockEntry(0);
                    auto column_vector = col->GetColumnVector(buffer_mgr);
                    for (u64 j = 0; j < kImportSize; ++j) {
                        Value v1 = column_vector.GetValue(j);
                        Value v2 = Value::MakeBigInt(i * 1000 + j);
                        EXPECT_EQ(v1, v2);
                    }
                }
            }
            txn_mgr->CommitTxn(txn);
        });
        ths.push_back(std::move(th));
    }
    for (SizeT i = 0; i < kThreadN; ++i) {
        ths[i].join();
    }
}
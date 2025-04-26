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
import new_txn_manager;
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
import memory_indexer;
import wal_manager;
import internal_types;
import persistence_manager;
import default_values;
import txn_state;
import new_txn;
import db_meeta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import mem_index;
import segment_meta;
import block_meta;
import column_meta;
import new_catalog;

using namespace infinity;

class BufferObjTest : public BaseTest {
    void SetUp() override { BaseTest::SetUp(); }

    void TearDown() override { BaseTest::TearDown(); }

public:
    void SaveBufferObj(BufferObj *buffer_obj) { buffer_obj->Save(); };

    void WaitCleanup(Storage *storage) {
        Catalog *catalog = storage->catalog();
        BufferManager *buffer_mgr = storage->buffer_manager();
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        TxnTimeStamp visible_ts = txn_mgr->GetCleanupScanTS1();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, buffer_mgr);
        cleanup_task->Execute();
    }

    void WaitFlushOp(Storage *storage) {
        NewTxnManager *txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();
        auto *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);

        TxnTimeStamp max_commit_ts{};
        i64 wal_size{};
        std::tie(max_commit_ts, wal_size) = wal_manager->GetCommitState();

        new_txn->SetWalSize(wal_size);
        TxnTimeStamp last_checkpoint_ts = wal_manager->LastCheckpointTS();
        Status status = new_txn->Checkpoint(last_checkpoint_ts);
        if (status.ok()) {
            status = txn_mgr->CommitTxn(new_txn);
        }
    }
};

// Test status transfer of buffer handle.
// ?? status transfer in all
TEST_F(BufferObjTest, test1) {

    RemoveDbDirs();
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
    //    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    SizeT memory_limit = 1024;
    String data_dir(GetFullDataDir());
    auto temp_dir = MakeShared<String>(data_dir + "/spill");
    auto base_dir = MakeShared<String>(GetFullDataDir());
    auto persistence_dir = MakeShared<String>(data_dir + "/persistence");

    UniquePtr<PersistenceManager> persistence_manager =
        MakeUnique<PersistenceManager>(*persistence_dir, *base_dir, DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT);
    BufferManager buffer_manager(memory_limit, base_dir, temp_dir, persistence_manager.get());

    SizeT test_size1 = 1024;
    auto file_dir1 = MakeShared<String>("dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(base_dir, temp_dir, file_dir1, test_fname1, test_size1, buffer_manager.persistence_manager());
    auto buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1));

    SizeT test_size2 = 1024;
    auto file_dir2 = MakeShared<String>("dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(base_dir, temp_dir, file_dir2, test_fname2, test_size2, buffer_manager.persistence_manager());
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

    {
        auto handle2 = buf2->Load();
    }
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
    {
        auto handle2 = buf2->Load();
    }

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

    {
        auto handle2 = buf2->Load();
    }
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

    {
        auto handle2 = buf2->Load();
    }
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
    {
        auto handle2 = buf2->Load();
    }
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
    {
        auto handle2 = buf2->Load();
    }
    {
        auto handle1 = buf1->Load();
    }
    SaveBufferObj(buf1);
    // kUnloaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    buf1->CheckState();
    infinity::InfinityContext::instance().UnInit();
}

// unit test for BufferStatus::kClean transformation
// TEST_F(BufferObjTest, test_status_clean) {
//     SizeT memory_limit = 1024;
//     String data_dir(GetFullDataDir());
//     auto temp_dir = MakeShared<String>(data_dir + "/spill");
//     auto base_dir = MakeShared<String>(data_dir);

//     BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

//     SizeT test_size1 = 1024;
//     auto file_dir1 = MakeShared<String>("dir1");
//     auto test_fname1 = MakeShared<String>("test1");
//     auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
//     auto *buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1));

//     SizeT test_size2 = 1024;
//     auto file_dir2 = MakeShared<String>("dir2");
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
    // GTEST_SKIP(); // FIXME
    RemoveDbDirs();
#ifdef INFINITY_DEBUG
    EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
    EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
    infinity::GlobalResourceUsage::UnInit();
    infinity::GlobalResourceUsage::Init();
#endif
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj_2.toml");
    //    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    constexpr u64 kInsertN = 2;
    constexpr u64 kImportSize = 8192;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)8 * 4 * 128 * 8192);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("test_hnsw");
    auto index_name = MakeShared<String>("hnsw_index");
    auto column_name = MakeShared<String>("col1");

    // CREATE TABLE test_hnsw (col1 embedding(float,128));
    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            i64 column_id = 0;
            auto embedding_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embedding_info), "col1", constraints);
            column_defs.emplace_back(column_def_ptr);
        }
        auto tbl1_def =
            MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("test_hnsw"), MakeShared<String>("test_comment"), column_defs);
        NewTxn *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    // CreateIndex
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Vector<String> columns1{"col1"};
        Vector<InitParameter *> parameters1;
        parameters1.emplace_back(new InitParameter("metric", "l2"));
        parameters1.emplace_back(new InitParameter("encode", "plain"));
        parameters1.emplace_back(new InitParameter("m", "16"));
        parameters1.emplace_back(new InitParameter("ef_construction", "200"));

        auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test_comment"), "hnsw_index_test_hnsw", columns1, parameters1);
        for (auto *init_parameter : parameters1) {
            delete init_parameter;
        }

        const String &db_name = "default_db";
        const String &table_name = "test_hnsw";
        ConflictType conflict_type = ConflictType::kError;

        // create index idx1
        Status status = txn->CreateIndex(db_name, table_name, index_base_hnsw, conflict_type);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    // Insert data
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
            Vector<SharedPtr<ColumnVector>> column_vectors;
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(column_defs[0]->type());
            column_vector->Initialize();
            for (SizeT j = 0; j < kImportSize; ++j) {
                Vector<float> vec;
                vec.reserve(128);
                for (SizeT k = 0; k < 128; ++k) {
                    vec.push_back(j * 1000 + k);
                }
                Value v = Value::MakeEmbedding(vec);
                column_vector->AppendValue(v);
            }
            column_vectors.push_back(column_vector);
            auto data_block = DataBlock::Make();
            data_block->Init(column_vectors);

            auto append_status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    WaitFlushOp(storage);
    // Get Index
    {

        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Vector<String> index_names;
        Status status = txn->ListIndex(*db_name, *table_name, index_names);
        EXPECT_TRUE(status.ok());

        for (const String &index_name : index_names) {
            status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
            EXPECT_TRUE(status.ok());

            {
                auto [segment_ids, status] = table_meta->GetSegmentIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
            }
            SegmentID segment_id = 0;
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

            SharedPtr<MemIndex> mem_index;
            status = segment_index_meta.GetMemIndex(mem_index);
            EXPECT_TRUE(status.ok());
            EXPECT_NE(mem_index, nullptr);
        }

        txn_mgr->CommitTxn(txn);
    }
    // Drop Table
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    WaitCleanup(storage);

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(BufferObjTest, test_big_with_gc_and_cleanup) {
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    constexpr u64 kInsertN = 256;
    constexpr u64 kImportSize = 8192;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)512 * 1024);

    NewTxnManager *txn_mgr = storage->new_txn_manager();
    //    BufferManager *buffer_mgr = storage->buffer_manager();

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    auto table_comment = MakeShared<String>("table1_commment");
    auto index_name = MakeShared<String>("idx1");
    auto column_name = MakeShared<String>("col1");

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), *column_name, constraints));
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, table_comment, column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
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

            auto append_status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(segment_ids->size(), 1);
        SegmentID segment_id = segment_ids->at(0);
        EXPECT_EQ(segment_id, 0);
        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(block_ids_ptr->size(), kInsertN);

        for (SizeT idx = 0; idx < kInsertN; ++idx) {
            BlockID block_id = block_ids_ptr->at(idx);
            EXPECT_EQ(block_id, idx);
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);

            EXPECT_EQ(range.second, DEFAULT_BLOCK_CAPACITY);

            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, DEFAULT_BLOCK_CAPACITY);

            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                for (SizeT row_id = 0; row_id < kImportSize; ++row_id) {
                    Value v1 = col.GetValue(row_id);
                    Value v2 = Value::MakeBigInt(idx * 1000 + row_id);
                    EXPECT_EQ(v1, v2);
                }
            }
        }
        txn_mgr->CommitTxn(txn);
    }

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        auto status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(BufferObjTest, test_multiple_threads_read) {
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    constexpr u64 kInsertN = 256;
    constexpr u64 kImportSize = 8192;
    constexpr u64 kThreadN = 4;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)512 * 1024);

    NewTxnManager *txn_mgr = storage->new_txn_manager();
    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    auto table_comment = MakeShared<String>("table_comment");
    auto index_name = MakeShared<String>("idx1");
    auto column_name = MakeShared<String>("col1");

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), *column_name, constraints));
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, table_comment, column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
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

            auto append_status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    Vector<std::thread> ths;
    for (SizeT i = 0; i < kThreadN; ++i) {
        std::thread th([&]() {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(segment_ids->size(), 1);
            SegmentID segment_id = segment_ids->at(0);
            EXPECT_EQ(segment_id, 0);
            SegmentMeta segment_meta(segment_id, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(block_ids_ptr->size(), kInsertN);

            for (SizeT idx = 0; idx < kInsertN; ++idx) {
                BlockID block_id = block_ids_ptr->at(idx);
                EXPECT_EQ(block_id, idx);
                BlockMeta block_meta(block_id, segment_meta);
                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());

                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);

                EXPECT_EQ(range.second, DEFAULT_BLOCK_CAPACITY);

                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);

                SizeT row_count = state.block_offset_end();
                EXPECT_EQ(row_count, DEFAULT_BLOCK_CAPACITY);

                {
                    SizeT column_idx = 0;
                    ColumnMeta column_meta(column_idx, block_meta);
                    ColumnVector col;

                    status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                    EXPECT_TRUE(status.ok());

                    for (SizeT row_id = 0; row_id < kImportSize; ++row_id) {
                        Value v1 = col.GetValue(row_id);
                        Value v2 = Value::MakeBigInt(idx * 1000 + row_id);
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

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        auto status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }

    infinity::InfinityContext::instance().UnInit();
}

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

using namespace infinity;

class BufferObjTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
#ifdef INFINITY_DEBUG
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }

public:
    void SaveBufferObj(BufferObj *buffer_obj) {
        if (buffer_obj->Save()) {
            buffer_obj->Sync();
            buffer_obj->CloseFile();
        }
    };
};

// Test status transfer of buffer handle.
// ?? status transfer in all
TEST_F(BufferObjTest, test1) {
    SizeT memory_limit = 1024;
    auto temp_dir = MakeShared<String>("/tmp/infinity/spill");
    auto base_dir = MakeShared<String>("/tmp/infinity/data");

    BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

    SizeT test_size1 = 1024;
    auto file_dir1 = MakeShared<String>("/tmp/infinity/data/dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    auto buf1 = buffer_manager.Allocate(std::move(file_worker1));

    SizeT test_size2 = 1024;
    auto file_dir2 = MakeShared<String>("/tmp/infinity/data/dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(file_dir2, test_fname2, test_size2);
    auto buf2 = buffer_manager.Allocate(std::move(file_worker2));

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
TEST_F(BufferObjTest, test_status_clean) {
    SizeT memory_limit = 1024;
    auto temp_dir = MakeShared<String>("/tmp/infinity/spill");
    auto base_dir = MakeShared<String>("/tmp/infinity/data");

    BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

    SizeT test_size1 = 1024;
    auto file_dir1 = MakeShared<String>("/tmp/infinity/data/dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    auto buf1 = buffer_manager.Allocate(std::move(file_worker1));

    SizeT test_size2 = 1024;
    auto file_dir2 = MakeShared<String>("/tmp/infinity/data/dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(file_dir2, test_fname2, test_size2);
    auto buf2 = buffer_manager.Allocate(std::move(file_worker2));

    /// kEphemeral
    // kNew, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        // kNew, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kEphemeral -> kUnloaded, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    // kUnloaded, kEphemeral -> kClean, kEphemeral
    buf1->SetAndTryCleanup();
    EXPECT_EQ(buf1->status(), BufferStatus::kClean);
    buf1->CheckState();

    // kClean, kEphemeral -> kLoaded, kEphemeral
    { auto handle2 = buf2->Load(); }
    {
        auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
        buf1 = buffer_manager.Allocate(std::move(file_worker1_new1));
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
        buf1->CheckState();
    }

    // kLoaded, kEphemeral -> kUnloaded, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kEphemeral -> kFreed, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    // kFreed, kEphemeral -> kNew, kEphemeral
    buf1->SetAndTryCleanup();
    auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    buf1 = buffer_manager.Allocate(std::move(file_worker1_new1));
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        // kNew, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }
    // kLoaded, kEphemeral -> kUnloaded, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();
    { auto handle2 = buf2->Load(); }
    // kUnloaded, kEphemeral -> kFreed, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    /// kTemp
    // kFreed, kEphemeral -> kLoaded, kTemp
    {
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kTemp);
        buf1->CheckState();
    }

    // kLoaded, kTemp -> kUnloaded, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    // kUnloaded, kTemp -> kClean, kTemp
    buf1->SetAndTryCleanup();
    EXPECT_EQ(buf1->status(), BufferStatus::kClean);
    buf1->CheckState();

    // kClean, kTemp -> kLoaded, kTemp
    { auto handle2 = buf2->Load(); }
    {
        auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
        buf1 = buffer_manager.Allocate(std::move(file_worker1_new1));
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }
    { auto handle2 = buf2->Load(); }
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    {
        auto handle1 = buf1->Load();
        // kFreed, kTemp -> kLoaded, kTemp
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kTemp -> kUnloaded, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kTemp -> kFreed, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    // kFreed, kTemp -> kNew, kTemp
    buf1->SetAndTryCleanup();
    auto file_worker1_new2 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    buf1 = buffer_manager.Allocate(std::move(file_worker1_new2));
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    buf1->CheckState();

    {
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    /// kPersistent
    SaveBufferObj(buf1);
    // kNew, kTemp -> kNew, kPersistent
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);

    {
        auto handle1 = buf1->Load();
        // kNew, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    // kUnloaded, kPersistent -> kClean, kPersistent
    buf1->SetAndTryCleanup();
    EXPECT_EQ(buf1->status(), BufferStatus::kClean);
    buf1->CheckState();

    // kClean, kPersistent -> kLoaded, kPersistent
    { auto handle2 = buf2->Load(); }
    {
        auto file_worker1_new1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
        buf1 = buffer_manager.Allocate(std::move(file_worker1_new1));
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }
    SaveBufferObj(buf1);
    {
        auto handle1 = buf1->Load();
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        buf1->CheckState();
    }

    // kLoaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    buf1->CheckState();

    { auto handle2 = buf2->Load(); }

    // kUnloaded, kPersistent -> kFreed, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);
    buf1->CheckState();

    // kFreed, kPersistent -> kClean, kPersistent
    buf1->SetAndTryCleanup();
    auto file_worker1_new3 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    buf1 = buffer_manager.Allocate(std::move(file_worker1_new3));
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    buf1->CheckState();
}

TEST_F(BufferObjTest, test_big_with_gc_and_cleanup) {
    constexpr u64 kInsertN = 1024;
    constexpr u64 kImportSize = 8192;

    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);
    EXPECT_EQ(storage->buffer_manager()->memory_limit(), (u64)512 * 1024);

    TxnManager *txn_mgr = storage->txn_manager();
    BufferManager *buffer_mgr = storage->buffer_manager();

    auto db_name = MakeShared<String>("default");
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
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
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

            auto append_status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

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

    auto db_name = MakeShared<String>("default");
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
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        for (SizeT i = 0; i < kInsertN; ++i) {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();
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

            auto append_status = txn->Append(*db_name, *table_name, data_block);
            ASSERT_TRUE(append_status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    Vector<std::thread> ths;
    for (SizeT i = 0; i < kThreadN; ++i) {
        std::thread th([&]() {
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

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
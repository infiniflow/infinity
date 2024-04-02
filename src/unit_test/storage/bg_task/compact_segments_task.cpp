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
#include <numeric>
#include <random>

import stl;
import storage;
import compact_segments_task;
import global_resource_usage;
import infinity_context;
import status;
import catalog;
import txn;
import buffer_manager;
import txn_manager;
import column_vector;

import table_def;
import value;
import physical_import;
import default_values;
import infinity_exception;
import base_table_ref;
import logical_type;
import internal_types;
import extra_ddl_info;
import column_def;
import data_type;
import segment_entry;
import block_entry;

using namespace infinity;

class CompactTaskTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override { system("rm -rf /tmp/infinity"); }

protected:
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

TEST_F(CompactTaskTest, compact_to_single_segment) {
    {
        String table_name = "tbl1";
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
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
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        { // add compact
            auto txn4 = txn_mgr->CreateTxn();
            txn4->Begin();

            auto [table_entry, status] = txn4->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            {
                auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn4);
                compact_task->Execute();
            }
            txn_mgr->CommitTxn(txn4);
        }

        {
            auto txn5 = txn_mgr->CreateTxn();
            txn5->Begin();
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default", table_name);
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
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_F(CompactTaskTest, compact_to_two_segment) {
    {
        String table_name = "tbl1";
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
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
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000, 1000000, 2000000, 4000000, 8000000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        { // add compact
            auto txn4 = txn_mgr->CreateTxn();
            txn4->Begin();

            auto [table_entry, status] = txn4->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            {
                auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn4);
                compact_task->Execute();
            }
            txn_mgr->CommitTxn(txn4);
        }
        {
            auto txn5 = txn_mgr->CreateTxn();
            txn5->Begin();
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            int test_segment_n = segment_sizes.size();

            for (int i = 0; i < test_segment_n; ++i) {
                auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
                EXPECT_NE(segment_entry, nullptr);
                EXPECT_EQ(segment_entry->status(), SegmentStatus::kDeprecated);
            }
            int cnt = 0;
            for (int i = test_segment_n; i < test_segment_n + 2; ++i) {
                auto compact_segment = table_entry->GetSegmentByID(i, begin_ts);
                EXPECT_NE(compact_segment, nullptr);
                EXPECT_NE(compact_segment->status(), SegmentStatus::kDeprecated);
                cnt += compact_segment->actual_row_count();
            }
            EXPECT_EQ(cnt, row_count);

            txn_mgr->CommitTxn(txn5);
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_F(CompactTaskTest, compact_with_delete) {
    {
        String table_name = "tbl1";
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
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
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000, 1000000, 2000000, 4000000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        SizeT delete_n = 0;
        {
            auto txn3 = txn_mgr->CreateTxn();
            txn3->Begin();

            Vector<RowID> delete_row_ids;
            for (int i = 0; i < (int)segment_sizes.size(); ++i) {
                int delete_n1 = segment_sizes[i] / 2;
                Vector<SegmentOffset> offsets;
                for (int j = 0; j < delete_n1; ++j) {
                    offsets.push_back(rand() % segment_sizes[i]);
                }
                std::sort(offsets.begin(), offsets.end());
                offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
                for (SegmentOffset offset : offsets) {
                    delete_row_ids.emplace_back(i, offset);
                }
                delete_n += offsets.size();
            }
            txn3->Delete("default", table_name, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        { // add compact
            auto txn4 = txn_mgr->CreateTxn();
            txn4->Begin();

            auto [table_entry, status] = txn4->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            {
                auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn4);
                compact_task->Execute();
            }
            txn_mgr->CommitTxn(txn4);
        }
        {
            auto txn5 = txn_mgr->CreateTxn();
            txn5->Begin();
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            int test_segment_n = segment_sizes.size();
            for (int i = 0; i < test_segment_n; ++i) {
                auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
                EXPECT_NE(segment_entry, nullptr);
                EXPECT_EQ(segment_entry->status(), SegmentStatus::kDeprecated);
            }
            auto compact_segment = table_entry->GetSegmentByID(test_segment_n, begin_ts);
            EXPECT_NE(compact_segment, nullptr);
            EXPECT_NE(compact_segment->status(), SegmentStatus::kDeprecated);

            EXPECT_EQ(compact_segment->actual_row_count(), row_count - delete_n);

            txn_mgr->CommitTxn(txn5);
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_F(CompactTaskTest, delete_in_compact_process) {
    {
        String table_name = "tbl1";
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
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
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        SizeT row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        int delete_n = 0;
        {
            auto txn3 = txn_mgr->CreateTxn();
            txn3->Begin();

            Vector<RowID> delete_row_ids;
            for (int i = 0; i < (int)segment_sizes.size(); ++i) {
                int delete_n1 = segment_sizes[i] / 4;
                Vector<SegmentOffset> offsets;
                for (int j = 0; j < delete_n1; ++j) {
                    offsets.push_back(rand() % (segment_sizes[i] / 2));
                }
                std::sort(offsets.begin(), offsets.end());
                offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
                for (SegmentOffset offset : offsets) {
                    delete_row_ids.emplace_back(i, offset);
                }
                delete_n += offsets.size();
            }
            txn3->Delete("default", table_name, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        { // add compact
            auto txn4 = txn_mgr->CreateTxn();
            txn4->Begin();

            auto [table_entry, status] = txn4->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            {
                auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn4);
                CompactSegmentsTaskState state(table_entry);
                compact_task->CompactSegments(state);

                {
                    auto txn5 = txn_mgr->CreateTxn();
                    txn5->Begin();

                    Vector<RowID> delete_row_ids;
                    for (int i = 0; i < (int)segment_sizes.size(); ++i) {
                        int delete_n2 = segment_sizes[i] / 4;
                        Vector<SegmentOffset> offsets;
                        for (int j = 0; j < delete_n2; ++j) {
                            offsets.push_back(rand() % (segment_sizes[i] - segment_sizes[i] / 2) + segment_sizes[i] / 2);
                        }
                        std::sort(offsets.begin(), offsets.end());
                        offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
                        for (SegmentOffset offset : offsets) {
                            delete_row_ids.emplace_back(i, offset);
                        }
                        delete_n += offsets.size();
                    }

                    txn5->Delete("default", table_name, delete_row_ids);
                    txn_mgr->CommitTxn(txn5);
                }

                compact_task->SaveSegmentsData(state);
                compact_task->ApplyDeletes(state);
            }

            txn_mgr->CommitTxn(txn4);
        }
        {
            auto txn5 = txn_mgr->CreateTxn();
            txn5->Begin();
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            int test_segment_n = segment_sizes.size();
            for (int i = 0; i < test_segment_n; ++i) {
                auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
                EXPECT_NE(segment_entry, nullptr);
                EXPECT_EQ(segment_entry->status(), SegmentStatus::kDeprecated);
            }
            auto compact_segment = table_entry->GetSegmentByID(test_segment_n, begin_ts);
            EXPECT_NE(compact_segment, nullptr);
            EXPECT_NE(compact_segment->status(), SegmentStatus::kDeprecated);

            EXPECT_EQ(compact_segment->actual_row_count(), row_count - delete_n);

            txn_mgr->CommitTxn(txn5);
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

// Cannot compile the test. So annotate it.

TEST_F(CompactTaskTest, uncommit_delete_in_compact_process) {
    {
        String table_name = "tbl1";
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
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
        { // create table
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        SizeT delete_n = 0;
        {
            auto txn3 = txn_mgr->CreateTxn();
            txn3->Begin();

            Vector<RowID> delete_row_ids;
            for (int i = 0; i < (int)segment_sizes.size(); ++i) {
                int delete_n1 = segment_sizes[i] / 6;
                Vector<SegmentOffset> offsets;
                for (int j = 0; j < delete_n1; ++j) {
                    offsets.push_back(rand() % (segment_sizes[i] / 3));
                }
                std::sort(offsets.begin(), offsets.end());
                offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
                for (SegmentOffset offset : offsets) {
                    delete_row_ids.emplace_back(i, offset);
                }
                delete_n += offsets.size();
            }
            txn3->Delete("default", table_name, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        { // add compact
            auto compact_txn = txn_mgr->CreateTxn();
            compact_txn->Begin();

            auto [table_entry, status] = compact_txn->GetTableByName("default", table_name);
            EXPECT_NE(table_entry, nullptr);

            Vector<RowID> delete_row_ids2;
            {
                auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, compact_txn);

                CompactSegmentsTaskState state(table_entry);
                compact_task->CompactSegments(state);

                Vector<RowID> delete_row_ids;

                int delete_row_n1 = 0;
                int delete_row_n2 = 0;

                for (int i = 0; i < (int)segment_sizes.size(); ++i) {
                    Vector<SegmentOffset> offsets;
                    Vector<SegmentOffset> offsets2;
                    for (int j = 0; j < (int)(segment_sizes[i] / 6); ++j) {
                        offsets.push_back(rand() % (segment_sizes[i] / 3) + segment_sizes[i] / 3);
                        offsets2.push_back(rand() % (segment_sizes[i] - segment_sizes[i] / 3 * 2) + segment_sizes[i] / 3 * 2);
                    }
                    std::sort(offsets.begin(), offsets.end());
                    std::sort(offsets2.begin(), offsets2.end());
                    offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
                    offsets2.erase(std::unique(offsets2.begin(), offsets2.end()), offsets2.end());
                    for (SegmentOffset offset : offsets) {
                        delete_row_ids.emplace_back(i, offset);
                    }
                    for (SegmentOffset offset2 : offsets2) {
                        delete_row_ids2.emplace_back(i, offset2);
                    }

                    delete_row_n1 += offsets.size();
                    delete_row_n2 += offsets2.size();
                }

                auto delete_txn2 = txn_mgr->CreateTxn();
                delete_txn2->Begin();
                delete_txn2->Delete("default", table_name, delete_row_ids2);
                {
                    auto delete_txn1 = txn_mgr->CreateTxn();
                    delete_txn1->Begin();

                    delete_txn1->Delete("default", table_name, delete_row_ids);
                    txn_mgr->CommitTxn(delete_txn1);

                    delete_n += delete_row_n1;
                }

                compact_task->SaveSegmentsData(state);
                {
                    Thread t([&]() {
                        // std::this_thread::sleep_for(std::chrono::seconds(1));
                        txn_mgr->CommitTxn(delete_txn2);
                        delete_n += delete_row_n2;
                    });
                    t.join();
                }
                compact_task->ApplyDeletes(state);
            }
            txn_mgr->CommitTxn(compact_txn);
            {
                auto txn5 = txn_mgr->CreateTxn();
                txn5->Begin();
                try {
                    txn5->Delete("default", table_name, delete_row_ids2);
                    ASSERT_EQ(0, 1);
                } catch (const RecoverableException &e) {
                    EXPECT_EQ(e.ErrorCode(), ErrorCode::kTxnRollback);
                }
                txn_mgr->RollBackTxn(txn5);
            }

            {
                auto txn5 = txn_mgr->CreateTxn();
                txn5->Begin();
                TxnTimeStamp begin_ts = txn5->BeginTS();
                auto [table_entry, status] = txn5->GetTableByName("default", table_name);
                EXPECT_NE(table_entry, nullptr);

                int test_segment_n = segment_sizes.size();
                for (int i = 0; i < test_segment_n; ++i) {
                    auto segment_entry = table_entry->GetSegmentByID(i, begin_ts);
                    EXPECT_NE(segment_entry, nullptr);
                    EXPECT_EQ(segment_entry->status(), SegmentStatus::kDeprecated);
                }
                auto compact_segment = table_entry->GetSegmentByID(test_segment_n, begin_ts);
                EXPECT_NE(compact_segment, nullptr);
                EXPECT_NE(compact_segment->status(), SegmentStatus::kDeprecated);

                EXPECT_EQ(compact_segment->actual_row_count(), row_count - delete_n);

                txn_mgr->CommitTxn(txn5);
            }
        }
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
}

TEST_F(CompactTaskTest, compact_not_exist_table) {
#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::Init();
#endif
    std::shared_ptr<std::string> config_path = nullptr;
    infinity::InfinityContext::instance().Init(config_path);

    Storage *storage = infinity::InfinityContext::instance().storage();
    BufferManager *buffer_mgr = storage->buffer_manager();
    TxnManager *txn_mgr = storage->txn_manager();

    String table_name = "tb1";
    SharedPtr<TableDef> tbl1_def = nullptr;
    {
        Vector<SharedPtr<ColumnDef>> columns;
        i64 column_id = 0;
        HashSet<ConstraintType> constraints;
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
        columns.emplace_back(column_def_ptr);
        tbl1_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_name), columns);
    }
    {
        // create table
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->CreateTable("default", tbl1_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        Vector<SizeT> segment_sizes{1, 10, 100};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_mgr);
    }
    {
        auto txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [table_entry, status] = txn->GetTableByName("default", table_name);
        ASSERT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);

        auto compact_txn = txn_mgr->CreateTxn();
        auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, compact_txn);

        { // drop tb1
            auto drop_txn = txn_mgr->CreateTxn();
            drop_txn->Begin();
            auto status = drop_txn->DropTableCollectionByName("default", table_name, ConflictType::kError);
            ASSERT_TRUE(status.ok());
            txn_mgr->CommitTxn(drop_txn);
        }

        compact_txn->Begin();
        bool res = compact_task->Execute();
        ASSERT_FALSE(res);
        txn_mgr->CommitTxn(compact_txn);
    }

    //------------------------------------------

    {
        // create table
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->CreateTable("default", tbl1_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        Vector<SizeT> segment_sizes{1, 10, 100};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_mgr);
    }
    {
        auto txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [table_entry, status] = txn->GetTableByName("default", table_name);
        ASSERT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);

        auto compact_txn = txn_mgr->CreateTxn();
        auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, compact_txn);

        { // drop tb1
            auto drop_txn = txn_mgr->CreateTxn();
            drop_txn->Begin();
            auto status = drop_txn->DropTableCollectionByName("default", table_name, ConflictType::kError);
            ASSERT_TRUE(status.ok());
            txn_mgr->CommitTxn(drop_txn);
        }
        { // create table with same name
            auto *txn = txn_mgr->CreateTxn();
            txn->Begin();

            Status status = txn->CreateTable("default", tbl1_def, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }

        compact_txn->Begin();
        bool res = compact_task->Execute();
        ASSERT_FALSE(res);
        txn_mgr->CommitTxn(compact_txn);
    }

    infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
    infinity::GlobalResourceUsage::UnInit();
#endif
}
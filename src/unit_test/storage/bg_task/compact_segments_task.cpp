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
#include <thread>

import stl;
import storage;
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
import compaction_process;
import compilation_config;
import logger;
import third_party;

using namespace infinity;

class CompactTaskTest : public BaseTest {
protected:
    void AddSegments(TxnManager *txn_mgr, const String &table_name, const Vector<SizeT> &segment_sizes, BufferManager *buffer_mgr) {
        for (SizeT segment_size : segment_sizes) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import table"));

            auto [table_entry, status] = txn->GetTableByName("default_db", table_name);
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

    void SetUp() override {
        auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        RemoveDbDirs();
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
};

class SilentLogTestCompactTaskTest : public CompactTaskTest {
    void SetUp() override {
        auto config_path = MakeShared<String>(String(test_data_path()) + "/config/test_cleanup_task_silent.toml");
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        RemoveDbDirs();
        infinity::InfinityContext::instance().Init(config_path);
    }
};

TEST_F(CompactTaskTest, compact_to_single_segment) {
    {
        String table_name = "tbl1";

        Storage *storage = infinity::InfinityContext::instance().storage();
        BufferManager *buffer_manager = storage->buffer_manager();
        TxnManager *txn_mgr = storage->txn_manager();
        CompactionProcessor *compaction_process = storage->compaction_processor();

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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        { // add compact
            auto commit_ts = compaction_process->ManualDoCompact("default_db", table_name, false);
            EXPECT_NE(commit_ts, 0u);
        }

        {
            auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("check table"));
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
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
    }
}

TEST_F(CompactTaskTest, compact_to_two_segment) {
    {
        String table_name = "tbl1";

        Storage *storage = infinity::InfinityContext::instance().storage();
        BufferManager *buffer_manager = storage->buffer_manager();
        TxnManager *txn_mgr = storage->txn_manager();
        CompactionProcessor *compaction_process = storage->compaction_processor();

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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000, 1000000, 2000000, 4000000, 8000000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        {
            auto commit_ts = compaction_process->ManualDoCompact("default_db", table_name, false);
            EXPECT_NE(commit_ts, 0u);
        }
        {
            auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("check table"));
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
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
    }
}

TEST_F(CompactTaskTest, compact_with_delete) {
    {
        String table_name = "tbl1";

        Storage *storage = infinity::InfinityContext::instance().storage();
        BufferManager *buffer_manager = storage->buffer_manager();
        TxnManager *txn_mgr = storage->txn_manager();
        CompactionProcessor *compaction_process = storage->compaction_processor();

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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000, 1000000, 2000000, 4000000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        SizeT delete_n = 0;
        {
            auto txn3 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));

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

            auto [table_entry, status] = txn3->GetTableByName("default_db", table_name);
            EXPECT_TRUE(status.ok());
            txn3->Delete(table_entry, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        {
            auto commit_ts = compaction_process->ManualDoCompact("default_db", table_name, false);
            EXPECT_NE(commit_ts, 0u);
        }
        {
            auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("check table"));
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
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
}

TEST_F(SilentLogTestCompactTaskTest, delete_in_compact_process) {
    {
        String table_name = "tbl1";

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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        SizeT row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        int delete_n = 0;
        {
            auto txn3 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));

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

            auto [table_entry, status] = txn3->GetTableByName("default_db", table_name);
            EXPECT_TRUE(status.ok());
            txn3->Delete(table_entry, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        { // add compact
            auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));

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

            auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
            EXPECT_TRUE(status.ok());
            txn5->Delete(table_entry, delete_row_ids);

            Thread t([&]() {
                auto commit_ts = compaction_processor->ManualDoCompact("default_db", table_name, false);
                EXPECT_NE(commit_ts, 0u);
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            txn_mgr->CommitTxn(txn5);
            row_count -= delete_n;
            t.join();
        }
        {
            auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("check table"));
            TxnTimeStamp begin_ts = txn5->BeginTS();
            auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
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

            EXPECT_EQ(compact_segment->actual_row_count(), row_count);

            txn_mgr->CommitTxn(txn5);
        }
    }
}

// Cannot compile the test. So annotate it.

TEST_F(CompactTaskTest, uncommit_delete_in_compact_process) {
    for (int task_i = 0; task_i < 10; ++task_i) {
        LOG_INFO(fmt::format("Test {}", task_i));
        String table_name = fmt::format("tbl{}", task_i);

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
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

            Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
        Vector<SizeT> segment_sizes{1, 10, 100, 1000, 10000, 100000};
        int row_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);

        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_manager);

        SizeT delete_n = 0;
        {
            auto txn3 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));

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
            LOG_INFO(fmt::format("A: delete_n: {}", delete_n));
            auto [table_entry, status] = txn3->GetTableByName("default_db", table_name);
            int total_row_n = table_entry->row_count();
            EXPECT_EQ(total_row_n, row_count);
            EXPECT_TRUE(status.ok());
            txn3->Delete(table_entry, delete_row_ids);

            txn_mgr->CommitTxn(txn3);
        }

        // add compact
        {
            Vector<RowID> delete_row_ids;
            Vector<RowID> delete_row_ids2;

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

                LOG_INFO(fmt::format("Delete1 {} in segment {}", offsets.size(), i));
            }

            auto commit_ts = compaction_processor->ManualDoCompact("default_db", table_name, false, [&]() {
                auto delete_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));
                LOG_INFO(fmt::format("delete1 txn id: {}", delete_txn1->TxnID()));
                auto [table_entry, status] = delete_txn1->GetTableByName("default_db", table_name);
                EXPECT_TRUE(status.ok());

                try {
                    delete_txn1->Delete(table_entry, delete_row_ids);
                    txn_mgr->CommitTxn(delete_txn1);
                    LOG_INFO(fmt::format("Delete 1 is committed, {}", delete_row_n1));
                    delete_n += delete_row_n1;
                } catch (const RecoverableException &e) {
                    LOG_INFO("Delete 1 is row backed");
                }
            });
            EXPECT_NE(commit_ts, 0u);
            {
                auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("delete table"));
                try {
                    auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
                    EXPECT_TRUE(status.ok());
                    txn5->Delete(table_entry, delete_row_ids2);
                    ASSERT_EQ(0, 1);
                } catch (const RecoverableException &e) {
                    EXPECT_EQ(e.ErrorCode(), ErrorCode::kTxnRollback);
                }
                txn_mgr->RollBackTxn(txn5);
            }

            {
                auto txn5 = txn_mgr->BeginTxn(MakeUnique<String>("check table"));
                TxnTimeStamp begin_ts = txn5->BeginTS();
                auto [table_entry, status] = txn5->GetTableByName("default_db", table_name);
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

                if (compact_segment->actual_row_count() != row_count - delete_n) {
                    LOG_ERROR(fmt::format("compact_segment->actual_row_count():{}, row_count: {}, delete_n: {}",
                                          compact_segment->actual_row_count(),
                                          row_count,
                                          delete_n));
                    LOG_ERROR(fmt::format("delete_row_n1: {}, delete_row_n2: {}, row_n: {}", delete_row_n1, delete_row_n2, row_count));
                }
                ASSERT_EQ(compact_segment->actual_row_count(), row_count - delete_n);

                txn_mgr->CommitTxn(txn5);
            }
        }
    }
}

TEST_F(CompactTaskTest, compact_not_exist_table) {
    Storage *storage = infinity::InfinityContext::instance().storage();
    BufferManager *buffer_mgr = storage->buffer_manager();
    TxnManager *txn_mgr = storage->txn_manager();
    CompactionProcessor *compaction_process = storage->compaction_processor();

    String table_name = "tb1";
    SharedPtr<TableDef> tbl1_def = nullptr;
    {
        Vector<SharedPtr<ColumnDef>> columns;
        i64 column_id = 0;
        std::set<ConstraintType> constraints;
        auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
        columns.emplace_back(column_def_ptr);
        tbl1_def = MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), columns);
    }
    {
        // create table
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

        Status status = txn->CreateTable("default_db", tbl1_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        Vector<SizeT> segment_sizes{1, 10, 100};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_mgr);
    }
    {
        { // drop tb1
            auto drop_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
            auto status = drop_txn->DropTableCollectionByName("default_db", table_name, ConflictType::kError);
            ASSERT_TRUE(status.ok());
            txn_mgr->CommitTxn(drop_txn);
        }

        auto commit_ts = compaction_process->ManualDoCompact("default_db", table_name, false);
        EXPECT_EQ(commit_ts, 0u);
    }

    //------------------------------------------

    {
        // create table
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("compact table"));

        Status status = txn->CreateTable("default_db", tbl1_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        Vector<SizeT> segment_sizes{1, 10, 100};
        this->AddSegments(txn_mgr, table_name, segment_sizes, buffer_mgr);
    }
    {
        auto commit_ts = compaction_process->ManualDoCompact("default_db", table_name, false, [&]() {
            { // drop tb1
                auto drop_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
                auto status = drop_txn->DropTableCollectionByName("default_db", table_name, ConflictType::kError);
                ASSERT_TRUE(status.ok());
                txn_mgr->CommitTxn(drop_txn);
            }
            { // create table with same name
                auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));

                Status status = txn->CreateTable("default_db", tbl1_def, ConflictType::kIgnore);
                EXPECT_TRUE(status.ok());

                txn_mgr->CommitTxn(txn);
            }
        });
        EXPECT_EQ(commit_ts, 0u);
    }
}
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

#ifdef CI
#include "gtest/gtest.h"
#include <numeric>
#include <thread>
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"
#include <numeric>
#include <thread>

module infinity_core:ut.compact_segments_task;

import :ut.base_test;
import :stl;
import :storage;
import :infinity_context;
import :status;
import :buffer_manager;
import :column_vector;
import :table_def;
import :value;
import :physical_import;
import :default_values;
import :infinity_exception;
import :base_table_ref;
import :compaction_process;
import :logger;
import :third_party;
import :txn_state;
import :data_block;
import :new_txn_manager;
import :new_txn;
import :db_meeta;
import :table_meeta;
#endif

import global_resource_usage;
import logical_type;
import internal_types;
import extra_ddl_info;
import column_def;
import data_type;
import compilation_config;

using namespace infinity;

class CompactTaskTest : public BaseTestParamStr {
protected:
    void AddSegments(NewTxnManager *txn_mgr, const String &table_name, const Vector<SizeT> &segment_sizes) {

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
                append_to_col(*col1, Value::MakeTinyInt(2));
                input_block->InsertVector(col1, 0);
            }
            input_block->Finalize();
            return input_block;
        };

        for (SizeT segment_size : segment_sizes) {
            for (SizeT i = 0; i < segment_size; ++i) {
                auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
                Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
                Status status = txn->Import("default_db", table_name, input_blocks);
                EXPECT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CompactTaskTest,
                         ::testing::Values((std::string(test_data_path()) + "/config/test_new_bg_on.toml").c_str(),
                                           (std::string(test_data_path()) + "/config/test_new_vfs_off_bg_on.toml").c_str()));

TEST_P(CompactTaskTest, bg_compact) {
    String table_name = "tbl1";
    Storage *storage = infinity::InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

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
    {
        // create table
        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>(table_name), MakeShared<String>(), columns);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);

        Status status = txn->CreateTable("default_db", std::move(tbl1_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    Vector<SizeT> segment_sizes{1, 10, 100};
    SizeT segment_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);
    this->AddSegments(txn_mgr, table_name, segment_sizes);

    i64 compact_interval = InfinityContext::instance().storage()->config()->CleanupInterval();

    SizeT last_seg_count = segment_count;
    i64 loop = 0;
    while (loop < 5) {
        loop++;
        // Wait for the compact task to run
        sleep(compact_interval + 1);

        // Check if the segment count has been reduced
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta("default_db", table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_LT(segment_ids_ptr->size(), last_seg_count);
        last_seg_count = segment_ids_ptr->size();
    }
}

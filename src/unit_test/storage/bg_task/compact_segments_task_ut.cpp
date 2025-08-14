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
        for (SizeT segment_size : segment_sizes) {
            for (SizeT i = 0; i < segment_size; ++i) {
                auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
                auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
                Vector<SharedPtr<DataBlock>> input_blocks = {input_block};
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

TEST_P(CompactTaskTest, DISABLED_bg_compact) {
    Storage *storage = infinity::InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        // create table
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Vector<SizeT> segment_sizes{1, 10, 100};
    SizeT segment_count = std::accumulate(segment_sizes.begin(), segment_sizes.end(), 0);
    this->AddSegments(txn_mgr, *table_name, segment_sizes);

    i64 compact_interval = InfinityContext::instance().storage()->config()->CompactInterval();
    LOG_INFO(fmt::format("compact_interval: {} seconds", compact_interval));
    SizeT last_seg_count = segment_count;
    i64 loop = 0;
    while (loop < 5) {
        loop++;
        // Wait for the compact task to run
        LOG_INFO("Wait for compact task to run");
        sleep(compact_interval + 1);

        // Check if the segment count has been reduced
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_LT(segment_ids_ptr->size(), last_seg_count);
        last_seg_count = segment_ids_ptr->size();
    }
}

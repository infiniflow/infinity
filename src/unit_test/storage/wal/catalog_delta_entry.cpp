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

#include "type/complex/row_id.h"
#include "gtest/gtest.h"
import base_test;

import infinity_exception;
import index_base;
import logger;
import third_party;
import stl;
import catalog_delta_entry;
import column_def;
import segment_entry;
import index_secondary;
import infinity_context;
import data_type;
import logical_type;
import constant_expr;

using namespace infinity;

class CatalogDeltaEntryTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CatalogDeltaEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(CatalogDeltaEntryTest, test_DeltaOpEntry) {
    auto db_name = String("db_test");
    auto db_dir = MakeShared<String>("data");
    auto table_name = String("table_test");
    auto table_entry_dir = MakeShared<String>("data/db_test/table_test");
    Vector<SharedPtr<ColumnDef>> column_defs{};
    {
        auto default_v_1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        default_v_1->integer_value_ = 33;
        auto column_def1 = std::make_shared<ColumnDef>(0,
                                                       std::make_shared<DataType>(LogicalType::kInteger),
                                                       "col1",
                                                       std::set<ConstraintType>(),
                                                       std::move(default_v_1));
        auto default_v_2 = std::make_shared<ConstantExpr>(LiteralType::kString);
        default_v_2->str_value_ = strdup("test_long_str_val_kdglkwhfjlkbn.lzxncl;ha;");
        auto column_def2 = std::make_shared<ColumnDef>(0,
                                                       std::make_shared<DataType>(LogicalType::kVarchar),
                                                       "col2",
                                                       std::set<ConstraintType>(),
                                                       std::move(default_v_2));
        column_defs.push_back(column_def1);
        column_defs.push_back(column_def2);
    }
    SegmentID segment_id = 0;
    BlockID block_id = 0;
    ColumnID column_id = 0;
    auto index_name = MakeShared<String>("index_test");
    auto index_dir = MakeShared<String>("data/db_test/table_test/0/0/index_test");
    auto index_base = IndexSecondary::Make(index_name, "file_name", Vector<String>{"col1", "col2"});
    String segment_filter_binary_data = "abcde";
    String block_filter_binary_data = "abcde";
    String base_name = "chunk1";
    RowID base_rowid = RowID::FromUint64(8192U);
    u32 row_count = 123;

    UniquePtr<char[]> buffer;
    i32 buffer_size = 0;
    UniquePtr<CatalogDeltaEntry> catalog_delta_entry1;
    {
        catalog_delta_entry1 = std::make_unique<CatalogDeltaEntry>();
        {
            auto op = MakeUnique<AddDBEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}", db_name));
            op->db_entry_dir_ = db_dir;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddTableEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}", db_name, table_name));
            op->table_entry_dir_ = table_entry_dir;
            op->column_defs_ = column_defs;
            op->row_count_ = 0;
            op->unsealed_id_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddSegmentEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}", db_name, table_name, segment_id));
            op->status_ = SegmentStatus::kUnsealed;
            op->column_count_ = op->row_count_ = op->actual_row_count_ = op->row_capacity_ = 0;
            op->min_row_ts_ = op->max_row_ts_ = op->deprecate_ts_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddBlockEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}", db_name, table_name, segment_id, block_id));
            op->row_capacity_ = op->row_count_ = op->min_row_ts_ = op->max_row_ts_ = op->checkpoint_ts_ = op->checkpoint_row_count_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddColumnEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}#{}", db_name, table_name, segment_id, block_id, column_id));
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddTableIndexEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}", db_name, table_name, *index_name));
            op->index_dir_ = index_dir;
            op->index_base_ = index_base;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddSegmentIndexEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}", db_name, table_name, *index_name, segment_id));
            op->min_ts_ = op->max_ts_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddChunkIndexEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}#{}", db_name, table_name, *index_name, segment_id, base_name));
            op->base_name_ = base_name;
            op->base_rowid_ = base_rowid;
            op->row_count_ = row_count;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddSegmentEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}", db_name, table_name, segment_id));
            op->segment_filter_binary_data_ = segment_filter_binary_data;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddBlockEntryOp>();
            op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}", db_name, table_name, segment_id, block_id));
            op->block_filter_binary_data_ = block_filter_binary_data;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }

        buffer_size = catalog_delta_entry1->GetSizeInBytes();
        buffer = MakeUnique<char[]>(buffer_size);
        auto *ptr = buffer.get();
        catalog_delta_entry1->WriteAdv(ptr);
        EXPECT_EQ(ptr - buffer.get(), buffer_size);
    }

    {
        const char *ptr = buffer.get();
        auto catalog_delta_entry2 = CatalogDeltaEntry::ReadAdv(ptr, buffer_size);

        size_t op_size = catalog_delta_entry1->operations().size();
        EXPECT_EQ(op_size, catalog_delta_entry2->operations().size());
        for (size_t i = 0; i < op_size; ++i) {
            const auto &op1 = *catalog_delta_entry1->operations()[i];
            const auto &op2 = *catalog_delta_entry2->operations()[i];
            EXPECT_EQ(op1, op2);
        }
    }
//    infinity::InfinityContext::instance().UnInit();
}

TEST_P(CatalogDeltaEntryTest, MergeEntries) {
    auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
    auto local_catalog_delta_entry = std::make_unique<CatalogDeltaEntry>();
    //    local_catalog_delta_entry->set_txn_ids({1});
    //    local_catalog_delta_entry->set_commit_ts(1);
    local_catalog_delta_entry->SaveState(1, 1, 1);

    auto db_name = MakeShared<String>("db_test");
    auto db_dir = MakeShared<String>("data");
    auto table_name = MakeShared<String>("table_test");
    auto table_entry_dir = MakeShared<String>("data/db_test/table_test");
    SegmentID segment_id = 0;
    BlockID block_id = 0;
    ColumnID column_id = 0;
    auto index_name = MakeShared<String>("index_test");
    auto index_dir = MakeShared<String>("data/db_test/table_test/0/0/index_test");
    Vector<SharedPtr<ColumnDef>> column_defs{};
    SharedPtr<IndexBase> index_base{nullptr};

    {
        auto op1 = MakeUnique<AddDBEntryOp>();
        auto op2 = MakeUnique<AddDBEntryOp>();
        auto op1_same_name = MakeUnique<AddDBEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}", *db_name));
        op1_same_name->encode_ = op2->encode_ = op1->encode_ = encode;

        op1_same_name->merge_flag_ = op1->merge_flag_ = MergeFlag::kNew;
        op2->merge_flag_ = MergeFlag::kDelete;

        op1_same_name->db_entry_dir_ = op2->db_entry_dir_ = op1->db_entry_dir_ = db_dir;

        auto op1_copy = MakeUnique<AddDBEntryOp>(*op1);
        op1_copy->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddTableEntryOp>();
        auto op2 = MakeUnique<AddTableEntryOp>();
        auto op1_same_name = MakeUnique<AddTableEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}", *db_name, *table_name));
        op1_same_name->encode_ = op2->encode_ = op1->encode_ = encode;

        op1_same_name->merge_flag_ = op1->merge_flag_ = MergeFlag::kNew;
        op2->merge_flag_ = MergeFlag::kDelete;

        op1_same_name->table_entry_dir_ = op2->table_entry_dir_ = op1->table_entry_dir_ = table_entry_dir;
        op1_same_name->column_defs_ = op2->column_defs_ = op1->column_defs_ = column_defs;

        auto op1_copy = MakeUnique<AddTableEntryOp>(*op1);
        op1_copy->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddSegmentEntryOp>();
        auto op2 = MakeUnique<AddSegmentEntryOp>();
        auto op1_same_name = MakeUnique<AddSegmentEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}#{}", *db_name, *table_name, segment_id));
        op1_same_name->encode_ = op2->encode_ = op1->encode_ = encode;

        op1->status_ = SegmentStatus::kSealed;
        op1->column_count_ = op1->row_count_ = op1->actual_row_count_ = op1->row_capacity_ = 0;
        op1->min_row_ts_ = op1->max_row_ts_ = op1->deprecate_ts_ = 0;

        op1_same_name->status_ = SegmentStatus::kSealed;
        op1_same_name->column_count_ = op1_same_name->row_count_ = op1_same_name->actual_row_count_ = op1_same_name->row_capacity_ = 0;
        op1_same_name->min_row_ts_ = op1_same_name->max_row_ts_ = op1_same_name->deprecate_ts_ = 0;

        op2->status_ = SegmentStatus::kDeprecated;
        op2->column_count_ = op2->row_count_ = op2->actual_row_count_ = op2->row_capacity_ = 0;
        op2->min_row_ts_ = op2->max_row_ts_ = op2->deprecate_ts_ = 0;

        op1_same_name->merge_flag_ = op1->merge_flag_ = MergeFlag::kNew;
        op2->merge_flag_ = MergeFlag::kUpdate;

        auto op1_copy = MakeUnique<AddSegmentEntryOp>(*op1);
        op1_copy->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddBlockEntryOp>();
        auto op1_same_name = MakeUnique<AddBlockEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}#{}#{}", *db_name, *table_name, segment_id, block_id));
        op1_same_name->encode_ = op1->encode_ = encode;

        op1->row_capacity_ = op1->row_count_ = op1->min_row_ts_ = op1->max_row_ts_ = op1->checkpoint_ts_ = op1->checkpoint_row_count_ = 0;

        op1_same_name->row_capacity_ = op1_same_name->row_count_ = op1_same_name->min_row_ts_ = op1_same_name->max_row_ts_ =
            op1_same_name->checkpoint_ts_ = op1_same_name->checkpoint_row_count_ = 0;

        op1->merge_flag_ = MergeFlag::kNew;
        op1_same_name->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<AddColumnEntryOp>();
        auto op1_same_name = MakeUnique<AddColumnEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}#{}#{}#{}", *db_name, *table_name, segment_id, block_id, column_id));
        op1_same_name->encode_ = op1->encode_ = encode;

        op1->merge_flag_ = MergeFlag::kNew;
        op1_same_name->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<AddTableIndexEntryOp>();
        auto op2 = MakeUnique<AddTableIndexEntryOp>();
        auto op1_same_name = MakeUnique<AddTableIndexEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}#{}", *db_name, *table_name, *index_name));
        op1_same_name->encode_ = op2->encode_ = op1->encode_ = encode;

        op1_same_name->merge_flag_ = op1->merge_flag_ = MergeFlag::kNew;
        op2->merge_flag_ = MergeFlag::kDelete;

        op1_same_name->index_dir_ = op2->index_dir_ = op1->index_dir_ = index_dir;
        op1_same_name->index_base_ = op2->index_base_ = op1->index_base_ = index_base;

        op1_same_name->merge_flag_ = op1->merge_flag_ = MergeFlag::kNew;
        op2->merge_flag_ = MergeFlag::kDelete;

        auto op1_copy = MakeUnique<AddTableIndexEntryOp>(*op1);
        op1_copy->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddSegmentIndexEntryOp>();
        auto op1_same_name = MakeUnique<AddSegmentIndexEntryOp>();

        auto encode = MakeShared<String>(fmt::format("#{}#{}#{}#{}", *db_name, *table_name, *index_name, segment_id));
        op1_same_name->encode_ = op1->encode_ = encode;

        op1->min_ts_ = op1->max_ts_ = 0;
        op1_same_name->min_ts_ = op1_same_name->max_ts_ = 0;

        op1->merge_flag_ = MergeFlag::kNew;
        op1_same_name->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<AddSegmentEntryOp>();

        op1->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}", *db_name, *table_name, segment_id));

        op1->segment_filter_binary_data_ = "abcde";

        op1->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
    }
    {
        auto op1 = MakeUnique<AddBlockEntryOp>();

        op1->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}#{}", *db_name, *table_name, segment_id, block_id));

        op1->block_filter_binary_data_ = "abcde";

        op1->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
    }
    {
        auto op = MakeUnique<AddSegmentEntryOp>();

        op->encode_ = MakeUnique<String>(fmt::format("#{}#{}#{}", *db_name, *table_name, segment_id));

        op->status_ = SegmentStatus::kDeprecated;
        op->column_count_ = op->row_count_ = op->actual_row_count_ = op->row_capacity_ = 0;
        op->min_row_ts_ = op->max_row_ts_ = op->deprecate_ts_ = 0;

        op->merge_flag_ = MergeFlag::kUpdate;

        local_catalog_delta_entry->operations().push_back(std::move(op));
    }

    // merge
    global_catalog_delta_entry->ReplayDeltaEntry(std::move(local_catalog_delta_entry));
    // check ops
    EXPECT_EQ(global_catalog_delta_entry->OpSize(), 5u);
}

TEST_P(CatalogDeltaEntryTest, ComplicateMergeEntries) {
    auto db_name = MakeShared<String>("db_test");
    auto db_dir = MakeShared<String>("data");
    auto table_name = MakeShared<String>("table_test");
    auto table_entry_dir = MakeShared<String>("data/db_test/table_test");

    auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
    auto AddDBEntry = [&](CatalogDeltaEntry *delta_entry, MergeFlag merge_flag, TxnTimeStamp commit_ts) {
        auto op1 = MakeUnique<AddDBEntryOp>();
        op1->encode_ = MakeUnique<String>(fmt::format("#{}", *db_name));

        op1->db_entry_dir_ = db_dir;
        op1->merge_flag_ = merge_flag;
        op1->commit_ts_ = commit_ts;
        delta_entry->operations().push_back(std::move(op1));
    };
    auto AddTableEntry = [&](CatalogDeltaEntry *delta_entry, MergeFlag merge_flag, TxnTimeStamp commit_ts) {
        auto op1 = MakeUnique<AddTableEntryOp>();
        op1->encode_ = MakeUnique<String>(fmt::format("#{}#{}", *db_name, *table_name));

        op1->table_entry_dir_ = table_entry_dir;
        op1->merge_flag_ = merge_flag;
        op1->commit_ts_ = commit_ts;
        delta_entry->operations().push_back(std::move(op1));
    };
    {
        TxnTimeStamp commit_ts = 1;
        auto delta_entry = std::make_unique<CatalogDeltaEntry>();
        delta_entry->set_txn_ids({1});
        delta_entry->set_commit_ts(commit_ts);
        AddDBEntry(delta_entry.get(), MergeFlag::kNew, commit_ts);
        AddDBEntry(delta_entry.get(), MergeFlag::kUpdate, commit_ts);
        global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry));

        auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
        EXPECT_EQ(merged_entry->operations().size(), 1u);
    }
    {
        TxnTimeStamp commit_ts = 2;
        auto delta_entry1 = std::make_unique<CatalogDeltaEntry>();
        delta_entry1->set_txn_ids({1});
        delta_entry1->set_commit_ts(commit_ts);
        AddDBEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
        AddDBEntry(delta_entry1.get(), MergeFlag::kUpdate, commit_ts);
        global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry1));

        auto delta_entry2 = std::make_unique<CatalogDeltaEntry>();
        delta_entry2->set_txn_ids({2});
        delta_entry2->set_commit_ts(commit_ts);
        AddDBEntry(delta_entry2.get(), MergeFlag::kDelete, commit_ts);
        AddDBEntry(delta_entry2.get(), MergeFlag::kNew, commit_ts);
        global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry2));

        auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
        EXPECT_EQ(merged_entry->operations().size(), 1u);
    }
    {
        Vector<UniquePtr<CatalogDeltaEntry>> merged_entries;
        {
            TxnTimeStamp commit_ts = 3;
            auto delta_entry1 = std::make_unique<CatalogDeltaEntry>();
            delta_entry1->set_txn_ids({1});
            delta_entry1->set_commit_ts(commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kUpdate, commit_ts);
            global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry1));

            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
            EXPECT_EQ(merged_entry->operations().size(), 1u);
            merged_entries.push_back(std::move(merged_entry));
        }
        {
            TxnTimeStamp commit_ts = 4;
            auto delta_entry1 = std::make_unique<CatalogDeltaEntry>();
            delta_entry1->set_txn_ids({2});
            delta_entry1->set_commit_ts(commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kDelete, commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
            global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry1));
            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
            EXPECT_EQ(merged_entry->operations().size(), 1u);
            merged_entries.push_back(std::move(merged_entry));
        }
        {
            auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
            for (auto &entry : merged_entries) {
                global_catalog_delta_entry->ReplayDeltaEntry(std::move(entry));
            }
            TxnTimeStamp max_ts;
            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(max_ts);
            EXPECT_EQ(max_ts, 4);
            EXPECT_EQ(merged_entry->operations().size(), 1u);
        }
    }
    {
        Vector<UniquePtr<CatalogDeltaEntry>> merged_entries;
        {
            TxnTimeStamp commit_ts = 5;
            auto delta_entry1 = std::make_unique<CatalogDeltaEntry>();
            delta_entry1->set_txn_ids({1});
            delta_entry1->set_commit_ts(commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kUpdate, commit_ts);
            AddTableEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
            AddTableEntry(delta_entry1.get(), MergeFlag::kUpdate, commit_ts);
            global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry1));
            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
            EXPECT_EQ(merged_entry->operations().size(), 2u);
            merged_entries.push_back(std::move(merged_entry));
        }
        {
            TxnTimeStamp commit_ts = 6;
            auto delta_entry1 = std::make_unique<CatalogDeltaEntry>();
            delta_entry1->set_txn_ids({2});
            delta_entry1->set_commit_ts(commit_ts);
            AddTableEntry(delta_entry1.get(), MergeFlag::kUpdate, commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kDelete, commit_ts);
            AddDBEntry(delta_entry1.get(), MergeFlag::kNew, commit_ts);
            global_catalog_delta_entry->ReplayDeltaEntry(std::move(delta_entry1));
            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(commit_ts);
            EXPECT_EQ(merged_entry->operations().size(), 1u);
            merged_entries.push_back(std::move(merged_entry));
        }
        {
            auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
            for (auto &entry : merged_entries) {
                global_catalog_delta_entry->ReplayDeltaEntry(std::move(entry));
            }
            TxnTimeStamp max_ts;
            auto merged_entry = global_catalog_delta_entry->PickFlushEntry(max_ts);
            EXPECT_EQ(max_ts, 6);
            EXPECT_EQ(merged_entry->operations().size(), 1u);
        }
    }
}
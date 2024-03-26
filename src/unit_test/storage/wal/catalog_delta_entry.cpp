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

class CatalogDeltaEntryTest : public BaseTest {
protected:
    void SetUp() override { system("rm -rf /tmp/infinity"); }
};

using namespace infinity;

TEST_F(CatalogDeltaEntryTest, test_DeltaOpEntry) {
    std::shared_ptr<std::string> config_path = nullptr;
    InfinityContext::instance().Init(config_path);

    auto db_name = MakeShared<String>("db_test");
    auto db_dir = MakeShared<String>("data");
    auto table_name = MakeShared<String>("table_test");
    auto table_entry_dir = MakeShared<String>("data/db_test/table_test");
    Vector<SharedPtr<ColumnDef>> column_defs{};
    {
        auto column_def1 =
            std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::unordered_set<ConstraintType>{});
        auto column_def2 =
            std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::unordered_set<ConstraintType>{});
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

    UniquePtr<char[]> buffer;
    i32 buffer_size = 0;
    UniquePtr<CatalogDeltaEntry> catalog_delta_entry1;
    {
        catalog_delta_entry1 = std::make_unique<CatalogDeltaEntry>();

        {
            auto op = MakeUnique<AddDBEntryOp>();
            op->db_name_ = db_name;
            op->db_entry_dir_ = db_dir;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddTableEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->table_entry_dir_ = table_entry_dir;
            op->column_defs_ = column_defs;
            op->row_count_ = 0;
            op->unsealed_id_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddSegmentEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->segment_id_ = segment_id;
            op->status_ = SegmentStatus::kUnsealed;
            op->column_count_ = op->row_count_ = op->actual_row_count_ = op->row_capacity_ = 0;
            op->min_row_ts_ = op->max_row_ts_ = op->deprecate_ts_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddBlockEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->segment_id_ = segment_id;
            op->block_id_ = block_id;
            op->row_capacity_ = op->row_count_ = op->min_row_ts_ = op->max_row_ts_ = op->checkpoint_ts_ = op->checkpoint_row_count_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddColumnEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->segment_id_ = segment_id;
            op->block_id_ = block_id;
            op->column_id_ = column_id;
            op->next_outline_idx_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddTableIndexEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->index_name_ = index_name;
            op->index_dir_ = index_dir;
            op->index_base_ = index_base;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<AddSegmentIndexEntryOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->index_name_ = index_name;
            op->segment_id_ = segment_id;
            op->min_ts_ = op->max_ts_ = 0;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<SetSegmentStatusSealedOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->segment_id_ = segment_id;
            op->segment_filter_binary_data_ = segment_filter_binary_data;
            catalog_delta_entry1->operations().push_back(std::move(op));
        }
        {
            auto op = MakeUnique<SetBlockStatusSealedOp>();
            op->db_name_ = db_name;
            op->table_name_ = table_name;
            op->segment_id_ = segment_id;
            op->block_id_ = block_id;
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
        char *ptr = buffer.get();
        auto catalog_delta_entry2 = CatalogDeltaEntry::ReadAdv(ptr, buffer_size);

        size_t op_size = catalog_delta_entry1->operations().size();
        EXPECT_EQ(op_size, catalog_delta_entry2->operations().size());
        for (size_t i = 0; i < op_size; ++i) {
            const auto &op1 = *catalog_delta_entry1->operations()[i];
            const auto &op2 = *catalog_delta_entry2->operations()[i];
            EXPECT_EQ(op1, op2);
        }
    }
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(CatalogDeltaEntryTest, MergeEntries) {
    auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
    auto local_catalog_delta_entry = std::make_unique<CatalogDeltaEntry>();
    local_catalog_delta_entry->set_txn_ids({1});
    local_catalog_delta_entry->set_commit_ts(1);

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

        op1_same_name->db_name_ = op2->db_name_ = op1->db_name_ = db_name;

        op2->is_delete_ = true;

        op1_same_name->db_entry_dir_ = op2->db_entry_dir_ = op1->db_entry_dir_ = db_dir;

        auto op1_copy = MakeUnique<AddDBEntryOp>(*op1);

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddTableEntryOp>();
        auto op2 = MakeUnique<AddTableEntryOp>();
        auto op1_same_name = MakeUnique<AddTableEntryOp>();

        op1_same_name->db_name_ = op2->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op2->table_name_ = op1->table_name_ = table_name;

        op2->is_delete_ = true;

        op1_same_name->table_entry_dir_ = op2->table_entry_dir_ = op1->table_entry_dir_ = table_entry_dir;
        op1_same_name->column_defs_ = op2->column_defs_ = op1->column_defs_ = column_defs;

        auto op1_copy = MakeUnique<AddTableEntryOp>(*op1);

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddSegmentEntryOp>();
        auto op2 = MakeUnique<AddSegmentEntryOp>();
        auto op1_same_name = MakeUnique<AddSegmentEntryOp>();

        op1_same_name->db_name_ = op2->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op2->table_name_ = op1->table_name_ = table_name;
        op1_same_name->segment_id_ = op2->segment_id_ = op1->segment_id_ = segment_id;

        op1->status_ = SegmentStatus::kSealed;
        op1->column_count_ = op1->row_count_ = op1->actual_row_count_ = op1->row_capacity_ = 0;
        op1->min_row_ts_ = op1->max_row_ts_ = op1->deprecate_ts_ = 0;

        op1_same_name->status_ = SegmentStatus::kSealed;
        op1_same_name->column_count_ = op1_same_name->row_count_ = op1_same_name->actual_row_count_ = op1_same_name->row_capacity_ = 0;
        op1_same_name->min_row_ts_ = op1_same_name->max_row_ts_ = op1_same_name->deprecate_ts_ = 0;

        op2->status_ = SegmentStatus::kDeprecated;
        op2->column_count_ = op2->row_count_ = op2->actual_row_count_ = op2->row_capacity_ = 0;
        op2->min_row_ts_ = op2->max_row_ts_ = op2->deprecate_ts_ = 0;

        auto op1_copy = MakeUnique<AddSegmentEntryOp>(*op1);

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddBlockEntryOp>();
        auto op1_same_name = MakeUnique<AddBlockEntryOp>();

        op1_same_name->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op1->table_name_ = table_name;
        op1_same_name->segment_id_ = op1->segment_id_ = segment_id;
        op1_same_name->block_id_ = op1->block_id_ = block_id;

        op1->row_capacity_ = op1->row_count_ = op1->min_row_ts_ = op1->max_row_ts_ = op1->checkpoint_ts_ = op1->checkpoint_row_count_ = 0;

        op1_same_name->row_capacity_ = op1_same_name->row_count_ = op1_same_name->min_row_ts_ = op1_same_name->max_row_ts_ =
            op1_same_name->checkpoint_ts_ = op1_same_name->checkpoint_row_count_ = 0;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<AddColumnEntryOp>();
        auto op1_same_name = MakeUnique<AddColumnEntryOp>();

        op1_same_name->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op1->table_name_ = table_name;
        op1_same_name->segment_id_ = op1->segment_id_ = segment_id;
        op1_same_name->block_id_ = op1->block_id_ = block_id;
        op1_same_name->column_id_ = op1->column_id_ = column_id;

        op1_same_name->next_outline_idx_ = op1->next_outline_idx_ = 0;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<AddTableIndexEntryOp>();
        auto op2 = MakeUnique<AddTableIndexEntryOp>();
        auto op1_same_name = MakeUnique<AddTableIndexEntryOp>();

        op1_same_name->db_name_ = op2->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op2->table_name_ = op1->table_name_ = table_name;
        op1_same_name->index_name_ = op2->index_name_ = op1->index_name_ = index_name;

        op2->is_delete_ = true;

        op1_same_name->index_dir_ = op2->index_dir_ = op1->index_dir_ = index_dir;
        op1_same_name->index_base_ = op2->index_base_ = op1->index_base_ = index_base;

        auto op1_copy = MakeUnique<AddTableIndexEntryOp>(*op1);

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op2));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
        local_catalog_delta_entry->operations().push_back(std::move(op1_copy));
    }
    {
        auto op1 = MakeUnique<AddSegmentIndexEntryOp>();
        auto op1_same_name = MakeUnique<AddSegmentIndexEntryOp>();

        op1_same_name->db_name_ = op1->db_name_ = db_name;
        op1_same_name->table_name_ = op1->table_name_ = table_name;
        op1_same_name->index_name_ = op1->index_name_ = index_name;
        op1_same_name->segment_id_ = op1->segment_id_ = segment_id;

        op1->min_ts_ = op1->max_ts_ = 0;
        op1_same_name->min_ts_ = op1_same_name->max_ts_ = 0;

        local_catalog_delta_entry->operations().push_back(std::move(op1));
        local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));
    }
    {
        auto op1 = MakeUnique<SetSegmentStatusSealedOp>();

        op1->db_name_ = db_name;
        op1->table_name_ = table_name;
        op1->segment_id_ = segment_id;
        op1->segment_filter_binary_data_ = "abcde";

        local_catalog_delta_entry->operations().push_back(std::move(op1));
    }
    {
        auto op1 = MakeUnique<SetBlockStatusSealedOp>();

        op1->db_name_ = db_name;
        op1->table_name_ = table_name;
        op1->segment_id_ = segment_id;
        op1->block_id_ = block_id;
        op1->block_filter_binary_data_ = "abcde";

        local_catalog_delta_entry->operations().push_back(std::move(op1));
    }
    {
        auto op = MakeUnique<AddSegmentEntryOp>();

        op->db_name_ = db_name;
        op->table_name_ = table_name;
        op->segment_id_ = segment_id;

        op->status_ = SegmentStatus::kDeprecated;
        op->column_count_ = op->row_count_ = op->actual_row_count_ = op->row_capacity_ = 0;
        op->min_row_ts_ = op->max_row_ts_ = op->deprecate_ts_ = 0;

        local_catalog_delta_entry->operations().push_back(std::move(op));
    }

    // merge
    global_catalog_delta_entry->AddDeltaEntry(std::move(local_catalog_delta_entry), 0);
    // check ops
    EXPECT_EQ(global_catalog_delta_entry->OpSize(), 4u);
}

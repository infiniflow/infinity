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
    auto block_filter_binary_data = Vector<Pair<BlockID, String>>{{0, "abcde"}, {1, "fghij"}};

    UniquePtr<char[]> buffer;
    i32 buffer_size = 0;
    UniquePtr<CatalogDeltaEntry> catalog_delta_entry1;
    {
        catalog_delta_entry1 = std::make_unique<CatalogDeltaEntry>();

        auto op0 = MakeUnique<AddDBEntryOp>(0, false, 0, 0, db_name, db_dir);
        catalog_delta_entry1->operations().push_back(std::move(op0));

        auto op1 = MakeUnique<AddTableEntryOp>(1, false, 0, 0, db_name, table_name, table_entry_dir, column_defs, 0, 0);
        catalog_delta_entry1->operations().push_back(std::move(op1));

        auto op2 = MakeUnique<AddSegmentEntryOp>(2, false, 0, 0, db_name, table_name, segment_id, SegmentStatus::kSealed, 0, 0, 0, 0, 0, 0, 0);
        catalog_delta_entry1->operations().push_back(std::move(op2));

        auto op3 = MakeUnique<AddBlockEntryOp>(3, false, 0, 0, db_name, table_name, segment_id, block_id, 0, 0, 0, 0, 0, 0);
        catalog_delta_entry1->operations().push_back(std::move(op3));

        auto op4 = MakeUnique<AddColumnEntryOp>(4, false, 0, 0, db_name, table_name, segment_id, block_id, column_id, 0);
        catalog_delta_entry1->operations().push_back(std::move(op4));

        auto op5 = MakeUnique<AddTableIndexEntryOp>(5, false, 0, 0, db_name, table_name, index_name, index_dir, index_base);
        catalog_delta_entry1->operations().push_back(std::move(op5));

        auto op6 = MakeUnique<AddFulltextIndexEntryOp>(6, false, 0, 0, db_name, table_name, index_name);
        catalog_delta_entry1->operations().push_back(std::move(op6));

        auto op7 = MakeUnique<AddSegmentIndexEntryOp>(7, false, 0, 0, db_name, table_name, index_name, segment_id, 0, 14);
        catalog_delta_entry1->operations().push_back(std::move(op7));

        auto op8 = MakeUnique<SetSegmentStatusSealedOp>(8,
                                                        false,
                                                        0,
                                                        0,
                                                        db_name,
                                                        table_name,
                                                        segment_id,
                                                        std::move(segment_filter_binary_data),
                                                        std::move(block_filter_binary_data));
        catalog_delta_entry1->operations().push_back(std::move(op8));

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
    local_catalog_delta_entry->set_txn_id(1);
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

    // db entry
    auto op1 = MakeUnique<AddDBEntryOp>(2, false, 0, 0, db_name, db_dir);
    auto op2 = MakeUnique<AddDBEntryOp>(3, true, 0, 0, db_name, db_dir);
    auto op1_same_name = MakeUnique<AddDBEntryOp>(2, false, 0, 0, db_name, db_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op1));
    local_catalog_delta_entry->operations().push_back(std::move(op2));
    local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));

    // table entry
    auto op4 = MakeUnique<AddTableEntryOp>(5, false, 0, 0, db_name, table_name, table_entry_dir, column_defs, 0, 0);
    auto op5 = MakeUnique<AddTableEntryOp>(6, true, 0, 0, db_name, table_name, table_entry_dir, column_defs, 0, 0);
    auto op4_same_name = MakeUnique<AddTableEntryOp>(5, 0, 0, false, db_name, table_name, table_entry_dir, column_defs, 0, 0);
    local_catalog_delta_entry->operations().push_back(std::move(op4));
    local_catalog_delta_entry->operations().push_back(std::move(op5));
    local_catalog_delta_entry->operations().push_back(std::move(op4_same_name));

    // segment entry
    auto op7 = MakeUnique<AddSegmentEntryOp>(7, false, 0, 0, db_name, table_name, segment_id, SegmentStatus::kSealed, 0, 0, 0, 0, 0, 0, 0);
    auto op7_same_name = MakeUnique<AddSegmentEntryOp>(7, false, 0, 0, db_name, table_name, segment_id, SegmentStatus::kSealed, 0, 0, 0, 0, 0, 0, 0);
    local_catalog_delta_entry->operations().push_back(std::move(op7));
    local_catalog_delta_entry->operations().push_back(std::move(op7_same_name));

    // block entry
    auto op9 = MakeUnique<AddBlockEntryOp>(8, false, 0, 0, db_name, table_name, segment_id, block_id, 0, 0, 0, 0, 0, 0);
    auto op9_same_name = MakeUnique<AddBlockEntryOp>(8, false, 0, 0, db_name, table_name, segment_id, block_id, 0, 0, 0, 0, 0, 0);
    local_catalog_delta_entry->operations().push_back(std::move(op9));
    local_catalog_delta_entry->operations().push_back(std::move(op9_same_name));

    // column entry
    auto op11 = MakeUnique<AddColumnEntryOp>(9, false, 0, 0, db_name, table_name, segment_id, block_id, column_id, 0);
    auto op11_same_name = MakeUnique<AddColumnEntryOp>(9, false, 0, 0, db_name, table_name, segment_id, block_id, column_id, 0);
    local_catalog_delta_entry->operations().push_back(std::move(op11));
    local_catalog_delta_entry->operations().push_back(std::move(op11_same_name));

    // table index entry
    auto op13 = MakeUnique<AddTableIndexEntryOp>(11, false, 0, 0, db_name, table_name, index_name, index_dir, index_base);
    auto op13_same_name = MakeUnique<AddTableIndexEntryOp>(11, false, 0, 0, db_name, table_name, index_name, index_dir, index_base);
    local_catalog_delta_entry->operations().push_back(std::move(op13));
    local_catalog_delta_entry->operations().push_back(std::move(op13_same_name));

    // irs index entry
    auto op15 = MakeUnique<AddFulltextIndexEntryOp>(12, false, 0, 0, db_name, table_name, index_name);
    auto op15_same_name = MakeUnique<AddFulltextIndexEntryOp>(12, false, 0, 0, db_name, table_name, index_name);
    local_catalog_delta_entry->operations().push_back(std::move(op15));
    local_catalog_delta_entry->operations().push_back(std::move(op15_same_name));

    // segment index entry
    auto op17 = MakeUnique<AddSegmentIndexEntryOp>(14, false, 0, 0, db_name, table_name, index_name, segment_id, 0, 14);
    auto op17_same_name = MakeUnique<AddSegmentIndexEntryOp>(14, false, 0, 0, db_name, table_name, index_name, segment_id, 1, 14);
    local_catalog_delta_entry->operations().push_back(std::move(op17));
    local_catalog_delta_entry->operations().push_back(std::move(op17_same_name));

    EXPECT_EQ(local_catalog_delta_entry->operations().size(), 18u);
    // merge
    Vector<UniquePtr<CatalogDeltaEntry>> delta_entries;
    delta_entries.push_back(std::move(local_catalog_delta_entry));
    global_catalog_delta_entry->AddDeltaEntries(std::move(delta_entries));
    // check ops
    EXPECT_EQ(global_catalog_delta_entry->OpSize(), 8u);
}
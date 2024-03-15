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

class CatalogDeltaEntryTest : public BaseTest {};

using namespace infinity;

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
    String col_index_dir{"data/db_test/table_test/0/0/index_test"};
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
    auto op4 = MakeUnique<AddTableEntryOp>(5, false, 0, 0, db_name, table_name, table_entry_dir, column_defs, 0);
    auto op5 = MakeUnique<AddTableEntryOp>(6, true, 0, 0, db_name, table_name, table_entry_dir, column_defs, 0);
    auto op4_same_name = MakeUnique<AddTableEntryOp>(5, 0, 0, false, db_name, table_name, table_entry_dir, column_defs, 0);
    local_catalog_delta_entry->operations().push_back(std::move(op4));
    local_catalog_delta_entry->operations().push_back(std::move(op5));
    local_catalog_delta_entry->operations().push_back(std::move(op4_same_name));

    // segment entry
    auto op7 = MakeUnique<AddSegmentEntryOp>(7, false, 0, 0, db_name, table_name, segment_id, SegmentStatus::kSealed, 0, 0, 0, 0, 0, 0);
    auto op7_same_name = MakeUnique<AddSegmentEntryOp>(7, false, 0, 0, db_name, table_name, segment_id, SegmentStatus::kSealed, 0, 0, 0, 0, 0, 0);
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
    global_catalog_delta_entry->Merge(std::move(local_catalog_delta_entry));
    // check ops
    EXPECT_EQ(global_catalog_delta_entry->operations().size(), 18u);
    // check member
    EXPECT_EQ(global_catalog_delta_entry->txn_id(), 1u);
    EXPECT_EQ(global_catalog_delta_entry->commit_ts(), 1u);
}
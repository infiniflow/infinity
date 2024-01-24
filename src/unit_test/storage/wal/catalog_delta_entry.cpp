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

import logger;
import third_party;
import stl;
import catalog_delta_entry;

class CatalogDeltaEntryTest : public BaseTest {};

using namespace infinity;

TEST_F(CatalogDeltaEntryTest, MergeEntries) {
    auto global_catalog_delta_entry = std::make_unique<GlobalCatalogDeltaEntry>();
    auto local_catalog_delta_entry = std::make_unique<CatalogDeltaEntry>();
    local_catalog_delta_entry->set_txn_id(1);
    local_catalog_delta_entry->set_commit_ts(1);

    String db_name{"db_test"};
    String db_dir{"data"};
    String table_name{"table_test"};
    String table_entry_dir{"data/db_test/table_test"};
    SegmentID segment_id = 0;
    String segment_dir{"data/db_test/table_test/0"};
    BlockID block_id = 0;
    String block_dir{"data/db_test/table_test/0/0"};
    ColumnID column_id = 0;
    String index_name{"index_test"};
    String index_dir{"data/db_test/table_test/0/0/index_test"};
    String col_index_dir{"data/db_test/table_test/0/0/index_test"};

    // db meta
    auto op1 = MakeUnique<AddDBMetaOperation>(1, false, db_name, db_dir);
    auto op1_same_name = MakeUnique<AddDBMetaOperation>(1, false, db_name, db_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op1));
    local_catalog_delta_entry->operations().push_back(std::move(op1_same_name));

    // db entry
    auto op2 = MakeUnique<AddDBEntryOperation>(2, false, db_name, db_dir);
    auto op3 = MakeUnique<AddDBEntryOperation>(3, true, db_name, db_dir);
    auto op2_same_name = MakeUnique<AddDBEntryOperation>(2, false, db_name, db_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op2));
    local_catalog_delta_entry->operations().push_back(std::move(op3));
    local_catalog_delta_entry->operations().push_back(std::move(op2_same_name));

    // table meta
    auto op4 = MakeUnique<AddTableMetaOperation>(4, false, db_name, table_name, db_dir);
    auto op4_same_name = MakeUnique<AddTableMetaOperation>(4, false, db_name, table_name, db_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op4));
    local_catalog_delta_entry->operations().push_back(std::move(op4_same_name));

    // table entry
    auto op5 = MakeUnique<AddTableEntryOperation>(5, false, db_name, table_name, table_entry_dir);
    auto op6 = MakeUnique<AddTableEntryOperation>(6, true, db_name, table_name, table_entry_dir);
    auto op5_same_name = MakeUnique<AddTableEntryOperation>(5, false, db_name, table_name, table_entry_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op5));
    local_catalog_delta_entry->operations().push_back(std::move(op6));
    local_catalog_delta_entry->operations().push_back(std::move(op5_same_name));

    // segment entry
    auto op7 = MakeUnique<AddSegmentEntryOperation>(7, false, db_name, table_name, segment_id, segment_dir);
    auto op7_same_name = MakeUnique<AddSegmentEntryOperation>(7, false, db_name, table_name, segment_id, segment_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op7));
    local_catalog_delta_entry->operations().push_back(std::move(op7_same_name));

    // block entry
    auto op8 = MakeUnique<AddBlockEntryOperation>(8, false, db_name, table_name, segment_id, block_id, block_dir);
    auto op8_same_name = MakeUnique<AddBlockEntryOperation>(8, false, db_name, table_name, segment_id, block_id, block_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op8));
    local_catalog_delta_entry->operations().push_back(std::move(op8_same_name));

    // column entry
    auto op9 = MakeUnique<AddColumnEntryOperation>(9, false, db_name, table_name, segment_id, block_id, column_id);
    auto op9_same_name = MakeUnique<AddColumnEntryOperation>(9, false, db_name, table_name, segment_id, block_id, column_id);
    local_catalog_delta_entry->operations().push_back(std::move(op9));
    local_catalog_delta_entry->operations().push_back(std::move(op9_same_name));

    // index meta
    auto op10 = MakeUnique<AddIndexMetaOperation>(10, false, db_name, table_name, index_name);
    auto op10_same_name = MakeUnique<AddIndexMetaOperation>(10, false, db_name, table_name, index_name);
    local_catalog_delta_entry->operations().push_back(std::move(op10));
    local_catalog_delta_entry->operations().push_back(std::move(op10_same_name));

    // table index entry
    auto op11 = MakeUnique<AddTableIndexEntryOperation>(11, false, db_name, table_name, index_name, index_dir);
    auto op11_same_name = MakeUnique<AddTableIndexEntryOperation>(11, false, db_name, table_name, index_name, index_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op11));
    local_catalog_delta_entry->operations().push_back(std::move(op11_same_name));

    // irs index entry
    auto op12 = MakeUnique<AddIrsIndexEntryOperation>(12, false, db_name, table_name, index_name, index_dir);
    auto op12_same_name = MakeUnique<AddIrsIndexEntryOperation>(12, false, db_name, table_name, index_name, index_dir);
    local_catalog_delta_entry->operations().push_back(std::move(op12));
    local_catalog_delta_entry->operations().push_back(std::move(op12_same_name));

    // column index entry
    auto op13 = MakeUnique<AddColumnIndexEntryOperation>(13, false, db_name, table_name, index_name, col_index_dir, column_id);
    auto op13_same_name = MakeUnique<AddColumnIndexEntryOperation>(13, false, db_name, table_name, index_name, col_index_dir, column_id);
    local_catalog_delta_entry->operations().push_back(std::move(op13));
    local_catalog_delta_entry->operations().push_back(std::move(op13_same_name));

    // segment column index entry
    auto op14 = MakeUnique<AddSegmentColumnIndexEntryOperation>(14, false, db_name, table_name, index_name, column_id, segment_id, 0, 14);
    auto op14_same_name = MakeUnique<AddSegmentColumnIndexEntryOperation>(14, false, db_name, table_name, index_name, column_id, segment_id, 1, 14);
    local_catalog_delta_entry->operations().push_back(std::move(op14));
    local_catalog_delta_entry->operations().push_back(std::move(op14_same_name));

    EXPECT_EQ(local_catalog_delta_entry->operations().size(), 26);
    // merge
    global_catalog_delta_entry->Merge(std::move(local_catalog_delta_entry));
    // check ops
    EXPECT_EQ(global_catalog_delta_entry->operations().size(), 14);
    // check maps
    EXPECT_EQ(global_catalog_delta_entry->encode_to_id_map().size(), 14);
    // check member
    EXPECT_EQ(global_catalog_delta_entry->txn_id(), 1);
    EXPECT_EQ(global_catalog_delta_entry->commit_ts(), 1);
}
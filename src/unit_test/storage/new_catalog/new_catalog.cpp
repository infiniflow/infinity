// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include "gtest/gtest.h"

import base_test;
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import data_access_state;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import constant_expr;

using namespace infinity;

class NewCatalogTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewCatalogTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(NewCatalogTest, test_block_lock) {
    using namespace infinity;

    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    String db_id_str = "0";
    String table_id_str = "0";
    SegmentID segment_id = 0;
    BlockID block_id = 0;
    String block_key1 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
    String block_key2 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id + 1, "lock");

    Status status = new_catalog->AddBlockLock(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->AddBlockLock(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
        EXPECT_TRUE(status.ok());
    }
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        EXPECT_FALSE(status.ok());
    }
}

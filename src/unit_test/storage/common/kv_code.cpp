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

// import infinity_context;
// import infinity_exception;
import kv_code;
import stl;
// import global_resource_usage;
// import third_party;
// import logger;
// import table_def;
// import value;
//
// import data_block;
// import default_values;
// import txn_manager;
// import buffer_manager;
// import txn;
// import catalog;
// import status;
// import extra_ddl_info;
// import column_def;
// import data_type;
// import logical_type;
// import embedding_info;
// import index_hnsw;
// import statement_common;
// import data_access_state;
// import txn_store;
// import txn_state;
//
// import base_entry;
// import kv_store;

using namespace infinity;

class KVCodeTest : public BaseTest {};

TEST_F(KVCodeTest, kv_code1) {
    // Test code here
    String db_name = "db1";
    String table_name = "tbl1";
    TxnTimeStamp ts = 123;
    TransactionID txn_id = 23;
    SegmentID segment_id = 2;
    BlockID block_id = 3;
    ColumnID column_id = 4;
    String index_name = "idx1";
    ChunkID chunk_id = 5;
    {
        String key = KeyEncode::DatabaseKey(db_name, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "db|db1|123|23");
        String key_prefix = KeyEncode::DatabasePrefix(db_name);
        EXPECT_STREQ(key_prefix.c_str(), "db|db1|");
    }
    {
        String key = KeyEncode::TableKey(db_name, table_name, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "tbl|db1|tbl1|123|23");
        String key_prefix = KeyEncode::TableKeyPrefix(db_name, table_name);
        EXPECT_STREQ(key_prefix.c_str(), "tbl|db1|tbl1|");
    }
    {
        String key = KeyEncode::TableSegmentKey(db_name, table_name, segment_id, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "seg|db1|tbl1|2|123|23");
        String key_prefix = KeyEncode::TableSegmentKeyPrefix(db_name, table_name, segment_id);
        EXPECT_STREQ(key_prefix.c_str(), "seg|db1|tbl1|2|");
    }
    {
        String key = KeyEncode::TableSegmentBlockKey(db_name, table_name, segment_id, block_id, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "blk|db1|tbl1|2|3|123|23");
        String key_prefix = KeyEncode::TableSegmentBlockKeyPrefix(db_name, table_name, segment_id, block_id);
        EXPECT_STREQ(key_prefix.c_str(), "blk|db1|tbl1|2|3|");
    }
    {
        String key = KeyEncode::TableSegmentBlockColumnKey(db_name, table_name, segment_id, block_id, column_id, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "col|db1|tbl1|2|3|4|123|23");
        String key_prefix = KeyEncode::TableSegmentBlockColumnKeyPrefix(db_name, table_name, segment_id, block_id, column_id);
        EXPECT_STREQ(key_prefix.c_str(), "col|db1|tbl1|2|3|4|");
    }
    {
        String key = KeyEncode::TableIndexKey(db_name, table_name, index_name, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "idx|db1|tbl1|idx1|123|23");
        String key_prefix = KeyEncode::TableIndexKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx|db1|tbl1|idx1|");
    }
    {
        String key = KeyEncode::TableIndexSegmentKey(db_name, table_name, index_name, segment_id, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "idx_seg|db1|tbl1|idx1|2|123|23");
        String key_prefix = KeyEncode::TableIndexSegmentKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_seg|db1|tbl1|idx1|");
    }
    {
        String key = KeyEncode::TableIndexChunkKey(db_name, table_name, index_name, segment_id, chunk_id, ts, txn_id);
        EXPECT_STREQ(key.c_str(), "idx_chunk|db1|tbl1|idx1|2|5|123|23");
        String key_prefix = KeyEncode::TableIndexChunkKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_chunk|db1|tbl1|idx1|");
    }
}
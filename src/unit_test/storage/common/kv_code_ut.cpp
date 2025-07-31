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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.kv_code;

import :ut.base_test;
import :kv_code;
import :stl;
#endif

using namespace infinity;

class KVCodeTest : public BaseTest {};

TEST_F(KVCodeTest, kv_code1) {
    // Test code here
    String db_name = "db1";
    String db_id = "3";
    String table_name = "tbl1";
    String table_id = "10";
    String column_name = "col1";
    TxnTimeStamp ts = 123;
    SegmentID segment_id = 2;
    //    BlockID block_id = 3;
    //    ColumnID column_id = 4;
    //    String index_name = "idx1";
    //    ChunkID chunk_id = 5;
    {
        String key = KeyEncode::CatalogDbKey(db_name, ts);
        EXPECT_STREQ(key.c_str(), "catalog|db|db1|123");
        key = KeyEncode::CatalogDbPrefix(db_name);
        EXPECT_STREQ(key.c_str(), "catalog|db|db1|");

        key = KeyEncode::CatalogDbTagPrefix(db_id, "comment");
        EXPECT_STREQ(key.c_str(), "db|3|comment|");
    }
    {
        String key = KeyEncode::CatalogTableKey(db_id, table_name, ts);
        EXPECT_STREQ(key.c_str(), "catalog|tbl|3|tbl1|123");
    }
    {
        String key = KeyEncode::TableColumnKey(db_id, table_id, column_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|col|3|10|col1|123");
    }
    {
        String key = KeyEncode::CatalogTableSegmentKey(db_id, table_id, segment_id);
        EXPECT_STREQ(key.c_str(), "catalog|seg|3|10|2");
    }
}

TEST_F(KVCodeTest, kv_code2) {
    // Test code here
    String db_name = "db1";
    String db_id_str = "3";
    String table_id_str = "10";
    String table_name = "tbl1";
    String column_name = "col1";
    TxnTimeStamp ts = 123;
    SegmentID segment_id = 2;
    BlockID block_id = 3;
    ColumnID column_id = 4;
    String column_id_str = "4";
    String index_name = "idx1";
    ChunkID chunk_id = 5;
    {
        String key = KeyEncode::DatabaseKey(db_name, ts);
        EXPECT_STREQ(key.c_str(), "db|db1|123");
        String key_prefix = KeyEncode::DatabasePrefix(db_name);
        EXPECT_STREQ(key_prefix.c_str(), "db|db1|");
    }
    {
        String key = KeyEncode::TableKey(db_name, table_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|db1|tbl1|123");
        String key_prefix = KeyEncode::TableKeyPrefix(db_name, table_name);
        EXPECT_STREQ(key_prefix.c_str(), "tbl|db1|tbl1|");
    }
    {
        String key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|col|3|10|col1|123");

        String key_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
        EXPECT_STREQ(key_prefix.c_str(), "tbl|col|3|10|");

        String tag_prefix = KeyEncode::TableColumnTagKey(db_id_str, table_id_str, column_id_str, "tag1");
        EXPECT_STREQ(tag_prefix.c_str(), "col|3|10|4|tag1");
    }
    {
        String key = KeyEncode::TableSegmentKey(db_name, table_name, segment_id, ts);
        EXPECT_STREQ(key.c_str(), "seg|db1|tbl1|2|123");
        String key_prefix = KeyEncode::TableSegmentKeyPrefix(db_name, table_name, segment_id);
        EXPECT_STREQ(key_prefix.c_str(), "seg|db1|tbl1|2|");
    }
    {
        String key = KeyEncode::TableSegmentBlockKey(db_name, table_name, segment_id, block_id, ts);
        EXPECT_STREQ(key.c_str(), "blk|db1|tbl1|2|3|123");
        String key_prefix = KeyEncode::TableSegmentBlockKeyPrefix(db_name, table_name, segment_id, block_id);
        EXPECT_STREQ(key_prefix.c_str(), "blk|db1|tbl1|2|3|");
    }
    {
        String key = KeyEncode::TableSegmentBlockColumnKey(db_name, table_name, segment_id, block_id, column_id, ts);
        EXPECT_STREQ(key.c_str(), "col|db1|tbl1|2|3|4|123");
        String key_prefix = KeyEncode::TableSegmentBlockColumnKeyPrefix(db_name, table_name, segment_id, block_id, column_id);
        EXPECT_STREQ(key_prefix.c_str(), "col|db1|tbl1|2|3|4|");
    }
    {
        String key = KeyEncode::TableIndexKey(db_name, table_name, index_name, ts);
        EXPECT_STREQ(key.c_str(), "idx|db1|tbl1|idx1|123");
        String key_prefix = KeyEncode::TableIndexKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx|db1|tbl1|idx1|");
    }
    {
        String key = KeyEncode::TableIndexSegmentKey(db_name, table_name, index_name, segment_id, ts);
        EXPECT_STREQ(key.c_str(), "idx_seg|db1|tbl1|idx1|2|123");
        String key_prefix = KeyEncode::TableIndexSegmentKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_seg|db1|tbl1|idx1|");
    }
    {
        String key = KeyEncode::TableIndexChunkKey(db_name, table_name, index_name, segment_id, chunk_id, ts);
        EXPECT_STREQ(key.c_str(), "idx_chunk|db1|tbl1|idx1|2|5|123");
        String key_prefix = KeyEncode::TableIndexChunkKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_chunk|db1|tbl1|idx1|");
    }
}

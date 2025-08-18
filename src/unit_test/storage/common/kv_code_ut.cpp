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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.kv_code;

import :ut.base_test;
import :kv_code;

using namespace infinity;

class KVCodeTest : public BaseTest {};

TEST_F(KVCodeTest, kv_code1) {
    // Test code here
    std::string db_name = "db1";
    std::string db_id = "3";
    std::string table_name = "tbl1";
    std::string table_id = "10";
    std::string column_name = "col1";
    TxnTimeStamp ts = 123;
    SegmentID segment_id = 2;
    //    BlockID block_id = 3;
    //    ColumnID column_id = 4;
    //    std::string index_name = "idx1";
    //    ChunkID chunk_id = 5;
    {
        std::string key = KeyEncode::CatalogDbKey(db_name, ts);
        EXPECT_STREQ(key.c_str(), "catalog|db|db1|123");
        key = KeyEncode::CatalogDbPrefix(db_name);
        EXPECT_STREQ(key.c_str(), "catalog|db|db1|");

        key = KeyEncode::CatalogDbTagPrefix(db_id, "comment");
        EXPECT_STREQ(key.c_str(), "db|3|comment|");
    }
    {
        std::string key = KeyEncode::CatalogTableKey(db_id, table_name, ts);
        EXPECT_STREQ(key.c_str(), "catalog|tbl|3|tbl1|123");
    }
    {
        std::string key = KeyEncode::TableColumnKey(db_id, table_id, column_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|col|3|10|col1|123");
    }
    {
        std::string key = KeyEncode::CatalogTableSegmentKey(db_id, table_id, segment_id);
        EXPECT_STREQ(key.c_str(), "catalog|seg|3|10|2");
    }
}

TEST_F(KVCodeTest, kv_code2) {
    // Test code here
    std::string db_name = "db1";
    std::string db_id_str = "3";
    std::string table_id_str = "10";
    std::string table_name = "tbl1";
    std::string column_name = "col1";
    TxnTimeStamp ts = 123;
    SegmentID segment_id = 2;
    BlockID block_id = 3;
    ColumnID column_id = 4;
    std::string column_id_str = "4";
    std::string index_name = "idx1";
    ChunkID chunk_id = 5;
    {
        std::string key = KeyEncode::DatabaseKey(db_name, ts);
        EXPECT_STREQ(key.c_str(), "db|db1|123");
        std::string key_prefix = KeyEncode::DatabasePrefix(db_name);
        EXPECT_STREQ(key_prefix.c_str(), "db|db1|");
    }
    {
        std::string key = KeyEncode::TableKey(db_name, table_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|db1|tbl1|123");
        std::string key_prefix = KeyEncode::TableKeyPrefix(db_name, table_name);
        EXPECT_STREQ(key_prefix.c_str(), "tbl|db1|tbl1|");
    }
    {
        std::string key = KeyEncode::TableColumnKey(db_id_str, table_id_str, column_name, ts);
        EXPECT_STREQ(key.c_str(), "tbl|col|3|10|col1|123");

        std::string key_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
        EXPECT_STREQ(key_prefix.c_str(), "tbl|col|3|10|");

        std::string tag_prefix = KeyEncode::TableColumnTagKey(db_id_str, table_id_str, column_id_str, "tag1");
        EXPECT_STREQ(tag_prefix.c_str(), "col|3|10|4|tag1");
    }
    {
        std::string key = KeyEncode::TableSegmentKey(db_name, table_name, segment_id, ts);
        EXPECT_STREQ(key.c_str(), "seg|db1|tbl1|2|123");
        std::string key_prefix = KeyEncode::TableSegmentKeyPrefix(db_name, table_name, segment_id);
        EXPECT_STREQ(key_prefix.c_str(), "seg|db1|tbl1|2|");
    }
    {
        std::string key = KeyEncode::TableSegmentBlockKey(db_name, table_name, segment_id, block_id, ts);
        EXPECT_STREQ(key.c_str(), "blk|db1|tbl1|2|3|123");
        std::string key_prefix = KeyEncode::TableSegmentBlockKeyPrefix(db_name, table_name, segment_id, block_id);
        EXPECT_STREQ(key_prefix.c_str(), "blk|db1|tbl1|2|3|");
    }
    {
        std::string key = KeyEncode::TableSegmentBlockColumnKey(db_name, table_name, segment_id, block_id, column_id, ts);
        EXPECT_STREQ(key.c_str(), "col|db1|tbl1|2|3|4|123");
        std::string key_prefix = KeyEncode::TableSegmentBlockColumnKeyPrefix(db_name, table_name, segment_id, block_id, column_id);
        EXPECT_STREQ(key_prefix.c_str(), "col|db1|tbl1|2|3|4|");
    }
    {
        std::string key = KeyEncode::TableIndexKey(db_name, table_name, index_name, ts);
        EXPECT_STREQ(key.c_str(), "idx|db1|tbl1|idx1|123");
        std::string key_prefix = KeyEncode::TableIndexKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx|db1|tbl1|idx1|");
    }
    {
        std::string key = KeyEncode::TableIndexSegmentKey(db_name, table_name, index_name, segment_id, ts);
        EXPECT_STREQ(key.c_str(), "idx_seg|db1|tbl1|idx1|2|123");
        std::string key_prefix = KeyEncode::TableIndexSegmentKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_seg|db1|tbl1|idx1|");
    }
    {
        std::string key = KeyEncode::TableIndexChunkKey(db_name, table_name, index_name, segment_id, chunk_id, ts);
        EXPECT_STREQ(key.c_str(), "idx_chunk|db1|tbl1|idx1|2|5|123");
        std::string key_prefix = KeyEncode::TableIndexChunkKeyPrefix(db_name, table_name, index_name);
        EXPECT_STREQ(key_prefix.c_str(), "idx_chunk|db1|tbl1|idx1|");
    }
}

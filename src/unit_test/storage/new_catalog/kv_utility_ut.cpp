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

#include "gtest/gtest.h"

module infinity_core:ut.kv_utility;

import :base_test1;
import :kv_utility;
import :kv_code;
import :stl;

using namespace infinity;

class KVUtilityTest : public BaseTest {};

TEST_F(KVUtilityTest, test1) {
    // Test code here
    String db_name = "db1";
    String db_id = "3";
    String table_name = "tbl1";
    String table_id = "10";
    String column_name = "col1";
    TxnTimeStamp ts1 = 123;
    TxnTimeStamp ts2 = 0;
    {
        String key = KeyEncode::CatalogDbKey(db_name, ts1);
        EXPECT_STREQ(key.c_str(), "catalog|db|db1|123");
        u64 ts = GetTimestampFromKey(key);
        EXPECT_EQ(ts, 123);
    }
    {
        String key = KeyEncode::CatalogTableKey(db_id, table_name, ts2);
        EXPECT_STREQ(key.c_str(), "catalog|tbl|3|tbl1|0");
        u64 ts = GetTimestampFromKey(key);
        EXPECT_EQ(ts, 0);
    }
}

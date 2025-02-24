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
import kv_store;
import status;

// import segment_meta;
// import block_meta;
// import column_meta;
import table_meeta;

using namespace infinity;

class TableMeetaTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TableMeetaTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TableMeetaTest, table_meeta) {
    using namespace infinity;

    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    kv_store->Init("/tmp/rocksdb_transaction_example");
    UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();
    TableMeeta table_meta("0", *kv_instance1);
    table_meta.InitSet();

    auto [segment_id, table_status] = table_meta.GetLatestSegmentID();
    EXPECT_TRUE(table_status.ok());
    EXPECT_EQ(segment_id, 0);

    Status status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());
    status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
    EXPECT_TRUE(status.ok());
}
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
import infinity_context;
import request_test;
import query_context;
import query_result;

using namespace infinity;

class TestDDLRequest : public NewRequestTest {
protected:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestDDLRequest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestDDLRequest, test_create_table) {
    String create_table_t1 = "create table t1(c1 int, c2 varchar)";
    {
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_t1);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    { // create table with same name
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_t1);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

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

import data_table;
import data_block;
import column_vector;
import value;

using namespace infinity;

class TestIndexRequest : public NewRequestTest {
protected:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestIndexRequest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestIndexRequest, index_scan) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String create_index_sql = "create index idx1 on t1(c1)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String search_req_sql = "select * from t1 where c1 > 1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValue(0), Value::MakeInt(2));
        }
        {
            SharedPtr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValue(0), Value::MakeVarchar("def"));
        }
    }
}

TEST_P(TestIndexRequest, fulltext_index_scan) {
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String create_index_sql = "create index idx2 on t1(c2) using fulltext";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String search_req_sql = "select * from t1 search match text ('c2', 'def', 'topn=1')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValue(0), Value::MakeInt(2));
        }
        {
            SharedPtr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValue(0), Value::MakeVarchar("def"));
        }
    }
}

TEST_P(TestIndexRequest, vector_index_scan) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(float, 4))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String append_req_sql = "insert into t1 values(1, [0.1, 0.1, 0.1, 0.1]), (2, [0.2, 0.2, 0.2, 0.2])";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    auto search_vec = [this] {
        String search_req_sql = "select c1 from t1 search match vector (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);
        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValue(0), Value::MakeInt(2));
        }
    };
    search_vec();
    {
        String create_index_sql = "create index idx2 on t1(c2) using hnsw with (M = 16, ef_construction = 200, metric = l2)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    search_vec();
}

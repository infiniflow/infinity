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
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
import request_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.index_request;

import :ut.base_test;
import :ut.request_test;

import :infinity_context;
import :query_context;
import :query_result;
import :data_table;
import :data_block;
import :column_vector;
import :value;
import :logger;
#endif

using namespace infinity;

class TestIndexRequest : public NewRequestTest {
protected:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestIndexRequest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestIndexRequest, index_scan) {
    {
        std::string create_table_sql = "create table t1(c1 int, c2 varchar)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string create_index_sql = "create index idx1 on t1(c1)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string search_req_sql = "select * from t1 where c1 > 1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
        {
            std::shared_ptr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValueByIndex(0), Value::MakeVarchar("def"));
        }
    }
}

TEST_P(TestIndexRequest, fulltext_index_scan) {
    {
        std::string create_table_sql = "create table t1(c1 int, c2 varchar)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string create_index_sql = "create index idx2 on t1(c2) using fulltext";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string search_req_sql = "select * from t1 search match text ('c2', 'def', 'topn=1')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];
        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
        {
            std::shared_ptr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValueByIndex(0), Value::MakeVarchar("def"));
        }
    }
}

TEST_P(TestIndexRequest, vector_index_scan) {
    {
        std::string create_table_sql = "create table t1(c1 int, c2 embedding(float, 4))";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into t1 values(1, [0.1, 0.1, 0.1, 0.1]), (2, [0.2, 0.2, 0.2, 0.2])";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    auto search_vec = [this](const std::string &index_name = "") {
        std::string search_req_sql;
        if (index_name.empty()) {
            search_req_sql = "select c1 from t1 search match vector (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)";
        } else {
            search_req_sql = "select c1 from t1 search match vector (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1) using index (" + index_name + ")";
        }
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);
        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
    };
    search_vec();
    {
        std::string create_index_sql = "create index idx1 on t1(c2) using hnsw with (M=16, ef_construction=200, metric=l2)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    search_vec("");
    search_vec("idx1");
    {
        std::string create_index_sql = "create index idx2 on t1(c2) using ivf with (metric=l2)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    search_vec("idx2");
}

TEST_P(TestIndexRequest, sparse_index_scan) {
    {
        std::string create_table_sql = "create table t1(c1 int, c2 sparse(float, 100))";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into t1 values(1, [10:1.0, 20:1.0, 30:1.0]), (2, [40:1.0,50:1.0,60:1.0  ])";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    auto search_vec = [this] {
        std::string search_req_sql = "select c1 from t1 search match sparse (c2, [20:1.0,30:1.0,40:1.0], 'ip', 1)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);
        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(1));
        }
    };
    search_vec();
    {
        std::string create_index_sql = "create index idx2 on t1(c2) using bmp with (block_size=8, compress_type=compress)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    search_vec();
}

TEST_P(TestIndexRequest, tensor_index_scan) {
    auto search_tensor = [this] {
        std::string search_req_sql = "select c1 from t1 search match tensor(c2, [0.4,0.5,0.6,0.7], 'float', 'maxsim', '')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);
        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
    };
    auto test = [&](bool use_index) {
        {
            std::string create_table_sql = "create table t1(c1 int, c2 tensor(float, 4))";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
        if (use_index) {
            std::string create_index_sql = "create index idx1 on t1(c2) using emvb with(pq_subspace_num=4, pq_subspace_bits=8)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_index_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
        {
            std::string append_req_sql = "insert into t1 values(1, [0.0,1.0,2.0,3.0]), (2, [0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0])";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
        search_tensor();
        {
            std::string create_table_sql = "drop table t1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };
    test(false);
    test(true);
}

TEST_P(TestIndexRequest, test_optimize_index) {
    {
        std::string create_table_sql = "create table t1(c1 int, c2 embedding(float, 4))";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string create_index_sql = "create index idx1 on t1(c2) using hnsw with (M=16, ef_construction=200, metric=l2,encode=lvq)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string append_req_sql = "insert into t1 values(1, [0.1, 0.1, 0.1, 0.1]), (2, [0.2, 0.2, 0.2, 0.2])";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string optimize_index_sql = "optimize idx1 on t1 with(lvq_avg)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(optimize_index_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        std::string search_req_sql = "select c1 from t1 search match vector (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(search_req_sql);
        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        std::shared_ptr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            std::shared_ptr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
    }
}

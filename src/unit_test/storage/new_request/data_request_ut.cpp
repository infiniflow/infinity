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
import request_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.data_request;

import :ut.base_test;
import :ut.request_test;
import :stl;
import :infinity_context;
import :query_context;
import :query_result;
import :data_table;
import :data_block;
import :column_vector;
import :value;
import :third_party;
#endif

using namespace infinity;

class TestDataRequest : public NewRequestTest {
protected:
    void PrepareCreateTable() {
        {
            String create_table_sql = "create table t1(c1 int, c2 varchar)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestDataRequest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestDataRequest, test_append) {
    PrepareCreateTable();
    {
        String append_req_sql = "insert into t1 values(1, 'abc')";

        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String append_err_req_sql = "insert into t2 values(1, 'abc')";

        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_err_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_FALSE(ok);
    }
}

TEST_P(TestDataRequest, test_select) {
    PrepareCreateTable();
    {
        String append_req_sql = "insert into t1 values(1, 'abc')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(1));
        }
        {
            SharedPtr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValueByIndex(0), Value::MakeVarchar("abc"));
        }
    }
}

TEST_P(TestDataRequest, test_delete) {
    PrepareCreateTable();
    {
        String append_req_sql = "insert into t1 values(1, 'abc'),(2, 'def')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String delete_req_sql = "delete from t1 where c1 = 1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(delete_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
        }
        {
            SharedPtr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValueByIndex(0), Value::MakeVarchar("def"));
        }
    }
}

TEST_P(TestDataRequest, test_update) {
    PrepareCreateTable();
    {
        String append_req_sql = "insert into t1 values(1, 'abc'),(2, 'def')";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(append_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String update_req_sql = "update t1 set c2 = 'xyz' where c1 = 1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(update_req_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        EXPECT_EQ(result_table->data_blocks_.size(), 1);
        SharedPtr<DataBlock> data_block = result_table->data_blocks_[0];

        {
            SharedPtr<ColumnVector> col0 = data_block->column_vectors[0];
            EXPECT_EQ(col0->GetValueByIndex(0), Value::MakeInt(2));
            EXPECT_EQ(col0->GetValueByIndex(1), Value::MakeInt(1));
        }
        {
            SharedPtr<ColumnVector> col1 = data_block->column_vectors[1];
            EXPECT_EQ(col1->GetValueByIndex(0), Value::MakeVarchar("def"));
            EXPECT_EQ(col1->GetValueByIndex(1), Value::MakeVarchar("xyz"));
        }
    }
}

TEST_P(TestDataRequest, test_import_csv) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,3))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/csv/embedding_int_dim3.csv' with(delimiter ',', format csv)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(3));
    }
}

TEST_P(TestDataRequest, test_import_json) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,3))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/json/pysdk_test.json' with(format json)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);
        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(3));
    }
}

TEST_P(TestDataRequest, test_import_jsonl) {
    {
        String create_table_sql = "create table t1(name varchar, age int, embedding_column embedding(int,5))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/jsonl/test_jsonl.jsonl' with(format jsonl)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(14));
    }
}

TEST_P(TestDataRequest, test_import_fvecs) {
    {
        String create_table_sql = "create table t1(c1 embedding(float,128))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/fvecs/test.fvecs' with(format fvecs)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(1000));
    }
}

TEST_P(TestDataRequest, test_import_csr) {
    {
        String create_table_sql = "create table t1(c1 sparse(float,30000))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/csr/test.csr' with(format csr)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(10000));
    }
}

TEST_P(TestDataRequest, test_import_bvecs) {
    {
        String create_table_sql = "create table t1(c1 embedding(unsigned tinyint,128))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/bvecs/test.bvecs' with(format bvecs)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(1000));
    }
}

TEST_P(TestDataRequest, test_import_parquet) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,128))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/parquet/gen_embedding.parquet' with(format parquet)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String select_req_sql = "select count(*) from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_req_sql);

        DataTable *result_table = nullptr;
        bool ok = HandleQueryResult(query_result, &result_table);
        EXPECT_TRUE(ok);

        Value row_count = result_table->data_blocks_[0]->GetValue(0, 0);
        EXPECT_EQ(row_count, Value::MakeBigInt(10));
    }
}

TEST_P(TestDataRequest, test_export_csv) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,3))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/csv/embedding_int_dim3.csv' with(delimiter ',', format csv)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String export_path = String(GetFullTmpDir()) + "/export_csv_test.csv";
        String import_table_sql = fmt::format("copy t1 to '{}' with(delimiter ',', format csv)", export_path);
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
}

TEST_P(TestDataRequest, test_export_jsonl) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,3))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/csv/embedding_int_dim3.csv' with(delimiter ',', format csv)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String export_path = String(GetFullTmpDir()) + "/export_csv_test.jsonl";
        String import_table_sql = fmt::format("copy t1 to '{}' with(delimiter ',', format jsonl)", export_path);
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
}

TEST_P(TestDataRequest, test_export_parquet) {
    {
        String create_table_sql = "create table t1(c1 int, c2 embedding(int,128))";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String import_table_sql = "copy t1 from 'test/data/parquet/gen_embedding.parquet' with(format parquet)";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
    {
        String export_path = String(GetFullTmpDir()) + "/export_csv_test.parquet";
        String import_table_sql = fmt::format("copy t1 to '{}' with(format parquet)", export_path);
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(import_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
}

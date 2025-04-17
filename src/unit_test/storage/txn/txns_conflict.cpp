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

import stl;
import base_test;
import status;
import new_txn_manager;
import infinity_context;
import new_txn;
import txn_state;
import extra_ddl_info;
import table_def;
import data_block;
import column_vector;
import query_result;
import query_context;
import request_test;
import data_table;
import value;

using namespace infinity;

class TestTxnsConflictTest : public NewRequestTest {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TestTxnsConflictTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TestTxnsConflictTest, delete_create_index_append) {
    struct sync_struct {
        std::mutex mtx;
        std::condition_variable cv;
        bool ready{false};
    } sync_1, sync_2;

    auto thread_create_index = [this, &sync_1, &sync_2]() mutable {
        SharedPtr<String> db_name = std::make_shared<String>("db1");

        for (int i = 0; i <= 1; i++) {
            {
                String create_db_sql = "create database db1";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(create_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " create db" << ok << std::endl;
            }

            {
                String use_db_sql = "use db1";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(use_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " use db1" << ok << std::endl;
            }

            {
                String create_table_sql = "create table t1(c1 int, c2 varchar)";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(create_table_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " create table" << ok << std::endl;
            }

            {
                String append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(append_req_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " insert into t1" << ok << std::endl;
            }

            {
                std::lock_guard<std::mutex> lock(sync_1.mtx);
                sync_1.ready = true;
                sync_1.cv.notify_one();
            }

            std::cout << " 111111111111111" << std::endl;

            {
                String create_index_sql = "create index idx1 on t1(c1)";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(create_index_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " create index" << ok << std::endl;
            }

            {
                String use_db_sql = "use default_db";
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(use_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " use default_db" << ok << std::endl;
            }

            {
                std::unique_lock<std::mutex> lock(sync_2.mtx);
                sync_2.cv.wait(lock, [&sync_2] { return sync_2.ready; });
                sync_2.ready = false;
            }

            String drop_db_sql = "drop database db1";
            {
                UniquePtr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(drop_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " drop db db1" << ok << std::endl;
            }
        }
    };

    auto thread_append = [this, &sync_1, &sync_2]() {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        for (int i = 0; i <= 1; i++) {

            {
                std::unique_lock<std::mutex> lock(sync_1.mtx);
                sync_1.cv.wait(lock, [&sync_1] { return sync_1.ready; });
                sync_1.ready = false;
            }

            std::cout << " 2222222222222222" << std::endl;

            {
                String use_db_sql = "use db1";
                UniquePtr<QueryContext> query_context = MakeQueryContext2();
                QueryResult query_result = query_context->Query(use_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " use db1" << ok << std::endl;
            }

            {
                String append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
                UniquePtr<QueryContext> query_context = MakeQueryContext2();
                QueryResult query_result = query_context->Query(append_req_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " thread 2 insert into t1" << ok << std::endl;
            }

            {
                String use_db_sql = "use default_db";
                UniquePtr<QueryContext> query_context = MakeQueryContext2();
                QueryResult query_result = query_context->Query(use_db_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
                std::cout << " use default_db" << ok << std::endl;
            }

            {
                std::lock_guard<std::mutex> lock(sync_2.mtx);
                sync_2.ready = true;
                sync_2.cv.notify_one();
            }
        }
    };

    Thread worker(thread_create_index);
    Thread waiter(thread_append);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }
}

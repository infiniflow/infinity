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

module infinity_core:ut.txns_conflict;

import :ut.base_test;
import :ut.request_test;
import :stl;
import :status;
import :new_txn_manager;
import :infinity_context;
import :new_txn;
import :txn_state;
import :table_def;
import :data_block;
import :column_vector;
import :query_result;
import :query_context;
import :data_table;
import :value;
import :logger;
import :third_party;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :db_meeta;
import :new_catalog;
#endif

import extra_ddl_info;

using namespace infinity;

class TestTxnsConflictTest : public NewRequestTest {

public:
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool ready_{false};

    SizeT GetRowCount() {
        NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta("default_db", "t1", db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>{0});
        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        BlockMeta block_meta((*block_ids_ptr)[0], segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
        return state.block_offset_end();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnsConflictTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnsConflictTest, create_index_append) {
    LOG_INFO("--create_index_append--");

    /*
    auto thread_create_index = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String create_index_sql = "create index idx1 on t1(c1)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_index_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_append = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
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

    SizeT row_count = GetRowCount();
    EXPECT_EQ(row_count, 4);

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
    */
}

/* FIXME: We are not retrying query if it is conflicted with others right now.
 * In this test, we expect query is retried and successful after conflicts.
TEST_P(TestTxnsConflictTest, add_column_append) {
    LOG_INFO("--add_column_append--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "alter table t1 add column (c3 varchar default 'default')";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_append = [this](bool &ok) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool append_ok = false;
    Thread worker(thread_add_column);
    Thread waiter(thread_append, std::ref(append_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    SizeT row_count = GetRowCount();
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("append ok {}", append_ok));
    EXPECT_TRUE((append_ok && row_count == 4) || (!append_ok && row_count == 2));

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, add_column_import) {
    LOG_INFO("--add_column_import--");

    auto thread_add_column = [this]() {
        {
            String create_table_sql = "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                                      "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "alter table t1 add column (c6 varchar default 'default')";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_import = [this](bool &ok) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String import_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                "',', FORMAT CSV );";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(import_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool import_ok = false;
    Thread worker(thread_add_column);
    Thread waiter(thread_import, std::ref(import_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    SizeT row_count = 0;
    if (import_ok) {
        row_count = GetRowCount();
        LOG_INFO(fmt::format("row_count {}", row_count));
        LOG_INFO(fmt::format("import ok {}", import_ok));
    }
    EXPECT_TRUE((import_ok && row_count == 10) || (!import_ok && row_count == 0));

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, add_column_compact) {
    LOG_INFO("--add_column_compact--");

    auto thread_add_column = [this]() {
        {
            String create_table_sql = "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                                      "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            String append_req_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                    "',', FORMAT CSV );";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "alter table t1 add column (c6 varchar default 'default')";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_compact = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String compact_sql = "compact table t1;";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(compact_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    Thread worker(thread_add_column);
    Thread waiter(thread_compact);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, add_column_add_column) {
    LOG_INFO("--add_column_add_column--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "alter table t1 add column (c3 varchar default 'default')";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_add_column2 = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String add_column_sql = "alter table t1 add column (c4 varchar default 'default')";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    Thread worker(thread_add_column);
    Thread waiter(thread_add_column2);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, drop_column_append) {
    LOG_INFO("--drop_column_append--");

    auto thread_drop_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String drop_column_sql = "alter table t1 drop column (c2)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(drop_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_append = [this](bool &ok) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool append_ok = false;
    Thread worker(thread_drop_column);
    Thread waiter(thread_append, std::ref(append_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    SizeT row_count = GetRowCount();
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("append ok {}", append_ok));
    EXPECT_TRUE((append_ok && row_count == 4) || (!append_ok && row_count == 2));

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, drop_column_import) {
    LOG_INFO("--drop_column_import--");

    auto thread_drop_column = [this]() {
        {
            String create_table_sql = "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                                      "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String drop_column_sql = "alter table t1 drop column (c2)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(drop_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_import = [this](bool &ok) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String import_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                "',', FORMAT CSV );";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(import_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool import_ok = false;
    Thread worker(thread_drop_column);
    Thread waiter(thread_import, std::ref(import_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    SizeT row_count = import_ok ? GetRowCount() : 0;
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("import ok {}", import_ok));
    EXPECT_TRUE((import_ok && row_count == 10) || (!import_ok && row_count == 0));

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, drop_column_compact) {
    LOG_INFO("--drop_column_compact--");

    auto thread_drop_column = [this]() {
        {
            String create_table_sql = "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                                      "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            String append_req_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                    "',', FORMAT CSV );";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "alter table t1 drop column (c2)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_compact = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String compact_sql = "compact table t1;";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(compact_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    Thread worker(thread_drop_column);
    Thread waiter(thread_compact);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, drop_column_create_index) {
    LOG_INFO("--drop_column_create_index--");

    auto thread_drop_column = [this](bool &ok) {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String drop_column_sql = "alter table t1 drop column (c1)";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(drop_column_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    auto thread_create_index = [this](bool &ok) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String create_index_sql = "create index idx1 on t1(c1)";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(create_index_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool drop_column_pass = false;
    bool create_index_pass = false;
    Thread worker(thread_drop_column, std::ref(drop_column_pass));
    Thread waiter(thread_create_index, std::ref(create_index_pass));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    EXPECT_TRUE(drop_column_pass || create_index_pass);

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestTxnsConflictTest, delete_append) {
    LOG_INFO("--delete_append--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String add_column_sql = "delete from t1 where c1 = 1";
            UniquePtr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    auto thread_append = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String append_req_sql = "insert into t1 values(1, 'hij'), (3, 'klm')";
            UniquePtr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    Thread worker(thread_add_column);
    Thread waiter(thread_append);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from t1";
        UniquePtr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}
*/

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

module infinity_core:ut.txns_conflict;

import :ut.base_test;
import :ut.request_test;

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
import third_party;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :db_meeta;
import :new_catalog;

import extra_ddl_info;

using namespace infinity;

class TestConflictRequestTest : public NewRequestTest {

public:
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool ready_{false};

    size_t GetRowCount() {
        auto *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        auto begin_ts = txn->BeginTS();
        auto commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta("default_db", "t1", db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>{0});
        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
        BlockMeta block_meta((*block_ids_ptr)[0], segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            std::pair<BlockOffset, BlockOffset> range;
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
                         TestConflictRequestTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestConflictRequestTest, create_index_append) {
    LOG_INFO("--create_index_append--");

    auto thread_create_index = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            std::string create_index_sql = "create index idx1 on t1(c1)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    std::thread worker(thread_create_index);
    std::thread waiter(thread_append);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    size_t row_count = GetRowCount();
    EXPECT_EQ(row_count, 4);

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, add_column_append) {
    LOG_INFO("--add_column_append--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            std::string add_column_sql = "alter table t1 add column (c3 varchar default 'default')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool append_ok = false;
    std::thread worker(thread_add_column);
    std::thread waiter(thread_append, std::ref(append_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    size_t row_count = GetRowCount();
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("append ok {}", append_ok));
    EXPECT_TRUE((append_ok && row_count == 4) || (!append_ok && row_count == 2));

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, add_column_import) {
    LOG_INFO("--add_column_import--");

    auto thread_add_column = [this]() {
        {
            std::string create_table_sql =
                "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string add_column_sql = "alter table t1 add column (c6 varchar default 'default')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string import_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                     "',', FORMAT CSV );";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(import_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool import_ok = false;
    std::thread worker(thread_add_column);
    std::thread waiter(thread_import, std::ref(import_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    size_t row_count = 0;
    if (import_ok) {
        row_count = GetRowCount();
        LOG_INFO(fmt::format("row_count {}", row_count));
        LOG_INFO(fmt::format("import ok {}", import_ok));
    }
    EXPECT_TRUE((import_ok && row_count == 10) || (!import_ok && row_count == 0));

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, add_column_compact) {
    LOG_INFO("--add_column_compact--");

    auto thread_add_column = [this]() {
        {
            std::string create_table_sql =
                "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::string append_req_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                         "',', FORMAT CSV );";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string add_column_sql = "alter table t1 add column (c6 varchar default 'default')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string compact_sql = "compact table t1;";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(compact_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    std::thread worker(thread_add_column);
    std::thread waiter(thread_compact);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, add_column_add_column) {
    LOG_INFO("--add_column_add_column--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            std::string add_column_sql = "alter table t1 add column (c3 varchar default 'default')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string add_column_sql = "alter table t1 add column (c4 varchar default 'default')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(add_column_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    std::thread worker(thread_add_column);
    std::thread waiter(thread_add_column2);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, drop_column_append) {
    LOG_INFO("--drop_column_append--");

    auto thread_drop_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            std::string drop_column_sql = "alter table t1 drop column (c2)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string append_req_sql = "insert into t1 values(3, 'abc'), (4, 'def')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool append_ok = false;
    std::thread worker(thread_drop_column);
    std::thread waiter(thread_append, std::ref(append_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    size_t row_count = GetRowCount();
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("append ok {}", append_ok));
    EXPECT_TRUE((append_ok && row_count == 4) || (!append_ok && row_count == 2));

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, drop_column_import) {
    LOG_INFO("--drop_column_import--");

    auto thread_drop_column = [this]() {
        {
            std::string create_table_sql =
                "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string drop_column_sql = "alter table t1 drop column (c2)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string import_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                     "',', FORMAT CSV );";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(import_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool import_ok = false;
    std::thread worker(thread_drop_column);
    std::thread waiter(thread_import, std::ref(import_ok));

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    size_t row_count = import_ok ? GetRowCount() : 0;
    LOG_INFO(fmt::format("row_count {}", row_count));
    LOG_INFO(fmt::format("import ok {}", import_ok));
    EXPECT_TRUE((import_ok && row_count == 10) || (!import_ok && row_count == 0));

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

TEST_P(TestConflictRequestTest, drop_column_compact) {
    LOG_INFO("--drop_column_compact--");

    auto thread_drop_column = [this]() {
        {
            std::string create_table_sql =
                "CREATE TABLE t1 (c1 integer default 1, c2 integer default 4, c3 embedding(float, 3) default [1,2,3], c4 "
                "TensorArray(float, 3) default [[[1,2,3],[5,7,8]],[[9,9,9]]], c5 embedding(int, 3) default [1.3, 4.1, 33.7])";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::string append_req_sql = "COPY t1 FROM 'test/data/csv/pysdk_test_import_default.csv' WITH ( DELIMITER "
                                         "',', FORMAT CSV );";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string add_column_sql = "alter table t1 drop column (c2)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string compact_sql = "compact table t1;";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(compact_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    std::thread worker(thread_drop_column);
    std::thread waiter(thread_compact);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

#if 0

TEST_P(TestConflictRequestTest, drop_column_create_index) {
    LOG_INFO("--drop_column_create_index--");

    auto thread_drop_column = [this](bool &ok) {
        {
            std::string create_table_sql = "create table t1(c1 int, c2 varchar)";
            auto query_context = MakeQueryContext();
            auto query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::string append_req_sql = "insert into t1 values(1, 'abc'), (2, 'def')";
            auto query_context = MakeQueryContext();
            auto query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard lock(mtx_);
            cv_.notify_one();
        }
        ready_ = true;

        {
            std::string drop_column_sql = "alter table t1 drop column (c1)";
            auto query_context = MakeQueryContext();
            auto query_result = query_context->Query(drop_column_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    auto thread_create_index = [this](bool &ok) {
        {
            std::unique_lock lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            std::string create_index_sql = "create index idx1 on t1(c1)";
            auto query_context = MakeQueryContext2();
            auto query_result = query_context->Query(create_index_sql);
            ok = HandleQueryResult(query_result);
        }
    };

    bool drop_column_pass = false;
    bool create_index_pass = false;
    std::thread worker{thread_drop_column, std::ref(drop_column_pass)};
    std::thread waiter{thread_create_index, std::ref(create_index_pass)};

    worker.join();
    waiter.join();
    EXPECT_TRUE(drop_column_pass || create_index_pass);
    if ([[maybe_unused]] bool no_conflict = drop_column_pass || create_index_pass) {
        std::string select_sql = "select * from t1";
        auto query_context = MakeQueryContext();
        auto query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    } else {
        std::string select_sql = "select * from t1";
        auto query_context = MakeQueryContext();
        auto query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

#endif

TEST_P(TestConflictRequestTest, delete_append) {
    LOG_INFO("--delete_append--");

    auto thread_add_column = [this]() {
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
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            std::string add_column_sql = "delete from t1 where c1 = 1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
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
            std::string append_req_sql = "insert into t1 values(1, 'hij'), (3, 'klm')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(append_req_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
    };

    std::thread worker(thread_add_column);
    std::thread waiter(thread_append);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        std::string select_sql = "select * from t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO(query_result.ToString());
    }
}

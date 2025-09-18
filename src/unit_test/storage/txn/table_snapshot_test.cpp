

#include "unit_test/gtest_expand.h"
import base_test;

import infinity_context;
import stl;
import status;
import txn_state;
import new_txn_manager;
import new_txn;
import snapshot_info;
import data_type;
import logical_type;
import column_def;
import table_def;
import extra_ddl_info;
import data_block;
import column_vector;
import value;
import config;
import persistence_manager;
import logger;
import query_result;
import query_context;
import request_test;

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace infinity;

class TableSnapshotTest : public NewRequestTest {
public:
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool ready_{false};

    std::shared_ptr<TableSnapshotInfo> table_snapshot_;

    void SetUp() override {
        NewRequestTest::SetUp();
        SetupTestTable();
    }

    void SetupTestTable() {
        using namespace infinity;
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        // Create a table to snapshot
        NewTxn *create_table_txn = txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kCreateTable);

        Vector<std::shared_ptr<ColumnDef>> columns;
        {
            std::shared_ptr<DataType> col_type = std::make_shared<DataType>(LogicalType::kInteger);
            std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(0, col_type, "col1", std::set<ConstraintType>());
            columns.emplace_back(col_def);
        }

        std::shared_ptr<TableDef> table_def = std::make_shared<TableDef>(std::make_shared<String>("default_db"),
                                                                         std::make_shared<String>("test_table"),
                                                                         std::make_shared<String>(""),
                                                                         columns);

        Status status = create_table_txn->CreateTable("default_db", table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(create_table_txn);

        // Insert data into the table
        NewTxn *insert_txn = txn_mgr->BeginTxn(std::make_unique<String>("insert data"), TransactionType::kAppend);

        Vector<std::shared_ptr<ColumnVector>> column_vectors;
        {
            auto column_vector = std::make_shared<ColumnVector>(table_def->columns()[0]->type());
            column_vector->Initialize();

            for (size_t i = 1; i <= 3; i++) {
                column_vector->AppendValue(Value::MakeInt(i));
            }
            column_vectors.push_back(column_vector);
        }

        auto data_block = DataBlock::Make();
        data_block->Init(column_vectors);

        status = insert_txn->Append("default_db", "test_table", data_block);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(insert_txn);

        // Create snapshot using the correct method
        NewTxn *snapshot_txn = txn_mgr->BeginTxn(std::make_unique<String>("create snapshot"), TransactionType::kCreateTableSnapshot);
        status = snapshot_txn->CreateTableSnapshot("default_db", "test_table", "test_snapshot");
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(snapshot_txn);

        // Drop the original table
        NewTxn *drop_table_txn = txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kDropTable);
        status = drop_table_txn->DropTable("default_db", "test_table", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(drop_table_txn);
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TableSnapshotTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TableSnapshotTest, test_restore_table_rollback_basic) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Test 1: Successful restore
    NewTxn *restore_txn = txn_mgr->BeginTxn(std::make_unique<String>("restore table"), TransactionType::kRestoreTable);

    // Deserialize the snapshot info
    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    std::shared_ptr<TableSnapshotInfo> table_snapshot;
    Status status;
    std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "test_snapshot");
    EXPECT_TRUE(status.ok());

    // Attempt to restore table
    status = restore_txn->RestoreTableSnapshot("default_db", table_snapshot);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(restore_txn);

    // Verify that the table was restored with data
    NewTxn *check_txn1 = txn_mgr->BeginTxn(std::make_unique<String>("check table"), TransactionType::kRead);
    auto [table_info1, check_status1] = check_txn1->GetTableInfo("default_db", "test_table");
    EXPECT_TRUE(check_status1.ok()); // Table should exist after restore
    txn_mgr->CommitTxn(check_txn1);

    NewTxn *drop_table_txn1 = txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kDropTable);
    status = drop_table_txn1->DropTable("default_db", "test_table", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(drop_table_txn1);

    // Test 2: Rollback restore
    NewTxn *restore_txn1 = txn_mgr->BeginTxn(std::make_unique<String>("restore table"), TransactionType::kRestoreTable);

    // Deserialize the snapshot info again
    std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "test_snapshot");
    EXPECT_TRUE(status.ok());

    // Attempt to restore table
    status = restore_txn1->RestoreTableSnapshot("default_db", table_snapshot);
    EXPECT_TRUE(status.ok());

    // Now rollback the transaction
    status = restore_txn1->Rollback();
    EXPECT_TRUE(status.ok());
    // DO NOT call CommitTxn here - the transaction is already rolled back

    // Verify that the table was not actually created (rollback worked)
    NewTxn *check_txn = txn_mgr->BeginTxn(std::make_unique<String>("check table"), TransactionType::kRead);
    auto [table_info, check_status] = check_txn->GetTableInfo("default_db", "test_table");
    EXPECT_FALSE(check_status.ok()); // Table should not exist after rollback
    txn_mgr->CommitTxn(check_txn);
}

TEST_P(TableSnapshotTest, test_restore_table_create_table_multithreaded) {
    LOG_INFO("--test_restore_table_create_table_multithreaded--");

    auto thread_restore_table = [this]() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String restore_sql = "restore table test_table from snapshot test_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("Restore table succeeded");
            } else {
                LOG_INFO("Restore table failed");
            }
        }
    };

    auto thread_create_table = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String create_table_sql = "create table test_table(c1 int, c2 varchar)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("Create table succeeded");
            } else {
                LOG_INFO("Create table failed");
            }
        }
    };

    std::thread worker(thread_restore_table);
    std::thread waiter(thread_create_table);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from test_table";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("Final table state: " + query_result.ToString());
        } else {
            LOG_INFO("Table does not exist after operations");
        }
    }
}

TEST_P(TableSnapshotTest, test_create_snapshot_same_name_multithreaded) {
    LOG_INFO("--test_create_snapshot_same_name_multithreaded--");

    auto thread_create_snapshot1 = [this]() {
        {
            String create_table_sql = "create table t1(c1 int, c2 varchar)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            String insert_sql = "insert into t1 values(1, 'abc'), (2, 'def'), (3, 'ghi')";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(insert_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String create_snapshot_sql = "create snapshot conflict_snapshot on table t1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_snapshot_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("Snapshot 1 creation succeeded");
            } else {
                LOG_INFO("Snapshot 1 creation failed");
            }
        }
    };

    auto thread_create_snapshot2 = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String create_snapshot_sql = "create snapshot conflict_snapshot on table t1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(create_snapshot_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("Snapshot 2 creation succeeded");
            } else {
                LOG_INFO("Snapshot 2 creation failed");
            }
        }
    };

    std::thread worker(thread_create_snapshot1);
    std::thread waiter(thread_create_snapshot2);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String list_snapshots_sql = "show snapshots";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(list_snapshots_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO("Final snapshots: " + query_result.ToString());
    }
}

TEST_P(TableSnapshotTest, test_show_snapshot_multithreaded) {
    LOG_INFO("--test_show_snapshot_multithreaded--");
    {
        String create_table_sql = "create table t1(c1 int, c2 varchar)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        String insert_sql = "insert into t1 values(1, 'abc'), (2, 'def'), (3, 'ghi')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(insert_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    auto thread_create_snapshot1 = [this]() {
        String create_snapshot_sql = "create snapshot conflict_snapshot on table t1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_snapshot_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("Snapshot 1 creation succeeded");
        } else {
            LOG_INFO("Snapshot 1 creation failed");
        }
    };

    auto thread_show_snapshot = [this]() {
        {
            String show_snapshot_sql = "show snapshots";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(show_snapshot_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
            LOG_INFO("Show snapshot: " + query_result.ToString());
        }
    };

    std::thread create_snapshot(thread_create_snapshot1);
    std::thread show_snapshot(thread_show_snapshot);

    if (create_snapshot.joinable()) {
        create_snapshot.join();
    }
    if (show_snapshot.joinable()) {
        show_snapshot.join();
    }
}

TEST_P(TableSnapshotTest, test_restore_table_same_snapshot_multithreaded) {
    LOG_INFO("--test_restore_table_same_snapshot_multithreaded--");

    auto thread_restore_snapshot1 = [this]() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            String restore_sql = "restore table snapshot test_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::thread 1: Restore table succeeded");
            } else {
                LOG_INFO("std::thread 1: Restore table failed");
            }
        }
    };

    auto thread_restore_snapshot2 = [this]() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        {
            String restore_sql = "restore table snapshot test_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::thread 2: Restore table succeeded");
            } else {
                LOG_INFO("std::thread 2: Restore table failed");
            }
        }
    };

    std::thread worker(thread_restore_snapshot1);
    std::thread waiter(thread_restore_snapshot2);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }

    {
        String select_sql = "select * from test_table";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("Final table state: " + query_result.ToString());
        } else {
            LOG_INFO("Table does not exist after operations");
        }
    }
}

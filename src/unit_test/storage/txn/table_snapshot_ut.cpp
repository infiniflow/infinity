module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.table_snapshot;

import :ut.base_test;
import :ut.request_test;
import :infinity_context;
import :status;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :snapshot_info;
import :data_block;
import :column_vector;
import :table_def;
import :value;
import :config;
import :persistence_manager;
import :logger;
import :query_result;
import :query_context;

import data_type;
import column_def;
import extra_ddl_info;
import logical_type;

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

        auto db_name = std::make_shared<std::string>("default_db");
        auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        auto table_name1 = std::make_shared<std::string>("tb1");
        auto table_name2 = std::make_shared<std::string>("tb2");
        auto table_def1 = TableDef::Make(db_name, table_name1, std::make_shared<std::string>(), {column_def1, column_def2});
        // auto table_def2 = TableDef::Make(db_name, table_name2, std::make_shared<std::string>(), {column_def1, column_def2});
        auto table_snapshot1 = std::make_shared<std::string>("tb1_snapshot");
        // auto table_snapshot2 = std::make_shared<std::string>("tb2_snapshot");

        // Create table tb1 & tb2
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        // {
        //     auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        //     auto status = txn->CreateTable(*db_name, std::move(table_def2), ConflictType::kIgnore);
        //     EXPECT_TRUE(status.ok());
        //     status = txn_mgr->CommitTxn(txn);
        //     EXPECT_TRUE(status.ok());
        // }

        // Insert data
        for (int i = 0; i < 5; ++i) {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
            auto status = txn->Append(*db_name, *table_name1, input_block);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // Create table snapshot
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
            auto status = txn->CreateTableSnapshot(*db_name, *table_name1, *table_snapshot1);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        // {
        //     auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
        //     auto status = txn->CreateTableSnapshot(*db_name, *table_name2, *table_snapshot2);
        //     EXPECT_TRUE(status.ok());
        //     txn_mgr->CommitTxn(txn);
        // }

        // Drop table
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
            auto status = txn->DropTable(*db_name, *table_name1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }
        // {
        //     auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        //     auto status = txn->DropTable(*db_name, *table_name2, ConflictType::kError);
        //     EXPECT_TRUE(status.ok());
        //     txn_mgr->CommitTxn(txn);
        // }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TableSnapshotTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TableSnapshotTest, test_restore_table_rollback_basic) {
    // using namespace infinity;
    // NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    //
    // // Test 1: Successful restore
    // NewTxn *restore_txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);
    //
    // // Deserialize the snapshot info
    // std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    // std::shared_ptr<TableSnapshotInfo> table_snapshot;
    // Status status;
    // std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "tb1_snapshot");
    // EXPECT_TRUE(status.ok());
    //
    // // Attempt to restore table
    // status = restore_txn->RestoreTableSnapshot("default_db", table_snapshot);
    // EXPECT_TRUE(status.ok());
    // txn_mgr->CommitTxn(restore_txn);
    //
    // // Verify that the table was restored with data
    // NewTxn *check_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
    // auto [table_info1, check_status1] = check_txn1->GetTableInfo("default_db", "tb1");
    // EXPECT_TRUE(check_status1.ok()); // Table should exist after restore
    // txn_mgr->CommitTxn(check_txn1);
    //
    // NewTxn *drop_table_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
    // status = drop_table_txn1->DropTable("default_db", "tb1", ConflictType::kError);
    // EXPECT_TRUE(status.ok());
    // txn_mgr->CommitTxn(drop_table_txn1);
    //
    // // Test 2: Rollback restore
    // NewTxn *restore_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);
    //
    // // Deserialize the snapshot info again
    // std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "tb1_snapshot");
    // EXPECT_TRUE(status.ok());
    //
    // // Attempt to restore table
    // status = restore_txn1->RestoreTableSnapshot("default_db", table_snapshot);
    // EXPECT_TRUE(status.ok());
    //
    // // Now rollback the transaction
    // status = restore_txn1->Rollback();
    // EXPECT_TRUE(status.ok());
    // // DO NOT call CommitTxn here - the transaction is already rolled back
    //
    // // Verify that the table was not actually created (rollback worked)
    // NewTxn *check_txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
    // auto [table_info, check_status] = check_txn->GetTableInfo("default_db", "tb1");
    // EXPECT_FALSE(check_status.ok()); // Table should not exist after rollback
    // txn_mgr->CommitTxn(check_txn);
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
            std::string restore_sql = "restore table snapshot tb1_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::Thread 1: restore table snapshot tb1_snapshot succeeded");
            } else {
                LOG_INFO("std::Thread 1: restore table snapshot tb1_snapshot failed");
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
            std::string create_table_sql = "create table tb1(col1 int, col2 varchar)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::Thread 2: create table tb1(col1 int, col2 varchar) succeeded");
            } else {
                LOG_INFO("std::Thread 2: create table tb1(col1 int, col2 varchar) failed");
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
        std::string select_sql = "select * from tb1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("Final table tb1: " + query_result.ToString());
        } else {
            LOG_INFO("select * from tb1 failed");
        }
    }
}

TEST_P(TableSnapshotTest, test_create_snapshot_same_name_multithreaded) {
    LOG_INFO("--test_create_snapshot_same_name_multithreaded--");

    auto thread_create_snapshot1 = [this]() {
        {
            std::string create_table_sql = "create table tb1(col1 int, col2 varchar)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_table_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        {
            std::string insert_sql = "insert into tb1 values(1, 'abc'), (2, 'def'), (3, 'ghi')";
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
            std::string create_snapshot_sql = "create snapshot conflict_snapshot on table tb1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_snapshot_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("create snapshot conflict_snapshot on table tb1 succeeded");
            } else {
                LOG_INFO("create snapshot conflict_snapshot on table tb1 failed");
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
            std::string create_snapshot_sql = "create snapshot conflict_snapshot on table t1";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(create_snapshot_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("create snapshot conflict_snapshot on table t1 succeeded");
            } else {
                LOG_INFO("create snapshot conflict_snapshot on table t1 failed");
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
        std::string list_snapshots_sql = "show snapshots";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(list_snapshots_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO("Final snapshots: " + query_result.ToString());
    }

    {
        std::string sql = "drop snapshot conflict_snapshot";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }
}

TEST_P(TableSnapshotTest, test_show_snapshot_multithreaded) {
    LOG_INFO("--test_show_snapshot_multithreaded--");
    {
        std::string create_table_sql = "create table tb1(col1 int, col2 varchar)";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_table_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    {
        std::string insert_sql = "insert into tb1 values(1, 'abc'), (2, 'def'), (3, 'ghi')";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(insert_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
    }

    auto thread_create_snapshot1 = [this]() {
        std::string create_snapshot_sql = "create snapshot conflict_snapshot on table tb1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(create_snapshot_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("std::Thread 1: create snapshot conflict_snapshot on table tb1 succeeded");
        } else {
            LOG_INFO("std::Thread 2: create snapshot conflict_snapshot on table tb1 failed");
        }
    };

    auto thread_show_snapshot = [this]() {
        {
            std::string show_snapshot_sql = "show snapshots";
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

    {
        std::string list_snapshots_sql = "show snapshots";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(list_snapshots_sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
        LOG_INFO("Final snapshots: " + query_result.ToString());
    }

    {
        std::string sql = "drop snapshot conflict_snapshot";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        EXPECT_TRUE(ok);
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
            std::string restore_sql = "restore table snapshot tb1_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::thread 1: restore table snapshot tb1_snapshot succeeded");
            } else {
                LOG_INFO("std::thread 1: restore table snapshot tb1_snapshot failed");
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
            std::string restore_sql = "restore table snapshot tb1_snapshot";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
            QueryResult query_result = query_context->Query(restore_sql);
            bool ok = HandleQueryResult(query_result);
            if (ok) {
                LOG_INFO("std::thread 2: restore table snapshot tb1_snapshot succeeded");
            } else {
                LOG_INFO("std::thread 2: restore table snapshot tb1_snapshot failed");
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
        std::string select_sql = "select * from tb1";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("Final table tb1: " + query_result.ToString());
        } else {
            LOG_INFO("select * from tb1 failed");
        }
    }
}

TEST_P(TableSnapshotTest, test_create_restore_snapshot_with_index) {
    // LOG_INFO("--test_create_restore_snapshot_with_index--");
    //
    // {
    //     std::string sql = "restore table snapshot test_snapshot";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("restore table snapshot test_snapshot succeeded");
    //     } else {
    //         LOG_INFO("restore table snapshot test_snapshot failed");
    //     }
    // }
    //
    // {
    //     std::string sql = "select * from test_table";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("test_table: " + query_result.ToString());
    //     } else {
    //         LOG_INFO("select * from test_table failed");
    //     }
    // }
    //
    // {
    //     std::string sql = "drop snapshot test_snapshot";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("drop snapshot test_snapshot succeeded");
    //     } else {
    //         LOG_INFO("drop snapshot test_snapshot failed");
    //     }
    // }
    //
    // // {
    // //     std::string sql = "create index idx on test_table(col1)";
    // //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    // //     QueryResult query_result = query_context->Query(sql);
    // //     bool ok = HandleQueryResult(query_result);
    // //     if (ok) {
    // //         LOG_INFO("create index idx on test_table(col1) succeeded");
    // //     } else {
    // //         LOG_INFO("create index idx on test_table(col1) failed");
    // //     }
    // // }
    //
    // // {
    // //     std::string sql = "insert into test_table values (123)";
    // //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    // //     QueryResult query_result = query_context->Query(sql);
    // //     bool ok = HandleQueryResult(query_result);
    // //     if (ok) {
    // //         LOG_INFO("insert into test_table values (123) succeeded");
    // //     } else {
    // //         LOG_INFO("insert into test_table values (123) failed");
    // //     }
    // // }
    //
    // {
    //     std::string sql = "create snapshot test_snapshot on table test_table";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("create snapshot test_snapshot on table test_table succeeded");
    //     } else {
    //         LOG_INFO("create snapshot test_snapshot on table test_table failed");
    //     }
    // }
    //
    // {
    //     std::string sql = "insert into test_table values (456)";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("insert into test_table values (456) succeeded");
    //     } else {
    //         LOG_INFO("insert into test_table values (456) failed");
    //     }
    // }
    //
    // {
    //     std::string sql = "select * from test_table";
    //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
    //     QueryResult query_result = query_context->Query(sql);
    //     bool ok = HandleQueryResult(query_result);
    //     if (ok) {
    //         LOG_INFO("test_table: " + query_result.ToString());
    //     } else {
    //         LOG_INFO("select * from test_table failed");
    //     }
    // }
}
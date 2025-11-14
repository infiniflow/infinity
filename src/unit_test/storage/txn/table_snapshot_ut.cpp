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

    std::shared_ptr<std::string> db_name;
    std::shared_ptr<ColumnDef> column_def1;
    std::shared_ptr<ColumnDef> column_def2;
    std::shared_ptr<std::string> table_name;
    std::shared_ptr<TableDef> table_def;
    std::shared_ptr<std::string> table_snapshot_name;

    std::shared_ptr<TableSnapshotInfo> table_snapshot_;

    void PrintTableRowCount() {
        std::string select_sql = fmt::format("select count(*) from {}", *table_name);
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO(fmt::format("RowCount: {}", query_result.ToString()));
        } else {
            LOG_INFO("GetTableRowCount failed");
        }
    }

    void TearDown() override {
        // std::string cmd = "rm -rf " + InfinityContext::instance().config()->SnapshotDir();
        // system(cmd.c_str());

        BaseTestParamStr::TearDown();
    }

    void SetUp() override {
        NewRequestTest::SetUp();
        SetupTestTable();
    }

    void SetupTestTable() {
        using namespace infinity;
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        db_name = std::make_shared<std::string>("default_db");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        table_snapshot_name = std::make_shared<std::string>("tb1_snapshot");

        // Create table
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // Create index
        {
            std::string create_index_sql = "create index idx1 on tb1(col1)";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_index_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }
        {
            std::string create_index_sql = "create index idx2 on tb1(col2) using fulltext";
            std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            QueryResult query_result = query_context->Query(create_index_sql);
            bool ok = HandleQueryResult(query_result);
            EXPECT_TRUE(ok);
        }

        for (IntegerT i = 1; i < 20; ++i) {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block = MakeInputBlock(Value::MakeInt(i), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1000);
            auto status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // Create table snapshot
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
            auto status = txn->CreateTableSnapshot(*db_name, *table_name, *table_snapshot_name);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        // Drop table
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
            auto status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            txn_mgr->CommitTxn(txn);
        }

        // Create Checkpoint
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS(), false);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // {
        //     auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        //     Status status = txn->Cleanup();
        //     EXPECT_TRUE(status.ok());
        //     status = txn_mgr->CommitTxn(txn);
        //     EXPECT_TRUE(status.ok());
        // }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TableSnapshotTest, ::testing::Values(BaseTestParamStr::NEW_VFS_OFF_BG_OFF_PATH));
// INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TableSnapshotTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TableSnapshotTest, test_restore_table_rollback_basic) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Test 1: Successful restore
    NewTxn *restore_txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);

    // Deserialize the snapshot info
    std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    std::shared_ptr<TableSnapshotInfo> table_snapshot;
    Status status;
    std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "tb1_snapshot");
    EXPECT_TRUE(status.ok());

    // Attempt to restore table
    status = restore_txn->RestoreTableSnapshot("default_db", table_snapshot);
    EXPECT_TRUE(status.ok());
    status = txn_mgr->CommitTxn(restore_txn);
    EXPECT_TRUE(status.ok());

    // Verify that the table was restored with data
    NewTxn *check_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
    auto [table_info1, check_status1] = check_txn1->GetTableInfo("default_db", "tb1");
    EXPECT_TRUE(check_status1.ok());
    status = txn_mgr->CommitTxn(check_txn1);
    EXPECT_TRUE(status.ok());

    NewTxn *drop_table_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
    status = drop_table_txn1->DropTable("default_db", "tb1", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    status = txn_mgr->CommitTxn(drop_table_txn1);
    EXPECT_TRUE(status.ok());

    // Test 2: Rollback restore
    NewTxn *restore_txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);

    // Deserialize the snapshot info again
    std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, "tb1_snapshot");
    EXPECT_TRUE(status.ok());

    // Attempt to restore table
    status = restore_txn1->RestoreTableSnapshot("default_db", table_snapshot);
    EXPECT_TRUE(status.ok());

    // Now rollback the transaction
    status = restore_txn1->Rollback();
    EXPECT_TRUE(status.ok());

    // Verify that the table was not actually created (rollback worked)
    NewTxn *check_txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
    auto [table_info, check_status] = check_txn->GetTableInfo("default_db", "tb1");
    EXPECT_FALSE(check_status.ok()); // Table should not exist after rollback
    status = txn_mgr->CommitTxn(check_txn);
    EXPECT_TRUE(status.ok());
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

    PrintTableRowCount();
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

    PrintTableRowCount();
}

TEST_P(TableSnapshotTest, test_create_snapshot_delete_data) {
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

    PrintTableRowCount();

    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    Status status;
    std::vector<RowID> row_ids;

    row_ids.clear();
    auto *txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
    for (size_t row_id = 1000; row_id < 3000; ++row_id) {
        row_ids.push_back(RowID(0, row_id));
    }
    status = txn1->Delete(*db_name, *table_name, row_ids);

    status = txn_mgr->CommitTxn(txn1);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    // Create Checkpoint
    {
        auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        auto status = txn->Checkpoint(wal_manager->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    PrintTableRowCount();

    auto *txn2 = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
    status = txn2->CreateTableSnapshot(*db_name, *table_name, "test_delete");
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    auto *txn3 = txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);

    row_ids.clear();
    for (size_t row_id = 4000; row_id < 5000; ++row_id) {
        row_ids.push_back(RowID(0, row_id));
    }
    status = txn3->Delete(*db_name, *table_name, row_ids);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    status = txn_mgr->CommitTxn(txn2);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    status = txn_mgr->CommitTxn(txn3);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    // Drop table
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        auto status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        if (!status.ok()) {
            LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
        }
        txn_mgr->CommitTxn(txn);
    }

    {
        std::string restore_sql = "restore table snapshot test_delete";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(restore_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("std::Thread 1: restore table snapshot test_delete succeeded");
        } else {
            LOG_INFO("std::Thread 1: restore table snapshot test_delete failed");
        }
    }

    PrintTableRowCount();
}

TEST_P(TableSnapshotTest, test_create_snapshot_insert_data) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

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

    Status status;

    auto *txn1 = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
    auto input_block = MakeInputBlock(Value::MakeInt(999), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 900);
    status = txn1->Append(*db_name, *table_name, input_block);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    auto *txn2 = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
    status = txn2->CreateTableSnapshot("default_db", "tb1", "test_insert");
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    status = txn_mgr->CommitTxn(txn2);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    status = txn_mgr->CommitTxn(txn1);
    if (!status.ok()) {
        LOG_INFO(fmt::format("Line: {} message: {}", __LINE__, status.message()));
    }

    // Drop table
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        auto status = txn->DropTable("default_db", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }

    {
        std::string restore_sql = "restore table snapshot test_insert";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(restore_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO("std::Thread 1: restore table snapshot test_insert succeeded");
        } else {
            LOG_INFO("std::Thread 1: restore table snapshot test_insert failed");
        }
    }

    PrintTableRowCount();
}
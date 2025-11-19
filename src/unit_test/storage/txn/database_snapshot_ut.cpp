module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.database_snapshot;

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

class DatabaseSnapshotTest : public NewRequestTest {
public:
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool ready_{false};
    std::vector<std::shared_ptr<std::string>> db_snapshot_names;

    void TearDown() override {
        for (auto snapshot_name : db_snapshot_names) {
            std::string cmd = fmt::format("rm -rf {}/{}", InfinityContext::instance().config()->SnapshotDir(), *snapshot_name);
            system(cmd.c_str());
            LOG_INFO(fmt::format("Exec cmd: {}", cmd));
        }
        BaseTestParamStr::TearDown();
    }

    void SetUp() override {
        NewRequestTest::SetUp();
        SetupDatabase();
    }

    void SetupDatabase() {
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        for (size_t i = 0; i < 2; i++) {
            auto db_name = std::make_shared<std::string>(fmt::format("db_{}", i));
            auto snapshot_name = std::make_shared<std::string>(fmt::format("snapshot_{}", i));
            db_snapshot_names.emplace_back(snapshot_name);

            auto table_name1 = std::make_shared<std::string>(fmt::format("db_{}_tb_1", i));
            auto table_name2 = std::make_shared<std::string>(fmt::format("db_{}_tb_2", i));
            auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
            auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
            auto table_def1 = TableDef::Make(db_name, table_name1, std::make_shared<std::string>(), {column_def1, column_def2});
            auto table_def2 = TableDef::Make(db_name, table_name2, std::make_shared<std::string>(), {column_def1, column_def2});

            // Create database
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
                Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // Create tables
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
                auto status = txn->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
                auto status = txn->CreateTable(*db_name, std::move(table_def2), ConflictType::kIgnore);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // // Create index
            // {
            //     std::string create_index_sql = "create index idx1 on tb1(col1)";
            //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            //     QueryResult query_result = query_context->Query(create_index_sql);
            //     bool ok = HandleQueryResult(query_result);
            //     EXPECT_TRUE(ok);
            // }
            // {
            //     std::string create_index_sql = "create index idx2 on tb2(col2) using fulltext";
            //     std::unique_ptr<QueryContext> query_context = MakeQueryContext();
            //     QueryResult query_result = query_context->Query(create_index_sql);
            //     bool ok = HandleQueryResult(query_result);
            //     EXPECT_TRUE(ok);
            // }

            // Insert datas
            for (size_t j = 0; j < 10; ++j) {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
                auto input_block = MakeInputBlock(Value::MakeInt(j), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 5000);
                auto status = txn->Append(*db_name, *table_name1, input_block);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
            for (size_t j = 0; j < 10; ++j) {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
                auto input_block = MakeInputBlock(Value::MakeInt(j), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 5000);
                auto status = txn->Append(*db_name, *table_name2, input_block);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // Create Snapshot
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateDBSnapshot);
                auto status = txn->CreateDBSnapshot(*db_name, *snapshot_name);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // Drop database
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop database"), TransactionType::kDropDB);
                auto status = txn->DropDatabase(*db_name, ConflictType::kError);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
        }
    }
};

// INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
//                          DatabaseSnapshotTest,
//                          ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DatabaseSnapshotTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(DatabaseSnapshotTest, test_restore_database_rollback_basic) {
    LOG_INFO("--test_restore_database_rollback_basic--");
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Test restore database
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore database"), TransactionType::kRestoreDatabase);

        std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
        std::shared_ptr<DatabaseSnapshotInfo> database_snapshot;
        Status status;
        std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(snapshot_dir, *db_snapshot_names[0]);
        EXPECT_TRUE(status.ok());

        status = txn->RestoreDatabaseSnapshot(database_snapshot);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Verify that the table was restored with data
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
        auto [table_info, status] = txn->GetTableInfo("db_0", "db_0_tb_1");
        ASSERT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop database"), TransactionType::kDropDB);
        auto status = txn->DropDatabase("db_0", ConflictType::kError);
        ASSERT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    // Test rollback
    {
        NewTxn *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);
        std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
        std::shared_ptr<DatabaseSnapshotInfo> database_snapshot;
        Status status;
        std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(snapshot_dir, *db_snapshot_names[0]);
        ASSERT_TRUE(status.ok());

        status = txn->RestoreDatabaseSnapshot(database_snapshot);
        ASSERT_TRUE(status.ok());

        status = txn->Rollback();
        ASSERT_TRUE(status.ok());
    }

    // Verify that the database was not actually created
    {
        NewTxn *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check database"), TransactionType::kRead);
        auto [database_info, status] = txn->GetDatabaseInfo("db_0");
        ASSERT_FALSE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }
}

TEST_P(DatabaseSnapshotTest, test_restore_database_create_database_multithreaded) {
    LOG_INFO("--test_restore_database_create_database_multithreaded--");

    auto thread_restore_database = [this]() {
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore database"), TransactionType::kRestoreDatabase);

            std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
            std::shared_ptr<DatabaseSnapshotInfo> database_snapshot;
            Status status;
            std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(snapshot_dir, *db_snapshot_names[0]);
            EXPECT_TRUE(status.ok());

            status = txn->RestoreDatabaseSnapshot(database_snapshot);
            if (!status.ok()) {
                LOG_INFO(fmt::format("RestoreDatabaseSnapshot failed, {}", status.message()));
                status = txn->Rollback();
                EXPECT_TRUE(status.ok());
                return;
            }

            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
                auto [table_info, status] = txn->GetTableInfo("db_0", "db_0_tb_1");
                EXPECT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
    };

    auto thread_create_database = [this]() {
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return ready_; });
            ready_ = false;
        }

        // Create database
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase("db_0", ConflictType::kError, std::make_shared<std::string>());
            ASSERT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            ASSERT_TRUE(status.ok());
        }
    };

    std::thread worker(thread_restore_database);
    std::thread waiter(thread_create_database);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }
}
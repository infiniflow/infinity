module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.system_snapshot;

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

class MySnapshotInfo {
public:
    explicit MySnapshotInfo(std::string snapshot_name) : snapshot_name_(snapshot_name) {}
    bool Insert(std::string db_name, std::string table_name) {
        if (map_.count(db_name) == 0) {
            map_[db_name] = {table_name};
            if (table_name == "") {
                map_[db_name].clear();
            }
        } else {
            auto &table_names = map_[db_name];
            table_names.push_back(table_name);
        }
        return true;
    }

    std::string snapshot_name_;
    std::unordered_map<std::string, std::vector<std::string>> map_;
};

class SystemSnapshotTest : public NewRequestTest {
public:
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool ready_{false};
    std::shared_ptr<MySnapshotInfo> snapshot_info;

    void TearDown() override {
        std::string cmd = fmt::format("rm -rf {}", InfinityContext::instance().config()->SnapshotDir());
        LOG_INFO(fmt::format("Exec cmd: {}", cmd));
        system(cmd.c_str());
        BaseTestParamStr::TearDown();
    }

    void SetUp() override {
        NewRequestTest::SetUp();
        SetupDatabase();
    }

    void SetupDatabase() {
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        std::string snapshot_name = fmt::format("system_snapshot");
        snapshot_info = std::make_shared<MySnapshotInfo>(snapshot_name);
        snapshot_info->Insert("default_db", "");

        for (size_t i = 0; i < 2; i++) {
            std::string db_name = fmt::format("db{}", i);
            std::string table_name1 = fmt::format("{}_tb1", db_name);
            std::string table_name2 = fmt::format("{}_tb2", db_name);
            auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
            auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
            auto table_def1 = TableDef::Make(std::make_shared<std::string>(db_name),
                                             std::make_shared<std::string>(table_name1),
                                             std::make_shared<std::string>(),
                                             {column_def1, column_def2});
            auto table_def2 = TableDef::Make(std::make_shared<std::string>(db_name),
                                             std::make_shared<std::string>(table_name2),
                                             std::make_shared<std::string>(),
                                             {column_def1, column_def2});

            snapshot_info->Insert(db_name, table_name1);
            snapshot_info->Insert(db_name, table_name2);

            // Create database
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
                Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // Create tables
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
                auto status = txn->CreateTable(db_name, std::move(table_def1), ConflictType::kIgnore);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
                auto status = txn->CreateTable(db_name, std::move(table_def2), ConflictType::kIgnore);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }

            // Create indexes
            {
                std::string create_index_sql = fmt::format("create index idx1 on {}.{}(col1)", db_name, table_name1);
                std::unique_ptr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(create_index_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
            }
            {
                std::string create_index_sql = fmt::format("create index idx2 on {}.{}(col2) using fulltext", db_name, table_name2);
                std::unique_ptr<QueryContext> query_context = MakeQueryContext();
                QueryResult query_result = query_context->Query(create_index_sql);
                bool ok = HandleQueryResult(query_result);
                EXPECT_TRUE(ok);
            }

            // Insert datas
            for (size_t j = 0; j < 10; ++j) {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
                auto input_block = MakeInputBlock(Value::MakeInt(j), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 5000);
                auto status = txn->Append(db_name, table_name1, input_block);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
            for (size_t j = 0; j < 10; ++j) {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
                auto input_block = MakeInputBlock(Value::MakeInt(j), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 5000);
                auto status = txn->Append(db_name, table_name2, input_block);
                ASSERT_TRUE(status.ok());
                status = txn_mgr->CommitTxn(txn);
                ASSERT_TRUE(status.ok());
            }
        }

        // Create Snapshot
        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateSystemSnapshot);
            auto status = txn->CreateSystemSnapshot(snapshot_info->snapshot_name_);
            ASSERT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            ASSERT_TRUE(status.ok());
        }

        for (const auto &[db_name, table_names] : snapshot_info->map_) {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop database"), TransactionType::kDropDB);
            auto status = txn->DropDatabase(db_name, ConflictType::kError);
            ASSERT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            ASSERT_TRUE(status.ok());
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         SystemSnapshotTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(SystemSnapshotTest, test_restore_system_rollback_basic) {
    LOG_INFO("--test_restore_system_rollback_basic--");
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    for (const auto &[db_name, table_names] : snapshot_info->map_) {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check database"), TransactionType::kRead);
        auto [table_info, status] = txn->GetDatabaseInfo(db_name);
        ASSERT_FALSE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    // Test restore system
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore system"), TransactionType::kRestoreSystem);

        std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
        std::shared_ptr<SystemSnapshotInfo> system_snapshot;
        Status status;
        std::tie(system_snapshot, status) = SystemSnapshotInfo::Deserialize(snapshot_dir, "system_snapshot");
        ASSERT_TRUE(status.ok());

        status = txn->RestoreSystemSnapshot(system_snapshot);
        ASSERT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    // Verify
    for (const auto &[db_name, table_names] : snapshot_info->map_) {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check database"), TransactionType::kRead);
        auto [table_info, status] = txn->GetDatabaseInfo(db_name);
        ASSERT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    {
        std::string sql = "show databases";
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(sql);
        bool ok = HandleQueryResult(query_result);
        ASSERT_TRUE(ok);
        LOG_INFO("After restore system snapshot: " + query_result.ToString());
    }

    {
        std::string db_name = "db0";
        std::string table_name = snapshot_info->map_[db_name][0];

        std::string select_sql = fmt::format("select count(*) from {}.{}", db_name, table_name);
        std::unique_ptr<QueryContext> query_context = MakeQueryContext();
        QueryResult query_result = query_context->Query(select_sql);
        bool ok = HandleQueryResult(query_result);
        if (ok) {
            LOG_INFO(fmt::format("database: {}, table:{}, count: {}", db_name, table_name, query_result.ToString()));
        } else {
            LOG_INFO("GetCount failed");
        }
    }

    for (const auto &[db_name, table_names] : snapshot_info->map_) {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop database"), TransactionType::kDropDB);
        auto status = txn->DropDatabase(db_name, ConflictType::kError);
        ASSERT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    // Test rollback
    {
        NewTxn *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore system"), TransactionType::kRestoreSystem);
        std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
        std::shared_ptr<SystemSnapshotInfo> system_snapshot;
        Status status;
        std::tie(system_snapshot, status) = SystemSnapshotInfo::Deserialize(snapshot_dir, snapshot_info->snapshot_name_);
        EXPECT_TRUE(status.ok());

        status = txn->RestoreSystemSnapshot(system_snapshot);
        ASSERT_TRUE(status.ok());

        status = txn->Rollback();
        ASSERT_TRUE(status.ok());
    }

    // Verify
    for (const auto &[db_name, table_names] : snapshot_info->map_) {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check database"), TransactionType::kRead);
        auto [table_info, status] = txn->GetDatabaseInfo(db_name);
        ASSERT_FALSE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }
}

TEST_P(SystemSnapshotTest, test_restore_system_create_database_multithreaded) {
    LOG_INFO("--test_restore_system_create_database_multithreaded--");

    auto thread_restore_system = [this]() {
        NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            cv_.notify_one();
        }

        {
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore system"), TransactionType::kRestoreSystem);

            std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
            std::shared_ptr<SystemSnapshotInfo> system_snapshot;
            Status status;
            std::tie(system_snapshot, status) = SystemSnapshotInfo::Deserialize(snapshot_dir, "system_snapshot");
            ASSERT_TRUE(status.ok());

            status = txn->RestoreSystemSnapshot(system_snapshot);
            if (!status.ok()) {
                LOG_INFO(fmt::format("[thread_restore_system] RestoreSystemSnapshot failed: {}", status.message()));
                status = txn->Rollback();
                ASSERT_TRUE(status.ok());
                return;
            }

            LOG_INFO("[thread_restore_system] RestoreSystemSnapshot success");
            status = txn_mgr->CommitTxn(txn);
            ASSERT_TRUE(status.ok());
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
            std::string db_name = "db0";

            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
            if (!status.ok()) {
                LOG_INFO(fmt::format("[thread_create_database] CreateDatabase failed: {}", status.message()));
                status = txn->Rollback();
                ASSERT_TRUE(status.ok());
                return;
            }

            LOG_INFO("[thread_create_database] CreateDatabase success");
            status = txn_mgr->CommitTxn(txn);
            ASSERT_TRUE(status.ok());
        }
    };

    std::thread worker(thread_restore_system);
    std::thread waiter(thread_create_database);

    if (worker.joinable()) {
        worker.join();
    }
    if (waiter.joinable()) {
        waiter.join();
    }
}

//
// TEST_P(DatabaseSnapshotTest, test_create_snapshot_same_name_multithreaded) {
//     LOG_INFO("--test_create_snapshot_same_name_multithreaded--");
//
//     auto thread_create_snapshot1 = [this]() {
//         NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
//         {
//             auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("restore database"), TransactionType::kRestoreDatabase);
//
//             std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
//             std::shared_ptr<DatabaseSnapshotInfo> database_snapshot;
//             Status status;
//             std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(snapshot_dir, *db_snapshot_names[0]);
//             EXPECT_TRUE(status.ok());
//
//             status = txn->RestoreDatabaseSnapshot(database_snapshot);
//             EXPECT_TRUE(status.ok());
//
//             status = txn_mgr->CommitTxn(txn);
//             EXPECT_TRUE(status.ok());
//
//             {
//                 auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
//                 auto [table_info, status] = txn->GetTableInfo("db_0", "db_0_tb_1");
//                 EXPECT_TRUE(status.ok());
//                 status = txn_mgr->CommitTxn(txn);
//                 EXPECT_TRUE(status.ok());
//             }
//         }
//
//         {
//             std::lock_guard<std::mutex> lock(mtx_);
//             ready_ = true;
//             cv_.notify_one();
//         }
//
//         {
//             std::string create_snapshot_sql = "create snapshot conflict_snapshot on database db_0";
//             std::unique_ptr<QueryContext> query_context = MakeQueryContext();
//             QueryResult query_result = query_context->Query(create_snapshot_sql);
//             bool ok = HandleQueryResult(query_result);
//             if (ok) {
//                 LOG_INFO("create snapshot conflict_snapshot on database db_0 succeeded");
//             } else {
//                 LOG_INFO("create snapshot conflict_snapshot on database db_0 failed");
//             }
//         }
//     };
//
//     auto thread_create_snapshot2 = [this]() {
//         {
//             std::unique_lock<std::mutex> lock(mtx_);
//             cv_.wait(lock, [this] { return ready_; });
//             ready_ = false;
//         }
//
//         {
//             std::string create_snapshot_sql = "create snapshot conflict_snapshot on database db_0";
//             std::unique_ptr<QueryContext> query_context = MakeQueryContext2();
//             QueryResult query_result = query_context->Query(create_snapshot_sql);
//             bool ok = HandleQueryResult(query_result);
//             if (ok) {
//                 LOG_INFO("create snapshot conflict_snapshot on database db_0 succeeded");
//             } else {
//                 LOG_INFO("create snapshot conflict_snapshot on database db_0 failed");
//             }
//         }
//     };
//
//     std::thread worker(thread_create_snapshot1);
//     std::thread waiter(thread_create_snapshot2);
//
//     if (worker.joinable()) {
//         worker.join();
//     }
//     if (waiter.joinable()) {
//         waiter.join();
//     }
//
//     {
//         std::string list_snapshots_sql = "show snapshots";
//         std::unique_ptr<QueryContext> query_context = MakeQueryContext();
//         QueryResult query_result = query_context->Query(list_snapshots_sql);
//         bool ok = HandleQueryResult(query_result);
//         EXPECT_TRUE(ok);
//         LOG_INFO("Final snapshots: " + query_result.ToString());
//     }
// }
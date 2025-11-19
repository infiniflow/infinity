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

class DBSnapshotTest : public NewRequestTest {
public:
    std::vector<std::shared_ptr<std::string>> db_names;
    std::vector<std::shared_ptr<std::string>> table_names;
    std::vector<std::shared_ptr<TableDef>> table_defs;
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

        for (size_t i = 0; i < 1; i++) {
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

            // Drop tables
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
                auto status = txn->DropTable(*db_name, *table_name1, ConflictType::kError);
                EXPECT_TRUE(status.ok());
                txn_mgr->CommitTxn(txn);
            }
            {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
                auto status = txn->DropTable(*db_name, *table_name2, ConflictType::kError);
                EXPECT_TRUE(status.ok());
                txn_mgr->CommitTxn(txn);
            }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DBSnapshotTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(DBSnapshotTest, test1) {}
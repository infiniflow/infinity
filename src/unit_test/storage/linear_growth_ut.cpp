module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.linear_growth;

import :ut.base_test;
import :result_cache_manager;
import :logical_node;
import :physical_operator;
import :cached_node_base;
import :column_binding;
import :logical_node_type;
import :physical_operator_type;
import :query_context;
import :operator_state;
import :data_block;
import :new_txn_manager;
import :new_txn;

import data_type;
import logical_type;

using namespace infinity;

class LinearGrowthTest : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr_ = InfinityContext::instance().storage()->new_txn_manager();

        db_name_ = std::make_shared<std::string>("db1");
        column_def1_ = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2_ = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name_ = std::make_shared<std::string>("tb1");
        table_def_ = TableDef::Make(db_name_, table_name_, std::make_shared<std::string>(), {column_def1_, column_def2_});
    }

    void TearDown() override {
        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    std::shared_ptr<DataBlock> MakeInputBlock(const Value &v1, const Value &v2, size_t row_cnt) {
        auto make_column = [&](std::shared_ptr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (size_t i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = std::make_shared<DataBlock>();
        {
            auto col1 = make_column(column_def1_, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(column_def2_, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    }

    void CreateDB() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name_, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CreateTable() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name_, std::move(table_def_), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

protected:
    std::shared_ptr<std::string> db_name_;
    std::shared_ptr<ColumnDef> column_def1_;
    std::shared_ptr<ColumnDef> column_def2_;
    std::shared_ptr<std::string> table_name_;
    std::shared_ptr<TableDef> table_def_;
    NewTxnManager *new_txn_mgr_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, LinearGrowthTest, ::testing::Values(BaseTestParamStr::NEW_VFS_OFF_BG_OFF_PATH));

TEST_P(LinearGrowthTest, test1) {
    CreateDB();
    CreateTable();
    {
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i <= 11451; ++i) {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
            auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
            std::vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
            auto status = txn->Import(*db_name_, *table_name_, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("A Import: {}", duration);
    }
    {
        auto start = std::chrono::system_clock::now();
        auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("A: {}", duration);
    }
    {
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i <= 11451; ++i) {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
            auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
            std::vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
            auto status = txn->Import(*db_name_, *table_name_, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("B Import: {}", duration);
    }
    {
        auto start = std::chrono::system_clock::now();
        auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("B: {}", duration);
    }
    {
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i <= 11451; ++i) {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
            auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
            std::vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
            auto status = txn->Import(*db_name_, *table_name_, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("C Import: {}", duration);
    }
    {
        auto start = std::chrono::system_clock::now();
        auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
        auto duration = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
        std::println("C: {}", duration);
    }
}

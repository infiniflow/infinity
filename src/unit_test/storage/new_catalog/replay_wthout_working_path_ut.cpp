module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.replay_without_working_path;

import :ut.base_test;
import :ut.replay_test;

import :new_txn;
import :table_meta;
import :segment_meta;
import :block_meta;

import third_party;

using namespace infinity;

class TestTxnReplayWithoutWorkingPath : public NewReplayTest {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayWithoutWorkingPath,
                         ::testing::Values(TestTxnReplayWithoutWorkingPath::NEW_CONFIG_PATH,
                                           TestTxnReplayWithoutWorkingPath::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayWithoutWorkingPath, test_1) {
    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto make_input_block = [&](const Value &v1, const Value &v2, size_t row_cnt) {
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
            auto col1 = make_column(column_def1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(column_def2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    auto append = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();
    append();

    UninitTxnMgr();
    CleanupWorkingDir();
    InitTxnMgr();

    auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta;
    TxnTimeStamp create_timestamp;
    auto status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
    EXPECT_TRUE(status.ok());

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));

    SegmentID segment_id = segment_ids_ptr->back();

    SegmentMeta segment_meta(segment_id, *table_meta);

    {
        size_t segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 8);
    }

    {
        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));

        BlockID block_id = block_ids_ptr->back();
        BlockMeta block_meta(block_id, segment_meta);
        size_t block_row_cnt = 0;
        std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
        EXPECT_EQ(block_row_cnt, 8);
    }

    status = new_txn_mgr_->CommitTxn(txn);
}

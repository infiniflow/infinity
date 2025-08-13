#ifndef CI
module;

#include <filesystem>

#include "gtest/gtest.h"

module infinity_core:ut.cleanup_rollback_txn;

import :ut.base_test;
import :stl;
import :third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :infinity_exception;
import :table_def;
import :index_base;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_bmp;
import :defer_op;
import :meta_info;
import :data_block;
import :column_vector;
import :value;
import :kv_code;
import :kv_store;
import :new_txn;
import :new_txn_store;
import :buffer_obj;
import :buffer_handle;
import :secondary_index_in_mem;
import :secondary_index_data;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
import :catalog_meta;
import :mem_index;
import :roaring_bitmap;
import :index_filter_evaluators;
import :index_emvb;
import :wal_manager;
#else
#include "gtest/gtest.h"
#include <filesystem>
import infinity_core;
import base_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import embedding_info;
import sparse_info;
import internal_types;
import statement_common;
import constant_expr;

using namespace infinity;

class TestTxnCleanupRollbackTxn : public BaseTestParamStr {
public:
    void SetUp() override {
        BaseTestParamStr::SetUp();

        new_txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void TearDown() override {
        new_txn_mgr_->PrintAllKeyValue();

        SizeT kv_num = new_txn_mgr_->KeyValueNum();
        EXPECT_EQ(kv_num, 3);

        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    void Init() {
        auto config_path = std::make_shared<std::string>(std::filesystem::absolute(GetParam()));
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        new_txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void UnInit() {
        new_txn_mgr_->PrintAllKeyValue();
        new_txn_mgr_ = nullptr;

        infinity::InfinityContext::instance().UnInit();
    }

    void Cleanup() {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void Checkpoint() {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

protected:
    NewTxnManager *new_txn_mgr_;
    WalManager *wal_manager_;
    Vector<String> delete_file_paths_;
    Vector<String> exist_file_paths_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCleanupRollbackTxn,
                         ::testing::Values(BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH, BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TestTxnCleanupRollbackTxn, test_import_with_index_rollback_cleanup) {
    // there may be many rollback, add rollback test case
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, const Value &v1, const Value &v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    {
        auto *txn_import = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
        Status status = txn_import->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn_drop_db = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn_drop_db->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn_drop_db);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn_import);
        EXPECT_FALSE(status.ok());
    }
    Checkpoint();
    Cleanup();

    CheckFilePaths(delete_file_paths_, exist_file_paths_);
}

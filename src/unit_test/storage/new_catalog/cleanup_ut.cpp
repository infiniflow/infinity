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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.cleanup;

import :ut.base_test;
import third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :table_def;
import :index_base;
import :index_secondary;
import :data_block;
import :column_vector;
import :value;
import :new_txn;
import :logger;
import :db_meeta;
import :table_meeta;
import :segment_meta;
import :block_meta;
import :mem_index;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :buffer_obj;
import :secondary_index_in_mem;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import internal_types;
import constant_expr;
import embedding_info;

using namespace infinity;

class TestTxnCleanup : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr_ = InfinityContext::instance().storage()->new_txn_manager();

        db_name_ = std::make_shared<String>("db1");
        column_def1_ = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2_ = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name_ = std::make_shared<std::string>("tb1");
        table_def_ = TableDef::Make(db_name_, table_name_, std::make_shared<String>(), {column_def1_, column_def2_});

        index_name1_ = std::make_shared<std::string>("index1");
        index_def1_ = IndexSecondary::Make(index_name1_, std::make_shared<String>(), "file_name", {column_def1_->name()});
    }

    void TearDown() override {
        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    void MappingFunction(const std::function<void()> &other_begin, const std::function<void()> &other, const std::function<void()> &other_commit) {
        function_dictionary_['A'] = other_begin;
        function_dictionary_['B'] = other;
        function_dictionary_['C'] = other_commit;

        auto cleanup_begin = [this] {
            {
                auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
                auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("checkpoint"), TransactionType::kNewCheckpoint);
                auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr_->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
            txn_clean_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("cleanup"), TransactionType::kCleanup);
        };
        auto cleanup = [this] {
            Status status = txn_clean_->Cleanup();
            EXPECT_TRUE(status.ok());
        };
        auto cleanup_commit = [this] {
            Status status = new_txn_mgr_->CommitTxn(txn_clean_);
            EXPECT_TRUE(status.ok());
        };
        function_dictionary_['a'] = cleanup_begin;
        function_dictionary_['b'] = cleanup;
        function_dictionary_['c'] = cleanup_commit;
    }

    void TestNoConflict() {
        LOG_INFO("a = cleanup_begin, b = cleanup, c = cleanup_commit");
        LOG_INFO("A = other_begin, B = other, C = other_commit");
        for (const auto &plan : plans_) {
            LOG_INFO(fmt::format("---{}", plan));
            PreTest();
            for (auto func_char : plan) {
                LOG_INFO(fmt::format("now running: {}", func_char));
                function_dictionary_[func_char]();
            }
            PostTest();
        }
    }

    void PreTest() {
        if (static_cast<bool>(pre_fucntion_)) {
            pre_fucntion_();
            return;
        }

        CreateDB();
        CreateTable();
        CreateIndex();

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("import"), TransactionType::kNormal);
            auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
            Vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
            auto status = txn->Import(*db_name_, *table_name_, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("import"), TransactionType::kNormal);
            auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
            Vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
            auto status = txn->Import(*db_name_, *table_name_, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal);
            auto status = txn->Compact(*db_name_, *table_name_, {0, 1});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    void PostTest() {
        validity_function_();
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
            auto status = txn->DropDatabase("db1", ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("cleanup"), TransactionType::kCleanup);
            auto status = txn->Cleanup();
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
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
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name_, ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CreateTable() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name_, std::move(table_def_), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CreateIndex() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("create index"), TransactionType::kNormal);
        auto status = txn->CreateIndex(*db_name_, *table_name_, index_def1_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

protected:
    NewTxnManager *new_txn_mgr_{};

    std::shared_ptr<String> db_name_;
    std::shared_ptr<ColumnDef> column_def1_;
    std::shared_ptr<ColumnDef> column_def2_;
    std::shared_ptr<String> table_name_;
    std::shared_ptr<TableDef> table_def_;

    std::shared_ptr<String> index_name1_;
    std::shared_ptr<IndexBase> index_def1_;
    NewTxn *txn_clean_{};
    NewTxn *txn_other_{};
    static constexpr Array<String, 12> plans_{/*"abcABC",*/
                                              "abAcBC",
                                              "abABcC",
                                              "abABCc",
                                              "aAbcBC",
                                              "aAbBcC",
                                              "aAbBCc",
                                              "aABbcC",
                                              "aABbCc",
                                              "aABCbc",
                                              // "AabcBC",
                                              // "AabBcC",
                                              // "AabBCc",
                                              // "AaBbcC",
                                              // "AaBbCc",
                                              // "AaBCbc",
                                              "ABabcC",
                                              "ABabCc",
                                              "ABaCbc"
                                              /*,"ABCabc"*/};
    HashMap<char, std::function<void()>> function_dictionary_;
    std::function<void()> validity_function_;
    std::function<void()> pre_fucntion_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCleanup,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCleanup, cleanup_with_drop_db) {
    LOG_INFO("Checking cleanup & drop db...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        { // check drop db
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get db"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            TxnTimeStamp db_create_ts;
            auto status = txn->GetDBMeta(*db_name_, db_meta, db_create_ts);
            EXPECT_FALSE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_drop_table) {
    LOG_INFO("Checking cleanup & drop table...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropTable(*db_name_, *table_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        { // check drop table
            auto txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get table"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_FALSE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_add_columns) {
    LOG_INFO("Checking cleanup & add columns...");
    Status status;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("add columns"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->AddColumns(*db_name_, *table_name_, Vector<std::shared_ptr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        { // check add column
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get column"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;
            Status status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2}));

            std::shared_ptr<Vector<std::shared_ptr<ColumnDef>>> column_defs;
            std::tie(column_defs, status) = table_meta->GetColumnDefs();
            EXPECT_TRUE(status.ok());

            bool has_column = false;
            for (const auto &column_def : *column_defs) {
                if (column_def->id() == 2) {
                    has_column = true;
                }
            }
            EXPECT_TRUE(has_column);
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_drop_columns) {
    LOG_INFO("Checking cleanup & drop columns...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop columns"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropColumns(*db_name_, *table_name_, Vector<String>({column_def2_->name()}));
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        { // check drop column
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop column"), TransactionType::kRead);

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;
            Status status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2}));

            std::shared_ptr<Vector<std::shared_ptr<ColumnDef>>> column_defs;
            std::tie(column_defs, status) = table_meta->GetColumnDefs();
            EXPECT_TRUE(status.ok());

            for (const auto &column_def : *column_defs) {
                EXPECT_NE(column_def->id(), 1);
            }
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_rename_table) {
    LOG_INFO("Checking cleanup & rename table...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("rename table"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->RenameTable(*db_name_, *table_name_, *new_table_name);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [&, this] {
        { // check rename table
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get table"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            auto status = txn->GetTableMeta(*db_name_, *new_table_name, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2}));

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_compact) {
    LOG_INFO("Checking cleanup & compact...");
    Status status;

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->Compact(*db_name_, *table_name_, {2});
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        { // check clean & compact
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get segment"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            auto status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({3}));

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_drop_index) {
    LOG_INFO("Checking cleanup & drop index...");
    Status status;

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("drop index"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropIndexByName(*db_name_, *table_name_, *index_name1_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        {
            // check drop index
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("get index"), TransactionType::kRead);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;
            Status status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2}));

            String index_key;
            String index_id;
            TxnTimeStamp create_index_ts;
            status = table_meta->GetIndexID(*index_name1_, index_key, index_id, create_index_ts);
            EXPECT_FALSE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_optimize_index) {
    LOG_INFO("Checking cleanup & optimize index...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("optimize index"), TransactionType::kNormal); };
    auto other = [&, this] {
        SegmentID segment_id = 0;
        status = txn_other_->OptimizeIndex(*db_name_, *table_name_, *index_name1_, segment_id);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    pre_fucntion_ = [this] {
        CreateDB();
        CreateTable();
        CreateIndex();
        SegmentID segment_id = 0;
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>(fmt::format("append {}", i)), TransactionType::kNormal);
                std::shared_ptr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
                Status status = txn->Append(*db_name_, *table_name_, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr_->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
            {
                auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name_, *table_name_, *index_name1_, segment_id);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr_->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
    };

    validity_function_ = [this] {
        { // check optimize index
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("check"), TransactionType::kNormal);
            size_t block_row_cnt = 2;
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Optional<TableIndexMeeta> table_index_meta;
            String table_key;
            String index_key;
            Status status =
                txn->GetTableIndexMeta(*db_name_, *table_name_, *index_name1_, db_meta, table_meta, table_index_meta, &table_key, &index_key);
            EXPECT_TRUE(status.ok());

            {
                auto [segment_ids, status] = table_meta->GetSegmentIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
            }
            SegmentID segment_id = 0;
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

            {
                auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*chunk_ids, Vector<ChunkID>({2}));
            }
            ChunkID chunk_id = 2;
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            {
                ChunkIndexMetaInfo *chunk_info = nullptr;
                Status status = chunk_index_meta.GetChunkInfo(chunk_info);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt);
                EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
            }

            BufferObj *buffer_obj = nullptr;
            status = chunk_index_meta.GetIndexBuffer(buffer_obj);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_append) {
    LOG_INFO("Checking cleanup & append...");
    Status status;

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal); };
    auto other = [&, this] {
        auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
        status = txn_other_->Append(*db_name_, *table_name_, input_block);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };
    validity_function_ = [this] {
        {
            // check append
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("check"), TransactionType::kNormal);

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            auto status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({2, 3}));

            SegmentID segment_id = segment_ids_ptr->back();

            SegmentMeta segment_meta(segment_id, *table_meta);

            {
                size_t segment_row_cnt = 0;
                std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
                EXPECT_EQ(segment_row_cnt, 1);
            }

            {
                Vector<BlockID> *block_ids_ptr = nullptr;
                std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

                BlockID block_id = block_ids_ptr->back();
                BlockMeta block_meta(block_id, segment_meta);
                size_t block_row_cnt = 0;
                std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
                EXPECT_EQ(block_row_cnt, 1);
            }

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_delete) {
    LOG_INFO("Checking cleanup & delete...");
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal); };
    auto other = [&, this] {
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(2, 0));
        auto status = txn_other_->Delete(*db_name_, *table_name_, row_ids);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        auto status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };
    validity_function_ = [this] {
        {
            auto *txn_compact = new_txn_mgr_->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal);
            auto status = txn_compact->Compact(*db_name_, *table_name_, {2});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn_compact);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("check"), TransactionType::kNormal);

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            auto status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({3}));
            SegmentID segment_id = segment_ids_ptr->back();

            SegmentMeta segment_meta(segment_id, *table_meta);

            size_t segment_row_cnt;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, 1);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_update) {
    LOG_INFO("Checking cleanup & update...");
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal); };
    auto other = [&, this] {
        auto input_block = MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(2, 0));
        auto status = txn_other_->Update(*db_name_, *table_name_, input_block, row_ids);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        auto status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    validity_function_ = [this] {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal);
            auto status = txn->Compact(*db_name_, *table_name_, {2, 3});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        { // check update
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("check"), TransactionType::kNormal);

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            auto status = txn->GetTableMeta(*db_name_, *table_name_, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({4}));
            SegmentID segment_id = segment_ids_ptr->back();

            SegmentMeta segment_meta(segment_id, *table_meta);

            size_t segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, 2);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

            BlockID block_id = (*block_ids_ptr)[0];
            BlockMeta block_meta(block_id, segment_meta);
            size_t block_row_cnt = 0;
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            EXPECT_EQ(block_row_cnt, 2);

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

TEST_P(TestTxnCleanup, cleanup_and_dump_index) {
    LOG_INFO("Checking cleanup & dump index...");
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(std::make_unique<String>("dump index"), TransactionType::kNormal); };
    auto other = [&, this] {
        SegmentID segment_id = 1;
        auto status = txn_other_->DumpMemIndex(*db_name_, *table_name_, *index_name1_, segment_id);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        auto status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };
    validity_function_ = [&, this] {
        { // check dump index
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<String>("check"), TransactionType::kNormal);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Optional<TableIndexMeeta> table_index_meta;
            String table_key;
            String index_key;
            auto status =
                txn->GetTableIndexMeta(*db_name_, *table_name_, *index_name1_, db_meta, table_meta, table_index_meta, &table_key, &index_key);
            EXPECT_TRUE(status.ok());

            {
                auto [segment_ids, status] = table_meta->GetSegmentIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*segment_ids, Vector<SegmentID>({2}));
            }
            SegmentID segment_id = 2;
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

            std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            ASSERT_NE(mem_index, nullptr);
            ASSERT_EQ(mem_index->GetSecondaryIndex(), nullptr);

            {
                auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
            }
            ChunkID chunk_id = 0;
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            {
                ChunkIndexMetaInfo *chunk_info = nullptr;
                auto status = chunk_index_meta.GetChunkInfo(chunk_info);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(chunk_info->row_cnt_, 2);
                EXPECT_EQ(chunk_info->base_row_id_, RowID(2, 0));
            }

            BufferObj *buffer_obj = nullptr;
            status = chunk_index_meta.GetIndexBuffer(buffer_obj);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict();
}

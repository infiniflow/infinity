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

#include "gtest/gtest.h"

import base_test;
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import internal_types;
import data_block;
import column_vector;
import value;
import new_txn;
import constant_expr;
import logger;

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
        table_def_ = TableDef::Make(db_name_, table_name_, MakeShared<String>(), {column_def1_, column_def2_});

        index_name1_ = std::make_shared<std::string>("index1");
        index_def1_ = IndexSecondary::Make(index_name1_, MakeShared<String>(), "file_name", {column_def1_->name()});

        CalculatePlans(0, 0);
    }

    void TearDown() override {
        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    void MappingFunction(const std::function<void()> &other_begin, const std::function<void()> &other, const std::function<void()> &other_commit) {
        function_dictionary_['A'] = other_begin;
        function_dictionary_['B'] = other;
        function_dictionary_['C'] = other_commit;

        MappingCleanFunction();
    }

    void MappingCleanFunction() {
        // Status status;
        auto cleanup_begin = [this] { txn_clean_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup); };
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

    void CalculatePlans(SizeT clean_function_index, SizeT other_function_index) {
        if (clean_function_index == 3 && other_function_index == 3) {
            plans_.push_back(tags_);
            return;
        }
        if (clean_function_index <= 2) {
            tags_.push_back(clean_function_index + 'a');
            CalculatePlans(clean_function_index + 1, other_function_index);
            tags_.pop_back();
        }
        if (other_function_index <= 2) {
            tags_.push_back(other_function_index + 'A');
            CalculatePlans(clean_function_index, other_function_index + 1);
            tags_.pop_back();
        }
    }

    void TestNoConflict(bool is_compact) {
        for (const auto &plan : plans_) {
            LOG_INFO(fmt::format("---{}", plan));
            PreTest(is_compact);
            for (auto func_char : plan) {
                function_dictionary_[func_char]();
            }
            PostTest();
        }
    }

    void PreTest(bool is_compact) {
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name_, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name_, std::move(table_def_), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name_, *table_name_, index_def1_, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        u32 block_row_cnt = 8192;
        auto make_input_block = [&](const Value &v1, const Value &v2) {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, const Value &v) {
                for (u32 i = 0; i < block_row_cnt; ++i) {
                    col.AppendValue(v);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(column_def1_->type());
                col1->Initialize();
                append_to_col(*col1, v1);
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = ColumnVector::Make(column_def2_->type());
                col2->Initialize();
                append_to_col(*col2, v2);
                input_block->InsertVector(col2, 1);
            }
            input_block->Finalize();
            return input_block;
        };
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
                Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
                Status status = txn->Import(*db_name_, *table_name_, input_blocks);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr_->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
            {
                auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("delete an element {}", i)), TransactionType::kDelete);
                Vector<RowID> row_ids;
                for (u32 j = 1; j < block_row_cnt; j += 2) {
                    row_ids.push_back(RowID(i, j));
                }
                Status status = txn->Delete(*db_name_, *table_name_, row_ids);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr_->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }
        if (!is_compact) {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
            Status status = txn->Compact(*db_name_, *table_name_, {0, 1});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    void PostTest() {
        Status status;
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
            status = txn->DropDatabase("db1", ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
            status = txn->Cleanup();
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            status = txn->Checkpoint(wal_manager->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

protected:
    NewTxnManager *new_txn_mgr_{};

    SharedPtr<String> db_name_;
    SharedPtr<ColumnDef> column_def1_;
    SharedPtr<ColumnDef> column_def2_;
    SharedPtr<String> table_name_;
    SharedPtr<TableDef> table_def_;

    SharedPtr<String> index_name1_;
    SharedPtr<IndexBase> index_def1_;
    NewTxn *txn_clean_{};
    NewTxn *txn_other_{};
    Vector<String> plans_;
    HashMap<char, std::function<void()>> function_dictionary_;
    String tags_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCleanup,
                         ::testing::Values(/*BaseTestParamStr::NEW_CONFIG_PATH, */ BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

// TEST_P(TestTxnCleanup, cleanup_and_create_db) {}

TEST_P(TestTxnCleanup, cleanup_with_drop_db) {
    LOG_INFO("Checking cleanup & drop db...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropDatabase(*db_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

// TEST_P(TestTxnCleanup, cleanup_and_create_table) {}

TEST_P(TestTxnCleanup, cleanup_and_drop_table) {
    LOG_INFO("Checking cleanup & drop table...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropTable(*db_name_, *table_name_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_add_columns) {
    LOG_INFO("Checking cleanup & add columns...");
    Status status;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->AddColumns(*db_name_, *table_name_, Vector<SharedPtr<ColumnDef>>{column_def3});
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_drop_columns) {
    LOG_INFO("Checking cleanup & drop columns...");
    Status status;
    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropColumns(*db_name_, *table_name_, Vector<String>({column_def2_->name()}));
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_rename_table) {
    LOG_INFO("Checking cleanup & rename table...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->RenameTable(*db_name_, *table_name_, *new_table_name);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_compact) {
    LOG_INFO("Checking cleanup & compact...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->Compact(*db_name_, *table_name_, {0, 1});
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(true);
}

// TEST_P(TestTxnCleanup, cleanup_and_create_index) {}

TEST_P(TestTxnCleanup, cleanup_and_drop_index) {
    LOG_INFO("Checking cleanup & drop index...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal); };
    auto other = [&, this] {
        status = txn_other_->DropIndexByName(*db_name_, *table_name_, *index_name1_, ConflictType::kError);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_optimize_index) {
    LOG_INFO("Checking cleanup & optimize index...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal); };
    auto other = [&, this] {
        SegmentID segment_id = 2;
        status = txn_other_->OptimizeIndex(*db_name_, *table_name_, *index_name1_, segment_id);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_append) {
    LOG_INFO("Checking cleanup & append...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal); };
    auto other = [&, this] {
        auto make_input_block = [&](const Value &v1, const Value &v2, SizeT row_cnt) {
            auto make_column = [&](SharedPtr<ColumnDef> &column_def, const Value &v) {
                auto col = ColumnVector::Make(column_def->type());
                col->Initialize();
                for (SizeT i = 0; i < row_cnt; ++i) {
                    col->AppendValue(v);
                }
                return col;
            };
            auto input_block = MakeShared<DataBlock>();
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
        };
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        status = txn_other_->Append(*db_name_, *table_name_, input_block);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_delete) {
    LOG_INFO("Checking cleanup & delete...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal); };
    auto other = [&, this] {
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(2, 0));
        status = txn_other_->Delete(*db_name_, *table_name_, row_ids);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_update) {
    LOG_INFO("Checking cleanup & update...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal); };
    auto other = [&, this] {
        auto make_input_block = [&](const Value &v1, const Value &v2, SizeT row_cnt) {
            auto make_column = [&](SharedPtr<ColumnDef> &column_def, const Value &v) {
                auto col = ColumnVector::Make(column_def->type());
                col->Initialize();
                for (SizeT i = 0; i < row_cnt; ++i) {
                    col->AppendValue(v);
                }
                return col;
            };
            auto input_block = MakeShared<DataBlock>();
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
        };
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(2, 0));
        status = txn_other_->Update(*db_name_, *table_name_, input_block, row_ids);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

TEST_P(TestTxnCleanup, cleanup_and_dump_index) {
    LOG_INFO("Checking cleanup & dump index...");
    Status status;
    auto new_table_name = std::make_shared<std::string>("tb2");

    auto other_begin = [this] { txn_other_ = new_txn_mgr_->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal); };
    auto other = [&, this] {
        SegmentID segment_id = 2;
        status = txn_other_->DumpMemIndex(*db_name_, *table_name_, *index_name1_, segment_id);
        EXPECT_TRUE(status.ok());
    };
    auto other_commit = [&, this] {
        status = new_txn_mgr_->CommitTxn(txn_other_);
        EXPECT_TRUE(status.ok());
    };

    MappingFunction(other_begin, other, other_commit);
    TestNoConflict(false);
}

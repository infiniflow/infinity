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

import stl;
import base_test;
import status;
import new_txn_manager;
import infinity_context;
import new_txn;
import txn_state;
import extra_ddl_info;
import third_party;
import logger;
import column_def;
import data_type;
import logical_type;
import table_def;
import data_block;
import column_vector;
import value;

using namespace infinity;

class TestTxnManagerTest : public NewBaseTestNoParam {
public:
};

TEST_F(TestTxnManagerTest, test_ts) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");

    Vector<TxnTimeStamp> begin_tss;
    Vector<TxnTimeStamp> commit_tss;
    {
        TxnTimeStamp txn1_begin_ts = 0;
        TxnTimeStamp txn1_commit_ts = 0;

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        txn1_begin_ts = txn->BeginTS();
        status = new_txn_mgr->CommitTxn(txn, &txn1_commit_ts);
        EXPECT_TRUE(status.ok());

        begin_tss.push_back(txn1_begin_ts);
        commit_tss.push_back(txn1_commit_ts);
    }
    EXPECT_LT(begin_tss[0], commit_tss[0]);

    {
        TxnTimeStamp txn2_begin_ts = 0;
        TxnTimeStamp txn2_commit_ts = 0;
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("read txn"), TransactionType::kNormal);

        txn2_begin_ts = txn->BeginTS();
        Status status = new_txn_mgr->CommitTxn(txn, &txn2_commit_ts);
        EXPECT_TRUE(status.ok());

        begin_tss.push_back(txn2_begin_ts);
        commit_tss.push_back(txn2_commit_ts);
    }

    EXPECT_GE(commit_tss[1], commit_tss[0]);
    EXPECT_EQ(begin_tss[1], commit_tss[1]);

    {
        TxnTimeStamp txn3_begin_ts = 0;
        // TxnTimeStamp txn3_commit_ts = 0;

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("rollback txn"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        txn3_begin_ts = txn->BeginTS();
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        begin_tss.push_back(txn3_begin_ts);
        commit_tss.push_back(0);
    }

    EXPECT_EQ(begin_tss[2], commit_tss[1]);

    {
        TxnTimeStamp txn4_begin_ts = 0;
        TxnTimeStamp txn4_commit_ts = 0;

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        {
            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db2"), TransactionType::kNormal);
            status = txn2->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }

        txn4_begin_ts = txn->BeginTS();
        status = new_txn_mgr->CommitTxn(txn, &txn4_commit_ts);
        EXPECT_FALSE(status.ok());

        begin_tss.push_back(txn4_begin_ts);
        commit_tss.push_back(txn4_commit_ts);
    }

    EXPECT_GE(begin_tss[3], commit_tss[2]);
    EXPECT_LT(begin_tss[3], commit_tss[3]);

    {
        TxnTimeStamp txn1_begin_ts = 0;
        TxnTimeStamp txn1_commit_ts = 0;

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);

        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());

        txn1_begin_ts = txn->BeginTS();
        status = new_txn_mgr->CommitTxn(txn, &txn1_commit_ts);
        EXPECT_TRUE(status.ok());

        begin_tss.push_back(txn1_begin_ts);
        commit_tss.push_back(txn1_commit_ts);
    }

    EXPECT_LT(begin_tss[4], commit_tss[3]);
    EXPECT_LT(begin_tss[4], commit_tss[4]);

    EXPECT_EQ(new_txn_mgr->CurrentTS(), new_txn_mgr->PrepareCommitTS());
}

TEST_F(TestTxnManagerTest, test_parallel_ts) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");

    SizeT thread_num = 2;
    SizeT loop_num = 1280;
    Vector<std::thread> worker_threads;

    for (SizeT thread_i = 0; thread_i < thread_num; ++thread_i) {
        worker_threads.push_back(std::thread([&, thread_i] {
            std::cout << "Thread " << thread_i << " start" << std::endl;
            for (SizeT loop_i = 0; loop_i < loop_num; ++loop_i) {
                Status status;
                {
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
                    TransactionID txn_id = txn->TxnID();
                    LOG_INFO(fmt::format("Thread: {}, txn_id: {} CreateDatabase", thread_i, txn_id));
                    status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
                    if (status.ok()) {
                        status = new_txn_mgr->CommitTxn(txn);
                        if (!status.ok()) {
                            LOG_WARN(fmt::format("Thread: {}, txn_id: {}, CreateDatabase CommitTxn failed: {}", thread_i, txn_id, status.message()));
                        } else {
                            LOG_INFO(fmt::format("Thread: {}, txn_id: {}, CreateDatabase CommitTxn success", thread_i, txn_id));
                        }
                    } else {
                        LOG_WARN(fmt::format("Thread: {}, txn_id: {}, CreateDatabase failed: {}", thread_i, txn_id, status.message()));
                        status = new_txn_mgr->RollBackTxn(txn);
                    }
                    LOG_INFO(fmt::format("Thread: {}, txn_id: {} Done", thread_i, txn_id));
                }
                {
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop"), TransactionType::kNormal);
                    TransactionID txn_id = txn->TxnID();
                    LOG_INFO(fmt::format("Thread: {}, txn_id: {} DropDatabase", thread_i, txn->TxnID()));
                    status = txn->DropDatabase(*db_name, ConflictType::kError);
                    if (status.ok()) {
                        status = new_txn_mgr->CommitTxn(txn);
                        if (!status.ok()) {
                            LOG_WARN(fmt::format("Thread: {}, txn_id: {}, DropDatabase CommitTxn failed: {}", thread_i, txn_id, status.message()));
                        } else {
                            LOG_INFO(fmt::format("Thread: {}, txn_id: {}, DropDatabase CommitTxn success", thread_i, txn_id));
                        }
                    } else {
                        LOG_ERROR(fmt::format("Thread: {}, DropDatabase failed: {}", thread_i, status.message()));
                        status = new_txn_mgr->RollBackTxn(txn);
                    }
                    LOG_INFO(fmt::format("Thread: {}, txn_id: {} Done", thread_i, txn_id));
                }
            }
        }));
    }
    for (SizeT thread_i = 0; thread_i < thread_num; ++thread_i) {
        worker_threads[thread_i].join();
    }

    EXPECT_LE(new_txn_mgr->CurrentTS(), new_txn_mgr->PrepareCommitTS());
}

TEST_F(TestTxnManagerTest, test_check_txns) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    Status status;
    Vector<NewTxn *> check_txns;

    auto get_check_txns = [&](NewTxn *txn) {
        //        TxnTimeStamp fake_commit_ts = new_txn_mgr->CurrentTS() + 1;
        Vector<SharedPtr<NewTxn>> check_txn_ptrs = new_txn_mgr->GetCheckCandidateTxns(txn);
        Vector<NewTxn *> check_txns;
        for (auto &check_txn : check_txn_ptrs) {
            check_txns.push_back(check_txn.get());
        }
        return check_txns;
    };

    auto check_empty = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check empty"), TransactionType::kNormal);
        check_txns = get_check_txns(txn);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({}));
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn1"), TransactionType::kNormal);
        status = txn1->Dummy();
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn2"), TransactionType::kNormal);
        status = txn2->Dummy();
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn3"), TransactionType::kNormal);
        status = txn3->Dummy();
        EXPECT_TRUE(status.ok());

        check_txns = get_check_txns(txn1);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({}));

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_txns = get_check_txns(txn2);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({txn1}));

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        check_txns = get_check_txns(txn3);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({txn1, txn2}));

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_empty();
    }

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn1"), TransactionType::kNormal);
        status = txn1->Dummy();
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn2"), TransactionType::kNormal);
        status = txn2->Dummy();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("txn3"), TransactionType::kNormal);
        status = txn3->Dummy();
        EXPECT_TRUE(status.ok());

        check_txns = get_check_txns(txn1);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({txn2}));

        check_txns = get_check_txns(txn3);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({}));

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_txns = get_check_txns(txn1);
        EXPECT_EQ(check_txns, Vector<NewTxn *>({txn2, txn3}));

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_empty();
    }
}

TEST_F(TestTxnManagerTest, test_parallel_insert_table) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Create db
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    Status status1 = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status1.ok());
    status1 = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status1.ok());

    SizeT loop_num = 1200;
    Vector<std::thread> worker_threads;

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");

    worker_threads.push_back(std::thread([&] {
        std::cout << "Thread create / drop table start" << std::endl;

        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

        for (SizeT loop_i = 0; loop_i < loop_num; ++loop_i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
                Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
                //                LOG_INFO("Thread: create table success");
            }
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
                Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
                if (status.ok()) {
                    status = new_txn_mgr->CommitTxn(txn);
                    EXPECT_TRUE(status.ok());
                    //                    LOG_INFO("Thread: drop table success");
                } else {
                    LOG_WARN(fmt::format("Thread: drop table failed, rollback: {}", status.message()));
                    status = new_txn_mgr->RollBackTxn(txn);
                    EXPECT_TRUE(status.ok());
                }
            }
        }

        LOG_INFO("Finish thread: create / drop table");
    }));

    worker_threads.push_back(std::thread([&] {
        std::cout << "Thread 1 insert table start" << std::endl;

        auto input_block = MakeShared<DataBlock>();
        {
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(column_def1->type());
                col1->Initialize();
                col1->AppendValue(Value::MakeInt(1));
                col1->AppendValue(Value::MakeInt(2));
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = ColumnVector::Make(column_def2->type());
                col2->Initialize();
                col2->AppendValue(Value::MakeVarchar("abc"));
                col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                input_block->InsertVector(col2, 1);
            }
            input_block->Finalize();
        }

        for (SizeT loop_i = 0; loop_i < loop_num; ++loop_i) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, input_block);
            if (status.ok()) {
                status = new_txn_mgr->CommitTxn(txn);
                if (!status.ok()) {
                    LOG_WARN(fmt::format("Thread 1: insert commit failed: {}", status.message()));
                } else {
                    //                    LOG_INFO("Thread 1: insert commit success");
                }
            } else {
                //                LOG_WARN(fmt::format("Thread 1: insert rollback: {}", status.message()));
                status = new_txn_mgr->RollBackTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        LOG_INFO("Finish thread 1");
    }));

    worker_threads.push_back(std::thread([&] {
        std::cout << "Thread 2 insert table start" << std::endl;

        auto input_block = MakeShared<DataBlock>();
        {
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(column_def1->type());
                col1->Initialize();
                col1->AppendValue(Value::MakeInt(1));
                col1->AppendValue(Value::MakeInt(2));
                input_block->InsertVector(col1, 0);
            }
            {
                auto col2 = ColumnVector::Make(column_def2->type());
                col2->Initialize();
                col2->AppendValue(Value::MakeVarchar("abc"));
                col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                input_block->InsertVector(col2, 1);
            }
            input_block->Finalize();
        }

        for (SizeT loop_i = 0; loop_i < loop_num; ++loop_i) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, input_block);
            if (status.ok()) {
                status = new_txn_mgr->CommitTxn(txn);
                if (!status.ok()) {
                    LOG_WARN(fmt::format("Thread 2: insert commit failed: {}", status.message()));
                } else {
                    //                    LOG_INFO("Thread 2: insert commit success");
                }
            } else {
                //                LOG_WARN(fmt::format("Thread 2: insert rollback: {}", status.message()));
                status = new_txn_mgr->RollBackTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        LOG_INFO("Finish thread 2");
    }));

    for (SizeT thread_i = 0; thread_i < worker_threads.size(); ++thread_i) {
        worker_threads[thread_i].join();
    }

    EXPECT_LE(new_txn_mgr->CurrentTS(), new_txn_mgr->PrepareCommitTS());
}

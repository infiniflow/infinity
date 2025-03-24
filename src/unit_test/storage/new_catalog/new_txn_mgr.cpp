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

using namespace infinity;

class NewTxnManagerTest : public BaseTestNoParam {
public:
};

TEST_F(NewTxnManagerTest, test_ts) {
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
            Status status = txn2->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
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

    EXPECT_GE(begin_tss[4], commit_tss[3]);
    EXPECT_LT(begin_tss[4], commit_tss[4]);

    EXPECT_EQ(new_txn_mgr->CurrentTS(), new_txn_mgr->PrepareCommitTS());
}

TEST_F(NewTxnManagerTest, test_parallel_ts) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");

    SizeT thread_num = 4;
    SizeT loop_num = 128;
    Vector<std::thread> worker_threads;

    for (SizeT thread_i = 0; thread_i < thread_num; ++thread_i) {
        worker_threads.push_back(std::thread([&, thread_i] {
            std::cout << "Thread " << thread_i << " start" << std::endl;
            for (SizeT loop_i = 0; loop_i < loop_num; ++loop_i) {
                Status status;
                {
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
                    status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
                    if (status.ok()) {
                        status = new_txn_mgr->CommitTxn(txn);
                    }
                }
                {
                    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop"), TransactionType::kNormal);
                    status = txn->DropDatabase(*db_name, ConflictType::kError);
                    if (status.ok()) {
                        status = new_txn_mgr->CommitTxn(txn);
                    }
                }
            }
        }));
    }
    for (SizeT thread_i = 0; thread_i < thread_num; ++thread_i) {
        worker_threads[thread_i].join();
    }

    EXPECT_EQ(new_txn_mgr->CurrentTS(), new_txn_mgr->PrepareCommitTS());
}

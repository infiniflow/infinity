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

import replay_test;
import base_test;
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import data_access_state;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import constant_expr;

using namespace infinity;

class NewCheckpointTest : public NewReplayTest {
protected:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewCheckpointTest,
                         ::testing::Values(NewCheckpointTest::NEW_CONFIG_NOWAL_PATH, NewCheckpointTest::NEW_VFS_OFF_CONFIG_NOWAL_PATH));

TEST_P(NewCheckpointTest, checkpoint_and_create_db) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");

    Status status;
    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
            Status status = txn2->Checkpoint();
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            std::sort(db_names.begin(), db_names.end());
            EXPECT_EQ(db_names, Vector<String>({*db_name, "default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
            Status status = txn2->Checkpoint();
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());

            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(db_names, Vector<String>({"default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
            Status status = txn2->Checkpoint();
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(db_names, Vector<String>({"default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    // UninitTxnMgr();
    // CleanupDbDirs();
    // InitTxnMgr();

    // {
    //     {
    //         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    //         status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
    //         EXPECT_TRUE(status.ok());

    //         auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);

    //         status = new_txn_mgr->CommitTxn(txn);
    //         EXPECT_TRUE(status.ok());

    //         Status status = txn2->Checkpoint();
    //         EXPECT_TRUE(status.ok());
    //         status = new_txn_mgr->CommitTxn(txn2);
    //         EXPECT_TRUE(status.ok());
    //     }
    //     RestartTxnMgr();
    //     {
    //         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

    //         Vector<String> db_names;
    //         Status status = txn->ListDatabase(db_names);
    //         EXPECT_TRUE(status.ok());
    //         EXPECT_EQ(db_names, Vector<String>({"default_db"}));

    //         status = new_txn_mgr->CommitTxn(txn);
    //         EXPECT_TRUE(status.ok());
    //     }
    // }
}

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
import replay_test;

import stl;
import third_party;
import status;
import new_txn_manager;
import new_txn;
import column_def;
import data_type;
import table_def;
import index_base;
import infinity_context;
import logical_type;
import txn_state;
import extra_ddl_info;
import data_block;
import column_vector;
import value;
import new_catalog;
import internal_types;

import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import catalog_meta;

import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import index_emvb;
import embedding_info;
import sparse_info;
import index_bmp;
import defer_op;
import statement_common;
import mem_index;

import buffer_obj;
import buffer_handle;
import constant_expr;
import logger;
import kv_store;

using namespace infinity;

class TxnReplayExceptionTest : public NewReplayTest {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TxnReplayExceptionTest,
                         ::testing::Values(TxnReplayExceptionTest::NEW_CONFIG_PATH, TxnReplayExceptionTest::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TxnReplayExceptionTest, test_replay_create_db) {
    using namespace infinity;

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn1->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->kv_store()->Flush();
    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
        Vector<String> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(Set<String>(db_names.begin(), db_names.end()), Set<String>({"default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

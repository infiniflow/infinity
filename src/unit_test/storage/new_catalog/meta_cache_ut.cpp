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

module infinity_core:ut.meta_cache;

import :ut.base_test;
import :kv_utility;
import :kv_code;
import :stl;
import :meta_cache;

#else

#include "gtest/gtest.h"
import infinity_core;
import base_test;

#endif

using namespace infinity;

class MetaCacheTest : public BaseTest {};

TEST_F(MetaCacheTest, test_db) {
    MetaCache cache(2);
    {
        SharedPtr<MetaDbCache> db_cache = MakeShared<MetaDbCache>("db1", 0, 4, "catalog|db|db1|4", false);
        cache.Operate({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts_, 4);
        EXPECT_EQ(db_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        SharedPtr<MetaDbCache> db_cache = MakeShared<MetaDbCache>("db1", 0, 6, "catalog|db|db1|6", true);
        cache.Operate({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts_, 4);
        EXPECT_EQ(db_cache->is_dropped_, false);
        db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts_, 6);
        EXPECT_EQ(db_cache->is_dropped_, true);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        SharedPtr<MetaDbCache> db_cache = MakeShared<MetaDbCache>("db1", 0, 8, "catalog|db|db1|8", false);
        cache.Operate({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts_, 6);
        EXPECT_EQ(db_cache->is_dropped_, true);
        db_cache = cache.GetDb("db1", 9);
        EXPECT_EQ(db_cache->commit_ts_, 8);
        EXPECT_EQ(db_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {

        // cache.PrintLRU();

        SharedPtr<MetaDbCache> db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts_, 6);
        EXPECT_EQ(db_cache->is_dropped_, true);

        // cache.PrintLRU();

        db_cache = MakeShared<MetaDbCache>("db2", 1, 10, "catalog|db|db2|10", false);
        cache.Operate({db_cache}, nullptr);

        // cache.PrintLRU();

        db_cache = cache.GetDb("db1", 9);
        EXPECT_EQ(db_cache->commit_ts_, 6);
        EXPECT_EQ(db_cache->is_dropped_, true);

        db_cache = cache.GetDb("db2", 11);
        EXPECT_EQ(db_cache->commit_ts_, 10);
        EXPECT_EQ(db_cache->is_dropped_, false);

        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
}

TEST_F(MetaCacheTest, test_table) {
    MetaCache cache(2);
    {
        SharedPtr<MetaTableCache> table_cache = MakeShared<MetaTableCache>(0, "tbl1", 0, 4, "catalog|tbl|0|tbl1|4", false);
        cache.Operate({table_cache}, nullptr);
        table_cache = cache.GetTable(0, "tbl1", 3);
        EXPECT_EQ(table_cache.get(), nullptr);
        table_cache = cache.GetTable(0, "tbl1", 5);
        EXPECT_EQ(table_cache->commit_ts_, 4);
        EXPECT_EQ(table_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        SharedPtr<MetaTableCache> table_cache = MakeShared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        cache.Operate({table_cache}, nullptr);
        table_cache = cache.GetTable(0, "tbl1", 3);
        EXPECT_EQ(table_cache.get(), nullptr);
        table_cache = cache.GetTable(0, "tbl1", 5);
        EXPECT_EQ(table_cache->commit_ts_, 4);
        EXPECT_EQ(table_cache->is_dropped_, false);
        table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        SharedPtr<MetaTableCache> table_cache = MakeShared<MetaTableCache>(0, "tbl1", 0, 8, "catalog|tbl|0|tbl1|8", false);
        cache.Operate({table_cache}, nullptr);
        table_cache = cache.GetTable(0, "tbl1", 3);
        EXPECT_EQ(table_cache.get(), nullptr);
        table_cache = cache.GetTable(0, "tbl1", 5);
        EXPECT_EQ(table_cache.get(), nullptr);
        table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);
        table_cache = cache.GetTable(0, "tbl1", 9);
        EXPECT_EQ(table_cache->commit_ts_, 8);
        EXPECT_EQ(table_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {

        // cache.PrintLRU();

        SharedPtr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);

        // cache.PrintLRU();

        table_cache = MakeShared<MetaTableCache>(0, "tbl2", 1, 10, "catalog|tbl|0|tbl2|8", false);
        cache.Operate({table_cache}, nullptr);

        // cache.PrintLRU();

        table_cache = cache.GetTable(0, "tbl1", 9);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);

        table_cache = cache.GetTable(0, "tbl2", 11);
        EXPECT_EQ(table_cache->commit_ts_, 10);
        EXPECT_EQ(table_cache->is_dropped_, false);

        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
}

TEST_F(MetaCacheTest, test_index) {
    MetaCache cache(2);
    {
        SharedPtr<MetaIndexCache> index_cache = MakeShared<MetaIndexCache>(0, 0, "idx1", 0, 4, "catalog|idx|0|0|idx1|4", false);
        cache.Operate({index_cache}, nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 3);
        EXPECT_EQ(index_cache.get(), nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 5);
        EXPECT_NE(index_cache.get(), nullptr);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        SharedPtr<MetaIndexCache> index_cache = MakeShared<MetaIndexCache>(0, 0, "idx1", 0, 6, "catalog|idx|0|0|idx1|6", true);
        cache.Operate({index_cache}, nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 3);
        EXPECT_EQ(index_cache.get(), nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 5);
        EXPECT_EQ(index_cache->commit_ts_, 4);
        EXPECT_EQ(index_cache->is_dropped_, false);
        index_cache = cache.GetIndex(0, 0, "idx1", 7);
        EXPECT_EQ(index_cache->commit_ts_, 6);
        EXPECT_EQ(index_cache->is_dropped_, true);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        SharedPtr<MetaIndexCache> index_cache = MakeShared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false);
        cache.Operate({index_cache}, nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 3);
        EXPECT_EQ(index_cache.get(), nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 5);
        EXPECT_EQ(index_cache.get(), nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 7);
        EXPECT_EQ(index_cache->commit_ts_, 6);
        EXPECT_EQ(index_cache->is_dropped_, true);
        index_cache = cache.GetIndex(0, 0, "idx1", 9);
        EXPECT_EQ(index_cache->commit_ts_, 8);
        EXPECT_EQ(index_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {

        // cache.PrintLRU();

        SharedPtr<MetaIndexCache> index_cache = cache.GetIndex(0, 0, "idx1", 7);
        EXPECT_EQ(index_cache->commit_ts_, 6);
        EXPECT_EQ(index_cache->is_dropped_, true);

        // cache.PrintLRU();

        index_cache = MakeShared<MetaIndexCache>(0, 0, "idx2", 1, 10, "catalog|idx|0|0|idx2|10", false);
        cache.Operate({index_cache}, nullptr);

        // cache.PrintLRU();

        index_cache = cache.GetIndex(0, 0, "idx1", 9);
        EXPECT_EQ(index_cache->commit_ts_, 6);
        EXPECT_EQ(index_cache->is_dropped_, true);

        index_cache = cache.GetIndex(0, 0, "idx2", 11);
        EXPECT_EQ(index_cache->commit_ts_, 10);
        EXPECT_EQ(index_cache->is_dropped_, false);

        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
}

TEST_F(MetaCacheTest, test_mix) {
    MetaCache cache(2);
    {
        SharedPtr<MetaDbCache> db_cache = MakeShared<MetaDbCache>("db1", 0, 4, "catalog|db|db1|4", false);
        cache.Operate({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts_, 4);
        EXPECT_EQ(db_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        SharedPtr<MetaTableCache> table_cache = MakeShared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        cache.Operate({table_cache}, nullptr);
        SharedPtr<MetaDbCache> db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts_, 4);

        table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        SharedPtr<MetaIndexCache> index_cache = MakeShared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false);
        cache.Operate({index_cache}, nullptr);
        SharedPtr<MetaDbCache> db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache.get(), nullptr);

        SharedPtr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);

        index_cache = cache.GetIndex(0, 0, "idx1", 9);
        EXPECT_EQ(index_cache->commit_ts_, 8);
        EXPECT_EQ(index_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
}

TEST_F(MetaCacheTest, test_erase) {
    MetaCache cache(3);
    {
        SharedPtr<MetaDbCache> db_cache = MakeShared<MetaDbCache>("db1", 0, 4, "catalog|db|db1|4", false);
        cache.Operate({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts_, 4);
        EXPECT_EQ(db_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        SharedPtr<MetaTableCache> table_cache = MakeShared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        cache.Operate({table_cache}, nullptr);
        table_cache = cache.GetTable(0, "tbl1", 7);
        EXPECT_EQ(table_cache->commit_ts_, 6);
        EXPECT_EQ(table_cache->is_dropped_, true);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        SharedPtr<MetaIndexCache> index_cache = MakeShared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false);
        cache.Operate({index_cache}, nullptr);
        index_cache = cache.GetIndex(0, 0, "idx1", 9);
        EXPECT_EQ(index_cache->commit_ts_, 8);
        EXPECT_EQ(index_cache->is_dropped_, false);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 3);
    }
    {
        SharedPtr<MetaEraseDbCache> drop_cache = MakeShared<MetaEraseDbCache>("db1");
        cache.Operate({drop_cache}, nullptr);
        SharedPtr<MetaDbCache> db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache.get(), nullptr);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 3);
    }
    {
        SharedPtr<MetaEraseTableCache> drop_cache = MakeShared<MetaEraseTableCache>(0, "tbl1");
        cache.Operate({drop_cache}, nullptr);
        SharedPtr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 5);
        EXPECT_EQ(table_cache.get(), nullptr);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 3);
    }
    {
        SharedPtr<MetaEraseIndexCache> drop_cache = MakeShared<MetaEraseIndexCache>(0, 0, "idx1");
        cache.Operate({drop_cache}, nullptr);
        SharedPtr<MetaIndexCache> index_cache = cache.GetIndex(0, 0, "idx1", 5);
        EXPECT_EQ(index_cache.get(), nullptr);
        SizeT size = cache.Size();
        EXPECT_EQ(size, 3);
    }
}

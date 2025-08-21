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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.meta_cache;

import :ut.base_test;
import :kv_utility;
import :kv_code;
import std;
import :meta_cache;

using namespace infinity;

class MetaCacheTest : public BaseTest {};

TEST_F(MetaCacheTest, test_db) {
    MetaCache cache(2);
    {
        std::string db_name = "db1";
        std::string db_key = "catalog|db|db1|4";
        std::shared_ptr<MetaDbCache> db_cache = std::make_shared<MetaDbCache>(db_name, 0, 4, db_key, false, 0);
        cache.PutOrErase({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts(), 4);
        EXPECT_EQ(db_cache->is_dropped(), false);
        size_t size = cache.Size();
        EXPECT_EQ(size, 1);
    }
    {
        std::shared_ptr<MetaDbCache> db_cache = std::make_shared<MetaDbCache>("db1", 0, 6, "catalog|db|db1|6", true, 0);
        cache.PutOrErase({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache->commit_ts(), 4);
        EXPECT_EQ(db_cache->is_dropped(), false);
        db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts(), 6);
        EXPECT_EQ(db_cache->is_dropped(), true);
        size_t size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {
        std::shared_ptr<MetaDbCache> db_cache = std::make_shared<MetaDbCache>("db1", 0, 8, "catalog|db|db1|8", false, 0);
        cache.PutOrErase({db_cache}, nullptr);
        db_cache = cache.GetDb("db1", 3);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 5);
        EXPECT_EQ(db_cache.get(), nullptr);
        db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts(), 6);
        EXPECT_EQ(db_cache->is_dropped(), true);
        db_cache = cache.GetDb("db1", 9);
        EXPECT_EQ(db_cache->commit_ts(), 8);
        EXPECT_EQ(db_cache->is_dropped(), false);
        size_t size = cache.Size();
        EXPECT_EQ(size, 2);
    }
    {

        // cache.PrintLRU();

        std::shared_ptr<MetaDbCache> db_cache = cache.GetDb("db1", 7);
        EXPECT_EQ(db_cache->commit_ts(), 6);
        EXPECT_EQ(db_cache->is_dropped(), true);

        // cache.PrintLRU();

        db_cache = std::make_shared<MetaDbCache>("db2", 1, 10, "catalog|db|db2|10", false, 0);
        cache.PutOrErase({db_cache}, nullptr);

        // cache.PrintLRU();

        db_cache = cache.GetDb("db1", 9);
        EXPECT_EQ(db_cache->commit_ts(), 6);
        EXPECT_EQ(db_cache->is_dropped(), true);

        db_cache = cache.GetDb("db2", 11);
        EXPECT_EQ(db_cache->commit_ts(), 10);
        EXPECT_EQ(db_cache->is_dropped(), false);

        size_t size = cache.Size();
        EXPECT_EQ(size, 2);
    }
}

//TEST_F(MetaCacheTest, test_table) {
//    MetaCache cache(2);
//    {
//        std::shared_ptr<MetaTableCache> table_cache = std::make_shared<MetaTableCache>(0, "tbl1", 0, 4, "catalog|tbl|0|tbl1|4", false, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 3);
//        EXPECT_EQ(table_cache.get(), nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 5);
//        EXPECT_EQ(table_cache->commit_ts(), 4);
//        EXPECT_EQ(table_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 1);
//    }
//    {
//        std::shared_ptr<MetaTableCache> table_cache = std::make_shared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 3);
//        EXPECT_EQ(table_cache.get(), nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 5);
//        EXPECT_EQ(table_cache->commit_ts(), 4);
//        EXPECT_EQ(table_cache->is_dropped(), false);
//        table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//        std::shared_ptr<MetaTableCache> table_cache = std::make_shared<MetaTableCache>(0, "tbl1", 0, 8, "catalog|tbl|0|tbl1|8", false, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 3);
//        EXPECT_EQ(table_cache.get(), nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 5);
//        EXPECT_EQ(table_cache.get(), nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//        table_cache = cache.GetTable(0, "tbl1", 9);
//        EXPECT_EQ(table_cache->commit_ts(), 8);
//        EXPECT_EQ(table_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//
//        // cache.PrintLRU();
//
//        std::shared_ptr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//
//        // cache.PrintLRU();
//
//        table_cache = std::make_shared<MetaTableCache>(0, "tbl2", 1, 10, "catalog|tbl|0|tbl2|8", false, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//
//        // cache.PrintLRU();
//
//        table_cache = cache.GetTable(0, "tbl1", 9);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//
//        table_cache = cache.GetTable(0, "tbl2", 11);
//        EXPECT_EQ(table_cache->commit_ts(), 10);
//        EXPECT_EQ(table_cache->is_dropped(), false);
//
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//}
//
//TEST_F(MetaCacheTest, test_index) {
//    MetaCache cache(2);
//    {
//        std::shared_ptr<MetaIndexCache> index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx1", 0, 4, "catalog|idx|0|0|idx1|4", false, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 3);
//        EXPECT_EQ(index_cache.get(), nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 5);
//        EXPECT_NE(index_cache.get(), nullptr);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 1);
//    }
//    {
//        std::shared_ptr<MetaIndexCache> index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx1", 0, 6, "catalog|idx|0|0|idx1|6", true, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 3);
//        EXPECT_EQ(index_cache.get(), nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 5);
//        EXPECT_EQ(index_cache->commit_ts(), 4);
//        EXPECT_EQ(index_cache->is_dropped(), false);
//        index_cache = cache.GetIndex(0, 0, "idx1", 7);
//        EXPECT_EQ(index_cache->commit_ts(), 6);
//        EXPECT_EQ(index_cache->is_dropped(), true);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//        std::shared_ptr<MetaIndexCache> index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 3);
//        EXPECT_EQ(index_cache.get(), nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 5);
//        EXPECT_EQ(index_cache.get(), nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 7);
//        EXPECT_EQ(index_cache->commit_ts(), 6);
//        EXPECT_EQ(index_cache->is_dropped(), true);
//        index_cache = cache.GetIndex(0, 0, "idx1", 9);
//        EXPECT_EQ(index_cache->commit_ts(), 8);
//        EXPECT_EQ(index_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//
//        // cache.PrintLRU();
//
//        std::shared_ptr<MetaIndexCache> index_cache = cache.GetIndex(0, 0, "idx1", 7);
//        EXPECT_EQ(index_cache->commit_ts(), 6);
//        EXPECT_EQ(index_cache->is_dropped(), true);
//
//        // cache.PrintLRU();
//
//        index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx2", 1, 10, "catalog|idx|0|0|idx2|10", false, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//
//        // cache.PrintLRU();
//
//        index_cache = cache.GetIndex(0, 0, "idx1", 9);
//        EXPECT_EQ(index_cache->commit_ts(), 6);
//        EXPECT_EQ(index_cache->is_dropped(), true);
//
//        index_cache = cache.GetIndex(0, 0, "idx2", 11);
//        EXPECT_EQ(index_cache->commit_ts(), 10);
//        EXPECT_EQ(index_cache->is_dropped(), false);
//
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//}
//
//TEST_F(MetaCacheTest, test_mix) {
//    MetaCache cache(2);
//    {
//        std::shared_ptr<MetaDbCache> db_cache = std::make_shared<MetaDbCache>("db1", 0, 4, "catalog|db|db1|4", false, 0);
//        cache.PutOrErase({db_cache}, nullptr);
//        db_cache = cache.GetDb("db1", 3);
//        EXPECT_EQ(db_cache.get(), nullptr);
//        db_cache = cache.GetDb("db1", 5);
//        EXPECT_EQ(db_cache->commit_ts(), 4);
//        EXPECT_EQ(db_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 1);
//    }
//    {
//        std::shared_ptr<MetaTableCache> table_cache = std::make_shared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//        std::shared_ptr<MetaDbCache> db_cache = cache.GetDb("db1", 3);
//        EXPECT_EQ(db_cache.get(), nullptr);
//        db_cache = cache.GetDb("db1", 5);
//        EXPECT_EQ(db_cache->commit_ts(), 4);
//
//        table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//        std::shared_ptr<MetaIndexCache> index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//        std::shared_ptr<MetaDbCache> db_cache = cache.GetDb("db1", 3);
//        EXPECT_EQ(db_cache.get(), nullptr);
//        db_cache = cache.GetDb("db1", 5);
//        EXPECT_EQ(db_cache.get(), nullptr);
//
//        std::shared_ptr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//
//        index_cache = cache.GetIndex(0, 0, "idx1", 9);
//        EXPECT_EQ(index_cache->commit_ts(), 8);
//        EXPECT_EQ(index_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//}
//
//TEST_F(MetaCacheTest, test_erase) {
//    MetaCache cache(3);
//    {
//        std::shared_ptr<MetaDbCache> db_cache = std::make_shared<MetaDbCache>("db1", 0, 4, "catalog|db|db1|4", false, 0);
//        cache.PutOrErase({db_cache}, nullptr);
//        db_cache = cache.GetDb("db1", 5);
//        EXPECT_EQ(db_cache->commit_ts(), 4);
//        EXPECT_EQ(db_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 1);
//    }
//    {
//        std::shared_ptr<MetaTableCache> table_cache = std::make_shared<MetaTableCache>(0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true, 0);
//        cache.PutOrErase({table_cache}, nullptr);
//        table_cache = cache.GetTable(0, "tbl1", 7);
//        EXPECT_EQ(table_cache->commit_ts(), 6);
//        EXPECT_EQ(table_cache->is_dropped(), true);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 2);
//    }
//    {
//        std::shared_ptr<MetaIndexCache> index_cache = std::make_shared<MetaIndexCache>(0, 0, "idx1", 0, 8, "catalog|idx|0|0|idx1|8", false, 0);
//        cache.PutOrErase({index_cache}, nullptr);
//        index_cache = cache.GetIndex(0, 0, "idx1", 9);
//        EXPECT_EQ(index_cache->commit_ts(), 8);
//        EXPECT_EQ(index_cache->is_dropped(), false);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 3);
//    }
//    {
//        std::shared_ptr<MetaEraseDbCache> drop_cache = std::make_shared<MetaEraseDbCache>("db1");
//        cache.Erase({drop_cache}, nullptr, 1);
//        std::shared_ptr<MetaDbCache> db_cache = cache.GetDb("db1", 5);
//        EXPECT_EQ(db_cache.get(), nullptr);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 3);
//    }
//    {
//        std::shared_ptr<MetaEraseTableCache> drop_cache = std::make_shared<MetaEraseTableCache>(0, "tbl1");
//        cache.Erase({drop_cache}, nullptr, 1);
//        std::shared_ptr<MetaTableCache> table_cache = cache.GetTable(0, "tbl1", 5);
//        EXPECT_EQ(table_cache.get(), nullptr);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 3);
//    }
//    {
//        std::shared_ptr<MetaEraseIndexCache> drop_cache = std::make_shared<MetaEraseIndexCache>(0, 0, "idx1");
//        cache.Erase({drop_cache}, nullptr, 1);
//        std::shared_ptr<MetaIndexCache> index_cache = cache.GetIndex(0, 0, "idx1", 5);
//        EXPECT_EQ(index_cache.get(), nullptr);
//        size_t size = cache.Size();
//        EXPECT_EQ(size, 3);
//    }
//}

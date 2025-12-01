// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:ut.test_high_cardinality;

import :ut.base_test;

import :new_txn_manager;
import :new_txn;
import third_party;
import :secondary_index_data;
import :roaring_bitmap;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :index_secondary;
import :table_index_meta;
import :segment_index_meta;
import :ut.request_test;

import data_type;
import logical_type;
import extra_ddl_info;
import column_def;
import embedding_info;
import sparse_info;
import internal_types;
import statement_common;
import constant_expr;

using namespace infinity;

class HighCardinalitySecondaryIndexTest : public NewRequestTest {
public:
    HighCardinalitySecondaryIndexTest() = default;
    ~HighCardinalitySecondaryIndexTest() = default;

protected:
    template <typename T>
    std::multimap<T, u32> CreateHighCardinalityData(u32 chunk_row_count, u32 unique_values = 5) {
        std::multimap<T, u32> test_data;

        for (u32 i = 0; i < chunk_row_count; ++i) {
            T key = static_cast<T>(i % unique_values);
            test_data.emplace(key, i);
        }

        return test_data;
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, HighCardinalitySecondaryIndexTest, ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(HighCardinalitySecondaryIndexTest, TestSaveLoadHighCardinality) {
    const u32 chunk_row_count = 10;
    const u32 unique_values = 5;

    auto test_data = CreateHighCardinalityData<i32>(chunk_row_count, unique_values);

    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto *index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, true);

    index->InsertData(&test_data);
    auto key_count = index->GetUniqueKeyCount();

    // Save data
    std::string tmp_path = fmt::format("{}/{}", "/var/infinity/tmp", "test001.idx");
    auto [file, status] = VirtualStore::Open(tmp_path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());
    index->SaveIndexInner(*file);

    auto handle = GetSecondaryIndexDataWithMeta(data_type, chunk_row_count, true, nullptr);
    auto index2 = static_cast<SecondaryIndexDataBase<HighCardinalityTag> *>(handle);

    // Load data
    auto [file2, status2] = VirtualStore::Open(tmp_path, FileAccessMode::kRead);
    EXPECT_TRUE(status2.ok());
    index2->ReadIndexInner(*file2);
    EXPECT_EQ(key_count, index2->GetUniqueKeyCount());
}

TEST_P(HighCardinalitySecondaryIndexTest, TestTxn) {
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_def =
        IndexSecondary::Make(std::make_shared<std::string>("second_idx"), std::make_shared<std::string>(), "my_file_name", {column_def1->name()});
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    const u32 chunk_row_count = 10;
    const u32 unique_values = 5;

    // Create table
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create index
    {
        auto *txn =
            txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_def->index_name_)), TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Just for coverage
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, "second_idx", db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        table_index_meta->SetSecondaryIndexCardinality(SecondaryIndexCardinality::kHighCardinality);

        auto handle = GetSecondaryIndexDataWithMeta(data_type, chunk_row_count, true, table_index_meta.get());
        auto index = static_cast<SecondaryIndexDataBase<HighCardinalityTag> *>(handle);

        auto test_data = CreateHighCardinalityData<i32>(chunk_row_count, unique_values);
        index->InsertData(&test_data);
    }
}
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

module infinity_core:ut.index_internal;

import :ut.base_test;

import third_party;
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
import :secondary_index_in_mem;
import :secondary_index_data;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meta;
import :catalog_meta;
import :mem_index;
import :secondary_index_in_mem;
import :ivf_index_data_in_mem;
import :emvb_index_in_mem;
import :memory_indexer;
import :hnsw_handler;
import :bmp_handler;
import :roaring_bitmap;
import :index_filter_evaluators;
import :index_emvb;

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

class TestTxnIndexInternal : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnIndexInternal,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnIndexInternal, test_index0) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto table_name = std::make_shared<std::string>("tb1");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);

    u32 block_row_cnt = 8192;
    auto input_block = std::make_shared<DataBlock>();
    {
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
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
        input_block->Finalize();
    }
    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto dump_index = [&](const std::string &index_name) {
        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", index_name)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append_a_block();

    dump_index(*index_name1);

    append_a_block();

    auto check_index = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index1"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({0}));
        }
        ChunkID chunk_id = 0;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });

    auto merge_index = [&](const std::string &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("merge index {}", index_name)), TransactionType::kOptimizeIndex);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto check_index2 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("check merged index {}", index_name)), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, 2 * block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({2}));
        }
        ChunkID chunk_id = 2;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    dump_index(*index_name1);

    append_a_block();

    merge_index(*index_name1);

    check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

TEST_P(TestTxnIndexInternal, SLOW_test_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def5 =
        std::make_shared<ColumnDef>(4, std::make_shared<DataType>(LogicalType::kTensor, column3_type_info), "col5", std::set<ConstraintType>());
    auto table_def =
        TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3, column_def4, column_def5});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
    auto index_name3 = std::make_shared<std::string>("index3");
    std::vector<InitParameter *> index3_parameters;
    index3_parameters.emplace_back(new InitParameter("metric", "l2"));
    index3_parameters.emplace_back(new InitParameter("plain_storage_data_type", "float"));
    auto index_def3 =
        IndexIVF::Make(index_name3, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def3->name()}, index3_parameters);
    auto index_name4 = std::make_shared<std::string>("index4");
    std::vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 =
        IndexHnsw::Make(index_name4, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    std::vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 =
        IndexBMP::Make(index_name5, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def4->name()}, index5_parameters);
    auto index_name6 = std::make_shared<std::string>("index6");
    std::vector<InitParameter *> index6_parameters;
    index6_parameters.emplace_back(new InitParameter("pq_subspace_num", "4"));
    index6_parameters.emplace_back(new InitParameter("pq_subspace_bits", "8"));
    auto index_def6 =
        IndexEMVB::Make(index_name6, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def5->name()}, index6_parameters);
    DeferFn defer_fn([&] {
        for (auto *parameter : index3_parameters) {
            delete parameter;
        }
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
        for (auto *parameter : index6_parameters) {
            delete parameter;
        }
    });
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);
    create_index(index_def3);
    create_index(index_def4);
    create_index(index_def5);
    create_index(index_def6);

    u32 block_row_cnt = 8192;
    auto input_block = std::make_shared<DataBlock>();
    {
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
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
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 2);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            std::pair<std::vector<float>, std::vector<int32_t>> vec{std::vector<float>{1.0, 2.0, 3.0, 4.0},
                                                                    std::vector<int32_t>{100, 1000, 10000, 20000}};
            std::pair<std::vector<float>, std::vector<int32_t>> vec2{std::vector<float>{1.0, 2.0, 3.0, 4.0},
                                                                     std::vector<int32_t>{100, 2000, 10000, 20000}};
            auto v1 = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                        reinterpret_cast<const char *>(vec.second.data()),
                                        vec.first.size(),
                                        column4_typeinfo);
            auto v2 = Value::MakeSparse(reinterpret_cast<const char *>(vec2.first.data()),
                                        reinterpret_cast<const char *>(vec2.second.data()),
                                        vec2.first.size(),
                                        column4_typeinfo);
            append_to_col(*col4, std::move(v1), std::move(v2));
            input_block->InsertVector(col4, 3);
        }
        {
            std::vector<float> vec1 = {1.0, 2.0, 3.0, 4.0};
            std::vector<float> vec2 = {5.0, 6.0, 7.0, 8.0};
            std::vector<float> vec3 = {9.0, 10.0, 11.0, 12.0};
            auto col5 = ColumnVector::Make(column_def5->type());
            col5->Initialize();

            std::vector<std::pair<char *, size_t>> embedding_data1;
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<char *>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
            Value v1 = Value::MakeTensor(embedding_data1, column3_type_info);

            std::vector<std::pair<char *, size_t>> embedding_data2;
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<char *>(vec3.data()), vec3.size() * sizeof(*vec3.data())));
            Value v2 = Value::MakeTensor(embedding_data2, column3_type_info);

            append_to_col(*col5, std::move(v1), std::move(v2));

            input_block->InsertVector(col5, 4);
        }
        input_block->Finalize();
    }
    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto dump_index = [&](const std::string &index_name) {
        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", index_name)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append_a_block();
    dump_index(*index_name1);
    dump_index(*index_name2);
    dump_index(*index_name3);
    dump_index(*index_name4);
    dump_index(*index_name5);
    dump_index(*index_name6);
    append_a_block();
    // new_txn_mgr->PrintAllKeyValue();

    auto check_index = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index1"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        // int32_t begin_val = 2;
        // int32_t end_val = 3;
        // {
        //     std::shared_ptr<SecondaryIndexInMem> memory_secondary_index = mem_index->GetSecondaryIndex();
        //     u32 segment_row_count = 2;
        //     Tuple<u32, int32_t, int32_t> arg_tuple = {segment_row_count, begin_val, end_val};
        //     [[maybe_unused]] Pair<u32, Bitmask> result = memory_secondary_index->RangeQuery(&arg_tuple);
        // }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({0}));
        }
        ChunkID chunk_id = 0;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        // {
        //     BufferHandle buffer_handle = buffer_obj->Load();
        //     auto *index = static_cast<const SecondaryIndexData *>(buffer_handle.GetData());

        //     [[maybe_unused]] const auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        //     [[maybe_unused]] const auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        // }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto fulltext_index = mem_index->GetFulltextIndex();
        RowID begin_id = fulltext_index->GetBeginRowID();
        u32 row_cnt = fulltext_index->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name3, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto ivf_index = mem_index->GetIVFIndex();
        RowID begin_id = ivf_index->GetBeginRowID();
        u32 row_cnt = ivf_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name4, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto hnsw_index = mem_index->GetHnswIndex();
        RowID begin_id = hnsw_index->GetBeginRowID();
        u32 row_cnt = hnsw_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name5, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto bmp_index = mem_index->GetBMPIndex();
        RowID begin_id = bmp_index->GetBeginRowID();
        u32 row_cnt = bmp_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name6, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto emvb_index = mem_index->GetEMVBIndex();
        RowID begin_id = emvb_index->GetBeginRowID();
        u32 row_cnt = emvb_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });

    auto merge_index = [&](const std::string &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("merge index {}", index_name)), TransactionType::kOptimizeIndex);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    auto check_index2 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("check merged index {}", index_name)), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, 2 * block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({2}));
        }
        ChunkID chunk_id = 2;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        // int32_t begin_val = 2;
        // int32_t end_val = 3;

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        // {
        //     BufferHandle buffer_handle = buffer_obj->Load();
        //     auto *index = static_cast<const SecondaryIndexData *>(buffer_handle.GetData());

        //     [[maybe_unused]] const auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        //     [[maybe_unused]] const auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        // }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    dump_index(*index_name1);
    dump_index(*index_name2);
    dump_index(*index_name3);
    dump_index(*index_name4);
    dump_index(*index_name5);
    dump_index(*index_name6);

    append_a_block();

    merge_index(*index_name1);
    merge_index(*index_name2);
    merge_index(*index_name3);
    merge_index(*index_name4);
    merge_index(*index_name5);
    merge_index(*index_name6);

    check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name2, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto fulltext_index = mem_index->GetFulltextIndex();
        RowID begin_id = fulltext_index->GetBeginRowID();
        u32 row_cnt = fulltext_index->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name3, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto ivf_index = mem_index->GetIVFIndex();
        RowID begin_id = ivf_index->GetBeginRowID();
        u32 row_cnt = ivf_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name4, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto hnsw_index = mem_index->GetHnswIndex();
        RowID begin_id = hnsw_index->GetBeginRowID();
        u32 row_cnt = hnsw_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name5, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto bmp_index = mem_index->GetBMPIndex();
        RowID begin_id = bmp_index->GetBeginRowID();
        u32 row_cnt = bmp_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index2(*index_name6, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto emvb_index = mem_index->GetEMVBIndex();
        RowID begin_id = emvb_index->GetBeginRowID();
        u32 row_cnt = emvb_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

TEST_P(TestTxnIndexInternal, test_populate_index0) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    u32 block_row_cnt = 8192;
    auto make_input_block = [&]() {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    auto check_index = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("check index {}", index_name)), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
            segment_id = 0;
        }
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        // std::shared_ptr<MemIndex> mem_index;
        // status = segment_index_meta.GetMemIndex(mem_index);
        // EXPECT_TRUE(status.ok());

        ChunkID chunk_id = 0;
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({0}));
            chunk_id = (*chunk_ids)[0];
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto fulltext_index = mem_index->GetFulltextIndex();
        RowID begin_id = fulltext_index->GetBeginRowID();
        u32 row_cnt = fulltext_index->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

TEST_P(TestTxnIndexInternal, SLOW_test_populate_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());
    auto column_def5 =
        std::make_shared<ColumnDef>(4, std::make_shared<DataType>(LogicalType::kTensor, column3_type_info), "col5", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def =
        TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3, column_def4, column_def5});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
    auto index_name3 = std::make_shared<std::string>("index3");
    std::vector<InitParameter *> index3_parameters;
    index3_parameters.emplace_back(new InitParameter("metric", "l2"));
    index3_parameters.emplace_back(new InitParameter("plain_storage_data_type", "float"));
    auto index_def3 =
        IndexIVF::Make(index_name3, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def3->name()}, index3_parameters);
    auto index_name4 = std::make_shared<std::string>("index4");
    std::vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 =
        IndexHnsw::Make(index_name4, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    std::vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 =
        IndexBMP::Make(index_name5, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def4->name()}, index5_parameters);
    auto index_name6 = std::make_shared<std::string>("index6");
    std::vector<InitParameter *> index6_parameters;
    index6_parameters.emplace_back(new InitParameter("pq_subspace_num", "4"));
    index6_parameters.emplace_back(new InitParameter("pq_subspace_bits", "8"));
    auto index_def6 =
        IndexEMVB::Make(index_name6, std::make_shared<std::string>(), "file_name", std::vector<std::string>{column_def5->name()}, index6_parameters);
    DeferFn defer_fn([&] {
        for (auto *parameter : index3_parameters) {
            delete parameter;
        }
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
        for (auto *parameter : index6_parameters) {
            delete parameter;
        }
    });
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    u32 block_row_cnt = 8192;
    auto input_block = std::make_shared<DataBlock>();
    {
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
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
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 2);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            std::pair<std::vector<float>, std::vector<int32_t>> vec{std::vector<float>{1.0, 2.0, 3.0, 4.0},
                                                                    std::vector<int32_t>{100, 1000, 10000, 20000}};
            std::pair<std::vector<float>, std::vector<int32_t>> vec2{std::vector<float>{1.0, 2.0, 3.0, 4.0},
                                                                     std::vector<int32_t>{100, 2000, 10000, 20000}};
            auto v1 = Value::MakeSparse(reinterpret_cast<const char *>(vec.first.data()),
                                        reinterpret_cast<const char *>(vec.second.data()),
                                        vec.first.size(),
                                        column4_typeinfo);
            auto v2 = Value::MakeSparse(reinterpret_cast<const char *>(vec2.first.data()),
                                        reinterpret_cast<const char *>(vec2.second.data()),
                                        vec2.first.size(),
                                        column4_typeinfo);
            append_to_col(*col4, std::move(v1), std::move(v2));
            input_block->InsertVector(col4, 3);
        }
        {
            std::vector<float> vec1 = {1.0, 2.0, 3.0, 4.0};
            std::vector<float> vec2 = {5.0, 6.0, 7.0, 8.0};
            std::vector<float> vec3 = {9.0, 10.0, 11.0, 12.0};
            auto col5 = ColumnVector::Make(column_def5->type());
            col5->Initialize();

            std::vector<std::pair<char *, size_t>> embedding_data1;
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data1.emplace_back(std::make_pair(reinterpret_cast<char *>(vec2.data()), vec2.size() * sizeof(*vec2.data())));
            Value v1 = Value::MakeTensor(embedding_data1, column3_type_info);

            std::vector<std::pair<char *, size_t>> embedding_data2;
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<char *>(vec1.data()), vec1.size() * sizeof(*vec1.data())));
            embedding_data2.emplace_back(std::make_pair(reinterpret_cast<char *>(vec3.data()), vec3.size() * sizeof(*vec3.data())));
            Value v2 = Value::MakeTensor(embedding_data2, column3_type_info);

            append_to_col(*col5, std::move(v1), std::move(v2));

            input_block->InsertVector(col5, 4);
        }
        input_block->Finalize();
    }
    auto append_a_block = [&] {
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };
    for (int i = 0; i < 2; ++i) {
        append_a_block();
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);
    create_index(index_def3);
    create_index(index_def4);
    create_index(index_def5);
    create_index(index_def6);
    append_a_block();

    auto check_index = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("check index {}", index_name)), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
            segment_id = 0;
        }
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, 2 * block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        ChunkID chunk_id = 0;
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_ids->size(), 1);
            chunk_id = chunk_ids->at(0);
            EXPECT_EQ(chunk_id, 0);
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        FileWorker *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto secondary_index = mem_index->GetSecondaryIndex();
        RowID begin_id = secondary_index->GetBeginRowID();
        u32 row_cnt = secondary_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto fulltext_index = mem_index->GetFulltextIndex();
        RowID begin_id = fulltext_index->GetBeginRowID();
        u32 row_cnt = fulltext_index->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name3, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto ivf_index = mem_index->GetIVFIndex();
        RowID begin_id = ivf_index->GetBeginRowID();
        u32 row_cnt = ivf_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name4, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto hnsw_index = mem_index->GetHnswIndex();
        RowID begin_id = hnsw_index->GetBeginRowID();
        u32 row_cnt = hnsw_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name5, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto bmp_index = mem_index->GetBMPIndex();
        RowID begin_id = bmp_index->GetBeginRowID();
        u32 row_cnt = bmp_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name6, [&](const std::shared_ptr<MemIndex> &mem_index) {
        auto emvb_index = mem_index->GetEMVBIndex();
        RowID begin_id = emvb_index->GetBeginRowID();
        u32 row_cnt = emvb_index->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
}

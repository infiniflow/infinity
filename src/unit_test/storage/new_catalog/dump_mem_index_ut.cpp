module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.dump_mem_index;

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

class TestTxnDumpMemIndex : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnDumpMemIndex,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnDumpMemIndex, dump_and_drop_db) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    // dump index and drop db
    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            auto secondary_index = mem_index->GetSecondaryIndex();
            RowID begin_id = secondary_index->GetBeginRowID();
            u32 row_cnt = secondary_index->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index                             commit (fail)
    //  |--------------|------------------------------------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index                             commit (fail)
    //  |--------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1                                  dump index                             commit (fail)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        ChunkID chunk_id = 0;
        std::vector<std::string> delete_file_paths;
        std::vector<std::string> exist_file_paths;
        status = txn->GetChunkIndexFilePaths(*db_name, *table_name, *index_name1, segment_id, chunk_id, delete_file_paths);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        CheckFilePaths(delete_file_paths, exist_file_paths);

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //                                 t1                    dump index                             commit (fail)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_table) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    // dump index and drop db
    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index                             commit (fail)
    //  |--------------|------------------------------------------|
    //                         |------------------|----------|
    //                        t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get table"), TransactionType::kRead);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index                             commit (fail)
    //  |--------------|------------------------------------------|
    //         |------------------|-------------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get table"), TransactionType::kRead);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1                                  dump index                             commit (fail)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get table"), TransactionType::kRead);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //                                 t1                    dump index                             commit (fail)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get table"), TransactionType::kRead);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get table"), TransactionType::kRead);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_add_column) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());
    auto column_def5 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    auto input_block_4_columns = std::make_shared<DataBlock>();
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
            input_block_4_columns->InsertVector(col1, 0);
        }
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block_4_columns->InsertVector(col3, 1);
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
            input_block_4_columns->InsertVector(col4, 2);
        }
        {
            auto col5 = ColumnVector::Make(column_def5->type());
            col5->Initialize();
            append_to_col(*col5, Value::Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block_4_columns->InsertVector(col5, 3);
        }
        input_block_4_columns->Finalize();
    }
    auto append_a_block_after_add_column = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block_4_columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                add column    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                add column  commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block_after_add_column();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                add column        commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                add column        commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                add column       commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //  t1                                  dump index                               commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                add column         commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                add column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                add column    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_column) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    auto input_block_2_columns = std::make_shared<DataBlock>();
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
            input_block_2_columns->InsertVector(col1, 0);
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
            input_block_2_columns->InsertVector(col4, 1);
        }
        input_block_2_columns->Finalize();
    }
    auto append_a_block_after_drop_column = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block_2_columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                drop column    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                drop column  commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block_after_drop_column();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                drop column     commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block_after_drop_column();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                drop column       commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block_after_drop_column();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                drop column      commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block_after_drop_column();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1                                  dump index                            commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                drop column       commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                drop column  commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        std::vector<std::string> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop column    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2                drop column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_rename_table) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                rename    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                rename  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                rename            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                rename              commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                rename              commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                                 commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                rename            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                rename    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                rename    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2                rename  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_create_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                rename    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // create index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_def1->index_name_)),
                                           TransactionType::kCreateIndex);
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

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
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            append_to_col(*col3,
                          Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                          Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
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
            input_block->InsertVector(col4, 2);
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

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                drop index  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                drop index  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                drop index          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (fail)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                drop index             commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (fail)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                drop index            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                                 commit (fail)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                drop index          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    //  t1                                  dump index                             commit (fail)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2               drop index   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (fail)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2               drop index  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2              drop index   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_import) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> std::shared_ptr<DataBlock> {
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
                auto col3 = ColumnVector::Make(column_def3->type());
                col3->Initialize();
                append_to_col(*col3,
                              Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
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
                input_block->InsertVector(col4, 2);
            }
            input_block->Finalize();
        }
        return input_block;
    };

    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index2 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0, 1}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index3 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0, 1}));
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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                import    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                import     commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                 import          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                 import             commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        //        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr);
        //        });

        append_a_block();

        //        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        //            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
        //            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
        //            return std::make_pair(begin_id, row_cnt);
        //        });

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                import              commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1                                  dump index (fail)                           rollback (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                import        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                import  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                import    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2                drop column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            auto secondary_index = mem_index->GetSecondaryIndex();
            RowID begin_id = secondary_index->GetBeginRowID();
            u32 row_cnt = secondary_index->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_append) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> std::shared_ptr<DataBlock> {
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
                auto col3 = ColumnVector::Make(column_def3->type());
                col3->Initialize();
                append_to_col(*col3,
                              Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
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
                input_block->InsertVector(col4, 2);
            }
            input_block->Finalize();
        }
        return input_block;
    };

    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index3 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
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
            EXPECT_EQ(chunk_info->row_cnt_, 2 * block_row_cnt);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                append    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() != nullptr); });

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                append     commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2           append                commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                 append             commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // Dump index only dumps rows before append, the rows appended by append still exist in mem index.
        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                append               commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // Dump index only dumps rows before append, the rows appended by append still exist in mem index.
        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1                                  dump index                            commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                append        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // Dump index only dumps rows before append, the rows appended by append still exist in mem index.
        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1                                  dump index                                  commit
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                append    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                append    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2               append     commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, dump_and_delete) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> std::shared_ptr<DataBlock> {
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
                auto col3 = ColumnVector::Make(column_def3->type());
                col3->Initialize();
                append_to_col(*col3,
                              Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
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
                input_block->InsertVector(col4, 2);
            }
            input_block->Finalize();
        }
        return input_block;
    };

    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            size_t row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|----------|
    //                                    t2                delete    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                delete     commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                delete            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                 delete          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                delete           commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //  t1                                            dump index                              commit
    //  |-------------------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2               delete        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //                  t1                                  dump index                                  commit
    //                  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2               delete    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                delete    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }

    //                                            t1                    dump index                             commit (success)
    //                                            |----------------------|------------------------------------------|
    //         |------------------|------------|
    //         t2               delete     commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, SLOW_dump_and_dump) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});

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

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const std::string &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const std::string &db_name, const std::shared_ptr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const std::string &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> std::shared_ptr<DataBlock> {
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
                auto col3 = ColumnVector::Make(column_def3->type());
                col3->Initialize();
                append_to_col(*col3,
                              Value::MakeEmbedding(std::vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(std::vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
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
                input_block->InsertVector(col4, 2);
            }
            input_block->Finalize();
        }
        return input_block;
    };

    auto append_a_block = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem index {}", index_name)),
    //        TransactionType::kDumpMemIndex); SegmentID segment_id = 0; Status status = txn->DumpMemIndex(*db_name, *table_name, index_name,
    //        segment_id); EXPECT_TRUE(status.ok()); status = new_txn_mgr->CommitTxn(txn); EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const std::string &index_name, std::function<void(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const std::string &index_name, std::function<std::pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> check_mem_index) {
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

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|-------------|
    //                                    t2            dump (success)   commit (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                dump     commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                 dump          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                 dump             commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                dump              commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);
        SegmentID segment_id = 0;

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }

    //             t1                         dump index                           commit (success)
    //             |-----------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                dump        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn1 =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kDumpMemIndex);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->GetSecondaryIndex() == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
            u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);
    }
}

TEST_P(TestTxnDumpMemIndex, SLOW_test_dump_index_and_optimize_index) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, const Value &v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    auto PrepareForDumpAndOptimize = [&] {
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
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For optimize
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("append {}", i)), TransactionType::kAppend);
                std::shared_ptr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }

            //            new_txn_mgr->PrintAllKeyValue();
            {
                auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump index {}", i)), TransactionType::kDumpMemIndex);
                Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            std::shared_ptr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const std::vector<SegmentID> &segment_ids, const std::vector<ChunkID> &chunk_ids) { //
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<TableIndexMeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, std::vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        std::vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, chunk_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kDropDB);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      dump      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------------|
    //                           t2                  optimize index     commit (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {3});

        DropDB();
    }

    //    t1      dump      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             optimize (fail)     commit
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();
    }

    //    t1      dump index                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    optimize fail        rollback (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();
    }

    //    t1      dump index                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize fail          rollback (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();
    }

    //    t1                                      dump index fail                               rollback (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                optimize                     commit (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});

        DropDB();
    }

    //    t1                                                       dump index (fail)                       Rollback (success)
    //    |-----------------------------------------------------------|------------------------------------------|
    //                    |----------------------|--------------|
    //                    t2                  optimize index   commit (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});

        DropDB();
    }

    //                                                  t1                  dump                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  optimize   commit (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});

        DropDB();
    }

    //                                                           t1                  dump                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           optimize index   commit (success)
    {
        PrepareForDumpAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index {}"), TransactionType::kDumpMemIndex);
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2, 3});

        DropDB();
    }
}

TEST_P(TestTxnDumpMemIndex, test_dump_index_and_compact) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, const Value &v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    auto PrepareForCompactAndOptimize = [&] {
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
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For compact
        for (int i = 0; i < 2; ++i) {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("import {}", i)), TransactionType::kImport);
            std::vector<std::shared_ptr<DataBlock>> input_blocks = {
                make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For dump index
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            std::shared_ptr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const std::vector<SegmentID> &segment_ids, SegmentID dump_segment_id, const std::vector<ChunkID> &chunk_ids) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<TableIndexMeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, segment_ids);

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[dump_segment_id], *table_index_meta);
        std::vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, chunk_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kDropDB);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      compact      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------------|
    //                           t2                  dump index     commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //    t1      compact      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             dump            commit
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //    t1      compact                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    dump              commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //    t1      compact                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        dump          commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //    t1                                      compact                               commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                dump                     commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //    t1                                                       compact                                   commit (success)
    //    |-----------------------------------------------------------|------------------------------------------|
    //                    |----------------------|--------------|
    //                    t2                  dump       commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //                                                  t1                  compact                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  dump   commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }

    //                                                           t1                  compact                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           dump   commit (success)
    {
        PrepareForCompactAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kDumpMemIndex);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3}, 0, {0});

        DropDB();
    }
}

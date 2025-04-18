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

class TestTxnDumpMemIndex : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnDumpMemIndex,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnDumpMemIndex, dump_and_drop_db) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //                         |------------------|----------|
    //                        t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kNormal);
        auto [db_info, db_status] = txn7->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(db_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    // dump index and drop db
    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop db    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kNormal);
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
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_table) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //                         |------------------|----------|
    //                        t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|-------------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    // dump index and drop db
    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2                drop table    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
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
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn5->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
        auto [table_info, table_status] = txn7->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(table_status.ok());
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_add_column) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                add column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                add column (fail)  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                add column (fail)   rollback
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                add column (fail)    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // add table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                  dump index (fail)                           rollback
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                add column         commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // add column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
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

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col5", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns4;
        columns4.emplace_back(column_def4);
        Status status = txn4->AddColumns(*db_name, *table_name, columns4);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_column) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                drop column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        Vector<String> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                drop column (fail)  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                drop column (fail)   rollback
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                drop column (fail)    commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col3");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                  dump index (fail)                           rollback (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                drop column       commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2                drop column  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Vector<String> column_names;
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

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col3");
        Status status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_rename_table) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // rename
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        Status status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_create_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // create index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_def1->index_name_)), TransactionType::kNormal);
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_drop_index) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                drop index             commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                drop index            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                  dump index                                 commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                drop index          commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                  dump index                             commit (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|-----------|
    //         t2               drop index   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                 t1                    dump index                             commit (success)
    //                                 |----------------------|------------------------------------------|
    //         |------------------|----------|
    //         t2               drop index  commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        // drop index
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_import) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> SharedPtr<DataBlock> {
        auto input_block = MakeShared<DataBlock>();
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
                              Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
            }
            {
                auto col4 = ColumnVector::Make(column_def4->type());
                col4->Initialize();
                Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
                Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index2 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index3 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        //        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr);
        //        });

        append_a_block();

        //        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
        //            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
        //            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
        //            return std::make_pair(begin_id, row_cnt);
        //        });

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
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

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
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

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_block()};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_append) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> SharedPtr<DataBlock> {
        auto input_block = MakeShared<DataBlock>();
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
                              Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
            }
            {
                auto col4 = ColumnVector::Make(column_def4->type());
                col4->Initialize();
                Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
                Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index2 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({}));
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index3 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                         |------------------|----------|
    //                        t2                append     commit (fail)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    //  t1            dump index                 commit (success)
    //  |--------------|------------------------------|
    //                         |------------------|------------------|
    //                        t2                append            commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    //  t1            dump index                                               commit (success)
    //  |--------------|--------------------------------------------------------------|
    //                         |------------------|--------------------|
    //                        t2                 append(fail)         rollback
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    //  t1            dump index                             commit (success)
    //  |--------------|------------------------------------------|
    //         |------------------|----------------------|
    //         t2                append(fail)           rollback
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    //  t1                                  dump index (fail)                           rollback (success)
    //  |---------------------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                append        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index2(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ != nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn3->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index3(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_delete) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> SharedPtr<DataBlock> {
        auto input_block = MakeShared<DataBlock>();
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
                              Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
            }
            {
                auto col4 = ColumnVector::Make(column_def4->type());
                col4->Initialize();
                Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
                Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
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

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
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

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
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

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
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
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        // Check data
        check_data();

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        EXPECT_EQ(new_catalog->GetTableReferenceCountForMemIndex(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_lock_table) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;
    auto input_block = MakeShared<DataBlock>();
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
            append_to_col(*col3, Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}), Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
            input_block->InsertVector(col3, 1);
        }
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
            Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, 0);
            EXPECT_EQ(row_cnt, 2 * block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({}));
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|-------------|
    //                                    t2                lock table   commit
    //                                                                               |------------------|---------------|
    //                                                                              t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index               commit (success)
    //  |--------------|------------------------------|
    //                                     |------------------|-------------|
    //                                    t2                lock table   commit
    //                                                                               |------------------|---------------|
    //                                                                              t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                            commit (success)
    //  |--------------|----------------------------------------|
    //                                     |------------------|--------------------|
    //                                    t2                lock table (fail)   rollback
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        status = txn6->LockTable(*db_name, *table_name);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1            dump index                                                         commit (success)
    //  |--------------|-------------------------------------------------------------------------|
    //         |------------------|-------------|
    //        t2                lock table   commit
    //                                              |------------------|---------------|
    //                                             t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn6->LockTable(*db_name, *table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        //        // unlock table
        //        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        //        status = txn7->UnlockTable(*db_name, *table_name);
        //        EXPECT_TRUE(status.ok());
        //        status = new_txn_mgr->CommitTxn(txn7);
        //        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                             dump index (fail)                                                         rollback (success)
    //  |---------------------------------|-------------------------------------------------------------------------|
    //         |------------------|-------------|
    //        t2                lock table   commit
    //                                              |------------------|---------------|
    //                                             t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        Status status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                            dump index (fail)                                rollback (success)
    //  |------------------------------------------------------|------------------------------------------------|
    //         |------------------|-------------|
    //        t2                lock table   commit
    //                                              |------------------|---------------|
    //                                             t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        Status status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                                           dump index (fail)                                rollback (success)
    //  |--------------------------------------------------------------------|------------------------------------------------|
    //         |------------------|-------------|
    //        t2                lock table   commit
    //                                              |------------------|---------------|
    //                                             t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        Status status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //  t1                                                                          dump index                  commit (success)
    //  |--------------------------------------------------------------------------------|---------------------------|
    //         |------------------|-------------|
    //        t2                lock table   commit
    //                                              |------------------|---------------|
    //                                             t3                unlock table   commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        // lock table again
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("lock table"), TransactionType::kNormal);
        Status status = txn6->LockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // unlock table
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("unlock table"), TransactionType::kNormal);
        status = txn7->UnlockTable(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, dump_and_dump) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    //    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column3_type_info = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);
    auto column_def3 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col3", std::set<ConstraintType>());
    auto column4_typeinfo = MakeShared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, 30000, SparseStoreType::kSort);
    auto column_def4 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kSparse, column4_typeinfo), "col4", std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def3, column_def4});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    //    auto index_name2 = std::make_shared<String>("index2");
    //    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto index_name4 = std::make_shared<std::string>("index4");
    Vector<InitParameter *> index4_parameters;
    index4_parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def4 = IndexHnsw::Make(index_name4, MakeShared<String>(), "file_name", Vector<String>{column_def3->name()}, index4_parameters);
    auto index_name5 = std::make_shared<std::string>("index5");
    Vector<InitParameter *> index5_parameters;
    index5_parameters.emplace_back(new InitParameter("block_size", "16"));
    index5_parameters.emplace_back(new InitParameter("compress_type", "compress"));
    auto index_def5 = IndexBMP::Make(index_name5, MakeShared<String>(), "file_name", Vector<String>{column_def4->name()}, index5_parameters);

    DeferFn defer_fn([&] {
        for (auto *parameter : index4_parameters) {
            delete parameter;
        }
        for (auto *parameter : index5_parameters) {
            delete parameter;
        }
    });

    auto create_db = [&](const String &db_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_table = [&](const String &db_name, const SharedPtr<TableDef> &table_def) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto drop_db = [&](const String &db_name) {
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn6->DropDatabase(db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    };

    //    create_index(index_def4);
    //    create_index(index_def5);

    u32 block_row_cnt = 8192;

    auto make_block = [&]() -> SharedPtr<DataBlock> {
        auto input_block = MakeShared<DataBlock>();
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
                              Value::MakeEmbedding(Vector<float>{1.0, 2.0, 3.0, 4.0}),
                              Value::MakeEmbedding(Vector<float>{5.0, 6.0, 7.0, 8.0}));
                input_block->InsertVector(col3, 1);
            }
            {
                auto col4 = ColumnVector::Make(column_def4->type());
                col4->Initialize();
                Pair<Vector<float>, Vector<int32_t>> vec{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 1000, 10000, 20000}};
                Pair<Vector<float>, Vector<int32_t>> vec2{Vector<float>{1.0, 2.0, 3.0, 4.0}, Vector<int32_t>{100, 2000, 10000, 20000}};
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //    auto dump_index = [&](const String &index_name) {
    //        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", index_name)), TransactionType::kNormal);
    //        SegmentID segment_id = 0;
    //        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
    //        EXPECT_TRUE(status.ok());
    //        status = new_txn_mgr->CommitTxn(txn);
    //        EXPECT_TRUE(status.ok());
    //    };

    auto check_index0 = [&](const String &index_name, std::function<void(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        check_mem_index(mem_index);
        //        {
        //            auto [row_id, row_cnt] = check_mem_index(mem_index);
        //            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
        //            EXPECT_EQ(row_cnt, block_row_cnt);
        //        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto check_index1 = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        {
            auto [row_id, row_cnt] = check_mem_index(mem_index);
            EXPECT_EQ(row_id, RowID(0, block_row_cnt));
            EXPECT_EQ(row_cnt, block_row_cnt);
        }

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
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

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    //  t1            dump index   commit (success)
    //  |--------------|---------------|
    //                                     |------------------|-------------|
    //                                    t2                dump (fail)   rollback (success)
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);
        SegmentID segment_id = 0;

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //             t1                         dump index (fail)                           rollback (success)
    //             |-----------------------------|------------------------------------------|
    //         |------------------|--------------------|
    //         t2                dump        commit
    {
        create_db(*db_name);
        create_table(*db_name, table_def);
        create_index(index_def1);

        append_a_block();

        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump mem index {}", *index_name1)), TransactionType::kNormal);

        SegmentID segment_id = 0;
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_TRUE(status.ok());

        status = txn1->DumpMemIndex(*db_name, *table_name, *index_name1, segment_id);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        check_index0(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) { EXPECT_TRUE(mem_index->memory_secondary_index_ == nullptr); });

        append_a_block();

        check_index1(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
            RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
            u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
            return std::make_pair(begin_id, row_cnt);
        });

        drop_db(*db_name);

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, test_dump_index_and_optimize_index) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

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
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For optimize
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("append {}", i)), TransactionType::kNormal);
                SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }

            //            new_txn_mgr->PrintAllKeyValue();
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const Vector<SegmentID> &segment_ids, const Vector<ChunkID> &chunk_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        status = table_index_meta->GetSegmentIDs(index_segment_ids_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, chunk_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {3});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      dump      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             optimize (fail)     commit
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      dump index                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    optimize fail        rollback (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      dump index                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize fail          rollback (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {0, 1, 2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      dump index fail                               rollback (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                optimize                     commit (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                       dump index                                   commit (success)
    //    |-----------------------------------------------------------|------------------------------------------|
    //                    |----------------------|--------------|
    //                    t2                  optimize index   commit (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  dump                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  optimize   commit (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  dump                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           optimize index   commit (success)
    {
        PrepareForDumpAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index {}"), TransactionType::kNormal);
        status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({0}, {2, 3});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDumpMemIndex, test_dump_index_and_compact) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

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
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For compact
        for (int i = 0; i < 2; ++i) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For dump index
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const Vector<SegmentID> &segment_ids, SegmentID dump_segment_id, const Vector<ChunkID> &chunk_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        status = table_index_meta->GetSegmentIDs(index_segment_ids_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, segment_ids);

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[dump_segment_id], *table_index_meta);
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, chunk_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      compact      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             dump            commit
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      compact                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    dump              commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      compact                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        dump          commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      compact                               commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                dump                     commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                       compact                                   commit (success)
    //    |-----------------------------------------------------------|------------------------------------------|
    //                    |----------------------|--------------|
    //                    t2                  dump       commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  compact                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  dump   commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  compact                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           dump   commit (success)
    {
        PrepareForCompactAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->DumpMemIndex(*db_name, *table_name, *index_name1, 2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        CheckTable({3}, 0, {0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

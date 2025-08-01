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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.wal_entry;

import :ut.base_test;
import :infinity_context;
import :infinity_exception;
import :stl;
import :third_party;
import :logger;
import :table_def;
import :wal_entry;
import :value;
import :data_block;
import :default_values;
import :index_base;
import :index_ivf;
import :persistence_manager;
import :kv_store;
import :status;
#endif

import global_resource_usage;
import logical_type;
import internal_types;
import column_def;
import statement_common;
import data_type;
import embedding_info;

using namespace infinity;

class WalEntryTest : public BaseTest {};

SharedPtr<TableDef> MockTableDesc2() {
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    return MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
}

WalSegmentInfo MakeSegmentInfo(SizeT row_count, TxnTimeStamp commit_ts, SizeT column_count) {
    WalSegmentInfo segment_info;
    segment_info.segment_id_ = 0;
    segment_info.column_count_ = column_count;
    segment_info.actual_row_count_ = segment_info.row_count_ = row_count;
    segment_info.row_capacity_ = row_count * 2;
    Vector<WalBlockInfo> block_infos_;
    {
        WalBlockInfo block_info;
        block_info.block_id_ = 0;
        block_info.row_count_ = row_count;
        block_info.row_capacity_ = row_count;
        Vector<Pair<u32, u64>> outline_info;
        outline_info.resize(column_count);
        block_info.outline_infos_ = std::move(outline_info);
    }
    segment_info.block_infos_ = std::move(block_infos_);
    return segment_info;
}

void MockWalFile(const String &wal_file_path, const String &ckp_file_path, const String &ckp_file_name) {
    for (int commit_ts = 0; commit_ts < 3; ++commit_ts) {
        SizeT row_count = DEFAULT_VECTOR_SIZE;

        auto entry = MakeShared<WalEntry>();
        entry->cmds_.push_back(MakeShared<WalCmdCreateDatabaseV2>("default2", "1", "default2_comment"));
        entry->cmds_.push_back(MakeShared<WalCmdCreateTableV2>("default_db", "0", "1", MockTableDesc2()));
        WalSegmentInfo segment_info = MakeSegmentInfo(row_count, commit_ts, 2);
        entry->cmds_.push_back(MakeShared<WalCmdImportV2>("default_db", "0", "tbl1", "1", std::move(segment_info)));

        auto data_block = DataBlock::Make();
        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
        data_block->Init(column_types);
        for (SizeT i = 0; i < row_count; ++i) {
            data_block->AppendValue(0, Value::MakeBool(i % 2 == 0));
            data_block->AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block->Finalize();

        RowID start_row(0, 0);
        entry->cmds_.push_back(
            MakeShared<WalCmdAppendV2>("db1", "2", "tbl1", "1", Vector<Pair<RowID, u64>>{Pair<RowID, u64>{start_row, 8192}}, data_block));
        entry->commit_ts_ = commit_ts;

        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            String error_message = fmt::format("Failed to open wal file: {}", wal_file_path);
            UnrecoverableError(error_message);
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = MakeShared<WalEntry>();
        Vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        Vector<SegmentID> deprecated_segment_ids{0, 1, 2};
        entry->cmds_.push_back(MakeShared<WalCmdCompactV2>("db1", "2", "tbl1", "1", std::move(new_segment_infos), std::move(deprecated_segment_ids)));
        entry->commit_ts_ = 5;
        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            String error_message = fmt::format("Failed to open wal file: {}", wal_file_path);
            UnrecoverableError(error_message);
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = MakeShared<WalEntry>();
        entry->cmds_.push_back(MakeShared<WalCmdCheckpointV2>(int64_t(123)));
        entry->commit_ts_ = 3;
        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            String error_message = fmt::format("Failed to open wal file: {}", wal_file_path);
            UnrecoverableError(error_message);
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = MakeShared<WalEntry>();
        entry->cmds_.push_back(MakeShared<WalCmdDropTableV2>("db1", "2", "tbl1", "1", 1, "table_key"));
        entry->commit_ts_ = 4;
        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            String error_message = fmt::format("Failed to open wal file: {}", wal_file_path);
            UnrecoverableError(error_message);
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
}

TEST_F(WalEntryTest, ReadWrite) {
    // Earlier cases may leave a dirty infinity instance. Destroy it first.
    infinity::InfinityContext::instance().UnInit();
    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(nullptr);
    infinity::InfinityContext::instance().InitPhase2();
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds_.push_back(MakeShared<WalCmdCreateDatabase>("db1", "default2_comment", "AAA_db1"));
    entry->cmds_.push_back(MakeShared<WalCmdDropDatabase>("db1"));
    entry->cmds_.push_back(MakeShared<WalCmdCreateTable>("db1", "BBB_tb1", MockTableDesc2()));
    entry->cmds_.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
    {
        WalSegmentInfo segment_info = MakeSegmentInfo(100, 8, 2);
        entry->cmds_.push_back(MakeShared<WalCmdImport>("db1", "tbl1", std::move(segment_info)));
    }
    {
        Vector<InitParameter *> parameters = {new InitParameter("metric", "ip")};
        SharedPtr<String> index_name = MakeShared<String>("idx1");
        auto index_base = IndexIVF::Make(index_name, MakeShared<String>("test comment"), "idx1_tbl1", Vector<String>{"col1", "col2"}, parameters);
        for (auto parameter : parameters) {
            delete parameter;
        }
        entry->cmds_.push_back(MakeShared<WalCmdCreateIndex>("db1", "tbl1", "CCC_idx1", index_base));
    }
    entry->cmds_.push_back(MakeShared<WalCmdDropIndex>("db1", "tbl1", "idx1"));
    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        data_block->Init(column_types);
        for (SizeT i = 0; i < row_count; ++i) {
            data_block->AppendValue(0, Value::MakeBool(i % 2 == 0));
            data_block->AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block->Finalize();

        entry->cmds_.push_back(MakeShared<WalCmdAppend>("db1", "tbl1", data_block));
    }
    {
        Vector<RowID> row_ids = {RowID(1, 3)};
        entry->cmds_.push_back(MakeShared<WalCmdDelete>("db1", "tbl1", row_ids));
    }
    entry->cmds_.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), "catalog", String("META_123.full.json")));
    {
        Vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        entry->cmds_.push_back(MakeShared<WalCmdCompact>("db1", "tbl1", std::move(new_segment_infos), Vector<SegmentID>{0, 1, 2}));
    }
    {
        WalChunkIndexInfo info;
        info.chunk_id_ = 2;
        info.base_name_ = "base_name";
        info.base_rowid_ = RowID(0, 0);
        info.row_count_ = 4;
        info.deprecate_ts_ = 0;
        Vector<WalChunkIndexInfo> chunk_infos{info};
        Vector<ChunkID> deprecate_ids{0, 1};
        entry->cmds_.push_back(MakeShared<WalCmdDumpIndex>("db1", "tbl1", "idx1", 0 /*segment_id*/, chunk_infos, deprecate_ids));
    }
    {
        entry->cmds_.push_back(MakeShared<WalCmdRenameTable>("db1", "tbl1", "tbl2"));
    }
    {
        Vector<SharedPtr<ColumnDef>> column_defs;
        std::set<ConstraintType> constraints;

        auto column_def3 = MakeShared<ColumnDef>(3 /*column_id*/, MakeShared<DataType>(LogicalType::kBoolean), "boolean_col", constraints);
        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
        auto column_def4 =
            MakeShared<ColumnDef>(4 /*column id*/, MakeShared<DataType>(LogicalType::kEmbedding, embedding_info), "embedding_col", constraints);

        column_defs.push_back(column_def3);
        column_defs.push_back(column_def4);
        entry->cmds_.push_back(MakeShared<WalCmdAddColumns>("db1", "tbl1", std::move(column_defs)));
    }
    {
        Vector<String> column_names;
        column_names.push_back("boolean_col");
        column_names.push_back("embedding_col");
        entry->cmds_.push_back(MakeShared<WalCmdDropColumns>("db1", "tbl1", std::move(column_names)));
    }

    i32 exp_size = entry->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    entry->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    SharedPtr<WalEntry> entry2 = WalEntry::ReadAdv(ptr_r, exp_size);
    EXPECT_NE(entry2, nullptr);
    EXPECT_EQ(*entry == *entry2, true);
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(WalEntryTest, ReadWriteV2) {
    // Earlier cases may leave a dirty infinity instance. Destroy it first.
    infinity::InfinityContext::instance().UnInit();
    RemoveDbDirs();
    auto config_path = std::make_shared<std::string>(BaseTestNoParam::NEW_CONFIG_PATH);
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds_.push_back(MakeShared<WalCmdCreateDatabaseV2>("db1", "1", "default2_comment"));
    entry->cmds_.push_back(MakeShared<WalCmdDropDatabaseV2>("db1", "1", 1));
    entry->cmds_.push_back(MakeShared<WalCmdCreateTableV2>("db1", "1", "2", MockTableDesc2()));
    entry->cmds_.push_back(MakeShared<WalCmdDropTableV2>("db1", "1", "tbl1", "2", 1, "table_key"));
    {
        WalSegmentInfo segment_info = MakeSegmentInfo(100, 8, 2);
        entry->cmds_.push_back(MakeShared<WalCmdImportV2>("db1", "1", "tbl1", "2", std::move(segment_info)));
    }
    {
        Vector<InitParameter *> parameters = {new InitParameter("metric", "ip")};
        SharedPtr<String> index_name = MakeShared<String>("idx1");
        auto index_base = IndexIVF::Make(index_name, MakeShared<String>("test comment"), "idx1_tbl1", Vector<String>{"col1", "col2"}, parameters);
        for (auto parameter : parameters) {
            delete parameter;
        }
        entry->cmds_.push_back(MakeShared<WalCmdCreateIndexV2>("db1", "1", "tbl1", "2", "3", index_base, "table_key"));
    }
    entry->cmds_.push_back(MakeShared<WalCmdDropIndexV2>("db1", "1", "tbl1", "2", "idx1", "3", 1, "index_key"));
    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        data_block->Init(column_types);
        for (SizeT i = 0; i < row_count; ++i) {
            data_block->AppendValue(0, Value::MakeBool(i % 2 == 0));
            data_block->AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block->Finalize();
        Vector<Pair<RowID, u64>> row_ranges = {{RowID(0, 0), row_count}};
        entry->cmds_.push_back(MakeShared<WalCmdAppendV2>("db1", "1", "tbl1", "2", row_ranges, data_block));
    }
    {
        Vector<RowID> row_ids = {RowID(1, 3)};
        entry->cmds_.push_back(MakeShared<WalCmdDeleteV2>("db1", "1", "tbl1", "2", row_ids));
    }
    entry->cmds_.push_back(MakeShared<WalCmdCheckpointV2>(int64_t(123)));
    {
        Vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        entry->cmds_.push_back(MakeShared<WalCmdCompactV2>("db1", "1", "tbl1", "2", std::move(new_segment_infos), Vector<SegmentID>{0, 1, 2}));
    }
    {
        WalChunkIndexInfo info;
        info.chunk_id_ = 2;
        info.base_name_ = "base_name";
        info.base_rowid_ = RowID(0, 0);
        info.row_count_ = 4;
        info.deprecate_ts_ = 0;
        Vector<WalChunkIndexInfo> chunk_infos{info};
        Vector<ChunkID> deprecate_ids{0, 1};
        entry->cmds_.push_back(
            MakeShared<WalCmdDumpIndexV2>("db1", "1", "tbl1", "2", "idx1", "3", 0 /*segment_id*/, chunk_infos, deprecate_ids, "table_key"));
    }
    {
        entry->cmds_.push_back(MakeShared<WalCmdRenameTableV2>("db1", "1", "tbl1", "2", "tbl2", "old_table_key"));
    }
    {
        Vector<SharedPtr<ColumnDef>> column_defs;
        std::set<ConstraintType> constraints;

        auto column_def3 = MakeShared<ColumnDef>(3 /*column_id*/, MakeShared<DataType>(LogicalType::kBoolean), "boolean_col", constraints);
        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
        auto column_def4 =
            MakeShared<ColumnDef>(4 /*column id*/, MakeShared<DataType>(LogicalType::kEmbedding, embedding_info), "embedding_col", constraints);

        column_defs.push_back(column_def3);
        column_defs.push_back(column_def4);
        entry->cmds_.push_back(MakeShared<WalCmdAddColumnsV2>("db1", "1", "tbl1", "2", std::move(column_defs), "table_key"));
    }
    {
        Vector<String> column_names;
        column_names.push_back("boolean_col");
        column_names.push_back("embedding_col");
        Vector<String> column_keys;
        column_keys.push_back("column_key1");
        column_keys.push_back("column_key2");
        entry->cmds_.push_back(MakeShared<WalCmdDropColumnsV2>("db1",
                                                               "1",
                                                               "tbl1",
                                                               "2",
                                                               std::move(column_names),
                                                               Vector<ColumnID>{3, 4},
                                                               "table_key",
                                                               std::move(column_keys)));
    }

    i32 exp_size = entry->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    entry->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    SharedPtr<WalEntry> entry2 = WalEntry::ReadAdv(ptr_r, exp_size);
    EXPECT_NE(entry2, nullptr);
    EXPECT_EQ(*entry == *entry2, true);
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(WalEntryTest, ReadWriteVFS) {
    RemoveDbDirs();
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();

    Vector<String> paths = {"path1", "path2"};
    String workspace = GetFullPersistDir();
    String data_dir = GetFullDataDir();
    SizeT object_size_limit = 100;
    auto kv_store = MakeUnique<KVStore>();
    Status status = kv_store->Init(GetCatalogDir());
    EXPECT_TRUE(status.ok());
    PersistenceManager pm(workspace, data_dir, object_size_limit);
    pm.SetKvStore(kv_store.get());

    ObjAddr obj_addr0{.obj_key_ = "key1", .part_offset_ = 0, .part_size_ = 10};
    ObjAddr obj_addr1{.obj_key_ = "key1", .part_offset_ = 10, .part_size_ = 20};
    pm.SaveLocalPath(paths[0], obj_addr0);
    pm.SaveLocalPath(paths[1], obj_addr1);

    AddrSerializer addr_serializer;
    addr_serializer.Initialize(&pm, paths);
    SizeT size = addr_serializer.GetSizeInBytes();
    auto buffer = MakeUnique<char[]>(size);
    char *ptr = buffer.get();
    addr_serializer.WriteBufAdv(ptr);
    SizeT write_size = ptr - buffer.get();
    ASSERT_EQ(write_size, size);

    AddrSerializer addr_serializer1;
    const char *ptr1 = buffer.get();
    Vector<String> paths1 = addr_serializer1.ReadBufAdv(ptr1);
    SizeT read_size = ptr1 - buffer.get();
    ASSERT_EQ(read_size, size);
    ASSERT_EQ(paths1, paths);
}

void Println(const String &message1, const String &message2) { std::cout << message1 << message2 << std::endl; }

TEST_F(WalEntryTest, WalEntryIterator) {
    using namespace infinity;
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    String wal_file_path = String(GetFullWalDir()) + "/wal.log";
    String ckp_file_path = "catalog";
    String ckp_file_name = String("META_123.full.json");
    MockWalFile(wal_file_path, ckp_file_path, ckp_file_name);
    {
        auto iterator1 = WalEntryIterator::Make(wal_file_path, true);

        while (iterator1->HasNext()) {
            auto wal_entry = iterator1->Next();
            if (wal_entry == nullptr) {
                break;
            }
            //            Println("WAL ENTRY COMMIT TS:", std::to_string(wal_entry->commit_ts_));
            //            for (const auto &cmd : wal_entry->cmds_) {
            //                Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
            //            }
        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    TxnTimeStamp max_commit_ts = 0;
    {
        auto iterator = WalEntryIterator::Make(wal_file_path, true);

        // phase 1: find the max commit ts and catalog path
        while (iterator->HasNext()) {
            auto wal_entry = iterator->Next();
            if (wal_entry == nullptr) {
                break;
            }
            WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
            if (!wal_entry->IsCheckPoint(checkpoint_cmd)) {
                replay_entries.push_back(wal_entry);
            } else {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;

                //                Println("Checkpoint Max Commit Ts: {}", std::to_string(max_commit_ts));
                //                Println("Catalog Path: {}", catalog_path);
                break;
            }
        }

        // phase 2: by the max commit ts, find the entries to replay
        while (iterator->HasNext()) {
            auto wal_entry = iterator->Next();
            if (wal_entry == nullptr) {
                break;
            }
            if (wal_entry->commit_ts_ > max_commit_ts) {
                replay_entries.push_back(wal_entry);
            }
        }
    }

    // phase 3: replay the entries
    //    Println("Start to replay the entries", "");
    //    for (const auto &entry : replay_entries) {
    //        Println("WAL ENTRY COMMIT TS:", std::to_string(entry->commit_ts_));
    //        for (const auto &cmd : entry->cmds_) {
    //            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
    //        }
    //    }
    EXPECT_EQ(max_commit_ts, 123ul);
    EXPECT_EQ(replay_entries.size(), 1u);
}

TEST_F(WalEntryTest, WalListIterator) {
    using namespace infinity;
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    String wal_file_path1 = String(GetFullWalDir()) + "/wal.log";
    String wal_file_path2 = String(GetFullWalDir()) + "/wal2.log";
    String ckp_file_path = "catalog";
    String ckp_file_name = String("META_123.full.json");
    MockWalFile(wal_file_path1, ckp_file_path, ckp_file_name);
    MockWalFile(wal_file_path2, ckp_file_path, ckp_file_name);

    WalListIterator iterator1({wal_file_path1, wal_file_path2});

    while (iterator1.HasNext()) {
        auto wal_entry = iterator1.Next();
        if (wal_entry.get() == nullptr) {
            break;
        }
        //        Println("WAL ENTRY COMMIT TS:", std::to_string(wal_entry->commit_ts_));
        //        for (const auto &cmd : wal_entry->cmds_) {
        //            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
        //        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    TxnTimeStamp max_commit_ts = 0;
    {
        WalListIterator iterator({wal_file_path1, wal_file_path2});

        // phase 1: find the max commit ts and catalog path
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                break;
            }
            WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
            if (!wal_entry->IsCheckPoint(checkpoint_cmd)) {
                replay_entries.push_back(wal_entry);
            } else {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                //                Println("Checkpoint Max Commit Ts: {}", std::to_string(max_commit_ts));
                //                Println("Catalog Path: {}", catalog_path);
                break;
            }
        }

        // phase 2: by the max commit ts, find the entries to replay
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                break;
            }
            if (wal_entry->commit_ts_ > max_commit_ts) {
                replay_entries.push_back(wal_entry);
            }
        }
    }

    // phase 3: replay the entries
    //    Println("Start to replay the entries", "");
    //    for (const auto &entry : replay_entries) {
    //        Println("WAL ENTRY COMMIT TS:", std::to_string(entry->commit_ts_));
    //        for (const auto &cmd : entry->cmds_) {
    //            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
    //        }
    //    }
    EXPECT_EQ(max_commit_ts, 123ul);
    EXPECT_EQ(replay_entries.size(), 1u);
}

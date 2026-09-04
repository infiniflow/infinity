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

module infinity_core:ut.wal_entry;

import :ut.base_test;
import :infinity_context;
import :infinity_exception;

import third_party;
import :logger;
import :table_def;
import :wal_entry;
import :value;
import :data_block;
import :default_values;
import :index_base;
import :index_ivf;
import :status;

import global_resource_usage;
import logical_type;
import internal_types;
import column_def;
import statement_common;
import data_type;
import embedding_info;

using namespace infinity;

class WalEntryTest : public BaseTest {};

std::shared_ptr<TableDef> MockTableDesc2() {
    // Define columns
    std::vector<std::shared_ptr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                std::make_shared<ColumnDef>(column_id++, std::make_shared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                std::make_shared<ColumnDef>(column_id++, std::make_shared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                std::make_shared<ColumnDef>(column_id++, std::make_shared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    return std::make_shared<TableDef>(std::make_shared<std::string>("default_db"),
                                      std::make_shared<std::string>("tbl1"),
                                      std::make_shared<std::string>(),
                                      columns);
}

WalSegmentInfo MakeSegmentInfo(size_t row_count, TxnTimeStamp commit_ts, size_t column_count) {
    WalSegmentInfo segment_info;
    segment_info.segment_id_ = 0;
    segment_info.column_count_ = column_count;
    segment_info.actual_row_count_ = segment_info.row_count_ = row_count;
    segment_info.row_capacity_ = row_count * 2;
    std::vector<WalBlockInfo> block_infos_;
    {
        WalBlockInfo block_info;
        block_info.block_id_ = 0;
        block_info.row_count_ = row_count;
        block_info.row_capacity_ = row_count;
        std::vector<std::pair<u32, u64>> outline_info;
        outline_info.resize(column_count);
        block_info.outline_infos_ = std::move(outline_info);
    }
    segment_info.block_infos_ = std::move(block_infos_);
    return segment_info;
}

void MockWalFile(const std::string &wal_file_path, const std::string &ckp_file_path, const std::string &ckp_file_name) {
    for (int commit_ts = 0; commit_ts < 3; ++commit_ts) {
        size_t row_count = DEFAULT_VECTOR_SIZE;

        auto entry = std::make_shared<WalEntry>();
        entry->cmds_.push_back(std::make_shared<WalCmdCreateDatabaseV2>("default2", "1", "default2_comment"));
        entry->cmds_.push_back(std::make_shared<WalCmdCreateTableV2>("default_db", "0", "1", MockTableDesc2()));
        WalSegmentInfo segment_info = MakeSegmentInfo(row_count, commit_ts, 2);
        entry->cmds_.push_back(std::make_shared<WalCmdImportV2>("default_db", "0", "tbl1", "1", std::move(segment_info)));

        auto data_block = DataBlock::Make();
        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(std::make_shared<DataType>(LogicalType::kBoolean));
        column_types.emplace_back(std::make_shared<DataType>(LogicalType::kTinyInt));
        data_block->Init(column_types);
        for (size_t i = 0; i < row_count; ++i) {
            data_block->AppendValue(0, Value::MakeBool(i % 2 == 0));
            data_block->AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block->Finalize();

        RowID start_row(0, 0);
        entry->cmds_.push_back(std::make_shared<WalCmdAppendV2>("db1",
                                                                "2",
                                                                "tbl1",
                                                                "1",
                                                                std::vector<std::pair<RowID, u64>>{std::pair<RowID, u64>{start_row, 8192}},
                                                                data_block));
        entry->commit_ts_ = commit_ts;

        i32 expect_size = entry->GetSizeInBytes();
        std::vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            UnrecoverableError(fmt::format("Failed to open wal file: {}", wal_file_path));
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = std::make_shared<WalEntry>();
        std::vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        std::vector<SegmentID> deprecated_segment_ids{0, 1, 2};
        std::vector<std::string> names;
        std::vector<std::string> ids;
        entry->cmds_.push_back(
            std::make_shared<WalCmdCompactV2>("db1", "2", "tbl1", "1", names, ids, std::move(new_segment_infos), std::move(deprecated_segment_ids)));
        entry->commit_ts_ = 5;
        i32 expect_size = entry->GetSizeInBytes();
        std::vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            UnrecoverableError(fmt::format("Failed to open wal file: {}", wal_file_path));
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = std::make_shared<WalEntry>();
        entry->cmds_.push_back(std::make_shared<WalCmdCheckpointV2>(int64_t(123)));
        entry->commit_ts_ = 3;
        i32 expect_size = entry->GetSizeInBytes();
        std::vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            UnrecoverableError(fmt::format("Failed to open wal file: {}", wal_file_path));
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = std::make_shared<WalEntry>();
        entry->cmds_.push_back(std::make_shared<WalCmdDropTableV2>("db1", "2", "tbl1", "1", 1, "table_key"));
        entry->commit_ts_ = 4;
        i32 expect_size = entry->GetSizeInBytes();
        std::vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            UnrecoverableError(fmt::format("Failed to open wal file: {}", wal_file_path));
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
}

TEST_F(WalEntryTest, ReadWriteV2) {
    // Earlier cases may leave a dirty infinity instance. Destroy it first.
    infinity::InfinityContext::instance().UnInit();
    RemoveDbDirs();
    auto config_path = std::make_shared<std::string>(BaseTestNoParam::NEW_CONFIG_PATH);
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();
    std::shared_ptr<WalEntry> entry = std::make_shared<WalEntry>();
    entry->cmds_.push_back(std::make_shared<WalCmdCreateDatabaseV2>("db1", "1", "default2_comment"));
    entry->cmds_.push_back(std::make_shared<WalCmdDropDatabaseV2>("db1", "1", 1));
    entry->cmds_.push_back(std::make_shared<WalCmdCreateTableV2>("db1", "1", "2", MockTableDesc2()));
    entry->cmds_.push_back(std::make_shared<WalCmdDropTableV2>("db1", "1", "tbl1", "2", 1, "table_key"));
    {
        WalSegmentInfo segment_info = MakeSegmentInfo(100, 8, 2);
        entry->cmds_.push_back(std::make_shared<WalCmdImportV2>("db1", "1", "tbl1", "2", std::move(segment_info)));
    }
    {
        std::vector<InitParameter *> parameters = {new InitParameter("metric", "ip")};
        std::shared_ptr<std::string> index_name = std::make_shared<std::string>("idx1");
        auto index_base = IndexIVF::Make(index_name,
                                         std::make_shared<std::string>("test comment"),
                                         "idx1_tbl1",
                                         std::vector<std::string>{"col1", "col2"},
                                         parameters);
        for (auto parameter : parameters) {
            delete parameter;
        }
        entry->cmds_.push_back(std::make_shared<WalCmdCreateIndexV2>("db1", "1", "tbl1", "2", "3", index_base, "table_key"));
    }
    entry->cmds_.push_back(std::make_shared<WalCmdDropIndexV2>("db1", "1", "tbl1", "2", "idx1", "3", 1, "index_key"));
    {
        std::shared_ptr<DataBlock> data_block = DataBlock::Make();
        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(std::make_shared<DataType>(LogicalType::kBoolean));
        column_types.emplace_back(std::make_shared<DataType>(LogicalType::kTinyInt));
        size_t row_count = DEFAULT_VECTOR_SIZE;
        data_block->Init(column_types);
        for (size_t i = 0; i < row_count; ++i) {
            data_block->AppendValue(0, Value::MakeBool(i % 2 == 0));
            data_block->AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block->Finalize();
        std::vector<std::pair<RowID, u64>> row_ranges = {{RowID(0, 0), row_count}};
        entry->cmds_.push_back(std::make_shared<WalCmdAppendV2>("db1", "1", "tbl1", "2", row_ranges, data_block));
    }
    {
        std::vector<RowID> row_ids = {RowID(1, 3)};
        entry->cmds_.push_back(std::make_shared<WalCmdDeleteV2>("db1", "1", "tbl1", "2", row_ids));
    }
    entry->cmds_.push_back(std::make_shared<WalCmdCheckpointV2>(int64_t(123)));
    {
        std::vector<std::string> names;
        std::vector<std::string> ids;
        std::vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        entry->cmds_.push_back(
            std::make_shared<WalCmdCompactV2>("db1", "1", "tbl1", "2", names, ids, std::move(new_segment_infos), std::vector<SegmentID>{0, 1, 2}));
    }
    {
        WalChunkIndexInfo info;
        info.chunk_id_ = 2;
        info.base_name_ = "base_name";
        info.base_rowid_ = RowID(0, 0);
        info.row_count_ = 4;
        info.deprecate_ts_ = 0;
        std::vector<WalChunkIndexInfo> chunk_infos{info};
        std::vector<ChunkID> deprecate_ids{0, 1};
        entry->cmds_.push_back(
            std::make_shared<WalCmdDumpIndexV2>("db1", "1", "tbl1", "2", "idx1", "3", 0 /*segment_id*/, chunk_infos, deprecate_ids, "table_key"));
    }
    {
        entry->cmds_.push_back(std::make_shared<WalCmdRenameTableV2>("db1", "1", "tbl1", "2", "tbl2", "old_table_key"));
    }
    {
        std::vector<u32> column_idx_list;
        std::vector<std::shared_ptr<ColumnDef>> column_defs;
        std::set<ConstraintType> constraints;

        auto column_def3 =
            std::make_shared<ColumnDef>(3 /*column_id*/, std::make_shared<DataType>(LogicalType::kBoolean), "boolean_col", constraints);
        auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
        auto column_def4 = std::make_shared<ColumnDef>(4 /*column id*/,
                                                       std::make_shared<DataType>(LogicalType::kEmbedding, embedding_info),
                                                       "embedding_col",
                                                       constraints);

        column_defs.push_back(column_def3);
        column_defs.push_back(column_def4);

        column_idx_list.push_back(3);
        column_idx_list.push_back(4);
        entry->cmds_.push_back(
            std::make_shared<WalCmdAddColumnsV2>("db1", "1", "tbl1", "2", std::move(column_idx_list), std::move(column_defs), "table_key"));
    }
    {
        std::vector<std::string> column_names;
        column_names.push_back("boolean_col");
        column_names.push_back("embedding_col");
        std::vector<std::string> column_keys;
        column_keys.push_back("column_key1");
        column_keys.push_back("column_key2");
        entry->cmds_.push_back(std::make_shared<WalCmdDropColumnsV2>("db1",
                                                                     "1",
                                                                     "tbl1",
                                                                     "2",
                                                                     std::move(column_names),
                                                                     std::vector<ColumnID>{3, 4},
                                                                     "table_key",
                                                                     std::move(column_keys)));
    }

    i32 exp_size = entry->GetSizeInBytes();
    std::vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    entry->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    std::shared_ptr<WalEntry> entry2 = WalEntry::ReadAdv(ptr_r, exp_size);
    EXPECT_NE(entry2, nullptr);
    EXPECT_EQ(*entry == *entry2, true);
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    infinity::InfinityContext::instance().UnInit();
}

void Println(const std::string &message1, const std::string &message2) { std::cout << message1 << message2 << std::endl; }

TEST_F(WalEntryTest, WalEntryIterator) {
    using namespace infinity;
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    std::string wal_file_path = std::string(GetFullWalDir()) + "/wal.log";
    std::string ckp_file_path = "catalog";
    std::string ckp_file_name = std::string("META_123.full.json");
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

    std::vector<std::shared_ptr<WalEntry>> replay_entries;
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
    std::string wal_file_path1 = std::string(GetFullWalDir()) + "/wal.log";
    std::string wal_file_path2 = std::string(GetFullWalDir()) + "/wal2.log";
    std::string ckp_file_path = "catalog";
    std::string ckp_file_name = std::string("META_123.full.json");
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

    std::vector<std::shared_ptr<WalEntry>> replay_entries;
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

// Byte offset of every entry of `wal_file_path`, in file order.
std::vector<i64> WalEntryOffsets(const std::string &wal_file_path) {
    std::vector<i64> offsets;
    auto iter = WalEntryIterator::Make(wal_file_path, false);
    while (iter->HasNext()) {
        offsets.push_back(iter->GetOffset());
        if (iter->Next() == nullptr) {
            break;
        }
    }
    return offsets;
}

// Flips a byte of the command count of the entry starting at `entry_offset`, so that the entry no longer
// matches its checksum. The size fields stay intact, so the entry is only rejected once it is read.
void CorruptEntry(const std::string &wal_file_path, i64 entry_offset) {
    const i64 offset = entry_offset + static_cast<i64>(sizeof(WalEntryHeader));
    std::fstream fs(wal_file_path, std::ios::in | std::ios::out | std::ios::binary);
    EXPECT_TRUE(fs.is_open());
    if (!fs.is_open()) {
        return;
    }
    fs.seekg(offset);
    char byte = 0;
    fs.read(&byte, 1);
    fs.seekp(offset);
    byte = static_cast<char>(byte ^ 0xFF);
    fs.write(&byte, 1);
}

// Cuts the checkpoint and everything behind it off a freshly mocked WAL file, so that the file holds no
// checkpoint. Returns the offsets of the four entries left in it.
std::vector<i64> MockWalFileWithoutCheckpoint(const std::string &wal_file) {
    MockWalFile(wal_file, "catalog", "META_123.full.json");
    const std::vector<i64> offsets = WalEntryOffsets(wal_file);
    // ASSERT_* expands to `return;`, which does not compile in a function returning a value, so check
    // explicitly and bail out: offsets[4] below and the four offsets returned both need this many.
    if (offsets.size() < 5) {
        ADD_FAILURE() << "MockWalFile produced " << offsets.size() << " entries in " << wal_file << ", need at least 5";
        return {};
    }
    std::filesystem::resize_file(wal_file, static_cast<size_t>(offsets[4]));
    return std::vector<i64>(offsets.begin(), std::next(offsets.begin(), 4));
}

// Builds a WAL pair and drops the checkpoint (and everything behind it) from the newer file, so that the
// purge has to look for the checkpoint in the older file. Returns the offsets of the entries left in the
// newer file, which are its first four entries.
std::vector<i64> MockWalPair(const std::string &newer_wal_file, const std::string &older_wal_file) {
    MockWalFile(older_wal_file, "catalog", "META_123.full.json");
    return MockWalFileWithoutCheckpoint(newer_wal_file);
}

// Drains the iterator and returns the max commit ts of the first checkpoint it reaches.
TxnTimeStamp ReplayToFirstCheckpoint(WalListIterator &iterator) {
    TxnTimeStamp max_commit_ts = 0;
    while (iterator.HasNext()) {
        auto entry = iterator.Next();
        if (entry.get() == nullptr) {
            break;
        }
        WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
        if (entry->IsCheckPoint(checkpoint_cmd)) {
            max_commit_ts = checkpoint_cmd->max_commit_ts_;
            break;
        }
    }
    return max_commit_ts;
}

// Drains the iterator and returns the commit ts of every entry replay sees, in that order.
std::vector<TxnTimeStamp> DrainWalList(WalListIterator &iterator) {
    std::vector<TxnTimeStamp> commit_tss;
    while (iterator.HasNext()) {
        auto entry = iterator.Next();
        if (entry.get() == nullptr) {
            break;
        }
        commit_tss.push_back(entry->commit_ts_);
    }
    return commit_tss;
}

// Damage in front of the checkpoint entry must be ignored: replay only reads the entries behind it, and
// the backward scan that locates the checkpoint must not walk the damaged prefix.
TEST_F(WalEntryTest, DamagedEntryBeforeCheckpointIsIgnored) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    MockWalFile(wal_file, "catalog", "META_123.full.json");

    const std::vector<i64> offsets = WalEntryOffsets(wal_file);
    ASSERT_FALSE(offsets.empty());
    const auto size_before = std::filesystem::file_size(wal_file);
    CorruptEntry(wal_file, offsets[0]);

    WalListIterator iterator({wal_file});
    EXPECT_EQ(ReplayToFirstCheckpoint(iterator), 123ul);
    EXPECT_EQ(std::filesystem::file_size(wal_file), size_before);
}

// Damage in a file that holds no checkpoint: the file is truncated at the first bad entry and replay
// continues into the older file, which still carries the checkpoint.
TEST_F(WalEntryTest, DamagedEntryTruncatesWalFile) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::vector<i64> offsets = MockWalPair(wal_file1, wal_file2);

    CorruptEntry(wal_file1, offsets[2]);

    WalListIterator iterator({wal_file1, wal_file2});
    EXPECT_EQ(ReplayToFirstCheckpoint(iterator), 123ul);
    EXPECT_EQ(std::filesystem::file_size(wal_file1), static_cast<uintmax_t>(offsets[2]));
}

// Same as above, but the very first entry is damaged, so nothing survives in the file and it is removed.
TEST_F(WalEntryTest, DamagedFirstEntryDropsWalFile) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::vector<i64> offsets = MockWalPair(wal_file1, wal_file2);

    CorruptEntry(wal_file1, offsets[0]);

    WalListIterator iterator({wal_file1, wal_file2});
    EXPECT_EQ(ReplayToFirstCheckpoint(iterator), 123ul);
    EXPECT_FALSE(std::filesystem::exists(wal_file1));
    EXPECT_TRUE(std::filesystem::exists(wal_file2));
}

// Damage behind the checkpoint entry is on the tail replay actually reads, so the checkpoint file itself
// gets cut too.
TEST_F(WalEntryTest, DamagedTailAfterCheckpointIsTruncated) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    MockWalFile(wal_file, "catalog", "META_123.full.json");

    const std::vector<i64> offsets = WalEntryOffsets(wal_file);
    ASSERT_EQ(offsets.size(), 6u);
    CorruptEntry(wal_file, offsets[5]);

    WalListIterator iterator({wal_file});
    const std::vector<TxnTimeStamp> drained = DrainWalList(iterator);
    EXPECT_EQ(std::filesystem::file_size(wal_file), static_cast<uintmax_t>(offsets[5]));
    // The checkpoint entry survived the cut and is replayed along with the four entries behind it.
    EXPECT_EQ(drained.size(), 5u);
}

// Two damaged files: the damage in the older one drops it and everything newer than it, so replay restarts
// from the oldest file only. Leaving the newer files in place would make replay open a file the purge just
// deleted.
TEST_F(WalEntryTest, DamagedOlderFileDropsNewerOnes) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::string wal_file3 = std::string(GetFullWalDir()) + "/wal3.log";
    const std::vector<i64> offsets = MockWalPair(wal_file1, wal_file3);
    MockWalFileWithoutCheckpoint(wal_file2);
    ASSERT_EQ(offsets.size(), 4u);

    CorruptEntry(wal_file1, offsets[2]);
    CorruptEntry(wal_file2, offsets[0]);

    WalListIterator iterator({wal_file1, wal_file2, wal_file3});
    const std::vector<TxnTimeStamp> drained = DrainWalList(iterator);
    EXPECT_EQ(std::filesystem::file_size(wal_file1), static_cast<uintmax_t>(offsets[2]));
    EXPECT_FALSE(std::filesystem::exists(wal_file2));
    // Only wal_file3 is left, so replay walks its six entries.
    EXPECT_EQ(drained.size(), 6u);
}

// No checkpoint is reachable anywhere: the damage is still purged, and no file is dropped from the list
// because there is no checkpoint to restart from.
TEST_F(WalEntryTest, NoCheckpointStillPurgesDamage) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::vector<i64> offsets = MockWalFileWithoutCheckpoint(wal_file1);
    MockWalFileWithoutCheckpoint(wal_file2);
    ASSERT_EQ(offsets.size(), 4u);

    CorruptEntry(wal_file1, offsets[1]);

    WalListIterator iterator({wal_file1, wal_file2});
    const std::vector<TxnTimeStamp> drained = DrainWalList(iterator);
    EXPECT_EQ(std::filesystem::file_size(wal_file1), static_cast<uintmax_t>(offsets[1]));
    // One entry survives in wal_file1, four in wal_file2.
    EXPECT_EQ(drained.size(), 5u);
}

// A file whose last entry was torn by a crash: the iterator stops at the damage instead of spinning on it.
TEST_F(WalEntryTest, TornTailStopsIteration) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    MockWalFile(wal_file, "catalog", "META_123.full.json");

    const std::vector<i64> offsets = WalEntryOffsets(wal_file);
    ASSERT_GE(offsets.size(), 6u);
    std::filesystem::resize_file(wal_file, static_cast<size_t>(offsets[5]) + 5);

    auto iter = WalEntryIterator::Make(wal_file, false);
    const std::vector<std::shared_ptr<WalEntry>> entries = iter->GetAllEntries();
    EXPECT_EQ(entries.size(), 5u);
    EXPECT_FALSE(iter->IsGood());
}

// Overwrites the trailing 4 bytes (the copy of the entry size) of the entry starting at `entry_offset`
// with a bogus value, leaving the leading size and the checksum intact. This is the "torn trailing size"
// failure mode: the backward scan trusts the trailing size and is led astray, while the forward scan reads
// the leading size and cross-checks the trailing one, so it recovers the real boundary.
void CorruptTrailingSize(const std::string &wal_file, i64 entry_offset, i32 bogus_size) {
    std::fstream fs(wal_file, std::ios::in | std::ios::out | std::ios::binary);
    ASSERT_TRUE(fs.is_open());
    if (!fs.is_open()) {
        return;
    }
    fs.seekp(entry_offset);
    i32 leading_size = 0;
    fs.read(reinterpret_cast<char *>(&leading_size), sizeof(leading_size));
    fs.seekp(entry_offset + leading_size - static_cast<std::streamoff>(sizeof(i32)));
    fs.write(reinterpret_cast<const char *>(&bogus_size), sizeof(bogus_size));
    fs.close();
}

// Corrupting only the trailing size of the LAST entry fools the backward scan into stopping at a frame
// boundary that is not real. The forward scan reads the leading size and cross-checks the trailing one, so
// it recovers the real boundary and the file is truncated at the torn entry, keeping everything before it.
TEST_F(WalEntryTest, CorruptedTrailingSizeIsRecoveredByForwardScan) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    MockWalFile(wal_file, "catalog", "META_123.full.json");

    const std::vector<i64> offsets = WalEntryOffsets(wal_file);
    ASSERT_GE(offsets.size(), 6u);
    CorruptTrailingSize(wal_file, offsets[5], 1024);

    WalListIterator iterator({wal_file});
    const std::vector<TxnTimeStamp> drained = DrainWalList(iterator);
    // The last entry is torn, so the file is cut before it: the four entries in front of the checkpoint
    // survive, and the checkpoint entry itself is the fifth and is replayed.
    EXPECT_EQ(std::filesystem::file_size(wal_file), static_cast<uintmax_t>(offsets[5]));
    EXPECT_EQ(drained.size(), 5u);
}

// A WAL file that cannot be read must be a RecoverableException, not an UnrecoverableException: the admin
// SHOW WAL commands inspect WAL files on a running server, where a file that disappears between being
// listed and being opened is a client error. Checkpointing removes old WAL files, so this is reachable.
// An empty WAL file is not an error: it simply holds no entries.
TEST_F(WalEntryTest, EmptyWalFileIsNotAnError) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    {
        std::ofstream ofs(wal_file, std::ios::binary);
    }

    auto iter = WalEntryIterator::Make(wal_file, false);
    EXPECT_TRUE(iter->GetAllEntries().empty());
    EXPECT_TRUE(iter->IsGood());
}

// A file too short to even hold an entry size must be reported as empty, not read out of bounds.
TEST_F(WalEntryTest, FileShorterThanEntrySize) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file = std::string(GetFullWalDir()) + "/wal.log";
    MockWalFile(wal_file, "catalog", "META_123.full.json");
    std::filesystem::resize_file(wal_file, 2);

    auto forward_iter = WalEntryIterator::Make(wal_file, false);
    EXPECT_TRUE(forward_iter->GetAllEntries().empty());
    EXPECT_FALSE(forward_iter->IsGood());

    auto backward_iter = WalEntryIterator::Make(wal_file, true);
    EXPECT_TRUE(backward_iter->GetAllEntries().empty());
    EXPECT_FALSE(backward_iter->IsGood());
}

// When every WAL file is damaged at its first entry and none of them holds a checkpoint, the purge deletes
// them all and the list becomes empty. The iterator must then report no entries instead of dereferencing a
// null iterator.
TEST_F(WalEntryTest, AllFilesPurgedLeavesEmptyList) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::vector<i64> offsets1 = MockWalFileWithoutCheckpoint(wal_file1);
    const std::vector<i64> offsets2 = MockWalFileWithoutCheckpoint(wal_file2);
    ASSERT_EQ(offsets1.size(), 4u);
    ASSERT_EQ(offsets2.size(), 4u);

    CorruptEntry(wal_file1, offsets1[0]);
    CorruptEntry(wal_file2, offsets2[0]);

    WalListIterator iterator({wal_file1, wal_file2});
    // Both files were deleted by the purge because nothing survived in either.
    EXPECT_FALSE(std::filesystem::exists(wal_file1));
    EXPECT_FALSE(std::filesystem::exists(wal_file2));
    // With an empty list the iterator is exhausted and Next() is safe.
    EXPECT_FALSE(iterator.HasNext());
    EXPECT_EQ(iterator.Next(), nullptr);
}

// Damage accumulates across several files: an older damaged file forces replay to start behind every newer
// file too. Here the checkpoint lives in the oldest file, so the two damaged files ahead of it (one
// truncated, one deleted) are both dropped and only the checkpoint file survives.
TEST_F(WalEntryTest, DamageAccumulatesToCheckpointFile) {
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    const std::string wal_file1 = std::string(GetFullWalDir()) + "/wal.log";
    const std::string wal_file2 = std::string(GetFullWalDir()) + "/wal2.log";
    const std::string wal_file3 = std::string(GetFullWalDir()) + "/wal3.log";
    const std::string wal_file4 = std::string(GetFullWalDir()) + "/wal4.log";
    const std::vector<i64> offsets1 = MockWalFileWithoutCheckpoint(wal_file1);
    MockWalFileWithoutCheckpoint(wal_file2);
    const std::vector<i64> offsets3 = MockWalFileWithoutCheckpoint(wal_file3);
    MockWalFile(wal_file4, "catalog", "META_123.full.json"); // checkpoint lives here (oldest)
    ASSERT_EQ(offsets1.size(), 4u);
    ASSERT_EQ(offsets3.size(), 4u);

    // wal.log: newest, truncated (damage not at offset 0).
    CorruptEntry(wal_file1, offsets1[2]);
    // wal3.log: damaged at its first entry, so it is deleted.
    CorruptEntry(wal_file3, offsets3[0]);

    WalListIterator iterator({wal_file1, wal_file2, wal_file3, wal_file4});
    // wal.log survives but is truncated before the damaged entry; wal3.log was deleted.
    EXPECT_EQ(std::filesystem::file_size(wal_file1), static_cast<uintmax_t>(offsets1[2]));
    EXPECT_TRUE(std::filesystem::exists(wal_file2));
    EXPECT_FALSE(std::filesystem::exists(wal_file3));
    // Replay restarts from the oldest file (wal4.log) which still carries the checkpoint.
    EXPECT_TRUE(std::filesystem::exists(wal_file4));
    EXPECT_EQ(ReplayToFirstCheckpoint(iterator), 123ul);
}

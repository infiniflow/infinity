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

#include "unit_test/base_test.h"

#include <filesystem>

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal_entry;
import segment_entry;
import value;

import data_block;
import default_values;
import index_ivfflat;
import index_base;
import logical_type;
import internal_types;
import column_def;
import statement_common;
import data_type;

class WalEntryTest : public BaseTest {};

using namespace infinity;

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

    return MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), columns);
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
        entry->cmds_.push_back(MakeShared<WalCmdCreateDatabase>("default2", "AAA_default2"));
        entry->cmds_.push_back(MakeShared<WalCmdCreateTable>("default_db", "BBB_default", MockTableDesc2()));
        WalSegmentInfo segment_info = MakeSegmentInfo(row_count, commit_ts, 2);
        entry->cmds_.push_back(MakeShared<WalCmdImport>("default_db", "tbl1", std::move(segment_info)));

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
        entry->cmds_.push_back(MakeShared<WalCmdAppend>("db1", "tbl1", data_block));
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
        entry->cmds_.push_back(MakeShared<WalCmdCompact>("db1", "tbl1", std::move(new_segment_infos), std::move(deprecated_segment_ids)));
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
        entry->cmds_.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), true, ckp_file_path, ckp_file_name));
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
        entry->cmds_.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
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
    RemoveDbDirs();
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds_.push_back(MakeShared<WalCmdCreateDatabase>("db1", "AAA_db1"));
    entry->cmds_.push_back(MakeShared<WalCmdDropDatabase>("db1"));
    entry->cmds_.push_back(MakeShared<WalCmdCreateTable>("db1", "BBB_tb1", MockTableDesc2()));
    entry->cmds_.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
    {
        WalSegmentInfo segment_info = MakeSegmentInfo(100, 8, 2);
        entry->cmds_.push_back(MakeShared<WalCmdImport>("db1", "tbl1", std::move(segment_info)));
    }
    {
        Vector<InitParameter *> parameters = {new InitParameter("centroids_count", "100"), new InitParameter("metric", "l2")};
        SharedPtr<String> index_name = MakeShared<String>("idx1");
        auto index_base = IndexIVFFlat::Make(index_name, "idx1_tbl1", Vector<String>{"col1", "col2"}, parameters);
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
    entry->cmds_.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), true, String(GetFullDataDir()) + "/catalog", String("META_123.full.json")));
    {
        Vector<WalSegmentInfo> new_segment_infos(3, MakeSegmentInfo(1, 0, 2));
        entry->cmds_.push_back(MakeShared<WalCmdCompact>("db1", "tbl1", std::move(new_segment_infos), Vector<SegmentID>{0, 1, 2}));
    }

    i32 exp_size = entry->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    entry->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    SharedPtr<WalEntry> entry2 = WalEntry::ReadAdv(ptr, exp_size);
    EXPECT_NE(entry2, nullptr);
    EXPECT_EQ(*entry == *entry2, true);
    EXPECT_EQ(ptr - buf_beg, exp_size);
}

void Println(const String &message1, const String &message2) { std::cout << message1 << message2 << std::endl; }

TEST_F(WalEntryTest, WalEntryIterator) {
    using namespace infinity;
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    String wal_file_path = String(GetFullWalDir()) + "/wal.log";
    String ckp_file_path = String(GetFullDataDir()) + "/catalog";
    String ckp_file_name = String("META_123.full.json");
    MockWalFile(wal_file_path, ckp_file_path, ckp_file_name);
    {
        auto iterator1 = WalEntryIterator::Make(wal_file_path, true);

        while (iterator1->HasNext()) {
            auto wal_entry = iterator1->Next();
            if (wal_entry == nullptr) {
                break;
            }
            Println("WAL ENTRY COMMIT TS:", std::to_string(wal_entry->commit_ts_));
            for (const auto &cmd : wal_entry->cmds_) {
                Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
            }
        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    TxnTimeStamp max_commit_ts = 0;
    String catalog_path;
    {
        auto iterator = WalEntryIterator::Make(wal_file_path, true);

        // phase 1: find the max commit ts and catalog path
        while (iterator->HasNext()) {
            auto wal_entry = iterator->Next();
            if (wal_entry == nullptr) {
                break;
            }
            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (!wal_entry->IsCheckPoint(replay_entries, checkpoint_cmd)) {
                replay_entries.push_back(wal_entry);
            } else {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                catalog_path = checkpoint_cmd->catalog_path_;

                Println("Checkpoint Max Commit Ts: {}", std::to_string(max_commit_ts));
                Println("Catalog Path: {}", catalog_path);
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
    Println("Start to replay the entries", "");
    for (const auto &entry : replay_entries) {
        Println("WAL ENTRY COMMIT TS:", std::to_string(entry->commit_ts_));
        for (const auto &cmd : entry->cmds_) {
            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
        }
    }
    EXPECT_EQ(max_commit_ts, 123ul);
    EXPECT_EQ(catalog_path, String(GetFullDataDir()) + "/catalog");
    EXPECT_EQ(replay_entries.size(), 1u);
}

TEST_F(WalEntryTest, WalListIterator) {
    using namespace infinity;
    RemoveDbDirs();
    std::filesystem::create_directories(GetFullWalDir());
    String wal_file_path1 = String(GetFullWalDir()) + "/wal.log";
    String wal_file_path2 = String(GetFullWalDir()) + "/wal2.log";
    String ckp_file_path = String(GetFullDataDir()) + "/catalog";
    String ckp_file_name = String("META_123.full.json");
    MockWalFile(wal_file_path1, ckp_file_path, ckp_file_name);
    MockWalFile(wal_file_path2, ckp_file_path, ckp_file_name);

    WalListIterator iterator1({wal_file_path1, wal_file_path2});

    while (iterator1.HasNext()) {
        auto wal_entry = iterator1.Next();
        if (wal_entry.get() == nullptr) {
            break;
        }
        Println("WAL ENTRY COMMIT TS:", std::to_string(wal_entry->commit_ts_));
        for (const auto &cmd : wal_entry->cmds_) {
            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    TxnTimeStamp max_commit_ts = 0;
    String catalog_path;
    {
        WalListIterator iterator({wal_file_path1, wal_file_path2});

        // phase 1: find the max commit ts and catalog path
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                break;
            }
            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (!wal_entry->IsCheckPoint(replay_entries, checkpoint_cmd)) {
                replay_entries.push_back(wal_entry);
            } else {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                catalog_path = checkpoint_cmd->catalog_path_;

                Println("Checkpoint Max Commit Ts: {}", std::to_string(max_commit_ts));
                Println("Catalog Path: {}", catalog_path);
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
    Println("Start to replay the entries", "");
    for (const auto &entry : replay_entries) {
        Println("WAL ENTRY COMMIT TS:", std::to_string(entry->commit_ts_));
        for (const auto &cmd : entry->cmds_) {
            Println("  WAL CMD: ", WalCmd::WalCommandTypeToString(cmd->GetType()));
        }
    }
    EXPECT_EQ(max_commit_ts, 123ul);
    EXPECT_EQ(catalog_path, ckp_file_path);
    EXPECT_EQ(replay_entries.size(), 1u);
}

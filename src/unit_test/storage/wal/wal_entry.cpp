
#include "unit_test/base_test.h"

#include <filesystem>

import infinity_context;
import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
//import data_table;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import wal_manager;
import ivfflat_index_def;

class WalEntryTest : public BaseTest {};

using namespace infinity;

SharedPtr<TableDef> MockTableDesc2() {
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    return MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
}

void MockWalFile(const String &wal_file_path = "/tmp/infinity/wal/wal.log") {

    for (int commit_ts = 0; commit_ts < 3; ++commit_ts) {
        auto entry = MakeShared<WalEntry>();
        entry->cmds.push_back(MakeShared<WalCmdCreateDatabase>("default2"));
        entry->cmds.push_back(MakeShared<WalCmdCreateTable>("default", MockTableDesc2()));
        Vector<i32> row_counts{1, 2, 3};
        entry->cmds.push_back(
            MakeShared<WalCmdImport>("default", "tbl1", "/tmp/infinity/data/default/txn_66/tbl1/ENkJMWTQ8N_seg_0", 0, 3, row_counts));

        auto data_block = DataBlock::Make();
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
        entry->cmds.push_back(MakeShared<WalCmdAppend>("db1", "tbl1", data_block));
        entry->commit_ts = commit_ts;

        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        std::filesystem::create_directories("/tmp/infinity/wal");
        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            throw Exception("Failed to open wal file: /tmp/infinity/wal/wal.log");
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = MakeShared<WalEntry>();
        entry->cmds.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), true, "/tmp/infinity/data/catalog/META_123.full.json"));
        entry->commit_ts = 3;
        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        std::filesystem::create_directories("/tmp/infinity/wal");
        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            throw Exception("Failed to open wal file: /tmp/infinity/wal/wal.log");
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
    {
        auto entry = MakeShared<WalEntry>();
        entry->cmds.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
        entry->commit_ts = 4;
        i32 expect_size = entry->GetSizeInBytes();
        Vector<char> buf(expect_size);
        char *ptr = buf.data();
        entry->WriteAdv(ptr);
        i32 actual_size = ptr - buf.data();
        EXPECT_EQ(actual_size, expect_size);

        std::filesystem::create_directories("/tmp/infinity/wal");
        auto ofs = std::ofstream(wal_file_path, std::ios::app | std::ios::binary);
        if (!ofs.is_open()) {
            throw Exception("Failed to open wal file: /tmp/infinity/wal/wal.log");
        }
        ofs.write(buf.data(), ptr - buf.data());
        ofs.flush();
        ofs.close();
    }
}

TEST_F(WalEntryTest, ReadWrite) {
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds.push_back(MakeShared<WalCmdCreateDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdDropDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdCreateTable>("db1", MockTableDesc2()));
    entry->cmds.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
    Vector<i32> row_counts{1, 2, 3};
    entry->cmds.push_back(MakeShared<WalCmdImport>("db1", "tbl1", "/tmp/infinity/data/default/txn_66/tbl1/ENkJMWTQ8N_seg_0", 0, 3, row_counts));

    auto index_def = IVFFlatIndexDef::Make(MakeShared<String>("idx1"),
                                           Vector<String>{"col1", "col2"},
                                           Vector<InitParameter *>{new InitParameter("centroids_count", "100"), new InitParameter("metric", "l2")});
    entry->cmds.push_back(MakeShared<WalCmdCreateIndex>("db1", "tbl1", index_def));

    entry->cmds.push_back(MakeShared<WalCmdDropIndex>("db1", "tbl1", "idx1"));

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

    entry->cmds.push_back(MakeShared<WalCmdAppend>("db1", "tbl1", data_block));
    Vector<RowID> row_ids = {RowID(1, 2, 3)};
    entry->cmds.push_back(MakeShared<WalCmdDelete>("db1", "tbl1", row_ids));
    entry->cmds.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), true, "/tmp/infinity/data/catalog/META_123.full.json"));

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
    MockWalFile();
    String wal_file_path = "/tmp/infinity/wal/wal.log";

    WalEntryIterator iterator1(wal_file_path);
    iterator1.Init();
    while (iterator1.Next()) {
        auto wal_entry = iterator1.GetEntry();
        Println("WAL ENTRY COMMIT TS:", ToStr(wal_entry->commit_ts));
        for (const auto &cmd : wal_entry->cmds) {
            Println("  WAL CMD: ", WalManager::WalCommandTypeToString(cmd->GetType()));
        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    int64_t max_commit_ts = 0;
    String catalog_path;
    WalEntryIterator iterator(wal_file_path);
    iterator.Init();

    // phase 1: find the max commit ts and catalog path
    while (iterator.Next()) {
        auto wal_entry = iterator.GetEntry();

        if (!wal_entry->IsCheckPoint()) {
            replay_entries.push_back(wal_entry);
        } else {
            std::tie(max_commit_ts, catalog_path) = wal_entry->GetCheckpointInfo();
            Println("Checkpoint Max Commit Ts: {}", ToStr(max_commit_ts));
            Println("Catalog Path: {}", catalog_path);
            break;
        }
    }

    // phase 2: by the max commit ts, find the entries to replay
    while (iterator.Next()) {
        auto wal_entry = iterator.GetEntry();
        if (wal_entry->commit_ts > max_commit_ts) {
            replay_entries.push_back(wal_entry);
        }
    }

    // phase 3: replay the entries
    Println("Start to replay the entries", "");
    for (const auto &entry : replay_entries) {
        Println("WAL ENTRY COMMIT TS:", ToStr(entry->commit_ts));
        for (const auto &cmd : entry->cmds) {
            Println("  WAL CMD: ", WalManager::WalCommandTypeToString(cmd->GetType()));
        }
    }
    EXPECT_EQ(max_commit_ts, 123);
    EXPECT_EQ(catalog_path, "/tmp/infinity/data/catalog/META_123.full.json");
    EXPECT_EQ(replay_entries.size(), 1);
}

TEST_F(WalEntryTest, WalListIterator) {
    using namespace infinity;
    MockWalFile();
    MockWalFile("/tmp/infinity/wal/wal2.log");

    WalListIterator iterator1({"/tmp/infinity/wal/wal.log", "/tmp/infinity/wal/wal2.log"});
    iterator1.Init();

    iterator1.Init();
    while (iterator1.Next()) {
        auto wal_entry = iterator1.GetEntry();
        Println("WAL ENTRY COMMIT TS:", ToStr(wal_entry->commit_ts));
        for (const auto &cmd : wal_entry->cmds) {
            Println("  WAL CMD: ", WalManager::WalCommandTypeToString(cmd->GetType()));
        }
    }

    Vector<SharedPtr<WalEntry>> replay_entries;
    int64_t max_commit_ts = 0;
    String catalog_path;
    WalListIterator iterator({"/tmp/infinity/wal/wal.log", "/tmp/infinity/wal/wal2.log"});
    iterator.Init();

    // phase 1: find the max commit ts and catalog path
    while (iterator.Next()) {
        auto wal_entry = iterator.GetEntry();

        if (!wal_entry->IsCheckPoint()) {
            replay_entries.push_back(wal_entry);
        } else {
            std::tie(max_commit_ts, catalog_path) = wal_entry->GetCheckpointInfo();
            Println("Checkpoint Max Commit Ts: {}", ToStr(max_commit_ts));
            Println("Catalog Path: {}", catalog_path);
            break;
        }
    }

    // phase 2: by the max commit ts, find the entries to replay
    while (iterator.Next()) {
        auto wal_entry = iterator.GetEntry();
        if (wal_entry->commit_ts > max_commit_ts) {
            replay_entries.push_back(wal_entry);
        }
    }

    // phase 3: replay the entries
    Println("Start to replay the entries", "");
    for (const auto &entry : replay_entries) {
        Println("WAL ENTRY COMMIT TS:", ToStr(entry->commit_ts));
        for (const auto &cmd : entry->cmds) {
            Println("  WAL CMD: ", WalManager::WalCommandTypeToString(cmd->GetType()));
        }
    }
    EXPECT_EQ(max_commit_ts, 123);
    EXPECT_EQ(catalog_path, "/tmp/infinity/data/catalog/META_123.full.json");
    EXPECT_EQ(replay_entries.size(), 1);
}

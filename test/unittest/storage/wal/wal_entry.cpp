
#include "base_test.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/profiler/base_profiler.h"
#include "main/stats/global_resource_usage.h"
#include "storage/data_block.h"
#include "storage/txn/txn_manager.h"
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

class WalEntryTest : public BaseTest {
    void SetUp() override {
        system("rm -rf /tmp/infinity");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/data/db");
        system("rm -rf /tmp/infinity/data/catalog/*");
        system("rm -rf /tmp/infinity/wal/*");
        system("rm -rf /tmp/infinity/_tmp");
    }
};

using namespace infinity;
namespace fs = std::filesystem;

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

TEST_F(WalEntryTest, ReadWrite) {
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds.push_back(MakeShared<WalCmdCreateDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdDropDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdCreateTable>("db1", MockTableDesc2()));
    entry->cmds.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
    entry->cmds.push_back(MakeShared<WalCmdImport>("db1", "tbl1", "/tmp/infinity/data/default/txn_66/tbl1/ENkJMWTQ8N_seg_0"));

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
    entry->cmds.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), std::string("catalog")));

    int32_t exp_size = entry->GetSizeInBytes();
    std::vector<char> buf(exp_size, char(0));
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

TEST_F(WalEntryTest, WalEntryIterator) {
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    entry->cmds.push_back(MakeShared<WalCmdCreateDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdDropDatabase>("db1"));
    entry->cmds.push_back(MakeShared<WalCmdCreateTable>("db1", MockTableDesc2()));
    entry->cmds.push_back(MakeShared<WalCmdDropTable>("db1", "tbl1"));
    entry->cmds.push_back(MakeShared<WalCmdImport>("db1", "tbl1", "/tmp/infinity/data/default/txn_66/tbl1/ENkJMWTQ8N_seg_0"));

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
    entry->cmds.push_back(MakeShared<WalCmdCheckpoint>(int64_t(123), std::string("catalog")));

    int32_t expect_size = entry->GetSizeInBytes();
    std::vector<char> buf(expect_size);
    char *ptr = buf.data();
    entry->WriteAdv(ptr);
    int32_t actual_size = ptr - buf.data();
    EXPECT_EQ(actual_size, expect_size);

    fs::create_directories("/tmp/infinity/wal");
    auto ofs = std::ofstream("/tmp/infinity/wal/wal.log", std::ios::app | std::ios::binary);
    if (!ofs.is_open()) {
        throw Exception("Failed to open wal file: /tmp/infinity/wal/wal.log");
    }

    ofs.write(buf.data(), ptr - buf.data());
    ofs.flush();

    Vector<String> wal_list;
    wal_list.push_back("/tmp/infinity/wal/wal.log");

    WalEntryIterator iterator(wal_list);
    while (iterator.Next()) {
        auto wal_entry = iterator.GetEntry();
        for (const auto &cmd : entry->cmds) {
            LOG_INFO("WAL CMD: {}", WalCommandTypeToString(cmd->GetType()).c_str());
        }
    }
}

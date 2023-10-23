
#include "unit_test/base_test.h"

import infinity;
import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
//import table;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
//import column_vector;

class WalEntryTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
//        system("rm -rf /tmp/infinity/data/db");
//        system("rm -rf /tmp/infinity/data/catalog/*");
//        system("rm -rf /tmp/infinity/_tmp");
    }
};

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

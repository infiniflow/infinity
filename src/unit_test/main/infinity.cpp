//
// Created by jinhai on 23-10-31.
//

#include "unit_test/base_test.h"

import stl;
import infinity;
import query_result;
import data_block;
import value;
import query_options;
import parser;
import database;

class InfinityTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        system("rm -rf /tmp/infinity/data /tmp/infinity/wal");
    }
};

TEST_F(InfinityTest, test1) {
    using namespace infinity;
    String path = "/tmp/infinity";
    SharedPtr<Infinity> infinity = Infinity::Connect(path);

    {
        QueryResult result = infinity->ListDatabases();
        //    EXPECT_EQ(result.result_table_->row_count(), 1); // Bug
        EXPECT_EQ(result.result_table_->ColumnCount(), 1);
        EXPECT_EQ(result.result_table_->GetColumnNameById(0), "database");
        EXPECT_EQ(result.result_table_->DataBlockCount(), 1);
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        EXPECT_EQ(data_block->row_count(), 1);
        Value value = data_block->GetValue(0, 0);
        EXPECT_EQ(value.type().type(), LogicalType::kVarchar);
        if (value.value_.varchar.IsInlined()) {
            String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "default");
        } else {
            String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), "default");
        }
    }

    {
        CreateDatabaseOptions create_db_opts;
        infinity->CreateDatabase("db1", create_db_opts);
        QueryResult result = infinity->ListDatabases();
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        EXPECT_EQ(data_block->row_count(), 2);
        Value value = data_block->GetValue(0, 0);
        EXPECT_EQ(value.type().type(), LogicalType::kVarchar);
        if (value.value_.varchar.IsInlined()) {
            String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "db1");
        } else {
            String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), "db1");
        }

        value = data_block->GetValue(0, 1);
        EXPECT_EQ(value.type().type(), LogicalType::kVarchar);
        if (value.value_.varchar.IsInlined()) {
            String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "default");
        } else {
            String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), "default");
        }

        DropDatabaseOptions drop_db_opts;
        result = infinity->DropDatabase("db1", drop_db_opts);
        EXPECT_TRUE(result.IsOk());

        result = infinity->ListDatabases();
        data_block = result.result_table_->GetDataBlockById(0);
        EXPECT_EQ(data_block->row_count(), 1);
        value = data_block->GetValue(0, 0);
        EXPECT_EQ(value.type().type(), LogicalType::kVarchar);
        if (value.value_.varchar.IsInlined()) {
            String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "default");
        } else {
            String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), "default");
        }

        SharedPtr<Database> db_instance = infinity->GetDatabase("default");
    }
    infinity->Disconnect();
}
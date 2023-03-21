//
// Created by jinhai on 23-1-25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/array_info.h"
#include "common/types/info/embedding_info.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class TableTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(TableTest, test1) {
    using namespace infinity;

    SizeT column_count = 2;
    SizeT block_count = 3;
    SizeT row_count = DEFAULT_VECTOR_SIZE;
    Vector<SharedPtr<ColumnDef>> columns;
    Vector<DataType> column_types;
    columns.reserve(column_count);
    column_types.reserve(column_count);

    DataType col_type = DataType(LogicalType::kBoolean);
    column_types.emplace_back(col_type);
    String col_name = "col1";
    auto col_def = MakeShared<ColumnDef>(0,
                                         col_type,
                                         col_name,
                                         HashSet<ConstraintType>());

    columns.emplace_back(col_def);

    col_type = DataType(LogicalType::kBigInt);
    column_types.emplace_back(col_type);
    col_name = "col2";
    col_def = MakeShared<ColumnDef>(1,
                                    col_type,
                                    col_name,
                                    HashSet<ConstraintType>());
    columns.emplace_back(col_def);

    SharedPtr<TableDef> table_def = TableDef::Make("order_by_table", columns);

    SharedPtr<Table> order_by_table = Table::Make(table_def, TableType::kOrderBy);

    for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        data_block->Init(column_types);

        // Column 1 & 2
        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            Value v1 = Value::MakeBool(row_id % 2 == 0);
            data_block->column_vectors[0]->AppendValue(v1);
            Value v2 = Value::MakeBigInt(row_id);
            data_block->column_vectors[1]->AppendValue(v2);
        }
        data_block->Finalize();
        order_by_table->Append(data_block);
    }

    SharedPtr<Vector<RowID>> offset_column_vector = order_by_table->GetRowIDVector();
    EXPECT_EQ(offset_column_vector->size(), block_count * DEFAULT_VECTOR_SIZE);
    for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
        // Check Column1 data
        SharedPtr<ColumnVector> column1 = order_by_table->GetDataBlockById(block_id)->column_vectors[0];
        EXPECT_EQ(column1->data_type().type(), LogicalType::kBoolean);
        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            EXPECT_EQ(((BooleanT *)column1->data())[row_id], row_id % 2 == 0);
        }

        // Check Column2 data
        SharedPtr<ColumnVector> column2 = order_by_table->GetDataBlockById(block_id)->column_vectors[1];
        EXPECT_EQ(column2->data_type().type(), LogicalType::kBigInt);
        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            EXPECT_EQ(((BigIntT *)column2->data())[row_id], row_id);
        }

        // Check offset
        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            const RowID& row = (*offset_column_vector)[block_id * DEFAULT_VECTOR_SIZE + row_id];
            EXPECT_EQ(row.block, block_id);
            EXPECT_EQ(row.offset, row_id);
        }
    }
}
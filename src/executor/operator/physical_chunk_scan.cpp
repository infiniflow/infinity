//
// Created by JinHai on 2022/9/8.
//

#include "common/utility/infinity_assert.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "common/types/info/varchar_info.h"
#include "expression/value_expression.h"
#include "physical_chunk_scan.h"
#include "main/infinity.h"

namespace infinity {

void
PhysicalChunkScan::Init() {

}

void
PhysicalChunkScan::Execute(SharedPtr<QueryContext>& query_context) {

    switch(scan_type_) {
        case ChunkScanType::kShowTables: {
            // Define output table schema
            Vector<SharedPtr<ColumnDef>> column_defs = {
                    ColumnDef::Make("table_name", 0, DataType(LogicalType::kVarchar), Set<ConstrainType>()),
                    ColumnDef::Make("column_count", 1, DataType(LogicalType::kBigInt), Set<ConstrainType>()),
                    ColumnDef::Make("row_count", 2, DataType(LogicalType::kBigInt), Set<ConstrainType>()),
                    ColumnDef::Make("block_count", 3, DataType(LogicalType::kBigInt), Set<ConstrainType>()),
                    ColumnDef::Make("block_size", 4, DataType(LogicalType::kBigInt), Set<ConstrainType>()),
            };

            outputs_[1] = MakeShared<Table>(MakeShared<TableDef>("Tables", column_defs, false), TableType::kResult);

            // Get tables from catalog
            // TODO: Use context to carry runtime information, such as current schema
            Vector<SharedPtr<Table>> tables = Infinity::instance().catalog()->GetTables("Default");

            // Prepare the output data block
            SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
            auto table_name_type_info_ptr = VarcharInfo::Make(TABLE_NAME_LIMIT);
            Vector<DataType> column_types {
                DataType(LogicalType::kVarchar, table_name_type_info_ptr),
                DataType(LogicalType::kBigInt),
                DataType(LogicalType::kBigInt),
                DataType(LogicalType::kBigInt),
                DataType(LogicalType::kBigInt)
            };

            output_block_ptr->Init(column_types);

            for(i64 i = 0; auto& table: tables) {

                size_t column_id = 0;
                {
                    // Append table name to the 1st column
                    const String& table_name = table->TableName();
                    Value value = Value::MakeVarchar(table_name, table_name_type_info_ptr);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                }

                ++ column_id;
                {
                    // Append column count the 2nd column
                    Value value = Value::MakeBigInt(static_cast<i64>(table->ColumnCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                }

                ++ column_id;
                {
                    // Append row count the 3rd column
                    Value value = Value::MakeBigInt(static_cast<i64>(table->row_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                }

                ++ column_id;
                {
                    // Append block count the 4th column
                    Value value = Value::MakeBigInt(static_cast<i64>(table->DataBlockCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                }

                ++ column_id;
                {
                    // Append block limit the 5th column
                    Value value = Value::MakeBigInt(Infinity::instance().config()->option_.default_row_count_);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                }
            }

            output_block_ptr->Finalize();
            outputs_[table_index_]->Append(output_block_ptr);
            break;
        }
        case ChunkScanType::kShowColumn: {
            break;
        }
        case ChunkScanType::kIntermediate: {
            break;
        }
        default:
            ExecutorError("Invalid chunk scan type");
    }

}

}



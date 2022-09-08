//
// Created by JinHai on 2022/9/8.
//

#include "storage/table_with_fix_row.h"
#include "common/utility/infinity_assert.h"
#include "expression/value_expression.h"
#include "physical_chunk_scan.h"
#include "main/infinity.h"

namespace infinity {

void
PhysicalChunkScan::Execute() {

    switch(scan_type_) {
        case ChunkScanType::kShowTables: {
            // Define output table schema
            std::vector<ColumnDefinition> column_defs = {
                    {"table_name", 0, LogicalType(LogicalTypeId::kVarchar), false, std::set<ConstrainType>()},
                    {"column_count", 1, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
                    {"row_count", 2, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
                    {"block_count", 3, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
                    {"block_size", 4, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
            };

            std::shared_ptr<TableDefinition> table_def_ptr = std::make_shared<TableDefinition>("Tables", column_defs, false);
            output_ = std::make_shared<FixedRowCountTable>(table_def_ptr);

            // Get tables from catalog
            // TODO: Use context to carry runtime information, such as current schema
            std::vector<std::shared_ptr<Table>> tables = Infinity::instance().catalog()->GetTables("Default");

            // Create two value type:
            LogicalType varchar_type = LogicalType(LogicalTypeId::kVarchar);
            LogicalType bigint_type = LogicalType(LogicalTypeId::kBigInt);

            for(std::shared_ptr<Table>& table: tables) {
                TransientBlock transient_block;

                // Append table name
                std::shared_ptr<ValueExpression> table_name
                    = std::make_shared<ValueExpression>(varchar_type, table->table_def()->name());
                table_name->AppendToBlock(transient_block.columns_);

                // Append column count
                std::shared_ptr<ValueExpression> column_count
                    = std::make_shared<ValueExpression>(bigint_type, static_cast<int64_t>(table->table_def()->column_count()));
                column_count->AppendToBlock(transient_block.columns_);

                // Append row count
                std::shared_ptr<ValueExpression> row_count
                    = std::make_shared<ValueExpression>(bigint_type, static_cast<int64_t>(table->row_count()));
                row_count->AppendToBlock(transient_block.columns_);

                // Append block count
                std::shared_ptr<ValueExpression> chunk_count
                    = std::make_shared<ValueExpression>(bigint_type, static_cast<int64_t>(table->block_count()));
                chunk_count->AppendToBlock(transient_block.columns_);

                // Append block size
                // FIXME: fixed block size
                std::shared_ptr<ValueExpression> block_size
                    = std::make_shared<ValueExpression>(bigint_type, static_cast<int64_t>(1024));
                block_size->AppendToBlock(transient_block.columns_);

                transient_block.row_count_ = 1;

                output_->Append(transient_block);
            }
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



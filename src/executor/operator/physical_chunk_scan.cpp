//
// Created by JinHai on 2022/9/8.
//

#include "common/utility/infinity_assert.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "expression/value_expression.h"
#include "physical_chunk_scan.h"
#include "main/infinity.h"
#include "storage/collection.h"

namespace infinity {

void
PhysicalChunkScan::Init() {

}

void
PhysicalChunkScan::Execute(SharedPtr<QueryContext>& query_context) {

    switch(scan_type_) {
        case ChunkScanType::kShowTables: {
            ExecuteShowTable(query_context);
            break;
        }
        case ChunkScanType::kShowColumn: {
            ExecuteShowColumns(query_context);
            break;
        }
        case ChunkScanType::kShowViews: {
            ExecuteShowViews(query_context);
            break;
        }
        case ChunkScanType::kIntermediate: {
            NotImplementError("Intermediate type of chunk scan isn't supported now.")
            break;
        }
        default:
            ExecutorError("Invalid chunk scan type");
    }

}

void
PhysicalChunkScan::ExecuteShowTable(SharedPtr<QueryContext>& query_context) {
    // Define output table schema
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kVarchar), "schema", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, DataType(LogicalType::kVarchar), "table", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(2, DataType(LogicalType::kVarchar), "type", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(3, DataType(LogicalType::kBigInt), "column_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(4, DataType(LogicalType::kBigInt), "row_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(5, DataType(LogicalType::kBigInt), "block_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(6, DataType(LogicalType::kBigInt), "block_size", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Vector<SharedPtr<BaseTable>> tables = Infinity::instance().catalog()->GetTables("default");

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<DataType> column_types {
            DataType(LogicalType::kVarchar),
            DataType(LogicalType::kVarchar),
            DataType(LogicalType::kVarchar),
            DataType(LogicalType::kBigInt),
            DataType(LogicalType::kBigInt),
            DataType(LogicalType::kBigInt),
            DataType(LogicalType::kBigInt)
    };

    output_block_ptr->Init(column_types);

    for(auto& base_table: tables) {

        BaseTableType base_table_type = base_table->kind_;
        size_t column_id = 0;
        {
            // Append schema name to the 1 column
            const String& schema_name = base_table->schema_name();
            Value value = Value::MakeVarchar(schema_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append table name to the 0 column
            const String& table_name = base_table->table_name();
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append base table type to the 2 column
            const String& base_table_type_str = ToString(base_table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column count the 3 column
            switch(base_table_type) {
                case BaseTableType::kTable: {
                    SharedPtr<Table> table = std::static_pointer_cast<Table>(base_table);
                    Value value = Value::MakeBigInt(static_cast<i64>(table->ColumnCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case BaseTableType::kCollection: {
                    SharedPtr<Collection> collection = std::static_pointer_cast<Collection>(base_table);

                    // TODO: column count need to be given for table.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    ExecutorError("Invalid table type");
                }
            }

        }

        ++ column_id;
        {
            // Append row count the 4 column
            switch(base_table_type) {
                case BaseTableType::kTable: {
                    SharedPtr<Table> table = std::static_pointer_cast<Table>(base_table);
                    Value value = Value::MakeBigInt(static_cast<i64>(table->row_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case BaseTableType::kCollection: {
                    SharedPtr<Collection> collection = std::static_pointer_cast<Collection>(base_table);

                    // TODO: row count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    ExecutorError("Invalid table type");
                }
            }
        }

        ++ column_id;
        {
            // Append block count the 5 column
            switch(base_table_type) {
                case BaseTableType::kTable: {
                    SharedPtr<Table> table = std::static_pointer_cast<Table>(base_table);
                    Value value = Value::MakeBigInt(static_cast<i64>(table->DataBlockCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case BaseTableType::kCollection: {
                    SharedPtr<Collection> collection = std::static_pointer_cast<Collection>(base_table);

                    // TODO: block count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    ExecutorError("Invalid table type");
                }
            }
        }

        ++ column_id;
        {
            // Append block limit the 6 column
            Value value = Value::MakeBigInt(Infinity::instance().config()->option_.default_row_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalChunkScan::ExecuteShowViews(SharedPtr<QueryContext>& query_context) {
    // Define output table schema
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kVarchar), "schema", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, DataType(LogicalType::kVarchar), "view", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(3, DataType(LogicalType::kBigInt), "column_count", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = MakeShared<TableDef>("Views", column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Vector<SharedPtr<View>> views = Infinity::instance().catalog()->GetViews("default");

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<DataType> column_types {
            DataType(LogicalType::kVarchar),
            DataType(LogicalType::kVarchar),
            DataType(LogicalType::kBigInt),
    };

    output_block_ptr->Init(column_types);

    for(auto& view: views) {

        size_t column_id = 0;
        {
            // Append schema name to the first column
            String schema_name;
            if(view->schema_name().empty()) {
                schema_name = "default";
            } else {
                schema_name = view->schema_name();
            }
            Value value = Value::MakeVarchar(schema_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append table name to the second column
            const String& table_name = view->view_name();
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column count to the third column
            Value value = Value::MakeBigInt(view->column_names()->size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalChunkScan::ExecuteShowColumns(SharedPtr<QueryContext>& query_context) {

}

}



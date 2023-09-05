//
// Created by JinHai on 2022/9/8.
//

#include "common/utility/infinity_assert.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "expression/value_expression.h"
#include "physical_show.h"
#include "storage/collection.h"

namespace infinity {

void
PhysicalShow::Init() {

}

void
PhysicalShow::Execute(QueryContext* query_context) {

    switch(scan_type_) {
        case ShowType::kShowTables: {
            ExecuteShowTable(query_context);
            break;
        }
        case ShowType::kShowColumn: {
            ExecuteShowColumns(query_context);
            break;
        }
        case ShowType::kShowViews: {
            ExecuteShowViews(query_context);
            break;
        }
        case ShowType::kIntermediate: {
            NotImplementError("Intermediate type of chunk scan isn't supported now.")
            break;
        }
        default:
            ExecutorError("Invalid chunk scan type");
    }

}

void
PhysicalShow::ExecuteShowTable(QueryContext* query_context) {
    // Define output table schema
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "schema", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "table", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(2, varchar_type, "type", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(3, bigint_type, "column_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(4, bigint_type, "row_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(5, bigint_type, "block_count", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(6, bigint_type, "block_size", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Txn* txn = query_context->GetTxn();
    Vector<TableCollectionEntry*> entries = txn->GetTableCollections(schema_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types {
            varchar_type,
            varchar_type,
            varchar_type,
            bigint_type,
            bigint_type,
            bigint_type,
            bigint_type
    };

    output_block_ptr->Init(column_types);

    for(auto& table_collection_entry: entries) {

        TableCollectionType table_type = table_collection_entry->table_collection_type_;
        SizeT column_id = 0;
        {
            // Append schema name to the 1 column
            const String& schema_name = schema_name_;
            Value value = Value::MakeVarchar(schema_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append table name to the 0 column
            const String& table_name = *table_collection_entry->table_collection_name_;
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append base table type to the 2 column
            const String& base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column count the 3 column
            switch(table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_entry->columns_.size()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: column count need to be given for table.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
            }

        }

        ++ column_id;
        {
            // Append row count the 4 column
            switch(table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_entry->row_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
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
            switch(table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_entry->segments_.size()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
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
            SizeT default_row_size = query_context->global_config()->default_row_size();
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalShow::ExecuteShowViews(QueryContext* query_context) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Define output table schema
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "schema", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "view", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(3, bigint_type, "column_count", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Txn* txn = query_context->GetTxn();
    Vector<BaseEntry*> views = txn->GetViews(schema_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types {
            varchar_type,
            varchar_type,
            bigint_type,
    };

    output_block_ptr->Init(column_types);

    for(auto& base_entry: views) {
        ViewEntry* view_entry = static_cast<ViewEntry*>(base_entry);

        SizeT column_id = 0;
        {
            // Append schema name to the first column
            String schema_name = schema_name_;
            Value value = Value::MakeVarchar(schema_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append table name to the second column
            const String& table_name = *view_entry->view_name();
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column count to the third column
            Value value = Value::MakeBigInt(view_entry->column_names()->size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalShow::ExecuteShowColumns(QueryContext* query_context) {

    Txn* txn = query_context->GetTxn();
    EntryResult result = txn->GetTableByName(schema_name_, object_name_);
    if(result.err_ != nullptr) {
        ExecutorError(*result.err_);
    } else {
        if(result.entry_ != nullptr) {
            TableCollectionEntry* table_collection_entry = static_cast<TableCollectionEntry*>(result.entry_);
            ExecuteShowTableDetail(query_context, table_collection_entry->columns_);
        }
    }

    result = txn->GetViewByName(schema_name_, object_name_);
    if(result.err_ != nullptr) {
        ExecutorError(*result.err_);
    }

    ViewEntry* view_entry = static_cast<ViewEntry*>(result.entry_);

    ExecuteShowViewDetail(query_context, view_entry->column_types(), view_entry->column_names());

    ExecutorError(fmt::format("No table, collection, or view name is {}.{}", this->schema_name_, this->object_name_));
}

void
PhysicalShow::ExecuteShowTableDetail(QueryContext* query_context,
                                     const Vector<SharedPtr<ColumnDef>>& table_collecton_columns) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(3, varchar_type, "constraint", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types {
            varchar_type,
            varchar_type,
            varchar_type,
    };

    output_block_ptr->Init(column_types);

    for(auto& column: table_collecton_columns) {

        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column type to the second column
            String column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column constraint to the third column
            String column_constraint;
            for(auto& constraint: column->constraints_) {
                column_constraint += " " + ConstrainTypeToString(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalShow::ExecuteShowViewDetail(QueryContext* query_context,
                                    const SharedPtr<Vector<SharedPtr<DataType>>>& view_column_types,
                                    const SharedPtr<Vector<String>>& view_column_names) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), output_column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> output_column_types {
            varchar_type,
            varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SizeT column_count = view_column_types->size();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_column_names->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column type to the second column
            String column_type = view_column_types->at(idx)->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

}



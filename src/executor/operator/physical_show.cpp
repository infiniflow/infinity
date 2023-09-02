//
// Created by JinHai on 2022/9/8.
//

#include "common/utility/infinity_assert.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "expression/value_expression.h"
#include "physical_show.h"
#include "main/infinity.h"
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
    Vector<SharedPtr<BaseTable>> tables = Infinity::instance().catalog()->GetTables("default");

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

    for(auto& base_table: tables) {

        BaseTableType base_table_type = base_table->kind_;
        SizeT column_id = 0;
        {
            // Append schema name to the 1 column
            const String& schema_name = *base_table->schema_name();
            Value value = Value::MakeVarchar(schema_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append table name to the 0 column
            const String& table_name = *base_table->table_name();
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
            Value value = Value::MakeBigInt(Infinity::instance().config()->default_row_size());
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
    Vector<SharedPtr<View>> views = Infinity::instance().catalog()->GetViews("default");

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types {
            varchar_type,
            varchar_type,
            bigint_type,
    };

    output_block_ptr->Init(column_types);

    for(auto& view: views) {

        SizeT column_id = 0;
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
PhysicalShow::ExecuteShowColumns(QueryContext* query_context) {
    SharedPtr<BaseTable> table_collection = Infinity::instance().catalog()->GetTableByNameNoExcept(this->schema_name_,
                                                                                        this->object_name_);
    if(table_collection != nullptr) {
        // Table or Collection
        switch(table_collection->kind()) {
            case BaseTableType::kInvalid: {
                ExecutorError("Invalid base table type")
            }
            case BaseTableType::kTable: {
                SharedPtr<Table> table = std::static_pointer_cast<Table>(table_collection);
                return ExecuteShowTableDetail(query_context, table);
            }
            case BaseTableType::kCollection: {
                SharedPtr<Collection> collection = std::static_pointer_cast<Collection>(table_collection);
                return ExecuteShowCollectionDetail(query_context, collection);
            }
        }
    }
    SharedPtr<View> view = Infinity::instance().catalog()->GetViewByNameNoExcept(this->schema_name_,
                                                                                 this->object_name_);
    if(view != nullptr) {
        return ExecuteShowViewDetail(query_context, view);
    }

    ExecutorError(fmt::format("No table, collection, or view name is {}.{}", this->schema_name_, this->object_name_));
}

void
PhysicalShow::ExecuteShowTableDetail(QueryContext* query_context,
                                     const SharedPtr<Table>& table_ptr) {
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

    const Vector<SharedPtr<ColumnDef>>& columns = table_ptr->definition_ptr_->columns();
    for(auto& column: columns) {

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
PhysicalShow::ExecuteShowCollectionDetail(QueryContext* query_context,
                                          const SharedPtr<Collection>& collection_ptr) {
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

#if 0
    const Vector<SharedPtr<ColumnDef>>& columns = collection_ptr->definition_ptr_->columns();
    for(auto& column: columns) {

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
            String column_type = column->type().ToString();
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
#endif
    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void
PhysicalShow::ExecuteShowViewDetail(QueryContext* query_context,
                                    const SharedPtr<View>& view_ptr) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types {
            varchar_type,
            varchar_type,
    };

    output_block_ptr->Init(column_types);

    SizeT column_count = view_ptr->column_names()->size();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_ptr->column_names()->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++ column_id;
        {
            // Append column type to the second column
            String column_type = view_ptr->column_types()->at(idx)->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

}



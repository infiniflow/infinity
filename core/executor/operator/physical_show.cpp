//
// Created by JinHai on 2022/9/8.
//

module;

#include <string>

import stl;
import txn;
import query_context;
import parser;
import operator_state;
import data_block;
import infinity_assert;
import infinity_exception;
import table_collection_type;
import value_expression;
import logical_show;
import table_collection_detail;
import view_entry;
import base_entry;
import value;
import table_collection_entry;
import table_def;
import table;
import third_party;

module physical_show;

namespace infinity {

void PhysicalShow::Init() {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (scan_type_) {
        case ShowType::kShowTables: {

            output_names_->reserve(8);
            output_types_->reserve(8);

            output_names_->emplace_back("database");
            output_names_->emplace_back("table");
            output_names_->emplace_back("type");
            output_names_->emplace_back("column_count");
            output_names_->emplace_back("row_count");
            output_names_->emplace_back("segment_count");
            output_names_->emplace_back("block_count");
            output_names_->emplace_back("segment_capacity");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowType::kShowColumn: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("column_name");
            output_names_->emplace_back("column_type");
            output_names_->emplace_back("constraint");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        default: {
            Error<NotImplementException>("Not implemented show type", __FILE_NAME__, __LINE__);
        }
    }
}

void PhysicalShow::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {

    auto show_input_state = (ShowInputState *)(input_state);
    auto show_output_state = (ShowOutputState *)(output_state);

    switch (scan_type_) {
        case ShowType::kShowTables: {
            ExecuteShowTable(query_context, show_input_state, show_output_state);
            break;
        }
        case ShowType::kShowColumn: {
            ExecuteShowColumns(query_context, show_input_state, show_output_state);
            break;
        }
        default: {
            Error<ExecutorException>("Invalid chunk scan type", __FILE_NAME__, __LINE__);
        }
    }

    output_state->SetComplete();
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowTable(QueryContext *query_context, ShowInputState *input_state, ShowOutputState *output_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<TableCollectionDetail> table_collections_detail = txn->GetTableCollections(db_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &table_collection_detail : table_collections_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_collection_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_collection_detail.table_collection_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableCollectionType table_type = table_collection_detail.table_collection_type_;
        {
            // Append base table type to the 2 column
            const String &base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.column_count_));
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

        ++column_id;
        {
            // Append row count the 4 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.row_count_));
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
                    Error<ExecutorException>("Invalid table type", __FILE_NAME__, __LINE__);
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.segment_count_));
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
                    Error<ExecutorException>("Invalid table type", __FILE_NAME__, __LINE__);
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.block_count_));
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
                    Error<ExecutorException>("Invalid table type", __FILE_NAME__, __LINE__);
                }
            }
        }

        ++column_id;
        {
            // Append block limit the 6 column
            SizeT default_row_size = table_collection_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_state->output_.emplace_back(output_block_ptr);
}

/**
 * @brief Execute Show table details statement (i.e. describe t1)
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowInputState *input_state, ShowOutputState *output_state) {

    auto txn = query_context->GetTxn();
    auto result = txn->GetTableByName(db_name_, object_name_);
    output_state->error_message_ = Move(result.err_);
    if (result.entry_ != nullptr) {
        auto table_collection_entry = dynamic_cast<TableCollectionEntry *>(result.entry_);

        auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

        Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
            MakeShared<ColumnDef>(2, varchar_type, "constraint", HashSet<ConstraintType>()),
        };

        SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);

        // create data block for output state
        auto output_block_ptr = DataBlock::Make();
        Vector<SharedPtr<DataType>> column_types{
            varchar_type,
            varchar_type,
            varchar_type,
        };

        output_block_ptr->Init(column_types);

        for (auto &column : table_collection_entry->columns_) {
            SizeT column_id = 0;
            {
                // Append column name to the first column
                Value value = Value::MakeVarchar(column->name());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                // Append column type to the second column, if the column type is embedded type, append the embedded type
                String column_type;
                if (column->type()->type() == kEmbedding) {
                    auto type = column->type();
                    auto embedding_type = type->type_info()->ToString();
                    column_type = Format("{}({})", type->ToString(), embedding_type);

                } else {
                    column_type = column->type()->ToString();
                }
                Value value = Value::MakeVarchar(column_type);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                // Append column constraint to the third column
                String column_constraint;
                for (auto &constraint : column->constraints_) {
                    column_constraint += " " + ConstrainType2String(constraint);
                }

                Value value = Value::MakeVarchar(column_constraint);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            output_block_ptr->Finalize();
        }

        output_state->output_.emplace_back(Move(output_block_ptr));
    }
}

void PhysicalShow::Execute(QueryContext *query_context) {

    switch (scan_type_) {
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
            Error<ExecutorException>("Intermediate type of chunk scan isn't supported now.", __FILE_NAME__, __LINE__);
            break;
        }
        default: {
            Error<ExecutorException>("Invalid chunk scan type", __FILE_NAME__, __LINE__);
        }
    }
}

void PhysicalShow::ExecuteShowTable(QueryContext *query_context) {
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

    auto table_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Txn *txn = query_context->GetTxn();

    Vector<TableCollectionDetail> table_collections_detail = txn->GetTableCollections(db_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &table_collection_detail : table_collections_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_collection_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_collection_detail.table_collection_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableCollectionType table_type = table_collection_detail.table_collection_type_;
        {
            // Append base table type to the 2 column
            const String &base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.column_count_));
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

        ++column_id;
        {
            // Append row count the 4 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.row_count_));
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
                    Error<ExecutorException>("Invalid table type", __FILE_NAME__, __LINE__);
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.segment_count_));
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
                    Error<ExecutorException>("Invalid table type", __FILE_NAME__, __LINE__);
                }
            }
        }

        ++column_id;
        {
            // Append block limit the 6 column
            SizeT default_row_size = table_collection_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowViews(QueryContext *query_context) {
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
    Txn *txn = query_context->GetTxn();
    Vector<BaseEntry *> views = txn->GetViews(db_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &base_entry : views) {
        ViewEntry *view_entry = static_cast<ViewEntry *>(base_entry);

        SizeT column_id = 0;
        {
            // Append schema name to the first column
            String &db_name = db_name_;
            Value value = Value::MakeVarchar(db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the second column
            const String &table_name = *view_entry->view_name();
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
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

void PhysicalShow::ExecuteShowColumns(QueryContext *query_context) {

    Txn *txn = query_context->GetTxn();
    EntryResult result = txn->GetTableByName(db_name_, object_name_);
    if (result.err_.get() != nullptr) {
        Error<ExecutorException>(*result.err_, __FILE_NAME__, __LINE__);
    } else {
        if (result.entry_ != nullptr) {
            TableCollectionEntry *table_collection_entry = dynamic_cast<TableCollectionEntry *>(result.entry_);
            ExecuteShowTableDetail(query_context, table_collection_entry->columns_);
        }
    }

    result = txn->GetViewByName(db_name_, object_name_);
    if (result.err_.get() != nullptr) {
        Error<ExecutorException>(*result.err_, __FILE_NAME__, __LINE__);
    }

    ViewEntry *view_entry = static_cast<ViewEntry *>(result.entry_);

    ExecuteShowViewDetail(query_context, view_entry->column_types(), view_entry->column_names());

    Error<ExecutorException>(Format("No table, collection, or view name is {}.{}", this->db_name_, this->object_name_), __FILE_NAME__, __LINE__);
}

void PhysicalShow::ExecuteShowTableDetail(QueryContext *query_context, const Vector<SharedPtr<ColumnDef>> &table_collecton_columns) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "constraint", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &column : table_collecton_columns) {

        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column
            String column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column constraint to the third column
            String column_constraint;
            for (auto &constraint : column->constraints_) {
                column_constraint += " " + ConstrainType2String(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowViewDetail(QueryContext *query_context,
                                         const SharedPtr<Vector<SharedPtr<DataType>>> &view_column_types,
                                         const SharedPtr<Vector<String>> &view_column_names) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), output_column_defs);
    output_ = MakeShared<Table>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SizeT column_count = view_column_types->size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_column_names->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
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

} // namespace infinity

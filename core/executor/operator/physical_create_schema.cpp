//
// Created by jinhai on 23-3-14.
//

module;

import stl;
import txn;
import query_context;
import table_def;
import table;
import parser;
import physical_operator_type;
import operator_state;

module physical_create_schema;

namespace infinity {

void PhysicalCreateSchema::Init() {}

void PhysicalCreateSchema::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto txn = query_context->GetTxn();
    auto result = txn->CreateDatabase(*schema_name_, conflict_type_);
    auto create_database_output_state = (CreateDatabaseOutputState *)(output_state);
    create_database_output_state->error_message_ = Move(result.err_);
    output_state->SetComplete();
}

void PhysicalCreateSchema::Execute(QueryContext *query_context) {
    //    ResponseError("Execute: Create table: " + table_def_ptr_->name());
    Txn *txn = query_context->GetTxn();
    txn->CreateDatabase(*schema_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity

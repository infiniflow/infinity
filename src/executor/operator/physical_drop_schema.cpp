//
// Created by jinhai on 23-3-14.
//

module;

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import base_entry;

import infinity_exception;

module physical_drop_schema;

namespace infinity {

void PhysicalDropSchema::Init() {}

void PhysicalDropSchema::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {

    auto txn = query_context->GetTxn();
    auto res = txn->DropDatabase(*schema_name_, conflict_type_);
    auto drop_database_output_state = (DropDatabaseOutputState *)(output_state);
    drop_database_output_state->error_message_ = Move(res.err_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    output_state->SetComplete();
}

void PhysicalDropSchema::Execute(QueryContext *query_context) {

    Txn *txn = query_context->GetTxn();
    EntryResult res = txn->DropDatabase(*schema_name_, conflict_type_);
    if (res.err_.get() != nullptr) {
        Error<ExecutorException>(*res.err_);
    }

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity

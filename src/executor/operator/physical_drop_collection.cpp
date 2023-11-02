//
// Created by jinhai on 23-3-16.
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

module physical_drop_collection;

namespace infinity {

void PhysicalDropCollection::Init() {}

void PhysicalDropCollection::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) { output_state->SetComplete(); }

void PhysicalDropCollection::Execute(QueryContext *query_context) {
    Txn *txn = query_context->GetTxn();
    txn->DropTableCollectionByName(*schema_name_, *collection_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity

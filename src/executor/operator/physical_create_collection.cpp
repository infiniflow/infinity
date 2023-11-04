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

module physical_create_collection;

namespace infinity {

PhysicalCreateCollection::PhysicalCreateCollection(SharedPtr<String> schema_name,
                                                   SharedPtr<String> collection_name,
                                                   ConflictType conflict_type,
                                                   SharedPtr<Vector<String>> output_names,
                                                   SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                   u64 table_index,
                                                   u64 id)
    : PhysicalOperator(PhysicalOperatorType::kCreateCollection, nullptr, nullptr, id), schema_name_(Move(schema_name)),
      collection_name_(Move(collection_name)), output_names_(Move(output_names)), output_types_(Move(output_types)), conflict_type_(conflict_type),
      table_index_(table_index) {}

void PhysicalCreateCollection::Init() {}

void PhysicalCreateCollection::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    output_state->SetComplete();
}

void PhysicalCreateCollection::Execute(QueryContext *query_context) {
    Txn *txn = query_context->GetTxn();
    txn->CreateCollection(*schema_name_, *collection_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity

//
// Created by jinhai on 23-3-16.
//

#include "executor/operator/physical_drop_collection.h"
#include "executor/operator_state.h"
#include "main/query_context.h"
#include "parser/definition/table_def.h"
#include "storage/table.h"
#include "storage/txn/txn.h"

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
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity

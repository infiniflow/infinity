//
// Created by jinhai on 23-3-14.
//

#include "physical_drop_schema.h"

namespace infinity {

void
PhysicalDropSchema::Init() {

}

void
PhysicalDropSchema::Execute(QueryContext* query_context) {

    Txn* txn = query_context->GetTxn();
    txn->DropDatabase(*schema_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

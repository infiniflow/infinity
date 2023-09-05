//
// Created by jinhai on 23-3-16.
//

#include "physical_drop_collection.h"

namespace infinity {

void
PhysicalDropCollection::Init() {

}

void
PhysicalDropCollection::Execute(QueryContext* query_context) {
    Txn* txn = query_context->GetTxn();
    txn->DropTableCollectionByName(*schema_name_, *collection_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

//
// Created by jinhai on 23-3-16.
//

#include "physical_drop_collection.h"

#include "main/infinity.h"

namespace infinity {

void
PhysicalDropCollection::Init() {

}

void
PhysicalDropCollection::Execute(SharedPtr<QueryContext>& query_context) {
    Infinity::instance().catalog()->DeleteTable(*schema_name_, *collection_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

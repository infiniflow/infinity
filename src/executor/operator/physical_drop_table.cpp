//
// Created by JinHai on 2022/7/28.
//

#include "physical_drop_table.h"

#include "main/infinity.h"

namespace infinity {

void
PhysicalDropTable::Init() {

}

void
PhysicalDropTable::Execute(std::shared_ptr<QueryContext>& query_context) {
    Infinity::instance().catalog()->DeleteTable(*schema_name_, *table_name_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            ColumnDef::Make("OK", 0, DataType(LogicalType::kInteger), Set<ConstrainType>()),
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs, false);
    outputs_[table_index_] = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

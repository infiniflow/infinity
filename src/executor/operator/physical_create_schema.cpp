//
// Created by jinhai on 23-3-14.
//

#include "physical_create_schema.h"
#include "main/infinity.h"
#include "common/utility/infinity_assert.h"

#include <utility>

namespace infinity {

void
PhysicalCreateSchema::Init() {

}

void
PhysicalCreateSchema::Execute(SharedPtr<QueryContext>& query_context) {
//    ResponseError("Execute: Create table: " + table_def_ptr_->name());
    auto schema_def = MakeShared<SchemaDefinition>(schema_name_, conflict_type_);

    Infinity::instance().catalog()->CreateSchema(schema_def);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

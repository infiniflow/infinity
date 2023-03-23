//
// Created by JinHai on 2022/7/28.
//

#include "physcial_drop_view.h"
#include "main/infinity.h"

namespace infinity {

void
PhysicalDropView::Init() {

}

void
PhysicalDropView::Execute(SharedPtr<QueryContext>& query_context) {
    Infinity::instance().catalog()->DeleteView(*schema_name_, *view_name_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Views", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}

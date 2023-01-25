//
// Created by JinHai on 2022/7/28.
//

#include "physical_limit.h"

namespace infinity {

void
PhysicalLimit::Init() {
    // output table definition is same as input
    SharedPtr<Table> output_table = left_->output();
    SizeT column_count = output_table->ColumnCount();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = output_table->GetColumnTypeById(idx);
        String col_name = output_table->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make("limit", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalLimit::Execute(std::shared_ptr<QueryContext>& query_context) {
    NotImplementError("Limit operator isn't implemented, yet.")
}

}



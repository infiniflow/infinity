//
// Created by JinHai on 2022/7/28.
//

#include "physical_filter.h"

namespace infinity {

void
PhysicalFilter::Init() {
    executor.Init({condition_});

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

    SharedPtr<TableDef> table_def = TableDef::Make("filter", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalFilter::Execute(SharedPtr<QueryContext>& query_context) {
    // Get input from left child
    auto input_table = left_->output();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(1);
    for(SizeT idx = 0; idx < 1; ++ idx) {
        DataType col_type(LogicalType::kBoolean);
        String col_name = "bool_col";

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }
    SharedPtr<TableDef> table_def = TableDef::Make("selected", columns, false);
    SharedPtr<Table> selected_table = Table::Make(table_def, TableType::kIntermediate);

    // Execute the expression on the input table
    executor.Execute(input_table, selected_table);

    output_ = left_->output();
}

}

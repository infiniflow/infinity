//
// Created by JinHai on 2022/7/28.
//

#include "physical_filter.h"

namespace infinity {

void
PhysicalFilter::Init() {
    executor.Init({condition_});

    ExecutorAssert(left()->outputs().size() == 1, "Input table count isn't matched.");

    for(const auto& input_table: left()->outputs()) {
        input_table_ = input_table.second;
        input_table_index_ = input_table.first;
    }

    SizeT column_count = input_table_->ColumnCount();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = input_table_->GetColumnTypeById(idx);
        String col_name = input_table_->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make("filter", columns, false);

    outputs_[input_table_index_] = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalFilter::Execute(SharedPtr<QueryContext>& query_context) {
    // Get input from left child
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
    executor.Select(input_table_, selected_table);

    outputs_[input_table_index_] = selected_table;
}

}

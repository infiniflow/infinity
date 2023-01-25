//
// Created by JinHai on 2022/7/28.
//

#include "physical_sort.h"

namespace infinity {

void
PhysicalSort::Init() {
    executor.Init(this->expressions_);

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

    SharedPtr<TableDef> table_def = TableDef::Make("sort", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalSort::Execute(SharedPtr<QueryContext>& query_context) {

    // Generate table before getting the order of row id.
    SharedPtr<Table> order_by_table = GetOrderTable();

    // Fill the order by table
    this->executor.Execute(left_->output(), order_by_table);

    // Generate offset column
    Vector<SharedPtr<Vector<i32>>> offset_column_vector = order_by_table->GetOffsetVectors();

    // Sort by each table data block and get sorted offset

    // merge each sorted data block and generated row id selection

    // Scatter the input table as the row id selection into output table

    output_ = left_->output();
}

SharedPtr<Table>
PhysicalSort::GetOrderTable() const {
    SizeT column_count = this->expressions_.size();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = this->expressions_[idx]->Type();
        String col_name = this->expressions_[idx]->ToString();

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

//    // offset column is used to indicate which row this data belong to.
//    SharedPtr<ColumnDef> offset_col = ColumnDef::Make("_offset",
//                                                      column_count,
//                                                      DataType(LogicalType::kInteger),
//                                                      Set<ConstrainType>());
//
//    columns.emplace_back(offset_col);

    SharedPtr<TableDef> table_def = TableDef::Make("order_by_key_table", columns, false);

    return Table::Make(table_def, TableType::kIntermediate);
}

}


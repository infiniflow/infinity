//
// Created by JinHai on 2022/7/28.
//

#include "physical_project.h"

namespace infinity {

void
PhysicalProject::Init() {
    executor.Init(expressions_);

    Vector<SharedPtr<ColumnDef>> columns;
    for(i64 idx = 0; auto& expr: expressions_) {
        SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
        columns.emplace_back(col_def);
        ++ idx;
    }
    SharedPtr<TableDef> table_def = TableDef::Make("project", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalProject::Execute(std::shared_ptr<QueryContext>& query_context) {
    // Get input from left child
    auto input_table = left_->output();

    // Execute the expression on the input table
    executor.Execute(input_table, output_);
}

}

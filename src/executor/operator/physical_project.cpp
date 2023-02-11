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

    outputs_[output_table_index_] = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalProject::Execute(SharedPtr<QueryContext>& query_context) {
    if(left_ == nullptr) {
        NotImplementError("Only select list.")
    } else {
        // Get input from left child
        ExecutorAssert(left()->outputs().size() == 1, "Input table count isn't matched.");

        for(const auto& input_table: left()->outputs()) {
            input_table_ = input_table.second;
            input_table_index_ = input_table.first;
        }
        // Execute the expression on the input table
        executor.Execute(input_table_, outputs_[output_table_index_]);
    }
}

}

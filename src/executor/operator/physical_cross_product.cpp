//
// Created by JinHai on 2022/7/28.
//

#include "physical_cross_product.h"

namespace infinity {

void
PhysicalCrossProduct::Init() {
    ExecutorAssert(left()->outputs().size() == 1, "Left Input table count isn't matched.");
    ExecutorAssert(right()->outputs().size() == 1, "Right Input table count isn't matched.");

    for(const auto& input_table: left()->outputs()) {
        left_table_ = input_table.second;
        left_table_index_ = input_table.first;
    }

    for(const auto& input_table: left()->outputs()) {
        right_table_ = input_table.second;
        right_table_index_ = input_table.first;
    }
}

void
PhysicalCrossProduct::Execute(std::shared_ptr<QueryContext>& query_context) {
    Vector<SharedPtr<ColumnDef>> columns_def;
    SizeT left_column_count = left_table_->ColumnCount();
    SizeT right_column_count = right_table_->ColumnCount();
    columns_def.reserve(left_column_count + right_column_count);

    i64 column_idx{0};
    const Vector<SharedPtr<ColumnDef>>& left_column_defs = left_table_->definition_ptr_->columns();
    for(const SharedPtr<ColumnDef>& input_col_def: left_column_defs) {
        SharedPtr<ColumnDef> output_col_def = ColumnDef::Make(input_col_def->name(),
                                                              column_idx,
                                                              input_col_def->type(),
                                                              input_col_def->constrains());
        columns_def.emplace_back(output_col_def);
        ++ column_idx;
    }

    const Vector<SharedPtr<ColumnDef>>& right_column_defs = right_table_->definition_ptr_->columns();
    for(const SharedPtr<ColumnDef>& input_col_def: right_column_defs) {
        SharedPtr<ColumnDef> output_col_def = ColumnDef::Make(input_col_def->name(),
                                                              column_idx,
                                                              input_col_def->type(),
                                                              input_col_def->constrains());
        columns_def.emplace_back(output_col_def);
        ++ column_idx;
    }

    SharedPtr<TableDef> table_def = TableDef::Make("project", columns_def, false);

}

}



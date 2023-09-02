//
// Created by JinHai on 2022/7/28.
//

#include "physical_filter.h"

namespace infinity {

void
PhysicalFilter::Init() {
//    executor.Init({condition_});
//    input_table_ = left_->output();
//    ExecutorAssert(input_table_ != nullptr, "No input.");
//
//    SizeT column_count = input_table_->ColumnCount();
//    Vector<SharedPtr<ColumnDef>> columns;
//    columns.reserve(column_count);
//    for(SizeT idx = 0; idx < column_count; ++ idx) {
//        DataType col_type = input_table_->GetColumnTypeById(idx);
//        String col_name = input_table_->GetColumnNameById(idx);
//
//        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
//        columns.emplace_back(col_def);
//    }
//
//    SharedPtr<TableDef> table_def = TableDef::Make("filter", columns, false);
//
//    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalFilter::Execute(QueryContext* query_context) {
    // Get input from left child
    input_table_ = left_->output();
    ExecutorAssert(input_table_ != nullptr, "No input.");

    // Execute the expression on the input table
    SharedPtr<ExpressionState> condition_state = ExpressionState::CreateState(condition_);

    // Output data block column types
    Vector<SharedPtr<DataType>> output_types;
    SizeT column_count = input_table_->ColumnCount();
    output_types.reserve(column_count);

    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);

    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<DataType> column_type = input_table_->GetColumnTypeById(idx);
        String column_name = input_table_->GetColumnNameById(idx);
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                             column_type,
                                                             column_name,
                                                             HashSet<ConstraintType>());
        columns.emplace_back(col_def);

        output_types.emplace_back(column_type);
    }
    SharedPtr<TableDef> selected_table_def = TableDef::Make(MakeShared<String>("default"),
                                                            MakeShared<String>("selected"),
                                                            columns);
    SharedPtr<Table> selected_table = Table::Make(selected_table_def, TableType::kIntermediate);

    SizeT input_data_block_count = input_table_->DataBlockCount();
    for(SizeT idx = 0; idx < input_data_block_count; ++ idx) {
        SharedPtr<DataBlock> input_data_block = input_table_->GetDataBlockById(idx);
        SizeT row_count = input_data_block->row_count();
        SharedPtr<DataBlock> output_data_block = DataBlock::Make();

        SizeT selected_count = selector_.Select(condition_,
                                                condition_state,
                                                input_data_block,
                                                output_data_block,
                                                row_count);

        if(selected_count > 0) {
            selected_table->Append(output_data_block);
        }
    }

    output_ = selected_table;
}

}

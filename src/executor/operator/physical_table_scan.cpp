//
// Created by JinHai on 2022/7/28.
//

#include "storage/table_with_fix_row.h"
#include "physical_table_scan.h"

namespace infinity {

void
PhysicalTableScan::Init() {
}

void
PhysicalTableScan::Execute(SharedPtr<QueryContext>& query_context) {
    // Generate the result table definition
    Vector<SharedPtr<ColumnDef>> column_defs;
    size_t column_count = column_names_.size();
    for(size_t idx = 0; idx < column_count; ++ idx) {
        // Use the column id to fetch column name and type
        String& col_name_ref = column_names_[idx];
        DataType& col_type_ref = column_types_[idx];

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                             col_type_ref,
                                                             col_name_ref,
                                                             HashSet<ConstraintType>());
        column_defs.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(table_alias_, column_defs);

    output_ = MakeShared<Table>(table_def_ptr, TableType::kResult);

    while(true) {
         SharedPtr<DataBlock> output_block = MakeShared<DataBlock>();
         output_block->Init(column_types_);
         table_scan_func_ptr_->main_function_(
                 query_context,
                 table_scan_function_data_ptr_,
                 *output_block);
        if(output_block->row_count() > 0) {
            output_->Append(output_block);
        } else {
            break;
        }
    }
}

}

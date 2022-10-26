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
PhysicalTableScan::Execute(std::shared_ptr<QueryContext>& query_context) {

    // Generate the result table definition
    std::vector<ColumnDefinition> column_defs;
    int64_t column_count = column_names_.size();
    for(int64_t i = 0; i < column_count; ++ i) {
        // Use the column id to fetch column name and type
        std::string& column_name_ref = column_names_[i];
        LogicalType& column_type_ref = column_types_[i];

        column_defs.emplace_back(column_name_ref,
                                 i,
                                 column_type_ref,
                                 true,
                                 std::set<ConstrainType>());
    }
    std::shared_ptr<TableDefinition> table_def_ptr
            = std::make_shared<TableDefinition>(table_alias_, column_defs, false);
    output_ = std::make_shared<FixedRowCountTable>(table_def_ptr);

    TransBlock output_block(column_types_);
    while(true) {
         table_scan_func_ptr_->main_function_(
                 query_context,
                 table_scan_function_data_ptr_,
                 output_block);
        if(output_block.row_count_ > 0) {
            output_->Append(output_block);
        } else {
            break;
        }
    }
}

}

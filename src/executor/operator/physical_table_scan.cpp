//
// Created by JinHai on 2022/7/28.
//

#include "physical_table_scan.h"

namespace infinity {

void
PhysicalTableScan::Execute(std::shared_ptr<QueryContext>& query_context) {
    TransBlock output_block;
    table_scan_func_ptr_->main_function_(query_context, table_function_data_, output_block);

    // Generate output table
    output_ = nullptr;
}

}
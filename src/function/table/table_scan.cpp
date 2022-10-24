//
// Created by JinHai on 2022/9/14.
//

#include "main/logger.h"
#include "table_scan.h"

namespace infinity {

static void
TableScanFunc(std::shared_ptr<QueryContext>& query_context,
              const std::shared_ptr<TableFunctionData>& table_function_data_ptr,
              TransBlock &output) {

    std::shared_ptr<TableScanFunctionData> table_scan_function_data_ptr
        = std::static_pointer_cast<TableScanFunctionData>(table_function_data_ptr);

    std::shared_ptr<Table>& table_ptr = table_scan_function_data_ptr->table_ptr_;
    std::vector<int64_t>& column_ids = table_scan_function_data_ptr->column_ids_;
    int64_t& current_block_id = table_scan_function_data_ptr->block_count_;
    if(current_block_id >= table_ptr->block_count()) {
        LOG_DEBUG("All blocks are read from storage.");
        output.Reset();
        return ;
    } else {
        std::shared_ptr<Block>& current_block = table_ptr->blocks()[current_block_id];
        int64_t output_column_id = 0;
        for(int64_t& column_id: column_ids) {
            output.chunks_[output_column_id ++ ].Append(current_block->columns()[column_id], 0);
        }
        output.row_count_ = current_block->row_count();
        ++ current_block_id;
    }
}

void
RegisterTableScanFunction(const std::unique_ptr<Catalog> &catalog_ptr) {

    std::shared_ptr<TableScanFunction> seq_scan_ptr = std::make_shared<TableScanFunction>("seq_scan", TableScanFunc);

    catalog_ptr->AddTableFunction(seq_scan_ptr);

}

}

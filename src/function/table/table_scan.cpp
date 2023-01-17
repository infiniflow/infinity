//
// Created by JinHai on 2022/9/14.
//

#include "main/logger.h"
#include "table_scan.h"

namespace infinity {

static void
TableScanFunc(SharedPtr<QueryContext>& query_context,
              const SharedPtr<TableFunctionData>& table_function_data_ptr,
              DataBlock &output) {

    SharedPtr<TableScanFunctionData> table_scan_function_data_ptr
        = std::static_pointer_cast<TableScanFunctionData>(table_function_data_ptr);

    SharedPtr<Table>& table_ptr = table_scan_function_data_ptr->table_ptr_;
    Vector<size_t>& column_ids = table_scan_function_data_ptr->column_ids_;
    i64& current_block_id = table_scan_function_data_ptr->block_count_;
    if(current_block_id >= table_ptr->DataBlockCount()) {
        LOG_DEBUG("All blocks are read from storage.");
        output.Reset();
        return ;
    } else {
        SharedPtr<DataBlock> current_block = table_ptr->GetDataBlockById(current_block_id);
        i64 output_column_id = 0;
        for(size_t column_id: column_ids) {
//            output.column_vectors[output_column_id ++] = current_block->column_vectors[column_id];
            output.column_vectors[output_column_id ++]->ShallowCopy(*current_block->column_vectors[column_id]);
        }
        // Fixme: use set_row_count to save time cost?
        output.Finalize();
        ++ current_block_id;
    }
}

void
RegisterTableScanFunction(const UniquePtr<Catalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> seq_scan_ptr = MakeShared<TableScanFunction>("seq_scan", TableScanFunc);

    catalog_ptr->AddTableFunction(seq_scan_ptr);

}

}

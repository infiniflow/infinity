//
// Created by JinHai on 2022/9/14.
//

#include "main/logger.h"
#include "table_scan.h"

namespace infinity {

static void
TableScanFunc(QueryContext* query_context,
              const SharedPtr<TableFunctionData>& table_function_data_ptr,
              DataBlock &output) {

    SharedPtr<TableScanFunctionData> table_scan_function_data_ptr
        = std::static_pointer_cast<TableScanFunctionData>(table_function_data_ptr);

    TableCollectionEntry* table_column_entry_ptr = table_scan_function_data_ptr->table_entry_ptr_;
    Vector<SizeT>& column_ids = table_scan_function_data_ptr->column_ids_;
//    i64& current_segment_id = table_scan_function_data_ptr->segment_count_;

    for(auto& segment_pair: table_column_entry_ptr->segments_) {
        for(SizeT column_id: column_ids) {
            ObjectHandle col_object = ColumnDataEntry::GetColumnData(segment_pair.second->columns_[column_id].get(),
                                                                     query_context->storage()->buffer_manager());
            output.column_vectors[column_id]->AppendWith(col_object.GetData(), 0, segment_pair.second->current_row_);
        }
    }
    output.Finalize();

//    StorageError("Not implemented");
#if 0
    if(current_block_id >= table_ptr->DataBlockCount()) {
        LOG_TRACE("All blocks are read from storage.");
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
#endif
}

void
RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> seq_scan_ptr = MakeShared<TableScanFunction>("seq_scan", TableScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), seq_scan_ptr);

}

}

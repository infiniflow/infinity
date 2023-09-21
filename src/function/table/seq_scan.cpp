//
// Created by JinHai on 2022/9/14.
//

#include "seq_scan.h"

namespace infinity {

static void
SeqScanFunc(QueryContext* query_context,
            TableFunctionData* table_function_data_ptr,
            DataBlock &output) {

    SeqScanFunctionData* table_scan_function_data_ptr
        = static_cast<SeqScanFunctionData*>(table_function_data_ptr);

    TableCollectionEntry* table_column_entry_ptr = table_scan_function_data_ptr->table_entry_ptr_;
    Vector<SizeT>& column_ids = table_scan_function_data_ptr->column_ids_;

    i64 current_segment_id = table_scan_function_data_ptr->current_segment_id_;
    SizeT read_offset = table_scan_function_data_ptr->read_offset_;

    if (current_segment_id == INITIAL_SEGMENT_ID) {
        auto iter = table_column_entry_ptr->segments_.begin();
        if (iter != table_column_entry_ptr->segments_.end()) {
            current_segment_id = iter->first;
        } else {
            current_segment_id = INVALID_SEGMENT_ID;
        }
    }


    // Here we assume output is a fresh databolck, we have never write anything into it.
    auto write_capacity = output.capacity();
    

    // fill the whole output datablock or we have read all data from table
    while (write_capacity > 0 && current_segment_id != INVALID_SEGMENT_ID) {
        auto iter = table_column_entry_ptr->segments_.find(current_segment_id);
        StorageAssert(iter != table_column_entry_ptr->segments_.end(), "non-exist segment id");
        auto segment_entry = iter->second;
        auto remaining = segment_entry->current_row_ - read_offset;
        auto write_size = std::min(write_capacity, remaining);
        SizeT output_column_ids = 0;
        for (auto column_id : column_ids) {
            // TODO shenyushi
            ColumnBuffer column_buffer = ColumnDataEntry::GetColumnData(segment_entry->columns_[column_id].get(),
                                                                     query_context->storage()->buffer_manager());
            output.column_vectors[output_column_ids++]->AppendWith(column_buffer, read_offset, write_size);
        }
        
        // write_size = already read size = already write size
        write_capacity -= write_size;
        remaining -= write_size;
        read_offset += write_size;

        // we have read all data from current segment, move to next block
        if (remaining == 0) {
            ++iter;
            if (iter != table_column_entry_ptr->segments_.end()) {
                current_segment_id = iter->first;
            } else {
                current_segment_id = INVALID_SEGMENT_ID;
            }
            read_offset = 0;
        }
    }
    output.Finalize();
    table_scan_function_data_ptr->current_segment_id_ = current_segment_id;
    table_scan_function_data_ptr->read_offset_ = read_offset;
}

void
RegisterSeqScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<SeqScanFunction> seq_scan_ptr = MakeShared<SeqScanFunction>("seq_scan", SeqScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), seq_scan_ptr);

}


}

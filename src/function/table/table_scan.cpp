//
// Created by jinhai on 23-9-17.
//

#include "table_scan.h"


namespace infinity {

static void
TableScanFunc(QueryContext* query_context,
              TableFunctionData* table_function_data_ptr,
              DataBlock &output) {

    TableScanFunctionData* table_scan_function_data_ptr
            = static_cast<TableScanFunctionData*>(table_function_data_ptr);
    const Vector<SegmentEntry*>& segment_entries = *table_scan_function_data_ptr->segment_entries_ptr_;
    const Vector<u64>* segment_indexes = table_scan_function_data_ptr->segment_indexes_.get();
    const Vector<SizeT>& column_ids = table_scan_function_data_ptr->column_ids_;


//    table_scan_function_data_ptr->
//    const SharedPtr<Vector<SegmentEntry*>> table_column_entry_ptr = table_scan_function_data_ptr->;
//    const Vector<SizeT>& column_ids = table_scan_function_data_ptr->column_ids_;
//
//    i64 current_segment_id = table_scan_function_data_ptr->current_segment_id_;
//    SizeT read_offset = table_scan_function_data_ptr->read_offset_;
//
//    if (current_segment_id == INITIAL_SEGMENT_ID) {
//        auto iter = table_column_entry_ptr->segments_.begin();
//        if (iter != table_column_entry_ptr->segments_.end()) {
//            current_segment_id = iter->first;
//        } else {
//            current_segment_id = INVALID_SEGMENT_ID;
//        }
//    }
//
//
//    // Here we assume output is a fresh databolck, we have never write anything into it.
//    auto write_capacity = output.capacity();
//
//
//    // fill the whole output datablock or we have read all data from table
//    while (write_capacity > 0 && current_segment_id != INVALID_SEGMENT_ID) {
//        auto iter = table_column_entry_ptr->segments_.find(current_segment_id);
//        StorageAssert(iter != table_column_entry_ptr->segments_.end(), "non-exist segment id");
//        auto segment_entry = iter->second;
//        auto remaining = segment_entry->current_row_ - read_offset;
//        auto write_size = std::min(write_capacity, remaining);
//        SizeT output_column_ids = 0;
//        for (auto column_id : column_ids) {
//
//            ObjectHandle col_object = ColumnDataEntry::GetColumnData(segment_entry->columns_[column_id].get(),
//                                                                     query_context->storage()->buffer_manager());
//            output.column_vectors[output_column_ids++]->AppendWith( col_object.GetData(), read_offset, write_size);
//        }
//
//        // write_size = already read size = already write size
//        write_capacity -= write_size;
//        remaining -= write_size;
//        read_offset += write_size;
//
//        // we have read all data from current segment, move to next block
//        if (remaining == 0) {
//            ++iter;
//            if (iter != table_column_entry_ptr->segments_.end()) {
//                current_segment_id = iter->first;
//            } else {
//                current_segment_id = INVALID_SEGMENT_ID;
//            }
//            read_offset = 0;
//        }
//    }
//    output.Finalize();
//    table_scan_function_data_ptr->current_segment_id_ = current_segment_id;
//    table_scan_function_data_ptr->read_offset_ = read_offset;
}

void
RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> table_scan_ptr = MakeShared<TableScanFunction>("table_scan", TableScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);

}


}

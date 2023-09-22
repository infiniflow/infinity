//
// Created by jinhai on 23-9-17.
//

#include "knn_scan.h"

namespace infinity {

void
KnnScanFunc(QueryContext* query_context,
            TableFunctionData* table_function_data_ptr,
            DataBlock &output) {

    KnnScanFunctionData* knn_scan_function_data_ptr
            = static_cast<KnnScanFunctionData*>(table_function_data_ptr);
    const Vector<SegmentEntry*>& segment_entries = *knn_scan_function_data_ptr->segment_entries_ptr_;
    const Vector<u64>* segment_indexes = knn_scan_function_data_ptr->segment_indexes_.get();
    const Vector<SizeT>& column_ids = knn_scan_function_data_ptr->column_ids_;
    i64& segments_idx = knn_scan_function_data_ptr->segment_idx_offset_;
    SizeT& read_offset = knn_scan_function_data_ptr->current_read_offset_;

    if(segments_idx >= segment_indexes->size()) {
        // No data or all data is read
        return ;
    }

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output.capacity();
    while(write_capacity > 0 && segments_idx < segment_indexes->size()) {
        SegmentEntry* current_segment_entry = segment_entries[segments_idx];
        auto remaining_rows = current_segment_entry->current_row_ - read_offset;
        auto write_size = std::min(write_capacity, remaining_rows);

        SizeT output_column_id{0};
        for (auto column_id : column_ids) {
            ColumnBuffer column_buffer = ColumnDataEntry::GetColumnData(current_segment_entry->columns_[column_id].get(),
                                                                     query_context->storage()->buffer_manager());
            output.column_vectors[output_column_id ++]->AppendWith(column_buffer, read_offset, write_size);
        }

        // write_size = already read size = already write size
        write_capacity -= write_size;
        remaining_rows -= write_size;
        read_offset += write_size;

        // we have read all data from current segment, move to next block
        if (remaining_rows == 0) {
            ++ segments_idx;
            read_offset = 0;
        }
    }
    output.Finalize();
}

void
RegisterKnnScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<KnnScanFunction> table_scan_ptr = MakeShared<KnnScanFunction>("knn_scan", KnnScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);

}


}

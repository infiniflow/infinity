//
// Created by jinhai on 23-9-17.
//

#include "table_scan.h"

namespace infinity {

void TableScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output) {

    TableScanFunctionData *table_scan_function_data_ptr = static_cast<TableScanFunctionData *>(table_function_data_ptr);
    const BlockIndex *block_index = table_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = table_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &column_ids = table_scan_function_data_ptr->column_ids_;
    i64 &block_ids_idx = table_scan_function_data_ptr->current_block_ids_idx_;
    if (block_ids_idx >= block_ids->size()) {
        // No data or all data is read
        return;
    }

    SizeT &read_offset = table_scan_function_data_ptr->current_read_offset_;

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output.capacity();
    while (write_capacity > 0 && block_ids_idx < block_ids->size()) {
        i32 segment_id = block_ids->at(block_ids_idx).segment_id_;
        i16 block_id = block_ids->at(block_ids_idx).block_id_;

        BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);

        auto remaining_rows = current_block_entry->row_count_ - read_offset;
        auto write_size = std::min(write_capacity, remaining_rows);

        SizeT output_column_id{0};
        for (auto column_id : column_ids) {
            ColumnBuffer column_buffer =
                BlockColumnEntry::GetColumnData(current_block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());
            output.column_vectors[output_column_id++]->AppendWith(column_buffer, read_offset, write_size);
        }

        // write_size = already read size = already write size
        write_capacity -= write_size;
        remaining_rows -= write_size;
        read_offset += write_size;

        // we have read all data from current segment, move to next block
        if (remaining_rows == 0) {
            ++block_ids_idx;
            read_offset = 0;
        }
    }
    output.Finalize();
}

void RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> table_scan_ptr = MakeShared<TableScanFunction>("table_scan", TableScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);
}

} // namespace infinity

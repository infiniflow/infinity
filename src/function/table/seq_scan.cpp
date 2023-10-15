//
// Created by JinHai on 2022/9/14.
//

#include "function/table/seq_scan.h"
#include "common/column_vector/column_vector.h"
#include "main/query_context.h"
#include "storage/buffer/column_buffer.h"
#include "storage/data_block.h"
#include "storage/meta/entry/block_column_entry.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "storage/storage.h"

namespace infinity {

static void SeqScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output) {

    SeqScanFunctionData *table_scan_function_data_ptr = static_cast<SeqScanFunctionData *>(table_function_data_ptr);

    TableCollectionEntry *table_column_entry_ptr = table_scan_function_data_ptr->table_entry_ptr_;
    Vector<SizeT> &column_ids = table_scan_function_data_ptr->column_ids_;

    i64 current_segment_id = table_scan_function_data_ptr->current_segment_id_;
    i64 current_block_id = table_scan_function_data_ptr->current_block_id;
    SizeT read_offset = table_scan_function_data_ptr->read_offset_;

    if (current_segment_id == INVALID_SEGMENT_ID) {
        auto iter = table_column_entry_ptr->segments_.begin();
        if (iter != table_column_entry_ptr->segments_.end()) {
            current_segment_id = iter->first;
        } else {
            current_segment_id = INVALID_SEGMENT_ID;
        }
    }

    if (current_block_id == INVALID_BLOCK_ID && current_segment_id != INVALID_SEGMENT_ID) {
        if (table_column_entry_ptr->segments_[current_segment_id]->block_entries_.empty()) {
            current_block_id = INVALID_BLOCK_ID;
        } else {
            current_segment_id = 0;
        }
    }

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output.capacity();

    // fill the whole output data block, or we have read all data from table
    while (write_capacity > 0 && current_segment_id != INVALID_SEGMENT_ID) {
        auto iter = table_column_entry_ptr->segments_.find(current_segment_id);
        StorageAssert(iter != table_column_entry_ptr->segments_.end(), "non-exist segment id");
        SegmentEntry *segment_entry = iter->second.get();

        i64 block_count = segment_entry->block_entries_.size();
        while (write_capacity > 0 && current_block_id < block_count) {
            auto *block_entry = segment_entry->block_entries_[current_block_id].get();
            auto remaining = block_entry->row_count_ - read_offset;

            auto write_size = std::min(write_capacity, remaining);

            for (i64 output_column_id = 0; auto column_id : column_ids) {
                ColumnBuffer column_buffer =
                    BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());
                output.column_vectors[output_column_id++]->AppendWith(column_buffer, read_offset, write_size);
            }

            // write_size = already read size = already write size
            write_capacity -= write_size;
            remaining -= write_size;
            if (remaining == 0) {
                ++current_block_id;
                read_offset = 0;
            } else {
                read_offset += write_size;
            }
        }

        // we have read all data from current segment, move to next segment
        if (write_capacity > 0) {
            ++iter;
            if (iter != table_column_entry_ptr->segments_.end()) {
                current_segment_id = iter->first;
                current_block_id = 0;
                read_offset = 0;
            } else {
                current_segment_id = INVALID_SEGMENT_ID;
            }
        }
    }
    output.Finalize();
    table_scan_function_data_ptr->current_segment_id_ = current_segment_id;
    table_scan_function_data_ptr->current_block_id = current_block_id;
    table_scan_function_data_ptr->read_offset_ = read_offset;
}

void RegisterSeqScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<SeqScanFunction> seq_scan_ptr = MakeShared<SeqScanFunction>("seq_scan", SeqScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), seq_scan_ptr);
}

} // namespace infinity

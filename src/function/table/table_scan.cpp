// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <memory>

import stl;
import table_function;
import data_block;
import table_scan_function_data;
import block_index;
import global_block_id;
import query_context;
import column_buffer;
import catalog;

module table_scan;

namespace infinity {

void TableScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output) {

    TableScanFunctionData *table_scan_function_data_ptr = static_cast<TableScanFunctionData *>(table_function_data_ptr);
    const BlockIndex *block_index = table_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = table_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &column_ids = table_scan_function_data_ptr->column_ids_;
    u64 &block_ids_idx = table_scan_function_data_ptr->current_block_ids_idx_;
    if (block_ids_idx >= block_ids->size()) {
        // No data or all data is read
        return;
    }

    SizeT &read_offset = table_scan_function_data_ptr->current_read_offset_;

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output.capacity();
    while (write_capacity > 0 && block_ids_idx < block_ids->size()) {
        u32 segment_id = block_ids->at(block_ids_idx).segment_id_;
        u16 block_id = block_ids->at(block_ids_idx).block_id_;

        BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);

        auto remaining_rows = current_block_entry->row_count() - read_offset;
        auto write_size = Min(write_capacity, remaining_rows);

        SizeT output_column_id{0};
        for (auto column_id : column_ids) {
            ColumnBuffer column_buffer =
                current_block_entry->GetColumnBlockEntry(column_id)->GetColumnData(query_context->storage()->buffer_manager());
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

SharedPtr<TableScanFunction> TableScanFunction::Make(NewCatalog *catalog, const String &func_name) {
    SharedPtr<TableFunction> table_func = NewCatalog::GetTableFunctionByName(catalog, func_name);
    SharedPtr<TableScanFunction> table_scan_func = std::static_pointer_cast<TableScanFunction>(table_func);
    return table_scan_func;
}

void RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> table_scan_ptr = MakeShared<TableScanFunction>("table_scan", TableScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);
}

} // namespace infinity

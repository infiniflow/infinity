//
// Created by JinHai on 2022/9/14.
//

#pragma once
//#include <utility>

#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/meta/catalog.h"
#include "storage/txn/constants.h"

namespace infinity {

void
RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

class TableScanFunctionData: public TableFunctionData {
public:
    TableScanFunctionData(TableCollectionEntry* table_entry_ptr, Vector<SizeT> column_ids)
        : table_entry_ptr_(table_entry_ptr),
        column_ids_(std::move(column_ids))
    {}

    TableCollectionEntry* table_entry_ptr_;
    Vector<SizeT> column_ids_;

    // How many segments are scanned.
    i64 segment_count_{0};

    i64 current_segment_id_{INITIAL_SEGMENT_ID};
};

class TableScanFunction : public TableFunction {
public:
    static inline SharedPtr<TableScanFunction>
    Make(NewCatalog* catalog, const String& func_name) {
        SharedPtr<TableFunction> table_func = NewCatalog::GetTableFunctionByName(catalog, func_name);
        SharedPtr<TableScanFunction> table_scan_func = std::static_pointer_cast<TableScanFunction>(table_func);
        return table_scan_func;
    }

public:
    explicit
    TableScanFunction(std::string name, TableFunctionType function)
        : TableFunction(std::move(name), {}, std::move(function))
        {}

private:
};

static void
TableScanFunc(QueryContext* query_context,
              const SharedPtr<TableFunctionData>& table_function_data_ptr,
              DataBlock &output) {

    SharedPtr<TableScanFunctionData> table_scan_function_data_ptr
        = std::static_pointer_cast<TableScanFunctionData>(table_function_data_ptr);

    TableCollectionEntry* table_column_entry_ptr = table_scan_function_data_ptr->table_entry_ptr_;
    Vector<SizeT>& column_ids = table_scan_function_data_ptr->column_ids_;

    i64 current_segment_id = table_scan_function_data_ptr->current_segment_id_;
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
        auto remaining = segment_entry->Remaining();
        auto write_size = std::min(write_capacity, remaining);
        SizeT output_column_ids = 0;
        for (auto column_id : column_ids) {

            ObjectHandle col_object = ColumnDataEntry::GetColumnData(segment_entry->columns_[column_id].get(),
                                                                     query_context->storage()->buffer_manager());
            output.column_vectors[output_column_ids++]->AppendWith( col_object.GetData(), segment_entry->read_offset(), write_size);
        }
        
        // write_size = already read size = already write size
        write_capacity -= write_size;
        remaining -= write_size;
        segment_entry->AdvanceReadOffset(write_size);

        // we have read all data from current segment, move to next block
        if (remaining == 0) {
            ++iter;
            if (iter != table_column_entry_ptr->segments_.end()) {
                current_segment_id = iter->first;
            } else {
                current_segment_id = INVALID_SEGMENT_ID;
            }
        }
    }
    output.Finalize();
    table_scan_function_data_ptr->current_segment_id_ = current_segment_id;
}

}
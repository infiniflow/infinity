//
// Created by JinHai on 2022/9/14.
//

#pragma once
//#include <utility>


#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/meta/catalog.h"
#include "storage/txn/txn_state.h"

namespace infinity {

void
RegisterSeqScanFunction(const UniquePtr<NewCatalog>& catalog_ptr);

class SeqScanFunctionData : public TableFunctionData {
public:
    SeqScanFunctionData(TableCollectionEntry* table_entry_ptr,
                        SharedPtr<Vector<SegmentEntry*>> segment_entries,
                        Vector<SizeT> column_ids)
            : table_entry_ptr_(table_entry_ptr),
              segment_entries_(std::move(segment_entries)),
              column_ids_(std::move(column_ids)) {}

    TableCollectionEntry* table_entry_ptr_;
    Vector<SizeT> column_ids_;

    // How many segments are scanned.
    SharedPtr<Vector<SegmentEntry*>> segment_entries_{};

    i64 segment_count_{0};

    i64 current_segment_id_{INVALID_SEGMENT_ID};
    i64 current_block_id{INVALID_BLOCK_ID};
    SizeT read_offset_{0};
};

class SeqScanFunction : public TableFunction {
public:
    static inline SharedPtr<SeqScanFunction>
    Make(NewCatalog* catalog, const String& func_name) {
        SharedPtr<TableFunction> table_func = NewCatalog::GetTableFunctionByName(catalog, func_name);
        SharedPtr<SeqScanFunction> seq_scan_func = std::static_pointer_cast<SeqScanFunction>(table_func);
        return seq_scan_func;
    }

public:
    explicit
    SeqScanFunction(String name, TableFunctionType function)
            : TableFunction(std::move(name), {}, std::move(function)) {}

private:
};

static void
SeqScanFunc(QueryContext* query_context,
            TableFunctionData* table_function_data_ptr,
            DataBlock& output);

}
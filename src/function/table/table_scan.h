//
// Created by JinHai on 2022/9/14.
//

#pragma once
//#include <utility>

#include "common/types/internal_types.h"
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
    SizeT read_offset_{0};
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
              DataBlock &output);

}
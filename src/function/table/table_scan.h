//
// Created by jinhai on 23-9-17.
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
    TableScanFunctionData(const Vector<SegmentEntry*>* segment_entries_ptr,
                          const SharedPtr<Vector<u64>>& segment_indexes,
                          const Vector<SizeT>& column_ids)
            : segment_entries_ptr_(segment_entries_ptr),
            segment_indexes_(segment_indexes),
            column_ids_(column_ids)
    {}

    const Vector<SegmentEntry*>* segment_entries_ptr_{};
    const SharedPtr<Vector<u64>>& segment_indexes_{};
    const Vector<SizeT>& column_ids_{};

    i64 current_segment_id_{INITIAL_SEGMENT_ID};
    SizeT current_read_offset{0};
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
    TableScanFunction(String name, TableFunctionType function)
            : TableFunction(std::move(name), {}, std::move(function))
    {}

private:
};

static void
TableScanFunc(QueryContext* query_context,
              TableFunctionData* table_function_data_ptr,
              DataBlock &output);

}

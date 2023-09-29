//
// Created by jinhai on 23-9-17.
//


#pragma once
//#include <utility>

#include "table_scan_data.h"

namespace infinity {

void
RegisterTableScanFunction(const UniquePtr<NewCatalog>& catalog_ptr);

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
            : TableFunction(std::move(name), {}, std::move(function)) {}

private:
};

static void
TableScanFunc(QueryContext* query_context,
              TableFunctionData* table_function_data_ptr,
              DataBlock& output);

}

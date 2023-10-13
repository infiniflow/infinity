//
// Created by jinhai on 23-9-17.
//

#pragma once

#include "table_scan_data.h"

namespace infinity {

class NewCatalog;

void RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

class TableScanFunction : public TableFunction {
public:
    static SharedPtr<TableScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    explicit TableScanFunction(String name, TableFunctionType function) : TableFunction(std::move(name), {}, std::move(function)) {}

private:
};

static void TableScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output);

} // namespace infinity

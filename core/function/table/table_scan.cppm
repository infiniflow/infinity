//
// Created by jinhai on 23-9-17.
//

module;

import stl;
import new_catalog;
import table_function;
import query_context;
import data_block;

export module table_scan;

namespace infinity {

void RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

class TableScanFunction : public TableFunction {
public:
    static SharedPtr<TableScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    explicit TableScanFunction(String name, TableFunctionType function) : TableFunction(Move(name), {}, Move(function)) {}

private:
};

static void TableScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output);

} // namespace infinity

//
// Created by jinhai on 23-9-17.
//

module;

import stl;
import new_catalog;
import table_function;
import query_context;
import data_block;

export module knn_scan;

namespace infinity {

export void RegisterKnnScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

export class KnnScanFunction : public TableFunction {
public:
    static SharedPtr<KnnScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    inline explicit KnnScanFunction(String name, TableFunctionType function) : TableFunction(Move(name), {}, Move(function)) {}

private:
};

static void KnnScanFunc(QueryContext *query_context, TableFunctionData *knn_function_data_ptr, DataBlock &output);

} // namespace infinity

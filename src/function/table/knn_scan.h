//
// Created by jinhai on 23-9-17.
//


#pragma once
//#include <utility>

#include "knn_scan_data.h"

namespace infinity {

void
RegisterKnnScanFunction(const UniquePtr<NewCatalog>& catalog_ptr);

class KnnScanFunction : public TableFunction {
public:
    static inline SharedPtr<KnnScanFunction>
    Make(NewCatalog* catalog, const String& func_name) {
        SharedPtr<TableFunction> table_func = NewCatalog::GetTableFunctionByName(catalog, func_name);
        SharedPtr<KnnScanFunction> knn_scan_func = std::static_pointer_cast<KnnScanFunction>(table_func);
        return knn_scan_func;
    }

public:
    explicit
    KnnScanFunction(String name, TableFunctionType function)
            : TableFunction(std::move(name), {}, std::move(function)) {}

private:
};

static void
KnnScanFunc(QueryContext* query_context,
            TableFunctionData* knn_function_data_ptr,
            DataBlock& output);

}

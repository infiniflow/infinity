//
// Created by jinhai on 23-9-17.
//

#pragma once

#include "knn_scan_data.h"

namespace infinity {

class NewCatalog;

void RegisterKnnScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

class KnnScanFunction : public TableFunction {
public:
    static SharedPtr<KnnScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    explicit KnnScanFunction(String name, TableFunctionType function) : TableFunction(std::move(name), {}, std::move(function)) {}

private:
};

static void KnnScanFunc(QueryContext *query_context, TableFunctionData *knn_function_data_ptr, DataBlock &output);

} // namespace infinity

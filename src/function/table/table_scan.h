//
// Created by JinHai on 2022/9/14.
//

#pragma once
#include <utility>

#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterTableScanFunction(const std::unique_ptr<Catalog> &catalog_ptr);

class TableScanFunctionData: public TableFunctionData {
public:

private:
    // How many chunk is scanned.
    int64_t chunk_count_{0};
};

class TableScanFunction : public TableFunction {
public:
    explicit TableScanFunction(std::string name, TableFunctionType function)
        : TableFunction(std::move(name), {}, std::move(function))
        {}

private:
};

}





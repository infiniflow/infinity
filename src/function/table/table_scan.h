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
    TableScanFunctionData(std::shared_ptr<Table> table_ptr, std::vector<size_t> column_ids)
        : table_ptr_(table_ptr),
        column_ids_(column_ids)
    {}

    std::shared_ptr<Table> table_ptr_;
    std::vector<size_t> column_ids_;

    // How many block is scanned.
    i64 block_count_{0};
};

class TableScanFunction : public TableFunction {
public:
    explicit TableScanFunction(std::string name, TableFunctionType function)
        : TableFunction(std::move(name), {}, std::move(function))
        {}

private:
};

}





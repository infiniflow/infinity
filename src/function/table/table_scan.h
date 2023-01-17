//
// Created by JinHai on 2022/9/14.
//

#pragma once
//#include <utility>

#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/catalog.h"
#include "main/infinity.h"

namespace infinity {

void
RegisterTableScanFunction(const UniquePtr<Catalog> &catalog_ptr);

class TableScanFunctionData: public TableFunctionData {
public:
    TableScanFunctionData(SharedPtr<Table> table_ptr, Vector<SizeT> column_ids)
        : table_ptr_(std::move(table_ptr)),
        column_ids_(std::move(column_ids))
    {}

    SharedPtr<Table> table_ptr_;
    Vector<SizeT> column_ids_;

    // How many block is scanned.
    i64 block_count_{0};
};

class TableScanFunction : public TableFunction {
public:
    static inline SharedPtr<TableScanFunction>
    Make(const String& func_name) {
        SharedPtr<TableFunction> table_func = Infinity::instance().catalog()->GetTableFunctionByName(func_name);
        SharedPtr<TableScanFunction> table_scan_func = std::static_pointer_cast<TableScanFunction>(table_func);
        return table_scan_func;
    }

public:
    explicit TableScanFunction(std::string name, TableFunctionType function)
        : TableFunction(std::move(name), {}, std::move(function))
        {}

private:
};

}





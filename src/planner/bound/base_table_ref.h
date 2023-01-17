//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"
#include "planner/node/logical_table_scan.h"

#include <utility>

namespace infinity {

class BaseTableRef: public TableRef {
public:
    explicit BaseTableRef(SharedPtr<TableScanFunction> table_func,
                          SharedPtr<TableScanFunctionData> table_func_data,
                          const String& alias,
                          u64 table_index,
                          const Vector<String>& column_names,
                          const Vector<DataType>& column_types)
        : TableRef(TableRefType::kBaseTable, alias),
          table_scan_func_(std::move(table_func)),
          table_scan_function_data_(std::move(table_func_data)),
          table_index_(table_index),
          column_names_(column_names),
          column_types_(column_types)
          {}

    SharedPtr<TableScanFunction> table_scan_func_{};
    SharedPtr<TableScanFunctionData> table_scan_function_data_{};
    u64 table_index_{};
    Vector<String> column_names_{};
    Vector<DataType> column_types_{};
};

}
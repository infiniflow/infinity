//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "function/table/table_scan.h"

#include "storage/table.h"

namespace infinity {

class LogicalTableScan : public LogicalNode {
public:
    explicit
    LogicalTableScan(SharedPtr<Table> table_ptr,
                     SharedPtr<BindContext>& bind_context,
                     SharedPtr<TableScanFunction> table_scan_func,
                     String table_alias,
                     Vector<String> column_names,
                     Vector<DataType> column_types);

    [[nodiscard]] SharedPtr<Table>
    table_ptr() const {
        return table_ptr_;
    }

    String
    ToString(i64& space) final;

    String table_alias_;
    Vector<String> column_names_;
    Vector<DataType> column_types_;

    SharedPtr<TableScanFunction> table_scan_func_ptr_;
private:
    SharedPtr<Table> table_ptr_{nullptr};

};

}


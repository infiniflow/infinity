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
    LogicalTableScan(std::shared_ptr<Table> table_ptr,
                     std::shared_ptr<BindContext>& bind_context,
                     std::shared_ptr<TableScanFunction> table_scan_func,
                     std::string table_alias,
                     std::vector<std::string> column_names,
                     std::vector<LogicalType> column_types);

    [[nodiscard]] std::shared_ptr<Table> table_ptr() const { return table_ptr_; }
    std::string ToString(int64_t& space) final;

    std::string table_alias_;
    std::vector<std::string> column_names_;
    std::vector<LogicalType> column_types_;
    std::vector<int64_t> columns_;

    std::shared_ptr<TableScanFunction> table_scan_func_ptr_;
private:
    std::shared_ptr<Table> table_ptr_{nullptr};

};

}


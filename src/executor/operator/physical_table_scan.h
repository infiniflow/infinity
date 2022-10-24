//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"
#include "function/table/table_scan.h"

namespace infinity {

class PhysicalTableScan : public PhysicalOperator{
public:
    explicit PhysicalTableScan(uint64_t id,
                               std::string table_alias,
                               std::vector<std::string> column_names,
                               std::vector<LogicalType> column_types,
                               std::shared_ptr<TableScanFunction> table_scan_function_ptr)
        : PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr,id),
          table_alias_(std::move(table_alias)),
          column_names_(std::move(column_names)),
          column_types_(std::move(column_types)),
          table_scan_func_ptr_(table_scan_function_ptr)
          {}
    ~PhysicalTableScan() = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;

private:
    std::string table_alias_;
    std::vector<std::string> column_names_;
    std::vector<LogicalType> column_types_;
    std::shared_ptr<TableScanFunction> table_scan_func_ptr_{nullptr};

    TableFunctionData table_function_data_;
};


}

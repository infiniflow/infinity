//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_operator.h"

#include "storage/table.h"

namespace infinity {

class LogicalTableScan : public LogicalOperator {
public:
    explicit LogicalTableScan(std::shared_ptr<Table> table_ptr)
    : LogicalOperator(LogicalOperatorType::kTableScan), table_ptr_(std::move(table_ptr)) {}

    [[nodiscard]] std::shared_ptr<Table> table_ptr() const { return table_ptr_; }
    std::string ToString(uint64_t space) final;
private:
    std::shared_ptr<Table> table_ptr_{nullptr};

};

}


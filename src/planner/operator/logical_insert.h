//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "storage/table.h"
#include "planner/logical_operator.h"

namespace infinity {

class LogicalInsert : public LogicalOperator {
public:
    explicit LogicalInsert(uint64_t node_id, std::shared_ptr<Table> table_ptr)
            : LogicalOperator(LogicalOperatorType::kInsert, node_id),
              table_ptr_(std::move(table_ptr)) {};

    std::string ToString(uint64_t space) final;

private:
    std::shared_ptr<Table> table_ptr_;
};

}

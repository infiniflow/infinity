//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "expression/base_expression.h"
#include "storage/table.h"
#include "planner/logical_node.h"

namespace infinity {

class LogicalInsert : public LogicalNode {
public:
    explicit LogicalInsert(std::shared_ptr<Table> table_ptr, std::vector<std::shared_ptr<BaseExpression>> value_list)
            : LogicalNode(LogicalNodeType::kInsert),
              table_ptr_(std::move(table_ptr)), value_list_(std::move(value_list)) {};

    std::string ToString(uint64_t space) final;
    void set_value_list(const std::vector<std::shared_ptr<BaseExpression>>& value_list) {
        value_list_ = value_list;
    }

    std::shared_ptr<Table>& table_ptr() { return table_ptr_; }
    std::vector<std::shared_ptr<BaseExpression>>& value_list() { return value_list_; }

private:
    std::shared_ptr<Table> table_ptr_;
    std::vector<std::shared_ptr<BaseExpression>> value_list_;
};

}

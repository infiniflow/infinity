//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "expression/base_expression.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalInsert : public PhysicalOperator {
public:
    explicit PhysicalInsert(uint64_t id, std::shared_ptr<Table> table_ptr, std::vector<std::shared_ptr<BaseExpression>> value_list)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id),
          table_ptr_(std::move(table_ptr)),
          value_list_(std::move(value_list)) {}
    ~PhysicalInsert() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;

private:
    std::shared_ptr<Table> table_ptr_;
    std::vector<std::shared_ptr<BaseExpression>> value_list_;
};

}

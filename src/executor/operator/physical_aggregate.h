//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalAggregate : public PhysicalOperator{
public:
    explicit PhysicalAggregate(uint64_t id) : PhysicalOperator(PhysicalOperatorType::kAggregate, nullptr, nullptr, id) {}
    ~PhysicalAggregate() = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}


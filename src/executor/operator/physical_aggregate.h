//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalAggregate : public PhysicalOperator{
public:
    explicit PhysicalAggregate(u64 id) : PhysicalOperator(PhysicalOperatorType::kAggregate, nullptr, nullptr, id) {}
    ~PhysicalAggregate() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;
};

}


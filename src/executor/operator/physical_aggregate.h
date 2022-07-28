//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalAggregate : public PhysicalOperator{
public:
    PhysicalAggregate() : PhysicalOperator(PhysicalOperatorType::kAggregate, nullptr, nullptr) {}
    ~PhysicalAggregate() = default;
};

}


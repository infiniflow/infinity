//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalLimit : public PhysicalOperator {
public:
    PhysicalLimit() : PhysicalOperator(PhysicalOperatorType::kLimit, nullptr, nullptr) {}
    ~PhysicalLimit() = default;
};

}

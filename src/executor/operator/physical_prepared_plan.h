//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalPreparedPlan : public PhysicalOperator {
public:
    PhysicalPreparedPlan(): PhysicalOperator(PhysicalOperatorType::kPreparedPlan, nullptr, nullptr) {}
    ~PhysicalPreparedPlan() = default;

};

}


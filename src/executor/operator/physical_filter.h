//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalFilter : public PhysicalOperator {
public:
    PhysicalFilter(): PhysicalOperator(PhysicalOperatorType::kFilter, nullptr, nullptr) {}
    ~PhysicalFilter() = default;
};


}

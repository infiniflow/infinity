//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalSort : public PhysicalOperator {
public:
    PhysicalSort(): PhysicalOperator(PhysicalOperatorType::kSort, nullptr, nullptr) {}
    ~PhysicalSort() = default;
};

}


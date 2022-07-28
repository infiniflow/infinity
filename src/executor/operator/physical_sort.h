//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalSort : public PhysicalOperator {
public:
    explicit PhysicalSort(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kSort, nullptr, nullptr, id) {}
    ~PhysicalSort() = default;
};

}


//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDelete : public PhysicalOperator {
public:
    explicit PhysicalDelete(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kDelete, nullptr, nullptr, id) {}
    ~PhysicalDelete() = default;
};

}
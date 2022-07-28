//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDelete : public PhysicalOperator {
public:
    PhysicalDelete(): PhysicalOperator(PhysicalOperatorType::kDelete, nullptr, nullptr) {}
    ~PhysicalDelete() = default;
};

}
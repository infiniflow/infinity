//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalInsert : public PhysicalOperator {
public:
    PhysicalInsert() : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr) {}
    ~PhysicalInsert() = default;
};

}

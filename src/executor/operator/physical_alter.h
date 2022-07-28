//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalAlter : public PhysicalOperator{
public:
    PhysicalAlter() : PhysicalOperator(PhysicalOperatorType::kAlter, nullptr, nullptr) {}
    ~PhysicalAlter() = default;
};

}
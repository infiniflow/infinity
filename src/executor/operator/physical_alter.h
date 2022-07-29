//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalAlter : public PhysicalOperator{
public:
    explicit PhysicalAlter(uint64_t id) : PhysicalOperator(PhysicalOperatorType::kAlter, nullptr, nullptr, id) {}
    ~PhysicalAlter() = default;

    void Execute() override;
};

}
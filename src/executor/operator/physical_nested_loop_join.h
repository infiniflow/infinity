//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalNestedLoopJoin : public PhysicalOperator {
public:
    explicit PhysicalNestedLoopJoin(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kJoinNestedLoop, nullptr, nullptr, id) {}
    ~PhysicalNestedLoopJoin() = default;

    void Execute() override;
};

}
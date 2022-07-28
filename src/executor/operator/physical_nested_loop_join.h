//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalNestedLoopJoin : public PhysicalOperator {
public:
    PhysicalNestedLoopJoin(): PhysicalOperator(PhysicalOperatorType::kJoinNestedLoop, nullptr, nullptr) {}
    ~PhysicalNestedLoopJoin() = default;
};

}
//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalIndexJoin : public PhysicalOperator {
public:
    PhysicalIndexJoin(): PhysicalOperator(PhysicalOperatorType::kJoinIndex, nullptr, nullptr) {}
    ~PhysicalIndexJoin() = default;
};

}

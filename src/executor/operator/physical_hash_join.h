//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalHashJoin : public PhysicalOperator {
public:
    PhysicalHashJoin(): PhysicalOperator(PhysicalOperatorType::kJoinHash, nullptr, nullptr) {}
    ~PhysicalHashJoin() = default;
};

}

//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDummyOperator : public PhysicalOperator {
public:
    PhysicalDummyOperator() : PhysicalOperator(PhysicalOperatorType::kInvalid, nullptr, nullptr) {}
    ~PhysicalDummyOperator() override = default;
};

}

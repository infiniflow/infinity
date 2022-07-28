//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalProject : public PhysicalOperator {
public:
    PhysicalProject() : PhysicalOperator(PhysicalOperatorType::kProjection, nullptr, nullptr) {}
    ~PhysicalProject() = default;
};

}



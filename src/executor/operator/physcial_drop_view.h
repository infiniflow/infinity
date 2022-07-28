//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {
class PhysicalDropView : public PhysicalOperator {
public:
    PhysicalDropView() : PhysicalOperator(PhysicalOperatorType::kDropView, nullptr, nullptr) {}
    ~PhysicalDropView() = default;
};

}

//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalCreateView : public PhysicalOperator {
public:
    PhysicalCreateView(): PhysicalOperator(PhysicalOperatorType::kCreateView, nullptr, nullptr) {}
    ~PhysicalCreateView() = default;
};

}


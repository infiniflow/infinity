//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalProduct : public PhysicalOperator {
public:
    PhysicalProduct(): PhysicalOperator(PhysicalOperatorType::kProduct, nullptr, nullptr) {}
    ~PhysicalProduct() = default;
};

}

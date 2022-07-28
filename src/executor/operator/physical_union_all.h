//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalUnionAll : public PhysicalOperator {
public:
    PhysicalUnionAll(): PhysicalOperator(PhysicalOperatorType::kUnionAll, nullptr, nullptr) {}
    ~PhysicalUnionAll() = default;
};

}

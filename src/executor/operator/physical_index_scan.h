//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalIndexScan : public PhysicalOperator {
public:
    PhysicalIndexScan(): PhysicalOperator(PhysicalOperatorType::kIndexScan, nullptr, nullptr) {}
    ~PhysicalIndexScan() = default;
};

}

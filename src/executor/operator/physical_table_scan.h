//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalTableScan : public PhysicalOperator{
public:
    PhysicalTableScan(): PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr) {}
    ~PhysicalTableScan() = default;
};


}

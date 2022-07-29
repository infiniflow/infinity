//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalTableScan : public PhysicalOperator{
public:
    explicit PhysicalTableScan(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr,id) {}
    ~PhysicalTableScan() = default;
    void Execute() override;
};


}

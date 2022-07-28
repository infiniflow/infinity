//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropTable : public PhysicalOperator {
public:
    explicit PhysicalDropTable(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kDropTable, nullptr, nullptr, id) {}

    ~PhysicalDropTable() = default;
};

}


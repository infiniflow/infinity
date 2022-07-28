//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropTable : public PhysicalOperator {
public:
    PhysicalDropTable(): PhysicalOperator(PhysicalOperatorType::kDropTable, nullptr, nullptr) {}

    ~PhysicalDropTable() = default;
};

}


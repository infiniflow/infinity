//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalExport : public PhysicalOperator {
public:
    PhysicalExport(): PhysicalOperator(PhysicalOperatorType::kExport, nullptr, nullptr) {}
    ~PhysicalExport() = default;
};

}

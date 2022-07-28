//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalImport : public PhysicalOperator {
public:
    PhysicalImport(): PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr) {}
    ~PhysicalImport() = default;
};


}
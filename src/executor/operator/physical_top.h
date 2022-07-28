//
// Created by JinHai on 2022/7/28.
//

#pragma

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalTop : public PhysicalOperator {
public:
    PhysicalTop(): PhysicalOperator(PhysicalOperatorType::kTop, nullptr, nullptr) {}
    ~PhysicalTop() = default;
};

}


//
// Created by JinHai on 2022/7/28.
//

#pragma

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalUpdate : public PhysicalOperator{
public:
    PhysicalUpdate(): PhysicalOperator(PhysicalOperatorType::kUpdate, nullptr, nullptr) {}
    ~PhysicalUpdate() override = default;
};

}

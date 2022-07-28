//
// Created by JinHai on 2022/7/28.
//

#pragma

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalUpdate : public PhysicalOperator{
public:
    explicit PhysicalUpdate(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kUpdate, nullptr, nullptr, id) {}
    ~PhysicalUpdate() override = default;
};

}

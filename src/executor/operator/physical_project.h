//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kProjection, nullptr, nullptr, id) {}
    ~PhysicalProject() = default;

    void Execute() override;
};

}



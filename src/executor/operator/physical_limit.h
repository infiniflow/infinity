//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalLimit : public PhysicalOperator {
public:
    explicit PhysicalLimit(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kLimit, nullptr, nullptr, id) {}
    ~PhysicalLimit() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}

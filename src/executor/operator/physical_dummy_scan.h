//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDummyScan : public PhysicalOperator {
public:
    explicit PhysicalDummyScan(uint64_t id)
            : PhysicalOperator(PhysicalOperatorType::kDummyScan, nullptr, nullptr,id)
    {}

    ~PhysicalDummyScan() override = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}

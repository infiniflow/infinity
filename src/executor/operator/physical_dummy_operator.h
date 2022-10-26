//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDummyOperator : public PhysicalOperator {
public:
    explicit PhysicalDummyOperator(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kInvalid, nullptr, nullptr, id) {}
    ~PhysicalDummyOperator() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}

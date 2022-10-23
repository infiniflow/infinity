//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalFilter : public PhysicalOperator {
public:
    explicit PhysicalFilter(uint64_t id): PhysicalOperator(PhysicalOperatorType::kFilter, nullptr, nullptr, id) {}
    ~PhysicalFilter() = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};


}

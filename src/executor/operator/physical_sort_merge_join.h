//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalSortMergeJoin : public PhysicalOperator {
public:
    explicit PhysicalSortMergeJoin(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kJoinSortMerge, nullptr, nullptr, id) {}
    ~PhysicalSortMergeJoin() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}

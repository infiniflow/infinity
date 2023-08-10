//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalHashJoin : public PhysicalOperator {
public:
    explicit PhysicalHashJoin(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kJoinHash, nullptr, nullptr, id) {}
    ~PhysicalHashJoin() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final;
};

}

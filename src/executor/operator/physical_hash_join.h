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
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final;
};

}
